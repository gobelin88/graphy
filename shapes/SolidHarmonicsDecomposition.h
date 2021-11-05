#pragma once

#include "shapes/Shape.hpp"
#include "../solidHarmonics/solidHarmonics.h"

/**
 * @class Plan
 * @brief Décrit un plan.
 */
class SolidHarmonicsDecomposition: public Shape<Eigen::Vector4d>
{
public:
    enum Mode
    {
        MODE_REGULAR,
        MODE_IRREGULAR,
    };

    SolidHarmonicsDecomposition(int n,SolidHarmonicsDecomposition::Mode mode);
    ~SolidHarmonicsDecomposition();

    SolidHarmonicsDecomposition::Mode getMode();
    static int maxOrder();
    int order();
    void setC(int l,int m,double value);
    double getC(int l,int m)const;
    Eigen::MatrixXd getC();
    Eigen::Vector4d delta(const Eigen::Vector4d& pt);
    double at(double x,double y,double z);
    int nb_params();
    void setParams(const Eigen::VectorXd& _p);
    const Eigen::VectorXd& getParams();

    void guessDecomposition(const std::vector<Eigen::Vector4d> & pt);

private:
    Eigen::VectorXd p;
    unsigned int n;
    Mode mode;
};
