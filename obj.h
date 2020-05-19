#include <iostream>

#include <Eigen/Dense>
#include <QObject>
#include <QFile>

#include "ShapeFit.h"
#include "qPosAtt.h"

#ifndef OBJ_H
#define OBJ_H

#define TO_DEG   57.29577951308232087679
#define TO_RAD   0.017453292519943295769

using Eigen::Vector3d;
using Eigen::Vector2d;
using Eigen::Matrix2d;
using Eigen::Matrix3d;

typedef Matrix3d Base;

typedef QList<int> Face;

class BoundingBox
{
public:
    BoundingBox(Eigen::Vector3d Pmin,Eigen::Vector3d Pmax)
    {
        this->Pmin=Pmin;
        this->Pmax=Pmax;
    }

    Eigen::Vector3d middle()
    {
        return (Pmin+Pmax)/2;
    }

    void operator=(const BoundingBox& other)
    {
        this->Pmin=other.Pmin;
        this->Pmax=other.Pmax;
    }

    Eigen::Vector3d Pmin,Pmax;
};

class Object:public Shape<Eigen::Vector3d>
{
public:
    Object(QString filename, double scale, QPosAtt posatt);

    void disp();

    //Bounding
    BoundingBox getBox();
    Vector3d getBarycenter();
    double getRadius();

    //Algorithms
    Vector3d nearest(int face_id, const Vector3d& params)const;
    Vector3d nearest(const Vector3d& params)const;

    bool isOpen()
    {
        return open;
    }

    //Shape3d interface
    Vector3d delta(const Vector3d& params)const;
    int nb_params();
    void setParams(const Eigen::VectorXd& params);
    const Eigen::VectorXd& getParams();
    Vector3d transform(const Vector3d& params);
    void transform();

    void setScalePosAtt(double scale,const QPosAtt& posatt);

    QPosAtt getPosAtt();
    double getScale();

private:
    //Misc
    void computeNormals();
    Base getBase(const Face& f) const;
    Vector2d getCoord2D(const Base& b, Vector3d params, Vector3d bary) const;
    Vector3d getCoord3D(const Base& b, Vector2d params, Vector3d bary) const;
    Vector3d getBarycenter(const Face& f) const;
    QRectF getSpan(Face f, Vector3d bary, const Base& b);


    QLineF clamp(QLineF line, QPolygonF r);

    std::vector<Vector3d> pts_base;
    std::vector<Vector3d> pts;
    std::vector<Face> faces;
    std::vector<Vector3d> normals;
    std::vector< std::vector<Vector2d> >  texCoord;
    std::vector<Vector3d> wire;

    Matrix3d Rx(double ang);
    Matrix3d Ry(double ang);
    Matrix3d Rz(double ang);
    Matrix3d matFromEular(double yaw,double pitch,double roll);

    bool open;

    Eigen::VectorXd params;
    Eigen::Matrix4d mat;
};



#endif // OBJ_H
