#include <QString>
#include <QObject>
#include <QDir>
#include <QVector>
#include "exprtk/exprtk.hpp"
#include "random"

#include "value.h"

#ifndef REGISTER_H
#define REGISTER_H


class Register:public QObject
{
    Q_OBJECT
public:
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
    bool customExpressionParse(unsigned int id, QString& result, std::function<QString(int,int)> at, int currentRow);
    bool customExpressionParse2(const MatrixXv & data,unsigned int id, ValueContainer & result, int currentRow);

    const QStringList & variablesNames()const;
    const QStringList & variablesExpressions()const;

    int size();

    exprtk::symbol_table<double> & symbols();

    void setVariable(int i,double value);

    bool compileExpression(int id);
    double currentCompiledExpressionValue();

signals:
    void sig_modified();

private:
    //Exprtk
    exprtk::parser<double> parser;
    exprtk::symbol_table<double> symbolsTable;
    exprtk::expression<double> current_compiled_expression;
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

    int getVarExpDialog(QString currentName, QString currentExpression, QString & newName, QString & newExpression);
};

#endif // REGISTER_H
