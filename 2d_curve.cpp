#include "2d_curve.h"

//#include "kdtree_flann.h"

Curve2D::Curve2D()
{
    this->legendname="none";
    this->type=GRAPH;
    clear();
}

Curve2D::Curve2D(const Eigen::VectorXd& y,
                 QString legendname,
                 CurveType type)
{
    this->x=buildX(y.size());
    this->y=y;
    this->legendname=legendname;
    this->type=type;
    clear();
}

Curve2D::Curve2D(const Eigen::VectorXd& x,
                 const Eigen::VectorXd& y,
                 QString legendname,
                 CurveType type)
{
    clear();
    this->x=x;
    this->y=y;
    this->legendname=legendname;
    this->type=type;
}

Curve2D::Curve2D(const Eigen::VectorXd& x,
                 const Eigen::VectorXd& y,
                 const Eigen::VectorXd& s,
                 QString legendname,
                 CurveType type)
{
    clear();
    this->x=x;
    this->y=y;
    this->s=s;
    this->legendname=legendname;
    this->type=type;
}

void Curve2D::clear()
{
    a.resize(0);
    s.resize(0);
    l.clear();

    ptrGraph=nullptr;
    ptrCurve=nullptr;
}

const Eigen::VectorXd & Curve2D::getX()const
{
    return x;
}
const Eigen::VectorXd & Curve2D::getY()const
{
    return y;
}
QVector<double> Curve2D::getQX()const
{
    return toQVector(x);
}

QVector<double> Curve2D::getQY()const
{
    return toQVector(y);
}

void Curve2D::setAlphaField(const Eigen::VectorXd& _a)
{
    this->a=_a;
}

Eigen::VectorXd Curve2D::getAlphaField()const
{
    return a;
}
QVector<double> Curve2D::getQAlphaField()const
{
    return toQVector(a);
}

void Curve2D::setScalarField(const Eigen::VectorXd& _s)
{
    this->s=_s;
}
const Eigen::VectorXd & Curve2D::getScalarField()const
{
    return s;
}
QVector<double> Curve2D::getQScalarField()const
{
    return toQVector(s);
}

void Curve2D::setLabelsField(const QVector<QString>& _l)
{
    this->l=_l;
}

QVector<QString> Curve2D::getLabelsField()const
{
    return  l;
}

Eigen::VectorXd Curve2D::buildX(int sz)
{
    Eigen::VectorXd qv(sz);
    for (int i=0; i<sz; i++)
    {
        qv[i]=static_cast<double>(i);
    }
    return qv;
}

Eigen::VectorXd Curve2D::getLinX(int n) const
{
    double min=x.minCoeff();
    double max=x.maxCoeff();

    Eigen::VectorXd xlin(n);

    for (int i=0; i<n; i++)
    {
        xlin[i]=i*(max-min)/n+min;
    }

    return xlin;
}

void Curve2D::getLinXY(int n,Eigen::VectorXd& valuesX,Eigen::VectorXd& valuesY)
{
    double minX=x.minCoeff();
    double maxX=x.maxCoeff();
    double minY=y.minCoeff();
    double maxY=y.maxCoeff();

    valuesX.resize(n*n);
    valuesY.resize(n*n);
    int id=0;
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
        {
            valuesX[id]=i*(maxX-minX)/n+minX;
            valuesY[id]=j*(maxY-minY)/n+minY;
            id++;
        }
    }
}

unsigned int getTn(unsigned int order)
{
    unsigned int sum=0;

    for (unsigned int i=0; i<=order; i++)
    {
        for (unsigned int j=0; j<=order; j++)
        {
            if ((i+j)<=order)
            {
                sum+=1;
            }
        }
    }

    return sum;
}

VectorXd getT(double x,double y,unsigned int order)
{
    VectorXd V(getTn(order));
    unsigned int id=0;
    for (unsigned int i=0; i<=order; i++)
    {
        for (unsigned int j=0; j<=order; j++)
        {
            if ((i+j)<=order)
            {
                V[static_cast<int>(id++)]=std::pow(x,i)*std::pow(y,j);
            }
        }
    }
    return V;
}

