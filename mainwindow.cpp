#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "FIR.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mdiArea=new QMdiArea();
    table=new QTableView(mdiArea);

    this->setCentralWidget(mdiArea);

    connect(ui->actionOpen           , &QAction::triggered,this,&MainWindow::slot_open);
    connect(ui->actionSave           , &QAction::triggered,this,&MainWindow::slot_save);
    connect(ui->actionExport           , &QAction::triggered,this,&MainWindow::slot_export);
    connect(ui->actionPlot_GraphY    , &QAction::triggered,this,&MainWindow::slot_plot_y);
    connect(ui->actionPlot_GraphXY   ,&QAction::triggered,this,&MainWindow::slot_plot_graph_xy);
    connect(ui->actionPlot_CurveXY   ,&QAction::triggered,this,&MainWindow::slot_plot_curve_xy);
    connect(ui->actionPlot_MapXYZ    , &QAction::triggered,this,&MainWindow::slot_plot_xyz);
    connect(ui->actionHistogram      , &QAction::triggered,this,&MainWindow::slot_plot_histogram);
    connect(ui->actionPlot_Cloud_XYZ   ,&QAction::triggered,this,&MainWindow::slot_plot_cloud_xys);
    connect(ui->actionPlot_Cloud_3D  , &QAction::triggered,this,&MainWindow::slot_plot_cloud_3D);
    connect(ui->actionFFT  , &QAction::triggered,this,&MainWindow::slot_plot_fft);

    connect(ui->actionPlot_Gain_Phase  , &QAction::triggered,this,&MainWindow::slot_plot_gain_phase);

    connect(ui->actionTile,&QAction::triggered,mdiArea,&QMdiArea::tileSubWindows);
    connect(ui->actionCascade,&QAction::triggered,mdiArea,&QMdiArea::cascadeSubWindows);
    connect(ui->actionNext,&QAction::triggered,mdiArea,&QMdiArea::activateNextSubWindow);
    connect(ui->actionPrevious,&QAction::triggered,mdiArea,&QMdiArea::activatePreviousSubWindow);

    connect(ui->actionPoints,&QAction::triggered,this,&MainWindow::slot_mode_changed);
    connect(ui->actionLines,&QAction::triggered,this,&MainWindow::slot_mode_changed);

//    QMdiSubWindow* w = mdiArea->addSubWindow(table);
//    w->setWindowFlags(Qt::FramelessWindowHint);
//    w->showMaximized();

    mdiArea->setViewport(table);

    //mdiArea->viewport()->stackUnder(table);

    mdiArea->setSizeAdjustPolicy (QAbstractScrollArea::AdjustToContents);
    table->setSizeAdjustPolicy   (QAbstractScrollArea::AdjustToContents);

    direct_new(10,10);

    graphMode=MODE_POINTS;

    QActionGroup * actionGroup=new QActionGroup(this);
    actionGroup->addAction(ui->actionPoints);
    actionGroup->addAction(ui->actionLines);
    ui->actionPoints->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_open()
{
    QString filename=QFileDialog::getOpenFileName(this,"Open data CSV",current_filename,tr("Data file (*.csv *.graphy)"));

    if(!filename.isEmpty())
    {
        direct_open(filename);
    }
}

void MainWindow::direct_open(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        model = new QStandardItemModel;

        int lineindex = 0;                     // file line counter
        QTextStream in(&file);                 // read to text stream

        hasheader=false;

        while (!in.atEnd()) {

            // read one line from textstream(separated by "\n")
            QString fileLine = in.readLine();

            // parse the read line into separate pieces(tokens) with "," as the delimiter
            QStringList lineToken = fileLine.split(";", QString::SkipEmptyParts);

            if(lineToken.size()>0)
            {
                bool anumber;
                lineToken[0].toDouble(&anumber);
                if(!anumber)
                {
                    hasheader=true;
                }
            }

            if(hasheader && lineindex==0)
            {
                for (int j = 0; j < lineToken.size(); j++)
                {
                    QString value = lineToken.at(j);
                    QStandardItem *item = new QStandardItem(value);
                    model->setHorizontalHeaderItem(j, item);
                }
            }
            else
            {
                // load parsed data to model accordingly
                for (int j = 0; j < lineToken.size(); j++) {
                    QString value = lineToken.at(j);
                    QStandardItem *item = new QStandardItem(value);
                    model->setItem(lineindex-hasheader, j, item);
                }
            }

            lineindex++;
        }

        if(!hasheader)
        {
            for (int j = 0; j < model->columnCount(); j++)
            {
                QString value = QString("C%1").arg(j);
                QStandardItem *item = new QStandardItem(value);
                model->setHorizontalHeaderItem(j, item);
            }
        }

        table->setModel(model);
        setCurrentFilename(filename);

        updateTable();
    }
    else
    {
        QMessageBox::information(this,"Erreur",QString("Impossible d'ouvrir le fichier : ")+filename);
    }
}

