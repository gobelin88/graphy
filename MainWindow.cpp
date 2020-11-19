#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Gui--------------------------------------------------------------------------
    ui->setupUi(this);
    setCurrentFilename("empty");
    mdiArea=new QMdiArea();
    table=new MyTableView(4,4,24,mdiArea);
    //  tableview.setSelectionBehavior(tableview.SelectRows)
    //  tableview.setSelectionMode(tableview.SingleSelection)
    //  table->setDragDropMode(QTableView::InternalMove);
    //  table->setDragDropOverwriteMode(false);
    //  table->setSortingEnabled(true);
    this->setCentralWidget(mdiArea);

    te_results=new QTextEdit;
    te_widget=new QTabWidget();
    te_widget->addTab(table->getContainer(),"Data");
    te_widget->addTab(te_results,"Results");
    QMdiSubWindow* subWindow = mdiArea->addSubWindow(te_widget, Qt::FramelessWindowHint );
    subWindow->showMaximized();
    mdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation);
    mdiArea->setSizeAdjustPolicy (QAbstractScrollArea::AdjustToContents);

    //Action Edition----------------------------------------------------------------
    a_updateColumns=new QAction(this);
    a_newColumn=new QAction(this);
    a_newRow=new QAction(this);
    a_newRows=new QAction(this);
    a_delete=new QAction(this);
    a_removeColumnsRows=new QAction(this);
    a_copy=new QAction(this);
    a_paste=new QAction(this);
    a_copy->setShortcut(QKeySequence("Ctrl+C"));
    a_paste->setShortcut(QKeySequence("Ctrl+V"));

    table->addAction(ui->actionSave);
    table->addAction(a_copy);
    table->addAction(a_paste);
    table->addAction(a_newColumn);
    table->addAction(a_newRow);
    table->addAction(a_newRows);
    table->addAction(a_delete);
    table->addAction(a_removeColumnsRows);
    table->addAction(a_updateColumns);

    //------------------------------------------------------------------------------
    isModified=false;

    //I/O Edition
    connect(ui->actionFilter, &QAction::triggered,table,&MyTableView::slot_filter);
    connect(a_newRow,&QAction::triggered,table->model(),&MyModel::slot_newRow);
    connect(a_newRows,&QAction::triggered,table->model(),&MyModel::slot_newRows);
    connect(a_updateColumns,&QAction::triggered,table->model(),&MyModel::slot_updateColumns);
    connect(a_delete,&QAction::triggered,table,&MyTableView::slot_deleteSelected);
    connect(a_removeColumnsRows,&QAction::triggered,table,&MyTableView::slot_removeSelectedRowsAndCols);
    connect(a_copy,&QAction::triggered,table,&MyTableView::slot_copy);
    connect(a_paste,&QAction::triggered,table,&MyTableView::slot_paste);

    connect(ui->actionNew, &QAction::triggered,this,&MainWindow::slot_new);//ok
    connect(ui->actionOpen, &QAction::triggered,this,&MainWindow::slot_open);//ok
    connect(ui->actionSave, &QAction::triggered,this,&MainWindow::slot_save);//ok
    connect(ui->actionSaveAs, &QAction::triggered,this,&MainWindow::slot_save_as);//ok
    connect(ui->actionExport, &QAction::triggered,this,&MainWindow::slot_export);//ok
    connect(ui->actionParameters, &QAction::triggered,this,&MainWindow::slot_parameters);
    connect(ui->actionSelection_Pattern, &QAction::triggered,this,&MainWindow::slot_select);
    connect(ui->actionColourize, &QAction::triggered,this,&MainWindow::slot_colourize);//todo

    //Graphs
    connect(ui->actionPlot_GraphY, &QAction::triggered,this,&MainWindow::slot_plot_y);
    connect(ui->actionPlot_GraphXY,&QAction::triggered,this,&MainWindow::slot_plot_graph_xy);
    connect(ui->actionPlot_CurveXY,&QAction::triggered,this,&MainWindow::slot_plot_curve_xy);
    connect(ui->actionPlot_MapXYZ, &QAction::triggered,this,&MainWindow::slot_plot_map_2D);
    connect(ui->actionHistogram, &QAction::triggered,this,&MainWindow::slot_plot_histogram);
    connect(ui->actionPlot_Field_2D,&QAction::triggered,this,&MainWindow::slot_plot_field_2D);
    connect(ui->actionPlot_Cloud_2D,&QAction::triggered,this,&MainWindow::slot_plot_cloud_2D);
    connect(ui->actionPlot_Cloud_3D, &QAction::triggered,this,&MainWindow::slot_plot_cloud_3D);
    connect(ui->actionFFT, &QAction::triggered,this,&MainWindow::slot_plot_fft);
    connect(ui->actionPlot_Gain_Phase, &QAction::triggered,this,&MainWindow::slot_plot_bode);

    connect(table->model(),&MyModel::sig_dataChanged,this,&MainWindow::fileModified);

    //------------------------------------------------------------------------------

    loadShortcuts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_new()
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("New");

    dialog->setMinimumWidth(400);
    QGridLayout* gbox = new QGridLayout();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QSpinBox* sb_sx=new QSpinBox(dialog);
    sb_sx->setValue(10);
    sb_sx->setPrefix("nbRows=");
    sb_sx->setRange(1,10000000);
    QSpinBox* sb_sy=new QSpinBox(dialog);
    sb_sy->setValue(10);
    sb_sy->setPrefix("nbCols=");
    sb_sy->setRange(1,100);

    gbox->addWidget(sb_sx,0,0);
    gbox->addWidget(sb_sy,0,1);
    gbox->addWidget(buttonBox,1,0,1,2);

    dialog->setLayout(gbox);
    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        table->model()->createEmpty(sb_sx->value(),sb_sy->value());

        setCurrentFilename("new");
        isModified=false;
    }
}


