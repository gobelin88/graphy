#include "Register.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QCompleter>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLocale>
#include <QLabel>
#include <iostream>
#include "MyTextEdit.h"
#include "MyHighLighter.h"

Register::Register()
{    
    clear();
    current_compiled_expression.register_symbol_table(symbolsTable);
}

Register::~Register()
{
    std::cout<<"Delete register3"<<std::endl;
    variables_names.clear();

    std::cout<<"Delete register4"<<std::endl;
    variables_expressions.clear();

    std::cout<<"Delete register5"<<std::endl;
    variables.clear();

    std::cout<<"Delete register6"<<std::endl;
    symbolsTable.clear();
}

void Register::swapVariables(int ida,int idb)
{
    std::swap(variables[ida],variables[idb]);
    variables_names.swap(ida,idb);
    variables_expressions.swap(ida,idb);
}

void Register::moveVariable(int ida,int idb)
{
    if (ida<idb)
    {
        for (int k=ida; k<idb; k++)
        {
            swapVariables(k,k+1);
        }
    }
    else if (ida>idb)
    {
        for (int k=ida; k>idb; k--)
        {
            swapVariables(k,k-1);
        }
    }
}

void Register::dispVariables()
{
    for (int i=0; i<variables_names.size(); i++)
    {
        std::cout<<variables_names[i].toLocal8Bit().data()<<"="<<variables_expressions[i].toLocal8Bit().data()<<"="<<*(variables[i])<<" ("<<(variables[i]) <<") ";
    }
    std::cout<<std::endl;
}

void Register::clear()
{
    variables_names.clear();
    variables_expressions.clear();
    variables.clear();
    symbolsTable.clear();

    symbolsTable.add_constant("i",VariableType(0,1));
    symbolsTable.add_constant("pi",VariableType(M_PI));
    symbolsTable.add_variable("Row",activeRow);
    symbolsTable.add_variable("Col",activeCol);
    symbolsTable.add_function("uniform"  ,  cf_uniform);
    symbolsTable.add_function("normal"  ,  cf_normal);
}

bool Register::newVariable(QString varname,QString varexpr)
{
    if(!isValidExpression(varexpr))
    {
        return false;
    }

    if(!isValidVariable(varname))
    {
        return false;
    }

    VariableType * p_variable=new VariableType(0.0);
    variables.push_back(p_variable);
    symbolsTable.add_variable(varname.toStdString(),*p_variable);

    variables_names.push_back(varname);
    variables_expressions.push_back(varexpr);

    return true;
}

void Register::delVariable(QString varname)
{
    int index=variables_names.indexOf(varname);
    variables_names.removeAt(index);
    variables_expressions.removeAt(index);
    variables.erase(variables.begin()+index);
    symbolsTable.remove_variable(varname.toStdString());
}

bool Register::renameVariable(QString old_varname,QString new_varname,QString oldExpression,QString newExpression)
{
    int index=variables_names.indexOf(old_varname);

    if (oldExpression!=newExpression)
    {
        if(isValidExpression(newExpression))
        {
            variables_expressions[index]=newExpression;
        }
        else
        {
            return false;
        }
    }

    if (old_varname!=new_varname)
    {
        if(isValidVariable(new_varname))
        {
            variables_names[index]=new_varname;
            symbolsTable.add_variable(new_varname.toStdString(),symbolsTable.variable_ref(old_varname.toStdString()));
            symbolsTable.remove_variable(old_varname.toStdString());
        }
        else
        {
            return false;
        }
    }

    return true;
}


bool Register::isValidExpression(QString variableExpression)
{
    if (variableExpression.isEmpty())
    {
        return true;
    }
    else if (variableExpression.startsWith("$"))
    {
        return true;
    }
    else
    {
        exprtk::expression<VariableType> current_expression;
        current_expression.register_symbol_table(symbolsTable);
        bool ok=parser.compile(variableExpression.toStdString(),current_expression);

        if (!ok)
        {
            error("Invalid formula",variableExpression+QString("\nError : ")+QString::fromStdString(parser.error()));
        }

        return ok;
    }
}

int Register::countVariable(QString variableName)
{
    unsigned int cpt=0;
    for(int i=0;i<variables_names.size();i++)
    {
        if(variables_names[i].contains(variableName))
        {
            cpt++;
        }
    }
    return cpt;
}

bool Register::existVariable(QString variableName)
{
    if (symbolsTable.symbol_exists(variableName.toStdString()))
    {
        return true;
    }

    return false;
}

bool Register::isValidVariable(QString variableName)
{
    if (variableName.isEmpty())
    {
        return false;
    }

    if (variableName.begin()->isDigit())
    {
        error("Variable",QString("%1 : Variables names can't start with a number").arg(variableName));
        return false;
    }

    if (variableName.contains(" "))
    {
        error("Variable",QString("%1 : Variables names can't have any space").arg(variableName));
        return false;
    }

    if (!symbolsTable.valid_symbol(variableName.toStdString()))
    {
        error("Variable",QString("%1 : Invalid variable name.\nVariables names can't have any of these characters :\n+ - / * ^ > < | & ...etc").arg(variableName));
        return false;
    }

    if (symbolsTable.symbol_exists(variableName.toStdString()))
    {
        error("Variable",QString("%1 : This variable name is already used").arg(variableName));
        return false;
    }

    return true;
}

