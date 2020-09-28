#include "register.h"

#include <QMessageBox>
#include <iostream>

Register::Register()
{
    noise_normal=new std::normal_distribution<double>(0.0,1.0);
    noise_uniform=new std::uniform_real<double>(0.0,1.0);

    clear();
    current_compiled_expression.register_symbol_table(symbolsTable);
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
    symbolsTable.add_pi();
    symbolsTable.add_variable("Row",activeRow);
    symbolsTable.add_variable("Col",activeCol);
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

    double * p_double=new double(0.0);
    variables.push_back(p_double);
    symbolsTable.add_variable(varname.toStdString(),*p_double);

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
        exprtk::parser<double> parser;
        exprtk::expression<double> expression;
        expression.register_symbol_table(symbolsTable);
        bool ok=parser.compile(variableExpression.toStdString(),expression);

        if (!ok)
        {
            error("Invalid formula",variableExpression+QString("\nError : ")+QString::fromStdString(parser.error()));
        }

        return ok;
    }
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

QString Register::fromNumber(double value)
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

const QStringList & Register::variablesNames()
{
    return variables_names;
}

const QStringList & Register::variablesExpressions()
{
    return variables_expressions;
}

unsigned int Register::size()
{
    return variables.size();
}

exprtk::symbol_table<double> & Register::symbols()
{
    return symbolsTable;
}

void Register::setVariable(int i,double value)
{
    if(i<variables.size())
    {
        *(variables[i])=value;
    }
}

bool Register::compileExpression(int id)
{


    return parser.compile(variables_expressions[id].toStdString(),current_compiled_expression);
}

double Register::currentCompiledExpressionValue()
{
    return current_compiled_expression.value();
}

QStringList Register::getCustomExpressionList()
{
    QStringList customExpList;
    customExpList<<"$uniform";
    customExpList<<"$normal";
    customExpList<<"$search ";
    customExpList<<"$str";
    return customExpList;
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

bool Register::customExpressionParse(unsigned int id,QString& result,std::function<QString(int,int)> at,int currentRow)
{
    QString expression=variables_expressions[id];
    expression.remove('$');
    QStringList args=expression.split(" ");
    if (args.size()>0)
    {

        if (args[0]=="uniform" && args.size()==1)
        {
            result=fromNumber((*noise_uniform)(generator));
            return true;
        }
        else if (args[0]=="normal" && args.size()==1)
        {
            result=fromNumber((*noise_normal)(generator));
            return true;
        }
        else if (args[0]=="search" && args.size()>=5)
        {
            //0      1    2      3  4     5      6      i
            //search what filter depth where0 where1 where2 wherei...

            QString search_for_name;
            int index1=variables_names.indexOf(args[1]);
            if (index1>=0)
            {
                search_for_name=at(currentRow,index1);
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
                custom_scanDir(args[i],filters,search_for_name,result,args[4].toInt());
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
                        result=result.arg(at(currentRow,index1));
                    }
                    else if (sub_args1[1]=="#")
                    {
                        result=result.arg(at(currentRow,index1));
                    }
                    else
                    {
                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0');
                    }
                }
                else if (sub_args1.size()==1)
                {
                    result=result.arg(*(variables[index1]));
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
                        result=result.arg(at(currentRow,index1)).arg(at(currentRow,index2));
                    }
                    else if (sub_args1[1]=="#")
                    {
                        result=result.arg(at(currentRow,index1)).arg(*(variables[index2]),sub_args2[1].toInt(),'g',-1,'0');
                    }
                    else if (sub_args2[1]=="#")
                    {
                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0').arg(at(currentRow,index2));
                    }
                    else
                    {
                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0').arg(*(variables[index2]),sub_args2[1].toInt(),'g',-1,'0');
                    }
                }
                else if (sub_args1.size()==2 && sub_args2.size()==1)
                {
                    if (sub_args1[1]=="#")
                    {
                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0').arg(*(variables[index2]));
                    }
                    else
                    {
                        result=result.arg(at(currentRow,index1)).arg(*(variables[index2]));
                    }
                }
                else if (sub_args1.size()==1 && sub_args2.size()==2)
                {
                    if (sub_args2[1]=="#")
                    {
                        result=result.arg(*(variables[index1])).arg(at(currentRow,index2));
                    }
                    else
                    {
                        result=result.arg(*(variables[index1])).arg(*(variables[index2]),sub_args2[1].toInt(),'g',-1,'0');
                    }
                }
                else if (sub_args1.size()==1 && sub_args2.size()==1)
                {
                    result=result.arg(*(variables[index1])).arg(*(variables[index2]));
                }

                return true;
            }
            else
            {
                error("Bad variable indexes",QString("Indexes : %1 %2").arg(index1).arg(index2));
                return false;
            }
        }
        else
        {
            error("Bad custom expression",QString("Expression : ")+expression);
            return false;
        }
    }
    else
    {
        error("Bad custom expression",QString("Expression : ")+expression);
        return false;
    }

    return false;
}

