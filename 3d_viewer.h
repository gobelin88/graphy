#ifndef VIEWER3D_H
#define VIEWER3D_H

#include <QWidget>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QCullFace>
#include <QAction>
#include <iostream>
#include <QScreen>
#include <Qt3DRender/QRenderCaptureReply>
#include <QPickEvent>
#include <3d_objectLoader.h>

#include <QMenu>
#include <QAction>
#include <QMouseEvent>

#include "PosAtt.h"
#include "Cloud.h"
#include "MyCustomPlot.h"
#include "3d_objectLoader.h"

#include "3d_label.h"
#include "3d_object.h"
#include "3d_shapes.h"
#include "3d_light.h"
#include "3d_cloud.h"
#include "ColorWheel.hpp"

#include "3d_customViewContainer.h"
#include "3d_cameraParams.h"
#include "MyGradientComboBox.h"

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
class Viewer3D:public Qt3DExtras::Qt3DWindow
{
    Q_OBJECT
public:
    Viewer3D(const QMap<QString, QKeySequence>& shortcuts_map, QWidget *parent);
    ~Viewer3D();

    void addCloudScalar(Cloud* cloudData,
                        Qt3DRender::QGeometryRenderer::PrimitiveType primitiveMode=Qt3DRender::QGeometryRenderer::PrimitiveType::Points);
    void addObject(Qt3DRender::QMesh* mesh_object, Object *object, PosAtt posatt, float scale, QColor color);
    void addSphere(Sphere * sphere,QColor color);
    void addEllipsoid(Ellipsoid * ellipsoid,QColor color);
    void addPlan(Plan* plan, float radius, QColor color);

    CustomViewContainer* getContainer();

    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);

public slots:
    void slot_copy();
    void slot_paste();
    void slot_saveImage();
    void slot_saveRevolution();
    void slot_setPointSize(double value);
    void slot_setPrimitiveType(int type);
    void slot_fitEllipsoid();
    void slot_fitSphere();
    void slot_fitPlan();
    void slot_fitCustomMesh();
    void slot_ScaleChanged();
    void slot_setGradient(int preset);
    void slot_ColorScaleChanged(const QCPRange& range);
    void slot_setCustomColor(QColor color);
    void slot_useCustomColor(int value);
    void slot_updateGridAndLabels();
    void slot_projectCustomMesh();
    void slot_projectPlan();
    void slot_projectSphere();
    void slot_randomSubSamples();
    void slot_resetView();
    void slot_resetViewOnSameRanges();
    void slot_resetViewOnSelected();
    void slot_resetViewOnSelectedSameRanges();
    void slot_export();
    void slot_addMesh();
    void slot_createRotegrity();
    void slot_showHideGrid(int value);
    void slot_showHideAxis(int value);
    void slot_showHideLabels(int value);
    void slot_removeSelected();
    void slot_updateLabels();
    void slot_computeSolidHarmonics();
    void slot_itemDoubleClicked(int index);
    void slot_fitPointCloud();
    void slot_movePointCloud();


signals:
    void sig_newColumn(QString varName,Eigen::VectorXd data);
    void sig_displayResults(QString results);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent * event);

private:

    //Registration Algorithms
    bool computeArun(Cloud * pcA,Cloud * pcB);
    bool computeHorn(Cloud * pcA,Cloud * pcB);

    //Ranges
    void extendScalarRange(QCPRange itemRangeS,int i);
    void extendRanges(QCPRange itemRangeX,QCPRange itemRangeY,QCPRange itemRangeZ,int i);
    void extendSameRanges(QCPRange itemRangeX,QCPRange itemRangeY,QCPRange itemRangeZ,int i);

    //3D
    CameraParams* cameraParams;
    CustomViewContainer* customContainer;
    void init3D();

    //Root
    Qt3DCore::QEntity* rootEntity;

    //3D data stuff Cloud/Mesh/obj
    std::vector<Base3D*> objects3D;
    Grid3D* grid3D;
    void referenceObjectEntity(Base3D *base3D, QString name);
    std::vector<Base3D*> getMeshs(bool selected);
    std::vector<Cloud3D*> getClouds(bool selected);
    std::vector<Base3D*> getObjects(bool selected);


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
    QMenu* popupMenu;
    QMenu* menuParameters;
    QMenu* menuFit;
    QMenu * menuProject;
    QMenu * menuScalarField;
    QMenu * menuTools;
    QMenu * menuView;
    QMenu * menuData;
    QMenu * menuMesh;

    QAction* actSave;
    QAction* actSaveRevolution;
    QAction* actFitEllipsoid;
    QAction* actFitSphere;
    QAction* actFitPlan;
    QAction* actFitMesh;
    QAction * actProjectSphere;
    QAction * actProjectPlan;
    QAction * actProjectMesh;
    QAction * actRandomSubSample;
    QAction * actMove;
    QAction * actFitPointCloud;
    QAction * actComputeSolidHarmonics;
    QAction * actRescale;
    QAction * actRescaleSelectedSameRanges;
    QAction * actRescaleSelected;
    QAction * actRemoveSelected;
    QAction * actExport;
    QAction * actMeshLoad;
    QAction * actMeshCreateRotegrity;
    QAction * actFullscreen;
    QAction* actCopy;
    QAction* actPaste;

    QComboBox* c_gradient;
    QDoubleSpinBox* sb_size;
    QComboBox* cb_mode;
    QCheckBox * cb_show_hide_grid;
    QCheckBox * cb_show_hide_axis;
    QCheckBox * cb_show_hide_labels;
    QCheckBox * cb_use_custom_color;
    ColorWheel * cw_custom_color;

    //Misc
    QString current_filename;
    float xp,yp;
    bool xy_reversed;
    bool yz_reversed;
    bool xz_reversed;
    void setCloudPointSize(Cloud3D * currentCloud3D, double value);
    void resetRanges();
    void dispRanges();
    void updateScalarRanges(std::vector<Cloud3D*> & cloudsList);
    void updateRanges(std::vector<Cloud3D*> & cloudsList,bool same);
};
#endif // VIEW3D_H
