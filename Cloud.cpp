#include "Cloud.h"

void Cloud::init()
{
    cloudType=TYPE_POINTS;
    useCustomColor=false;
    customColor=qRgb(0,0,0);
    setGradientPreset(QCPColorGradient::gpPolar);

    setName(QString("Cloud : %4(%1,%2,%3)")
                       .arg(getLabelX())
                       .arg(getLabelY())
                       .arg(getLabelZ())
                       .arg(getLabelS()));
}

Cloud::Cloud()
{
    if(sizeof(DataCloudNode)%sizeof(double)!=0)
    {
        std::cout<<"Bad sizeof(DataNode)="<<sizeof(DataCloudNode)<<" sizeof(double)="<<sizeof(double)<<std::endl;
    }

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
    nodes.resize(plist.size());
    for (int i=0; i<plist.size(); i++)
    {
        nodes[i].position=plist[i];
        nodes[i].scalar=0;
        nodes[i].attitude=Eigen::Quaterniond(1,0,0,0);
    }

    calcRanges();

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS .clear();

    init();
}

Cloud::Cloud(std::vector<Eigen::Vector3d> plist,
      const Eigen::VectorXd& scalarField,
      QString labelX,
      QString labelY,
      QString labelZ,
      QString labelS)
{
    if(scalarField.rows()==plist.size())
    {
        nodes.resize(plist.size());
        for (int i=0; i<plist.size(); i++)
        {
            nodes[i].position=plist[i];
            nodes[i].scalar=scalarField[i];
            nodes[i].attitude=Eigen::Quaterniond(1,0,0,0);
        }
    }
    else
    {
        std::cout<<"Error : Bad scalarfield size="<<scalarField.rows()<<" expected="<<plist.size()<<std::endl;
    }

    calcRanges();

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS =labelS;

    init();
}

Cloud::Cloud(std::vector<Eigen::Vector4d> plist,
      QString labelX,
      QString labelY,
      QString labelZ,
      QString labelS)
{
    nodes.resize(plist.size());
    for (int i=0; i<plist.size(); i++)
    {
        nodes[i].position=plist[i].head<3>();
        nodes[i].scalar=plist[i][3];
        nodes[i].attitude=Eigen::Quaterniond(1,0,0,0);
    }

    calcRanges();

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;
    this->labelS =labelS;

    init();
}