void MainWindow::slot_save()
{
    QString filename=QFileDialog::getSaveFileName(this,"Save data CSV",current_filename,"*.csv");

    if(!filename.isEmpty())
    {
        direct_save(filename);
    }
}

void MainWindow::slot_export()
{
    QFileInfo info(current_filename);
    QString filename=QFileDialog::getSaveFileName(this,"Export data tex",info.path()+"/"+info.baseName()+".tex","*.tex");

    if(!filename.isEmpty())
    {
        direct_export(filename);
    }
}

void MainWindow::direct_export(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString textData;
        int rows = table->model()->rowCount();
        int columns = table->model()->columnCount();

        textData+= QString("\\begin{tabular}{|*{%1}{c|}} \n").arg(columns+1);
        textData += "\\hline \n";
        if(hasheader)
        {
            for (int j = 0; j < columns; j++)
            {
                textData+="$"+model->horizontalHeaderItem(j)->text()+"$";
                textData += "&";
            }
            textData += "\\\\ \\hline \n";
        }

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                bool okvf,okvi;
                double vf=table->model()->data(table->model()->index(i,j)).toDouble(&okvf);
                int vi=table->model()->data(table->model()->index(i,j)).toInt(&okvi);
                if(okvi)
                {
                        textData += QString::number(vi);
                }
                else if(okvf)
                {
                        textData += QString::number(vf,'f',1);
                }
                else
                {
                    textData+=table->model()->data(table->model()->index(i,j)).toString();
                }
                textData += "&";      // for .csv file format
            }
            textData += "\\\\ \\hline \n";             // (optional: for new line segmentation)
        }

        textData+=QString("\\end{tabular}");

        QTextStream out(&file);
        out << textData;



    }
    else
    {
        QMessageBox::information(this,"Erreur",QString("Impossible d'ouvrir le fichier : ")+filename);
    }
}

void MainWindow::direct_save(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString textData;
        int rows = table->model()->rowCount();
        int columns = table->model()->columnCount();

        if(hasheader)
        {
            for (int j = 0; j < columns; j++)
            {
                textData+=model->horizontalHeaderItem(j)->text();
                textData += ";";
            }
            textData += "\n";
        }

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {

                textData += table->model()->data(table->model()->index(i,j)).toString();
                textData += ";";      // for .csv file format
            }
            textData += "\n";             // (optional: for new line segmentation)
        }

        QTextStream out(&file);
        out << textData;

    }
    else
    {
        QMessageBox::information(this,"Erreur",QString("Impossible d'ouvrir le fichier : ")+filename);
    }
}

void MainWindow::direct_new(int sx,int sy)
{
    hasheader=false;
    model = new QStandardItemModel;

    for(int dx=0;dx<sx;dx++)
    {
        for(int dy=0;dy<sy;dy++)
        {
            QStandardItem *item = new QStandardItem();
            model->setItem(dx, dy, item);
        }
    }

    for (int j = 0; j < model->columnCount(); j++)
    {
        QString value = QString("C%1").arg(j);
        QStandardItem *item = new QStandardItem(value);
        model->setHorizontalHeaderItem(j, item);
    }

    table->setModel(model);

    setCurrentFilename("new.csv");
    updateTable();
}

void MainWindow::setCurrentFilename(QString filename)
{
    current_filename=filename;
    this->setWindowTitle(QString("Graphy v1.0 : %1").arg(current_filename));
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    table->hide();
    table->show();
}

void MainWindow::updateTable()
{
    datatable.clear();
    int rows = table->model()->rowCount();
    int columns = table->model()->columnCount();

    for (int j = 0; j < columns; j++)
    {
        QVector<double> coli;
        for (int i = 0; i < rows; i++)
        {
            coli.push_back( table->model()->data(table->model()->index(i,j)).toDouble() );
        }
        datatable.push_back(coli);
    }
}

void MainWindow::updateTable(int i,int j,double value)
{
    if(j<datatable.size())
    {
        if(i<datatable[j].size())
        {
            datatable[j][i]=value;
        }
    }
}


QString MainWindow::getColName(int id)
{
    auto item=model->horizontalHeaderItem(id);

    if(item!=nullptr)
    {
        return model->horizontalHeaderItem(id)->text();
    }
    else
    {
        return QString("C%1").arg(id);
    }

}

