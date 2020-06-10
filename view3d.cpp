#include "view3d.h"


CustomViewContainer::CustomViewContainer(QWidget* container)
{
    QGridLayout* glayout=new QGridLayout(this);

    axisSize=100;
    createColorAxisPlot();
    createXAxisPlot();
    createYAxisPlot();
    createZAxisPlot();

    glayout->addWidget(axisX_plot,2,1);
    glayout->addWidget(axisY_plot,1,0);
    glayout->addWidget(axisZ_plot,0,1);
    glayout->addWidget(color_plot,1,2);
    glayout->addWidget(container,1,1);

    //white
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    this->container=container;


}

void CustomViewContainer::createColorAxisPlot()
{
    color_plot=new QCustomPlot(this);
    scale=new QCPColorScale(color_plot);
    scale->setRangeDrag(true);
    scale->setRangeZoom(true);
    color_plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    color_plot->plotLayout()->clear();
    color_plot->plotLayout()->addElement(scale);

    color_plot->setMaximumWidth(axisSize);
}

void CustomViewContainer::createXAxisPlot()
{
    axisX_plot=new QCustomPlot(this);

    axisX_rect=new QCPAxisRect(axisX_plot,false);
    axisX=new QCPAxis(axisX_rect,QCPAxis::AxisType::atBottom);
    axisX_rect->addAxis(QCPAxis::AxisType::atBottom,axisX);
    axisX_plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    axisX_rect->setRangeDrag(Qt::Horizontal);
    axisX_rect->setRangeZoom(Qt::Horizontal);
    axisX_rect->setRangeDragAxes(axisX,nullptr);
    axisX_rect->setRangeZoomAxes(axisX,nullptr);
    axisX_plot->plotLayout()->clear();
    axisX_plot->plotLayout()->addElement(axisX_rect);
    axisX->setPadding(0);
    axisX->setLabelPadding(0);


    axisX_plot->setMaximumHeight(axisSize);
}

void CustomViewContainer::createYAxisPlot()
{
    axisY_plot=new QCustomPlot(this);

    axisY_rect=new QCPAxisRect(axisY_plot,false);
    axisY=new QCPAxis(axisY_rect,QCPAxis::AxisType::atLeft);
    axisY_rect->addAxis(QCPAxis::AxisType::atLeft,axisY);
    axisY_plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    axisY_rect->setRangeDrag(Qt::Vertical);
    axisY_rect->setRangeZoom(Qt::Vertical);
    axisY_rect->setRangeDragAxes(nullptr,axisY);
    axisY_rect->setRangeZoomAxes(nullptr,axisY);
    axisY_plot->plotLayout()->clear();
    axisY_plot->plotLayout()->addElement(axisY_rect);
    axisY->setPadding(0);
    axisY->setLabelPadding(0);

    axisY_plot->setMaximumWidth(axisSize);
}

