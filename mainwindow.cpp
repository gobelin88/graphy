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

    connect(ui->actionNew, &QAction::triggered,this,&MainWindow::slot_new);
    connect(ui->actionOpen, &QAction::triggered,this,&MainWindow::slot_open);
    connect(ui->actionSave, &QAction::triggered,this,&MainWindow::slot_save);
    connect(ui->actionExport, &QAction::triggered,this,&MainWindow::slot_export);
    connect(ui->actionParameters, &QAction::triggered,this,&MainWindow::slot_parameters);

    connect(ui->actionPlot_GraphY, &QAction::triggered,this,&MainWindow::slot_plot_y);
    connect(ui->actionPlot_GraphXY,&QAction::triggered,this,&MainWindow::slot_plot_graph_xy);
    connect(ui->actionPlot_CurveXY,&QAction::triggered,this,&MainWindow::slot_plot_curve_xy);
    connect(ui->actionPlot_MapXYZ, &QAction::triggered,this,&MainWindow::slot_plot_map_2D);
    connect(ui->actionHistogram, &QAction::triggered,this,&MainWindow::slot_plot_histogram);
    connect(ui->actionPlot_Cloud_XYZ,&QAction::triggered,this,&MainWindow::slot_plot_cloud_2D);
    connect(ui->actionPlot_Cloud_3D, &QAction::triggered,this,&MainWindow::slot_plot_cloud_3D);
    connect(ui->actionFFT, &QAction::triggered,this,&MainWindow::slot_plot_fft);
    connect(ui->actionPlot_Gain_Phase, &QAction::triggered,this,&MainWindow::slot_plot_gain_phase);

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
    a_delete=new QAction(this);

    this->addAction(a_newColumn);
    this->addAction(a_newRow);
    this->addAction(a_delete);
    this->addAction(a_updateColumns);

    connect(a_newColumn,&QAction::triggered,this,&MainWindow::slot_editColumn);
    connect(a_newRow,&QAction::triggered,this,&MainWindow::slot_newRow);
    connect(a_delete,&QAction::triggered,this,&MainWindow::slot_delete);
    connect(a_updateColumns,&QAction::triggered,this,&MainWindow::slot_updateColumns);
    //------------------------------------------------------------------------------

    te_results=new QTextEdit;
    QTabWidget* te_widget=new QTabWidget();
    te_widget->addTab(table,"Data");
    te_widget->addTab(te_results,"Results");
    mdiArea->setViewport(te_widget);

    //mdiArea->viewport()->stackUnder(table);

    mdiArea->setSizeAdjustPolicy (QAbstractScrollArea::AdjustToContents);
    table->setSizeAdjustPolicy   (QAbstractScrollArea::AdjustToContents);


    direct_new(10,10);


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
        direct_new(sb_sx->value(),sb_sy->value());
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