void MainWindow::slot_open()
{
    QString filename=QFileDialog::getOpenFileName(this,"Open data",current_filename,tr("Data file (*.csv *.graphy)"));

    if (!filename.isEmpty())
    {
        direct_open(filename);
    }
}

void MainWindow::direct_open(QString filename)
{
    table->model()->open(filename);
    table->resizeColumnsToContents();
    setCurrentFilename(filename);
    isModified=false;
}

void MainWindow::slot_save()
{
    if (!current_filename.isEmpty())
    {
        direct_save(current_filename);
    }
    else
    {
        slot_save_as();
    }
}

void MainWindow::slot_save_as()
{
    QString filename=QFileDialog::getSaveFileName(this,"Save data",current_filename,tr("Data file (*.csv *.graphy)"));

    if (!filename.isEmpty())
    {
        direct_save(filename);
    }
}

void MainWindow::slot_export()
{
    QFileInfo info(current_filename);
    QString filename=QFileDialog::getSaveFileName(this,"Export data",info.path()+"/"+info.baseName()+".tex","*.tex");

    if (!filename.isEmpty())
    {
        table->model()->exportLatex(filename);
    }
}

void MainWindow::direct_save(QString filename)
{
    if(table->model()->save(filename))
    {
        setCurrentFilename(filename);
        isModified=false;
    }
}

void MainWindow::slot_results(QString results)
{
    te_results->append("------------------------");
    te_results->append(results);
}

void MainWindow::error(QString title,QString msg)
{
    QMessageBox::information(this,QString("Error : ")+title,msg);
    std::cout<<"Error : "<<msg.toLocal8Bit().data()<<std::endl;
}

void MainWindow::setCurrentFilename(QString filename)
{

    current_filename=filename;
    this->setWindowTitle(QString("Graphy %1 : %2").arg(graphyVersion).arg(current_filename));

    std::cout<<this->windowTitle().toLocal8Bit().data()<<std::endl;
}

void MainWindow::fileModified()
{
    this->setWindowTitle(QString("Graphy %1 : %2*").arg(graphyVersion).arg(current_filename));
    isModified=true;
}

Viewer1D* MainWindow::createViewer1D()
{
    Viewer1D* viewer1d=new Viewer1D(shortcuts,this);
    QObject::connect(viewer1d,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),table,SLOT(slot_newColumn(QString,Eigen::VectorXd)));
    QObject::connect(viewer1d,SIGNAL(sig_displayResults(QString)),this,SLOT(slot_results(QString)));
    viewer1d->setMinimumSize(600,400);
    viewer1d->setAttribute(Qt::WA_DeleteOnClose);

    mdiArea->addSubWindow(viewer1d,Qt::WindowStaysOnTopHint);
    viewer1d->show();

    return viewer1d;
}

