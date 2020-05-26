#include "curve2d.h"

Curve2D::Curve2D()
{
    this->legendname="none";
    this->type=GRAPH;
}

Curve2D::Curve2D(const Eigen::VectorXd& y,
                 QString legendname,
                 CurveType type)
{
    this->x=buildX(y.size());
    this->y=y;
    this->legendname=legendname;
    this->type=type;
}

Curve2D::Curve2D(const Eigen::VectorXd& x,
                 const Eigen::VectorXd& y,
                 QString legendname,
                 CurveType type)
{
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
    this->x=x;
    this->y=y;
    this->s=s;
    this->legendname=legendname;
    this->type=type;
}

Eigen::VectorXd Curve2D::getX()const
{
    return x;
}
Eigen::VectorXd Curve2D::getY()const
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

void Curve2D::setScalarField(const Eigen::VectorXd& s)
{
    this->s=s;
}
Eigen::VectorXd Curve2D::getScalarField()const
{
    return s;
}
QVector<double> Curve2D::getQScalarField()const
{
    return toQVector(s);
}

void Curve2D::setLabelsField(const QVector<QString>& l)
{
    this->l=l;
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

Eigen::VectorXd Curve2D::getLinX(int n)
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
    uint index;
    y.maxCoeff(&index);
    return index;
}

Curve2D Curve2D::getFFT()
{
    std::vector<double> data_y=toStdVector(y);
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

    return Curve2D(fromStdVector(time),
                   fromStdVector(fft_data_module),
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

Eigen::VectorXd Curve2D::at(const Eigen::VectorXd& A, Eigen::VectorXd values)
{
    Eigen::VectorXd y(values.size());
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
    this->l=other.l;
    this->legendname=other.legendname;
    this->type=other.type;
    this->qcpData=other.qcpData;
}

void Curve2D::fromQCP(const QCPCurve* other)
{
    x.resize(other->data()->size());
    y.resize(other->data()->size());
    auto it =other->data()->begin();
    int i=0;
    while (it!=other->data()->end())
    {
        x[i]=it->key;
        y[i]=it->value;
        it++;
        i++;
    }

    this->s=fromQVector(other->getScalarField());
    this->l=other->getLabelField();
    this->legendname=other->name();
    this->type=Curve2D::CURVE;
    this->qcpData.mLineStyle=other->lineStyle();
    this->qcpData.mScatterStyle=other->scatterStyle().shape();
    this->qcpData.pen=other->pen();
    this->qcpData.brush=other->brush();
}

void Curve2D::fromQCP(const QCPGraph* other)
{
    x.resize(other->data()->size());
    y.resize(other->data()->size());
    auto it =other->data()->begin();
    int i=0;
    while (it!=other->data()->end())
    {
        x[i]=it->key;
        y[i]=it->value;
        it++;
        i++;
    }

    this->s=fromQVector(other->getScalarField());
    this->l=other->getLabelField();
    this->legendname=other->name();
    this->type=Curve2D::GRAPH;
    this->qcpData.mLineStyle=other->lineStyle();
    this->qcpData.mScatterStyle=other->scatterStyle().shape();
    this->qcpData.pen=other->pen();
    this->qcpData.brush=other->brush();
}

QCPGraph* Curve2D::toQCPGraph(QCustomPlot* plot)const
{
    QCPGraph* pgraph = new QCPGraph(plot->xAxis, plot->yAxis);

    pgraph->setScalarField(getQScalarField());
    pgraph->setLabelField(getLabelsField());
    pgraph->setLineStyle(static_cast<QCPGraph::LineStyle>(qcpData.mLineStyle));
    pgraph->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(qcpData.mScatterStyle), 10));
    pgraph->setSelectable(QCP::stWhole);
    pgraph->setSelectionDecorator(nullptr);
    pgraph->setName(getLegend());
    pgraph->setData(getQX(),getQY());
    pgraph->setPen(qcpData.pen);
    pgraph->setBrush(qcpData.brush);

    if (getLabelsField().size()>0)
    {
        buildX(getY().size());
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(getQX(), getLabelsField() );
        plot->xAxis->setTicker(textTicker);
        plot->xAxis->setTickLabelColor(Qt::black);
        plot->xAxis->setLabelColor(Qt::black);
    }

    return pgraph;
}

QCPCurve* Curve2D::toQCPCurve(QCustomPlot* plot)const
{
    QCPCurve* pcurve = new QCPCurve(plot->xAxis, plot->yAxis);

    pcurve->setScalarField(getQScalarField());
    pcurve->setLabelField(getLabelsField());
    pcurve->setLineStyle(static_cast<QCPCurve::LineStyle>(qcpData.mLineStyle));
    pcurve->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(qcpData.mScatterStyle), 10));
    pcurve->setSelectable(QCP::stWhole);
    pcurve->setSelectionDecorator(nullptr);
    pcurve->setName(getLegend());
    pcurve->setData(getQX(),getQY());
    pcurve->setPen(qcpData.pen);
    pcurve->setBrush(qcpData.brush);

    if (getLabelsField().size()>0)
    {
        buildX(getY().size());
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(getQX(), getLabelsField() );
        plot->xAxis->setTicker(textTicker);
        plot->xAxis->setTickLabelColor(Qt::black);
        plot->xAxis->setLabelColor(Qt::black);
    }

    if (getScalarField().size()>0)
    {
        pcurve->setColorScale(new QCPColorScale(plot));
        pcurve->getColorScale()->setType(QCPAxis::atRight);
        pcurve->getColorScale()->setDataRange(pcurve->getScalarFieldRange());
        pcurve->getColorScale()->setGradient(pcurve->getGradient());
        pcurve->setLineStyle(QCPCurve::lsNone);
        pcurve->setScatterStyle(QCPScatterStyle::ScatterShape::ssDisc);

        plot->plotLayout()->addElement(0, plot->plotLayout()->columnCount(), pcurve->getColorScale());
    }

    return pcurve;
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
