#ifndef CURVE2D_GAINPHASE_H
#define CURVE2D_GAINPHASE_H

#include <Eigen/Dense>
#include "ShapeFit.h"
#include "curve2d.h"

class Curve2DComplex
{
public:
    enum FitMode
    {
        CARTESIAN,
        POLAR
    };

    Curve2DComplex();

    Curve2DComplex(const Eigen::VectorXcd & y,
                   QString legend);

    Curve2DComplex(const Eigen::VectorXd & x,
                   const Eigen::VectorXcd & y,
                   QString legend);

    Curve2DComplex(const Eigen::VectorXd & x,
                   const Eigen::VectorXd & ya,
                   const Eigen::VectorXd & yb,
                   FitMode mode,
                   QString legend);

    Curve2D getModulesCurve()const;
    Curve2D getArgumentsCurve()const;

    void fromModuleArgument(Eigen::VectorXd modules,Eigen::VectorXd arguments);
    void fromRealImag(Eigen::VectorXd reals,Eigen::VectorXd imags);
    void fromModuleArgument(Eigen::VectorXd x,Eigen::VectorXd modules,Eigen::VectorXd arguments);
    void fromRealImag(Eigen::VectorXd x,Eigen::VectorXd reals,Eigen::VectorXd imags);
    void fit(Shape<Eigen::Vector3d>* model,FitMode mode);
    void fitRealImag(Shape<Eigen::Vector3d>* model);
    void fitModuleArgument(Shape<Eigen::Vector3d>* model);

    double getModule(int i)const;
    double getArgument(int i)const;
    Eigen::VectorXd getModules()const;
    Eigen::VectorXd getArguments()const;

    QString getLegend();
    void setLegend(QString legend);

    void operator=(const Curve2DComplex & other);
private:
    Eigen::VectorXd x;
    Eigen::VectorXcd y;
    QString legend;
};
#endif // CURVE2D_H
