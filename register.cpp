#include "register.h"

#include <QMessageBox>
#include <iostream>

Register::Register()
{
    noise_normal=new std::normal_distribution<double>(0.0,1.0);
    noise_uniform=new std::uniform_real<double>(0.0,1.0);

    clear();
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