void CustomViewContainer::createZAxisPlot()
{
    axisZ_plot=new QCustomPlot(this);

    axisZ_rect=new QCPAxisRect(axisZ_plot,false);
    axisZ=new QCPAxis(axisZ_rect,QCPAxis::AxisType::atTop);
    axisZ_rect->addAxis(QCPAxis::AxisType::atTop,axisZ);
    axisZ_plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    axisZ_rect->setRangeDrag(Qt::Horizontal);
    axisZ_rect->setRangeZoom(Qt::Horizontal);
    axisZ_rect->setRangeDragAxes(axisZ,nullptr);
    axisZ_rect->setRangeZoomAxes(axisZ,nullptr);
    axisZ_plot->plotLayout()->clear();
    axisZ_plot->plotLayout()->addElement(axisZ_rect);
    axisZ->setPadding(0);
    axisZ->setLabelPadding(0);

    axisZ_plot->setMaximumHeight(axisSize);
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

    sb_size=new QDoubleSpinBox;
    sb_size->setRange(0.1,100);
    sb_size->setSingleStep(1.0);

    cb_mode=new QComboBox;
    cb_mode->addItem("POINTS");
    cb_mode->addItem("LINES");

    c_gradient=new QComboBox;
    c_gradient->addItem("gpGrayscale",QCPColorGradient::GradientPreset::gpGrayscale);
    c_gradient->addItem("gpHot",QCPColorGradient::GradientPreset::gpHot);
    c_gradient->addItem("gpCold",QCPColorGradient::GradientPreset::gpCold);
    c_gradient->addItem("gpNight",QCPColorGradient::GradientPreset::gpNight);
    c_gradient->addItem("gpCandy",QCPColorGradient::GradientPreset::gpCandy);
    c_gradient->addItem("gpGeography",QCPColorGradient::GradientPreset::gpGeography);
    c_gradient->addItem("gpIon",QCPColorGradient::GradientPreset::gpIon);
    c_gradient->addItem("gpThermal",QCPColorGradient::GradientPreset::gpThermal);
    c_gradient->addItem("gpPolar",QCPColorGradient::GradientPreset::gpPolar);
    c_gradient->addItem("gpSpectrum",QCPColorGradient::GradientPreset::gpSpectrum);
    c_gradient->addItem("gpJet",QCPColorGradient::GradientPreset::gpJet);
    c_gradient->addItem("gpHues",QCPColorGradient::GradientPreset::gpHues);

    gbox->addWidget(cb_mode,0,0);
    gbox->addWidget(sb_size,0,1);
    gbox->addWidget(c_gradient,1,0);

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
    QObject::connect(c_gradient, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setGradient(int)));
}

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

    emit sig_displayResults( QString("Fit Plan:\nNormal=+-(%1 , %2 , %3)\nRms=%4\n").arg(N[0]).arg(N[1]).arg(N[2]).arg(plan.getRMS()));
    emit sig_newColumn("Err(Plan)",plan.getErrNorm());
}

void View3D::slot_fitCustomMesh()
{
    QString filename=QFileDialog::getOpenFileName(nullptr,"3D Mesh","./obj","Object (*.obj)");

    QElapsedTimer timer;
    timer.start();

    auto* m_obj = new Qt3DRender::QMesh();
    m_obj->setSource(QUrl(QString("file:///")+filename));

    Object obj(filename,1.0,QPosAtt());

    obj.setScalePosAtt(cloud->getBoundingRadius()/obj.getRadius(),
                       QPosAtt(cloud->getBarycenter()-obj.getBox().middle(),Eigen::Quaterniond(1,0,0,0)));


    cloud->fit(&obj);

    addObj(m_obj,obj.getPosAtt(),obj.getScale(),QColor(64,64,64));

    emit sig_displayResults( QString("Fit Mesh :\nScale=%1\nPosition=(%2,%3,%4)\nQ=(%5,%6,%7,%8)\nRms=%9\ndt=%10 ms")
                             .arg(obj.getScale())
                             .arg(obj.getPosAtt().P[0]).arg(obj.getPosAtt().P[1]).arg(obj.getPosAtt().P[2])
                             .arg(obj.getPosAtt().Q.w()).arg(obj.getPosAtt().Q.x()).arg(obj.getPosAtt().Q.y()).arg(obj.getPosAtt().Q.w())
                             .arg(obj.getRMS())
                             .arg(timer.nsecsElapsed()*1e-6));

    emit sig_newColumn("Err(Mesh)",obj.getErrNorm());
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
    xy_reversed=false;
    yz_reversed=false;
    xz_reversed=false;

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

    connect(customContainer->getXAxis(),SIGNAL(rangeChanged(const QCPRange&)),this,SLOT(slot_ScaleChanged()));
    connect(customContainer->getYAxis(),SIGNAL(rangeChanged(const QCPRange&)),this,SLOT(slot_ScaleChanged()));
    connect(customContainer->getZAxis(),SIGNAL(rangeChanged(const QCPRange&)),this,SLOT(slot_ScaleChanged()));
    connect(customContainer->getColorScale(),SIGNAL(dataRangeChanged(const QCPRange&)),this,SLOT(slot_ColorScaleChanged(const QCPRange&)));
    //addSphere(QPosAtt(Eigen::Vector3d(0.05,0.05,0.05),Eigen::Quaterniond(1,0,0,0)),1.0,QColor(128,128,128),0.01);


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

void View3D::createGrid(unsigned int N,
                        QColor color)
{
    auto* gridGeometry = new Qt3DRender::QGeometry(rootEntity);

    unsigned int n=(N+1)*3;

    gridBuf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,gridGeometry);
    gridBuf->setData(getGridBuffer(false,false,false,N));

    gridPositionAttribute = new Qt3DRender::QAttribute(gridGeometry);
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

    cloudTransform = new Qt3DCore::QTransform();
    cloudTransform->setScale(1.0);
    cloudTransform->setRotation(QQuaternion(1,0,0,0));
    cloudTransform->setTranslation(QVector3D(0,0,0));

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
    cloudPrimitivesEntity->addComponent(cloudTransform);

    pointSize = new Qt3DRender::QPointSize();
    pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
    pointSize->setValue(4.0f);
    lineWidth = new Qt3DRender::QLineWidth();
    lineWidth->setValue(4.0f);

    createGrid(10,QColor(128,128,128));

    labelx=new Label3D(rootEntity,"XXXXXX",QVector3D(0,1,-1),0.1,0,0,0);
    labely=new Label3D(rootEntity,"YYYYYY",QVector3D(-1,0,1),0.1,0,90,90);
    labelz=new Label3D(rootEntity,"ZZZZZZ",QVector3D(1,-1,0),0.1,-90,-90,0);
}

