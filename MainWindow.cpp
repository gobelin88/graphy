#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QTextStream>
#include <QMessageBox>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Gui--------------------------------------------------------------------------
    ui->setupUi(this);
    this->setWindowTitle(QString("Graphy %1").arg(graphyVersion));

    pb_bar=new QProgressBar(this);
    pb_bar->setRange(0,100);
    pb_bar->setFixedHeight(15);
    l_what=new QLabel;
    l_what->setFixedWidth(200);

    this->statusBar()->addWidget(l_what);
    this->statusBar()->addWidget(pb_bar,1);


    mdiArea=new QMdiArea();

    this->setCentralWidget(mdiArea);

    te_results=new QTextEdit;

    subWindowsResults=mdiArea->addSubWindow(te_results,Qt::WindowStaysOnTopHint );
    subWindowsResults->setAttribute( Qt::WA_DeleteOnClose, false );
    subWindowsResults->hide();

    te_widget=new QTabWidget();
    te_widget->setTabsClosable(true);
    te_widget->setTabShape(QTabWidget::Triangular);
    te_widget->setMovable(true);


    connect(te_widget,&QTabWidget::tabCloseRequested,this,&MainWindow::closeTable);

    QMdiSubWindow* subWindow = mdiArea->addSubWindow(te_widget, Qt::FramelessWindowHint );
    subWindow->showMaximized();
    mdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation);
    mdiArea->setSizeAdjustPolicy (QAbstractScrollArea::AdjustToContents);

    //------------------------------------------------------------------------------
    //I/O Edition
    connect(ui->actionFilter, &QAction::triggered,this,&MainWindow::slot_filter);
    connect(ui->actionNew, &QAction::triggered,this,&MainWindow::slot_new);//ok
    connect(ui->actionOpen, &QAction::triggered,this,&MainWindow::slot_open);//ok
    connect(ui->actionSave, &QAction::triggered,this,&MainWindow::slot_save);//ok
    connect(ui->actionSaveAs, &QAction::triggered,this,&MainWindow::slot_save_as);//ok
    connect(ui->actionExport, &QAction::triggered,this,&MainWindow::slot_export);//ok
    connect(ui->actionParameters, &QAction::triggered,this,&MainWindow::slot_parameters);
    connect(ui->actionSelection_Pattern, &QAction::triggered,this,&MainWindow::slot_select);
    connect(ui->actionColourize, &QAction::triggered,this,&MainWindow::slot_colourize);//todo
    connect(ui->actionCloseTab,&QAction::triggered,this,&MainWindow::closeCurrentTable);

    this->addAction(ui->actionCloseTab);

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

    connect(te_widget->tabBar(), &QTabBar::tabMoved ,this,&MainWindow::slot_tab_moved);

    connect(ui->actionTerminal,&QAction::triggered,this,&MainWindow::slot_showHideTerminal);
    //------------------------------------------------------------------------------

    loadShortcuts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receivedMessage(int instanceId, QByteArray message)
{
    Q_UNUSED(instanceId);
//    message.replace('=','\\');
//    message.replace('+',':');

    //QMessageBox::information(nullptr,"Recv",QString(message));

    QStringList filenames=QString::fromLocal8Bit(message).split('#',QString::SkipEmptyParts);

    direct_open(filenames);
}

void MainWindow::slot_filter()
{
    MyTableView * table=getCurrentTable();
    if(table)
    {
        table->slot_filter();
    }
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
    sb_sy->setRange(1,1000);

    gbox->addWidget(sb_sx,0,0);
    gbox->addWidget(sb_sy,0,1);
    gbox->addWidget(buttonBox,1,0,1,2);

    dialog->setLayout(gbox);
    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        MyTableView * newtable=new MyTableView(1,this,sb_sx->value(),sb_sy->value());
        connectTable(newtable);
        addNewTable(newtable);
    }
}

