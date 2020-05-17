#include "view3d.h"


CustomViewContainer::CustomViewContainer(QWidget* container)
{
    QGridLayout* glayout=new QGridLayout(this);
    glayout->addWidget(container,0,0);

    this->container=container;
}

QWidget* CustomViewContainer::getContainer()
{
    return container;
}

void View3D::createPopup()
{
    popup_menu=new QMenu();
    actSave   = new QAction("Save",  this);

    menuFit= new QMenu("Fit");
    actFitSphere= new QAction("Sphere",  this);
    actFitPlan= new QAction("Plan",  this);
    actFitMesh= new QAction("Custom Mesh",  this);

    menuParameters= new QMenu("Parameters");

    ///////////////////////////////////////////////
    QWidgetAction* actWidget=new QWidgetAction(popup_menu);
    QWidget* widget=new QWidget;
    actWidget->setDefaultWidget(widget);

    QGridLayout* gbox = new QGridLayout();

    QDoubleSpinBox* sb_size=new QDoubleSpinBox;
    sb_size->setRange(0.1,100);
    sb_size->setSingleStep(1.0);
    sb_size->setValue(static_cast<double>(pointSize->value()));

    QComboBox* cb_mode=new QComboBox;
    cb_mode->addItem("POINTS");
    cb_mode->addItem("LINES");
    cb_mode->setCurrentIndex(static_cast<int>(mode));

    gbox->addWidget(sb_size,0,0);
    gbox->addWidget(cb_mode,1,0);

    widget->setLayout(gbox);

    ///////////////////////////////////////////////
    popup_menu->addAction(actSave);
    popup_menu->addMenu(menuFit);
    menuFit->addAction(actFitSphere);
    menuFit->addAction(actFitPlan);
    menuFit->addAction(actFitMesh);
    popup_menu->addMenu(menuParameters);
    menuParameters->addAction(actWidget);

    QObject::connect(actSave,SIGNAL(triggered()),this,SLOT(slot_saveImage()));
    QObject::connect(sb_size, SIGNAL(valueChanged(double)), this, SLOT(slot_setPointSize(double)));
    QObject::connect(cb_mode, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setPrimitiveType(int)));

    QObject::connect(actFitSphere, SIGNAL(triggered() ), this, SLOT(slot_fitSphere()));
    QObject::connect(actFitPlan, SIGNAL(triggered() ), this, SLOT(slot_fitPlan()));
    QObject::connect(actFitMesh, SIGNAL(triggered() ), this, SLOT(slot_fitCustomMesh()));
}

//void View3D::slot_parameters()
//{
//    QDialog* dialog=new QDialog;
//    dialog->setLocale(QLocale("C"));
//    dialog->setWindowTitle("Style Parameters");
//    QGridLayout* gbox = new QGridLayout();
//    dialog->setMinimumWidth(200);

//    QDoubleSpinBox* sb_pointSize=new QDoubleSpinBox(dialog);
//    sb_pointSize->setRange(0.1,100);
//    sb_pointSize->setSingleStep(0.1);
//    sb_pointSize->setValue(pointSize->value());
//    sb_pointSize->setPrefix("Point size=");

//    QComboBox* cb_mode=new QComboBox(dialog);
//    cb_mode->addItem("POINTS");
//    cb_mode->addItem("LINES");
//    cb_mode->setCurrentIndex(static_cast<int>(mode));

//    QObject::connect(sb_pointSize, SIGNAL(valueChanged(double)), this, SLOT(slot_setPointSize(double)));
//    QObject::connect(cb_mode, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setPrimitiveType(int)));

//    gbox->addWidget(sb_pointSize,0,0);
//    gbox->addWidget(cb_mode,1,0);

//    dialog->setLayout(gbox);

//    int result=dialog->exec();
//}

void View3D::slot_saveImage()
{

    Qt3DRender::QRenderCapture renderCapture(rootEntity);

    QFileInfo info(current_filename);
    QString where=info.path();

    QString filename=QFileDialog::getSaveFileName(nullptr,"Save Image",where,"(*.png)");
    if (!filename.isEmpty())
    {
        this->current_filename=filename;
        QPixmap::grabWindow(getContainer()->winId()).save(filename);
    }

}

