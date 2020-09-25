#include "Cloud.h"

Cloud::Cloud(const Eigen::VectorXd& x,
             const Eigen::VectorXd& y,
             const Eigen::VectorXd& z,
             QString labelX,
             QString labelY,
             QString labelZ)
{
    for (int i=0; i<x.size(); i++)
    {
        pts.push_back(Eigen::Vector3d(x[i],y[i],z[i]));
    }
    scalarField.resize(x.size());
    scalarField.fill(0.0);

    rangeX=getRange(x);
    rangeY=getRange(y);
    rangeZ=getRange(z);
    rangeS=getRange(scalarField);

    setGradientPreset(QCPColorGradient::gpPolar);

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS .clear();

    calcBarycenterAndBoundingRadius();
}

Cloud::Cloud(const Eigen::VectorXd& x,
             const Eigen::VectorXd& y,
             const Eigen::VectorXd& z,
             const Eigen::VectorXd& s,
             QString labelX,
             QString labelY,
             QString labelZ,
             QString labelS)
{
    for (int i=0; i<x.size(); i++)
    {
        pts.push_back(Eigen::Vector3d(x[i],y[i],z[i]));
    }
    scalarField=s;

    rangeX=getRange(x);
    rangeY=getRange(y);
    rangeZ=getRange(z);
    rangeS=getRange(s);

    setGradientPreset(QCPColorGradient::gpPolar);

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS =labelS;

    calcBarycenterAndBoundingRadius();
}

void Cloud::setGradientPreset(QCPColorGradient::GradientPreset preset)
{
    this->gradientPreset=preset;
    gradient.loadPreset(gradientPreset);
}

QCPColorGradient::GradientPreset Cloud::getGradientPreset()
{
    return gradientPreset;
}

const QCPColorGradient& Cloud::getGradient()
{
    return gradient;
}

QCPRange Cloud::getRange(const Eigen::VectorXd& v)
{
    return QCPRange(v.minCoeff(),v.maxCoeff());
}

QCPRange Cloud::getXRange()
{
    return rangeX;
}
QCPRange Cloud::getYRange()
{
    return rangeY;
}
QCPRange Cloud::getZRange()
{
    return rangeZ;
}
QCPRange Cloud::getScalarFieldRange()
{
    if (rangeS.lower==rangeS.upper)
    {
        return QCPRange(-1,1);
    }
    else
    {
        return rangeS;
    }
}

std::vector<QRgb>& Cloud::getColors()
{
    return colors;
}

const std::vector<Eigen::Vector3d>& Cloud::positions()const
{
    return pts;
}

void Cloud::operator=(const Cloud& other)
{
    this->pts=other.pts;
}

Eigen::Vector3d Cloud::getCenter()
{
    return Eigen::Vector3d(rangeX.center(),rangeY.center(),rangeZ.center());
}

Eigen::Vector3d Cloud::getBarycenter()
{
    return Barycenter;
}

void Cloud::calcBarycenterAndBoundingRadius()
{
    Barycenter=Eigen::Vector3d(0,0,0);
    boundingRadius=0.0;

    if (pts.size()>0)
    {
        for (int i=0; i<pts.size(); i++)
        {
            Barycenter+=pts[i];
        }
        Barycenter/=pts.size();


        for (int i=0; i<pts.size(); i++)
        {
            double radius=(pts[i]-Barycenter).norm();

            if (radius>boundingRadius)
            {
                boundingRadius=radius;
            }
        }
    }
}

double Cloud::getBoundingRadius()
{
    return boundingRadius;
}

QString Cloud::getLabelX()
{
    return labelX;
}
QString Cloud::getLabelY()
{
    return labelY;
}
QString Cloud::getLabelZ()
{
    return labelZ;
}
QString Cloud::getLabelS()
{
    return labelS;
}

QVector3D Cloud::toQVec3D(Eigen::Vector3d p)
{
    return QVector3D(p[0],p[1],p[2]);
}

void Cloud::fit(Shape<Eigen::Vector3d>* model,int it)
{
    model->fit(pts,it);
}

QByteArray Cloud::getColorBuffer(QCPRange range)
{
    colors.resize(scalarField.size());
    gradient.colorize(scalarField.data(),range,colors.data(),scalarField.size());

    QByteArray bufferBytes;
    bufferBytes.resize(2 * 3 * ( static_cast<int>(pts.size()) ) * sizeof(float));
    float* vertices = reinterpret_cast<float*>(bufferBytes.data());

    for (int i=0; i<pts.size(); i++)
    {
        *vertices++ = pts[i][0];
        *vertices++ = pts[i][1];
        *vertices++ = pts[i][2];

        *vertices++ = qRed  (colors[i])/255.0;
        *vertices++ = qGreen(colors[i])/255.0;
        *vertices++ = qBlue (colors[i])/255.0;
    }

    return bufferBytes;
}