ViewerBode* MainWindow::createViewerBode()
{
    ViewerBode* viewerBode=new ViewerBode(shortcuts,this);
    QObject::connect(viewerBode,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),table,SLOT(slot_newColumn(QString,Eigen::VectorXd)));
    QObject::connect(viewerBode,SIGNAL(sig_displayResults(QString)),this,SLOT(slot_results(QString)));

    viewerBode->setMinimumSize(600,400);
    viewerBode->setAttribute(Qt::WA_DeleteOnClose);

    mdiArea->addSubWindow(viewerBode,Qt::WindowStaysOnTopHint);
    viewerBode->show();

    return viewerBode;
}

void MainWindow::slot_plot_y()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        Viewer1D* viewer1d=createViewer1D();

        for (int k=0; k<id_list.size(); k++)
        {
            Eigen::VectorXd data_y=table->getLogicalColDataDouble(id_list[k].column()); //getCol(id_list[k].column(),datatable);

            if (data_y.size()>0)
            {
                viewer1d->slot_add_data(Curve2D(data_y,
                                                table->getLogicalColName(id_list[k  ].column()),
                                                Curve2D::GRAPH));
            }
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select k columns (k>1)");
    }

}


void MainWindow::slot_plot_graph_xy()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>=2)
    {
        Viewer1D* viewer1d=createViewer1D();

        Eigen::VectorXd data_x=table->getLogicalColDataDouble(id_list[0  ].column());
        for (int k=1; k<id_list.size(); k+=1)
        {
            Eigen::VectorXd data_y=table->getLogicalColDataDouble(id_list[k].column());

            if (data_x.size()>0 && data_y.size()>0)
            {
                if (!table->model()->asColumnStrings(table->horizontalHeader()->visualIndex(id_list[k  ].column())))
                {
                    Curve2D curve(data_x,data_y,QString("%2=f(%1)").arg(table->getLogicalColName(id_list[0  ].column())).arg(table->getLogicalColName(id_list[k].column())),Curve2D::GRAPH);
                    viewer1d->slot_add_data(curve);
                }
                else
                {
                    Curve2D curve(data_y,QString("%2=f(%1)").arg(table->getLogicalColName(id_list[0  ].column())).arg(table->getLogicalColName(id_list[k].column())),Curve2D::GRAPH);
                    curve.setLabelsField(table->getLogicalColDataString(table->horizontalHeader()->visualIndex(id_list[k  ].column())));
                    viewer1d->slot_add_data(curve);
                }
            }
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 2 columns or more X and Yi in order to plot (X,Yi)");
    }
}

void MainWindow::slot_plot_curve_xy()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>=2)
    {
        Viewer1D* viewer1d=createViewer1D();

        Eigen::VectorXd data_x=table->getLogicalColDataDouble(id_list[0  ].column());
        for (int k=1; k<id_list.size(); k+=2)
        {
            Eigen::VectorXd data_y=table->getLogicalColDataDouble(id_list[k].column());

            if (data_x.size()>0 && data_y.size()>0)
            {
                viewer1d->slot_add_data(Curve2D(data_x,
                                                data_y,
                                                QString("(%1,%2)").arg(table->getLogicalColName(id_list[0  ].column())).arg(table->getLogicalColName(id_list[k].column())),
                                        Curve2D::CURVE));
            }
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 2 columns or more X and Yi in order to plot (X,Yi)");
    }
}

void MainWindow::slot_plot_cloud_2D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()==3 || id_list.size()==2)
    {
        Viewer1D* viewer1d=createViewer1D();

        Eigen::VectorXd data_x=table->getLogicalColDataDouble(id_list[0].column());
        Eigen::VectorXd data_y=table->getLogicalColDataDouble(id_list[1].column());
        Eigen::VectorXd data_s=Eigen::VectorXd::Zero(data_x.rows());

        if(id_list.size()==3)
        {
            data_s=table->getLogicalColDataDouble(id_list[2].column());
        }

        if (data_x.size()>0 && data_y.size()>0)
        {
            Curve2D curve(data_x,
                          data_y,
                          QString("(%1,%2)").arg(table->getLogicalColName(id_list[0  ].column())).arg(table->getLogicalColName(id_list[1].column())),
                    Curve2D::CURVE);

            curve.setScalarField(data_s);
            curve.getStyle().mLineStyle=QCPCurve::lsNone;
            curve.getStyle().mScatterShape=QCPScatterStyle::ssDisc;
            viewer1d->slot_add_data(curve);
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 2 or 3 columns P(X,Y) and Scalarfield S in order to plot S(P)");
    }
}