void View3D::slot_fitSphere()
{
    Sphere sphere;
    cloud->fit(&sphere);

    Eigen::Vector3d C=sphere.getCenter();

    addSphere(QPosAtt(C,Eigen::Quaterniond()),1.0,QColor(64,64,64),sphere.getRadius());

    emit sig_displayResults( QString("Fit Sphere:\nCenter=(%1 , %2 , %3) Radius=%4\nRms=%5\n").arg(C[0]).arg(C[1]).arg(C[2]).arg(sphere.getRadius()).arg(sphere.getRMS()) );
    emit sig_newColumn("Err(Sphere)",sphere.getErrNorm());
}

void View3D::slot_fitPlan()
{
    Plan plan;
    cloud->fit(&plan);

    Eigen::Vector3d N=plan.getNormal();

    addPlan(QPosAtt( cloud->getBarycenter(),
                     Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,1,0),N)),1.0,QColor(64,64,64),
            2*cloud->getBoundingRadius(),2*cloud->getBoundingRadius());
    addPlan(QPosAtt( cloud->getBarycenter(),
                     Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,1,0),-N)),1.0,QColor(64,64,64),
            2*cloud->getBoundingRadius(),2*cloud->getBoundingRadius());


    emit sig_displayResults( QString("Fit Plan:\nNormal=+-(%1 , %2 , %3)\nRms=%4\n").arg(N[0]).arg(N[1]).arg(N[2]).arg(plan.getRMS()));
    emit sig_newColumn("Err(Plan)",plan.getErrNorm());
}

void View3D::slot_fitCustomMesh()
{

}

//----------------------------

QQuaternion toQQuaternion(Eigen::Quaterniond q)
{
    return QQuaternion(float(q.w()),float(q.x()),float(q.y()),float(q.z()));
}
QVector3D toQVector3D(Eigen::Vector3d v)
{
    return QVector3D(float(v.x()),float(v.y()),float(v.z()));
}

View3D::View3D()
{
    cloud=nullptr;


    current_filename.clear();

    //renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::RenderPolicy::OnDemand);
    defaultFrameGraph()->setClearColor(QColor(255,255,255));
    defaultFrameGraph()->setFrustumCullingEnabled(true);


    QWidget* container = QWidget::createWindowContainer(this);
    container->setMinimumSize(QSize(512, 512));
    customContainer=new CustomViewContainer(container);
    //container->setMaximumSize(screen()->size());
    //registerAspect(new Qt3DInput::QInputAspect);

    //Camera
    camera_params=new CameraParams(camera(),0,0,0);
    camera_params->entity()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.0001f, 100000.0f);
    camera_params->reset();

    //Racine
    rootEntity = new Qt3DCore::QEntity();
    setRootEntity(rootEntity);

    ///////////////////////////////////////////////////////////////////////////




    ///////////////////////////////////////////////////////////////////////////

    //Culling


    //Camera controler
    //        Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    //        camController->setCamera(cameraEntity);

    //Light
    Qt3DCore::QEntity* lightEntity_a = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight* light_a = new Qt3DRender::QPointLight(lightEntity_a);
    light_a->setColor("white");
    light_a->setIntensity(1);
    lightEntity_a->addComponent(light_a);
    Qt3DCore::QTransform* lightTransform_a = new Qt3DCore::QTransform(lightEntity_a);
    lightTransform_a->setTranslation(QVector3D(5,5,-5));
    lightEntity_a->addComponent(lightTransform_a);

    Qt3DCore::QEntity* lightEntity_b = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight* light_b = new Qt3DRender::QPointLight(lightEntity_b);
    light_b->setColor("white");
    light_b->setIntensity(1);
    lightEntity_b->addComponent(light_b);
    Qt3DCore::QTransform* lightTransform_b = new Qt3DCore::QTransform(lightEntity_b);
    lightTransform_b->setTranslation(QVector3D(5,5,5));
    lightEntity_b->addComponent(lightTransform_b);

    Qt3DCore::QEntity* lightEntity_c = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight* light_c = new Qt3DRender::QPointLight(lightEntity_c);
    light_c->setColor("white");
    light_c->setIntensity(1);
    lightEntity_c->addComponent(light_c);
    Qt3DCore::QTransform* lightTransform_c = new Qt3DCore::QTransform(lightEntity_c);
    lightTransform_c->setTranslation(QVector3D(0,5,0));
    lightEntity_c->addComponent(lightTransform_c);


    init3D();
    createPopup();


    //addSphere(QPosAtt(Eigen::Vector3d(0.05,0.05,0.05),Eigen::Quaterniond(1,0,0,0)),1.0,QColor(128,128,128),0.01);
}

