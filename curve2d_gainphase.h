#ifndef CURVE2D_GAINPHASE_H
#define CURVE2D_GAINPHASE_H

#include <Eigen/Dense>
#include <QVector>
#include "ShapeFit.h"

class Curve2D_GainPhase
{
public:
    Curve2D_GainPhase();
    Curve2D_GainPhase(const QVector<double> & f,const QVector<double> & g,const QVector<double> & p,QString legendpname="no name",QString legendgname="no name");

    QVector<double> getF()const {return f;}
    QVector<double> getG()const {return g;}
    QVector<double> getP()const {return p;}

    QVector<double> getLinF(int n)const
    {
        double min=f[0];
        double max=f[f.size()-1];

        std::cout<<max<<" "<<min<<std::endl;

        QVector<double> flin(n);

        for(int i=0;i<n;i++)
        {
            flin[i]=i*(max-min)/n+min;
        }

        return flin;
    }

    void fit(Shape<Eigen::Vector3d> * model);

    QVector<double> at(const Eigen::VectorXd & A,QVector<double> values);

    QString gname()const {return legendgname;}
    void setgName(QString legendgname){this->legendgname=legendgname;}

    QString pname()const {return legendpname;}
    void setpName(QString legendpname){this->legendpname=legendpname;}

    void operator=(const Curve2D_GainPhase & other);

    static QVector<double> buildFQVector(unsigned int sz);

private:
    QVector<double> f,g,p;
    QString legendgname;
    QString legendpname;
};

#endif // CURVE2D_H