QString Curve2D::getPolynome2VString(const Eigen::VectorXd& C,unsigned int order)
{
    QString name;
    unsigned int id=0;
    for (unsigned int i=0; i<=order; i++)
    {
        for (unsigned int j=0; j<=order; j++)
        {
            if ((i+j)<=order)
            {
                QString monome;

                if (i==0 && j!=0)
                {
                    if (j==1)
                    {
                        monome=QString("Y");
                    }
                    else
                    {
                        monome=QString("Y^%1").arg(j);
                    }
                }
                else if (i!=0 && j==0)
                {
                    if (i==1)
                    {
                        monome=QString("X");
                    }
                    else
                    {
                        monome=QString("X^%1").arg(i);
                    }
                }
                else if (i!=0 && j!=0)
                {
                    if (i==1)
                    {
                        monome=QString("X");
                    }
                    else
                    {
                        monome=QString("X^%1").arg(i);
                    }

                    if (j==1)
                    {
                        monome+=QString(" Y");
                    }
                    else
                    {
                        monome+=QString(" Y^%1").arg(j);
                    }
                }


                if (i==0 && j==0)
                {
                    name+= QString("%1").arg(C[id]);
                }
                else if (C[id]<0)
                {
                    name+= QString("%1 %2").arg(C[id]).arg(monome);
                }
                else if (C[id]>0)
                {
                    name+= QString("+%1 %2").arg(C[id]).arg(monome);
                }

                id++;
            }
        }
    }
    return name;
}

void Curve2D::knnMeanDistance(int knn)
{
    s.resize(x.rows());

    Eigen::MatrixXd datapoints(2,s.size());

    for (int i=0; i<datapoints.cols(); i++)
    {
        datapoints(0,i)=x[i];
        datapoints(1,i)=y[i];
    }

    kdt::KDTreed kdtree(datapoints);
    kdtree.build();

    kdt::KDTreed::Matrix dists;
    kdt::KDTreed::MatrixI idx;

    kdtree.query(datapoints, knn, idx, dists);

    for(int i=0;i<s.rows();i++)
    {
        s[i]=dists.col(i).mean();
    }
}

Eigen::VectorXd Curve2D::fit2d(unsigned int order)
{
    Eigen::VectorXd C;

    //(1) Build A matrix -----------------------------------------------------
    MatrixXd A(x.size(),getTn(order));
    for (int i=0; i<A.rows(); i++)
    {
        VectorXd V=getT(x[i],y[i],order);
        for (int j=0; j<A.cols(); j++)
        {
            A(i,j)=V[j];
        }
    }

    //(2) Build B matrix
    Eigen::VectorXd B(s.size());
    for (int i=0; i<A.rows(); i++)
    {
        B[i]=s[i];
    }

    //Solve A C = B for C
    C=A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(B);

    return C;
}

double Curve2D::at(const Eigen::VectorXd& C,double valuex,double valuey,unsigned int order)
{
    return C.dot(getT(valuex,valuey,order));
}

Eigen::VectorXd Curve2D::at(const Eigen::VectorXd& C,Eigen::VectorXd valuex,Eigen::VectorXd valuey,unsigned int order)
{
    Eigen::VectorXd valuesScalar(valuex.size());

    for (int i=0; i<valuex.size(); i++)
    {
        valuesScalar[i]=at(C,valuex[i],valuey[i],order);
    }

    return valuesScalar;
}

Eigen::VectorXd Curve2D::fit(unsigned int order,bool derivate)
{
    if(!derivate)
    {
        Eigen::MatrixXd X=Eigen::MatrixXd::Zero(x.size(),order+1);

        for (unsigned int n=0; n<order+1; ++n)
        {
            for (int m=0; m<x.size(); ++m)
            {
                X(m,n)=(n==0)?1.0:((n==1)?x[m]:std::pow(x[m],n));
            }
        }

        Eigen::VectorXd Y(x.size());
        for (int i=0; i<x.size(); i++)
        {
            Y[i]=y[i];
        }

        return X.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(Y);
    }
    else
    {
        Eigen::MatrixXd X=Eigen::MatrixXd::Zero(x.size()*2,order+1);

        for (unsigned int n=0; n<order+1; ++n)
        {
            for (int m=0; m<x.size(); ++m)
            {
                X(m,n)=(n==0)?1.0:((n==1)?x[m]:std::pow(x[m],n));
            }
            for (int m=0; m<x.size(); ++m)
            {
                X(x.size()+m,n)=((n==0)?0.0:((n==1)?1:n*std::pow(x[m],n-1)));
            }
        }

        Eigen::VectorXd Y(x.size()*2);
        for (int i=0; i<x.size(); i++)
        {
            Y[i]=y[i];
            if( (i-1)>=0 && (i+1)<x.size() )
            {
                Y[x.size()+i]=(y[i+1]-y[i-1])/(x[i+1]-x[i-1]);
            }
            else
            {
                Y[x.size()+i]=0.0;
            }
        }

        return X.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(Y);
    }
}