void View3D::addLabel(QString text, QVector3D coord, float scale, float anglex, float angley,float anglez)
{
    // Set label's text
    auto* textEntity = new Qt3DCore::QEntity();
    textEntity->setParent(rootEntity);

    auto* textMaterial = new Qt3DExtras::QPhongMaterial(rootEntity);
    textMaterial->setDiffuse(QColor(0,0,0));

    auto* textTransform = new Qt3DCore::QTransform();
    textTransform->setTranslation(coord);
    textTransform->setRotationX(anglex);
    textTransform->setRotationY(angley);
    textTransform->setRotationZ(anglez);
    textTransform->setScale(scale);

    auto textMesh = new Qt3DExtras::QExtrudedTextMesh();
    textMesh->setText(text);
    textMesh->setDepth(.001f);//flat


    textEntity->addComponent(textMaterial);
    textEntity->addComponent(textTransform);
    textEntity->addComponent(textMesh);
}

void View3D::addGrid(Cloud* cloud,
                     unsigned int N,
                     QColor color)
{


    auto* gridGeometry = new Qt3DRender::QGeometry(rootEntity);

    unsigned int n=(N+1)*3;

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * (4*n) * sizeof(float));
    float* positions = reinterpret_cast<float*>(bufferBytes.data());

    double stepX=cloud->getXRange().size()/(N-1);
    double stepY=cloud->getYRange().size()/(N-1);
    double stepZ=cloud->getZRange().size()/(N-1);

    float scale=cloud->getBoundingRadius()/N*0.75;
    addLabel(cloud->getLabelX(),QVector3D(cloud->getXRange().upper+scale*5,
                                          cloud->getYRange().lower,
                                          cloud->getZRange().center()+cloud->getLabelX().size()*scale),
             scale,-90,90,0);
    addLabel(cloud->getLabelY(),QVector3D(cloud->getXRange().upper+scale*5,
                                          cloud->getYRange().center(),
                                          cloud->getZRange().lower),scale,0,0,90);

    addLabel(cloud->getLabelZ(),QVector3D(cloud->getXRange().center(),
                                          cloud->getYRange().lower,
                                          cloud->getZRange().upper+stepZ+scale*4),scale,-90,0,0);

    //Tiks
    double dec=scale*2;
    for (unsigned int i=0; i<N+1; i++)
    {
        addLabel(QString("%1").arg( (i)*stepX+cloud->getXRange().lower,0,'g',2),
                 QVector3D(i*stepX+cloud->getXRange().lower,
                           cloud->getYRange().lower,
                           cloud->getZRange().upper+stepZ+dec),scale*0.5,-90,90,0);

        addLabel(QString("%1").arg( (i)*stepZ+cloud->getZRange().lower,0,'g',2),
                 QVector3D(cloud->getXRange().upper+stepX,
                           cloud->getYRange().lower,
                           (i)*stepZ+cloud->getZRange().lower),scale*0.5,-90,0,0);

        addLabel(QString("%1").arg( (i)*stepY+cloud->getYRange().lower,0,'g',2),
                 QVector3D(cloud->getXRange().upper+stepX,
                           (i)*stepY+cloud->getYRange().lower,
                           cloud->getZRange().lower),scale*0.5,0,0,0);
    }

    if (!cloud->getLabelS().isEmpty())
    {
        addLabel(cloud->getLabelS(),QVector3D(cloud->getXRange().upper+stepX,cloud->getYRange().lower-stepY,cloud->getZRange().upper+stepZ),scale,0,45,0);
    }

    for (unsigned int i=0; i<N+1; i++)
    {
        *positions++ = (i)*stepX+cloud->getXRange().lower;
        *positions++ = cloud->getYRange().lower;
        *positions++ = cloud->getZRange().upper+stepZ;

        *positions++ = (i)*stepX+cloud->getXRange().lower;
        *positions++ = cloud->getYRange().lower;
        *positions++ = cloud->getZRange().lower;

        *positions++ = cloud->getXRange().upper+stepX;
        *positions++ = cloud->getYRange().lower;
        *positions++ = (i)*stepZ+cloud->getZRange().lower;

        *positions++ = cloud->getXRange().lower;
        *positions++ = cloud->getYRange().lower;
        *positions++ = (i)*stepZ+cloud->getZRange().lower;
    }

    for (unsigned int i=0; i<N+1; i++)
    {
        *positions++ = cloud->getXRange().lower;
        *positions++ = (i)*stepY+cloud->getYRange().lower;
        *positions++ = cloud->getZRange().upper+stepZ;

        *positions++ = cloud->getXRange().lower;
        *positions++ = (i)*stepY+cloud->getYRange().lower;
        *positions++ = cloud->getZRange().lower;

        *positions++ = cloud->getXRange().lower;
        *positions++ = cloud->getYRange().upper+stepY;
        *positions++ = (i)*stepZ+cloud->getZRange().lower;

        *positions++ = cloud->getXRange().lower;
        *positions++ = cloud->getYRange().lower;
        *positions++ = (i)*stepZ+cloud->getZRange().lower;
    }

    for (unsigned int i=0; i<N+1; i++)
    {
        *positions++ = (i)*stepX+cloud->getXRange().lower;
        *positions++ = cloud->getYRange().upper+stepY;
        *positions++ = cloud->getZRange().lower;

        *positions++ = (i)*stepX+cloud->getXRange().lower;
        *positions++ = cloud->getYRange().lower;
        *positions++ = cloud->getZRange().lower;

        *positions++ = cloud->getXRange().upper+stepX;
        *positions++ = (i)*stepY+cloud->getYRange().lower;
        *positions++ = cloud->getZRange().lower;

        *positions++ = cloud->getXRange().lower;
        *positions++ = (i)*stepY+cloud->getYRange().lower;
        *positions++ = cloud->getZRange().lower;
    }

    auto* gridBuf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,gridGeometry);
    gridBuf->setData(bufferBytes);

    auto* gridPositionAttribute = new Qt3DRender::QAttribute(gridGeometry);
    gridPositionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    gridPositionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    gridPositionAttribute->setVertexSize(3);
    gridPositionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    gridPositionAttribute->setBuffer(gridBuf);
    gridPositionAttribute->setByteStride(3 * sizeof(float));
    gridPositionAttribute->setCount(4*n);
    gridGeometry->addAttribute(gridPositionAttribute); // We add the vertices in the geometry

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

    auto* gridIndexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,gridGeometry);
    gridIndexBuffer->setData(indexBytes);

    auto* gridIndexAttribute = new Qt3DRender::QAttribute(gridGeometry);
    gridIndexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    gridIndexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    gridIndexAttribute->setBuffer(gridIndexBuffer);
    gridIndexAttribute->setCount(4*n);
    gridGeometry->addAttribute(gridIndexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto* gridLine = new Qt3DRender::QGeometryRenderer(rootEntity);
    gridLine->setGeometry(gridGeometry);
    gridLine->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto* gridMaterial = new Qt3DExtras::QPhongMaterial(rootEntity);
    gridMaterial->setAmbient(color);

    // entity
    auto* gridEntity = new Qt3DCore::QEntity(rootEntity);
    gridEntity->addComponent(gridLine);
    gridEntity->addComponent(gridMaterial);
}

