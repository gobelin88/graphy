#include "complex_hack.h"

bool operator>=(std::complex<double> a,std::complex<double> b)
{
    if(std::abs(a)>=std::abs(b))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool operator>(std::complex<double> a,std::complex<double> b)
{
    if(std::abs(a)>std::abs(b))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool operator<=(std::complex<double> a,std::complex<double> b)
{
    if(std::abs(a)<=std::abs(b))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool operator<(std::complex<double> a,std::complex<double> b)
{
    if(std::abs(a)<std::abs(b))
    {
        return true;
    }
    else
    {
        return false;
    }
}
