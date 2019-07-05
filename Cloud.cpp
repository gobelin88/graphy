#include "Cloud.h"

Cloud::Cloud(const QVector<double> & x,const QVector<double> &  y,const QVector<double> &  z)
{
    for(int i=0;i<x.size();i++)
    {
        pts.push_back(QVector3D(x[i],y[i],z[i]));
    }
}

void Cloud::operator=(const Cloud & other)
{
    this->pts=other.pts;
}
