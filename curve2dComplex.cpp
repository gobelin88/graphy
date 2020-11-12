#include "curve2dComplex.h"

Curve2DComplex::Curve2DComplex()
{

}

Curve2DComplex::Curve2DComplex(const Eigen::VectorXcd & y,
               QString legend)
{
    this->legend=legend;
    this->x=Eigen::VectorXd::LinSpaced(y.rows(),0,y.rows()-1);
    this->y=y;
}

Curve2DComplex::Curve2DComplex(const Eigen::VectorXd & x,
               const Eigen::VectorXcd & y,
               QString legend)
{
    this->legend=legend;
    this->x=x;
    this->y=y;
}

Curve2DComplex::Curve2DComplex(const Eigen::VectorXd & x,
               const Eigen::VectorXd & ya,
               const Eigen::VectorXd & yb,
               FitMode mode,
               QString legend)
{
    this->legend=legend;
    if(mode==CARTESIAN)
    {
        fromRealImag(x,ya,yb);
    }
    else
    {
        fromModuleArgument(x,ya,yb);
    }
}

Curve2D Curve2DComplex::getModulesCurve()const
{
    return Curve2D(x,getModules(),QString("Modules : %1").arg(legend),Curve2D::GRAPH);
}

Curve2D Curve2DComplex::getArgumentsCurve()const
{
    return Curve2D(x,getArguments(),QString("Phases : %1").arg(legend),Curve2D::GRAPH);
}

Curve2D Curve2DComplex::getRealCurve()const
{
    return Curve2D(x,getReal(),QString("Real : %1").arg(legend),Curve2D::GRAPH);
}

Curve2D Curve2DComplex::getImagCurve()const
{
    return Curve2D(x,getImag(),QString("Imag : %1").arg(legend),Curve2D::GRAPH);
}

void Curve2DComplex::fromModuleArgument(Eigen::VectorXd modules,Eigen::VectorXd arguments)
{
    y.resize(modules.rows());

    for(int i=0;i<y.rows();i++)
    {
        y[i]=std::polar(modules[i],arguments[i]*M_PI/180);
    }

    this->x=Eigen::VectorXd::LinSpaced(y.rows(),0,y.rows()-1);
}

void Curve2DComplex::fromRealImag(Eigen::VectorXd reals,Eigen::VectorXd imags)
{
    y.resize(reals.rows());

    for(int i=0;i<y.rows();i++)
    {
        y[i]=std::complex<double>(reals[i],imags[i]);
    }

    this->x=Eigen::VectorXd::LinSpaced(y.rows(),0,y.rows()-1);
}

void Curve2DComplex::fromModuleArgument(Eigen::VectorXd x,Eigen::VectorXd modules,Eigen::VectorXd arguments)
{
    y.resize(modules.rows());

    for(int i=0;i<y.rows();i++)
    {
        y[i]=std::polar(modules[i],arguments[i]*M_PI/180);
    }

    this->x=x;
}

void Curve2DComplex::fromRealImag(Eigen::VectorXd x,Eigen::VectorXd reals,Eigen::VectorXd imags)
{
    y.resize(reals.rows());

    for(int i=0;i<y.rows();i++)
    {
        y[i]=std::complex<double>(reals[i],imags[i]);
    }

    this->x=x;
}
void Curve2DComplex::fit(Shape<Eigen::Vector3d>* model,FitMode mode)
{
    if(mode==CARTESIAN)
    {
        fitRealImag(model);
    }
    else
    {
        fitModuleArgument(model);
    }
}

void Curve2DComplex::fitRealImag(Shape<Eigen::Vector3d>* model)
{
    std::vector<Eigen::Vector3d> points;
    for (int k=0; k<x.rows(); ++k)
    {
        points.push_back(Eigen::Vector3d(x[k],y[k].real(),y[k].imag()));
    }

    model->fit(points,10000);
}

void Curve2DComplex::fitModuleArgument(Shape<Eigen::Vector3d>* model)
{
    std::vector<Eigen::Vector3d> points;
    for (int k=0; k<x.rows(); ++k)
    {
        points.push_back(Eigen::Vector3d(x[k],getModule(k),getArgument(k)));
    }

    model->fit(points,10000);
}

double Curve2DComplex::getModule(int i)const
{
    return std::abs(y[i]);
}

double Curve2DComplex::getArgument(int i)const
{
    return std::arg(y[i])*180/M_PI;
}

Eigen::VectorXd Curve2DComplex::getModules()const
{
    Eigen::VectorXd modules(y.rows());
    for(int i=0;i<y.rows();i++)
    {
        modules[i]=getModule(i);
    }
    return modules;
}

Eigen::VectorXd Curve2DComplex::getArguments()const
{
    Eigen::VectorXd arguments(y.rows());
    for(int i=0;i<y.rows();i++)
    {
        arguments[i]=getArgument(i);
    }
    return arguments;
}

Eigen::VectorXd Curve2DComplex::getReal()const
{
    Eigen::VectorXd reals(y.rows());
    for(int i=0;i<y.rows();i++)
    {
        reals[i]=y[i].real();
    }
    return reals;
}

Eigen::VectorXd Curve2DComplex::getImag()const
{
    Eigen::VectorXd imags(y.rows());
    for(int i=0;i<y.rows();i++)
    {
        imags[i]=y[i].imag();
    }
    return imags;
}

QString Curve2DComplex::getLegend()
{
    return legend;
}

void Curve2DComplex::setLegend(QString legend)
{
    this->legend=legend;
}

void Curve2DComplex::operator=(const Curve2DComplex & other)
{
    x=other.x;
    y=other.y;
    legend=other.legend;
}
