#ifndef CURVE2D_H
#define CURVE2D_H

#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
#include <QVector>
#include "ShapeFit.h"
#include "qcustomplot.h"
#include "FIR.h"
#include "tabledata.h"

class Curve2D
{
public:
    enum CurveType
    {
        GRAPH=0,
        CURVE=1
    };

    Curve2D();

    Curve2D(const Eigen::VectorXd& y,
            QString legendname,
            CurveType type);

    Curve2D(const Eigen::VectorXd& x,
            const Eigen::VectorXd& y,
            QString legendname,
            CurveType type);

    Curve2D(const Eigen::VectorXd& x,
            const Eigen::VectorXd& y,
            const Eigen::VectorXd& s,
            QString legendname,
            CurveType type);

    Eigen::VectorXd getX()const;
    Eigen::VectorXd getY()const;
    QVector<double> getQX()const;
    QVector<double> getQY()const;

    //Optional scalar field
    void setScalarField(const Eigen::VectorXd& s);
    Eigen::VectorXd getScalarField()const;
    QVector<double> getQScalarField()const;

    //Fit a model
    void fit(Shape<Eigen::Vector2d>* model);

    //Fit a Polynome
    Eigen::VectorXd fit(unsigned int order);
    double at(const Eigen::VectorXd& A,double valuex);
    Eigen::VectorXd at(const Eigen::VectorXd& A,Eigen::VectorXd values);
    static QString getPolynomeString(const Eigen::VectorXd& C,unsigned int order);

    //Fit a 2d Polynome
    Eigen::VectorXd fit2d(unsigned int order);
    double at(const Eigen::VectorXd& C,double valuex,double valuey,unsigned int order);
    Eigen::VectorXd at(const Eigen::VectorXd& C, Eigen::VectorXd valuex, Eigen::VectorXd valuey, unsigned int order);
    static QString getPolynome2VString(const Eigen::VectorXd& C,unsigned int order);

    //Legend name
    QString getLegend()const;
    void setLegend(QString legendname);

    //Type
    CurveType getType() const;

    //misc
    Curve2D getFFT();
    double getRms();
    double guessMainFrequency();
    uint getMaxIndex();
    Eigen::Vector2d getBarycenter();
    Eigen::VectorXd getLinX(int n);
    void getLinXY(int n, Eigen::VectorXd& valuesX, Eigen::VectorXd& valuesY);

    static Eigen::VectorXd buildX(int sz);

    void operator=(const Curve2D& other);
private:
    Eigen::VectorXd x;
    Eigen::VectorXd y;
    Eigen::VectorXd s;
    QString legendname;
    CurveType type;
};

#endif // CURVE2D_H