void MainWindow::slot_currentTableModified()
{
    int id=te_widget->currentIndex();
    if(id<tables.size())
    {
        te_widget->setTabText(id,tables[id]->model()->getTabTitle());
    }
}

void MainWindow::addNewTable(MyTableView * newTable)
{
    //QRect rec = QApplication::desktop()->screenGeometry();

    newTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    newTable->verticalHeader()->setDefaultSectionSize(24);
    newTable->model()->setRowSpan(100);
    newTable->addAction(ui->actionSave);
    tables.push_back(newTable);
    newTable->applyShortcuts(shortcuts);
    newTable->getContainer()->setToolTip(newTable->model()->getCurrentFilename());
    te_widget->addTab(newTable->getContainer(),newTable->model()->getTabTitle());
    te_widget->setCurrentWidget(newTable->getContainer());
    resizeEvent(nullptr);

    connect(newTable->model(),&MyModel::sig_dataChanged,this,&MainWindow::slot_currentTableModified);
}

MyTableView * MainWindow::getCurrentTable()
{
    int id=te_widget->currentIndex();
    if(id<tables.size() && id>=0)
    {
        return tables[id];
    }
    else
    {
        return nullptr;
    }
}

QString MainWindow::getCurrentFilename()
{
    MyTableView * table=getCurrentTable();
    if(table)
    {
        return table->model()->getCurrentFilename();
    }
    else
    {
        return QString("");
    }
}

void MainWindow::slot_open()
{
    QStringList filenames=QFileDialog::getOpenFileNames(this,"Open data",getCurrentFilename(),tr("Data file (*.csv *.graphy)"));

    if (!filenames.isEmpty())
    {
        direct_open(filenames);
    }
}

int MainWindow::fileAlreadyOpened(QString filename)
{
    for(int i=0;i<tables.size();i++)
    {
        if(tables[i]->model()->getCurrentFilename()==filename)
        {
            return i;
        }
    }
    return -1;
}

void MainWindow::connectTable(MyTableView * newtable)
{
    connect(newtable->model()->getProgressHandler(),&MyProgressHandler::sig_progress,this,&MainWindow::slot_progress,Qt::BlockingQueuedConnection);
    connect(newtable->model()->getProgressHandler(),&MyProgressHandler::sig_what,this,&MainWindow::slot_what,Qt::QueuedConnection);
    connect(newtable->model()->getProgressHandler(),&MyProgressHandler::sig_error,this,&MainWindow::slot_error,Qt::QueuedConnection);
    connect(newtable,&MyTableView::sig_opened,this,&MainWindow::slot_open_end);
    connect(newtable,&MyTableView::sig_saved,this,&MainWindow::slot_save_end);
}

void MainWindow::direct_open(QStringList filenames)
{
    for(int i=0;i<filenames.size();i++)
    {
        int index=fileAlreadyOpened(filenames[i]);
        if(index<0)
        {
            MyTableView * newtable=new MyTableView(25,this);            
            connectTable(newtable);

            QtConcurrent::run(newtable, &MyTableView::open,filenames[i]);
        }
        else
        {
            te_widget->setCurrentIndex(index);
        }
    }
}

void MainWindow::slot_open_end(MyTableView * newtable)
{
    if(newtable->isOpen())
    {
        newtable->resizeColumnsToContents();
        addNewTable(newtable);
    }
    else
    {
        delete newtable;
    }
}

void MainWindow::slot_save()
{
    std::cout<<"Save"<<std::endl;
    if (QFile(getCurrentFilename()).exists())
    {
        direct_save(getCurrentFilename());
    }
    else
    {
        slot_save_as();
    }
}

void MainWindow::slot_save_as()
{
    QString filename=QFileDialog::getSaveFileName(this,"Save data",getCurrentFilename(),tr("Data file (*.csv *.graphy)"));

    if (!filename.isEmpty())
    {
        direct_save(filename);
    }
}

