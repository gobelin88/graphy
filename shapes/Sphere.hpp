#pragma once

#include "shapes/Shape.hpp"

class Sphere: public Shape<Eigen::Vector3d>
{
public:
    Sphere()
	{
		p.resize(4);
		p[0]=0;
		p[1]=0;
		p[2]=0;
		p[3]=0;
	}
    Sphere(Eigen::Vector3d center, double radius)
	{
		p.resize(4);
		p[0]=center[0];
		p[1]=center[1];
		p[2]=center[2];
		p[3]=radius>0 ? radius : 0.0 ;
	}

    ~Sphere()
	{
		std::cout<<"Delete Sphere"<<std::endl;
	}

    inline Eigen::Vector3d getCenter()const
    {
        return Eigen::Vector3d(p[0],p[1],p[2]);
    }
    inline double getRadius()const
    {
        return p[3];
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)
	{
		Eigen::Vector3d u=pt-getCenter();
		return  u-u*getRadius()/u.norm();
	}

    int nb_params()
	{
		return p.rows();
	}
	
    void setParams(const Eigen::VectorXd& _p)
	{
        this->p=_p;
	}

    const Eigen::VectorXd& getParams()
	{
		return p;
	}

private:
    Eigen::VectorXd p;
};