QStringList extractToken(QString fileLine)
{
    if (fileLine.endsWith("\n"))
    {
        fileLine.chop(1);
    }
    if (fileLine.endsWith(";"))
    {
        fileLine.chop(1);
    }

    return fileLine.split(";");
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
            QString dataLine = in.readLine();

            if (dataLine=="<header>")
            {
                hasheader=true;
                QString varLine = in.readLine();
                QString expLine = in.readLine();
                QStringList varToken = extractToken(varLine);
                QStringList expToken = extractToken(expLine);

                for (int j = 0; j < varToken.size(); j++)
                {
                    QString varname = varToken.at(j);
                    QString varexpr = expToken.at(j);
                    model->setHorizontalHeaderItem(j, new QStandardItem(varname));
                    registerNewVariable(varname,varexpr);
                }

                do
                {
                    dataLine = in.readLine();
                }
                while (dataLine!="</header>");
            }
            else
            {
                QStringList dataToken = extractToken(dataLine);
                addModelRow(dataToken);
                lineindex++;
            }
        }

        if (!hasheader)//add default header
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
        connect(model,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(updateTable(const QModelIndex&,const QModelIndex&)));
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
        setCurrentFilename(filename);

        QString textData;
        int rows = table->model()->rowCount();
        int columns = table->model()->columnCount();

        if (hasheader)
        {
            textData += "<header>\n";
            for (int j = 0; j < columns; j++)
            {
                textData+=variables_names[j];
                textData += ";";
            }
            textData += "\n";
            for (int j = 0; j < columns; j++)
            {
                textData+=variables_expressions[j];
                textData += ";";
            }
            textData += "\n";
            textData += "</header>\n";
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
    else if (variableExpression.startsWith("$"))
    {
        QString result;
        return custom_exp_parse(variableExpression,0,result);
    }
    else
    {
        exprtk::parser<double> parser;
        exprtk::expression<double> expression;
        expression.register_symbol_table(symbolsTable);
        bool ok=parser.compile(variableExpression.toStdString(),expression);

        if (!ok)
        {
            QMessageBox::information(this,"Error",QString("Invalid formula : ")+variableExpression);
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
        QMessageBox::information(this,"Error",QString("Variables names can't start with a number"));
        return false;
    }

    if (variableName.contains(" "))
    {
        QMessageBox::information(this,"Error",QString("Variables names can't have any space"));
        return false;
    }

    QStringList remainder=variables_names;
    if (currentIndex>=0)
    {
        remainder.removeAt(currentIndex);
    }

    if (remainder.contains(variableName))
    {
        QMessageBox::information(this,"Error",QString("This variable name is already used"));
        return false;
    }

    return true;
}

bool MainWindow::editVariableAndExpression(int currentIndex)
{
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
    dialog->setWindowTitle((currentName.isEmpty())?"Add variable":"Edit variable");

    dialog->setMinimumWidth(400);
    QGridLayout* gbox = new QGridLayout();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    gbox->addWidget(le_variableName,0,1);
    gbox->addWidget(le_variableExpression,1,1);
    gbox->addWidget(new QLabel("Variable name"),0,0);
    gbox->addWidget(new QLabel("Variable formula"),1,0);
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
    variables.push_back(0.0);
    variables_names.push_back(varname);
    variables_expressions.push_back(varexpr);
    symbolsTable.add_variable(varname.toStdString(),variables.last());
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
    int nbCols=model->columnCount();
    std::cout<<"slot_newRow"<<std::endl;
    QList<QStandardItem*> items;
    items.reserve(nbCols);
    for (int i=0; i<nbCols; i++)
    {
        items.append(new QStandardItem);
    }
    model->appendRow(items);


    addRow(datatable,Eigen::VectorXd::Zero(items.size()));


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
        std::cout<<"editColumn 2 "<<std::endl;
        setColumn(currentColIndex,evalColumn(currentColIndex));

        std::cout<<"editColumn 3"<<std::endl;
        model->setHorizontalHeaderItem(currentColIndex, new QStandardItem(variables_names[currentColIndex]));
        table->setModel(model);

        hasheader=true;
    }
}

void MainWindow::slot_updateColumns()
{
    int nbCols=model->columnCount();

    for (int i=0; i<nbCols; i++)
    {
        setColumn(i,evalColumn(i));
    }
}

void MainWindow::slot_newColumn(QString name,Eigen::VectorXd data)
{
    int index=datatable.cols();
    registerNewVariable(name,"");
    setColumn(index,toQVectorStr(data));
    model->setHorizontalHeaderItem(index, new QStandardItem(name));
    table->setModel(model);
}

void MainWindow::slot_results(QString results)
{
    te_results->append("------------------------");
    te_results->append(results);
}

