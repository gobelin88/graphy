#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <functional>

#include "FIR.h"

///////////////////
void MainWindow::createExperimental()
{
    experimental_table=new MyTableView(100,4,10,mdiArea);
    te_widget->addTab(experimental_table,"Experimental");

    experimental_table->addAction(a_newRow);
    experimental_table->addAction(a_newRows);
    experimental_table->addAction(a_updateColumns);
    experimental_table->addAction(a_delete);
    experimental_table->addAction(a_removeColumnsRows);


    connect(a_newRow,&QAction::triggered,experimental_table->model(),&MyModel::slot_newRow);
    connect(a_newRows,&QAction::triggered,experimental_table->model(),&MyModel::slot_newRows);
    connect(a_updateColumns,&QAction::triggered,experimental_table->model(),&MyModel::slot_updateColumns);
    connect(a_delete,&QAction::triggered,experimental_table,&MyTableView::slot_deleteSelected);
    connect(a_removeColumnsRows,&QAction::triggered,experimental_table,&MyTableView::slot_removeSelectedRowsAndCols);

}

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    isModified=false;
    separator=";";

    ui->setupUi(this);
    mdiArea=new QMdiArea();
    table=new QTableView(mdiArea);

    model=nullptr;
    old_model=table->model();

    //  tableview.setSelectionBehavior(tableview.SelectRows)
    //  tableview.setSelectionMode(tableview.SingleSelection)
    //  table->setDragDropMode(QTableView::InternalMove);
    //  table->setDragDropOverwriteMode(false);
    //  table->setSortingEnabled(true);

    table->horizontalHeader()->setSectionsMovable(true);
    table->verticalHeader()->setSectionsMovable(true);


    this->setCentralWidget(mdiArea);

    connect(table->horizontalHeader(),SIGNAL(sectionMoved(int,int,int)),this,SLOT(slot_hSectionMoved(int,int,int)));
    connect(table->verticalHeader(),SIGNAL(sectionMoved(int,int,int)),this,SLOT(slot_vSectionMoved(int,int,int)));

    std::cout<<"A"<<std::endl;

    connect(ui->actionNew, &QAction::triggered,this,&MainWindow::slot_new);
    connect(ui->actionOpen, &QAction::triggered,this,&MainWindow::slot_open);//ok
    connect(ui->actionSave, &QAction::triggered,this,&MainWindow::slot_save);//ok
    connect(ui->actionSaveAs, &QAction::triggered,this,&MainWindow::slot_save_as);//ok
    connect(ui->actionExport, &QAction::triggered,this,&MainWindow::slot_export);
    connect(ui->actionParameters, &QAction::triggered,this,&MainWindow::slot_parameters);

    connect(ui->actionPlot_GraphY, &QAction::triggered,this,&MainWindow::slot_plot_y);
    connect(ui->actionPlot_GraphXY,&QAction::triggered,this,&MainWindow::slot_plot_graph_xy);
    connect(ui->actionPlot_CurveXY,&QAction::triggered,this,&MainWindow::slot_plot_curve_xy);
    connect(ui->actionPlot_MapXYZ, &QAction::triggered,this,&MainWindow::slot_plot_map_2D);
    connect(ui->actionHistogram, &QAction::triggered,this,&MainWindow::slot_plot_histogram);
    connect(ui->actionPlot_Field_2D,&QAction::triggered,this,&MainWindow::slot_plot_field_2D);
    connect(ui->actionPlot_Cloud_2D,&QAction::triggered,this,&MainWindow::slot_plot_cloud_2D);
    connect(ui->actionPlot_Cloud_3D, &QAction::triggered,this,&MainWindow::slot_plot_cloud_3D);
    connect(ui->actionFFT, &QAction::triggered,this,&MainWindow::slot_plot_fft);
    connect(ui->actionPlot_Gain_Phase, &QAction::triggered,this,&MainWindow::slot_plot_gain_phase);

    connect(ui->actionFilter, &QAction::triggered,this,&MainWindow::slot_filter);
    connect(ui->actionSelection_Pattern, &QAction::triggered,this,&MainWindow::slot_select);
    connect(ui->actionColourize, &QAction::triggered,this,&MainWindow::slot_colourize);

    std::cout<<"B"<<std::endl;

    //    connect(ui->actionTile,&QAction::triggered,mdiArea,&QMdiArea::tileSubWindows);
    //    connect(ui->actionCascade,&QAction::triggered,mdiArea,&QMdiArea::cascadeSubWindows);
    //    connect(ui->actionNext,&QAction::triggered,mdiArea,&QMdiArea::activateNextSubWindow);
    //    connect(ui->actionPrevious,&QAction::triggered,mdiArea,&QMdiArea::activatePreviousSubWindow);

    //    QMdiSubWindow* w = mdiArea->addSubWindow(table);
    //    w->setWindowFlags(Qt::FramelessWindowHint);
    //    w->showMaximized();

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

    std::cout<<"C"<<std::endl;

    connect(table->horizontalHeader(),&QHeaderView::sectionDoubleClicked,this,&MainWindow::slot_sectionDoubleClicked);
    connect(a_newColumn,&QAction::triggered,this,&MainWindow::slot_editColumn);
    connect(a_newRow,&QAction::triggered,this,&MainWindow::slot_newRow);
    connect(a_newRows,&QAction::triggered,this,&MainWindow::slot_newRows);
    connect(a_delete,&QAction::triggered,this,&MainWindow::slot_delete_selected);
    connect(a_removeColumnsRows,&QAction::triggered,this,&MainWindow::slot_remove_columns_and_rows);
    connect(a_updateColumns,&QAction::triggered,this,&MainWindow::slot_updateColumns);

    connect(a_copy,&QAction::triggered,this,&MainWindow::slot_copy);
    connect(a_paste,&QAction::triggered,this,&MainWindow::slot_paste);

    connect(&reg,&Register::sig_modified,this,&MainWindow::fileModified);
    //------------------------------------------------------------------------------

    te_results=new QTextEdit;

    te_widget=new QTabWidget();
    te_widget->addTab(table,"Data");
    te_widget->addTab(te_results,"Results");

    //mdiArea->setViewport(te_widget);

    QMdiSubWindow* subWindow = mdiArea->addSubWindow(te_widget, Qt::FramelessWindowHint );
    //mdiArea->addSubWindow(te_widget, Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint );
    subWindow->showMaximized();
    mdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation);

    mdiArea->setSizeAdjustPolicy (QAbstractScrollArea::AdjustToContents);
    //table->setSizeAdjustPolicy   (QAbstractScrollArea::AdjustToContents);

    std::cout<<"D"<<std::endl;
    directNew(3,3);


    std::cout<<"E"<<std::endl;
    loadShortcuts();

    createExperimental();
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
        directNew(sb_sx->value(),sb_sy->value());
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