void View3D::slot_ColorScaleChanged(const QCPRange& range)
{
    if (cloud && cloudBuf)
    {
        cloudBuf->setData(cloud->getColorBuffer(range));
    }
}

void View3D::slot_ScaleChanged()
{
    Qt3DCore::QTransform t;
    t.setTranslation(customContainer->getTranslation());
    t.setScale3D(customContainer->getScale());
    cloudTransform->setMatrix(t.matrix().inverted());

    for (int i=0; i<transforms.size(); i++)
    {
        transforms[i]->setMatrix(t.matrix().inverted()*baseTransforms[i]);
    }
}

void View3D::slot_setGradient(int preset)
{
    if (cloud && cloudBuf)
    {
        cloud->setGradientPreset(static_cast<QCPColorGradient::GradientPreset>(preset));
        customContainer->getColorScale()->setGradient(cloud->getGradient());
        cloudBuf->setData(cloud->getColorBuffer(customContainer->getColorScale()->dataRange()));
        customContainer->getColorScalePlot()->rescaleAxes();
        customContainer->getColorScalePlot()->replot();
    }
}

void View3D::setCloudScalar(Cloud* cloud, PrimitiveMode primitiveMode)
{
    this->cloud=cloud;

    camera_params->setBarycenter( QVector3D(0,0,0) );
    camera_params->setBoundingRadius( 1.20 );

    //Set Data Buffers
    //cloudBuf->setData(cloud->getBuffer());
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

    customContainer->getColorScale()->axis()->setLabel(cloud->getLabelS());
    customContainer->getColorScale()->setGradient(cloud->getGradient());
    customContainer->getColorScale()->setDataRange(cloud->getScalarFieldRange());
    customContainer->getColorScalePlot()->rescaleAxes();
    customContainer->getColorScalePlot()->replot();

    customContainer->getXAxis()->setRange(cloud->getXRange());
    customContainer->getYAxis()->setRange(cloud->getYRange());
    customContainer->getZAxis()->setRange(cloud->getZRange());

    customContainer->getXAxis()->setLabel(cloud->getLabelX());
    customContainer->getYAxis()->setLabel(cloud->getLabelY());
    customContainer->getZAxis()->setLabel(cloud->getLabelZ());

    labelx->setText(cloud->getLabelX());
    labely->setText(cloud->getLabelY());
    labelz->setText(cloud->getLabelZ());

    customContainer->adjustSize();
    customContainer->replot();
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

    addObj(reinterpret_cast<Qt3DRender::QMesh*>(m_obj),posatt,scale,color);
}

