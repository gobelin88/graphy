#pragma once
#include "shapes/Shape.hpp"

#include <QDialog>
#include <QDoubleSpinBox>

/**
 * @class Circle
 * @brief Décrit un cercle.
 */
class Circle2D: public Shape<Eigen::Vector2d>
{
public:
    Circle2D();
    Circle2D(Eigen::Vector2d center,double radius);

    static QDialog* createDialog(QDoubleSpinBox*& getR,
                                 QDoubleSpinBox*& getA,
                                 QDoubleSpinBox*& getB);

    void create(Eigen::VectorXd & x,Eigen::VectorXd & y,int N);

    Eigen::Vector2d getCenter()const;
    double getRadius()const;

    Eigen::Vector2d delta(const Eigen::Vector2d& P);
    int nb_params();
    void setParams(const Eigen::VectorXd& p);
    const Eigen::VectorXd& getParams();
private:
    Eigen::VectorXd p;
};
