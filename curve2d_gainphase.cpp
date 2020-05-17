#include "curve2d_gainphase.h"

Curve2D_GainPhase::Curve2D_GainPhase()
{
    this->legendgname="no name";
    this->legendpname="no name";
}

Curve2D_GainPhase::Curve2D_GainPhase(const Eigen::VectorXd& f,
                                     const Eigen::VectorXd& g,
                                     const Eigen::VectorXd& p,
                                     QString legendgname,
                                     QString legendpname)
{
    this->f=f;
    this->g=g;
    this->p=p;
    this->legendgname=legendgname;
    this->legendpname=legendpname;
}

Eigen::VectorXd Curve2D_GainPhase::buildFQVector(unsigned int sz)
{
    Eigen::VectorXd qv(sz);
    for (int i=0; i<sz; i++)
    {
        qv[i]=(double)i;
    }
    return qv;
}


void Curve2D_GainPhase::fit(Shape<Eigen::Vector3d>* model)
{
    std::vector<Eigen::Vector3d> points;
    for (int m=0; m<f.size(); ++m)
    {
        points.push_back(Eigen::Vector3d(f[m],g[m],p[m]));
    }

    model->fit(points,10000);
}

void Curve2D_GainPhase::operator=(const Curve2D_GainPhase& other)
{
    this->f=other.f;
    this->g=other.g;
    this->p=other.p;
    this->legendgname=other.legendgname;
    this->legendpname=other.legendpname;
}
