#include <complex>

double factorielle(double n);
double legendrePolynomeP(double z,int l,int m);
double sphericalHarmonicsY(double theta,double phi,int l,int m);

double sph_solidHarmonicsR(double r,double theta,double phi,int l,int m);
double sph_solidHarmonicsI(double r,double theta,double phi,int l,int m);

double cart_solidHarmonicsR(double x,double y,double z,int l,int m);
double cart_solidHarmonicsI(double x,double y,double z,int l,int m);

void cartesian_to_spherical(double x,double y,double z,
                   double & r,double & theta,double & phi);

void testHarmonics();
