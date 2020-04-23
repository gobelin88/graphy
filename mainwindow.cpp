#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "FIR.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mdiArea=new QMdiArea();
    table=new QTableView(mdiArea);

    this->setCentralWidget(mdiArea);

    connect(ui->actionOpen, &QAction::triggered,this,&MainWindow::slot_open);
    connect(ui->actionSave, &QAction::triggered,this,&MainWindow::slot_save);
    connect(ui->actionExport, &QAction::triggered,this,&MainWindow::slot_export);
    connect(ui->actionPlot_GraphY, &QAction::triggered,this,&MainWindow::slot_plot_y);
    connect(ui->actionPlot_GraphXY,&QAction::triggered,this,&MainWindow::slot_plot_graph_xy);
    connect(ui->actionPlot_CurveXY,&QAction::triggered,this,&MainWindow::slot_plot_curve_xy);
    connect(ui->actionPlot_MapXYZ, &QAction::triggered,this,&MainWindow::slot_plot_xyz);
    connect(ui->actionHistogram, &QAction::triggered,this,&MainWindow::slot_plot_histogram);
    connect(ui->actionPlot_Cloud_XYZ,&QAction::triggered,this,&MainWindow::slot_plot_cloud_xys);
    connect(ui->actionPlot_Cloud_3D, &QAction::triggered,this,&MainWindow::slot_plot_cloud_3D);
    connect(ui->actionFFT, &QAction::triggered,this,&MainWindow::slot_plot_fft);

    connect(ui->actionPlot_Gain_Phase, &QAction::triggered,this,&MainWindow::slot_plot_gain_phase);

    connect(ui->actionTile,&QAction::triggered,mdiArea,&QMdiArea::tileSubWindows);
    connect(ui->actionCascade,&QAction::triggered,mdiArea,&QMdiArea::cascadeSubWindows);
    connect(ui->actionNext,&QAction::triggered,mdiArea,&QMdiArea::activateNextSubWindow);
    connect(ui->actionPrevious,&QAction::triggered,mdiArea,&QMdiArea::activatePreviousSubWindow);

    connect(ui->actionPoints,&QAction::triggered,this,&MainWindow::slot_mode_changed);
    connect(ui->actionLines,&QAction::triggered,this,&MainWindow::slot_mode_changed);

//    QMdiSubWindow* w = mdiArea->addSubWindow(table);
//    w->setWindowFlags(Qt::FramelessWindowHint);
//    w->showMaximized();

    //Action Edition----------------------------------------------------------------
    a_newColumn=new QAction(this);
    a_newColumn->setShortcut(QKeySequence("Ins"));

    a_newRow=new QAction(this);
    a_newRow->setShortcut(QKeySequence("PgDown"));

    a_delColumn=new QAction(this);
    a_delColumn->setShortcut(QKeySequence("Del"));

    this->addAction(a_newColumn);
    this->addAction(a_newRow);
    this->addAction(a_delColumn);

    connect(a_newColumn,&QAction::triggered,this,&MainWindow::slot_editColumn);
    connect(a_newRow,&QAction::triggered,this,&MainWindow::slot_newRow);
    connect(a_delColumn,&QAction::triggered,this,&MainWindow::slot_delColumn);
    //------------------------------------------------------------------------------

    mdiArea->setViewport(table);

    //mdiArea->viewport()->stackUnder(table);

    mdiArea->setSizeAdjustPolicy (QAbstractScrollArea::AdjustToContents);
    table->setSizeAdjustPolicy   (QAbstractScrollArea::AdjustToContents);


    direct_new(10,1);

    graphMode=View3D::MODE_POINTS;

    QActionGroup* actionGroup=new QActionGroup(this);
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

    if (!filename.isEmpty())
    {
        direct_open(filename);
    }
}

