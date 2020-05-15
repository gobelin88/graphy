#include <QVector>
#include <QVector3D>
#include <QQuaternion>
#include <Eigen/Dense>

#include "qcustomplot.h"

#ifndef CLOUD_H
#define CLOUD_H

/////////////////////////////////////////////////////////////////////////////////////////////
class CloudScalar
{
public:
    CloudScalar(const QVector<double>& x,
                const QVector<double>& y,
                const QVector<double>& z,
                QString labelX,
                QString labelY,
                QString labelZ);

    CloudScalar(const QVector<double>& x,
                const QVector<double>& y,
                const QVector<double>& z,
                const QVector<double>& scalarField,
                QString labelX,
                QString labelY,
                QString labelZ,
                QString labelS);

    void operator=(const CloudScalar& other);
    const QVector<QVector3D>& positions()const;

    QCPRange getXRange();
    QCPRange getYRange();
    QCPRange getZRange();
    QCPRange getScalarFieldRange();
    QVector<QRgb>& getColors();

    QVector3D getBarycenter();
    float getBoundingRadius();

    QCPColorGradient getGradient();

    QString getLabelX();
    QString getLabelY();
    QString getLabelZ();
    QString getLabelS();

private:
    void calcBarycenterAndBoundingRadius();

    QCPRange getRange(const QVector<double>& v);

    QVector<QVector3D> pts;
    QVector<double> scalarField;
    QVector<QRgb> colors;

    QCPColorGradient gradient;

    QCPRange rangeX,rangeY,rangeZ,rangeS;
    QString labelX ;
    QString labelY ;
    QString labelZ ;
    QString labelS ;

    QVector3D Barycenter;
    float boundingRadius;
};

#endif