void MainWindow::slot_plot_field_2D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()==2 ||  (id_list.size()==4))
    {
        Viewer1D* viewer1d=createViewer1D();

        Eigen::VectorXd data_x ;
        Eigen::VectorXd data_y ;
        Eigen::VectorXd data_vx;
        Eigen::VectorXd data_vy;


        QString legendString;

        if (id_list.size()==2)
        {
            data_x =table->getLogicalColDataComplex(id_list[0].column()).real();
            data_y =table->getLogicalColDataComplex(id_list[0].column()).imag();
            data_vx=table->getLogicalColDataComplex(id_list[1].column()).real();
            data_vy=table->getLogicalColDataComplex(id_list[1].column()).imag();

            std::cout<<data_x.transpose()<<std::endl;
            std::cout<<data_y.transpose()<<std::endl;

            legendString=QString("V(re(%3),im(%4))=f(re(%1),im(%2))").
                    arg(table->getLogicalColName(id_list[0].column())).
                    arg(table->getLogicalColName(id_list[0].column())).
                    arg(table->getLogicalColName(id_list[1].column())).
                    arg(table->getLogicalColName(id_list[1].column()));

        }
        else if (id_list.size()==4)
        {
            data_x =table->getLogicalColDataDouble(id_list[0].column());
            data_y =table->getLogicalColDataDouble(id_list[1].column());
            data_vx=table->getLogicalColDataDouble(id_list[2].column());
            data_vy=table->getLogicalColDataDouble(id_list[3].column());
            legendString=QString("V(%3,%4)=f(%1,%2)").
                    arg(table->getLogicalColName(id_list[0].column())).
                    arg(table->getLogicalColName(id_list[1].column())).
                    arg(table->getLogicalColName(id_list[2].column())).
                    arg(table->getLogicalColName(id_list[3].column()));

        }

        Eigen::VectorXd data_a(data_vx.size());
        Eigen::VectorXd data_s(data_vx.size());

        for (int i=0; i<data_a.size(); i++)
        {
            data_a[i]=-atan2(data_vy[i],data_vx[i]);
            data_s[i]=sqrt(data_vy[i]*data_vy[i]+data_vx[i]*data_vx[i]);
        }

        if (data_x.size()>0 && data_y.size()>0)
        {
            Curve2D curve(data_x,
                          data_y,
                          legendString,
                          Curve2D::CURVE);
            curve.setScalarField(data_s);
            curve.setAlphaField(data_a);
            curve.getStyle().mLineStyle=QCPCurve::lsNone;
            curve.getStyle().mScatterShape=QCPScatterStyle::ssArrow;
            curve.getStyle().mScatterSize=20;
            viewer1d->slot_add_data(curve);
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 4 columns P(X,Y) and V(X,Y) in order to plot vector 2d field V(P)");
    }
}

