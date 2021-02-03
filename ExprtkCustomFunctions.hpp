//#include <cstdio>
// #include <cstdlib>
// #include <ctime>
// #include <string>

#include "exprtk/exprtk.hpp"
#include <qmath.h>

#ifndef EXPRTKCUSTOMFUNCTIONS_HPP
#define EXPRTKCUSTOMFUNCTIONS_HPP

//Noise management


template <typename T>
struct uniformFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    uniformFunction()
        : exprtk::ifunction<T>(2)
    {
        noise_uniform=std::uniform_real<double>(0,1.0);
        //exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& a, const T& b)
    {
        return (b-a)*noise_uniform(generator)+a;
    }

    std::default_random_engine generator;
    std::uniform_real<double> noise_uniform;
};

template <typename T>
struct normalFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    normalFunction()
        : exprtk::ifunction<T>(2)
    {
        noise_normal=std::normal_distribution<double>(0,1.0);
        //exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& mu, const T& sigma)
    {
        return sigma*noise_normal(generator)+mu;
    }

    std::default_random_engine generator;
    std::normal_distribution<double> noise_normal;
};

template <typename T>
struct linFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    linFunction()
        : exprtk::ifunction<T>(2)
    {
        //exprtk::disable_has_side_effects(*this);
        rows=nullptr;
        current_rows=nullptr;
    }

    void setNumberOfRowsPtr(T * rows)
    {
        this->rows=rows;
    }
    void setCurrentRowPtr(T * current_rows)
    {
        this->current_rows=current_rows;
    }

    inline T operator()(const T& a, const T& b)
    {
        if(current_rows && rows)
        {
            return (b-a)*(*current_rows)/(*rows-1.0)+a;
        }
        else
        {
            return 0.0;
        }
    }

    T * rows;
    T * current_rows;
};

template <typename T>
struct gammaFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    gammaFunction()
        : exprtk::ifunction<T>(1)
    {
        exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& s)
    {
        //Hadamar weierstrass
        //      const double euler_mascheroni=0.5772156649015328606;
        //      T value= exp(-euler_mascheroni*s)/s;
        //      for(int k=1;k<100000;k++)
        //      {
        //          T s_k=s/double(k);
        //          value*=exp(s_k)/(1.0+s_k);
        //      }
        //      return value;

        //Lancoz approx
        const double p [] = {676.5203681218851
                             ,-1259.1392167224028
                             ,771.32342877765313
                             ,-176.61502916214059
                             ,12.507343278686905
                             ,-0.13857109526572012
                             ,9.9843695780195716e-6
                             ,1.5056327351493116e-7};

        if (s.real() < 0.5)
        {
            return M_PI / (sin(M_PI * s) * this->operator()(1.0 - s));// Reflection formula
        }
        else
        {
            T x = 0.99999999999980993;
            for(int i=0;i<8;i++)
            {
                x =x + p[i] / ( (s-1.0) + double(i) + 1.0);
            }
            T t = (s-1.0) + 8.0 - 0.5;
            return sqrt(2.0 * M_PI) * std::pow(t,(s-1.0) + 0.5) * exp(-t) * x;
        }
    }


};


template <typename T>
struct zetaFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    zetaFunction()
        : exprtk::ifunction<T>(1)
    {
        exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& s)
    {
        if(s.real()>0.5)
        {
            ///by eta function
            T eta=1.0;
            double precision=1e-3;
            T deta;
            unsigned int n=2.0;
            do
            {
               deta=std::pow(n,-s);
               eta+=(n%2==0)?-deta:deta;
               n++;
            }
            while(std::norm(deta)>(precision*precision));
            return eta/(1.0-std::pow(2.0,1.0-s));

            //----------------
        }
        else
        {
            return 2.0*std::pow(2.0*M_PI,s-1.0)*sin(s*M_PI*0.5)*m_gamma(1.0-s)*this->operator()(1.0-s);
        }
    }

    void d(double k,unsigned )
    {

    }

    gammaFunction<T> m_gamma;

};

#endif // EXPRTKCUSTOMFUNCTIONS_HPP
