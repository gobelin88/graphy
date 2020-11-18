#include <complex>

#ifndef COMPLEX_HACK_H
#define COMPLEX_HACK_H
bool operator>=(std::complex<double> a,std::complex<double> b);
bool operator>(std::complex<double> a,std::complex<double> b);
bool operator<=(std::complex<double> a,std::complex<double> b);
bool operator<(std::complex<double> a,std::complex<double> b);

#endif // COMPLEX_HACK_H
