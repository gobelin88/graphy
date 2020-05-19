#include <QFile>
#include <iostream>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#ifndef QPOSATT_H
#define QPOSATT_H


/**
 * @brief The QPosAtt class
 */
class QPosAtt
{
public:
    QPosAtt()
    {
        this->P=Eigen::Vector3d(0,0,0);
        this->Q=Eigen::Quaterniond(1,0,0,0);
    }

    QPosAtt(Eigen::Vector3d p,Eigen::Quaterniond q)
    {
        this->P=p;
        this->Q=q;
    }

    Eigen::Vector3d P;
    Eigen::Quaterniond Q;

    void operator=(const QPosAtt& other)
    {
        this->P=other.P;
        this->Q=other.Q;
    }

    QPosAtt compose(const QPosAtt& other) const
    {
        return QPosAtt(other.Q._transformVector(P)+other.P,other.Q*Q);
    }

    Eigen::Vector3d apply(const Eigen::Vector3d& v) const
    {
        return Q._transformVector(v)+P;
    }

    QPosAtt inverse()
    {
        QPosAtt pos_att_inverse;
        pos_att_inverse.Q=Q.conjugate();
        pos_att_inverse.P =pos_att_inverse.Q._transformVector(-P);
        return pos_att_inverse;
    }

    Eigen::Matrix4d toMatrice(double scale=1.0)
    {
        Eigen::Matrix4d M;
        Eigen::Matrix3d R=scale*Q.toRotationMatrix();

        M(0,0)=R(0,0);
        M(0,1)=R(0,1);
        M(0,2)=R(0,2);
        M(0,3)=P[0];
        M(1,0)=R(1,0);
        M(1,1)=R(1,1);
        M(1,2)=R(1,2);
        M(1,3)=P[1];
        M(2,0)=R(2,0);
        M(2,1)=R(2,1);
        M(2,2)=R(2,2);
        M(2,3)=P[2];
        M(3,0)=0     ;
        M(3,1)=0     ;
        M(3,2)=0;
        M(3,3)=1;

        return M;
    }

    void disp()
    {
        std::cout<<" p="<<P[0]<<" "<<P[1]<<" "<<P[2]<<" q="<<Q.w()<<" "<<Q.x()<<" "<<Q.y()<<" "<<Q.z()<<std::endl;
    }

    QString toStr()
    {
        Eigen::Vector3d ea=Q.toRotationMatrix().eulerAngles(0,1,2)*57.2957795131;
        return QString("p=(%1,%2,%3)\nq=(%4,%5,%6,%7)\nr=(%8,%9,%10)\n").arg(P[0]).arg(P[1]).arg(P[2]).arg(Q.w()).arg(Q.x()).arg(Q.y()).arg(Q.z()).arg(ea[0]).arg(ea[1]).arg(ea[2]);
    }



};

typedef std::vector<QPosAtt> QPosAttList;
typedef std::vector<Eigen::Vector3d> QPosList;

bool loadPosAttList(QString filename,QPosAttList& list);
bool loadPosList(QString filename,QPosList& list);
QPosList extractPos(QPosAttList& list);

Eigen::MatrixXd toEigenMatXd(const QPosList& ptlist);

#endif // QPOSATT_H
