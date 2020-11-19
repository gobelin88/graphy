#include <QString>
#include <QObject>
#include <QDir>
#include <QVector>
#include "exprtk/exprtk.hpp"
#include "random"
#include <cmath>
#include <qmath.h>

#include "MyVariant.h"

#ifndef REGISTER_H
#define REGISTER_H


class Register:public QObject
{
    Q_OBJECT

public:
    //using VariableType=double;
    using VariableType=std::complex<double>;


    Register();

    void swapVariables(int ida,int idb);
    void moveVariable(int ida,int idb);
    void dispVariables();
    bool editVariableAndExpression(int currentIndex);

    void clear();
    bool newVariable(QString varname,QString varexpr);
    void delVariable(QString varname);
    bool renameVariable(QString old_varname, QString new_varname, QString oldExpression, QString newExpression);

    bool isValidExpression(QString variableExpression);
    bool isValidVariable(QString variableName);
    bool existVariable(QString variableName);
    int countVariable(QString variableName);

    void setActiveRow(int i){this->activeRow=i;}
    void setActiveCol(int i){this->activeCol=i;}

    QStringList getCustomExpressionList();
    //bool customExpressionParse(unsigned int id, QString& result, std::function<QString(int,int)> at, int currentRow);
    bool customExpressionParse(const MatrixXv & data,unsigned int id, MyVariant & result, int currentRow);

    const QStringList & variablesNames()const;
    const QStringList & variablesExpressions()const;

    int size();

    exprtk::symbol_table<VariableType> & symbols();

    void setVariable(int i,VariableType value);

    bool compileExpression(int id);
    void Register::currentCompiledExpressionValue(MyVariant & variant)const;

signals:
    void sig_modified();

private:
    //Exprtk
    exprtk::parser<VariableType> parser;
    exprtk::symbol_table<VariableType> symbolsTable;
    exprtk::expression<VariableType> current_compiled_expression;
    QVector<VariableType*> variables;
    QStringList variables_names;
    QStringList variables_expressions;

    //Misc variables
    VariableType activeRow;
    VariableType activeCol;

    //Error message
    void error(QString title,QString msg);

    //Noise management
    std::default_random_engine generator;
    std::normal_distribution<double> * noise_normal;
    std::uniform_real<double> * noise_uniform;

    //Misc
    QString fromNumber(double value);

    int getVarExpDialog(QString currentName, QString currentExpression, QString & newName, QString & newExpression);
};

#endif // REGISTER_H
