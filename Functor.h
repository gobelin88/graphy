#include <unsupported/Eigen/NonLinearOptimization>
#include <Eigen/Dense>

#pragma once

using::Eigen::Matrix;
using::Eigen::MatrixXd;
using::Eigen::VectorXd;

//////////////////////////////////////////////////////////////////

/**
 *  @class Functor
 *  @brief Functor pour la résolution inverse.
 */
template<typename _Scalar>
struct Functor
{
  typedef _Scalar Scalar;
  typedef Matrix<Scalar,Eigen::Dynamic,1> InputType;
  typedef Matrix<Scalar,Eigen::Dynamic,1> ValueType;
  typedef Matrix<Scalar,Eigen::Dynamic,Eigen::Dynamic> JacobianType;

    /**
   * @brief Functor
   */
  Functor() : m_inputs(0), m_values(0) {}

  /**
   * @brief Functor
   * @param inputs
   * @param values
   */
  Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}

  int inputs() const { return m_inputs; }
  int values() const { return m_values; }

  void setInputs(int value){this->m_inputs=value;}
  void setValues(int value){this->m_values=value;}

  int m_inputs, m_values;

  // you should define that in the subclass :
//  void operator() (const InputType& x, ValueType* v, JacobianType* _j=0) const;
};

//////////////////////////////////////////////////////////////////
