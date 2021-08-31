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
    CustomRealExp();
    bool setExpression(QString exp);
    QString getExpression();
    void registerParams(QString exp);
    QString getError();
    double at(double x);
    Eigen::VectorXd at(Eigen::VectorXd v);
    Eigen::Vector2d delta(const Eigen::Vector2d& P);
    int nb_params();
    void setParams(const Eigen::VectorXd& _p);
    const Eigen::VectorXd& getParams();
    const QStringList & getParamsNames();

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
    CustomRealExp * getCustomRealExpression();
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
