#include "curve2d.h"

Curve2D::Curve2D()
{
    this->x.clear();
    this->y.clear();
    this->legendname="none";
    this->type=GRAPH;
}

Curve2D::Curve2D(const QVector<double>& y,
                 QString legendname,
                 CurveType type)
{
    this->x=buildXQVector(y.size());
    this->y=y;
    this->legendname=legendname;
    this->type=type;
}

Curve2D::Curve2D(const QVector<double>& x,
                 const QVector<double>& y,
                 QString legendname,
                 CurveType type)
{
    this->x=x;
    this->y=y;
    this->legendname=legendname;
    this->type=type;
}

Curve2D::Curve2D(const QVector<double>& x,
                 const QVector<double>& y,
                 const QVector<double>& s,
                 QString legendname,
                 CurveType type)
{
    this->x=x;
    this->y=y;
    this->s=s;
    this->legendname=legendname;
    this->type=type;
}

QVector<double> Curve2D::getX()const
{
    return x;
}
QVector<double> Curve2D::getY()const
{
    return y;
}

void Curve2D::setScalarField(const QVector<double>& s)
{
    this->s=s;
}
QVector<double> Curve2D::getScalarField()const
{
    return s;
}

QVector<double> Curve2D::buildXQVector(int sz)
{
    QVector<double> qv(sz,0.0);
    for (int i=0; i<sz; i++)
    {
        qv[i]=(double)i;
    }
    return qv;
}

QVector<double> Curve2D::getLinX(int n)
{
    double min=*std::min_element(x.constBegin(), x.constEnd());
    double max=*std::max_element(x.constBegin(), x.constEnd());

    QVector<double> xlin(n);

    for (int i=0; i<n; i++)
    {
        xlin[i]=i*(max-min)/n+min;
    }

    return xlin;
}

void Curve2D::getLinXY(int n,QVector<double>& valuesX,QVector<double>& valuesY)
{
    double minX=*std::min_element(x.constBegin(), x.constEnd());
    double maxX=*std::max_element(x.constBegin(), x.constEnd());
    double minY=*std::min_element(x.constBegin(), x.constEnd());
    double maxY=*std::max_element(x.constBegin(), x.constEnd());

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

QVector<double> Curve2D::at(const Eigen::VectorXd& C,QVector<double> valuex,QVector<double> valuey,unsigned int order)
{
    QVector<double> valuesScalar(valuex.size());

    for (int i=0; i<valuex.size(); i++)
    {
        valuesScalar[i]=at(C,valuex[i],valuey[i],order);
    }

    return valuesScalar;
}

Eigen::VectorXd Curve2D::fit(unsigned int order)
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

void Curve2D::fit(Shape<Eigen::Vector2d>* model)
{
    std::vector<Eigen::Vector2d> points;
    for (int m=0; m<x.size(); ++m)
    {
        points.push_back(Eigen::Vector2d(x[m],y[m]));
    }

    model->fit(points,10000);
}


double Curve2D::getRms()
{
    double sum_squares=0.0;
    for (int i=0; i<y.size(); i++)
    {
        sum_squares+=y[i]*y[i];
    }
    return sqrt(sum_squares/y.size());
}

double Curve2D::guessMainFrequency()
{
    Curve2D fft=getFFT();
    return fft.getX()[fft.getMaxIndex()];
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
    return std::distance(y.constBegin(),std::max_element(y.constBegin(), y.constEnd()));
}

Curve2D Curve2D::getFFT()
{
    std::vector<double> data_y=y.toStdVector();
    std::vector< double > time=std::vector< double >(data_y.size()/2, 0);
    for (unsigned int i=0; i<time.size(); i++)
    {
        time[i]=i*1.0/data_y.size();
    }

    FIR win;
    win.getHannCoef(uint(data_y.size()));

    for (unsigned int i=0; i<uint(data_y.size()); i++)
    {
        data_y[i]*=win.at(i);
    }

    Eigen::FFT<double> fft;
    fft.SetFlag(Eigen::FFT<double>::HalfSpectrum);
    fft.SetFlag(Eigen::FFT<double>::Unscaled);

    std::vector< std::complex<double> > fft_data_cplx;
    fft.fwd(fft_data_cplx,data_y);
    fft_data_cplx.pop_back();

    std::vector< double > fft_data_module(fft_data_cplx.size());
    for (unsigned int k=0; k<fft_data_cplx.size(); k++)
    {
        fft_data_module[k]=20*log10(std::abs(fft_data_cplx[k]));
    }

    return Curve2D(QVector<double>::fromStdVector(time),
                   QVector<double>::fromStdVector(fft_data_module),
                   QString("FFT %1").arg(getLegend()),Curve2D::GRAPH);
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

QVector<double> Curve2D::at(const Eigen::VectorXd& A,QVector<double> values)
{
    QVector<double> y(values.size());
    for (int i=0; i<y.size(); i++)
    {
        y[i]=at(A,values[i]);
    }
    return y;
}

QString Curve2D::getPolynomeString(const Eigen::VectorXd& C,unsigned int order)
{
    QString name;
    for (unsigned int i=0; i<=order; i++)
    {
        QString monome;

        if (i==1)
        {
            monome=QString("X");
        }
        else
        {
            monome=QString("X^%1").arg(i);
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
    this->legendname=other.legendname;
    this->type=other.type;
}

QString Curve2D::getLegend() const
{
    return legendname;
}

void Curve2D::setLegend(QString legendname)
{
    this->legendname=legendname;
}

Curve2D::CurveType Curve2D::getType()const
{
    return type;
}