void MainWindow::direct_open(QString filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        registerClear();

        model = new QStandardItemModel;

        int lineindex = 0;                     // file line counter
        QTextStream in(&file);                 // read to text stream

        hasheader=false;

        while (!in.atEnd())
        {

            // read one line from textstream(separated by "\n")
            QString fileLine = in.readLine();

            // parse the read line into separate pieces(tokens) with "," as the delimiter
            if (fileLine.endsWith("\n"))
            {
                fileLine.chop(1);
            }
            if (fileLine.endsWith(";"))
            {
                fileLine.chop(1);
            }
            QStringList lineToken = fileLine.split(";");

            if (lineToken.size()>0)
            {
                bool isnumber=false;
                lineToken[0].toDouble(&isnumber);
                if (!isnumber)
                {
                    hasheader=true;
                    std::cout<<"Has header:"<<lineToken[0].toLocal8Bit().data()<<std::endl;
                }
            }

            if (hasheader && lineindex==0)
            {
                for (int j = 0; j < lineToken.size(); j++)
                {
                    QString varname = lineToken.at(j);
                    QString varexpr =QString("");
                    model->setHorizontalHeaderItem(j, new QStandardItem(varname));
                    registerNewVariable(varname,varexpr);
                }
            }
            else
            {
                addRow(lineToken);
            }

            lineindex++;
        }

        if (!hasheader)
        {
            for (int j = 0; j < model->columnCount(); j++)
            {
                QString varname = QString("C%1").arg(j);
                QString varexpr =QString("");
                QStandardItem* item = new QStandardItem(varname);
                model->setHorizontalHeaderItem(j, item);
                registerNewVariable(varname,varexpr);
            }
        }

        table->setModel(model);
        connect(model,SIGNAL(dataChanged(const QModelIndex&)),this,SLOT(updateTable(const QModelIndex&)));
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

    if (!filename.isEmpty())
    {
        direct_save(filename);
    }
}

