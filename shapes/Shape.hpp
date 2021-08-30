#include "Functor.h"
#include <iostream>
#include <qmath.h>

#define EPS (sqrt(Eigen::NumTraits<double>::epsilon()))

/**
 * @class Shape
 * @brief Interface pour une forme quelconque.
 */
#ifndef SHAPE_HPP
#define SHAPE_HPP
 
template<typename T>
class Shape:public Functor<double>
{
public:
    virtual ~Shape(){}

    virtual T delta(const T& p)=0;
    virtual int nb_params()=0;

    virtual void setParams(const Eigen::VectorXd& p)=0;
    virtual const Eigen::VectorXd& getParams()=0;

    int operator()(const Eigen::VectorXd& p, Eigen::VectorXd& h)
    {
        setParams(p);

        T dn;
        int n=dn.rows();

        if(threaded)
        {
            #pragma omp parallel for
            for (int i=0; i<points.size(); i++)
            {
                T d=delta(points[i]);

                for (int k=0; k<n; k++)
                {
                    h[n*i+k]=d[k];
                }
            }
        }
        else
        {
            for (int i=0; i<points.size(); i++)
            {
                T d=delta(points[i]);

                for (int k=0; k<n; k++)
                {
                    h[n*i+k]=d[k];
                }
            }
        }

        return 0;
    }

    int df(const Eigen::VectorXd& p, Eigen::MatrixXd& J)
    {
        for (int j=0; j< inputs(); j++)
        {
            Eigen::VectorXd dp=Eigen::VectorXd::Zero(inputs());
            dp[j]=EPS;
            Eigen::VectorXd h_df_p(values());
            this->operator ()(p+dp,h_df_p);
            Eigen::VectorXd h_df_m(values());
            this->operator ()(p-dp,h_df_m);

            J.col(j)=(h_df_p-h_df_m)/(2*EPS);
        }

        return 0;
    }

    void setPointsList(const std::vector<T> & points_list)
    {
        m_values=(int)points_list.size()*points_list[0].rows();
        m_inputs=nb_params();
        points=points_list;
    }

    void fit(const std::vector<T> & points, int it, double xtol=-1)
    {
        Eigen::VectorXd p=this->getParams();
        this->setPointsList(points);
        Eigen::LevenbergMarquardt<Shape<T>,double> lm(*this);
        lm.parameters.maxfev = it;
        lm.parameters.factor = 2;

        if(xtol>0)
        {
            lm.parameters.xtol= 1e-7;
        }
        lm.minimize(p);
    }

    double getRMS()
    {
        double rms=0.0;
        for (unsigned int i=0; i< points.size(); i++)
        {
            T delta=this->delta(points[i]);
            rms+=delta.squaredNorm();
        }
        return rms;
    }

    std::vector<T> getErr()
    {
        std::vector<Eigen::Vector3d> err_dist(points.size());

        #pragma omp parallel for
        for (unsigned int i=0; i< points.size(); i++)
        {
            err_dist[i]=this->delta(points[i]);
        }

        return err_dist;
    }

    Eigen::VectorXd getErrNorm()
    {
        Eigen::VectorXd err_dist(points.size());
        for (unsigned int i=0; i< points.size(); i++)
        {
            err_dist[i]=this->delta(points[i]).norm();
        }
        return err_dist;
    }
	
private:
    std::vector<T> points;
};

#endif
