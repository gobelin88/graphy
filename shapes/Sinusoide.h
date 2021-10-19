#pragma once

#include "shapes/Shape.hpp"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

#include "FitDialog.h"
#include "ModelCurveInterface.h"

class Sinusoide:
        public Shape<Eigen::Vector2d>,
        public ModelCurveInterface
{
public:

    Sinusoide(double A,double F,double P,double C,
              bool fixedA,bool fixedF,bool fixedP,bool fixedC
              );

    void set(double A,double F,double P,double C,
             bool fixedA,bool fixedF,bool fixedP,bool fixedC
             );

    Eigen::Vector2d delta(const Eigen::Vector2d& pt);

    double getA()const;
    double getF()const;
    double getP()const;
    double getC()const;

    void  setA(double A);
    void  setF(double F);
    void  setP(double P);
    void  setC(double C);

    void regularized();

    int nb_params();
    void setParams(const Eigen::VectorXd& _p);
    const Eigen::VectorXd& getParams();

    //ModelCurveInterface
    double at(double t);
    Eigen::VectorXd at(Eigen::VectorXd t);
    void setParameter(QString parameterName,double value);
    double getParameter(QString parameterName);
    void setFixedParameter(QString parameterName,bool fixed);


private:
    Eigen::VectorXd p;
    Eigen::VectorXd p_fixed;
    int idA,idF,idP,idC;
    bool fixedA,fixedF,fixedP,fixedC;
};
