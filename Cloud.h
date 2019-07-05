#include <QVector>
#include <QVector3D>
#include <Eigen/Dense>

#ifndef CLOUD_H
#define CLOUD_H

using Surface=Eigen::MatrixXd;

class Cloud
{
public:
    Cloud(const QVector<double> & x,const QVector<double> &  y,const QVector<double> &  z);

    void operator=(const Cloud & other);

    const QVector<QVector3D> & data()const {return pts;}

    Surface getExtrapolated()const
    {
        Surface s;

        return s;
    }

private:
    QVector<QVector3D> pts;
};

#endif
