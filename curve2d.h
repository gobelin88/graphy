#ifndef CURVE2D_H
#define CURVE2D_H

#include <Eigen/Dense>
#include <QVector>
#include "ShapeFit.h"
#include "qcustomplot.h"

class Curve2D
{
public:
    Curve2D();
    Curve2D(const QVector<double>& y,QString legendname);
    Curve2D(const QVector<double>& x,const QVector<double>& y,QString legendname="no name");

    QVector<double> getX()const;
    QVector<double> getY()const;

    //Optional scalar field
    void setScalarField(QVector<double>& scalarField);
    QVector<double> getScalarField()const;

    //Fit a model
    void fit(Shape<Eigen::Vector2d>* model);
    Eigen::Vector2d getBarycenter();

    //Fit a Polynome
    Eigen::VectorXd fit(unsigned int order);
    double at(const Eigen::VectorXd& A,double x);
    QVector<double> at(const Eigen::VectorXd& A,QVector<double> values);

    //Legend name
    QString name()const;
    void setName(QString legendname);

    //misc
    QVector<double> getLinX(int n);
    static QVector<double> buildXQVector(unsigned int sz);

    void operator=(const Curve2D& other);
private:
    QVector<double> x;
    QVector<double> y;
    QVector<double> s;
    QString legendname;
};

#endif // CURVE2D_H
