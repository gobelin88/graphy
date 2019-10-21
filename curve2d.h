#ifndef CURVE2D_H
#define CURVE2D_H

#include <Eigen/Dense>
#include <QVector>
#include "ShapeFit.h"

class Curve2D
{
public:
    Curve2D();
    Curve2D(const QVector<double> & y,QString legendname);
    Curve2D(const QVector<double> & x,const QVector<double> & y,QString legendname="no name");

    QVector<double> getX()const {return x;}
    QVector<double> getY()const {return y;}

    QVector<double> getLinX(int n)const
    {
        double min=x[0];
        double max=x[x.size()-1];

        std::cout<<max<<" "<<min<<std::endl;

        QVector<double> xlin(n);

        for(int i=0;i<n;i++)
        {
            xlin[i]=i*(max-min)/n+min;
        }

        return xlin;
    }

    void fit(Shape<Eigen::Vector2d> * model);

    Eigen::VectorXd fit(unsigned int order);
    double at(const Eigen::VectorXd & A,double x);
    QVector<double> at(const Eigen::VectorXd & A,QVector<double> values);

    QString name()const {return legendname;}
    void setName(QString legendname){this->legendname=legendname;}

    Eigen::Vector2d getBarycenter();

    void operator=(const Curve2D & other);

    static QVector<double> buildXQVector(unsigned int sz);

private:
    QVector<double> x;
    QVector<double> y;
    QString legendname;
};

#endif // CURVE2D_H