QVector<double> MainWindow::getCol(int id,const TableData & table)
{
    if(table.size()>id)
    {
        return table[id];
    }
    else
    {
        std::cout<<id<<" "<<table.size()<<std::endl;
        return QVector<double>();
    }
}

double getMin(QVector<double> data)
{
    double min=DBL_MAX;
    foreach(double d,data)
    {
        if(d<min)min=d;
    }
    return min;
}
double getMax(QVector<double> data)
{
    double max=DBL_MIN;
    foreach(double d,data)
    {
        if(d>max)max=d;
    }
    return max;
}

void MainWindow::slot_plot_y()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()>0)
    {
        Viewer1D * viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for(int k=0;k<id_list.size();k++)
        {
            QVector<double> data_y=getCol(id_list[k].column(),datatable);

            if(data_y.size()>0)
            {
                viewer1d->slot_add_data_graph(Curve2D(data_y,QString("Graph %1").arg(getColName(id_list[k  ].column()))));
            }
        }

        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(viewer1d);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        viewer1d->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select k columns (k>1)");
    }

}

void MainWindow::slot_plot_graph_xy()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()%2==0 && id_list.size()>0)
    {
        Viewer1D * viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for(int k=0;k<id_list.size();k+=2)
        {

            QVector<double> data_x=getCol(id_list[k  ].column(),datatable);
            QVector<double> data_y=getCol(id_list[k+1].column(),datatable);

            if(data_x.size()>0 && data_y.size()>0)
            {
                viewer1d->slot_add_data_graph(Curve2D(data_x,data_y,QString("Graph %2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column()))));
            }
        }

        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(viewer1d);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        viewer1d->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 2k columns (k>1)");
    }
}

void MainWindow::slot_plot_curve_xy()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()%2==0 && id_list.size()>0)
    {
        Viewer1D * viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for(int k=0;k<id_list.size();k+=2)
        {

            QVector<double> data_x=getCol(id_list[k  ].column(),datatable);
            QVector<double> data_y=getCol(id_list[k+1].column(),datatable);

            if(data_x.size()>0 && data_y.size()>0)
            {
                viewer1d->slot_add_data_curve(Curve2D(data_x,data_y,QString("Curve (%2,%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column()))));
            }
        }

        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(viewer1d);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        viewer1d->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 2k columns (k>1)");
    }
}

void MainWindow::slot_plot_cloud_xys()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()%3==0 && id_list.size()>0)
    {
        Viewer1D * viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for(int k=0;k<id_list.size();k+=3)
        {

            QVector<double> data_x=getCol(id_list[k  ].column(),datatable);
            QVector<double> data_y=getCol(id_list[k+1].column(),datatable);
            QVector<double> data_s=getCol(id_list[k+2].column(),datatable);

            if(data_x.size()>0 && data_y.size()>0)
            {
                Curve2D curve(data_x,data_y,QString("Curve (%2,%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column())));
                curve.setScalarField(data_s);
                viewer1d->slot_add_data_cloud(curve);
            }
        }

        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(viewer1d);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        viewer1d->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3k columns (k>1)");
    }
}

void MainWindow::slot_plot_xyz()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()==3)
    {
        QVector<double> data_x=getCol(id_list[0].column(),datatable);
        QVector<double> data_y=getCol(id_list[1].column(),datatable);
        QVector<double> data_z=getCol(id_list[2].column(),datatable);

        if(data_x.size()>0 && data_y.size()>0 && data_z.size()>0)
        {
            Viewer2D * viewer2d=new Viewer2D();
            viewer2d->setMinimumSize(600,400);

            QMdiSubWindow *subWindow = new QMdiSubWindow;
            subWindow->setWidget(viewer2d);
            subWindow->setAttribute(Qt::WA_DeleteOnClose);
            mdiArea->addSubWindow(subWindow);
            viewer2d->show();

            viewer2d->slot_set_data(datatable,BoxPlot(512,512,
                                                      id_list[0].column(),
                                    id_list[1].column(),
                    id_list[2].column()),5);
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3 columns");
    }
}

void MainWindow::slot_mode_changed()
{
    if(ui->actionPoints->isChecked())
    {
        graphMode=MODE_POINTS;
    }
    else if(ui->actionLines->isChecked())
    {
        graphMode=MODE_LINES;
    }
}

void MainWindow::slot_plot_fft()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()>0)
    {
        Viewer1D * viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for(int k=0;k<id_list.size();k++)
        {
            QVector<double> data_y=getCol(id_list[k].column(),datatable);

            std::vector< double > time=std::vector< double >(data_y.size()/2.0, 0);
            for(unsigned int i=0;i<time.size();i++)time[i]=i*1.0/data_y.size();

            FIR win;
            win.getHannCoef(data_y.size());

            for(int i=0;i<data_y.size();i++)
            {
                data_y[i]*=win.at(i);
            }

            Eigen::FFT<double> fft;
            fft.SetFlag(Eigen::FFT<double>::HalfSpectrum);
            fft.SetFlag(Eigen::FFT<double>::Unscaled);

            std::vector< std::complex<double> > fft_data_cplx;
            fft.fwd(fft_data_cplx,data_y.toStdVector());
            fft_data_cplx.pop_back();

            std::vector< double > fft_data_module(fft_data_cplx.size());
            for(unsigned int k=0;k<fft_data_cplx.size();k++)fft_data_module[k]=20*log10(std::abs(fft_data_cplx[k]));



            if(data_y.size()>0)
            {
                viewer1d->slot_add_data_graph(Curve2D(QVector<double>::fromStdVector(time),QVector<double>::fromStdVector(fft_data_module) ,QString("FFT %1").arg(getColName(id_list[k  ].column()))));
            }
        }

        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(viewer1d);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        viewer1d->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select k columns (k>1)");
    }
}