void MainWindow::slot_plot_map_2D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Interpolation map");
    dialog->setMinimumWidth(400);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QComboBox * cb_type=new QComboBox(dialog);

    if (id_list.size()==2)
    {
        cb_type->addItem("Complex Phase  Scalar field",1);
        cb_type->addItem("Complex Module Scalar field",3);
    }
    else if (id_list.size()==3)
    {
        cb_type->addItem("Standard Scalar field",2);
    }

    if(cb_type->count()==0)
    {
        QMessageBox::information(this,"Information",
                                 "Please select\n"\
                                 "-3 columns of real numbers X,Y,Z in order to plot Z=f(X,Y)\n"\
                                 "-2 columns of complex numbers X,Y in order to plot mod(Y)=f(re(X),im(Y)) or arg(Y)=f(re(X),im(Y))\n");
        return;
    }

    QSpinBox * sb_resX=new QSpinBox(dialog);
    sb_resX->setRange(4,4096*2);
    sb_resX->setValue(512);

    QSpinBox * sb_resY=new QSpinBox(dialog);
    sb_resY->setRange(4,4096*2);
    sb_resY->setValue(512);

    QGridLayout* gbox = new QGridLayout();
    gbox->addWidget(cb_type,0,0,1,3);
    gbox->addWidget(new QLabel("Resolution :"),1,0);
    gbox->addWidget(sb_resX,1,1);
    gbox->addWidget(sb_resY,1,2);
    gbox->addWidget(buttonBox,2,0,1,3);

    dialog->setLayout(gbox);

    int result=dialog->exec();
    if(result==QDialog::Accepted)
    {
        Eigen::Vector2d resolution(sb_resX->value(),sb_resY->value());
        Eigen::VectorXd data_x,data_y,data_z;
        QCPColorGradient::GradientPreset gradientType;
        bool ok=false;

        if (cb_type->currentData().toInt()==1 || cb_type->currentData().toInt()==3)//Phase plot
        {
            data_x=table->getLogicalColDataComplex(id_list[0].column()).real();
            data_y=table->getLogicalColDataComplex(id_list[0].column()).imag();
            Eigen::VectorXcd data_cplx_z=table->getLogicalColDataComplex(id_list[1].column());
            data_z=Eigen::VectorXd(data_cplx_z.rows());

            if(cb_type->currentData().toInt()==1)
            {
                for(int i=0;i<data_cplx_z.rows();i++)
                {
                    data_z[i]=std::arg(data_cplx_z[i]);
                }
                gradientType= QCPColorGradient::gpHues;
            }
            else if(cb_type->currentData().toInt()==3)
            {
                for(int i=0;i<data_cplx_z.rows();i++)
                {
                    data_z[i]=std::abs(data_cplx_z[i]);
                }
                gradientType= QCPColorGradient::gpPolar;
            }

            ok=true;
        }
        else if (cb_type->currentData().toInt()==2)
        {
            data_x=table->getLogicalColDataDouble(id_list[0].column());
            data_y=table->getLogicalColDataDouble(id_list[1].column());
            data_z=table->getLogicalColDataDouble(id_list[2].column());
            gradientType= QCPColorGradient::gpPolar;
            ok=true;
        }

        if(ok)
        {
            Viewer2D* viewer2d=new Viewer2D();
            viewer2d->slot_setGradient(gradientType);
            viewer2d->setMinimumSize(512+32,512);
            QMdiSubWindow* subWindow = new QMdiSubWindow;
            subWindow->setWidget(viewer2d);
            subWindow->setAttribute(Qt::WA_DeleteOnClose);
            mdiArea->addSubWindow(subWindow,Qt::WindowStaysOnTopHint);
            viewer2d->show();
            viewer2d->slot_setData(data_x,data_y,data_z,resolution);
        }
    }
}

void MainWindow::slot_plot_fft()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()==1 )//1 real or complex signal
    {
        FFTDialog* dialog=new FFTDialog;

        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            ViewerBode* viewerBode=createViewerBode();

            QString nameReal=table->getLogicalColName(id_list[0  ].column());
            //QString nameImag =(id_list.size()==2)?table->getLogicalColName(id_list[1  ].column()):QString("");
            //QString name=nameReal+QString("_")+nameImag;

            Eigen::VectorXcd dataIn=table->getLogicalColDataComplex(id_list[0].column());

            std::cout<<"compute fft"<<std::endl;
            Eigen::VectorXcd dataOut=MyFFT::getFFT(dataIn,
                                                     dialog->getWindowsType(),
                                                     dialog->isNormalized(),
                                                     //cb_halfspectrum->isChecked(),
                                                     false,
                                                     dialog->isInverse());
            std::cout<<"compute fft end"<<std::endl;

            Eigen::VectorXd data_x=Eigen::VectorXd::LinSpaced(dataOut.rows(),0,dialog->getFe());

            Curve2DComplex curveCplx(data_x,dataOut,QString("FFT-%1").arg(nameReal));

            if(dialog->isInverse())
            {
                table->slot_newColumn(QString("invFFT"),dataOut);
            }
            else
            {
                table->slot_newColumn(QString("FFT"),dataOut);
            }

            viewerBode->slot_add_data(curveCplx);
        }

    }
    else
    {
        QMessageBox::information(this,"Information","Please select 1 or 2 columns");
    }
}