void Register::error(QString title,QString msg)
{
    QMessageBox::information(nullptr,QString("Error : ")+title,msg);
    std::cout<<"Error : "<<msg.toLocal8Bit().data()<<std::endl;
}

const QStringList & Register::variablesNames() const
{
    return variables_names;
}

const QStringList & Register::variablesExpressions() const
{
    return variables_expressions;
}

QString Register::getSaveVariablesExpression(int i) const
{
    QString exp=variables_expressions[i];
    exp.replace('\n','#');
    exp.replace(';','@');
    return exp;
}

QString Register::getLoadVariableExpression(QString exp)
{
    exp.replace('#','\n');
    exp.replace('@',';');
    return exp;
}

int Register::size()
{
    return variables.size();
}

exprtk::symbol_table<Register::VariableType> &Register::symbols()
{
    return symbolsTable;
}

void Register::setVariable(int i,Register::VariableType value)
{
    if(i<variables.size())
    {
        *(variables[i])=value;
    }
}

bool Register::compileExpression(int id)
{

//Compilation options
//    static const std::size_t compile_options =
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_replacer         +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_joiner            +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_numeric_check     +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_bracket_check     +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_sequence_check    +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_commutative_check +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_strength_reduction;
//    parser.settings().load_compile_options(compile_options);
//    std::cout<<"replacer_enabled="<<parser.settings().replacer_enabled()<<std::endl;
//    std::cout<<"commutative_check_enabled="<<parser.settings().commutative_check_enabled()<<std::endl;
//    std::cout<<"joiner_enabled="<<parser.settings().joiner_enabled()<<std::endl;
//    std::cout<<"numeric_check_enabled="<<parser.settings().numeric_check_enabled()<<std::endl;
//    std::cout<<"bracket_check_enabled="<<parser.settings().bracket_check_enabled()<<std::endl;
//    std::cout<<"sequence_check_enabled="<<parser.settings().sequence_check_enabled()<<std::endl;
//    std::cout<<"collect_variables_enabled="<<parser.settings().collect_variables_enabled()<<std::endl;
//    std::cout<<"collect_functions_enabled="<<parser.settings().collect_functions_enabled()<<std::endl;
//    std::cout<<"collect_assignments_enabled="<<parser.settings().collect_assignments_enabled()<<std::endl;




    return parser.compile(variables_expressions[id].toStdString(),current_compiled_expression);
}

Register * Register::copy()
{
    Register * copy_reg=new Register;

    for(int i=0;i<variables.size();i++)
    {
        VariableType * p_variable =new VariableType;
        copy_reg->variables.push_back(p_variable);
        copy_reg->symbolsTable.add_variable(variables_names[i].toStdString(),*p_variable);
    }
    copy_reg->variables_names=variables_names;
    copy_reg->variables_expressions=variables_expressions;

    return copy_reg;
}

void Register::currentCompiledExpressionValue(MyVariant & variant)const
{
    variant=current_compiled_expression.value();
}

QStringList Register::getVariablesList()
{
    QStringList qlist;
    std::vector<std::string> list;
    symbolsTable.get_variable_list(list);
    for(int i=0;i<list.size();i++)
    {
        qlist.append(QString::fromStdString(list[i]));
    }

    return qlist;
}

QStringList Register::getFunctionsList()
{
    QStringList qlist;
    std::vector<std::string> list;
    symbolsTable.get_function_list(list);
    for(int i=0;i<list.size();i++)
    {
        qlist.append(QString::fromStdString(list[i]));
    }

    return qlist;
}

QStringList Register::getReservedList()
{
    QStringList qlist;

    for(int i=0;i<exprtk::details::reserved_symbols_size;i++)
    {
        qlist.append(QString::fromStdString(exprtk::details::reserved_symbols[i]));
    }

    return qlist;
}

void custom_scanDir(QDir dir,QStringList filters,QString name,QString& result,int depth)
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
        custom_scanDir(QDir(newPath),filters,name,result,depth-1);
    }
}

//bool Register::customExpressionParse(unsigned int id,QString& result,std::function<QString(int,int)> at,int currentRow)
//{
//    QString expression=variables_expressions[id];
//    expression.remove('$');
//    QStringList args=expression.split(" ");
//    if (args.size()>0)
//    {

//        if (args[0]=="uniform" && args.size()==1)
//        {
//            result=fromNumber((*noise_uniform)(generator));
//            return true;
//        }
//        else if (args[0]=="normal" && args.size()==1)
//        {
//            result=fromNumber((*noise_normal)(generator));
//            return true;
//        }
//        else if (args[0]=="search" && args.size()>=5)
//        {
//            //0      1    2      3  4     5      6      i
//            //search what filter depth where0 where1 where2 wherei...

//            QString search_for_name;
//            int index1=variables_names.indexOf(args[1]);
//            if (index1>=0)
//            {
//                search_for_name=at(currentRow,index1);
//            }
//            else
//            {
//                return false;
//            }

