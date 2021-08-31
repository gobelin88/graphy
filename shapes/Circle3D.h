#pragma once
#include "shapes/Shape.hpp"

/**
 * @class Circle
 * @brief Décrit un cercle.
 */
class Circle3D: public Shape<Eigen::Vector3d>
{
public:
    Circle3D();
    Circle3D(Eigen::Vector3d center,double radius);

    Eigen::Vector3d getNormal()const;
    Eigen::Vector3d getCenter()const;
    double getRadius()const;

    Eigen::Vector3d delta(const Eigen::Vector3d& pt);
    int nb_params();
    void setParams(const Eigen::VectorXd& p);
    const Eigen::VectorXd& getParams();
private:
    Eigen::VectorXd p;
};