void MainWindow::slot_plot_cloud_3D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()==0 || id_list.size()%3==0 || id_list.size()%4==0)
    {
        Viewer3D* view3d=new Viewer3D(shortcuts);

        QObject::connect(view3d,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),table,SLOT(slot_newColumn(QString,Eigen::VectorXd)));
        QObject::connect(view3d,SIGNAL(sig_displayResults(QString)),this,SLOT(slot_results(QString)));

        Cloud* cloud=nullptr;

        if (id_list.size()==0)
        {
            //            Eigen::VectorXd data_x(1);
            //            Eigen::VectorXd data_y(1);
            //            Eigen::VectorXd data_z(1);

            //            cloud=new Cloud(data_x,data_y,data_z,"X","Y","Z");
            //            view3d->addCloudScalar(cloud,Qt3DRender::QGeometryRenderer::Points);

        }
        else if (id_list.size()%3==0)
        {
            for(int i=0;i<id_list.size();i+=3)
            {
                Eigen::VectorXd data_x=table->getLogicalColDataDouble(id_list[i].column());
                Eigen::VectorXd data_y=table->getLogicalColDataDouble(id_list[i+1].column());
                Eigen::VectorXd data_z=table->getLogicalColDataDouble(id_list[i+2].column());

                cloud=new Cloud(data_x,data_y,data_z,
                                table->getLogicalColName(id_list[i].column()),
                                table->getLogicalColName(id_list[i+1].column()),
                        table->getLogicalColName(id_list[i+2].column()));

                view3d->addCloudScalar(cloud,Qt3DRender::QGeometryRenderer::Points);
            }

        }
        else if (id_list.size()%4==0)
        {
            for(int i=0;i<id_list.size();i+=4)
            {
                Eigen::VectorXd data_x=table->getLogicalColDataDouble(id_list[i].column());
                Eigen::VectorXd data_y=table->getLogicalColDataDouble(id_list[i+1].column());
                Eigen::VectorXd data_z=table->getLogicalColDataDouble(id_list[i+2].column());
                Eigen::VectorXd data_s=table->getLogicalColDataDouble(id_list[i+3].column());
                cloud=new Cloud(data_x,data_y,data_z,data_s,
                                table->getLogicalColName(id_list[i].column()),
                                table->getLogicalColName(id_list[i+1].column()),
                        table->getLogicalColName(id_list[i+2].column()),
                        table->getLogicalColName(id_list[i+3].column()));

                view3d->addCloudScalar(cloud,Qt3DRender::QGeometryRenderer::Points);
            }
        }

        mdiArea->addSubWindow(view3d->getContainer(),Qt::WindowStaysOnTopHint);
        view3d->getContainer()->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3 columns (x,y,z) or 4 columns (x,y,z,scalar)");
    }
}

void MainWindow::slot_plot_bode()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()%3==0 && id_list.size()>0)
    {
        ViewerBode * viewer_bode=createViewerBode();


        for (int k=0; k<id_list.size(); k+=3)
        {
            Eigen::VectorXd data_f=table->getLogicalColDataDouble(id_list[k].column());
            Eigen::VectorXd data_module=table->getLogicalColDataDouble(id_list[k+1].column());
            Eigen::VectorXd data_phase=table->getLogicalColDataDouble(id_list[k+2].column());

            if (data_f.size()>0 && data_module.size()>0 && data_phase.size()>0)
            {
                QString name=QString("%2=f(%1)").arg(table->getLogicalColName(id_list[k  ].column())).arg(table->getLogicalColName(id_list[k+1].column()));

                viewer_bode->slot_add_data(Curve2DComplex(data_f,data_module,data_phase,Curve2DComplex::POLAR,name));
            }
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3 columns (frequency,module,phase)");
    }
}

void MainWindow::slot_plot_histogram()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        Viewer1D* viewer1d=createViewer1D();

        for (int k=0; k<id_list.size(); k++)
        {
            Eigen::VectorXd data_y=table->getLogicalColDataDouble(id_list[k].column());

            if (data_y.size()>0)
            {
                bool ok;
                int nbbins=QInputDialog::getInt(this,"Number of bins","Nb bins=",100,2,10000,1,&ok);
                if (ok)
                {
                    viewer1d->slot_histogram(data_y,QString("Histogram %1").arg(table->getLogicalColName(id_list[k  ].column())),nbbins);
                }
            }
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select k columns (k>1)");
    }

}

