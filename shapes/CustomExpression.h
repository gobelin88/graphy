#pragma once

#include "shapes/Shape.hpp"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QLabel>
#include "ModelCurveInterface.h"

#include "exprtk/exprtk.hpp"

class CustomRealExp:
        public Shape<Eigen::Vector2d>,
        public ModelCurveInterface
{
public:
    CustomRealExp();
    bool setExpression(QString exp);
    QString getExpression();
    void registerParams(QString exp);
    QString getError();

    int nb_params();
    void setParams(const Eigen::VectorXd& _p);
    const Eigen::VectorXd& getParams();
    const QStringList & getParamsNames();
    Eigen::Vector2d delta(const Eigen::Vector2d& P);

    //ModelCurveInterface
    double at(double x);
    Eigen::VectorXd at(Eigen::VectorXd v) ;
    void setParameter(QString parameterName,double value);
    double getParameter(QString parameterName);
    void setFixedParameter(QString parameterName,bool fixed);


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
    void slot_setExp();
    void slot_p0Changed();

signals:
    void sig_modelChanged(ModelCurveInterface * model);

private:
    CustomRealExp customRealExp;
    QTextEdit * le_expr;
    QVector<QDoubleSpinBox*> sp_params;
    QVBoxLayout* vbox;
    QLabel * l_error;
    bool valid;
};
