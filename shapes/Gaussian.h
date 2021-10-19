#pragma once
#include "shapes/Shape.hpp"

#include <QDialog>
#include <QDoubleSpinBox>
#include "ModelCurveInterface.h"

class Gaussian: public Shape<Eigen::Vector2d>,public ModelCurveInterface
{
public:

    Gaussian(double S,double M,double K);

    double getS()const;
    double getM()const;
    double getK()const;

    void  setS(double R);
    void  setM(double L);
    void  setK(double K);


    Eigen::Vector2d delta(const Eigen::Vector2d& pt);
    int nb_params();
    void setParams(const Eigen::VectorXd& p);
    const Eigen::VectorXd& getParams();

    //ModelCurveInterface
    double at(double x);
    Eigen::VectorXd at(Eigen::VectorXd f);
    void setParameter(QString parameterName,double value);
    double getParameter(QString parameterName);
    void setFixedParameter(QString parameterName,bool fixed);

private:
    Eigen::VectorXd p;
};