void MainWindow::slot_export()
{
    QFileInfo info(current_filename);
    QString filename=QFileDialog::getSaveFileName(this,"Export data tex",info.path()+"/"+info.baseName()+".tex","*.tex");

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

        textData+= QString("\\begin{tabular}{|*{%1}{c|}} \n").arg(columns+1);
        textData += "\\hline \n";
        if (hasheader)
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
                if (okvi)
                {
                    textData += QString::number(vi);
                }
                else if (okvf)
                {
                    textData += QString::number(vf,'f',prec);
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
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString textData;
        int rows = table->model()->rowCount();
        int columns = table->model()->columnCount();

        if (hasheader)
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

bool MainWindow::isValidExpression(QString variableExpression)
{
    if (variableExpression.isEmpty())
    {
        return true;
    }
    else
    {
        exprtk::parser<double> parser;
        exprtk::expression<double> expression;
        expression.register_symbol_table(symbolsTable);
        bool ok=parser.compile(variableExpression.toStdString(),expression);

        if (!ok)
        {
            QMessageBox::information(this,"Erreur",QString("Erreur dans l'expression : ")+variableExpression);
        }

        return ok;
    }
}

bool MainWindow::isValidVariable(QString variableName,int currentIndex)
{
    if (variableName.isEmpty())
    {
        return false;
    }

    if (variableName.begin()->isDigit())
    {
        QMessageBox::information(this,"Erreur",QString("Un nom valide ne commence pas par un chiffre"));
        return false;
    }

    if (variableName.contains(" "))
    {
        QMessageBox::information(this,"Erreur",QString("Un nom valide ne contient pas d'espace"));
        return false;
    }

    QStringList remainder=variables_names;
    if (currentIndex>=0)
    {
        remainder.removeAt(currentIndex);
    }

    if (remainder.contains(variableName))
    {
        QMessageBox::information(this,"Erreur",QString("Ce nom existe deja"));
        return false;
    }

    return true;
}

bool MainWindow::editVariableAndExpression(int currentIndex)
{
    std::cout<<"editVariableAndExpression 1"<<std::endl;
    QString currentName,currentExpression;
    QString newName,newExpression;

    if (currentIndex<variables.size()) //fetch variable and expression
    {
        currentName=variables_names[currentIndex];
        currentExpression=variables_expressions[currentIndex];
    }
    else
    {
        currentName.clear();
        currentExpression.clear();
    }

    QLineEdit* le_variableName=new QLineEdit(currentName);
    QLineEdit* le_variableExpression=new QLineEdit(currentExpression);

    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle((currentName.isEmpty())?"Modification d'une variable":"Edition d'une variable");

    dialog->setMinimumWidth(400);
    QGridLayout* gbox = new QGridLayout();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    gbox->addWidget(le_variableName,0,1);
    gbox->addWidget(le_variableExpression,1,1);
    gbox->addWidget(new QLabel("Nom de la variable"),0,0);
    gbox->addWidget(new QLabel("Expression"),1,0);
    gbox->addWidget(buttonBox,2,0,1,2);

    dialog->setLayout(gbox);
    int result=dialog->exec();
    if (result == QDialog::Accepted)
    {
        newName=le_variableName->text();
        newExpression=le_variableExpression->text();

        if (isValidVariable(newName,currentIndex) && isValidExpression(newExpression))
        {
            if (currentName.isEmpty()) //add New
            {
                registerNewVariable(newName,newExpression);
            }
            else //modify
            {
                registerRenameVariable(currentName,newName,currentExpression,newExpression);
            }
        }
        else
        {
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

void MainWindow::registerClear()
{
    variables_names.clear();
    variables_expressions.clear();
    variables.clear();
    symbolsTable.clear();
    symbolsTable.add_pi();
    symbolsTable.add_variable("Row",activeRow);
}

void MainWindow::registerNewVariable(QString varname,QString varexpr)
{
    std::cout<<"registerNewVariable 1"<<std::endl;

    variables.push_back(0.0);
    variables_names.push_back(varname);
    variables_expressions.push_back(varexpr);
    symbolsTable.add_variable(varname.toStdString(),variables.last());
    std::cout<<"registerNewVariable 2"<<std::endl;
}

void MainWindow::registerDelVariable(QString varname)
{
    int index=variables_names.indexOf(varname);
    variables_names.removeAt(index);
    variables_expressions.removeAt(index);
    variables.erase(variables.begin()+index);
    symbolsTable.remove_variable(varname.toStdString());
}

void MainWindow::registerRenameVariable(QString old_varname,QString new_varname,QString oldExpression,QString newExpression)
{
    std::cout<<"registerRenameVariable 1"<< old_varname.toLocal8Bit().data() <<" -- "<<new_varname.toLocal8Bit().data() <<std::endl;
    int index=variables_names.indexOf(old_varname);

    if (oldExpression!=newExpression)
    {
        variables_expressions[index]=newExpression;
    }

    if (old_varname!=new_varname)
    {
        variables_names[index]=new_varname;
        symbolsTable.add_variable(new_varname.toStdString(),symbolsTable.variable_ref(old_varname.toStdString()));
        symbolsTable.remove_variable(old_varname.toStdString());
    }

    std::cout<<"registerRenameVariable 2"<<std::endl;
}


void MainWindow::slot_newRow()
{
    std::cout<<"slot_newRow"<<std::endl;
    QList<QStandardItem*> items;
    items.reserve(model->columnCount());
    for (int i=0; i<items.size(); i++)
    {
        items.append(new QStandardItem);
    }
    model->appendRow(items);

    addRowTable(QVector<double>(items.size(),0));

    table->setModel(model);
}

void MainWindow::slot_editColumn()
{
    int currentColIndex=model->columnCount();

    QModelIndexList id_list=table->selectionModel()->selectedColumns();
    if (id_list.size()==1)
    {
        currentColIndex=id_list[0].column();
    }

    if (editVariableAndExpression(currentColIndex))
    {
        std::cout<<"newColumn 1"<<std::endl;
        QVector<double> results=evalColumn(currentColIndex);

        std::cout<<"newColumn 2 "<<results.size()<<std::endl;
        setColumn(currentColIndex,results);

        std::cout<<"newColumn 3"<<std::endl;
        model->setHorizontalHeaderItem(currentColIndex, new QStandardItem(variables_names[currentColIndex]));
        table->setModel(model);
        updateTable();

        hasheader=true;
    }

    dispVariables();
}

void MainWindow::slot_delColumn()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();
    if (id_list.size()>=1)
    {
        for (int i=0; i<id_list.size(); i++)
        {
            registerDelVariable(variables_names[id_list[i].column()-i]);
            model->removeColumn(id_list[i].column()-i);
        }
        table->setModel(model);
        updateTable();
    }
    else
    {
        QMessageBox::information(this,"Message",QString("Selectionner au moins une colonne à supprimer"));
    }
}

void MainWindow::dispVariables()
{
    for (int i=0; i<variables_names.size(); i++)
    {
        std::cout<<variables_names[i].toLocal8Bit().data()<<"="<<variables_expressions[i].toLocal8Bit().data()<<"="<<*(variables.begin()+i)<<" ("<<& *(variables.begin()+i) <<") ";
    }
    std::cout<<std::endl;
}

QVector<double> MainWindow::evalColumn(int colId)
{
    assert(colId<variables_expressions.size());

    if (variables_expressions[colId].isEmpty())
    {
        return QVector<double>();
    }
    else
    {
        int nbRows=model->rowCount();
        std::cout<<"evalColumn 1 "<<std::endl;

        QVector<double> colResults(nbRows);

        exprtk::parser<double> parser;
        exprtk::expression<double> compiled_expression;
        compiled_expression.register_symbol_table(symbolsTable);
        parser.compile(variables_expressions[colId].toStdString(),compiled_expression);

        for (int i=0; i<nbRows; i++)
        {
            activeRow=i;
            //std::cout<<float(i)/nbRows<<std::endl;
            for (int j=0; j<datatable.size(); j++)
            {
                *(variables.begin()+j)=datatable[j][i];
            }
            colResults[i]=compiled_expression.value();
        }


        std::cout<<"evalColumn 3"<<std::endl;
        return colResults;
    }
}

void MainWindow::setColumn(int idCol,const QVector<double>& vec_col)
{
    QList<QStandardItem*> items;
    items.reserve(model->rowCount());
    int nbRows=model->rowCount();

    if (idCol<model->columnCount()) //set
    {
        for (int i=0; i<nbRows; i++)
        {
            if (vec_col.size()>0)
            {
                model->item(i,idCol)->setText(QString::number(vec_col[i]));
            }
        }
    }
    else//new column
    {
        for (int i=0; i<nbRows; i++)
        {
            if (vec_col.size()==0)
            {
                items.append(new QStandardItem());
            }
            else
            {
                items.append(new QStandardItem(QString::number(vec_col[i])));
            }
        }
        model->appendColumn(items);
    }
}

void MainWindow::addRow(const QStringList& str_row)
{
    QList<QStandardItem*> items;
    items.reserve(str_row.size());
    for (int i=0; i<str_row.size(); i++)
    {
        items.append(new QStandardItem(str_row[i]));
    }
    model->appendRow(items);
}



void MainWindow::direct_new(int sx,int sy)
{
    registerClear();

    hasheader=false;
    model = new QStandardItemModel;

    for (int dx=0; dx<sx; dx++)
    {
        for (int dy=0; dy<sy; dy++)
        {
            QStandardItem* item = new QStandardItem();
            model->setItem(dx, dy, item);
        }
    }

    for (int j = 0; j < model->columnCount(); j++)
    {
        QString value = QString("C%1").arg(j);
        registerNewVariable(value,"");
        QStandardItem* item = new QStandardItem(value);
        model->setHorizontalHeaderItem(j, item);
    }

    table->setModel(model);
    connect(model,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(updateTable(const QModelIndex&,const QModelIndex&)));

    setCurrentFilename("new.csv");
    updateTable();
}

void MainWindow::setCurrentFilename(QString filename)
{
    current_filename=filename;
    this->setWindowTitle(QString("Graphy v1.0 : %1").arg(current_filename));
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    table->hide();
    table->show();
}

void MainWindow::addRowTable(QVector<double> dataRow)
{
    for (int i=0; i<datatable.size(); i++)
    {
        datatable[i].push_back(dataRow[i]);
    }
}

void MainWindow::addColTable(QVector<double> dataCol)
{
    datatable.push_back(dataCol);
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

void MainWindow::updateTable(const QModelIndex& indexA,const QModelIndex& indexB)
{
    int i=indexA.row();
    int j=indexA.column();
    double value=table->model()->data(table->model()->index(i,j)).toDouble();
    std::cout<<"updateTable at ("<<i<<" "<<j<<")="<<value<<std::endl;

    if (j<datatable.size())
    {
        if (i<datatable[j].size())
        {
            datatable[j][i]=value;
        }
    }
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
}

QVector<double> MainWindow::getCol(int id,const TableData& tableData)
{
    if (tableData.size()>id)
    {
        return tableData[id];
    }
    else
    {
        std::cout<<id<<" "<<tableData.size()<<std::endl;
        return QVector<double>();
    }
}

double getMin(QVector<double> data)
{
    double min=DBL_MAX;
    foreach (double d,data)
    {
        if (d<min)
        {
            min=d;
        }
    }
    return min;
}
double getMax(QVector<double> data)
{
    double max=DBL_MIN;
    foreach (double d,data)
    {
        if (d>max)
        {
            max=d;
        }
    }
    return max;
}

void MainWindow::slot_plot_y()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k++)
        {
            QVector<double> data_y=getCol(id_list[k].column(),datatable);

            if (data_y.size()>0)
            {
                viewer1d->slot_add_data_graph(Curve2D(data_y,QString("Graph %1").arg(getColName(id_list[k  ].column()))));
            }
        }

        QMdiSubWindow* subWindow = new QMdiSubWindow;
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

    if (id_list.size()%2==0 && id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k+=2)
        {

            QVector<double> data_x=getCol(id_list[k  ].column(),datatable);
            QVector<double> data_y=getCol(id_list[k+1].column(),datatable);

            if (data_x.size()>0 && data_y.size()>0)
            {
                viewer1d->slot_add_data_graph(Curve2D(data_x,data_y,QString("Graph %2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column()))));
            }
        }

        QMdiSubWindow* subWindow = new QMdiSubWindow;
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

    if (id_list.size()%2==0 && id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k+=2)
        {

            QVector<double> data_x=getCol(id_list[k  ].column(),datatable);
            QVector<double> data_y=getCol(id_list[k+1].column(),datatable);

            if (data_x.size()>0 && data_y.size()>0)
            {
                viewer1d->slot_add_data_curve(Curve2D(data_x,data_y,QString("Curve (%2,%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column()))));
            }
        }

        QMdiSubWindow* subWindow = new QMdiSubWindow;
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

    if (id_list.size()%3==0 && id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k+=3)
        {

            QVector<double> data_x=getCol(id_list[k  ].column(),datatable);
            QVector<double> data_y=getCol(id_list[k+1].column(),datatable);
            QVector<double> data_s=getCol(id_list[k+2].column(),datatable);

            if (data_x.size()>0 && data_y.size()>0)
            {
                Curve2D curve(data_x,data_y,QString("Curve (%2,%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column())));
                curve.setScalarField(data_s);
                viewer1d->slot_add_data_cloud(curve);
            }
        }

        QMdiSubWindow* subWindow = new QMdiSubWindow;
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

    if (id_list.size()==3)
    {
        QVector<double> data_x=getCol(id_list[0].column(),datatable);
        QVector<double> data_y=getCol(id_list[1].column(),datatable);
        QVector<double> data_z=getCol(id_list[2].column(),datatable);

        if (data_x.size()>0 && data_y.size()>0 && data_z.size()>0)
        {
            Viewer2D* viewer2d=new Viewer2D();
            viewer2d->setMinimumSize(600,400);

            QMdiSubWindow* subWindow = new QMdiSubWindow;
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
    if (ui->actionPoints->isChecked())
    {
        graphMode=View3D::MODE_POINTS;
    }
    else if (ui->actionLines->isChecked())
    {
        graphMode=View3D::MODE_LINES;
    }
}

void MainWindow::slot_plot_fft()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k++)
        {
            QVector<double> data_y=getCol(id_list[k].column(),datatable);

            std::vector< double > time=std::vector< double >(data_y.size()/2.0, 0);
            for (unsigned int i=0; i<time.size(); i++)
            {
                time[i]=i*1.0/data_y.size();
            }

            FIR win;
            win.getHannCoef(data_y.size());

            for (int i=0; i<data_y.size(); i++)
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
            for (unsigned int k=0; k<fft_data_cplx.size(); k++)
            {
                fft_data_module[k]=20*log10(std::abs(fft_data_cplx[k]));
            }



            if (data_y.size()>0)
            {
                viewer1d->slot_add_data_graph(Curve2D(QVector<double>::fromStdVector(time),QVector<double>::fromStdVector(fft_data_module),QString("FFT %1").arg(getColName(id_list[k  ].column()))));
            }
        }

        QMdiSubWindow* subWindow = new QMdiSubWindow;
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

    if (id_list.size()==3 || id_list.size()==4)
    {
        View3D* view3d=new View3D;
        QVector<double> data_x=getCol(id_list[0].column(),datatable);
        QVector<double> data_y=getCol(id_list[1].column(),datatable);
        QVector<double> data_z=getCol(id_list[2].column(),datatable);

        CloudScalar* cloud=nullptr;

        if (id_list.size()==3)
        {
            cloud=new CloudScalar(data_x,data_y,data_z,
                                  getColName(id_list[0].column()),
                                  getColName(id_list[1].column()),
                                  getColName(id_list[2].column()));

        }
        else if (id_list.size()==4)
        {
            QVector<double> data_s=getCol(id_list[3].column(),datatable);
            cloud=new CloudScalar(data_x,data_y,data_z,data_s,
                                  getColName(id_list[0].column()),
                                  getColName(id_list[1].column()),
                                  getColName(id_list[2].column()),
                                  getColName(id_list[3].column()));
        }

        if (cloud)
        {
            view3d->setCloudScalar(cloud,graphMode);
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


    if (id_list.size()%3==0 && id_list.size()>0)
    {
        Viewer1DCPLX* viewer1d=new Viewer1DCPLX();
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k+=3)
        {

            QVector<double> data_f=getCol(id_list[k  ].column(),datatable);
            QVector<double> data_gain=getCol(id_list[k+1].column(),datatable);
            QVector<double> data_phase=getCol(id_list[k+2].column(),datatable);

            if (data_f.size()>0 && data_gain.size()>0 && data_phase.size()>0)
            {
                viewer1d->slot_add_data_graph(
                    Curve2D_GainPhase(data_f,data_gain,data_phase,QString("Gain %2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column()))
                                      ,QString("Phase %2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+2].column())))
                );
            }
        }

        QMdiSubWindow* subWindow = new QMdiSubWindow;
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

void MainWindow::slot_plot_histogram()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k++)
        {
            QVector<double> data_y=getCol(id_list[k].column(),datatable);

            if (data_y.size()>0)
            {
                viewer1d->slot_histogram(data_y,QString("Curve Histogram %1").arg(getColName(id_list[k  ].column())));
            }
        }

        QMdiSubWindow* subWindow = new QMdiSubWindow;
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