void MainWindow::slot_export()
{
    QFileInfo info(getCurrentFilename());
    QString filename=QFileDialog::getSaveFileName(this,"Export data",info.path()+"/"+info.baseName()+".tex","*.tex");

    if (!filename.isEmpty())
    {
        getCurrentTable()->model()->exportLatex(filename);
    }
}

void MainWindow::direct_save(QString filename)
{
    MyTableView * table=getCurrentTable();
    if(table)
    {
        if(!table->model()->hasHeader())
        {
            QMessageBox::StandardButton ret=QMessageBox::question(nullptr,"Save header ?","Do you wish to save the header ?");
            if(ret==QMessageBox::StandardButton::Yes)
            {
                table->model()->setHasHeader(true);
            }
        }

        QtConcurrent::run(table, &MyTableView::save,filename);
    }
}

void MainWindow::slot_save_end(MyTableView * table)
{
    if(table->isSave())
    {
        te_widget->setTabText(te_widget->currentIndex(),table->model()->getTabTitle());
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

Viewer1D* MainWindow::createViewer1D(int sx,int sy)
{
    Viewer1D* viewer1d=new Viewer1D(shortcuts,this);
    QObject::connect(viewer1d,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),getCurrentTable(),SLOT(slot_newColumn(QString,Eigen::VectorXd)));
    QObject::connect(viewer1d,SIGNAL(sig_displayResults(QString)),this,SLOT(slot_results(QString)));
    viewer1d->setMinimumSize(sx,sy);
    viewer1d->setAttribute(Qt::WA_DeleteOnClose);

    mdiArea->addSubWindow(viewer1d,Qt::WindowStaysOnTopHint);
    viewer1d->show();

    return viewer1d;
}

ViewerBode* MainWindow::createViewerBode()
{
    ViewerBode* viewerBode=new ViewerBode(shortcuts,this);
    QObject::connect(viewerBode,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),getCurrentTable(),SLOT(slot_newColumn(QString,Eigen::VectorXd)));
    QObject::connect(viewerBode,SIGNAL(sig_displayResults(QString)),this,SLOT(slot_results(QString)));

    viewerBode->setMinimumSize(600,400);
    viewerBode->setAttribute(Qt::WA_DeleteOnClose);

    mdiArea->addSubWindow(viewerBode,Qt::WindowStaysOnTopHint);
    viewerBode->show();

    return viewerBode;
}

void MainWindow::slot_plot_y()
{
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)//y1 y2 y3 y4 ...
    {
        Viewer1D* viewer1d=createViewer1D();

        for (int k=0; k<id_list.size(); k++)
        {
            Eigen::VectorXd data_y=table->getLogicalColDataComplex(id_list[k].column()).real(); //getCol(id_list[k].column(),datatable);

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

void MainWindow::plot_xy(Curve2D::CurveType type)
{
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>=2)//x y1 y2 y3 y4...
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
                    Curve2D curve(data_x,data_y,QString("%2=f(%1)").arg(table->getLogicalColName(id_list[0  ].column())).arg(table->getLogicalColName(id_list[k].column())),type);
                    viewer1d->slot_add_data(curve);
                }
                else
                {
                    Curve2D curve(data_y,QString("%2=f(%1)").arg(table->getLogicalColName(id_list[0  ].column())).arg(table->getLogicalColName(id_list[k].column())),type);
                    curve.setLabelsField(table->getLogicalColDataString(table->horizontalHeader()->visualIndex(id_list[k  ].column())));
                    viewer1d->slot_add_data(curve);
                }
            }
        }
    }
    else if (id_list.size()==1)//Single complex entry possible
    {
        Viewer1D* viewer1d=createViewer1D();

        Eigen::VectorXcd data_cplx=table->getLogicalColDataComplex(id_list[0  ].column());
        Eigen::VectorXd data_x=data_cplx.real();
        Eigen::VectorXd data_y=data_cplx.imag();

        if (data_x.size()>0 && data_y.size()>0)
        {
            Curve2D curve(data_x,data_y,QString("Im(%1)=f(Re(%1))").arg(table->getLogicalColName(id_list[0  ].column())),type);
            viewer1d->slot_add_data(curve);
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 2 columns X and Y in order to plot (X,Y) or 1 column X in order to plot (Re(X),Im(X))");
    }
}

