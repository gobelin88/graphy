#ifndef CURVE2D_GAINPHASE_H
#define CURVE2D_GAINPHASE_H

#include <Eigen/Dense>
#include <QVector>
#include "ShapeFit.h"
#include "tabledata.h"

class Curve2D_GainPhase
{
public:
    Curve2D_GainPhase();
    Curve2D_GainPhase(const Eigen::VectorXd& f,
                      const Eigen::VectorXd& g,
                      const Eigen::VectorXd& p,
                      QString legendpname="no name",
                      QString legendgname="no name");

    Eigen::VectorXd getF()const
    {
        return f;
    }
    Eigen::VectorXd getG()const
    {
        return g;
    }
    Eigen::VectorXd getP()const
    {
        return p;
    }

    QVector<double> getQF()const
    {
        return toQVector(f);
    }
    QVector<double> getQG()const
    {
        return toQVector(g);
    }
    QVector<double> getQP()const
    {
        return toQVector(p);
    }

    Eigen::VectorXd getLinF(int n)const
    {
        double min=f[0];
        double max=f[f.size()-1];

        Eigen::VectorXd flin(n);

        for (int i=0; i<n; i++)
        {
            flin[i]=i*(max-min)/n+min;
        }

        return flin;
    }

    void fit(Shape<Eigen::Vector3d>* model);

    QVector<double> at(const Eigen::VectorXd& A,QVector<double> values);

    QString gname()const
    {
        return legendgname;
    }
    void setgName(QString legendgname)
    {
        this->legendgname=legendgname;
    }

    QString pname()const
    {
        return legendpname;
    }
    void setpName(QString legendpname)
    {
        this->legendpname=legendpname;
    }

    void operator=(const Curve2D_GainPhase& other);

    static Eigen::VectorXd buildFQVector(unsigned int sz);

private:
    Eigen::VectorXd f,g,p;
    QString legendgname;
    QString legendpname;
};

#endif // CURVE2D_H
