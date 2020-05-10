#include "ShapeFit.h"

/////////////////////////////////////////////////////////////////////////////////////
//Some standard Shapes
Plan::Plan()
{
    p.resize(3);
    p[0]=0;
    p[1]=0;
    p[2]=0;
}

Eigen::Vector3d Plan::delta(const Eigen::Vector3d & pt) const
{
    double dist=pt.dot(getNormal())+getOffset();
    return Eigen::Vector3d(dist,dist,dist);
}

int Plan::nb_params()
{
    return 3;
}

void Plan::setParams(const Eigen::VectorXd & p)
{
    this->p=p;
}

const Eigen::VectorXd &Plan::getParams()
{
    return p;
}

/////////////////////////////////////////////////////////////////////////////////////
//Some standard Shapes
Sphere::Sphere()
{
    p.resize(4);
    p[0]=0;
    p[1]=0;
    p[2]=0;
    p[3]=0;
}

Sphere::Sphere(Eigen::Vector3d center, double radius)
{
    p.resize(4);
    p[0]=center[0];
    p[1]=center[1];
    p[2]=center[2];
    p[3]=radius>0 ? radius : 0.0 ;
}

Eigen::Vector3d Sphere::delta(const Eigen::Vector3d & pt) const
{
    Eigen::Vector3d u=pt-getCenter();
    u.normalize();
    return  (pt-getCenter())-u*getRadius();
}

int Sphere::nb_params()
{
    return 4;
}

void Sphere::setParams(const Eigen::VectorXd & p)
{
    this->p=p;
}

const Eigen::VectorXd &Sphere::getParams()
{
    return p;
}


/////////////////////////////////////////////////////////////////////////////////////
void testShapeFit()
{
    std::cout<<"Test Fit Sphere"<<std::endl;

    Eigen::Vector3d center(1,2,3);
    double rayon=1.123456789;

    std::vector<Eigen::Vector3d> points;
    for(int i=0;i<1000;i++)
    {
        double alpha=Ru()*M_PI*2,beta=Ru()*M_PI*2;
        Eigen::Vector3d U(cos(alpha)*cos(beta),sin(beta),sin(alpha)*cos(beta));
        points.push_back( center + rayon*U + Eigen::Vector3d(Rg(0,.05),Rg(0,.05),Rg(0,.05)));
    }

    Sphere sphere(Eigen::Vector3d(0,0,0),0);

    sphere.fit(points,500);

    std::cout<<"center="<<center.transpose()<<std::endl;
    std::cout<<"center_hat="<<sphere.getCenter().transpose()<<std::endl;
    std::cout<<"radius="<<rayon<<std::endl;
    std::cout<<"radius_hat="<<sphere.getRadius()<<std::endl;
    std::cout<<"RMS="<<sphere.getRMS()<<std::endl;
}

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

Eigen::Vector3d Circle3D::delta(const Eigen::Vector3d & pt) const
{
    Eigen::Vector3d u=pt-getCenter();
    return  Eigen::Vector3d( u.dot(getNormal()), (u-u.dot(getNormal())*getNormal()).norm()-getRadius() , 0 );
}

int Circle3D::nb_params()
{
    return 6;
}
void Circle3D::setParams(const Eigen::VectorXd & p)
{
    this->p=p;
}
const Eigen::VectorXd & Circle3D::getParams()
{
    return p;
}

double Ru()
{
    return (((double)(rand()%RAND_MAX))/RAND_MAX-0.5)*2;
}

double Rg(double mean,double sigma)
{

    double x=0;double y=0;
    while(x==0) x=(((double)(rand()%RAND_MAX))/(RAND_MAX-1));
    while(y==0) y=(((double)(rand()%RAND_MAX))/(RAND_MAX-1));
    double w=sqrt(-2*log(x))*cos(2*M_PI*y);
    return mean+(sigma*w);
}