QStringList MainWindow::extractToken(QString fileLine)
{
    if (fileLine.endsWith("\n"))
    {
        fileLine.chop(1);
    }
    if (fileLine.endsWith(separator))
    {
        fileLine.chop(1);
    }

    return fileLine.split(separator);
}

void MainWindow::direct_open(QString filename)
{
    experimental_table->model()->open(filename);

    createModel();

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        clear();

        int lineindex = 0;                     // file line counter
        int numberOfColumns=0;
        QTextStream in(&file);                 // read to text stream

        hasheader=false;

        while (!in.atEnd())
        {
            // read one line from textstream(separated by "\n")
            QString dataLine = in.readLine();

            //std::cout<<dataLine.toLocal8Bit().data()<<std::endl;

            if (dataLine=="<header>")
            {
                hasheader=true;
                QString varLine = in.readLine();
                QString expLine = in.readLine();
                QStringList varToken = extractToken(varLine);
                dataLine=expLine;

                QStringList expToken;
                if(expLine!=QString("</header>"))
                {
                    expToken = extractToken(expLine);
                }
                else
                {
                    for(int i=0;i<varToken.size();i++)
                    {
                        expToken.push_back(QString(""));
                    }
                }


                if(varToken.size()==expToken.size())
                {
                    for (int j = 0; j < varToken.size(); j++)
                    {
                        QString varname = varToken.at(j);
                        QString varexpr = expToken.at(j);
                        if ( reg.newVariable(varname,varexpr) )
                        {
                            model->setHorizontalHeaderItem(j, new QStandardItem(varname));
                        }
                        else
                        {
                            clear();
                            affectModel();
                            return;
                        }
                    }
                }
                else
                {
                    error("Open",QString("Number of expressions (%1) and number of variables (%2) are different.").arg(expToken.size()).arg(varToken.size()));
                    clear();
                    affectModel();
                    return;
                }

                while (dataLine!="</header>" && !in.atEnd())
                {
                    dataLine = in.readLine();
                }

            }
            else
            {
                QStringList valuesToken=extractToken(dataLine);
                if(lineindex==0 && !hasheader)
                {
                    numberOfColumns=valuesToken.size();
                }
                else if (lineindex==0 && hasheader)
                {
                    numberOfColumns=reg.variablesNames().size();
                }

                if(valuesToken.size()==numberOfColumns)
                {
                    addModelRow(valuesToken);
                    lineindex++;
                }
                else
                {
                    error("Open",QString("Error line %1 detected\n%2 elements expected got %3 elements").arg(lineindex+1).arg(numberOfColumns).arg(valuesToken.size()));
                    return;
                }
            }
        }


        if (!hasheader)//add default header
        {
            for (int j = 0; j < model->columnCount(); j++)
            {
                QString varname = QString("C%1").arg(j+1);
                QString varexpr =QString("");
                QStandardItem* item = new QStandardItem(varname);
                model->setHorizontalHeaderItem(j, item);
                reg.newVariable(varname,varexpr);
            }
        }


        affectModel();


        setCurrentFilename(filename);
        updateTable();

        table->resizeColumnsToContents();

        isModified=false;
    }
    else
    {
        QMessageBox::information(this,"Erreur",QString("Impossible d'ouvrir le fichier : ")+filename);
    }
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
    QString filename=QFileDialog::getSaveFileName(this,"Save data",current_filename,"*.csv");

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
        direct_export(filename);
    }
}

