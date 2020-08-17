#ifndef CURVE2D_GAINPHASE_H
#define CURVE2D_GAINPHASE_H

#include <Eigen/Dense>
#include <QVector>
#include "ShapeFit.h"
#include "tabledata.h"
#include "curve2d.h"

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

//---------------------------------------------------------
class Curve2DModulePhase
{
public:
    Curve2DModulePhase()
    {

    }

    Curve2DModulePhase(const Curve2D & modules_curve,
                       const Curve2D & phase_curve)
    {
        this->modules_curve=modules_curve;
        this->phase_curve=phase_curve;
    }

    const Curve2D & getModules()const
    {
        return modules_curve;
    }

    const Curve2D & getPhases()const
    {
        return phase_curve;
    }

    void fit(Shape<Eigen::Vector3d>* model)
    {
        std::vector<Eigen::Vector3d> points;
        for (int m=0; m<modules_curve.getX().size(); ++m)
        {
            points.push_back(Eigen::Vector3d(modules_curve.getX()[m],modules_curve.getY()[m],phase_curve.getY()[m]));
        }

        model->fit(points,10000);
    }

private:
    Curve2D modules_curve;
    Curve2D phase_curve;
};
#endif // CURVE2D_H