void View3D::init3D()
{
    mode=MODE_POINTS;

    cloudGeometry = new Qt3DRender::QGeometry(rootEntity);
    cloudBuf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,cloudGeometry);

    cloudPositionAttribute = new Qt3DRender::QAttribute(cloudGeometry);
    cloudPositionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    cloudPositionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    cloudPositionAttribute->setVertexSize(3);
    cloudPositionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    cloudPositionAttribute->setBuffer(cloudBuf);
    cloudPositionAttribute->setByteStride(6 * sizeof(float));
    cloudGeometry->addAttribute(cloudPositionAttribute); // We add the vertices in the geometry

    cloudColorsAttribute = new Qt3DRender::QAttribute(cloudGeometry);
    cloudColorsAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
    cloudColorsAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    cloudColorsAttribute->setVertexSize(3);
    cloudColorsAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    cloudColorsAttribute->setBuffer(cloudBuf);
    cloudColorsAttribute->setByteOffset(3 * sizeof(float));
    cloudColorsAttribute->setByteStride(6 * sizeof(float));

    cloudGeometry->addAttribute(cloudColorsAttribute);

    cloudMaterial = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);
    cloudPrimitives = new Qt3DRender::QGeometryRenderer(rootEntity);
    cloudPrimitives->setGeometry(cloudGeometry);

    cloudPrimitivesEntity = new Qt3DCore::QEntity(rootEntity);
    cloudPrimitivesEntity->addComponent(cloudPrimitives);
    cloudPrimitivesEntity->addComponent(cloudMaterial);

    pointSize = new Qt3DRender::QPointSize();
    pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
    pointSize->setValue(4.0f);
    lineWidth = new Qt3DRender::QLineWidth();
    lineWidth->setValue(4.0f);
}

