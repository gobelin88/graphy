#ifndef VIEW3D_H
#define VIEW3D_H

#include <QWidget>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QCullFace>
#include <QAction>
#include <iostream>
#include <QScreen>
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
#include "grid3d.h"
#include "light3d.h"
#include "cloud3d.h"

#include "customviewcontainer.h"
#include "cameraparams.h"
#include "qgradientcombobox.h"

class MyCapture : public QObject
{
    Q_OBJECT
public:
    MyCapture(Qt3DRender::QRenderCapture* capture)
        : m_capture(capture)
        , m_reply(nullptr)
    {
    }
public slots:
    void onCompleted()
    {
        done.store(1);
    }
    QImage capture()
    {
        done.store(0);

        m_reply = m_capture->requestCapture();
        connection = QObject::connect(m_reply, &Qt3DRender::QRenderCaptureReply::completed,
                                      this, &MyCapture::onCompleted);


        while (!done.load())
        {
            QApplication::processEvents();
            QThread::yieldCurrentThread();
        }

        return m_reply->image();
    }
private:
    Qt3DRender::QRenderCapture* m_capture;
    Qt3DRender::QRenderCaptureReply* m_reply;
    QMetaObject::Connection connection;
    QAtomicInt done;
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
        MODE_TRIANGLE,
        MODE_TRIANGLE_STRIP,
    };

    View3D();

    void setCloudScalar(Cloud* cloud, PrimitiveMode primitiveMode);

    void addObj(Qt3DRender::QMesh* m_obj, QPosAtt posatt,float scale,QColor color);
    void setObjPosAtt(unsigned int id, const QPosAtt& T);
    void setObjColor(unsigned int id, QColor color);

    void addSphere(QPosAtt posatt,float scale,QColor color,double radius);
    void addPlan(QPosAtt posatt,float scale,QColor color,double width,double height);

    void addSphere(Sphere * sphere,QColor color);
    void addPlan(Plan* plan, float radius, QColor color);

    CustomViewContainer* getContainer();

public slots:
    void slot_saveImage();
    void slot_saveRevolution();
    void slot_setPointSize(double value);
    void slot_setPrimitiveType(int type);
    void slot_fitSphere();
    void slot_fitPlan();
    void slot_fitCustomMesh();
    void slot_ColorScaleChanged(const QCPRange& range);
    void slot_ScaleChanged();
    void slot_setGradient(int preset);
    void updateGridAndLabels();

    void slot_projectCustomMesh();
    void slot_projectPlan();
    void slot_projectSphere();

    void slot_randomSubSamples();

    void slot_resetView();
    void slot_export();

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
    std::vector<QMatrix4x4> baseR;
    std::vector<QMatrix4x4> baseT;
    std::vector<Qt3DExtras::QPhongMaterial*> materials;

    std::vector<Base3D*> objects_list;

    //Cloud3D
    Cloud3D* cloud3D;

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
    QAction* actSaveRevolution;
    QMenu* menuParameters;
    QMenu* menuFit;
    QAction* actFitSphere;
    QAction* actFitPlan;
    QAction* actFitMesh;

    QMenu * menuProject;
    QAction * actProjectSphere;
    QAction * actProjectPlan;
    QAction * actProjectMesh;

    QMenu * menuSubSample;
    QAction * actRandomSubSample;

    QMenu * menuView;
    QAction * actRescale;

    QMenu * menuData;
    QAction * actExport;

    QComboBox* c_gradient;
    QDoubleSpinBox* sb_size;
    QComboBox* cb_mode;


    //
    bool xy_reversed;
    bool yz_reversed;
    bool xz_reversed;
};
#endif // VIEW3D_H
