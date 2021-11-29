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

double NormCoeff(int l)//sqrt(4.0*M_PI/(2.0*l+1.0))
{
    double normCoeff[50]={
        3.54490770181103 ,
        2.04665341589298 ,
        1.5853309190424  ,
        1.33984917138136 ,
        1.18163590060368 ,
        1.06882988757711 ,
        0.983180498897453,
        0.915291232863769,
        0.859766405154849,
        0.813257601914384,//10
        0.773562279770524,
        0.739164352329539,
        0.708981540362206,
        0.682217805297659,
        0.658272834494235,
        0.636684218440811,
        0.617089223310564,
        0.599198765361096,
        0.582779236268695,
        0.567639525700435,//20
        0.553621571339722,
        0.540593356487222,
        0.528443639680801,
        0.517077931785487,
        0.506415385973005,
        0.496386365456349,
        0.486930521066815,
        0.477995257001177,
        0.469534495385779,
        0.461507673226313,//30
        0.453878921789977,
        0.446616390460453,
        0.439691685937372,
        0.433079404224641,
        0.426756737792835,
        0.420703144049946,
        0.414900064124362,
        0.409330683178595,
        0.403979725194596,
        0.398833276520707,//40
        0.393878633534559,
        0.389104170621042,
        0.384499225339374,
        0.380053998195465,
        0.375759464873791,
        0.371607299138725,
        0.367589804905589,
        0.363699856219805,
        0.359930844078646,
        0.356276629192371};//50

    return normCoeff[l];
}

double sph_solidHarmonicsR(double r,double theta,double phi,int l,int m)
{
    return NormCoeff(l)*std::pow(r,l)*sphericalHarmonicsY(theta,phi,l,m);
}

double sph_solidHarmonicsI(double r,double theta,double phi,int l,int m)
{
    return NormCoeff(l)*sphericalHarmonicsY(theta,phi,l,m)/std::pow(r,l);
}

double cart_solidHarmonicsR(double x,double y,double z,int l,int m)
{
    double r,theta,phi;
    cartesian_to_spherical(x,y,z,r,theta,phi);
    return sph_solidHarmonicsR(r,theta,phi,l,m);
}

double cart_solidHarmonicsI(double x,double y,double z,int l,int m)
{
    double r,theta,phi;
    cartesian_to_spherical(x,y,z,r,theta,phi);
    return sph_solidHarmonicsI(r,theta,phi,l,m);
}

void testHarmonics()
{
    std::cout<<sphericalHarmonicsY(0,0,0,0)<<" "<<1.0/sqrt(4*M_PI)<<std::endl;
}

void cartesian_to_spherical(double x,double y,double z,
                 double & r,double & theta,double & phi)
{
    r=std::hypot(x,y,z);
    theta=acos(z/r);
    phi=(x>0)?atan(y/x):(atan(y/x)+M_PI);
}
