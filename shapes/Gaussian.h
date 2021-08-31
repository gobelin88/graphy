#pragma once
#include "shapes/Shape.hpp"

#include <QDialog>
#include <QDoubleSpinBox>

class Gaussian: public Shape<Eigen::Vector2d>
{
public:

    Gaussian(double S,double M,double K);

    static QDialog* createDialog(QDoubleSpinBox*& getS,
                                 QDoubleSpinBox*& getM,
                                 QDoubleSpinBox*& getK);

    double getS()const;
    double getM()const;
    double getK()const;

    void  setS(double R);
    void  setM(double L);
    void  setK(double K);

    double at(double x)const;
    Eigen::VectorXd at(Eigen::VectorXd f);

    Eigen::Vector2d delta(const Eigen::Vector2d& pt);
    int nb_params();
    void setParams(const Eigen::VectorXd& p);
    const Eigen::VectorXd& getParams();

private:
    Eigen::VectorXd p;
};
