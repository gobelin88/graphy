#ifndef CURVE2D_H
#define CURVE2D_H

#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
#include <QVector>
#include "ShapeFit.h"
#include "qcustomplot.h"
#include "FIR.h"

class Curve2D
{
public:
    enum CurveType
    {
        GRAPH=0,
        CURVE=1
    };

    Curve2D();

    Curve2D(const QVector<double>& y,
            QString legendname,
            CurveType type);

    Curve2D(const QVector<double>& x,
            const QVector<double>& y,
            QString legendname,
            CurveType type);

    Curve2D(const QVector<double>& x,
            const QVector<double>& y,
            const QVector<double>& s,
            QString legendname,
            CurveType type);

    QVector<double> getX()const;
    QVector<double> getY()const;

    //Optional scalar field
    void setScalarField(const QVector<double>& s);
    QVector<double> getScalarField()const;

    //Fit a model
    void fit(Shape<Eigen::Vector2d>* model);

    //Fit a Polynome
    Eigen::VectorXd fit(unsigned int order);
    double at(const Eigen::VectorXd& A,double valuex);
    QVector<double> at(const Eigen::VectorXd& A,QVector<double> values);
    static QString getPolynomeString(const Eigen::VectorXd& C,unsigned int order);

    //Fit a 2d Polynome
    Eigen::VectorXd fit2d(unsigned int order);
    double at(const Eigen::VectorXd& C,double valuex,double valuey,unsigned int order);
    QVector<double> at(const Eigen::VectorXd& C,QVector<double> valuex,QVector<double> valuey,unsigned int order);
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
    QVector<double> getLinX(int n);
    void getLinXY(int n,QVector<double>& valuesX,QVector<double>& valuesY);

    static QVector<double> buildXQVector(int sz);

    void operator=(const Curve2D& other);
private:
    QVector<double> x;
    QVector<double> y;
    QVector<double> s;
    QString legendname;
    CurveType type;
};

#endif // CURVE2D_H
