#include "ShapeFit.h"

/////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////
Circle2D::Circle2D()
{
    p.resize(3);
    p[0]=0;
    p[1]=0;
    p[2]=1;
}

Circle2D::Circle2D(Eigen::Vector2d center,double radius)
{
    p.resize(3);
    p[0]=center[0];
    p[1]=center[1];
    p[2]=radius;
}

Eigen::Vector2d Circle2D::delta(const Eigen::Vector2d& P)
{
    Eigen::Vector2d CP=P-getCenter();
    return  CP*(1-getRadius()/CP.norm());
}

int Circle2D::nb_params()
{
    return 3;
}
void Circle2D::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}
const Eigen::VectorXd& Circle2D::getParams()
{
    return p;
}

