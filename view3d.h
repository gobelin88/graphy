#ifndef VIEW3D_H
#define VIEW3D_H

#include <QWidget>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QCullFace>
#include <iostream>

#include "qPosAtt.h"
#include "Cloud.h"
#include "qcustomplot.h"

/**
@class View3D
@brief Classe pour gestion d'affichage avec Qt3D
*/



class View3D:public Qt3DExtras::Qt3DWindow
{
public:
    View3D();

    void drawLine(const QVector3D& start, const QVector3D& end, const QColor& color);
    void addGrid(float step, unsigned int n, QColor color);
    void addCircle(float radius,unsigned int n,QColor color);
    void addCloud(Cloud * cloud,QColor color);
    void addCloudLine(Cloud * cloud,QColor color);
    void addCloudScalar(CloudScalar * cloud);
    void addCloudScalarLine(CloudScalar * cloud);

    void addMultiObj(QStringList filenames, QPosAtt posatt, float scale, QList<QColor> color);
    void addMultiObj(QStringList filenames, QPosAtt posatt,float scale,QColor color);
    void addObj(QString filename, QPosAtt posatt,float scale,QColor color);
    void setObjPosAtt(int id,const QPosAtt & T);
    void setObjColor(int id,QColor color);

    QWidget * getContainer();

    int size(){return transforms.size();}

protected:
    void mouseMoveEvent(QMouseEvent * event);
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent * event);


private:
    QWidget *container;
    Qt3DCore::QEntity *rootEntity;
    std::vector<Qt3DCore::QTransform *> transforms;
    std::vector<Qt3DExtras::QPhongMaterial *> materials;
    double xp,yp;

    struct CameraParams
    {
        CameraParams(double alpha,double beta,double radius)
        {
            init(alpha,beta,radius);
        }

        void init(double alpha,double beta,double radius)
        {
            this->alpha=alpha;
            this->beta=beta;
            this->radius=radius;
        }

        void update(Qt3DRender::QCamera * cameraEntity)
        {
            cameraEntity->setPosition(QVector3D(-radius*cos(alpha)*cos(beta),-radius*sin(beta),-radius*sin(alpha)*cos(beta)));
            cameraEntity->setUpVector(QVector3D(0, 1, 0));
            cameraEntity->setViewCenter(QVector3D(0, 0, 0));
        }

        void move(Qt3DRender::QCamera * cameraEntity,double dx,double dy)
        {
            double speed =0.01,eps=1e-2;
            this->alpha-=dx*speed;

            this->beta-=dy*speed;

            if(beta>(M_PI/2-eps))beta=(M_PI/2-eps);
            else if(beta<(-M_PI/2+eps))beta=(-M_PI/2+eps);

            update(cameraEntity);
        }

        void moveTo(Qt3DRender::QCamera * cameraEntity,double alpha,double beta,double radius)
        {
            init(alpha,beta,radius);
            update(cameraEntity);
        }

        double alpha,beta,radius;
    };

    CameraParams  * camera_params;
};
#endif // VIEW3D_H
