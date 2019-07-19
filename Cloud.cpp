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

CloudScalar::CloudScalar(const QVector<double> & x,const QVector<double> &  y,const QVector<double> &  z,const QVector<double> &  s)
{
    for(int i=0;i<x.size();i++)
    {
        pts.push_back(QVector4D(x[i],y[i],z[i],s[i]));
    }

    gradient.loadPreset(QCPColorGradient::gpHues);
    QCPRange range=getRange();
    colors.resize(s.size());
    gradient.colorize(s.data(),range,colors.data(),s.size());
}

QCPRange CloudScalar::getRange()
{
    float min=FLT_MAX,max=-FLT_MAX;

    for(int i=0;i<pts.size();i++)
    {
        if(pts[i][3]>max)max=pts[i][3];
        if(pts[i][3]<min)min=pts[i][3];
    }

    return QCPRange(min,max);
}

void CloudScalar::operator=(const CloudScalar & other)
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