void MainWindow::slot_parameters()
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Parameters");
    dialog->setMinimumWidth(400);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    //Table shortcuts
    QTableView* tableShortcuts=new QTableView();
    QMapIterator<QString, QKeySequence> shortcuts_it(shortcuts);
    QStandardItemModel* modelParametersShortcuts = new QStandardItemModel;
    int line=0;
    while (shortcuts_it.hasNext())
    {
        shortcuts_it.next();
        QStandardItem* itemA=new QStandardItem(shortcuts_it.key());
        QStandardItem* itemB=new QStandardItem(shortcuts_it.value().toString());
        itemA->setEditable(false);
        itemB->setEditable(true);
        modelParametersShortcuts->setItem(line,0,itemA);
        modelParametersShortcuts->setItem(line,1,itemB);
        line++;
    }
    tableShortcuts->setModel(modelParametersShortcuts);

    //Table variables
    QTableView* tableVariables=new QTableView();
    QStandardItemModel* modelParametersVariables = new QStandardItemModel;

    const Register & reg=table->model()->getRegister();
    for (int i=0; i<reg.variablesNames().size(); i++)
    {
        QStandardItem* itemA=new QStandardItem(reg.variablesNames()[i]);
        QStandardItem* itemB=new QStandardItem(reg.variablesExpressions()[i]);
        itemA->setEditable(false);
        itemB->setEditable(false);
        modelParametersVariables->setItem(i,0,itemA);
        modelParametersVariables->setItem(i,1,itemB);
    }
    tableVariables->setModel(modelParametersVariables);

    QTabWidget* tab=new QTabWidget(dialog);
    tab->addTab(tableShortcuts,"Shortcuts");
    tab->addTab(tableVariables,"Variables");

    QGridLayout* gbox = new QGridLayout();
    gbox->addWidget(tab,0,0);
    gbox->addWidget(buttonBox,1,0);
    dialog->setLayout(gbox);

    int result=dialog->exec();
    if (result == QDialog::Accepted)
    {
        //Shortcuts apply
        shortcuts.clear();
        for (int i=0; i<modelParametersShortcuts->rowCount(); i++)
        {
            shortcuts.insert(modelParametersShortcuts->item(i,0)->text(),QKeySequence(modelParametersShortcuts->item(i,1)->text()));
        }

        applyShortcuts(shortcuts);
        saveShortcuts(shortcuts);
    }

}

void MainWindow::slot_select()
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle(QString("Selection pattern"));
    QGridLayout* gbox = new QGridLayout();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QLineEdit * le_pattern=new QLineEdit();
    le_pattern->setText(table->getSelectionPattern());
    le_pattern->setToolTip("For example to select row 1 and column 1 type : R1,C1");

    gbox->addWidget(le_pattern,0,0);
    gbox->addWidget(buttonBox,1,0,1,2);

    dialog->setLayout(gbox);
    dialog->setMinimumWidth(300);
    dialog->adjustSize();

    int result=dialog->exec();
    if (result == QDialog::Accepted)
    {
        table->setSelectionPattern(le_pattern->text());
    }
}

bool MainWindow::loadShortcuts()
{
    QString shorcuts_cfg=QCoreApplication::applicationDirPath()+QString("/shortcuts.csv");
    QFile file(shorcuts_cfg);

    if (file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        while (!file.atEnd())
        {
            QString lineraw=file.readLine();

            if(lineraw.contains("<header>"))
            {
                while( !lineraw.contains("</header>") && !file.atEnd() )
                {
                    lineraw=file.readLine();
                }
                lineraw=file.readLine();
            }

            if (lineraw.endsWith("\n"))
            {
                lineraw.chop(1);
            }
            QStringList line=QString(lineraw).split(";");

            if (line.size()>=2)
            {
                shortcuts.insert(line[0],QKeySequence(line[1]));
            }
            else
            {
                error("Shortcuts",QString("Bad line in shortcut configuration file : ")+lineraw);
            }
        }
        file.close();
    }
    else
    {
        error("Shortcuts",QString("Can't find shorcuts configuration file : ")+shorcuts_cfg);
    }

    applyShortcuts(shortcuts);
    return true;
}

