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
        return (b-a)*(*current_rows)/(*rows)+a;
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
      const double euler_mascheroni=0.5772156649015328606;
      T value= exp(-euler_mascheroni*s)/s;
      for(int k=1;k<100000;k++)
      {
          T s_k=s/double(k);
          value*=exp(s_k)/(1.0+s_k);
      }
      return value;
   }
};

#endif // EXPRTKCUSTOMFUNCTIONS_HPP
