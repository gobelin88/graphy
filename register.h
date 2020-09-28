#include <QString>
#include <QDir>
#include <QVector>
#include "exprtk/exprtk.hpp"
#include "random"

#ifndef REGISTER_H
#define REGISTER_H


class Register
{
public:
    Register();

    void swapVariables(int ida,int idb);
    void moveVariable(int ida,int idb);
    void dispVariables();

    void clear();
    bool newVariable(QString varname,QString varexpr);
    void delVariable(QString varname);
    bool renameVariable(QString old_varname, QString new_varname, QString oldExpression, QString newExpression);

    bool isValidExpression(QString variableExpression);
    bool isValidVariable(QString variableName);

    void setActiveRow(int i){this->activeRow=i;}
    void setActiveCol(int i){this->activeCol=i;}

private:
    //Exprtk
    exprtk::symbol_table<double> symbolsTable;
    QVector<double*> variables;
    QStringList variables_names;
    QStringList variables_expressions;

    //Misc variables
    double activeRow;
    double activeCol;

    //Error message
    void error(QString title,QString msg);

    //Noise management
    std::default_random_engine generator;
    std::normal_distribution<double> * noise_normal;
    std::uniform_real<double> * noise_uniform;

    //Misc
    QString fromNumber(double value);
};

#endif // REGISTER_H
