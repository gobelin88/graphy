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

CloudTransform::CloudTransform(const QVector<double> & x,
                               const QVector<double> & y,
                               const QVector<double> & z,
                               const QVector<double> & qw,
                               const QVector<double> & qx,
                               const QVector<double> & qy,
                               const QVector<double> & qz)
{
    for(int i=0;i<x.size();i++)
    {
        pts.push_back(QPair<QVector3D,QQuaternion>(QVector3D(x[i],y[i],z[i]),QQuaternion(qw[i],qx[i],qy[i],qz[i])));
    }
}

void CloudTransform::operator=(const CloudTransform & other)
{
    this->pts=other.pts;
}
