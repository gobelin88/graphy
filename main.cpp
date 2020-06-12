#include "mainwindow.h"
#include <QApplication>
#include <iostream>

class DotProductMinimizer
{
public:
    DotProductMinimizer(int n)
    {
        s=Eigen::MatrixXd::Zero(n,2);
    }

    Eigen::Vector3d getV(int i)
    {
        double alpha=s(i,0)*M_PI;
        double beta=s(i,1)*M_PI/2;
        return Eigen::Vector3d(cos(alpha)*cos(beta),
                               sin(alpha)*cos(beta),
                               sin(beta));
    }

    Eigen::VectorXd getErr()
    {
        Eigen::VectorXd E(s.rows()*(s.rows()-1)/2);

        int id=0;
        for (int i=0; i<s.rows(); i++)
        {
            for (int j=i+1; j<s.rows(); j++)
            {
                E[id++]=getV(i).dot(getV(j));
            }
        }

        return E;
    }

    void randomize()
    {
        s.setRandom();
        s(0,0)=0;
        s(0,1)=0;
    }

    void search(int it)
    {
        randomize();

        Eigen::MatrixXd best_s=s;
        double best_err=getErr().squaredNorm();

        for (int k=0; k<it; k++)
        {
            randomize();
            double err=getErr().squaredNorm();
            if (err<best_err)
            {
                best_err=err;
                best_s=s;
            }
        }

        s=best_s;
    }

    void save()
    {
        QFile file(QString("DotProductMinimizer_%1.csv").arg(s.rows()));
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream ts(&file);
            for (int k=0; k<s.rows(); k++)
            {
                Eigen::Vector3d v=getV(k);
                for (int j=0; j<3; j++)
                {
                    ts<<v[j]<<";";
                }
                ts<<"\n";
                for (int j=0; j<3; j++)
                {
                    ts<<-v[j]<<";";
                }
                ts<<"\n";
            }
        }

    }

    Eigen::MatrixXd s;
};

void testDotProductMinimizer()
{
    DotProductMinimizer minimizer(3);
    std::cout<<"err="<<minimizer.getErr().squaredNorm()<<std::endl;
    minimizer.search(1000000);
    std::cout<<"err="<<minimizer.getErr().squaredNorm()<<std::endl;
    minimizer.save();
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QStringList args=a.arguments();

    MainWindow w;
    if (args.size()==2)
    {
        w.direct_open(args[1]);
    }
    else
    {
        std::cout<<args.size()<<std::endl;
    }

    w.showMaximized();
    return a.exec();
}
