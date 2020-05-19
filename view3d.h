#ifndef VIEW3D_H
#define VIEW3D_H

#include <QWidget>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QCullFace>
#include <iostream>
#include <Qt3DRender/QRenderCaptureReply>
#include <obj.h>

#include <QMenu>
#include <QAction>
#include <QMouseEvent>

#include "qPosAtt.h"
#include "Cloud.h"
#include "qcustomplot.h"
#include "obj.h"

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
    QCPColorScale* getColorScale()
    {
        return scale;
    }
    QCustomPlot* getColorScalePlot()
    {
        return plot;
    }

private:
    QWidget* container;

    QCustomPlot* plot;
    QCPColorScale* scale;
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

    void addGrid(Cloud* cloud,unsigned int N,QColor color);
    void addLabel(QString text, QVector3D coord, float scale, float anglex, float angley, float anglez);

    void setCloudScalar(Cloud* cloud, PrimitiveMode primitiveMode);

    void addObj(Qt3DRender::QMesh* m_obj, QPosAtt posatt,float scale,QColor color);
    void setObjPosAtt(unsigned int id, const QPosAtt& T);
    void setObjColor(unsigned int id, QColor color);

    void addSphere(QPosAtt posatt,float scale,QColor color,double radius);
    void addPlan(QPosAtt posatt,float scale,QColor color,double width,double height);

    CustomViewContainer* getContainer();

public slots:
    void slot_saveImage();
    void slot_setPointSize(double value);
    void slot_setPrimitiveType(int type);
    void slot_fitSphere();
    void slot_fitPlan();
    void slot_fitCustomMesh();
    void slot_ColorScaleChanged(const QCPRange& range);
    void slot_setGradient(int preset);

signals:
    void sig_newColumn(QString varName,Eigen::VectorXd data);
    void sig_displayResults(QString results);

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
    QMenu* menuParameters;
    QMenu* menuFit;
    QAction* actFitSphere;
    QAction* actFitPlan;
    QAction* actFitMesh;

    //Misc
    QString current_filename;
    float xp,yp;

    //3D
    void init3D();
    PrimitiveMode mode;
    Qt3DExtras::QPerVertexColorMaterial* cloudMaterial;
    Qt3DRender::QGeometryRenderer* cloudPrimitives;
    Qt3DRender::QPointSize* pointSize;
    Qt3DRender::QLineWidth* lineWidth;
    Qt3DCore::QEntity* cloudPrimitivesEntity;
    Qt3DRender::QGeometry* cloudGeometry;
    Qt3DRender::QBuffer* cloudBuf;
    Qt3DRender::QAttribute* cloudPositionAttribute;
    Qt3DRender::QAttribute* cloudColorsAttribute;
    Qt3DCore::QEntity* rootEntity;
    std::vector<Qt3DCore::QTransform*> transforms;
    std::vector<Qt3DExtras::QPhongMaterial*> materials;

    //Data
    Cloud* cloud;

    QComboBox* c_gradient;
    QDoubleSpinBox* sb_size;
    QComboBox* cb_mode;
};
#endif // VIEW3D_H
