#include "Cloud.h"

CloudScalar::CloudScalar(const QVector<double>& x,
                         const QVector<double>& y,
                         const QVector<double>& z,
                         QString labelX,
                         QString labelY,
                         QString labelZ)
{
    for (int i=0; i<x.size(); i++)
    {
        pts.push_back(QVector3D(x[i],y[i],z[i]));
    }
    scalarField.resize(x.size());
    scalarField.fill(0.0);

    rangeX=getRange(x);
    rangeY=getRange(y);
    rangeZ=getRange(z);
    rangeS=getRange(scalarField);

    gradient.loadPreset(QCPColorGradient::gpPolar);
    colors.resize(scalarField.size());
    gradient.colorize(scalarField.data(),rangeS,colors.data(),scalarField.size());

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS .clear();

    calcBarycenterAndBoundingRadius();
}

CloudScalar::CloudScalar(const QVector<double>& x,
                         const QVector<double>& y,
                         const QVector<double>& z,
                         const QVector<double>& s,
                         QString labelX,
                         QString labelY,
                         QString labelZ,
                         QString labelS)
{
    for (int i=0; i<x.size(); i++)
    {
        pts.push_back(QVector3D(x[i],y[i],z[i]));
    }
    scalarField=s;

    rangeX=getRange(x);
    rangeY=getRange(y);
    rangeZ=getRange(z);
    rangeS=getRange(s);

    gradient.loadPreset(QCPColorGradient::gpPolar);
    colors.resize(scalarField.size());
    gradient.colorize(scalarField.data(),rangeS,colors.data(),scalarField.size());

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS =labelS;

    calcBarycenterAndBoundingRadius();
}

QCPRange CloudScalar::getRange(const QVector<double>& v)
{
    double min = *std::min_element(v.constBegin(), v.constEnd());
    double max = *std::max_element(v.constBegin(), v.constEnd());
    return QCPRange(min,max);
}

QCPRange CloudScalar::getXRange()
{
    return rangeX;
}
QCPRange CloudScalar::getYRange()
{
    return rangeY;
}
QCPRange CloudScalar::getZRange()
{
    return rangeZ;
}
QCPRange CloudScalar::getScalarFieldRange()
{
    return rangeS;
}

QVector<QRgb>& CloudScalar::getColors()
{
    return colors;
}

const QVector<QVector3D>& CloudScalar::data()const
{
    return pts;
}

void CloudScalar::operator=(const CloudScalar& other)
{
    this->pts=other.pts;
}

QVector3D CloudScalar::getBarycenter()
{
    return Barycenter;
}

void CloudScalar::calcBarycenterAndBoundingRadius()
{
    Barycenter=QVector3D(0,0,0);
    for (int i=0; i<pts.size(); i++)
    {
        Barycenter+=pts[i];
    }
    Barycenter/=pts.size();

    boundingRadius=0.0;

    for (int i=0; i<pts.size(); i++)
    {
        float radius=pts[i].distanceToPoint(Barycenter);

        if (radius>boundingRadius)
        {
            boundingRadius=radius;
        }
    }
}

float CloudScalar::getBoundingRadius()
{
    return boundingRadius;
}

QCPColorGradient CloudScalar::getGradient()
{
    return gradient;
}

QString CloudScalar::getLabelX()
{
    return labelX;
}
QString CloudScalar::getLabelY()
{
    return labelY;
}
QString CloudScalar::getLabelZ()
{
    return labelZ;
}
QString CloudScalar::getLabelS()
{
    return labelS;
}