//            result="none";
//            for (int i=4; i<args.size(); i++)
//            {
//                QStringList filters;
//                filters<<args[2];
//                custom_scanDir(args[i],filters,search_for_name,result,args[4].toInt());
//            }

//            return true;
//        }
//        else if (args[0]=="str" && args.size()==3)
//        {
//            QStringList sub_args1=args[2].split(',');
//            int index1=variables_names.indexOf(sub_args1[0]);

//            if (index1>=0)
//            {
//                result=args[1];
//                if (sub_args1.size()==2 )
//                {
//                    if (sub_args1[1]=="#" )
//                    {
//                        result=result.arg(at(currentRow,index1));
//                    }
//                    else if (sub_args1[1]=="#")
//                    {
//                        result=result.arg(at(currentRow,index1));
//                    }
//                    else
//                    {
//                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0');
//                    }
//                }
//                else if (sub_args1.size()==1)
//                {
//                    result=result.arg(*(variables[index1]));
//                }

//                return true;
//            }
//        }
//        else if (args[0]=="str" && args.size()==4)
//        {
//            QStringList sub_args1=args[2].split(',');
//            QStringList sub_args2=args[3].split(',');
//            int index1=variables_names.indexOf(sub_args1[0]);
//            int index2=variables_names.indexOf(sub_args2[0]);


//            if (index1>=0 && index2>=0)
//            {
//                result=args[1];

//                if (sub_args1.size()==2 && sub_args2.size()==2)
//                {
//                    if (sub_args1[1]=="#" && sub_args2[1]=="#")
//                    {
//                        result=result.arg(at(currentRow,index1)).arg(at(currentRow,index2));
//                    }
//                    else if (sub_args1[1]=="#")
//                    {
//                        result=result.arg(at(currentRow,index1)).arg(*(variables[index2]),sub_args2[1].toInt(),'g',-1,'0');
//                    }
//                    else if (sub_args2[1]=="#")
//                    {
//                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0').arg(at(currentRow,index2));
//                    }
//                    else
//                    {
//                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0').arg(*(variables[index2]),sub_args2[1].toInt(),'g',-1,'0');
//                    }
//                }
//                else if (sub_args1.size()==2 && sub_args2.size()==1)
//                {
//                    if (sub_args1[1]=="#")
//                    {
//                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0').arg(*(variables[index2]));
//                    }
//                    else
//                    {
//                        result=result.arg(at(currentRow,index1)).arg(*(variables[index2]));
//                    }
//                }
//                else if (sub_args1.size()==1 && sub_args2.size()==2)
//                {
//                    if (sub_args2[1]=="#")
//                    {
//                        result=result.arg(*(variables[index1])).arg(at(currentRow,index2));
//                    }
//                    else
//                    {
//                        result=result.arg(*(variables[index1])).arg(*(variables[index2]),sub_args2[1].toInt(),'g',-1,'0');
//                    }
//                }
//                else if (sub_args1.size()==1 && sub_args2.size()==1)
//                {
//                    result=result.arg(*(variables[index1])).arg(*(variables[index2]));
//                }

//                return true;
//            }
//            else
//            {
//                error("Bad variable indexes",QString("Indexes : %1 %2").arg(index1).arg(index2));
//                return false;
//            }
//        }
//        else
//        {
//            error("Bad custom expression",QString("Expression : ")+expression);
//            return false;
//        }
//    }
//    else
//    {
//        error("Bad custom expression",QString("Expression : ")+expression);
//        return false;
//    }

//    return false;
//}

int Register::getVarExpDialog(QString currentName, QString currentExpression, QString & newName, QString & newExpression)
{
    QLineEdit* le_variableName=new QLineEdit(newName);
    MyTextEdit* le_variableExpression=new MyTextEdit(newExpression);
    QScopedPointer<MyHighLighter> highlighter(new MyHighLighter(getVariablesList(),getFunctionsList()+getReservedList(),le_variableExpression->document()) );

    QCompleter * completer= new QCompleter(getVariablesList()+getFunctionsList()+getReservedList(), nullptr);
    le_variableExpression->setCompleter(completer);

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
        newExpression=le_variableExpression->textContent();

        if (currentName.isEmpty()) //add New
        {
            if (!newVariable(newName,newExpression) )
            {
                return -1;
            }
        }
        else //modify
        {
            if( !renameVariable(currentName,newName,currentExpression,newExpression) )
            {
                return -1;
            }
        }

        emit sig_modified();
        return 1;
    }
    else
    {
        return 0;
    }
}

bool Register::editVariableAndExpression(int currentIndex)
{
    QString currentName,currentExpression;
    QString newName,newExpression;

    if (currentIndex<this->size() && currentIndex>=0) //fetch variable and expression
    {
        currentName=variables_names[currentIndex];
        currentExpression=variables_expressions[currentIndex];
    }
    else
    {
        currentName.clear();
        currentExpression.clear();
    }
    newName=currentName;
    newExpression=currentExpression;

    int ret=0;
    do
    {
        ret=getVarExpDialog(currentName,currentExpression, newName, newExpression);
    }
    while(ret==-1);

    return bool(ret);

}