void MainWindow::slot_plot_graph_xy()
{
    plot_xy(Curve2D::GRAPH);
}

void MainWindow::slot_plot_curve_xy()
{
    plot_xy(Curve2D::CURVE);
}

void MainWindow::slot_plot_cloud_2D()
{
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()==3 || id_list.size()==2 || id_list.size()==1)
    {
        Viewer1D* viewer1d=createViewer1D();

        Eigen::VectorXd data_x,data_y;
        QString legend;

        if(id_list.size()==1)
        {
            Eigen::VectorXcd data_cplx=table->getLogicalColDataComplex(id_list[0].column());
            data_x=data_cplx.real();
            data_y=data_cplx.imag();
            legend=QString("(Re(%1),Im(%1))")
                    .arg(table->getLogicalColName(id_list[0].column()));
        }
        else
        {
            data_x=table->getLogicalColDataDouble(id_list[0].column());
            data_y=table->getLogicalColDataDouble(id_list[1].column());
            legend=QString("(%1,%2)")
                    .arg(table->getLogicalColName(id_list[0].column()))
                    .arg(table->getLogicalColName(id_list[1].column()));
        }
        Eigen::VectorXd data_s=Eigen::VectorXd::Zero(data_x.rows());

        if(id_list.size()==3)
        {
            data_s=table->getLogicalColDataDouble(id_list[2].column());
        }

        if (data_x.size()>0 && data_y.size()>0)
        {
            Curve2D curve(data_x,
                          data_y,
                          legend,
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
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()==2 ||  (id_list.size()==4))
    {
        Viewer1D* viewer1d=createViewer1D(512+54,512);

        Eigen::VectorXd data_x ;
        Eigen::VectorXd data_y ;
        Eigen::VectorXd data_vx;
        Eigen::VectorXd data_vy;


        QString legendString;

        if (id_list.size()==2)
        {
            Eigen::VectorXcd X=table->getLogicalColDataComplex(id_list[0].column());
            Eigen::VectorXcd V=table->getLogicalColDataComplex(id_list[1].column());

            data_x =X.real();
            data_y =X.imag();
            data_vx=V.real();
            data_vy=V.imag();

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
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

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
        QString name_x=table->getLogicalColName(id_list[0].column());
        QString name_y=table->getLogicalColName(id_list[1].column());

        cb_type->addItem(QString("Real(%1):Abscissa  Imag(%1):Ordinate  Phase(%2) :Scalar").arg(name_x).arg(name_y),1);
        cb_type->addItem(QString("Real(%1):Abscissa  Imag(%1):Ordinate  Module(%2):Scalar").arg(name_x).arg(name_y),2);
        cb_type->addItem(QString("Real(%1):Abscissa  Imag(%1):Ordinate  Real(%2)  :Scalar").arg(name_x).arg(name_y),3);
        cb_type->addItem(QString("Real(%1):Abscissa  Imag(%1):Ordinate  Imag(%2)  :Scalar").arg(name_x).arg(name_y),4);
    }
    else if (id_list.size()==3)
    {
        QString name_x=table->getLogicalColName(id_list[0].column());
        QString name_y=table->getLogicalColName(id_list[1].column());
        QString name_z=table->getLogicalColName(id_list[2].column());

        cb_type->addItem(QString("%1:Abscissa  %2:Ordinate  %3:ScalarField").arg(name_x).arg(name_y).arg(name_z),0);
    }

    if(cb_type->count()==0)
    {
        QMessageBox::information(this,"Information",
                                 "You may select :\n"\
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

    QComboBox* cb_mode=new QComboBox(dialog);
    cb_mode->addItem("NEAREST");
    cb_mode->addItem("WEIGHTED");
    cb_mode->setCurrentIndex(0);
    cb_mode->setToolTip("NEAREST : Look for the nearest neighbour and get his value.\n"\
                        "WEIGHTED : Look for the knn nearest neighbours and get a weighted average value.");

    QSpinBox* sb_knn=new QSpinBox(dialog);
    sb_knn->setRange(1,10000);
    sb_knn->setValue(5);
    sb_knn->setPrefix("Knn=");
    sb_knn->setToolTip("Number of nearest neighbour to search for");

    QGridLayout* gbox = new QGridLayout();
    gbox->addWidget(cb_type,0,0,1,3);
    gbox->addWidget(new QLabel("Resolution :"),1,0);
    gbox->addWidget(sb_resX,1,1);
    gbox->addWidget(sb_resY,1,2);
    gbox->addWidget(new QLabel("Interpolation :"),2,0);
    gbox->addWidget(cb_mode,2,1);
    gbox->addWidget(sb_knn,2,2);
    gbox->addWidget(buttonBox,3,0,1,3);

    dialog->setLayout(gbox);

    int result=dialog->exec();
    if(result==QDialog::Accepted)
    {
        Eigen::VectorXd data_x,data_y,data_z;
        QCPColorGradient::GradientPreset gradientType;
        bool ok=false;

        if (cb_type->currentData().toInt()>=1 && cb_type->currentData().toInt()<=4)//Phase plot
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
            else if(cb_type->currentData().toInt()==2)
            {
                for(int i=0;i<data_cplx_z.rows();i++)
                {
                    data_z[i]=std::abs(data_cplx_z[i]);
                }
                gradientType= QCPColorGradient::gpHues;
            }
            else if(cb_type->currentData().toInt()==3)
            {
                for(int i=0;i<data_cplx_z.rows();i++)
                {
                    data_z[i]=std::real(data_cplx_z[i]);
                }
                gradientType= QCPColorGradient::gpHues;
            }
            else if(cb_type->currentData().toInt()==4)
            {
                for(int i=0;i<data_cplx_z.rows();i++)
                {
                    data_z[i]=std::imag(data_cplx_z[i]);
                }
                gradientType= QCPColorGradient::gpHues;
            }

            ok=true;
        }
        else if (cb_type->currentData().toInt()==0)
        {
            data_x=table->getLogicalColDataDouble(id_list[0].column());
            data_y=table->getLogicalColDataDouble(id_list[1].column());
            data_z=table->getLogicalColDataDouble(id_list[2].column());
            gradientType= QCPColorGradient::gpPolar;
            ok=true;
        }

        if(ok)
        {
//            Viewer2D* viewer2d=new Viewer2D();
//            viewer2d->slot_setGradient(gradientType);
//            viewer2d->setMinimumSize(512+54,512);
//            QMdiSubWindow* subWindow = new QMdiSubWindow;
//            subWindow->setWidget(viewer2d);
//            subWindow->setAttribute(Qt::WA_DeleteOnClose);
//            mdiArea->addSubWindow(subWindow,Qt::WindowStaysOnTopHint);
//            viewer2d->show();
//            viewer2d->slot_setData(data_x,data_y,data_z,resolution);

            Viewer1D * viewer1d=createViewer1D(512+54,512);

            Curve2D curve(data_x,data_y,data_z,"Map",Curve2D::MAP);
            curve.getMapParams().resolutionX= sb_resX->value();
            curve.getMapParams().resolutionY= sb_resY->value();
            curve.getMapParams().knn= sb_knn->value();
            curve.getMapParams().mode=static_cast<QCPColorMap::MapParams::InterpolationMode>(cb_mode->currentIndex());
            curve.getStyle().gradientType=gradientType;

            std::cout<<data_z.rows()<<std::endl;
            std::cout<<curve.getScalarField().rows()<<std::endl;

            viewer1d->slot_add_data(curve);
        }
    }
}

void MainWindow::slot_plot_fft()
{
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

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
                                                     dialog->isSymetrical(),
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
        QMessageBox::information(this,"Information","Please select 1 columns");
    }
}

void MainWindow::slot_plot_cloud_3D()
{
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

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

        //mdiArea->addSubWindow(view3d->getContainer(),Qt::WindowStaysOnTopHint);
        view3d->getContainer()->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3 columns (x,y,z) or 4 columns (x,y,z,scalar)");
    }
}

void MainWindow::slot_plot_bode()
{
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Complex plot disambiguation");
        dialog->setMinimumWidth(400);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        QComboBox * cb_type=new QComboBox(dialog);

        QString nameX,nameY,nameZ;

        if (id_list.size()==1)
        {
            nameX="x";
            nameY=table->getLogicalColName(id_list[0].column());
            nameZ=nameY;

            cb_type->addItem(QString("Real(%1):Ordinate  Imag(%1):Ordinate").arg(nameY),0);
        }
        else if (id_list.size()==2)
        {
            nameX=table->getLogicalColName(id_list[0].column());
            nameY=table->getLogicalColName(id_list[1].column());
            nameZ=nameY;

            cb_type->addItem(QString("%1:Abscissa  Real(%2):Ordinate  Imag(%2):Ordinate").arg(nameX).arg(nameY),1);
        }
        else if (id_list.size()==3)
        {
            nameX=table->getLogicalColName(id_list[0].column());
            nameY=table->getLogicalColName(id_list[1].column());
            nameZ=table->getLogicalColName(id_list[2].column());

            cb_type->addItem(QString("%1:Abscissa  Module(%2):Ordinate  Phase(%3):Ordinate").arg(nameX).arg(nameY).arg(nameZ),2);
            cb_type->addItem(QString("%1:Abscissa  Real(%2)  :Ordinate  Imag(%3) :Ordinate").arg(nameX).arg(nameY).arg(nameZ),3);
        }

        if(cb_type->count()==0)
        {
            QMessageBox::information(this,"Information",
                                     "You may select :\n"\
                                     "-3 columns of real numbers\n"\
                                     "-1 column of real numbers and 1 column of complex numbers\n");
            return;
        }

        QGridLayout* gbox = new QGridLayout();
        gbox->addWidget(new QLabel("What does the selected data represent ?"),0,0,1,3);
        gbox->addWidget(cb_type,1,0,1,3);
        gbox->addWidget(buttonBox,2,0,1,3);
        dialog->setLayout(gbox);

        int result=dialog->exec();
        if(result==QDialog::Accepted)
        {
            Eigen::VectorXd data_x,data_y,data_z;

            if (cb_type->currentData()==2 || cb_type->currentData()==3)
            {
                ViewerBode * viewer_bode=createViewerBode();
                data_z=table->getLogicalColDataDouble(id_list[2].column());
                data_y=table->getLogicalColDataDouble(id_list[1].column());
                data_x=table->getLogicalColDataDouble(id_list[0].column());

                QString name=QString("%2=f(%1)").arg(nameX).arg(nameY);

                if(cb_type->currentData()==2)
                {
                    viewer_bode->slot_add_data(Curve2DComplex(data_x,data_y,data_z,Curve2DComplex::POLAR,name));
                }
                else
                {
                    viewer_bode->slot_add_data(Curve2DComplex(data_x,data_y,data_z,Curve2DComplex::CARTESIAN,name));
                }
            }
            else if (cb_type->currentData()==1)
            {
                ViewerBode * viewer_bode=createViewerBode();
                data_z=table->getLogicalColDataComplex(id_list[1].column()).imag();
                data_y=table->getLogicalColDataComplex(id_list[1].column()).real();
                data_x=table->getLogicalColDataDouble(id_list[0].column());

                QString name=QString("%2=f(%1)").arg(nameX).arg(nameY);
                viewer_bode->slot_add_data(Curve2DComplex(data_x,data_y,data_z,Curve2DComplex::CARTESIAN,name));
            }
            else if (cb_type->currentData()==0)
            {
                ViewerBode * viewer_bode=createViewerBode();
                data_z=table->getLogicalColDataComplex(id_list[0].column()).imag();
                data_y=table->getLogicalColDataComplex(id_list[0].column()).real();
                //data_x.resize(data_y.rows());
                data_x=Eigen::VectorXd::LinSpaced(data_y.rows(),0,1);

                QString name=QString("%2=f(%1)").arg(nameX).arg(nameY);
                viewer_bode->slot_add_data(Curve2DComplex(data_x,data_y,data_z,Curve2DComplex::CARTESIAN,name));
            }


        }

    }
}