Cloud::Cloud(const Eigen::VectorXd& x,
             const Eigen::VectorXd& y,
             const Eigen::VectorXd& z,
             QString labelX,
             QString labelY,
             QString labelZ)
{
    if(x.rows()==y.rows() && x.rows()==z.rows())
    {
        nodes.resize(x.size());
        for (int i=0; i<x.size(); i++)
        {
             nodes[i].position=Eigen::Vector3d(x[i],y[i],z[i]);
             nodes[i].scalar=0;
             nodes[i].attitude=Eigen::Quaterniond(1,0,0,0);
        }
    }
    else
    {
        std::cout<<"Error : Bad y,z size="<<y.rows()<<" "<<z.rows()<<" expected="<<x.size()<<std::endl;
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
          const std::vector<Eigen::Quaterniond> & attitudes,
          QString labelX,
          QString labelY,
          QString labelZ)
{
    if(x.rows()==y.rows() && x.rows()==z.rows() && x.rows()==attitudes.size())
    {
        for (int i=0; i<x.size(); i++)
        {
            nodes.resize(x.size());
            for (int i=0; i<x.size(); i++)
            {
                 nodes[i].position=Eigen::Vector3d(x[i],y[i],z[i]);
                 nodes[i].scalar=0.0;
                 nodes[i].attitude=attitudes[i];
            }
        }


    }
    else
    {
        std::cout<<"Error : Bad y,z or attitudes size="<<y.rows()<<" "<<z.rows()<<" "<<attitudes.size()<<" expected="<<x.size()<<std::endl;
    }

    calcRanges();

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;

    init();
    cloudType=TYPE_TRANSFORMS;
}

Cloud::Cloud(const Eigen::VectorXd& x,
            const Eigen::VectorXd& y,
            const Eigen::VectorXd& z,
            const Eigen::VectorXd& qw,
            const Eigen::VectorXd& qx,
            const Eigen::VectorXd& qy,
            const Eigen::VectorXd& qz,
          QString labelX,
          QString labelY,
          QString labelZ)
{
    if(x.rows()==y.rows() && x.rows()==z.rows() &&
       x.rows()==qw.rows() && x.rows()==qx.rows() && x.rows()==qy.rows() && x.rows()==qz.rows())
    {
        for (int i=0; i<x.size(); i++)
        {
            nodes.resize(x.size());
            for (int i=0; i<x.size(); i++)
            {
                 nodes[i].position=Eigen::Vector3d(x[i],y[i],z[i]);
                 nodes[i].scalar=0.0;
                 nodes[i].attitude=Eigen::Quaterniond(qw[i],qx[i],qy[i],qz[i]);
            }
        }


    }
    else
    {
        std::cout<<"Error : Bad y,z,attitudes size="<<y.rows()<<" "<<z.rows()<<" "<<qw.rows()<<" expected="<<x.size()<<std::endl;
    }

    calcRanges();

    this->labelX =labelX;
    this->labelY =labelY;
    this->labelZ =labelZ;

    init();
    cloudType=TYPE_TRANSFORMS;
}

Cloud::Cloud(const Eigen::VectorXd& x,
             const Eigen::VectorXd& y,
             const Eigen::VectorXd& z,
             const Eigen::VectorXd& scalarField,
             QString labelX,
             QString labelY,
             QString labelZ,
             QString labelS)
{
    if(x.rows()==y.rows() && x.rows()==z.rows() && x.rows()==scalarField.rows())
    {
        for (int i=0; i<x.size(); i++)
        {
            nodes.resize(x.size());
            for (int i=0; i<x.size(); i++)
            {
                 nodes[i].position=Eigen::Vector3d(x[i],y[i],z[i]);
                 nodes[i].scalar=scalarField[i];
                 nodes[i].attitude=Eigen::Quaterniond(1,0,0,0);
            }
        }
    }
    else
    {
        std::cout<<"Error : Bad y,z,s size="<<y.rows()<<" "<<z.rows()<<" "<<scalarField.rows()<<" expected="<<x.size()<<std::endl;
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

    for(int i=0;i<nodes.size();i++)
    {
        for(int j=0;j<4;j++)
        {
            if(j!=3)
            {
                if(nodes[i].position[j]<min[j]){min[j]=nodes[i].position[j];}
                if(nodes[i].position[j]>max[j]){max[j]=nodes[i].position[j];}
            }
            else
            {
                if(nodes[i].scalar<min[j]){min[j]=nodes[i].scalar;}
                if(nodes[i].scalar>max[j]){max[j]=nodes[i].scalar;}
            }
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
    return rangeS;
}

std::vector<Eigen::Vector4d> Cloud::positionsAndScalarField()const
{
    std::vector<Eigen::Vector4d> positionsAndScalarField(nodes.size());

    for(int i=0;i<nodes.size();i++)
    {
        positionsAndScalarField[i]=Eigen::Vector4d(
                nodes[i].position[0],
                nodes[i].position[1],
                nodes[i].position[2],
                nodes[i].scalar);
    }

    return positionsAndScalarField;
}

std::vector<Eigen::Vector3d> Cloud::positions()const
{
    std::vector<Eigen::Vector3d> positions(nodes.size());

    for(int i=0;i<nodes.size();i++)
    {
        positions[i]=nodes[i].position;
    }

    return positions;
}

int Cloud::size()const
{
    return static_cast<int>(nodes.size());
}

void Cloud::operator=(const Cloud& other)
{
    this->nodes=other.nodes;
    this->cloudType=other.cloudType;
    this->useCustomColor=other.useCustomColor;

    this->labelX  = other.labelX;
    this->labelY  = other.labelY;
    this->labelZ  = other.labelZ;
    this->labelS  = other.labelS;
    this->name    = other.name;
}

Eigen::Vector3d Cloud::getCenter()
{
    return Eigen::Vector3d(rangeX.center(),rangeY.center(),rangeZ.center());
}

void Cloud::getBarycenterAndBoundingRadius(
        Eigen::Vector3d & Barycenter,
        double & boundingRadius)
{
    Barycenter=Eigen::Vector3d(0,0,0);
    boundingRadius=0.0;

    if (nodes.size()>0)
    {
        Barycenter=getBarycenter();


        for (int i=0; i<nodes.size(); i++)
        {
            double radius=(nodes[i].position-Barycenter).norm();

            if (radius>boundingRadius)
            {
                boundingRadius=radius;
            }
        }
    }
}

Eigen::Vector3d Cloud::getBarycenter()
{
    Eigen::Vector3d Barycenter(0,0,0);

    if (nodes.size()>0)
    {
        for (int i=0; i<nodes.size(); i++)
        {
            Barycenter+=nodes[i].position;
        }
        Barycenter/=nodes.size();
    }
    return Barycenter;
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

void Cloud::fit(Shape<Eigen::Vector4d>* model,int it,double xtol)
{
    model->fit(positionsAndScalarField(),it,xtol);
}

void Cloud::fit(Shape<Eigen::Vector3d>* model,int it,double xtol)
{
    model->fit(positions(),it,xtol);
}

void Cloud::project(Shape<Eigen::Vector3d>* model)
{
    for(int i=0;i<nodes.size();i++)
    {
        nodes[i].position-=model->delta(nodes[i].position);
    }
}

void Cloud::subSample(unsigned int nbPoints)
{
    int nbpoints_to_remove=(int)nodes.size()-(int)nbPoints;

    for(int i=0;i<nbpoints_to_remove;i++)
    {
        int random_index=rand()%nodes.size();
        nodes.erase (nodes.begin()+random_index);
    }
    calcRanges();
}

void Cloud::move(const Eigen::Matrix3d & R, const Eigen::Vector3d & T,double scale)
{
    for(int i=0;i<nodes.size();i++)
    {
        nodes[i].position=scale*(R*nodes[i].position+T);
    }
    calcRanges();
}

QByteArray Cloud::getBuffer(QCPRange range)
{
    std::vector<QRgb> colors;

    if(!useCustomColor)
    {        
        const double * colordata=&nodes[0].scalar;//(const double *)nodes.data()+7;
        colors.resize(nodes.size());
        unsigned int dataFactor=sizeof(DataCloudNode)/sizeof(double);
        gradient.colorize(colordata,range,colors.data(),static_cast<int>(nodes.size()),dataFactor,false);
    }

    QByteArray bufferBytes;
    bufferBytes.resize(2 * 3 * ( static_cast<int>(nodes.size()) ) * sizeof(float));
    float* vertices = reinterpret_cast<float*>(bufferBytes.data());

    for (int i=0; i<nodes.size(); i++)
    {
        *vertices++ = nodes[i].position[0];
        *vertices++ = nodes[i].position[1];
        *vertices++ = nodes[i].position[2];

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

QByteArray Cloud::getBufferTransfos(double size)
{
    QByteArray bufferBytes;
    bufferBytes.resize(6 * 2 * 3 * ( static_cast<int>(nodes.size()) ) * sizeof(float));
    float* vertices = reinterpret_cast<float*>(bufferBytes.data());

    for (int i=0; i<nodes.size(); i++)
    {
        //X
        *vertices++ = nodes[i].position[0];
        *vertices++ = nodes[i].position[1];
        *vertices++ = nodes[i].position[2];
        *vertices++ = 255.0;
        *vertices++ = 0;
        *vertices++ = 0;
        *vertices++ = nodes[i].position[0]+size;
        *vertices++ = nodes[i].position[1];
        *vertices++ = nodes[i].position[2];
        *vertices++ = 255.0;
        *vertices++ = 0;
        *vertices++ = 0;

        //Y
        *vertices++ = nodes[i].position[0];
        *vertices++ = nodes[i].position[1];
        *vertices++ = nodes[i].position[2];
        *vertices++ = 0;
        *vertices++ = 255;
        *vertices++ = 0;
        *vertices++ = nodes[i].position[0];
        *vertices++ = nodes[i].position[1]+size;
        *vertices++ = nodes[i].position[2];
        *vertices++ = 0;
        *vertices++ = 255;
        *vertices++ = 0;

        //Z
        *vertices++ = nodes[i].position[0];
        *vertices++ = nodes[i].position[1];
        *vertices++ = nodes[i].position[2];
        *vertices++ = 0;
        *vertices++ = 0;
        *vertices++ = 0;
        *vertices++ = nodes[i].position[0];
        *vertices++ = nodes[i].position[1];
        *vertices++ = nodes[i].position[2]+size;
        *vertices++ = 0;
        *vertices++ = 0;
        *vertices++ = 255;
    }

    return bufferBytes;
}

QByteArray Cloud::toByteArray()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream ds(&buffer);

    ds<<nodes;
    ds<<static_cast<int>(cloudType);
    ds<<useCustomColor;
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

    ds>>nodes;
    int cloudTypei;ds>>cloudTypei;cloudType=(Type)(cloudTypei);
    ds>>useCustomColor;
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

QDataStream & operator>>(QDataStream & ds, std::vector<DataCloudNode> & nodes)
{
    size_t sz;
    ds>>sz;
    nodes.resize(sz);
    for(int i=0;i<nodes.size();i++)
    {
        ds>>nodes[i].position;
        ds>>nodes[i].attitude;
        ds>>nodes[i].scalar;
    }
    return ds;
}
QDataStream & operator<<(QDataStream & ds,std::vector<DataCloudNode> & nodes)
{
    ds<<nodes.size();
    for(int i=0;i<nodes.size();i++)
    {
        ds<<nodes[i].position;
        ds<<nodes[i].attitude;
        ds<<nodes[i].scalar;
    }
    return ds;
}
