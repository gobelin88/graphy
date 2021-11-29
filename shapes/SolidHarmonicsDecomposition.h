#pragma once

#include "shapes/Shape.hpp"
#include "../solidHarmonics/solidHarmonics.h"

/**
 * @class Plan
 * @brief Décrit un plan.
 */
class SolidHarmonicsDecomposition: public Shape<Eigen::Vector2d>
{
public:
    enum Mode
    {
        MODE_REGULAR,
        MODE_IRREGULAR,
    };

    SolidHarmonicsDecomposition(const std::vector<Eigen::Vector4d> & _dataPoints,
                                int n,
                                SolidHarmonicsDecomposition::Mode mode,
                                bool cart);
    ~SolidHarmonicsDecomposition();

    SolidHarmonicsDecomposition::Mode getMode();
    static int maxOrder();
    int order();
    void setC(int l,int m,double value);
    double getC(int l,int m)const;
    Eigen::MatrixXd getC();
    //Eigen::Vector4d delta(const Eigen::Vector4d& pt);
    Eigen::Vector2d delta(const Eigen::Vector2d& pt);
    //double at(double x,double y,double z);
    double at(int index);

    int nb_params();
    void setParams(const Eigen::VectorXd& _p);
    const Eigen::VectorXd& getParams();

    void preCalcSolidsHarmonics();
    void guessDecomposition();

private:
    std::vector<Eigen::VectorXd> precalYs;
    const std::vector<Eigen::Vector4d> & dataPoints;
    double getYs(int index,int l,int m);
    int getLMIndex(int l,int m)const;

    Eigen::VectorXd p;
    unsigned int n;
    Mode mode;
    bool cart;
};
