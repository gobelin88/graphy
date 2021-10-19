#include "Sinusoide.h"


Sinusoide::Sinusoide(double A,double F,double P,double C,
          bool fixedA,bool fixedF,bool fixedP,bool fixedC
          )
{
    set(A,F,P,C,fixedA,fixedF,fixedP,fixedC);
}

void Sinusoide::set(double A,double F,double P,double C,
         bool fixedA,bool fixedF,bool fixedP,bool fixedC
         )
{
    this->fixedA=fixedA;
    this->fixedF=fixedF;
    this->fixedP=fixedP;
    this->fixedC=fixedC;

    int N=0;
    if(!fixedA)
    {
        idA=N;
        N++;
    }
    if(!fixedF)
    {
        idF=N;
        N++;
    }
    if(!fixedP)
    {
        idP=N;
        N++;
    }
    if(!fixedC)
    {
        idC=N;
        N++;
    }

    std::cout<<"Sinusoide N="<<N<<std::endl;
    std::cout<<"idA="<<idA<<std::endl;
    std::cout<<"idF="<<idF<<std::endl;
    std::cout<<"idP="<<idP<<std::endl;
    std::cout<<"idC="<<idC<<std::endl;

    p.resize(N);
    p_fixed.resize(4);

    setA(A);
    setF(F);
    setP(P);
    setC(C);
}

Eigen::Vector2d Sinusoide::delta(const Eigen::Vector2d& pt)
{
    return Eigen::Vector2d (pt[1]-at(pt[0]),0);
}

double Sinusoide::getA()const
{
    return (fixedA)?p_fixed[0]: p[idA];
}
double Sinusoide::getF()const
{
    return (fixedF)?p_fixed[1]: p[idF];
}
double Sinusoide::getP()const
{
    return (fixedP)?p_fixed[2]: p[idP];
}
double Sinusoide::getC()const
{
    return (fixedC)?p_fixed[3]: p[idC];
}

void  Sinusoide::setA(double A)
{
    if(fixedA){p_fixed[0]=A;}else{p[idA]=A;}
}
void  Sinusoide::setF(double F)
{
    if(fixedF){p_fixed[1]=F;}else{p[idF]=F;}
}
void  Sinusoide::setP(double P)
{
    if(fixedP){p_fixed[2]=P;}else{p[idP]=P;}
}
void  Sinusoide::setC(double C)
{
    if(fixedC){p_fixed[3]=C;}else{p[idC]=C;}
}

void Sinusoide::regularized()
{
    double A=getA();
    double F=getF();
    double P=getP();

    if (A<0)
    {
        A=-A;
        P+=M_PI;
    }
    if (F<0)
    {
        F=-F;
        P+=M_PI;
    }
    P=std::fmod(P,2*M_PI);
    if(P<0){P+=2*M_PI;}

    setA(A);
    setF(F);
    setP(P);
}

int Sinusoide::nb_params()
{
    return p.rows();
}
void Sinusoide::setParams(const Eigen::VectorXd& _p)
{
    this->p=_p;
}
const Eigen::VectorXd& Sinusoide::getParams()
{
    return p;
}

//ModelCurveInterface
double Sinusoide::at(double t)
{
    double A=getA();
    double F=getF();
    double P=getP();
    double C=getC();
    //std::cout<<t<<" "<<A<<" "<<F<<" "<<P<<" "<<A* std::sin(2*M_PI*F*t+P)<<std::endl;
    return C+A*std::sin(2*M_PI*F*t+P);
}

Eigen::VectorXd Sinusoide::at(Eigen::VectorXd t)
{
    Eigen::VectorXd y(t.size());

    for (int i=0; i<t.size(); i++)
    {
        y[i]=at(t[i]);
    }
    return y;
}

void Sinusoide::setParameter(QString parameterName,double value)
{
    if(parameterName==QString("A")){setA(value);}
    else if(parameterName==QString("F")){setF(value);}
    else if(parameterName==QString("P")){setP(value);}
    else if(parameterName==QString("C")){setC(value);}
    else
    {
        std::cout<<"Bad parameter name : "<<parameterName.toStdString()<<std::endl;
    }
}

double Sinusoide::getParameter(QString parameterName)
{
    if(parameterName==QString("A")){return getA();}
    else if(parameterName==QString("F")){return getF();}
    else if(parameterName==QString("P")){return getP();}
    else if(parameterName==QString("C")){return getC();}
    else
    {
        std::cout<<"Bad parameter name : "<<parameterName.toStdString()<<std::endl;
        return 0;
    }
}

void Sinusoide::setFixedParameter(QString parameterName,bool fixed)
{
    bool tfixedA=fixedA,tfixedF=fixedF,tfixedP=fixedP,tfixedC=fixedC;
    if(parameterName==QString("A")){tfixedA=fixed;}
    else if(parameterName==QString("F")){tfixedF=fixed;}
    else if(parameterName==QString("P")){tfixedP=fixed;}
    else if(parameterName==QString("C")){tfixedC=fixed;}
    else
    {
        std::cout<<"Bad parameter name : "<<parameterName.toStdString()<<std::endl;
    }

    set(getA(),getF(),getP(),getC(),
        tfixedA,tfixedF,tfixedP,tfixedC);
}


