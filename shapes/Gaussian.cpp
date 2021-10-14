#include "shapes/Gaussian.h"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

Gaussian::Gaussian(double S,double M,double K)
{
    p.resize(3);
    setS(S);
    setM(M);
    setK(K);
}

Eigen::Vector2d Gaussian::delta(const Eigen::Vector2d& pt)
{
    return Eigen::Vector2d (pt[1]-at(pt[0]),pt[1]-at(pt[0]));
}

double Gaussian::getS()const
{
    return p[0];
}
double Gaussian::getM()const
{
    return p[1];
}
double Gaussian::getK()const
{
    return p[2];
}

void  Gaussian::setS(double R)
{
    p[0]=R;
}
void  Gaussian::setM(double L)
{
    p[1]=L;
}
void  Gaussian::setK(double K)
{
    p[2]=K;
}

double Gaussian::at(double x)const
{
    double S=getS();
    double M=getM();
    double K=getK();
    double I=(x-M)/(S);
    return K/(S*sqrt(2*M_PI))*exp(-0.5*I*I);
}

Eigen::VectorXd Gaussian::at(Eigen::VectorXd f)const
{
    Eigen::VectorXd y(f.size());

    for (int i=0; i<f.size(); i++)
    {
        y[i]=at(f[i]);
    }
    return y;
}

int Gaussian::nb_params()
{
    return 3;
}
void Gaussian::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}
const Eigen::VectorXd& Gaussian::getParams()
{
    return p;
}
void Gaussian::setParameter(QString parameterName,double value)
{
    if(parameterName==QString("S")){setS(value);}
    else if(parameterName==QString("M")){setM(value);}
    else if(parameterName==QString("K")){setK(value);}
    else
    {
        std::cout<<"Bad parameter name : "<<parameterName.toStdString()<<std::endl;
    }
}
double Gaussian::getParameter(QString parameterName)
{
    if(parameterName==QString("S")){return getS();}
    else if(parameterName==QString("M")){return getM();}
    else if(parameterName==QString("K")){return getK();}
    else
    {
        std::cout<<"Bad parameter name : "<<parameterName.toStdString()<<std::endl;
        return 0;
    }
}

void Gaussian::setFixedParameter(QString parameterName,bool fixed)
{

}
