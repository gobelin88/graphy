#include <QString>
#include <QObject>
#include <QDir>
#include <QVector>
#include "exprtk/exprtk.hpp"
#include "ExprtkCustomFunctions.hpp"
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
    ~Register();

    void setDataPtr(MatrixXv * data_ptr)
    {
        this->data_ptr=data_ptr;
        cf_data.setDataPtr(data_ptr);
    }

    void swapVariables(int ida,int idb);
    void moveVariable(int ida,int idb);
    void dispVariables();
    bool editVariableAndExpression(int currentIndex);

    void clear();
    bool newVariable   (QString varname,QString varexpr);
    void delVariable   (QString varname);
    bool renameVariable(QString old_varname, QString new_varname, QString oldExpression, QString newExpression);

    bool isValidExpression(QString variableExpression);
    bool isValidVariable  (QString variableName);
    bool existVariable    (QString variableName);
    int  countVariable    (QString variableName);

    void setActiveRow(int i,int rowCount)
    {
        this->activeRow=i;
        this->numberRows=rowCount;
    }
    void setActiveCol(int i){this->activeCol=i;}

    QStringList getVariablesList();

    QStringList getFunctionsList();
    QStringList getReservedList();

    //bool customExpressionParse(unsigned int id, QString& result, std::function<QString(int,int)> at, int currentRow);

    const QStringList & variablesNames()const;
    const QStringList & variablesExpressions()const;

    QString getSaveVariablesExpression(int i) const;
    static QString getLoadVariableExpression(QString var);

    int size();

    exprtk::symbol_table<VariableType> & symbols();

    void setVariable(int i,VariableType value);

    bool compileExpression(int id);
    void Register::currentCompiledExpressionValue(MyVariant & variant)const;

    Register * copy();

signals:
    void sig_modified();

private:
    //Data access
    MatrixXv * data_ptr;

    //Exprtk
    exprtk::parser<VariableType> parser;
    exprtk::expression<VariableType> current_compiled_expression;

    exprtk::symbol_table<VariableType> symbolsTable;
    QVector<VariableType*> variables;
    QStringList variables_names;
    QStringList variables_expressions;

    //Custom functions
    uniformFunction<VariableType> cf_uniform;
    normalFunction<VariableType> cf_normal;
    gammaFunction<VariableType> cf_gamma;
    zetaFunction<VariableType> cf_zeta;
    xsiFunction<VariableType> cf_xsi;
    linFunction<VariableType> cf_lin;
    dataFunction<VariableType,MatrixXv> cf_data;
    indexFunction<VariableType> cf_id;

    //Misc variables
    VariableType activeRow;
    VariableType activeCol;
    VariableType numberRows;
    VariableType numberCols;

    //Error message
    void error(QString title,QString msg);



    //Misc
    int getVarExpDialog(QString currentName, QString currentExpression, QString & newName, QString & newExpression);
};

#endif // REGISTER_H