void View3D::setCloudScalar(Cloud* cloud, PrimitiveMode primitiveMode)
{
    addGrid(cloud,10,QColor(128,128,128));
    camera_params->setBarycenter( Cloud::toQVec3D(cloud->getBarycenter()) );
    camera_params->setBoundingRadius( cloud->getBoundingRadius() );

    //Set Data Buffers
    QByteArray bufferBytes;
    bufferBytes.resize(2 * 3 * ( cloud->positions().size() ) * sizeof(float));
    float* vertices = reinterpret_cast<float*>(bufferBytes.data());

    for (int i=0; i<cloud->positions().size(); i++)
    {
        *vertices++ = cloud->positions()[i][0];
        *vertices++ = cloud->positions()[i][1];
        *vertices++ = cloud->positions()[i][2];

        *vertices++ = qRed  (cloud->getColors()[i])/255.0;
        *vertices++ = qGreen(cloud->getColors()[i])/255.0;
        *vertices++ = qBlue (cloud->getColors()[i])/255.0;
    }
    cloudBuf->setData(bufferBytes);
    cloudPositionAttribute->setCount(cloud->positions().size());
    cloudColorsAttribute->setCount(cloud->positions().size());

    // mesh
    if (primitiveMode==MODE_LINES)
    {
        cloudPrimitives->setPrimitiveType(Qt3DRender::QGeometryRenderer::LineStrip);
    }
    else
    {
        cloudPrimitives->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    }

    //points size
    auto effect = cloudMaterial->effect();
    for (auto t : effect->techniques())
    {
        for (auto rp : t->renderPasses())
        {
            rp->addRenderState(pointSize);
            rp->addRenderState(lineWidth);
        }
    }

    // entity
    mode=primitiveMode;
    camera_params->reset();

    //Keep a pointer to data
    this->cloud=cloud;
}

void View3D::slot_setPointSize(double value)
{
    pointSize->setValue(value);
    lineWidth->setValue(value);

    auto effect = cloudMaterial->effect();
    for (auto t : effect->techniques())
    {
        for (auto rp : t->renderPasses())
        {
            rp->addRenderState(pointSize);
            rp->addRenderState(lineWidth);
        }
    }
}

void View3D::slot_setPrimitiveType(int type)
{
    if (type==0)
    {
        mode=MODE_POINTS;
        cloudPrimitives->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    }
    else if (type==1)
    {
        mode=MODE_LINES;
        cloudPrimitives->setPrimitiveType(Qt3DRender::QGeometryRenderer::LineStrip);
    }
}