void MainWindow::slot_delete()
{
    QModelIndexList id_list_cols=table->selectionModel()->selectedColumns();
    if (id_list_cols.size()>=1)
    {
        for (int i=0; i<id_list_cols.size(); i++)
        {
            int index=id_list_cols[i].column()-i;
            registerDelVariable(variables_names[index]);
            model->removeColumn(index);
            removeColumn(datatable,index);
        }
        table->setModel(model);
    }
    QModelIndexList id_list_rows=table->selectionModel()->selectedRows();
    if (id_list_rows.size()>=1)
    {
        for (int i=0; i<id_list_rows.size(); i++)
        {
            int index=id_list_rows[i].row()-i;
            model->removeRow(index);
            removeRow(datatable,index);
        }
        table->setModel(model);
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

QVector<QString> MainWindow::evalColumn(int colId)
{
    assert(colId<variables_expressions.size());

    if (variables_expressions[colId].isEmpty())
    {
        return QVector<QString>();
    }
    else
    {
        int nbRows=model->rowCount();
        std::cout<<"evalColumn 1 "<<std::endl;

        QVector<QString> colResults(nbRows);

        exprtk::parser<double> parser;
        exprtk::expression<double> compiled_expression;
        compiled_expression.register_symbol_table(symbolsTable);
        if (parser.compile(variables_expressions[colId].toStdString(),compiled_expression))
        {
            for (int i=0; i<nbRows; i++)
            {
                activeRow=i;
                //std::cout<<float(i)/nbRows<<std::endl;
                for (int j=0; j<datatable.cols(); j++)
                {
                    *(variables.begin()+j)=datatable(i,j);
                }
                colResults[i]=QString::number(compiled_expression.value());
            }
        }
        else
        {
            for (int i=0; i<nbRows; i++)
            {
                activeRow=i;
                //std::cout<<float(i)/nbRows<<std::endl;
                for (int j=0; j<datatable.cols(); j++)
                {
                    *(variables.begin()+j)=datatable(i,j);
                }
                custom_exp_parse(variables_expressions[colId],i,colResults[i]);
            }
        }

        std::cout<<"evalColumn 3"<<std::endl;
        return colResults;
    }
}

void scanDir(QDir dir,QStringList filters,QString name,QString& result,int depth)
{
    if (depth==0)
    {
        return;
    }

    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = dir.entryList();
    for ( int i=0; i<fileList.size(); i++)
    {
        if ( fileList[i] .contains(name) )
        {
            result=fileList[i];
            return;
        }
    }

    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList dirList = dir.entryList();
    for (int i=0; i<dirList.size(); ++i)
    {
        QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
        scanDir(QDir(newPath),filters,name,result,depth-1);
    }
}

bool MainWindow::custom_exp_parse(QString expression,int currentRow,QString& result)
{
    expression.remove('$');
    QStringList args=expression.split(" ");
    if (args.size()>0)
    {

        if (args[0]=="rand" && args.size()==1)
        {
            result=QString::number(2.0*(static_cast<double>(rand())/RAND_MAX-0.5));
            return true;
        }
        else if (args[0]=="search" && args.size()>=5)
        {
            QString search_for_name;
            int index1=variables_names.indexOf(args[1]);
            if (index1>=0)
            {
                search_for_name=model->item(currentRow,index1)->text();
            }
            else
            {
                return false;
            }

            result="none";
            for (int i=4; i<args.size(); i++)
            {
                QStringList filters;
                filters<<args[2];
                scanDir(args[i],filters,search_for_name,result,args[4].toInt());
            }

            return true;
        }
        else if (args[0]=="str" && args.size()==3)
        {
            QStringList sub_args1=args[2].split(',');
            int index1=variables_names.indexOf(sub_args1[0]);

            if (index1>=0)
            {
                result=args[1];
                if (sub_args1.size()==2 )
                {
                    if (sub_args1[1]=="#" )
                    {
                        result=result.arg(model->item(currentRow,index1)->text());
                    }
                    else if (sub_args1[1]=="#")
                    {
                        result=result.arg(model->item(currentRow,index1)->text());
                    }
                    else
                    {
                        result=result.arg(*(variables.begin()+index1),sub_args1[1].toInt(),'g',-1,'0');
                    }
                }
                else if (sub_args1.size()==1)
                {
                    result=result.arg(*(variables.begin()+index1));
                }

                return true;
            }
        }
        else if (args[0]=="str" && args.size()==4)
        {
            QStringList sub_args1=args[2].split(',');
            QStringList sub_args2=args[3].split(',');
            int index1=variables_names.indexOf(sub_args1[0]);
            int index2=variables_names.indexOf(sub_args2[0]);

            if (index1>=0 && index2>=0)
            {
                result=args[1];
                if (sub_args1.size()==2 && sub_args2.size()==2)
                {
                    if (sub_args1[1]=="#" && sub_args2[1]=="#")
                    {
                        result=result.arg(model->item(currentRow,index1)->text()).arg(model->item(currentRow,index2)->text());
                    }
                    else if (sub_args1[1]=="#")
                    {
                        result=result.arg(model->item(currentRow,index1)->text()).arg(*(variables.begin()+index2),sub_args2[1].toInt(),'g',-1,'0');
                    }
                    else if (sub_args2[1]=="#")
                    {
                        result=result.arg(*(variables.begin()+index1),sub_args1[1].toInt(),'g',-1,'0').arg(model->item(currentRow,index2)->text());
                    }
                    else
                    {
                        result=result.arg(*(variables.begin()+index1),sub_args1[1].toInt(),'g',-1,'0').arg(*(variables.begin()+index2),sub_args2[1].toInt(),'g',-1,'0');
                    }
                }
                else if (sub_args1.size()==2 && sub_args2.size()==1)
                {
                    if (sub_args1[1]=="#")
                    {
                        result=result.arg(*(variables.begin()+index1),sub_args1[1].toInt(),'g',-1,'0').arg(*(variables.begin()+index2));
                    }
                    else
                    {
                        result=result.arg(model->item(currentRow,index1)->text()).arg(*(variables.begin()+index2));
                    }
                }
                else if (sub_args1.size()==1 && sub_args2.size()==2)
                {
                    if (sub_args2[1]=="#")
                    {
                        result=result.arg(*(variables.begin()+index1)).arg(model->item(currentRow,index2)->text());
                    }
                    else
                    {
                        result=result.arg(*(variables.begin()+index1)).arg(*(variables.begin()+index2),sub_args2[1].toInt(),'g',-1,'0');
                    }
                }
                else if (sub_args1.size()==1 && sub_args2.size()==1)
                {
                    result=result.arg(*(variables.begin()+index1)).arg(*(variables.begin()+index2));
                }

                return true;
            }
            else
            {
                std::cout<<"Bad variable indexes :"<<index1<<" "<<index2<<std::endl;
                return false;
            }
        }
        else
        {
            std::cout<<"Bad custom expression "<<expression.toLocal8Bit().data()<<std::endl;
            return false;
        }
    }
    else
    {
        std::cout<<"Bad custom expression "<<expression.toLocal8Bit().data()<<std::endl;
        return false;
    }

    return false;
}

Eigen::VectorXd toDouble(QVector<QString> vec_col_str)
{
    Eigen::VectorXd vec_col(vec_col_str.size());

    for (int i=0; i<vec_col.size(); i++)
    {
        vec_col[i]=vec_col_str[i].toDouble();
    }

    return vec_col;
}

void MainWindow::setColumn(int idCol,const QVector<QString>& vec_col)
{
    std::cout<<"setColumn 1 id="<<idCol<<std::endl;
    QList<QStandardItem*> items;
    items.reserve(model->rowCount());
    int nbRows=model->rowCount();

    if (idCol<model->columnCount()) //set
    {
        std::cout<<"setColumn 2 "<<std::endl;
        if (vec_col.size()>0)
        {
            for (int i=0; i<nbRows; i++)
            {
                if (model->item(i,idCol))
                {
                    model->item(i,idCol)->setText(vec_col[i]);
                }
            }
            datatable.col(idCol)=toDouble(vec_col);
        }
        std::cout<<"setColumn 2 end"<<std::endl;
    }
    else//new column
    {
        std::cout<<"setColumn 3 "<<std::endl;
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
            addColumn(datatable,toDouble(vec_col));
            model->appendColumn(items);
        }
    }


    std::cout<<"setColumn end "<<std::endl;
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
    this->setWindowTitle(QString("Graphy v2.0 : %1").arg(current_filename));
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
            datatable(i,j)=table->model()->data(table->model()->index(i,j)).toDouble();
        }
    }
}

