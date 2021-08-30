#pragma once

#include "shapes/Shape.hpp"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

#include "exprtk/exprtk.hpp"

class CustomRealExp: public Shape<Eigen::Vector2d>
{
public:

    CustomRealExp()
    {
        parser=new exprtk::parser<double>;
        current_compiled_expression=new exprtk::expression<double>;
        symbolsTable=new exprtk::symbol_table<double>;
        current_compiled_expression->register_symbol_table(*symbolsTable);


    }

    bool setExpression(QString exp)
    {
        this->exp=exp;
        registerParams(exp);
        bool ok=parser->compile(exp.toStdString(),*current_compiled_expression);
        return ok;
    }

    QString getExpression()
    {
        return exp;
    }

    void registerParams(QString exp)
    {
        if(current_compiled_expression)
        {
            delete current_compiled_expression;
            current_compiled_expression=new exprtk::expression<double>;
        }

        symbolsTable->clear();
        symbolsTable->add_variable("x",x);

        QStringList paramsList;
        paramsList<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H";
        int id=0;
        paramsNames.clear();

        for(int i=0;i<paramsList.size();i++)
        {
            if(exp.contains(QRegExp(QString(R"abcd((^|[^A-Za-z0-9])%1(?=\$|[^A-Za-z0-9]))abcd").arg(paramsList[i]))))
            {
                paramsNames.append(paramsList[i]);
                id++;
            }
        }
        p.resize(id);
        p.setZero();

        for(int i=0;i<paramsNames.size();i++)
        {
            symbolsTable->add_variable(paramsNames[i].toStdString(),p[i]);
        }

        current_compiled_expression->register_symbol_table(*symbolsTable);
    }

    QString getError()
    {
        return QString::fromStdString(parser->error());
    }

    double at(double x)
    {
        this->x=x;
        return current_compiled_expression->value();
    }

    Eigen::VectorXd at(Eigen::VectorXd v)
    {
        Eigen::VectorXd y(v.size());

        for (int i=0; i<v.size(); i++)
        {
            y[i]=at(v[i]);
        }
        return y;
    }

    Eigen::Vector2d delta(const Eigen::Vector2d& P)
    {
        return Eigen::Vector2d(0,P[1]-at(P[0]));
    }

    int nb_params(){return p.rows();}
    void setParams(const Eigen::VectorXd& _p)
    {
        this->p=_p;
    }
    const Eigen::VectorXd& getParams(){return p;}

    const QStringList & getParamsNames()
    {
        return paramsNames;
    }

private:
    Eigen::VectorXd p;
    double x;

    QStringList paramsNames;

    QString exp;

    exprtk::parser<double> * parser;
    exprtk::expression<double> * current_compiled_expression;
    exprtk::symbol_table<double> * symbolsTable;
};

class CustomExpDialog:public QDialog
{
    Q_OBJECT
public:
    CustomExpDialog();

    const QVector<QDoubleSpinBox*> & getSpinBoxParams();
    bool isValid();
    CustomRealExp * getCustomRealExpression(){return &customRealExp;}
    VectorXd getP0();

public slots:
    void slot_setExp(QString expStr);
    void slot_p0Changed();

private:
    CustomRealExp customRealExp;
    QLineEdit * le_expr;
    QVector<QDoubleSpinBox*> sp_params;
    QVBoxLayout* vbox;
    QLabel * l_error;
    bool valid;
};