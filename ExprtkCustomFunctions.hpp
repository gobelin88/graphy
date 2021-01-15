//#include <cstdio>
// #include <cstdlib>
// #include <ctime>
// #include <string>

#include "exprtk/exprtk.hpp"

#ifndef EXPRTKCUSTOMFUNCTIONS_HPP
#define EXPRTKCUSTOMFUNCTIONS_HPP

//Noise management
static std::default_random_engine generator;
static std::uniform_real<double> noise_uniform(0.0,1.0);
static std::normal_distribution<double> noise_normal(0.0,1.0);

template <typename T>
struct uniformFunction : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   uniformFunction()
   : exprtk::ifunction<T>(2)
   {
       //exprtk::disable_has_side_effects(*this);
   }

   inline T operator()(const T& a, const T& b)
   {
      return (b-a)*noise_uniform(generator)+a;
   }
};

template <typename T>
struct normalFunction : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   normalFunction()
   : exprtk::ifunction<T>(2)
   {
       //exprtk::disable_has_side_effects(*this);
   }

   inline T operator()(const T& mu, const T& sigma)
   {
      return sigma*noise_normal(generator)+mu;
   }
};

#endif // EXPRTKCUSTOMFUNCTIONS_HPP