void MainWindow::slot_plot_histogram()
{
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

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
    QTabWidget* tab=new QTabWidget(dialog);
    tab->addTab(tableShortcuts,"Shortcuts");

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
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

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
    le_pattern->setToolTip("Examples :\n"\
                           "To select row 1 and column 1 : R1,C1\n"\
                           "To select rows modulo two plus one : R%2+1");

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
    shortcuts_links.insert(QString("New")       ,ui->actionNew);
    shortcuts_links.insert(QString("Save")      ,ui->actionSave);
    shortcuts_links.insert(QString("Open")      ,ui->actionOpen);
    shortcuts_links.insert(QString("Export")    ,ui->actionExport);
    shortcuts_links.insert(QString("Parameters"),ui->actionParameters);
    shortcuts_links.insert(QString("Filter")    ,ui->actionFilter);
    shortcuts_links.insert(QString("CloseTab")  ,ui->actionCloseTab);

    QMapIterator<QString, QKeySequence> i(shortcuts_map);
    while (i.hasNext())
    {
        i.next();

        if (shortcuts_links.contains(i.key()))
        {
            shortcuts_links[i.key()]->setShortcut(i.value());
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

void MainWindow::slot_tab_moved(int from,int to)
{
    std::swap(tables[from],tables[to]);
}

void MainWindow::closeCurrentTable()
{
    closeTable(te_widget->currentIndex());
}

void MainWindow::closeTable(int index)
{
    if(index<0){return;}

    if(tables[index]->model()->isModified())
    {
        QMessageBox::StandardButton resBtn =
                QMessageBox::question( this, "Confirm close" ,
                                       QString("Some changes have not been saved :\n\n")+tables[index]->model()->getTabTitle()+QString("\n\nDiscard changes and exit ?\n"),
                                       QMessageBox::No | QMessageBox::Yes,
                                       QMessageBox::No);

        if (resBtn != QMessageBox::Yes)
        {
            return;
        }
    }

    delete tables[index];
    tables.removeAt(index);
    te_widget->removeTab(index);
}


void MainWindow::slot_showHideTerminal()
{
    te_results->show();
}

void MainWindow::slot_what(QString what)
{
    l_what->setText(what);
}

void MainWindow::slot_error(QString what,QString msg)
{
    QMessageBox::information(this,QString("Error : ")+what,msg);
    std::cout<<"Error : "<<what.toStdString()<<" : "<<msg.toStdString()<<std::endl;
}

void MainWindow::slot_progress(int t)
{
    pb_bar->setValue(t);
}

void MainWindow::slot_colourize()
{
    MyTableView * table=getCurrentTable();
    if(table==nullptr)return;

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
    bool somethingModified=false;

    QString filesNotSaved;

    for(int i=0;i<tables.size();i++)
    {
        if(tables[i]->model()->isModified())
        {
            somethingModified=true;
            filesNotSaved+=tables[i]->model()->getTabTitle()+QString("\n");
        }
    }

    if(somethingModified)
    {
        QMessageBox::StandardButton resBtn =
                QMessageBox::question( this, "Confirm quit" ,QString("Some changes have not been saved :\n\n")+filesNotSaved+QString("\nDiscard changes and exit ?\n"),
                                       QMessageBox::No | QMessageBox::Yes,
                                       QMessageBox::No);
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
