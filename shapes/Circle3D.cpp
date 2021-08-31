#include "Circle3D.h"

Circle3D::Circle3D()
{
    p.resize(6);
    p[0]=0;
    p[1]=0;
    p[2]=0;
    p[3]=0;
    p[4]=0;
    p[5]=0.5;
}

Circle3D::Circle3D(Eigen::Vector3d center,double radius)
{
    p.resize(6);
    p[0]=0;
    p[1]=0;
    p[2]=center[0];
    p[3]=center[1];
    p[4]=center[2];
    p[5]=radius;
}

Eigen::Vector3d Circle3D::delta(const Eigen::Vector3d& pt)
{
    Eigen::Vector3d u=pt-getCenter();
    return  Eigen::Vector3d( u.dot(getNormal()), (u-u.dot(getNormal())*getNormal()).norm()-getRadius(), 0 );
}

int Circle3D::nb_params()
{
    return 6;
}
void Circle3D::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}
const Eigen::VectorXd& Circle3D::getParams()
{
    return p;
}

Eigen::Vector3d Circle3D::getNormal()const
{
    return Eigen::Vector3d(sin(p[0])*sin(p[1]),cos(p[1]),cos(p[0])*sin(p[1]));
}
Eigen::Vector3d Circle3D::getCenter()const
{
    return Eigen::Vector3d(p[2],p[3],p[4]);
}
double Circle3D::getRadius()const
{
    return p[5];
}
