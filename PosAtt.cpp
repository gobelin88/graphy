#include "PosAtt.h"

QQuaternion toQQuaternion(Eigen::Quaterniond q)
{
    return QQuaternion(float(q.w()),float(q.x()),float(q.y()),float(q.z()));
}

QVector3D toQVector3D(Eigen::Vector3d v)
{
    return QVector3D(float(v.x()),float(v.y()),float(v.z()));
}

bool loadPosAttList(QString filename,QPosAttList& list)
{
    if (filename.isEmpty())
    {
        return false;
    }

    QFile file(filename);
    if (file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        list.clear();
        while (!file.atEnd())
        {
            QStringList line=QString(file.readLine()).split(";",QString::SkipEmptyParts);

            if (line.size()>=7)
            {

                Eigen::Quaterniond q(line[3].toDouble(),line[4].toDouble(),line[5].toDouble(),line[6].toDouble());
                q.normalize();

                PosAtt new_pos_att(Eigen::Vector3d(line[0].toDouble(),line[1].toDouble(),line[2].toDouble()),q);

                list.push_back(new_pos_att);
            }
            else
            {
                std::cout<<line.size()<<std::endl;
                return false;
            }
        }
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

Eigen::MatrixXd toEigenMatXd(const QPosList& ptlist)
{
    Eigen::MatrixXd M(ptlist.size(),3);

    for (int i=0; i<ptlist.size(); i++)
    {
        M(i,0)=ptlist[i].x();
        M(i,1)=ptlist[i].y();
        M(i,2)=ptlist[i].z();
    }

    return M.transpose();
}

bool loadPosList(QString filename,QPosList& list)
{
    if (filename.isEmpty())
    {
        return false;
    }

    QFile file(filename);
    if (file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        list.clear();
        while (!file.atEnd())
        {
            QStringList line=QString(file.readLine()).split(";",QString::SkipEmptyParts);

            if (line.size()>=3)
            {
                Eigen::Vector3d new_pos(line[0].toDouble(),line[1].toDouble(),line[2].toDouble());
                list.push_back(new_pos);
            }
            else
            {
                std::cout<<line.size()<<std::endl;
                return false;
            }
        }
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

QPosList extractPos(QPosAttList& list)
{
    QPosList listp(list.size());

    for (int i=0; i<list.size(); i++)
    {
        listp[i]=list[i].P;
    }

    return listp;
}

//-----------------------
PosAtt::PosAtt()
{
    this->P=Eigen::Vector3d(0,0,0);
    this->Q=Eigen::Quaterniond(1,0,0,0);
}

PosAtt::PosAtt(Eigen::Vector3d p,Eigen::Quaterniond q)
{
    this->P=p;
    this->Q=q;
}

void PosAtt::operator=(const PosAtt& other)
{
    this->P=other.P;
    this->Q=other.Q;
}

PosAtt PosAtt::compose(const PosAtt& other) const
{
    return PosAtt(other.Q._transformVector(P)+other.P,other.Q*Q);
}

Eigen::Vector3d PosAtt::apply(const Eigen::Vector3d& v) const
{
    return Q._transformVector(v)+P;
}

PosAtt PosAtt::inverse()
{
    PosAtt pos_att_inverse;
    pos_att_inverse.Q=Q.conjugate();
    pos_att_inverse.P =pos_att_inverse.Q._transformVector(-P);
    return pos_att_inverse;
}

Eigen::Matrix4d PosAtt::toMatrice(double scale)
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

void PosAtt::disp()
{
    std::cout<<" p="<<P[0]<<" "<<P[1]<<" "<<P[2]<<" q="<<Q.w()<<" "<<Q.x()<<" "<<Q.y()<<" "<<Q.z()<<std::endl;
}

QString PosAtt::toStr()
{
    Eigen::Vector3d ea=Q.toRotationMatrix().eulerAngles(0,1,2)*57.2957795131;
    return QString("p=(%1,%2,%3)\nq=(%4,%5,%6,%7)\nr=(%8,%9,%10)\n").arg(P[0]).arg(P[1]).arg(P[2]).arg(Q.w()).arg(Q.x()).arg(Q.y()).arg(Q.z()).arg(ea[0]).arg(ea[1]).arg(ea[2]);
}

