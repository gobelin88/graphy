#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    connect(ui->actionPlot_GraphY    , &QAction::triggered,this,&MainWindow::slot_plot_y);
    connect(ui->actionPlot_GraphXY   ,&QAction::triggered,this,&MainWindow::slot_plot_graph_xy);
    connect(ui->actionPlot_CurveXY   ,&QAction::triggered,this,&MainWindow::slot_plot_curve_xy);
    connect(ui->actionPlot_MapXYZ    , &QAction::triggered,this,&MainWindow::slot_plot_xyz);
    connect(ui->actionHistogram      , &QAction::triggered,this,&MainWindow::slot_plot_histogram);
    connect(ui->actionPlot_Cloud_3D  , &QAction::triggered,this,&MainWindow::slot_plot_cloud_3D);


    connect(ui->actionTile,&QAction::triggered,mdiArea,&QMdiArea::tileSubWindows);
    connect(ui->actionCascade,&QAction::triggered,mdiArea,&QMdiArea::cascadeSubWindows);
    connect(ui->actionNext,&QAction::triggered,mdiArea,&QMdiArea::activateNextSubWindow);
    connect(ui->actionPrevious,&QAction::triggered,mdiArea,&QMdiArea::activatePreviousSubWindow);

//    QMdiSubWindow* w = mdiArea->addSubWindow(table);
//    w->setWindowFlags(Qt::FramelessWindowHint);
//    w->showMaximized();

    mdiArea->setViewport(table);

    mdiArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    direct_new(10,10);
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

void MainWindow::slot_plot_cloud_3D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if(id_list.size()==3)
    {
        QVector<double> data_x=getCol(id_list[0].column(),datatable);
        QVector<double> data_y=getCol(id_list[1].column(),datatable);
        QVector<double> data_z=getCol(id_list[2].column(),datatable);

        ViewerScatter3D * viewer3d=new ViewerScatter3D;

        Cloud * cloud=new Cloud(data_x,data_y,data_z);
        viewer3d->set_data(*cloud);

        QMdiSubWindow *subWindow = new QMdiSubWindow;
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