void MainWindow::applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map)
{
    QMap<QString,QAction*> shortcuts_links;
    shortcuts_links.insert(QString("New"),ui->actionNew);
    shortcuts_links.insert(QString("Save"),ui->actionSave);
    shortcuts_links.insert(QString("Open"),ui->actionOpen);
    shortcuts_links.insert(QString("Export"),ui->actionExport);
    shortcuts_links.insert(QString("Parameters"),ui->actionParameters);
    shortcuts_links.insert(QString("Update"),a_updateColumns);
    shortcuts_links.insert(QString("Edit/Add-variable"),a_newColumn);
    shortcuts_links.insert(QString("New-row"),a_newRow);
    shortcuts_links.insert(QString("New-rows"),a_newRows);
    shortcuts_links.insert(QString("Delete"),a_delete);
    shortcuts_links.insert(QString("Delete-cols-rows"),a_removeColumnsRows);
    shortcuts_links.insert(QString("Filter"),ui->actionFilter);


    QMapIterator<QString, QKeySequence> i(shortcuts_map);
    while (i.hasNext())
    {
        i.next();

        if (shortcuts_links.contains(i.key()))
        {
            shortcuts_links[i.key()]->setShortcut(i.value());
        }
        else
        {
            //std::cout<<"not in main shortcut list : "<<i.key().toLocal8Bit().data()<<std::endl;
        }
    }
}

void MainWindow::saveShortcuts(const QMap<QString,QKeySequence>& shortcuts_map)
{
    QString shorcuts_cfg=QCoreApplication::applicationDirPath()+QString("/shortcuts.csv");
    QFile file(shorcuts_cfg);

    if (file.open(QIODevice::Text | QIODevice::WriteOnly))
    {
        QTextStream ts(&file);
        QMapIterator<QString, QKeySequence> i(shortcuts_map);
        while (i.hasNext())
        {
            i.next();
            ts<<i.key()<<";"<<i.value().toString()<<"\n";
        }
        file.close();
    }
    else
    {
        //QMessageBox::critical(this,"Error",QString("Can't find shorcuts configuration file : ")+shorcuts_cfg);
        error("Configuration",QString("Can't find shorcuts configuration file : ")+shorcuts_cfg);
    }

    applyShortcuts(shortcuts);
}

void MainWindow::slot_colourize()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        //Dialog
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Colourize");

        dialog->setMinimumWidth(400);
        QGridLayout* gbox = new QGridLayout();

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
        QPushButton * pb_clear=new QPushButton("Clear");
        pb_clear->setCheckable(true);
        buttonBox->addButton(pb_clear,QDialogButtonBox::ButtonRole::AcceptRole);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        MyGradientComboBox * cb_gradients=new MyGradientComboBox(this);

        QCheckBox * cb_percolumns=new QCheckBox("Range per columns");
        cb_percolumns->setChecked(false);

        gbox->addWidget(cb_gradients,0,0);
        gbox->addWidget(cb_percolumns,1,0);
        gbox->addWidget(buttonBox,2,0);

        dialog->setLayout(gbox);
        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            //Range
            QCPRange range;
            if(!cb_percolumns->isChecked())
            {
                for(int i=0;i<id_list.size();i++)
                {
                    int logicalIndex=id_list[i].column();
                    int visualIndex=table->horizontalHeader()->visualIndex(logicalIndex);
                    Eigen::VectorXd colData=table->getLogicalColDataDouble(visualIndex);

                    if(i==0)
                    {
                        range=QCPRange (colData.minCoeff(),colData.maxCoeff());
                    }
                    else
                    {
                        range=QCPRange (std::min(range.lower,colData.minCoeff()),
                                        std::max(range.upper,colData.maxCoeff()));
                    }
                }
            }
            std::cout<<range.lower<<" "<<range.upper<<std::endl;

            //Process
            for(int i=0;i<id_list.size();i++)
            {
                int logicalIndex=id_list[i].column();
                int visualIndex=table->horizontalHeader()->visualIndex(logicalIndex);
                Eigen::VectorXd colData=table->getLogicalColDataDouble(visualIndex);

                if(cb_percolumns->isChecked())
                {
                    range=QCPRange (colData.minCoeff(),colData.maxCoeff());
                }

                if(!pb_clear->isChecked())
                {
                    std::vector<QRgb> colors=cb_gradients->colourize(colData,range);
                    table->model()->colourizeCol(visualIndex,colors);
                }
                else
                {
                    table->model()->colourizeCol(visualIndex,qRgb(255,255,255));
                }
            }
        }
    }
}



void MainWindow::closeEvent (QCloseEvent *event)
{
    if(isModified)
    {
        QMessageBox::StandardButton resBtn =
                QMessageBox::question( this, "File is not saved" ,tr("Are you sure?\n"),
                                       QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                       QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
        {
            event->ignore();
        }
        else
        {
            event->accept();
        }
    }
}
