#include <QFile>
#include <iostream>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <QVector3D>
#include <QQuaternion>

#ifndef QPOSATT_H
#define QPOSATT_H

QQuaternion toQQuaternion(Eigen::Quaterniond q);
QVector3D toQVector3D(Eigen::Vector3d v);

/**
 * @brief The QPosAtt class
 */
class PosAtt
{
public:
    PosAtt();
    PosAtt(Eigen::Vector3d p,Eigen::Quaterniond q=Eigen::Quaterniond(1,0,0,0));

    Eigen::Vector3d P;
    Eigen::Quaterniond Q;

    void operator=(const PosAtt& other);
    PosAtt compose(const PosAtt& other)const;
    Eigen::Vector3d apply(const Eigen::Vector3d& v)const;
    PosAtt inverse();
    Eigen::Matrix4d toMatrice(double scale=1.0);
    void disp();
    QString toStr();
};

typedef std::vector<PosAtt> QPosAttList;
typedef std::vector<Eigen::Vector3d> QPosList;

bool loadPosAttList(QString filename,QPosAttList& list);
bool loadPosList(QString filename,QPosList& list);
QPosList extractPos(QPosAttList& list);

Eigen::MatrixXd toEigenMatXd(const QPosList& ptlist);

#endif // QPOSATT_H
