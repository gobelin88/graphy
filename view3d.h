#ifndef VIEW3D_H
#define VIEW3D_H

#include <QWidget>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QCullFace>
#include <iostream>
#include <Qt3DRender/QRenderCaptureReply>


#include <QMenu>
#include <QAction>
#include <QMouseEvent>

#include "qPosAtt.h"
#include "Cloud.h"
#include "qcustomplot.h"

/**
@class View3D
@brief Classe pour gestion d'affichage avec Qt3D
*/

class CustomViewContainer: public QWidget
{
    Q_OBJECT
public:
    CustomViewContainer(QWidget* container);

    QWidget* getContainer();

private:
    QWidget* container;
};


class View3D:public Qt3DExtras::Qt3DWindow
{
    Q_OBJECT
public:
    enum PrimitiveMode
    {
        MODE_POINTS,
        MODE_LINES,
    };

    View3D();

    void addGrid(CloudScalar* cloud,unsigned int N,QColor color);
    void addLabel(QString text, QVector3D coord, double scale, double anglex, double angley,double anglez);

    void setCloudScalar(CloudScalar* cloud, PrimitiveMode primitiveMode);

    void addObj(QString filename, QPosAtt posatt,float scale,QColor color);
    void setObjPosAtt(unsigned int id, const QPosAtt& T);
    void setObjColor(unsigned int id, QColor color);

    CustomViewContainer* getContainer();

public slots:
    void slot_saveImage();
    void slot_setPointSize(double value);
    void slot_setPrimitiveType(int type);
    void slot_parameters();

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:

    struct CameraParams
    {
        CameraParams(Qt3DRender::QCamera* cameraEntity,float alpha,float beta,float radius,QVector3D barycenter=QVector3D(0,0,0),float boundingRadius=0.0)
        {
            this->cameraEntity=cameraEntity;
            init(alpha,beta,radius);
            this->barycenter=barycenter;
            this->boundingRadius=boundingRadius;
        }

        void init(float alpha,float beta,float radius)
        {
            this->alpha=alpha;
            this->beta=beta;
            this->radius=radius;
        }

        void update()
        {
            cameraEntity->setPosition(barycenter+QVector3D(-radius*cos(alpha)*cos(beta),-radius*sin(beta),-radius*sin(alpha)*cos(beta)));
            cameraEntity->setUpVector(QVector3D(0, 1, 0));
            cameraEntity->setViewCenter(barycenter);
        }

        void move(float dx,float dy)
        {
            float speed =0.01f,eps=1.0e-2f;
            this->alpha-=dx*speed;

            this->beta+=dy*speed;

            if (beta>(float(M_PI)/2.0f-eps))
            {
                beta=(float(M_PI)/2.0f-eps);
            }
            else if (beta<(-float(M_PI)/2.0f+eps))
            {
                beta=(-float(M_PI)/2.0f+eps);
            }

            update();
        }

        void moveTo(float alpha,float beta,float radius)
        {
            init(alpha,beta,radius);
            update();
        }

        void setBarycenter(QVector3D barycenter)
        {
            this->barycenter=barycenter;
            update();
        }

        QVector3D getBarycenter()
        {
            return barycenter;
        }

        void setBoundingRadius(float boundingRadius)
        {
            this->boundingRadius=boundingRadius;
        }

        float getBoundingRadius()
        {
            return boundingRadius;
        }

        float getRadius()
        {
            return radius;
        }

        void setRadius(float radius)
        {
            this->radius=radius;
            update();
        }

        void reset()
        {
            moveTo(float(M_PI*0.25+M_PI),-float(M_PI*0.10),4*boundingRadius);
        }

        Qt3DRender::QCamera* entity()
        {
            return cameraEntity;
        }

    private:
        float alpha,beta;
        float radius;
        float boundingRadius;
        QVector3D barycenter;
        Qt3DRender::QCamera* cameraEntity;
    };

    CameraParams* camera_params;

    CustomViewContainer* customContainer;

    //Menu
    void createPopup();
    QMenu* popup_menu;
    QAction* actSave;
    QAction* actParameters;

    //Misc
    QString current_filename;
    float xp,yp;

    //3D
    void init3D();
    PrimitiveMode mode;
    Qt3DExtras::QPerVertexColorMaterial* material;
    Qt3DRender::QGeometryRenderer* primitives;
    Qt3DRender::QPointSize* pointSize;
    Qt3DCore::QEntity* primitivesEntity;
    Qt3DRender::QGeometry* geometry;
    Qt3DRender::QBuffer* buf;
    Qt3DRender::QAttribute* positionAttribute;
    Qt3DRender::QAttribute* colorsAttribute;
    Qt3DCore::QEntity* rootEntity;
    std::vector<Qt3DCore::QTransform*> transforms;
    std::vector<Qt3DExtras::QPhongMaterial*> materials;
};
#endif // VIEW3D_H
