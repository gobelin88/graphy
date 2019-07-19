#include <QVector>
#include <QVector3D>
#include <QQuaternion>
#include <Eigen/Dense>

#include "qcustomplot.h"

#ifndef CLOUD_H
#define CLOUD_H

class Cloud
{
public:
    Cloud(const QVector<double> & x,const QVector<double> &  y,const QVector<double> &  z);
    void operator=(const Cloud & other);
    const QVector<QVector3D> & data()const {return pts;}

private:
    QVector<QVector3D> pts;
};
/////////////////////////////////////////////////////////////////////////////////////////////
class CloudScalar
{
public:
    CloudScalar(const QVector<double> & x,const QVector<double> &  y,const QVector<double> &  z,const QVector<double> &  s);
    void operator=(const CloudScalar & other);
    const QVector<QVector4D> & data()const {return pts;}

    QCPRange getRange();

    QVector<QRgb> & getColors(){return colors;}

private:
    QVector<QVector4D> pts;
    QVector<double> scalar;
    QVector<QRgb> colors;

    QCPColorGradient gradient;
};
/////////////////////////////////////////////////////////////////////////////////////////////
class CloudTransform
{
public:
    CloudTransform(const QVector<double> & x,
                   const QVector<double> & y,
                   const QVector<double> & z,
                   const QVector<double> & qw,
                   const QVector<double> & qx,
                   const QVector<double> & qy,
                   const QVector<double> & qz);

    void operator=(const CloudTransform & other);
    const QVector<QPair<QVector3D,QQuaternion>> & data()const {return pts;}

private:
    QVector<QPair<QVector3D,QQuaternion>> pts;
};

#endif
