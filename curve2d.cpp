#include "curve2d.h"

Curve2D::Curve2D()
{
    this->x.clear();
    this->y.clear();
    this->legendname="no name";
}

Curve2D::Curve2D(const QVector<double>& y, QString legendname)
{
    this->x=buildXQVector(y.size());
    this->y=y;
    this->legendname=legendname;
}

Curve2D::Curve2D(const QVector<double>& x, const QVector<double>& y, QString legendname)
{
    this->x=x;
    this->y=y;
    this->legendname=legendname;
}

QVector<double> Curve2D::getX()const
{
    return x;
}
QVector<double> Curve2D::getY()const
{
    return y;
}

void Curve2D::setScalarField(QVector<double>& scalarField)
{
    this->s=scalarField;
}
QVector<double> Curve2D::getScalarField()const
{
    return s;
}

QVector<double> Curve2D::buildXQVector(unsigned int sz)
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
    double min=x[0];
    double max=x[x.size()-1];

    std::cout<<max<<" "<<min<<std::endl;

    QVector<double> xlin(n);

    for (int i=0; i<n; i++)
    {
        xlin[i]=i*(max-min)/n+min;
    }

    return xlin;
}

Eigen::VectorXd Curve2D::fit(unsigned int order)
{
    Eigen::MatrixXd X=Eigen::MatrixXd::Zero(x.size(),order);

    for (unsigned int n=0; n<order; ++n)
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

double Curve2D::at(const Eigen::VectorXd& A,double value)
{
    double X=1.0;
    double val=0.0;
    for (int i=0; i<A.rows(); ++i)
    {
        val+=X*A[i];
        X*=value;
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

void Curve2D::operator=(const Curve2D& other)
{
    this->x=other.x;
    this->y=other.y;
    this->s=other.s;
    this->legendname=other.legendname;
}

QString Curve2D::name()const
{
    return legendname;
}
void Curve2D::setName(QString legendname)
{
    this->legendname=legendname;
}
