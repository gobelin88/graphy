#include <iostream>

#include <Eigen/Dense>
#include <QObject>
#include <QFile>

#include "ShapeFit.h"
#include "PosAtt.h"

#include <omp.h>

#ifndef OBJ_H
#define OBJ_H

#define TO_DEG   57.29577951308232087679
#define TO_RAD   0.017453292519943295769

using Eigen::Vector2i;
using Eigen::Vector3d;
using Eigen::Vector2d;
using Eigen::Matrix2d;
using Eigen::Matrix3d;
using Eigen::Quaterniond;
using Eigen::AngleAxisd;

typedef Matrix3d Base;

class Edge:public Vector2i
{
public:
    Edge(int a,int b):Vector2i(a,b)
    {
        value=0;
    }
    Edge(int a,int b,double value):Vector2i(a,b)
    {
        this->value=value;
    }
    double value;

    void operator =(Edge & other)
    {
        *this=other;
        value=other.value;
    }
};

using Face=std::vector<int>;

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
    Object(QString filename, PosAtt scale_posatt);
    ~Object();

    void disp();

    //Bounding
    BoundingBox getBox();
    Vector3d getBarycenter();
    double getRadius(Vector3d center);

    //Algorithms
    double nearestDelta(int face_id, const Vector3d& p, Vector3d & delta) const;
    void nearestTriangle(int face_id, const Vector3d& p, double *r) const;
    void nearestPolygon(int face_id, const Vector3d& p,double * r) const;

    bool isOpen()
    {
        return open;
    }

    //Shape3d interface
    Vector3d delta(const Vector3d& params);
    int nb_params();
    void setParams(const Eigen::VectorXd& params);
    const Eigen::VectorXd& getParams();

    void setScalePosAtt(const PosAtt& scalePosatt);

    PosAtt getPosAtt();
    double getScale();

    //I/O
    void save(QString filename);

    //Misc
    void rotegrity(double angle,
                   int nbsub,
                   double strech,
                   double radius_int,
                   double radius_dr,
                   double width, double delta_ext, double delta_int, QString filename, bool generate_parts);

    void exportEdges(QString filename);

private:
    //Misc
    void project(Shape<Eigen::Vector3d>* model);

    void computeNormals();
    void computeEdges();
    bool isNewEdge(const Edge & edge);
    void cutEdges();
    void dispEdges();
    void cutEdge(int id_edge);
    void subdiviseEdges();
    void projectOnBoundingSphere(Vector3d center);

    void dataAddColumn(Matrix<double,3,Eigen::Dynamic> &matrix, Eigen::Vector3d colToAdd);

    Vector3d getNormal(const Face& f)const;
    Base getBase(const Face& f) const;
    Vector2d getCoord2D(const Base& b, Vector3d params, Vector3d bary) const;
    Vector3d getCoord3D(const Base& b, Vector2d params, Vector3d bary) const;
    Vector3d getBarycenter(const Face& f) const;
    QRectF getSpan(Face f, Vector3d bary, const Base& b);


    QLineF clamp(QLineF line, QPolygonF r);

    Matrix<double,3,Eigen::Dynamic> pts_base;
    Matrix<double,3,Eigen::Dynamic> pts;

    std::vector<Face> faces;
//    std::vector<Vector3d> normals_base;
//    std::vector<Vector3d> normals;

    Matrix<double,3,Eigen::Dynamic> normals_base;
    Matrix<double,3,Eigen::Dynamic> normals;

    std::vector< std::vector<Vector2d> >  texCoord;
    std::vector<Vector3d> wire;
    std::vector<Edge> edges;


    Matrix3d Rx(double ang);
    Matrix3d Ry(double ang);
    Matrix3d Rz(double ang);
    Matrix3d matFromEular(double yaw,double pitch,double roll);

    bool open;

    VectorXd params;

    double Q_norm;
    Quaterniond Q_transform;
    Matrix3d R_transform;
    Vector3d P_transform;
};



#endif // OBJ_H