void View3D::addSphere(QPosAtt posatt,float scale,QColor color,double radius)
{
    auto* m_obj = new Qt3DExtras::QSphereMesh();
    m_obj->setRadius(radius);

    auto* t_obj = new Qt3DCore::QTransform();
    t_obj->setScale(scale);
    t_obj->setRotation(toQQuaternion(posatt.Q));
    t_obj->setTranslation(toQVector3D(posatt.P));

    auto* mat_obj = new Qt3DExtras::QPhongMaterial();
    mat_obj->setDiffuse(color);

    auto* m_objEntity = new Qt3DCore::QEntity(rootEntity);
    m_objEntity->addComponent(m_obj);
    m_objEntity->addComponent(mat_obj);
    m_objEntity->addComponent(t_obj);

    transforms.push_back(t_obj);
    materials.push_back(mat_obj);
}

void View3D::addPlan(QPosAtt posatt,float scale,QColor color,double width,double height)
{
    auto* m_obj = new Qt3DExtras::QPlaneMesh();
    m_obj->setWidth(width);
    m_obj->setHeight(height);

    auto* t_obj = new Qt3DCore::QTransform();
    t_obj->setScale(scale);
    t_obj->setRotation(toQQuaternion(posatt.Q));
    t_obj->setTranslation(toQVector3D(posatt.P));

    auto* mat_obj = new Qt3DExtras::QPhongMaterial();
    mat_obj->setDiffuse(color);

    auto* m_objEntity = new Qt3DCore::QEntity(rootEntity);
    m_objEntity->addComponent(m_obj);
    m_objEntity->addComponent(mat_obj);
    m_objEntity->addComponent(t_obj);

    transforms.push_back(t_obj);
    materials.push_back(mat_obj);
}

void View3D::addObj(QString filename, QPosAtt posatt,float scale,QColor color)
{
    auto* m_obj = new Qt3DRender::QMesh();
    m_obj->setSource(QUrl(QString("file:///")+filename));

    auto* t_obj = new Qt3DCore::QTransform();
    t_obj->setScale(scale);
    t_obj->setRotation(toQQuaternion(posatt.Q));
    t_obj->setTranslation(toQVector3D(posatt.P));

    auto* mat_obj = new Qt3DExtras::QPhongMaterial();
    mat_obj->setDiffuse(color);

    //    Qt3DExtras::QPerVertexColorMaterial * mat_obj = new Qt3DExtras::QPerVertexColorMaterial();

    auto* m_objEntity = new Qt3DCore::QEntity(rootEntity);
    m_objEntity->addComponent(m_obj);
    m_objEntity->addComponent(mat_obj);
    m_objEntity->addComponent(t_obj);

    transforms.push_back(t_obj);
    materials.push_back(mat_obj);
}

void View3D::setObjColor(unsigned int id,QColor color)
{
    if (id<materials.size())
    {
        materials[id]->setDiffuse(color);
    }
}

void View3D::setObjPosAtt(unsigned int id,const QPosAtt& T)
{
    if (id<transforms.size())
    {
        transforms[id]->setTranslation(toQVector3D(T.P)*1e-3f);
        transforms[id]->setRotation(toQQuaternion(T.Q));
    }
}

CustomViewContainer* View3D::getContainer()
{
    return customContainer;
}

void View3D::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        float dx=xp-event->x();
        float dy=yp-event->y();

        camera_params->move(dx,dy);

        xp=event->x();
        yp=event->y();
    }
}

void View3D::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        camera_params->reset();
    }
}

void View3D::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        xp=event->x();
        yp=event->y();
    }

    if (event->button() == Qt::RightButton)
    {
        popup_menu->exec(mapToGlobal(event->pos()));
    }
}
void View3D::wheelEvent(QWheelEvent* event)
{
    float dw=event->delta();
    if (dw>0)
    {
        if (camera_params->getRadius()<10*camera_params->getBoundingRadius())
        {
            camera_params->setRadius(camera_params->getRadius()*dw*0.01f);
        }
    }
    else
    {
        if (camera_params->getRadius()>camera_params->getBoundingRadius())
        {
            camera_params->setRadius(camera_params->getRadius()/(std::abs(dw)*0.01f));
        }
    }
}


