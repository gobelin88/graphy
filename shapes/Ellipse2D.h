#pragma once
#include "shapes/Shape.hpp"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

class Ellipse2D: public Shape<Eigen::Vector2d>
{
public:
    Ellipse2D();
    Ellipse2D(Eigen::Vector2d center,double radiusA,double radiusB,double theta);

    static QDialog* createDialog(QDoubleSpinBox*& getRa,
                                 QDoubleSpinBox*& getRb,
                                 QDoubleSpinBox*& getA,
                                 QDoubleSpinBox*& getB,
                                 QDoubleSpinBox*& getT);

    void create(Eigen::VectorXd & x,Eigen::VectorXd & y,int N);
    Eigen::Vector2d getCenter()const;
    double getRa()const;
    double getRb()const;
    double getTheta()const;
    double getR(double theta)const;
    Eigen::Vector2d delta(const Eigen::Vector2d& P);
    int nb_params();
    void setParams(const Eigen::VectorXd& p);
    const Eigen::VectorXd& getParams();

private:
    Eigen::VectorXd p;
};