void MainWindow::slot_plot_cloud_3D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()==3 || id_list.size()==4)
    {
        View3D * view3d=new View3D;
        view3d->addGrid(0.01,100,QColor(64,64,64));
        QVector<double> data_x=getCol(id_list[0].column(),datatable);
        QVector<double> data_y=getCol(id_list[1].column(),datatable);
        QVector<double> data_z=getCol(id_list[2].column(),datatable);

        if(id_list.size()==3)
        {
            Cloud * cloud=new Cloud(data_x,data_y,data_z);

            if(graphMode==MODE_POINTS)
            {
                view3d->addCloud(cloud,QColor(0,255,0));
            }
            else
            {
                view3d->addCloudLine(cloud,QColor(0,255,0));
            }
        }
        else if(id_list.size()==4)
        {
            QVector<double> data_s=getCol(id_list[3].column(),datatable);
            CloudScalar * cloud=new CloudScalar(data_x,data_y,data_z,data_s);

            if(graphMode==MODE_POINTS)
            {
                view3d->addCloudScalar(cloud);
            }
            else
            {
                view3d->addCloudScalarLine(cloud);
            }
        }

//        mdiArea->addSubWindow(view3d->getContainer());
        view3d->getContainer()->show();

//        view3d->show();

    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3 columns (x,y,z) or 4 columns (x,y,z,scalar)");
    }
}

void MainWindow::slot_plot_gain_phase()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();


    if(id_list.size()%3==0 && id_list.size()>0)
    {
        Viewer1DCPLX * viewer1d=new Viewer1DCPLX();
        viewer1d->setMinimumSize(600,400);

        for(int k=0;k<id_list.size();k+=3)
        {

            QVector<double> data_f=getCol(id_list[k  ].column(),datatable);
            QVector<double> data_gain=getCol(id_list[k+1].column(),datatable);
            QVector<double> data_phase=getCol(id_list[k+2].column(),datatable);

            if(data_f.size()>0 && data_gain.size()>0 && data_phase.size()>0)
            {
                viewer1d->slot_add_data_graph(
                Curve2D_GainPhase(data_f,data_gain,data_phase,QString("Gain %2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column()))
                ,QString("Phase %2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+2].column())))
                );
            }
        }

        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(viewer1d);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        viewer1d->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3k columns (k>1)");
    }
}

void MainWindow::slot_plot_surface_3D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()==3)
    {
        QVector<double> data_x=getCol(id_list[0].column(),datatable);
        QVector<double> data_y=getCol(id_list[1].column(),datatable);
        QVector<double> data_z=getCol(id_list[2].column(),datatable);

        ViewerSurface3D * viewer3d=new ViewerSurface3D;

        Cloud * cloud=new Cloud(data_x,data_y,data_z);
        viewer3d->set_data(*cloud);

        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWindowTitle("Surface");
        subWindow->setWidget(viewer3d);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        viewer3d->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3 columns");
    }
}

void MainWindow::slot_plot_histogram()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()>0)
    {
        Viewer1D * viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for(int k=0;k<id_list.size();k++)
        {
            QVector<double> data_y=getCol(id_list[k].column(),datatable);

            if(data_y.size()>0)
            {
                viewer1d->slot_histogram(data_y,QString("Curve Histogram %1").arg(getColName(id_list[k  ].column())));
            }
        }

        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(viewer1d);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        viewer1d->show();
    }
    else
    {
        QMessageBox::information(this,"Information","Please select k columns (k>1)");
    }

}
