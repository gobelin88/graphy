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
        pts.push_back(Eigen::Vector4d(x[i],y[i],z[i],0.0));
    }

    rangeX=getRange(x);
    rangeY=getRange(y);
    rangeZ=getRange(z);
    rangeS=QCPRange(-1,1);

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
        pts.push_back(Eigen::Vector4d(x[i],y[i],z[i],s[i]));
    }

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

const std::vector<Eigen::Vector4d> & Cloud::data()const
{
    return pts;
}

std::vector<Eigen::Vector3d> Cloud::positions()const
{
    std::vector<Eigen::Vector3d> positions(pts.size());

    for(int i=0;i<pts.size();i++)
    {
        positions[i]=pts[i].head<3>();
    }

    return positions;
}

int Cloud::size()const
{
    return pts.size();
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
            Barycenter+=pts[i].head<3>();
        }
        Barycenter/=pts.size();


        for (int i=0; i<pts.size(); i++)
        {
            double radius=(pts[i].head<3>()-Barycenter).norm();

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

void Cloud::fit(Shape<Eigen::Vector3d>* model,int it,double xtol)
{
    model->fit(positions(),it,xtol);
}

void Cloud::project(Shape<Eigen::Vector3d>* model)
{
    for(int i=0;i<pts.size();i++)
    {
        pts[i].head<3>()-=model->delta(pts[i].head<3>());
    }
}

void Cloud::subSample(unsigned int nbPoints)
{
    int nbpoints_to_remove=(int)pts.size()-(int)nbPoints;

    for(int i=0;i<nbpoints_to_remove;i++)
    {
        int random_index=rand()%pts.size();
        pts.erase (pts.begin()+random_index);
    }
}

QByteArray Cloud::getBuffer(QCPRange range)
{
    const double * colordata=(const double *)pts.data()+3;
    colors.resize(pts.size());
    gradient.colorize(colordata,range,colors.data(),pts.size(),4,false);

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