void MainWindow::updateTable(const QModelIndex& indexA,const QModelIndex& indexB)
{
    Q_UNUSED(indexB)

    int i=indexA.row();
    int j=indexA.column();
    double value=table->model()->data(table->model()->index(i,j)).toDouble();
    std::cout<<"updateTable at ("<<i<<" "<<j<<")="<<value<<std::endl;

    if (i<datatable.rows() && j<datatable.cols())
    {
        datatable(i,j)=value;
    }
    std::cout<<"updateTable end"<<std::endl;
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

void MainWindow::slot_plot_y()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared,shortcuts,this);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k++)
        {
            Eigen::VectorXd data_y=datatable.col(id_list[k].column()); //getCol(id_list[k].column(),datatable);

            if (data_y.size()>0)
            {
                viewer1d->slot_add_data(Curve2D(data_y,
                                                getColName(id_list[k  ].column()),
                                                Curve2D::GRAPH));
            }
        }

        QMdiSubWindow* subWindow = new QMdiSubWindow;
        subWindow->setWidget(viewer1d);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        //subWindow->show();
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
        Viewer1D* viewer1d=new Viewer1D(&shared,shortcuts,this);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k+=2)
        {
            Eigen::VectorXd data_x=datatable.col(id_list[k  ].column());
            Eigen::VectorXd data_y=datatable.col(id_list[k+1].column());

            if (data_x.size()>0 && data_y.size()>0)
            {
                viewer1d->slot_add_data(Curve2D(data_x,
                                                data_y,
                                                QString("%2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column())),
                                                Curve2D::GRAPH));
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
        Viewer1D* viewer1d=new Viewer1D(&shared,shortcuts,this);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k+=2)
        {
            Eigen::VectorXd data_x=datatable.col(id_list[k  ].column());
            Eigen::VectorXd data_y=datatable.col(id_list[k+1].column());

            if (data_x.size()>0 && data_y.size()>0)
            {
                viewer1d->slot_add_data(Curve2D(data_x,
                                                data_y,
                                                QString("(%1,%2)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column())),
                                                Curve2D::CURVE));
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

void MainWindow::slot_plot_cloud_2D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()%3==0 && id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared,shortcuts,this);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k+=3)
        {
            Eigen::VectorXd data_x=datatable.col(id_list[k  ].column());
            Eigen::VectorXd data_y=datatable.col(id_list[k+1].column());
            Eigen::VectorXd data_s=datatable.col(id_list[k+2].column());

            if (data_x.size()>0 && data_y.size()>0)
            {
                Curve2D curve(data_x,
                              data_y,
                              QString("(%1,%2)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column())),
                              Curve2D::CURVE);
                curve.setScalarField(data_s);
                viewer1d->slot_add_data(curve);
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

void MainWindow::slot_plot_map_2D()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()==3)
    {
        Eigen::VectorXd data_x=datatable.col(id_list[0].column());
        Eigen::VectorXd data_y=datatable.col(id_list[1].column());
        Eigen::VectorXd data_z=datatable.col(id_list[2].column());

        if (data_x.size()>0 && data_y.size()>0 && data_z.size()>0)
        {
            Viewer2D* viewer2d=new Viewer2D();
            viewer2d->setMinimumSize(600,400);

            QMdiSubWindow* subWindow = new QMdiSubWindow;
            subWindow->setWidget(viewer2d);
            subWindow->setAttribute(Qt::WA_DeleteOnClose);
            mdiArea->addSubWindow(subWindow);
            viewer2d->show();
            viewer2d->slot_setData(datatable,BoxPlot(512,512,
                                                     static_cast<unsigned int>(id_list[0].column()),
                                                     static_cast<unsigned int>(id_list[1].column()),
                                                     static_cast<unsigned int>(id_list[2].column())));
        }
    }
    else
    {
        QMessageBox::information(this,"Information","Please select 3 columns");
    }
}

void MainWindow::slot_plot_fft()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared,shortcuts,this);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k++)
        {
            Eigen::VectorXd data_y=datatable.col(id_list[k  ].column());
            Curve2D curve(data_y,QString("%1").arg(getColName(id_list[k  ].column())),Curve2D::GRAPH);

            if (data_y.size()>0)
            {
                viewer1d->slot_add_data(curve.getFFT());
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

        QObject::connect(view3d,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),this,SLOT(slot_newColumn(QString,Eigen::VectorXd)));
        QObject::connect(view3d,SIGNAL(sig_displayResults(QString)),this,SLOT(slot_results(QString)));

        Eigen::VectorXd data_x=datatable.col(id_list[0].column());
        Eigen::VectorXd data_y=datatable.col(id_list[1].column());
        Eigen::VectorXd data_z=datatable.col(id_list[2].column());

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
            Eigen::VectorXd data_s=datatable.col(id_list[3].column());
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
        Viewer1DCPLX* viewer1d=new Viewer1DCPLX(shortcuts);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k+=3)
        {
            Eigen::VectorXd data_f=datatable.col(id_list[k].column());
            Eigen::VectorXd data_module=datatable.col(id_list[k+1].column());
            Eigen::VectorXd data_phase=datatable.col(id_list[k+2].column());

            if (data_f.size()>0 && data_module.size()>0 && data_phase.size()>0)
            {
                viewer1d->slot_add_data_graph(
                    Curve2D_GainPhase(data_f,data_module,data_phase,QString("%2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+1].column()))
                                      ,QString("%2=f(%1)").arg(getColName(id_list[k  ].column())).arg(getColName(id_list[k+2].column())))
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
        QMessageBox::information(this,"Information","Please select 3k columns (f,module,phase)");
    }
}

void MainWindow::slot_plot_histogram()
{
    QModelIndexList id_list=table->selectionModel()->selectedColumns();

    if (id_list.size()>0)
    {
        Viewer1D* viewer1d=new Viewer1D(&shared,shortcuts,this);
        viewer1d->setMinimumSize(600,400);

        for (int k=0; k<id_list.size(); k++)
        {
            Eigen::VectorXd data_y=datatable.col(id_list[k].column());

            if (data_y.size()>0)
            {
                bool ok;
                int nbbins=QInputDialog::getInt(this,"Number of bins","Nb bins=",100,2,10000,1,&ok);
                if (ok)
                {
                    viewer1d->slot_histogram(data_y,QString("Histogram %1").arg(getColName(id_list[k  ].column())),nbbins);
                    QMdiSubWindow* subWindow = new QMdiSubWindow;
                    subWindow->setWidget(viewer1d);
                    subWindow->setAttribute(Qt::WA_DeleteOnClose);
                    mdiArea->addSubWindow(subWindow);
                    viewer1d->show();
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
    QMapIterator<QString, QKeySequence> i(shortcuts);
    QStandardItemModel* modelParameters = new QStandardItemModel;
    int line=0;
    while (i.hasNext())
    {
        i.next();
        QStandardItem* itemA=new QStandardItem(i.key());
        QStandardItem* itemB=new QStandardItem(i.value().toString());
        itemA->setEditable(false);
        itemB->setEditable(true);
        modelParameters->setItem(line,0,itemA);
        modelParameters->setItem(line,1,itemB);
        line++;
    }
    tableShortcuts->setModel(modelParameters);

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
        for (int i=0; i<modelParameters->rowCount(); i++)
        {
            std::cout<<modelParameters->item(i,0)->text().toLocal8Bit().data()<<" "<<modelParameters->item(i,1)->text().toLocal8Bit().data()<<std::endl;
            shortcuts.insert(modelParameters->item(i,0)->text(),QKeySequence(modelParameters->item(i,1)->text()));
        }
        applyShortcuts(shortcuts);
        saveShortcuts(shortcuts);
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
            QStringList line=QString(lineraw).split(";");

            if (line.size()>=2)
            {
                shortcuts.insert(line[0],QKeySequence(line[1]));
            }
            else
            {
                QMessageBox::information(this,"Error",QString("Bad line in shortcut configuration file : ")+lineraw);
            }
        }
        file.close();
    }
    else
    {
        QMessageBox::critical(this,"Error",QString("Can't find shorcuts configuration file : ")+shorcuts_cfg);
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
    shortcuts_links.insert(QString("Delete"),a_delete);

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
        QMessageBox::critical(this,"Error",QString("Can't find shorcuts configuration file : ")+shorcuts_cfg);
    }

    applyShortcuts(shortcuts);
}