void MainWindow::direct_export(QString filename)
{
    QFile file(filename);

    bool ok;
    int prec=QInputDialog::getInt(this,"Precision","Floating point precision",1,0,30,1,&ok);

    if (!ok)
    {
        return ;
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString textData;
        int rows = table->model()->rowCount();
        int columns = table->model()->columnCount();

        textData+= QString("\\begin{tabular}{|*{%1}{c|}} \n").arg(columns);
        textData += "\\hline \n";
        if (hasheader)
        {
            for (int j = 0; j < columns; j++)
            {
                textData+="$"+model->horizontalHeaderItem(j)->text()+"$";
                if (j!=columns-1)
                {
                    textData += "&";
                }
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
                if (okvi)
                {
                    textData += fromNumber(vi);
                }
                else if (okvf)
                {
                    textData += fromNumber(vf,prec);
                }
                else
                {
                    textData+=table->model()->data(table->model()->index(i,j)).toString();
                }

                if (j!=columns-1)
                {
                    textData += "&";
                }
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
//    if(experimental_table->model()->save(filename))
//    {
//        setCurrentFilename(filename);
//        isModified=false;
//    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        setCurrentFilename(filename);

        QString textData;
        int rows = table->model()->rowCount();
        int columns = table->model()->columnCount();

        if (hasheader)
        {
            textData += "<header>\n";
            for (int j = 0; j < columns; j++)
            {
                textData+=reg.variablesNames()[j];
                textData += separator;
            }
            textData += "\n";
            for (int j = 0; j < columns; j++)
            {
                textData+=reg.variablesExpressions()[j];
                textData += separator;
            }
            textData += "\n";
            textData += "</header>\n";
        }

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {

                textData += at(i,j);
                textData += separator;      // for .csv file format
            }
            textData += "\n";             // (optional: for new line segmentation)
        }

        QTextStream out(&file);
        out << textData;

        isModified=false;
    }
    else
    {
        QMessageBox::information(this,"Erreur",QString("Impossible d'ouvrir le fichier : ")+filename);
    }
}

void MainWindow::slot_newRow()
{
    int nbCols=model->columnCount();
    QList<QStandardItem*> items;
    items.reserve(nbCols);
    for (int i=0; i<nbCols; i++)
    {
        items.append(new QStandardItem);
    }

    model->appendRow(items);
    addRow(datatable,Eigen::VectorXd::Zero(items.size()));
}

void MainWindow::slot_newRows()
{
    int nbCols=model->columnCount();

    bool ok=false;
    int N=QInputDialog::getInt(this,"Number of rows","Number of rows to add",1,1,10000000,1,&ok);

    if (ok)
    {
        QElapsedTimer timer;
        timer.start();

        for (int k=0; k<N; k++)
        {
            QList<QStandardItem*> items;
            items.reserve(nbCols);
            for (int i=0; i<nbCols; i++)
            {
                items.append(new QStandardItem);
            }

            model->appendRow(items);
        }

        std::cout<<timer.nsecsElapsed()*1e-9<<std::endl;
        addRows(datatable,N);
    }
}

void MainWindow::slot_sectionDoubleClicked()
{
    slot_editColumn();
}

void MainWindow::slot_editColumn()
{
    int currentColIndex=model->columnCount();
    int visualIndex=currentColIndex;

    QModelIndexList id_list=table->selectionModel()->selectedColumns();
    if (id_list.size()==1)
    {
        currentColIndex=id_list[0].column();
        visualIndex=table->horizontalHeader()->visualIndex( id_list[0].column() );
    }

    if (reg.editVariableAndExpression(visualIndex))
    {
        setColumn(visualIndex,evalColumn(visualIndex));
        model->setHorizontalHeaderItem(currentColIndex, new QStandardItem(reg.variablesNames()[visualIndex]));
        hasheader=true;
    }
}

void MainWindow::slot_updateColumns()
{
    int nbCols=model->columnCount();

    for (int i=0; i<nbCols; i++)
    {
        setColumn(i,evalColumn( i ));
    }
}

void MainWindow::slot_newColumn(QString name,Eigen::VectorXd data)
{
    if (data.size()==datatable.rows())
    {
        int index=datatable.cols();
        reg.newVariable(name,"");
        setColumn(index,toQVectorStr(data));
        model->setHorizontalHeaderItem(index, new QStandardItem(name));

        fileModified();
    }
}

void MainWindow::slot_results(QString results)
{
    te_results->append("------------------------");
    te_results->append(results);
}

void MainWindow::slot_delete_selectedColumns()
{
    //table->setModel(new QStandardItemModel);

    QModelIndexList id_list_cols=table->selectionModel()->selectedColumns();

    //////////////////////////////////////////////
    std::vector<unsigned int> indexs_cols,indexs_cols_visual;
    for(int i=0;i<id_list_cols.size();i++)
    {
        indexs_cols.push_back(id_list_cols[i].column());
        indexs_cols_visual.push_back(table->horizontalHeader()->visualIndex(id_list_cols[i].column()));
    }
    std::sort(indexs_cols.begin(),indexs_cols.end());
    std::sort(indexs_cols_visual.begin(),indexs_cols_visual.end());

    //////////////////////////////////////////////
    if (indexs_cols.size()>=1)
    {
        modelMute();
        //-----------------------------------------------------logical
        std::cout<<"ok A"<<std::endl;

        QAbstractItemModel * pabstract_model=model;

        for (int i=0; i<indexs_cols.size();)
        {
            int indexa=indexs_cols[i];

            int n=1;
            if ((i+n)<indexs_cols.size())
            {
                int indexb=indexs_cols[i+n];
                while ((i+n)<indexs_cols.size() && indexb==indexa+n)
                {
                    n++;
                    if ((i+n)<indexs_cols.size())
                    {
                        indexb=indexs_cols[i+n];
                    }
                }
            }

            pabstract_model->removeColumns(indexa-i,n);


            i+=n;
        }

        std::cout<<"ok B"<<std::endl;

        //-----------------------------------------------------visual
        for (int i=0; i<indexs_cols_visual.size();)
        {
            int indexa=indexs_cols_visual[i];

            int n=1;
            if ((i+n)<indexs_cols_visual.size())
            {
                int indexb=indexs_cols_visual[i+n];
                while ((i+n)<indexs_cols_visual.size() && indexb==indexa+n)
                {
                    n++;
                    if ((i+n)<indexs_cols_visual.size())
                    {
                        indexb=indexs_cols_visual[i+n];
                    }
                }
            }

            for (int k=0; k<n; k++)
            {
                reg.delVariable(reg.variablesNames()[indexa-i]);
            }

            removeColumns(datatable,indexa-i,n);

            i+=n;
        }

        std::cout<<"ok C"<<std::endl;

        fileModified();
        modelUnMute();
    }
}

void MainWindow::slot_delete_selectedRows()
{


    QModelIndexList id_list_rows=table->selectionModel()->selectedRows();

    //////////////////////////////////////////////
    std::vector<unsigned int> indexs_rows,indexs_rows_visual;
    for(int i=0;i<id_list_rows.size();i++)
    {
        indexs_rows.push_back(id_list_rows[i].row());
        indexs_rows_visual.push_back(table->verticalHeader()->visualIndex(id_list_rows[i].row()));
    }
    std::sort(indexs_rows.begin(),indexs_rows.end());
    std::sort(indexs_rows_visual.begin(),indexs_rows_visual.end());

    //////////////////////////////////////////////
    if (indexs_rows.size()>=1)
    {
        modelMute();

        //-----------------------------------------------------logical
        for (int i=0; i<indexs_rows.size();)
        {
            int indexa=indexs_rows[i];

            int n=1;
            if ((i+n)<indexs_rows.size())
            {
                int indexb=indexs_rows[i+n];
                while ((i+n)<indexs_rows.size() && indexb==indexa+n)
                {
                    n++;
                    if ((i+n)<indexs_rows.size())
                    {
                        indexb=indexs_rows[i+n];
                    }
                }
            }

            model->removeRows(indexa-i,n);

            i+=n;
        }

        //-----------------------------------------------------visual
        for (int i=0; i<indexs_rows_visual.size();)
        {
            int indexa=indexs_rows_visual[i];

            int n=1;
            if ((i+n)<indexs_rows_visual.size())
            {
                int indexb=indexs_rows_visual[i+n];
                while ((i+n)<indexs_rows_visual.size() && indexb==indexa+n)
                {
                    n++;
                    if ((i+n)<indexs_rows_visual.size())
                    {
                        indexb=indexs_rows_visual[i+n];
                    }
                }
            }

            removeRows(datatable,indexa-i,n);

            i+=n;
        }


        fileModified();
        modelUnMute();
    }


}

void MainWindow::slot_delete_selected()
{
    QModelIndexList selectedIndexes = table->selectionModel()->selectedIndexes();

    for (int i = 0; i < selectedIndexes.count(); ++i)
    {
        QModelIndex current = selectedIndexes[i];
        model->item(current.row(),current.column())->setText("");
    }
    fileModified();
}

void MainWindow::slot_remove_columns_and_rows()
{
    QElapsedTimer timer;
    timer.start();

    slot_delete_selectedColumns();
    slot_delete_selectedRows();

    std::cout<<"dt="<<timer.nsecsElapsed()*1e-9<<"s "<<std::endl;

    //std::cout<<datatable<<std::endl;
    //dispVariables();
}


QString MainWindow::fromNumber(double value)
{
    //return QString::number(value,'f',internal_precision);//Mauvaise idée
    if(!std::isnan(value))
    {
        return QString::number(value);
    }
    else
    {
        return QString("");
    }
}
QString MainWindow::fromNumber(double value,int precision)
{
    return QString::number(value,'f',precision);
}

QVector<QString> MainWindow::evalColumn(int colId)
{
    int logicalColId=colId;
    reg.setActiveCol(colId);

    assert(logicalColId<variables_expressions.size());

    if (reg.variablesExpressions()[logicalColId].isEmpty())
    {
        return QVector<QString>();
    }
    else
    {
        int nbRows=model->rowCount();

        QVector<QString> colResults(nbRows);


        if (reg.compileExpression(logicalColId))
        {
            for (int i=0; i<nbRows; i++)
            {
                reg.setActiveRow(i);
                for (int j=0; j<datatable.cols(); j++)
                {
                    reg.setVariable(j,datatable(i,j));
                }


                colResults[i]=fromNumber(reg.currentCompiledExpressionValue());
            }
        }
        else
        {
            for (int i=0; i<nbRows; i++)
            {
                reg.setActiveRow(i);
                for (int j=0; j<datatable.cols(); j++)
                {
                    reg.setVariable(j,datatable(i,j));
                }


                // store a call to a member function and object ptr
                using std::placeholders::_1;
                using std::placeholders::_2;
                reg.customExpressionParse(logicalColId,colResults[i],  std::bind( &MainWindow::at, this, _1, _2 ),i);
            }
        }

        return colResults;
    }
}

void MainWindow::error(QString title,QString msg)
{
    QMessageBox::information(this,QString("Error : ")+title,msg);
    std::cout<<"Error : "<<msg.toLocal8Bit().data()<<std::endl;
}

QVector<QString> MainWindow::getColumn(int idCol)
{
    int nbRows=model->rowCount();
    QVector<QString> contentCol(nbRows);
    for (int i=0; i<nbRows; i++)
    {
        contentCol[i]=at(i,idCol);
    }
    return contentCol;
}

QVector<QString> MainWindow::getRow(int idRow)
{
    int nbCols=model->columnCount();
    QVector<QString> contentRow(nbCols);
    for (int i=0; i<nbCols; i++)
    {
        contentRow[i]=at(idRow,i);
    }
    return contentRow;
}

void MainWindow::setColumn(int idCol,const QVector<QString>& vec_col)
{
    QList<QStandardItem*> items;
    items.reserve(model->rowCount());
    int nbRows=model->rowCount();

    if (idCol<model->columnCount()) //set
    {
        modelMute();
        if (vec_col.size()>0)
        {
            for (int i=0; i<nbRows; i++)
            {
                QStandardItem* item=itemAt(i,idCol);
                if (item)
                {
                    item->setText(vec_col[i]);
                }
            }
            datatable.col(idCol)=toSafeDouble(vec_col);
        }
        modelUnMute();
    }
    else//new column
    {
        modelMute();
        if (vec_col.size()==0)
        {
            for (int i=0; i<nbRows; i++)
            {
                items.append(new QStandardItem());
            }
            addColumn(datatable,Eigen::VectorXd::Zero(nbRows));
            model->appendColumn(items);
        }
        else
        {
            for (int i=0; i<nbRows; i++)
            {
                items.append(new QStandardItem(vec_col[i]));
            }
            addColumn(datatable,toSafeDouble(vec_col));
            model->appendColumn(items);
        }
        modelUnMute();
    }
}

void MainWindow::addModelRow(const QStringList& str_row)
{
    QList<QStandardItem*> items;
    items.reserve(str_row.size());
    for (int i=0; i<str_row.size(); i++)
    {
        items.append(new QStandardItem(str_row[i]));
    }
    model->appendRow(items);
}

void MainWindow::addModelRow(const Eigen::VectorXd & value_row)
{
    QList<QStandardItem*> items;
    items.reserve(value_row.size());
    for (int i=0; i<value_row.size(); i++)
    {
        if(std::isnan(value_row[i]))
        {
            items.append(new QStandardItem());
        }
        else
        {
            items.append(new QStandardItem(fromNumber(value_row[i])));
        }
    }
    model->appendRow(items);
}

void MainWindow::clear()
{
    reg.clear();
    //model->removeRows(0,model->rowCount());
    //model->removeColumns(0,model->columnCount());

    model->clear();
}

void MainWindow::createModel()
{
    if(model)
    {
        old_model=model;
    }
    model = new QStandardItemModel;
}

void MainWindow::affectModel()
{
    table->setModel(model);
    if(old_model)
    {
        disconnect(old_model,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(updateTable(const QModelIndex&,const QModelIndex&)));
        delete old_model;
    }
    connect(model,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(updateTable(const QModelIndex&,const QModelIndex&)));
}

void MainWindow::modelMute()
{
    table->setUpdatesEnabled(false);
    model->blockSignals(true);
}

void MainWindow::modelUnMute()
{
    model->blockSignals(false);
    table->setUpdatesEnabled(true);
    model->layoutChanged();
}

void MainWindow::directNew(int sx,int sy)
{
    createModel();

    QElapsedTimer timer;
    timer.start();
    clear();

    hasheader=false;

    //QStandardItem * new_items=new QStandardItem[sx*sy];
    for (int dx=0,id=0; dx<sx; dx++)
    {
        for (int dy=0; dy<sy; dy++)
        {
            //model->setItem(dx, dy, new_items+id);
            model->setItem(dx, dy, new QStandardItem);
            id++;
        }
    }

    for (int j = 0; j < model->columnCount(); j++)
    {
        QString value = QString("C%1").arg(j+1);
        reg.newVariable(value,"");
        model->setHorizontalHeaderItem(j, new QStandardItem(value));
    }


    //table->setUpdatesEnabled(true);

    affectModel();

    setCurrentFilename("");
    isModified=false;
    updateTable();

    std::cout<<"direct_new() dt="<<timer.nsecsElapsed()*1e-9<<"s "<<std::endl;
}

void MainWindow::setCurrentFilename(QString filename)
{
    current_filename=filename;
    this->setWindowTitle(QString("Graphy %1 : %2").arg(graphyVersion).arg(current_filename));
}

void MainWindow::fileModified()
{
    this->setWindowTitle(QString("Graphy %1 : %2*").arg(graphyVersion).arg(current_filename));
    isModified=true;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    table->hide();
    table->show();
}

void MainWindow::updateTable()
{
    int rows = table->model()->rowCount();
    int columns = table->model()->columnCount();

    datatable=Eigen::MatrixXd(rows,columns);

    for (int j = 0; j < columns; j++)
    {
        for (int i = 0; i < rows; i++)
        {
            datatable(i,j)=toSafeDouble(at(i,j));
        }
    }
}

void MainWindow::updateTableViewRows()
{
    model->removeRows(0,model->rowCount());

    for (int i = 0; i < datatable.rows(); i++)
    {
        addModelRow(datatable.row(i));
    }

}

QStandardItem* MainWindow::itemAt(int i,int j)
{
    int row=table->verticalHeader()->logicalIndex(i);
    int column=table->horizontalHeader()->logicalIndex(j);
    return model->item(row,column);
}

QString MainWindow::at(int i,int j)
{
    if(itemAt(i,j))
    {
        return itemAt(i,j)->text();
    }
    else
    {
        return QString();
    }
}

void MainWindow::updateTable(const QModelIndex& indexA,const QModelIndex& indexB)
{
    Q_UNUSED(indexB)

    int i=table->verticalHeader()->visualIndex(indexA.row());
    int j=table->horizontalHeader()->visualIndex(indexA.column());

    if (i<datatable.rows() && j<datatable.cols())
    {
        datatable(i,j)=toSafeDouble(model->item(indexA.row(),indexA.column())->text());
    }

    fileModified();
}


QString MainWindow::getColName(int id)
{
    auto item=model->horizontalHeaderItem(id);

    if (item!=nullptr)
    {
        return model->horizontalHeaderItem(id)->text();
    }
    else
    {
        return QString("C%1").arg(id);
    }
}

int MainWindow::getColId(QString colName)
{
    int nbCols=model->columnCount();
    for (int i=0; i<nbCols; i++)
    {
        if (getColName(i)==colName)
        {
            return i;
        }
    }
    return -1;
}

Viewer1D* MainWindow::createViewerId()
{
    Viewer1D* viewer1d=new Viewer1D(&shared,shortcuts,this);
    QObject::connect(viewer1d,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),this,SLOT(slot_newColumn(QString,Eigen::VectorXd)));
    QObject::connect(viewer1d,SIGNAL(sig_displayResults(QString)),this,SLOT(slot_results(QString)));
    viewer1d->setMinimumSize(600,400);
    viewer1d->setAttribute(Qt::WA_DeleteOnClose);

    mdiArea->addSubWindow(viewer1d,Qt::WindowStaysOnTopHint);
    viewer1d->show();

    return viewer1d;
}

