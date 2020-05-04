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
    connect(ui->actionPlot_GraphY, &QAction::triggered,this,&MainWindow::slot_plot_y);
    connect(ui->actionPlot_GraphXY,&QAction::triggered,this,&MainWindow::slot_plot_graph_xy);
    connect(ui->actionPlot_CurveXY,&QAction::triggered,this,&MainWindow::slot_plot_curve_xy);
    connect(ui->actionPlot_MapXYZ, &QAction::triggered,this,&MainWindow::slot_plot_xyz);
    connect(ui->actionHistogram, &QAction::triggered,this,&MainWindow::slot_plot_histogram);
    connect(ui->actionPlot_Cloud_XYZ,&QAction::triggered,this,&MainWindow::slot_plot_cloud_xys);
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
    a_updateColumns->setShortcut(QKeySequence("Space"));

    a_newColumn=new QAction(this);
    a_newColumn->setShortcut(QKeySequence("Ins"));

    a_newRow=new QAction(this);
    a_newRow->setShortcut(QKeySequence("PgDown"));

    a_delete=new QAction(this);
    a_delete->setShortcut(QKeySequence("Del"));

    this->addAction(a_newColumn);
    this->addAction(a_newRow);
    this->addAction(a_delete);
    this->addAction(a_updateColumns);

    connect(a_newColumn,&QAction::triggered,this,&MainWindow::slot_editColumn);
    connect(a_newRow,&QAction::triggered,this,&MainWindow::slot_newRow);
    connect(a_delete,&QAction::triggered,this,&MainWindow::slot_delete);
    connect(a_updateColumns,&QAction::triggered,this,&MainWindow::slot_updateColumns);
    //------------------------------------------------------------------------------

    mdiArea->setViewport(table);

    //mdiArea->viewport()->stackUnder(table);

    mdiArea->setSizeAdjustPolicy (QAbstractScrollArea::AdjustToContents);
    table->setSizeAdjustPolicy   (QAbstractScrollArea::AdjustToContents);


    direct_new(10,10);


    QActionGroup* actionGroup=new QActionGroup(this);
    actionGroup->addAction(ui->actionPoints);
    actionGroup->addAction(ui->actionLines);
    ui->actionPoints->setChecked(true);
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
    QString filename=QFileDialog::getOpenFileName(this,"Open data CSV",current_filename,tr("Data file (*.csv *.graphy)"));

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
                addRow(dataToken);
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
    int nbCols=model->columnCount();
    std::cout<<"slot_newRow"<<std::endl;
    QList<QStandardItem*> items;
    items.reserve(nbCols);
    for (int i=0; i<nbCols; i++)
    {
        items.append(new QStandardItem);
    }
    model->appendRow(items);

    std::cout<<datatable.size()<<" "<<datatable[0].size()<<std::endl;
    addRowTable(QVector<double>(items.size(),0));

    std::cout<<datatable.size()<<" "<<datatable[0].size()<<std::endl;

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
            delColTable(index);
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
            delRowTable(index);
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
                for (int j=0; j<datatable.size(); j++)
                {
                    *(variables.begin()+j)=datatable[j][i];
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
                for (int j=0; j<datatable.size(); j++)
                {
                    *(variables.begin()+j)=datatable[j][i];
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

        if (args[0]=="search" && args.size()>=5)
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
}

QVector<double> toDouble(QVector<QString> vec_col_str)
{
    QVector<double> vec_col(vec_col_str.size());

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
            datatable[idCol]=toDouble(vec_col);
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
            datatable.push_back(QVector<double>(nbRows,0.0));
            model->appendColumn(items);
        }
        else
        {
            for (int i=0; i<nbRows; i++)
            {
                items.append(new QStandardItem(vec_col[i]));
            }
            datatable.push_back(toDouble(vec_col));
            model->appendColumn(items);
        }
    }


    std::cout<<"setColumn end "<<std::endl;
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

void MainWindow::addRow(const QVector<double>& vec_row)
{
    QList<QStandardItem*> items;
    items.reserve(vec_row.size());
    for (int i=0; i<vec_row.size(); i++)
    {
        items.append(new QStandardItem(QString::number(vec_row[i])));
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

void MainWindow::delColTable(int id)
{
    datatable.removeAt(id);
}

void MainWindow::delRowTable(int id)
{
    for (int i=0; i<datatable.size(); i++)
    {
        datatable[i].removeAt(id);
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
            view3d->setCloudScalar(cloud,View3D::PrimitiveMode::MODE_POINTS);
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
