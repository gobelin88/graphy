#pragma once

#include <Eigen/Dense>

class ModelCurveInterface
{
public:
    virtual double at(double t)=0;
    virtual Eigen::VectorXd at(Eigen::VectorXd t)=0;

    virtual void setParameter(QString parameterName,double value)=0;
    virtual double getParameter(QString parameterName)=0;
    virtual void setFixedParameter(QString parameterName,bool fixed)=0;
};