void MainWindow::slot_plot_y()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        Viewer1D* viewer1d=createViewerId();

        for (int k=0; k<id_list.size(); k++)
        {
            Eigen::VectorXd data_y=datatable.col(table->horizontalHeader()->visualIndex(id_list[k].column())); //getCol(id_list[k].column(),datatable);

            if (data_y.size()>0)
            {
                viewer1d->slot_add_data(Curve2D(data_y,
                                                getColName(id_list[k  ].column()),
                                                Curve2D::GRAPH));
            }
        }        
    }
    else
    {
        QMessageBox::information(this,"Information","Please select k columns (k>1)");
    }

}

bool MainWindow::asColumnStrings(int idCol)
{
    return isnan(datatable.col(idCol).sum());
}

void MainWindow::slot_plot_graph_xy()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>=2)
    {
        Viewer1D* viewer1d=createViewerId();

        Eigen::VectorXd data_x=datatable.col(table->horizontalHeader()->visualIndex(id_list[0  ].column()));
        for (int k=1; k<id_list.size(); k+=1)
        {
            Eigen::VectorXd data_y=datatable.col(table->horizontalHeader()->visualIndex(id_list[k].column()));

            if (data_x.size()>0 && data_y.size()>0)
            {
                if (!asColumnStrings(table->horizontalHeader()->visualIndex(id_list[k  ].column())))
                {
                    Curve2D curve(data_x,data_y,QString("%2=f(%1)").arg(getColName(id_list[0  ].column())).arg(getColName(id_list[k].column())),Curve2D::GRAPH);
                    viewer1d->slot_add_data(curve);
                }
                else
                {
                    Curve2D curve(data_y,QString("%2=f(%1)").arg(getColName(id_list[0  ].column())).arg(getColName(id_list[k].column())),Curve2D::GRAPH);
                    curve.setLabelsField(getColumn(id_list[k  ].column()));
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
        Viewer1D* viewer1d=createViewerId();

        Eigen::VectorXd data_x=datatable.col(table->horizontalHeader()->visualIndex(id_list[0  ].column()));
        for (int k=1; k<id_list.size(); k+=2)
        {            
            Eigen::VectorXd data_y=datatable.col(table->horizontalHeader()->visualIndex(id_list[k].column()));

            if (data_x.size()>0 && data_y.size()>0)
            {
                viewer1d->slot_add_data(Curve2D(data_x,
                                                data_y,
                                                QString("(%1,%2)").arg(getColName(id_list[0  ].column())).arg(getColName(id_list[k].column())),
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

    if (id_list.size()%3==0 && id_list.size()>0)
    {
        Viewer1D* viewer1d=createViewerId();

        for (int k=0; k<id_list.size(); k+=3)
        {
            Eigen::VectorXd data_x=datatable.col(table->horizontalHeader()->visualIndex(id_list[k  ].column()));
            Eigen::VectorXd data_y=datatable.col(table->horizontalHeader()->visualIndex(id_list[k+1].column()));
            Eigen::VectorXd data_s=datatable.col(table->horizontalHeader()->visualIndex(id_list[k+2].column()));

            if (data_x.size()>0 && data_y.size()>0)
            {
                Curve2D curve(data_x,
                              data_y,
                              QString("(%1,%2)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column())),
                        Curve2D::CURVE);
                curve.setScalarField(data_s);
                curve.getStyle().mLineStyle=QCPCurve::lsNone;
                curve.getStyle().mScatterShape=QCPScatterStyle::ssDisc;
                viewer1d->slot_add_data(curve);
            }
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3 columns P(X,Y) and Scalarfield S in order to plot S(P)");
    }
}

void MainWindow::slot_plot_field_2D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()%4==0 && id_list.size()>0)
    {
        Viewer1D* viewer1d=createViewerId();

        for (int k=0; k<id_list.size(); k+=4)
        {
            Eigen::VectorXd data_x=datatable.col(table->horizontalHeader()->visualIndex(id_list[k  ].column()));
            Eigen::VectorXd data_y=datatable.col(table->horizontalHeader()->visualIndex(id_list[k+1].column()));
            Eigen::VectorXd data_vx=datatable.col(table->horizontalHeader()->visualIndex(id_list[k+2].column()));
            Eigen::VectorXd data_vy=datatable.col(table->horizontalHeader()->visualIndex(id_list[k+3].column()));

            Eigen::VectorXd data_a(data_vx.size());
            Eigen::VectorXd data_s(data_vx.size());

            for (int i=0; i<data_a.size(); i++)
            {
                data_a[i]=atan2(data_vx[i],data_vy[i])-M_PI/2;
                data_s[i]=sqrt(data_vy[i]*data_vy[i]+data_vx[i]*data_vx[i]);
            }

            if (data_x.size()>0 && data_y.size()>0)
            {
                Curve2D curve(data_x,
                              data_y,
                              QString("V(%3,%4)=f(%1,%2)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column())).arg(getColName(id_list[k+2].column())).arg(getColName(id_list[k+3].column())),
                        Curve2D::CURVE);
                curve.setScalarField(data_s);
                curve.setAlphaField(data_a);
                curve.getStyle().mLineStyle=QCPCurve::lsNone;
                curve.getStyle().mScatterShape=QCPScatterStyle::ssArrow;
                curve.getStyle().mScatterSize=20;
                viewer1d->slot_add_data(curve);
            }
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

    if (id_list.size()==3)
    {
        Eigen::VectorXd data_x=datatable.col(table->horizontalHeader()->visualIndex(id_list[0].column()));
        Eigen::VectorXd data_y=datatable.col(table->horizontalHeader()->visualIndex(id_list[1].column()));
        Eigen::VectorXd data_z=datatable.col(table->horizontalHeader()->visualIndex(id_list[2].column()));

        if (data_x.size()>0 && data_y.size()>0 && data_z.size()>0)
        {
            Viewer2D* viewer2d=new Viewer2D();
            viewer2d->setMinimumSize(600,400);

            QMdiSubWindow* subWindow = new QMdiSubWindow;
            subWindow->setWidget(viewer2d);
            subWindow->setAttribute(Qt::WA_DeleteOnClose);
            mdiArea->addSubWindow(subWindow,Qt::WindowStaysOnTopHint);
            viewer2d->show();
            viewer2d->slot_setData(datatable,BoxPlot(512,512,
                                                     static_cast<unsigned int>(id_list[0].column()),
                                   static_cast<unsigned int>(id_list[1].column()),
                    static_cast<unsigned int>(id_list[2].column())));
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3 columns P(X,Y) and S in order to plot S(P)");
    }
}

void MainWindow::slot_plot_fft()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("FFT : Fast Fourier Transform parameters");
        QGridLayout* gbox = new QGridLayout();

        QComboBox* cb_mode=new QComboBox(dialog);
        cb_mode->addItem("RECTANGLE");
        cb_mode->addItem("BLACKMAN");
        cb_mode->addItem("HANN");
        cb_mode->addItem("FLAT_TOP");

        QDoubleSpinBox* sb_fe=new QDoubleSpinBox(dialog);
        sb_fe->setPrefix("Fe=");
        sb_fe->setValue(1.0);
        sb_fe->setRange(0.0,1e100);
        sb_fe->setSuffix(" [Hz]");

        QCheckBox* cb_normalize=new QCheckBox("Normalized");
        cb_normalize->setToolTip("Parseval theorem don't apply if normalized");
        cb_normalize->setChecked(true);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));


        gbox->addWidget(new QLabel("Windows type : "),0,0);
        gbox->addWidget(cb_mode,0,1);
        gbox->addWidget(new QLabel("Sample frequency : "),1,0);
        gbox->addWidget(sb_fe,1,1);
        gbox->addWidget(cb_normalize,2,0,1,2);
        gbox->addWidget(buttonBox,3,0,1,2);

        dialog->setLayout(gbox);

        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            Viewer1D* viewer1d=createViewerId();

            for (int k=0; k<id_list.size(); k++)
            {
                Eigen::VectorXd data_y=datatable.col(table->horizontalHeader()->visualIndex(id_list[k  ].column()));
                Curve2D curve(data_y,QString("%1").arg(getColName(id_list[k  ].column())),Curve2D::GRAPH);

                if (data_y.size()>0)
                {
                    Curve2D fft=curve.getFFT((Curve2D::FFTMode)cb_mode->currentIndex(),sb_fe->value(),cb_normalize->isChecked());
                    viewer1d->slot_add_data(fft);
                    //slot_newColumn(QString("FFT_%1").arg(getColName(id_list[k  ].column())),fft.getY());
                }
            }
        }

    }
    else
    {
        QMessageBox::information(this,"Information","Please select 1 columns");
    }
}

void MainWindow::slot_plot_cloud_3D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()==3 || id_list.size()==4)
    {
        View3D* view3d=new View3D;

        QObject::connect(view3d,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),this,SLOT(slot_newColumn(QString,Eigen::VectorXd)));
        QObject::connect(view3d,SIGNAL(sig_displayResults(QString)),this,SLOT(slot_results(QString)));

        Eigen::VectorXd data_x=datatable.col(table->horizontalHeader()->visualIndex(id_list[0].column()));
        Eigen::VectorXd data_y=datatable.col(table->horizontalHeader()->visualIndex(id_list[1].column()));
        Eigen::VectorXd data_z=datatable.col(table->horizontalHeader()->visualIndex(id_list[2].column()));

        Cloud* cloud=nullptr;

        if (id_list.size()==3)
        {
            cloud=new Cloud(data_x,data_y,data_z,
                            getColName(id_list[0].column()),
                    getColName(id_list[1].column()),
                    getColName(id_list[2].column()));

        }
        else if (id_list.size()==4)
        {
            Eigen::VectorXd data_s=datatable.col(table->horizontalHeader()->visualIndex(id_list[3].column()));
            cloud=new Cloud(data_x,data_y,data_z,data_s,
                            getColName(id_list[0].column()),
                    getColName(id_list[1].column()),
                    getColName(id_list[2].column()),
                    getColName(id_list[3].column()));
        }

        if (cloud)
        {
            view3d->setCloudScalar(cloud,View3D::PrimitiveMode::MODE_POINTS);
            //view3d->setCloudScalar(*cloud);
        }

        mdiArea->addSubWindow(view3d->getContainer(),Qt::WindowStaysOnTopHint);
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

    if (id_list.size()%3==0 && id_list.size()>0)
    {
        ViewerBode * viewer_bode=new ViewerBode(&shared,shortcuts,this);
        viewer_bode->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k+=3)
        {
            Eigen::VectorXd data_f=datatable.col(table->horizontalHeader()->visualIndex(id_list[k].column()));
            Eigen::VectorXd data_module=datatable.col(table->horizontalHeader()->visualIndex(id_list[k+1].column()));
            Eigen::VectorXd data_phase=datatable.col(table->horizontalHeader()->visualIndex(id_list[k+2].column()));

            if (data_f.size()>0 && data_module.size()>0 && data_phase.size()>0)
            {
                Curve2D curve_module(data_f,data_module,QString("%2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column())),Curve2D::GRAPH);
                Curve2D curve_phase (data_f,data_phase,QString("%2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+2].column())),Curve2D::GRAPH);

                viewer_bode->slot_add_data(Curve2DModulePhase(curve_module,curve_phase));
            }
        }

        mdiArea->addSubWindow(viewer_bode,Qt::WindowStaysOnTopHint);
        viewer_bode->show();
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
        Viewer1D* viewer1d=createViewerId();

        for (int k=0; k<id_list.size(); k++)
        {
            Eigen::VectorXd data_y=datatable.col(table->horizontalHeader()->visualIndex(id_list[k].column()));

            if (data_y.size()>0)
            {
                bool ok;
                int nbbins=QInputDialog::getInt(this,"Number of bins","Nb bins=",100,2,10000,1,&ok);
                if (ok)
                {
                    viewer1d->slot_histogram(data_y,QString("Histogram %1").arg(getColName(id_list[k  ].column())),nbbins);
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

QString MainWindow::getSelectionPattern()
{
    QString patterns;
    QModelIndexList selectedCols=table->selectionModel()->selectedColumns();
    QModelIndexList selectedRows=table->selectionModel()->selectedRows();

    for(int i=0;i<selectedCols.size();i++)
    {
        int index=table->horizontalHeader()->visualIndex(selectedCols[i].column());
        patterns+=QString("C%1,").arg(index+1);
    }

    for(int i=0;i<selectedRows.size();i++)
    {
        int index=table->horizontalHeader()->visualIndex(selectedRows[i].row());
        patterns+=QString("R%1,").arg(index+1);
    }

    return patterns;
}

void MainWindow::setSelectionPattern(QString pattern)
{
    table->clearSelection();
    QAbstractItemView::SelectionMode currentSelectionMode=table->selectionMode();
    table->setSelectionMode(QAbstractItemView::MultiSelection);
    QStringList patterns=pattern.split(",",QString::SkipEmptyParts);

    for(int i=0;i<patterns.size();i++)
    {
        if(patterns[i].startsWith('R'))
        {
            patterns[i].remove('R');
            bool ok=false;
            unsigned int index=patterns[i].toUInt(&ok);
            if(ok && index>0)
            {
                table->selectRow(index-1);
            }
        }
        else if(patterns[i].startsWith('C'))
        {
            patterns[i].remove('C');
            bool ok=false;
            unsigned int index=patterns[i].toUInt(&ok);
            if(ok && index>0)
            {
                table->selectColumn(index-1);
            }
        }
    }
    table->setSelectionMode(currentSelectionMode);
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
    le_pattern->setText(getSelectionPattern());
    le_pattern->setToolTip("For example to select row 1 and column 1 type : R1,C1");

    gbox->addWidget(le_pattern,0,0);
    gbox->addWidget(buttonBox,1,0,1,2);

    dialog->setLayout(gbox);
    dialog->setMinimumWidth(300);
    dialog->adjustSize();

    int result=dialog->exec();
    if (result == QDialog::Accepted)
    {
        setSelectionPattern(le_pattern->text());
    }
}

void MainWindow::slot_filter()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        int index=table->horizontalHeader()->visualIndex(id_list[0].column());

        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle(QString("Filter by : %1").arg(getColName(id_list[0].column())));
        QGridLayout* gbox = new QGridLayout();

        QComboBox* cb_mode=new QComboBox(dialog);
        cb_mode->addItem("Ascending sort");
        cb_mode->addItem("Decending sort");
        cb_mode->addItem("Keep greater than threshold");
        cb_mode->addItem("Keep lower than threshold");

        QDoubleSpinBox* sb_threshold=new QDoubleSpinBox(dialog);
        sb_threshold->setPrefix("Threshold=");
        sb_threshold->setRange(-1e100,1e100);

        double defaultThresholdValue=datatable.col(index).mean();
        if(std::isnan(defaultThresholdValue))
        {
            sb_threshold->setValue(0);
        }
        else
        {
            sb_threshold->setValue(defaultThresholdValue);
        }
        //sb_threshold->setEnabled(false);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));


        gbox->addWidget(cb_mode,0,0);
        gbox->addWidget(sb_threshold,0,1);
        gbox->addWidget(buttonBox,2,0,1,2);

        dialog->setLayout(gbox);
        dialog->setMinimumWidth(300);
        dialog->adjustSize();
        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            if(cb_mode->currentIndex()==0)
            {
                sortBy(datatable,index,SortMode::ASCENDING);
            }
            else if(cb_mode->currentIndex()==1)
            {
                sortBy(datatable,index,SortMode::DECENDING);
            }
            else if(cb_mode->currentIndex()==2)
            {
                thresholdBy(datatable,index,ThresholdMode::KEEP_GREATER,sb_threshold->value());
            }
            else if(cb_mode->currentIndex()==3)
            {
                thresholdBy(datatable,index,ThresholdMode::KEEP_LOWER,sb_threshold->value());
            }
            updateTableViewRows();
        }
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
            if (lineraw.endsWith("\n"))
            {
                lineraw.chop(1);
            }
            QStringList line=QString(lineraw).split(separator);

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
            ts<<i.key()<<separator<<i.value().toString()<<"\n";
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

double MainWindow::toSafeDouble(const QString& str)const
{
    bool ok;
    double value=str.toDouble(&ok);
    return ok?value:nan("");
}
Eigen::VectorXd MainWindow::toSafeDouble(QVector<QString> vec_col_str)
{
    Eigen::VectorXd vec_col(vec_col_str.size());

    for (int i=0; i<vec_col.size(); i++)
    {
        vec_col[i]=toSafeDouble(vec_col_str[i]);
    }

    return vec_col;
}

void MainWindow::slot_vSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    moveRow(datatable,oldVisualIndex,newVisualIndex);
    fileModified();
}

void MainWindow::slot_hSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    reg.moveVariable(oldVisualIndex,newVisualIndex);
    moveColumn(datatable,oldVisualIndex,newVisualIndex);
    fileModified();

    reg.dispVariables();
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

        QGradientComboBox * cb_gradients=new QGradientComboBox(this);

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
                    int logicalindex=id_list[i].column();
                    int index=table->horizontalHeader()->visualIndex(logicalindex);

                    if(i==0)
                    {
                        range=QCPRange (datatable.col(index).minCoeff(),datatable.col(index).maxCoeff());
                    }
                    else
                    {
                        range=QCPRange (std::min(range.lower,datatable.col(index).minCoeff()),
                                        std::max(range.upper,datatable.col(index).maxCoeff()));
                    }
                }
            }
            std::cout<<range.lower<<" "<<range.upper<<std::endl;

            //Process
            for(int i=0;i<id_list.size();i++)
            {
                int logicalindex=id_list[i].column();
                int index=table->horizontalHeader()->visualIndex(logicalindex);

                if(cb_percolumns->isChecked())
                {
                    range=QCPRange (datatable.col(index).minCoeff(),datatable.col(index).maxCoeff());
                }

                if(!pb_clear->isChecked())
                {
                    std::vector<QRgb> colors=cb_gradients->colourize(datatable.col(index),range);
                    for(int j=0;j<datatable.rows();j++)
                    {
                        if(qGray(colors[j])>100)
                        {
                            model->item(j,logicalindex)->setForeground(QColor(Qt::black));
                        }
                        else
                        {
                            std::cout<<qGray(colors[j])<<std::endl;
                            model->item(j,logicalindex)->setForeground(QColor(Qt::white));
                        }

                        model->item(j,logicalindex)->setBackground(QColor(colors[j]));
                    }
                }
                else
                {
                    for(int j=0;j<datatable.rows();j++)
                    {
                        model->item(j,logicalindex)->setForeground(QColor(Qt::black));
                        model->item(j,logicalindex)->setBackground(QColor(Qt::white));
                    }
                }
            }
        }
    }
}

void MainWindow::slot_copy()
{
    QString clipboardString;
    QModelIndexList selectedIndexes = table->selectionModel()->selectedIndexes();
    QCPRange range_row,range_col;
    getRowColSelectedRanges(range_row,range_col);

    int nrows=range_row.upper-range_row.lower+1;
    int ncols=range_col.upper-range_col.lower+1;

    Eigen::Matrix<QString,Eigen::Dynamic,Eigen::Dynamic> content(nrows,ncols);

    for (int i = 0; i < selectedIndexes.count(); ++i)
    {
        QModelIndex current = selectedIndexes[i];
        int visualIndexRows=table->verticalHeader()->visualIndex(current.row())-range_row.lower;
        int visualIndexCols=table->horizontalHeader()->visualIndex(current.column())-range_col.lower;

        std::cout<<visualIndexRows<<" "<<visualIndexCols<<std::endl;

        if(current.data(Qt::DisplayRole).isValid())
        {
            content(visualIndexRows,visualIndexCols)=current.data(Qt::DisplayRole).toString();
        }
    }

    for(int i=0;i<nrows;i++)
    {
        for(int j=0;j<ncols;j++)
        {
            clipboardString+=content(i,j);
            clipboardString+=separator;
        }
        clipboardString+="\n";
    }

    QApplication::clipboard()->setText(clipboardString);
}

void MainWindow::slot_paste()
{
    QString clipboardString=QApplication::clipboard()->text();

    if(!clipboardString.isEmpty())
    {
        QCPRange range_row,range_col;
        getRowColSelectedRanges(range_row,range_col);

        //---------------------------------------------------------------------------------
        QStringList lines=clipboardString.split("\n",QString::SkipEmptyParts);
        for(int i=0.0;i<lines.size();i++)
        {
            QStringList valuesToken=extractToken(lines[i]);
            for(int j=0;j<valuesToken.size();j++)
            {
                int indexRow=i+range_row.lower;
                int indexCol=j+range_col.lower;

                int logicalIndexRow=table->verticalHeader()->logicalIndex(indexRow);
                int logicalIndexCol=table->horizontalHeader()->logicalIndex(indexCol);

                if(indexRow<datatable.rows() && indexCol<datatable.cols())
                {
                    model->item(logicalIndexRow,logicalIndexCol)->setText(valuesToken[j]);
                    datatable(indexRow,indexCol)=toSafeDouble(valuesToken[j]);
                }
            }
        }
    }
}

void MainWindow::getRowColSelectedRanges(QCPRange &range_row,QCPRange &range_col)
{
    QModelIndexList selectedIndexes = table->selectionModel()->selectedIndexes();
    for (int i = 0; i < selectedIndexes.count(); ++i)
    {
        QModelIndex current = selectedIndexes[i];
        int visualIndexRows=table->verticalHeader()->visualIndex(current.row());
        int visualIndexCols=table->horizontalHeader()->visualIndex(current.column());

        if(i==0)
        {
            range_row.lower=visualIndexRows;
            range_row.upper=visualIndexRows;
            range_col.lower=visualIndexCols;
            range_col.upper=visualIndexCols;
        }
        else
        {
            if(visualIndexRows<range_row.lower)range_row.lower=visualIndexRows;
            else if(visualIndexRows>range_row.upper)range_row.upper=visualIndexRows;
            if(visualIndexCols<range_col.lower)range_col.lower=visualIndexCols;
            else if(visualIndexCols>range_col.upper)range_col.upper=visualIndexCols;
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
