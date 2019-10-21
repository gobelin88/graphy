#include "curve2d_gainphase.h"

Curve2D_GainPhase::Curve2D_GainPhase()
{
    this->f.clear();
    this->g.clear();
    this->p.clear();
    this->legendgname="no name";
    this->legendpname="no name";
}

Curve2D_GainPhase::Curve2D_GainPhase(const QVector<double> & f,
                                     const QVector<double> & g,
                                     const QVector<double> & p,
                                     QString legendgname,
                                     QString legendpname)
{
    this->f=f;
    this->g=g;
    this->p=p;
    this->legendgname=legendgname;
    this->legendpname=legendpname;
}

QVector<double> Curve2D_GainPhase::buildFQVector(unsigned int sz)
{
    QVector<double> qv(sz,0.0);
    for(int i=0;i<sz;i++)
    {
        qv[i]=(double)i;
    }
    return qv;
}


void Curve2D_GainPhase::fit(Shape<Eigen::Vector3d> * model)
{
    std::vector<Eigen::Vector3d> points;
    for(int m=0;m<f.size();++m)
    {
        points.push_back(Eigen::Vector3d(f[m],g[m],p[m]));
    }

    model->fit(points,10000);
}

void Curve2D_GainPhase::operator=(const Curve2D_GainPhase & other)
{
    this->f=other.f;
    this->g=other.g;
    this->p=other.p;
    this->legendgname=other.legendgname;
    this->legendpname=other.legendpname;
}
