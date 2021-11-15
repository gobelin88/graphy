#include "Cloud.h"

void Cloud::init()
{
    useCustomColor=false;
    customColor=qRgb(0,0,0);
    setGradientPreset(QCPColorGradient::gpPolar);
    calcBarycenterAndBoundingRadius();

    setName(QString("Cloud : %4(%1,%2,%3)")
                       .arg(getLabelX())
                       .arg(getLabelY())
                       .arg(getLabelZ())
                       .arg(getLabelS()));
}

Cloud::Cloud()
{
    rangeX=QCPRange(-1,1);
    rangeY=QCPRange(-1,1);
    rangeZ=QCPRange(-1,1);
    rangeS=QCPRange(-1,1);

    this->labelX ="X";
    this->labelY ="Y";
    this->labelZ ="Z";
    this->labelS .clear();

    init();
}

Cloud::Cloud(std::vector<Eigen::Vector3d> plist,
      QString labelX,
      QString labelY,
      QString labelZ)
{
    for (int i=0; i<plist.size(); i++)
    {
        pts.push_back(Eigen::Vector4d(plist[i][0],plist[i][1],plist[i][2],0.0));
    }

    calcRanges();

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS .clear();

    init();
}

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

    calcRanges();

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS .clear();

    init();
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

    calcRanges();

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS =labelS;

    init();
}

Cloud::~Cloud()
{
    std::cout<<"Delete Cloud"<<std::endl;
}

void Cloud::setName(QString name)
{
    this->name=name;
}

QString Cloud::getName()
{
    return name;
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

//QCPRange Cloud::getRange(const Eigen::VectorXd& v)
//{
//    double min=v.minCoeff();
//    double max=v.maxCoeff();
//    if(min!=max)
//    {
//        return QCPRange(min,max);
//    }
//    else
//    {
//        double eps=0.1;
//        return QCPRange(min-eps,max+eps);
//    }
//}

void Cloud::calcRanges()
{
    Eigen::Vector4d min,max;
    min.setConstant( DBL_MAX);
    max.setConstant(-DBL_MAX);

    for(int i=0;i<pts.size();i++)
    {
        for(int j=0;j<4;j++)
        {
            if(pts[i][j]<min[j]){min[j]=pts[i][j];}
            if(pts[i][j]>max[j]){max[j]=pts[i][j];}
        }
    }

    rangeX=QCPRange(min[0],max[0]);
    rangeY=QCPRange(min[1],max[1]);
    rangeZ=QCPRange(min[2],max[2]);
    rangeS=QCPRange(min[3],max[3]);

    std::cout<<"Ranges"<<std::endl;
    std::cout<<rangeX.lower<<" "<<rangeX.upper<<std::endl;
    std::cout<<rangeY.lower<<" "<<rangeY.upper<<std::endl;
    std::cout<<rangeZ.lower<<" "<<rangeZ.upper<<std::endl;
    std::cout<<rangeS.lower<<" "<<rangeS.upper<<std::endl;
}

QCPRange regularized(const QCPRange & range)
{
    if (range.lower==range.upper)
    {
        return QCPRange(-1,1);
    }
    else
    {
        return range;
    }
}

QCPRange Cloud::getXRange()
{
    return regularized(rangeX);
}
QCPRange Cloud::getYRange()
{
    return regularized(rangeY);
}
QCPRange Cloud::getZRange()
{
    return regularized(rangeZ);
}
QCPRange Cloud::getScalarFieldRange()
{
    return regularized(rangeS);
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
    return static_cast<int>(pts.size());
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

void Cloud::fit(Shape<Eigen::Vector4d>* model,int it,double xtol)
{
    model->fit(pts,it,xtol);
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
    if(!useCustomColor)
    {
        const double * colordata=(const double *)pts.data()+3;
        colors.resize(pts.size());
        gradient.colorize(colordata,range,colors.data(),static_cast<int>(pts.size()),4,false);
    }

    QByteArray bufferBytes;
    bufferBytes.resize(2 * 3 * ( static_cast<int>(pts.size()) ) * sizeof(float));
    float* vertices = reinterpret_cast<float*>(bufferBytes.data());

    for (int i=0; i<pts.size(); i++)
    {
        *vertices++ = pts[i][0];
        *vertices++ = pts[i][1];
        *vertices++ = pts[i][2];

        if(useCustomColor)
        {
            *vertices++ = qRed  (customColor)/255.0;
            *vertices++ = qGreen(customColor)/255.0;
            *vertices++ = qBlue (customColor)/255.0;
        }
        else
        {
            *vertices++ = qRed  (colors[i])/255.0;
            *vertices++ = qGreen(colors[i])/255.0;
            *vertices++ = qBlue (colors[i])/255.0;
        }
    }

    return bufferBytes;
}

QByteArray Cloud::toByteArray()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream ds(&buffer);

    ds<<pts;
    ds<<name;
    ds<<labelX;
    ds<<labelY;
    ds<<labelZ;
    ds<<labelS;
    ds<<rangeX.lower<<rangeX.upper;
    ds<<rangeY.lower<<rangeY.upper;
    ds<<rangeZ.lower<<rangeZ.upper;
    ds<<rangeS.lower<<rangeS.upper;

    buffer.close();

    return data;
}

void Cloud::fromByteArray(QByteArray data)
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream ds(&buffer);

    ds>>pts;
    ds>>name;
    ds>>labelX;
    ds>>labelY;
    ds>>labelZ;
    ds>>labelS;
    ds>>rangeX.lower>>rangeX.upper;
    ds>>rangeY.lower>>rangeY.upper;
    ds>>rangeZ.lower>>rangeZ.upper;
    ds>>rangeS.lower>>rangeS.upper;

    buffer.close();
}