std::vector<Eigen::Vector2d> Curve2D::getPoints()
{
    std::vector<Eigen::Vector2d> points(x.rows());
    for (int i=0; i<x.rows(); ++i)
    {
        points[i]=Eigen::Vector2d(x[i],y[i]);
    }
    return points;
}

void Curve2D::fit(Shape<Eigen::Vector2d>* model)
{
    model->fit(getPoints(),10000);
}


double Curve2D::getRms()
{
    return sqrt(y.cwiseAbs2().mean());
}

std::complex<double> Curve2D::guessMainFrequencyPhaseModule(double & mainFrequency,double & a0)
{
    //Curve2D fft=getFFT(Curve2D::BLACKMAN,1.0,true,true,false);
    //return fft.getX()[fft.getMaxIndex()];

    //faudrait reechantillonner le signal en fait...pfuu
    //Eigen::VectorXd x=getLinX(x.size());

    Eigen::VectorXcd s_in=y;

    double minX=x.minCoeff();
    double maxX=x.maxCoeff();

    Eigen::VectorXcd s_out=MyFFT::getFFT(s_in,
                  MyFFT::WindowsType::RECTANGLE,
                  true,
                  true,
                  false,
                  false);

    unsigned int N=s_out.rows();

    Eigen::VectorXd s_mod=s_out.segment(1,N/2).cwiseAbs();

    int maxindex;
    s_mod.maxCoeff(&maxindex);
    a0=s_out[0].real()/sqrt(N);

    mainFrequency=double(maxindex+1)/N*N/(maxX-minX);

    return s_out[maxindex+1]/sqrt(N)*2.0;//+s_out[s_out.size()-maxindex];//Todo
}

Eigen::Vector2d Curve2D::getBarycenter()
{
    double xm=0.0,ym=0.0;
    for (int i=0; i<x.size(); i++)
    {
        xm+=x[i];
        ym+=y[i];
    }
    return Eigen::Vector2d(xm/x.size(),ym/y.size());
}

uint Curve2D::getMaxIndex()
{
    uint index;
    y.maxCoeff(&index);
    return index;
}



double Curve2D::at(const Eigen::VectorXd& A, double valuex)
{
    double X=1.0;
    double val=0.0;
    for (int i=0; i<A.rows(); ++i)
    {
        val+=X*A[i];
        X*=valuex;
    }
    return val;
}

Eigen::VectorXd Curve2D::at(const Eigen::VectorXd& A, Eigen::VectorXd values)
{
    Eigen::VectorXd _y(values.size());
    for (int i=0; i<_y.size(); i++)
    {
        _y[i]=at(A,values[i]);
    }
    return _y;
}

QString Curve2D::getPolynomeString(const Eigen::VectorXd& C,unsigned int order)
{
    QString name;
    for (unsigned int i=0; i<=order; i++)
    {
        QString monome;

        if (i==1)
        {
            monome=QString("*X");
        }
        else
        {
            monome=QString("*X^%1").arg(i);
        }

        if (i==0)
        {
            name+= QString("%1").arg(C[i]);
        }
        else if (C[i]<0)
        {
            name+= QString("%1 %2").arg(C[i]).arg(monome);
        }
        else if (C[i]>0)
        {
            name+= QString("+%1 %2").arg(C[i]).arg(monome);
        }
    }
    return name;
}

void Curve2D::operator=(const Curve2D& other)
{
    this->x=other.x;
    this->y=other.y;
    this->s=other.s;
    this->a=other.a;
    this->l=other.l;
    this->legendname=other.legendname;
    this->type=other.type;
    this->style=other.style;
    this->mapParams=other.mapParams;

    this->ptrGraph=other.ptrGraph;
    this->ptrCurve=other.ptrCurve;
}

void Curve2D::fromByteArray(QByteArray data)
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream ds(&buffer);

    int typei;
    ds>>x>>y>>s>>a;
    ds>>l;
    ds>>legendname;
    ds>>typei;
    style.read(ds);
    mapParams.read(ds);

    type=static_cast<CurveType>(typei);

    buffer.close();

    std::cout<<"toByteArray size="<<data.size()<<std::endl;
}

QByteArray Curve2D::toByteArray()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream ds(&buffer);

    ds<<x<<y<<s<<a;
    ds<<l;
    ds<<legendname;
    ds<<static_cast<int>(type);
    style.write(ds);
    mapParams.write(ds);

    buffer.close();

    std::cout<<"toByteArray size="<<data.size()<<std::endl;
    return data;
}

QString Curve2D::getLegend() const
{
    return legendname;
}

void Curve2D::setLegend(QString _legendname)
{
    this->legendname=_legendname;
}

Curve2D::CurveType Curve2D::getType()const
{
    return type;
}

