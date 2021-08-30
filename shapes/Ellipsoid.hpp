#pragma once

#include "shapes/Shape.hpp"

class Ellipsoid: public Shape<Eigen::Vector3d>
{
public:
    Ellipsoid(bool searchR)
	{
		this->searchR=searchR;

		if(searchR)
		{
			p.resize(9);
			p[0]=0;
			p[1]=0;
			p[2]=0;
			p[3]=0;
			p[4]=0;
			p[5]=0;
			p[6]=0;
			p[7]=0;
			p[8]=0;
		}
		else
		{
			p.resize(6);
			p[0]=0;
			p[1]=0;
			p[2]=0;
			p[3]=0;
			p[4]=0;
			p[5]=0;
		}
	}
    Ellipsoid(Eigen::Vector3d center, double A,double B,double C,bool searchR)
	{
		this->searchR=searchR;

		if(searchR)
		{
			p.resize(9);
			p[0]=center[0];
			p[1]=center[1];
			p[2]=center[2];
			p[3]=sqrt(A);
			p[4]=sqrt(B);
			p[5]=sqrt(C);
			p[6]=0;
			p[7]=0;
			p[8]=0;
		}
		else
		{
			p.resize(6);
			p[0]=center[0];
			p[1]=center[1];
			p[2]=center[2];
			p[3]=sqrt(A);
			p[4]=sqrt(B);
			p[5]=sqrt(C);
		}
	}
	
    ~Ellipsoid()
	{
		std::cout<<"Delete Ellipsoid"<<std::endl;
	}

    inline Eigen::Vector3d getCenter()const
    {
        return Eigen::Vector3d(p[0],p[1],p[2]);
    }
    double getA()const
	{
		return p[3]*p[3];
	}
    double getB()const
	{
		return p[4]*p[4];
	}
    double getC()const
	{
		return p[5]*p[5];
	}
    Eigen::Matrix3d getS()
	{
		Eigen::Matrix3d S;
		S<<1/(p[3]*p[3]),0      ,0,
		   0     ,1/(p[4]*p[4]) ,0,
		   0     ,0      ,1/(p[5]*p[5]);


		return S;
	}
    Eigen::Matrix3d getR()
	{
		if(searchR)
		{
			double minR=std::min(std::min(getA(),getB()),getC());
			double maxR=std::max(std::max(getA(),getB()),getC());

			//Si les rayons sont identiques R n'a pas de sens
			Eigen::Vector3d a=(maxR-minR)*Eigen::Vector3d(p[6],p[7],p[8]);
			double theta=a.norm();
			if(a.norm()>0)
			{
				return Eigen::AngleAxisd(theta,a/theta).toRotationMatrix();
			}
			else
			{
				return Eigen::Matrix3d::Identity();
			}
		}
		else
		{
			return Eigen::Matrix3d::Identity();
		}
	}

    Eigen::Vector3d delta(const Eigen::Vector3d& P)
	{
		Eigen::Vector3d PC=P-getCenter();
		Eigen::Vector3d u= getS()*getR().transpose()*PC;
		return PC*(1.0-1.0/u.norm());
	}
	
    int nb_params()
	{
		return p.rows();
	}
	
    void setParams(const Eigen::VectorXd& p)
	{
		this->p=p;
	}

    const Eigen::VectorXd& getParams()
	{
		return p;
	}

private:
    Eigen::VectorXd p;
    bool searchR;
};
