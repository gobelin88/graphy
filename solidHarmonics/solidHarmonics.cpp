#include "solidHarmonics.h"

#include <iostream>

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double factorielle(double n)
{
    double result=1.0;
    for(int i=2;i<=n;i++)
    {
        result*=i;
    }
    return result;
}

double legendrePolynomeP(double z,int l,int m)
{
    return std::assoc_legendre(l,m,z);
}

double sphericalHarmonicsY(double theta,double phi,int l,int m)//version reelle
{
    if(std::abs(m)<=l)
    {
        if(m<0)
        {
            return std::sph_legendre(l,m,theta)*sin(std::abs(m)*phi);
        }
        else if(m>0)
        {
            return std::sph_legendre(l,m,theta)*cos(m*phi);
        }
        else
        {
            return std::sph_legendre(l,m,theta);
        }
    }
    else
    {
        return 0;
    }

    /*if(std::abs(m)<=l)
    {
        if(m<0)
        {
            return sqrt(2)*sqrt((2.0*l+1)/(4*M_PI)*factorielle(l-std::abs(m))/factorielle(l+std::abs(m)))*legendrePolynomeP(cos(theta),l,std::abs(m))*sin(std::abs(m)*phi);
        }
        else if(m==0)
        {
            return sqrt((2.0*l+1)/(4*M_PI))*P(cos(theta),l,0);
        }
        else
        {
            return sqrt(2)*sqrt((2.0*l+1)/(4*M_PI)*factorielle(l-m)/factorielle(l+m))*legendrePolynomeP(cos(theta),l,m)*cos(m*phi);
        }

    }
    else
    {
        return 0;
    }*/
    //return sqrt(2.0*factorielle(l-m)/factorielle(l+m))*P(cos(theta),m,l)*exp(i*m*phi);
}

double solidHarmonicsR(double x,double y,double z,int l,int m)
{
    double r=sqrt(x*x+y*y+z*z);
    double theta=acos(z/r);
    double phi=(x>0)?atan(y/x):(atan(y/x)+M_PI);
    return sqrt(4.0*M_PI/(2.0*l+1.0))*std::pow(r,l)*sphericalHarmonicsY(theta,phi,l,m);
}

double solidHarmonicsI(double x,double y,double z,int l,int m)
{
    double r=sqrt(x*x+y*y+z*z);
    double theta=acos(z/r);
    double phi=(x>0)?atan(y/x):(atan(y/x)+M_PI);
    return sqrt(4.0*M_PI/(2.0*l+1.0))*sphericalHarmonicsY(theta,phi,l,m)/std::pow(r,l);
}

void testHarmonics()
{
    std::cout<<sphericalHarmonicsY(0,0,0,0)<<" "<<1.0/sqrt(4*M_PI)<<std::endl;
}
