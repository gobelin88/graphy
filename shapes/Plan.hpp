#pragma once

#include "shapes/Shape.hpp"

/**
 * @class Plan
 * @brief Décrit un plan.
 */
class Plan: public Shape<Eigen::Vector3d>
{
public:
    Plan(Eigen::Vector3d n0,Eigen::Vector3d center)
	{
		p=n0;
        this->barycenter=center;
	}

    ~Plan()
	{
		std::cout<<"Delete Plan"<<std::endl;
	}

    inline Eigen::Vector3d getNormal()const
    {
        Eigen::Vector3d n(p[0],p[1],p[2]);
        return n/n.norm();
    }

    inline Eigen::Matrix3d getStableBase()const
    {
        Eigen::Matrix3d M;

        M.col(0)=Eigen::Vector3d(p[0],p[1],p[2]);

        if (std::abs(p[2]-1.0)>std::abs(p[0]-1.0) && std::abs(p[2]-1.0)>std::abs(p[1]-1.0))
        {
            M.col(1)=Eigen::Vector3d(p[1],-p[0],0);
        }
        else if (std::abs(p[1]-1.0)>std::abs(p[0]-1.0) && std::abs(p[1]-1.0)>std::abs(p[0]-1.0))
        {
            M.col(1)=Eigen::Vector3d(p[2],0,-p[0]);
        }
        else
        {
            M.col(1)=Eigen::Vector3d(0,p[2],-p[1]);
        }

        M.col(0).normalize();
        M.col(1).normalize();

        M.col(2)=M.col(0).cross(M.col(1));
        return M;
    }

    Eigen::Vector3d proj(const Eigen::Vector3d& P,const Eigen::Vector3d& V)const
	{
		Eigen::Vector3d n=getNormal();
		if (n.dot(V)!=0)
		{
			double k=n.dot(barycenter-P)/n.dot(V);
			return P+k*V;
		}
		else
		{
			return P;
		}
	}

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)
	{
		Eigen::Vector3d n=getNormal();
		return (pt-barycenter).dot(n)*n;
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


    Eigen::Vector3d getBarycenter()
    {
        return barycenter;
    }

private:
    Eigen::VectorXd p;
    Eigen::Vector3d barycenter;
};
