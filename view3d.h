#ifndef VIEW3D_H
#define VIEW3D_H

#include <QWidget>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QCullFace>
#include <QAction>
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

#include "label3d.h"
#include "object3d.h"
#include "customviewcontainer.h"
#include "cameraparams.h"

struct Base3D
{
    Qt3DCore::QEntity* entity;
    Qt3DRender::QBuffer* buffer;
    Qt3DRender::QAttribute* positionAttribute;
    Qt3DRender::QGeometry* geometry;
    Qt3DRender::QGeometryRenderer* geometryRenderer;
};

struct Grid3D:public Base3D
{
    Grid3D(Qt3DCore::QEntity* rootEntity,unsigned int N,QColor color)
    {
        geometry = new Qt3DRender::QGeometry(rootEntity);

        unsigned int n=(N+1)*3;

        buffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
        buffer->setData(getGridBuffer(false,false,false,N));

        positionAttribute = new Qt3DRender::QAttribute(geometry);
        positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        positionAttribute->setVertexSize(3);
        positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        positionAttribute->setBuffer(buffer);
        positionAttribute->setByteStride(3 * sizeof(float));
        positionAttribute->setCount(4*n);
        geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

        // connectivity between vertices
        QByteArray indexBytes;
        indexBytes.resize( (4*n) * sizeof(unsigned int)); // start to end
        unsigned int* indices = reinterpret_cast<unsigned int*>(indexBytes.data());
        for (unsigned int i=0; i<n; i++)
        {
            *indices++ = 0+4*i;
            *indices++ = 1+4*i;
            *indices++ = 2+4*i;
            *indices++ = 3+4*i;
        }

        gridIndexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
        gridIndexBuffer->setData(indexBytes);

        gridIndexAttribute = new Qt3DRender::QAttribute(geometry);
        gridIndexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
        gridIndexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        gridIndexAttribute->setBuffer(gridIndexBuffer);
        gridIndexAttribute->setCount(4*n);
        geometry->addAttribute(gridIndexAttribute); // We add the indices linking the points in the geometry

        // mesh
        geometryRenderer = new Qt3DRender::QGeometryRenderer(rootEntity);
        geometryRenderer->setGeometry(geometry);
        geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
        gridMaterial = new Qt3DExtras::QPhongMaterial(rootEntity);
        gridMaterial->setAmbient(color);

        // entity
        entity = new Qt3DCore::QEntity(rootEntity);
        entity->addComponent(geometryRenderer);
        entity->addComponent(gridMaterial);
    }

    QByteArray getGridBuffer(bool xy_swap,bool xz_swap,bool yz_swap,int N)
    {
        unsigned int n=(N+1)*3;

        QByteArray bufferBytes;
        bufferBytes.resize(3 * (4*n) * sizeof(float));
        float* positions = reinterpret_cast<float*>(bufferBytes.data());

        double step=2.0/N;

        for (unsigned int i=0; i<N+1; i++)
        {
            *positions++ = i*step-1;
            *positions++ = xz_swap?1:-1;
            *positions++ = 1;

            *positions++ = i*step-1;
            *positions++ = xz_swap?1:-1;
            *positions++ = -1;

            *positions++ = 1;
            *positions++ = xz_swap?1:-1;
            *positions++ = i*step-1;

            *positions++ = -1;
            *positions++ = xz_swap?1:-1;
            *positions++ = i*step-1;
        }

        for (unsigned int i=0; i<N+1; i++)
        {
            *positions++ = yz_swap?1:-1;
            *positions++ = (i)*step-1;
            *positions++ = 1;

            *positions++ = yz_swap?1:-1;
            *positions++ = i*step-1;
            *positions++ = -1;

            *positions++ = yz_swap?1:-1;
            *positions++ = 1;
            *positions++ = i*step-1;

            *positions++ = yz_swap?1:-1;
            *positions++ = -1;
            *positions++ = (i)*step-1;
        }

        for (unsigned int i=0; i<N+1; i++)
        {
            *positions++ = (i)*step-1;
            *positions++ = 1;
            *positions++ = xy_swap?1:-1;

            *positions++ = (i)*step-1;
            *positions++ = -1;
            *positions++ = xy_swap?1:-1;

            *positions++ = 1;
            *positions++ = (i)*step-1;
            *positions++ = xy_swap?1:-1;

            *positions++ = -1;
            *positions++ = (i)*step-1;
            *positions++ = xy_swap?1:-1;
        }

        return bufferBytes;
    }

    Qt3DRender::QBuffer* gridIndexBuffer;
    Qt3DRender::QAttribute* gridIndexAttribute;
    Qt3DExtras::QPhongMaterial* gridMaterial;
};

struct Cloud3D:public Base3D
{
    Qt3DExtras::QPerVertexColorMaterial* cloudMaterial;
    Qt3DRender::QPointSize* pointSize;
    Qt3DRender::QLineWidth* lineWidth;
    Qt3DCore::QTransform* cloudTransform;
    Qt3DRender::QAttribute* cloudColorsAttribute;
};

/**
@class View3D
@brief Classe pour gestion d'affichage avec Qt3D
*/
class View3D:public Qt3DExtras::Qt3DWindow
{
    Q_OBJECT
public:
    enum PrimitiveMode
    {
        MODE_POINTS,
        MODE_LINES,
        MODE_LINE_STRIP,
    };

    View3D();

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
    void slot_ScaleChanged();
    void slot_setGradient(int preset);
    void updateGridAndLabels();

signals:
    void sig_newColumn(QString varName,Eigen::VectorXd data);
    void sig_displayResults(QString results);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:
    void updateLabels();

    CameraParams* camera_params;
    CustomViewContainer* customContainer;

    //Misc
    QString current_filename;
    float xp,yp;

    //3D
    void init3D();
    PrimitiveMode mode;
    Qt3DCore::QEntity* rootEntity;
    std::vector<Qt3DCore::QTransform*> transforms;
    std::vector<QMatrix4x4> baseTransforms;
    std::vector<Qt3DExtras::QPhongMaterial*> materials;

    //Cloud3D
    Cloud3D cloud3D;

    //Grid
    Grid3D* grid3D;

    //Data
    Cloud* cloud;

    //Labels Tiks and Arrows
    Label3D* labelx;
    Label3D* labely;
    Label3D* labelz;
    Object3D* objArrowX;
    Object3D* objArrowY;
    Object3D* objArrowZ;

    //Menu
    void configurePopup();
    void createPopup();
    QMenu* popup_menu;
    QAction* actSave;
    QMenu* menuParameters;
    QMenu* menuFit;
    QAction* actFitSphere;
    QAction* actFitPlan;
    QAction* actFitMesh;

    QComboBox* c_gradient;
    QDoubleSpinBox* sb_size;
    QComboBox* cb_mode;

    //
    bool xy_reversed;
    bool yz_reversed;
    bool xz_reversed;
};
#endif // VIEW3D_H