void View3D::addPlan(QPosAtt posatt,float scale,QColor color,double width,double height)
{
    auto* m_obj = new Qt3DExtras::QPlaneMesh();
    m_obj->setWidth(width);
    m_obj->setHeight(height);

    addObj(reinterpret_cast<Qt3DRender::QMesh*>(m_obj),posatt,scale,color);
}

void View3D::addObj(Qt3DRender::QMesh* m_obj, QPosAtt posatt,float scale,QColor color)
{
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

    Qt3DRender::QCullFace* culling = new Qt3DRender::QCullFace();
    culling->setMode(Qt3DRender::QCullFace::NoCulling);
    auto effect = mat_obj->effect();
    for (auto t : effect->techniques())
    {
        for (auto rp : t->renderPasses())
        {
            rp->addRenderState(culling);
        }
    }

    baseTransforms.push_back(t_obj->matrix());
    transforms.push_back(t_obj);
    materials.push_back(mat_obj);

    slot_ScaleChanged();
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



void View3D::updateLabels()
{
    labelz->setPosRot(QVector3D((yz_reversed?-1:1),(xy_reversed?1:-1),0),
                      -90+(xy_reversed?180:0),
                      (yz_reversed?-270:-90),
                      (xy_reversed?180:0));

    if (!xz_reversed)
    {
        labelx->setPosRot(QVector3D(0,(xy_reversed?-1:1),-1),(xy_reversed?180:0),0,0);
    }
    else
    {
        labelx->setPosRot(QVector3D(0,(xy_reversed?-1:1),1),(xy_reversed?180:0),-180,0);
    }

    if (!yz_reversed)
    {
        labely->setPosRot(QVector3D(-1,0,(xz_reversed?-1:1)),(xz_reversed?180:0),90+(xz_reversed?180:0),90);
    }
    else
    {
        labely->setPosRot(QVector3D(1,0,(xz_reversed?-1:1)),(xz_reversed?180:0)+180,90+(xz_reversed?180:0),90);
    }
}

void View3D::updateGrid()
{
    if (camera_params->getBeta()>0 && xy_reversed==false)
    {
        xy_reversed=true;
        gridBuf->setData(getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }
    else if (camera_params->getBeta()<0 && xy_reversed==true)
    {
        xy_reversed=false;
        gridBuf->setData(getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }

    if ( cos(camera_params->getAlpha())>0 && yz_reversed==false)
    {
        yz_reversed=true;
        gridBuf->setData(getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }
    else if ( cos(camera_params->getAlpha())<0 && yz_reversed==true)
    {
        yz_reversed=false;
        gridBuf->setData(getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }

    if ( sin(camera_params->getAlpha())>0 && xz_reversed==false)
    {
        xz_reversed=true;
        gridBuf->setData(getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }
    else if ( sin(camera_params->getAlpha())<0 && xz_reversed==true)
    {
        xz_reversed=false;
        gridBuf->setData(getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }
}

void View3D::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        float dx=xp-event->x();
        float dy=yp-event->y();

        camera_params->move(dx,dy);

        updateGrid();

        xp=event->x();
        yp=event->y();
    }
}

void View3D::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        camera_params->reset();

        customContainer->getXAxis()->setRange(cloud->getXRange());
        customContainer->getYAxis()->setRange(cloud->getYRange());
        customContainer->getZAxis()->setRange(cloud->getZRange());
        customContainer->getColorScale()->setDataRange(cloud->getScalarFieldRange());
        customContainer->getColorScalePlot()->rescaleAxes();
        customContainer->replot();


        updateGrid();
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
        sb_size->blockSignals(true);
        cb_mode->blockSignals(true);
        c_gradient->blockSignals(true);


        sb_size->setValue(static_cast<double>(pointSize->value()));
        cb_mode->setCurrentIndex(static_cast<int>(mode));
        c_gradient->setCurrentIndex(static_cast<int>(cloud->getGradientPreset()));

        sb_size->blockSignals(false);
        cb_mode->blockSignals(false);
        c_gradient->blockSignals(false);

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


