#pragma once
#include "shapes/Shape.hpp"

#include <QDialog>
#include <QDoubleSpinBox>

class Sigmoid: public Shape<Eigen::Vector2d>
{
public:

    Sigmoid(double A,double B,double C,double P);

    static QDialog* createDialog(QDoubleSpinBox*& getA,
                                 QDoubleSpinBox*& getB,
                                 QDoubleSpinBox*& getC,
                                 QDoubleSpinBox*& getP);

    Eigen::Vector2d delta(const Eigen::Vector2d& pt);

    double getA()const;
    double getB()const;
    double getC()const;
    double getP()const;

    void  setA(double A);
    void  setB(double B);
    void  setC(double C);
    void  setP(double P);

    double at(double t)const;

    Eigen::VectorXd at(Eigen::VectorXd t);

    int nb_params();
    void setParams(const Eigen::VectorXd& p);
    const Eigen::VectorXd& getParams();

private:
    Eigen::VectorXd p;
};
