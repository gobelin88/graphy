#include "view3d.h"

View3D::View3D()
{
    cloud=nullptr;
    xy_reversed=false;
    yz_reversed=false;
    xz_reversed=false;

    current_filename.clear();

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

    //Light
    new Light3D(rootEntity, "white",QVector3D(5,5,5));
    new Light3D(rootEntity, "white",QVector3D(5,5,-5));
    new Light3D(rootEntity, "white",QVector3D(-5,5,5));
    new Light3D(rootEntity, "white",QVector3D(-5,5,-5));

    init3D();

    connect(customContainer->getXAxis(),SIGNAL(rangeChanged(const QCPRange&)),this,SLOT(slot_ScaleChanged()));
    connect(customContainer->getYAxis(),SIGNAL(rangeChanged(const QCPRange&)),this,SLOT(slot_ScaleChanged()));
    connect(customContainer->getZAxis(),SIGNAL(rangeChanged(const QCPRange&)),this,SLOT(slot_ScaleChanged()));
    connect(customContainer->getColorScale(),SIGNAL(dataRangeChanged(const QCPRange&)),this,SLOT(slot_ColorScaleChanged(const QCPRange&)));
    //addSphere(QPosAtt(Eigen::Vector3d(0.05,0.05,0.05),Eigen::Quaterniond(1,0,0,0)),1.0,QColor(128,128,128),0.01);

    createPopup();
}

void View3D::init3D()
{
    mode=MODE_POINTS;

    cloud3D=new Cloud3D(rootEntity);

    grid3D=new Grid3D(rootEntity,10,QColor(255,255,255));

    auto* meshArrow = new Qt3DRender::QMesh();
    meshArrow->setSource(QUrl( QUrl::fromLocalFile(":/obj/obj/axis.obj") ) );

    objArrowX=new Object3D(rootEntity,meshArrow,QPosAtt(Eigen::Vector3d(-0.0,1,-1),Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(1,0,0),Eigen::Vector3d(0,-1,0))),0.1f,QColor(255,0,0));
    objArrowY=new Object3D(rootEntity,meshArrow,QPosAtt(Eigen::Vector3d(-1,-0.0,1),Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,1,0),Eigen::Vector3d(0,1,0))),0.1f,QColor(0,255,0));
    objArrowZ=new Object3D(rootEntity,meshArrow,QPosAtt(Eigen::Vector3d(1,-1,-0.0),Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,0,1),Eigen::Vector3d(0,-1,0))),0.1f,QColor(0,0,255));

    labelx=new Label3D(rootEntity,"XXXXXX",QVector3D(0,1,-1),0.1f,0,0,0);
    labely=new Label3D(rootEntity,"YYYYYY",QVector3D(-1,0,1),0.1f,0,90,90);
    labelz=new Label3D(rootEntity,"ZZZZZZ",QVector3D(1,-1,0),0.1f,-90,-90,0);
    updateLabels();
}

void View3D::createPopup()
{
    popup_menu=new QMenu();

    menuParameters= new QMenu("Parameters");

    actSave   = new QAction("Save",  this);
    actSaveRevolution   = new QAction("Save a revolution",  this);

    menuData= new QMenu("Point cloud");
    actExport= new QAction("Export",  this);

    menuFit= new QMenu("Fit");
    actFitSphere= new QAction("Sphere",  this);
    actFitPlan= new QAction("Plan",  this);
    actFitMesh= new QAction("Custom mesh",  this);

    menuProject= new QMenu("Project");
    actProjectSphere= new QAction("Sphere",  this);
    actProjectPlan= new QAction("Plan",  this);
    actProjectMesh= new QAction("Custom mesh",  this);

    menuSubSample= new QMenu("Subsample");
    actRandomSubSample= new QAction("Random",  this);

    menuView= new QMenu("View");
    actRescale= new QAction("Rescale",  this);

    menuMesh= new QMenu("Mesh");
    actMeshLoad= new QAction("Load",  this);
    actMeshCreateRotegrity= new QAction("Rotegrity",  this);;

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
    cb_mode->addItem("LINE_STRIP");
    cb_mode->addItem("TRIANGLES");
    cb_mode->addItem("TRIANGLE_STRIP");

    c_gradient=new QGradientComboBox(nullptr);

    gbox->addWidget(cb_mode,0,0);
    gbox->addWidget(sb_size,0,1);
    gbox->addWidget(c_gradient,1,0);

    widget->setLayout(gbox);

    ///////////////////////////////////////////////
    popup_menu->addMenu(menuParameters);
    popup_menu->addMenu(menuView);
    popup_menu->addSeparator();
    popup_menu->addMenu(menuData);
    popup_menu->addMenu(menuMesh);
    popup_menu->addSeparator();
    popup_menu->addAction(actSave);
    popup_menu->addAction(actSaveRevolution);

    menuView->addAction(actRescale);
    menuParameters->addAction(actWidget);
    menuData->addMenu(menuFit);
    menuData->addMenu(menuProject);
    menuData->addMenu(menuSubSample);
    menuData->addAction(actExport);

    menuFit->addAction(actFitSphere);
    menuFit->addAction(actFitPlan);
    menuFit->addAction(actFitMesh);

    menuProject->addAction(actProjectSphere);
    menuProject->addAction(actProjectPlan);
    menuProject->addAction(actProjectMesh);

    menuMesh->addAction(actMeshLoad);
    menuMesh->addAction(actMeshCreateRotegrity);

    menuSubSample->addAction(actRandomSubSample);

    QObject::connect(actMeshLoad,SIGNAL(triggered()),this,SLOT(slot_addMesh()));
    QObject::connect(actMeshCreateRotegrity,SIGNAL(triggered()),this,SLOT(slot_createRotegrity()));

    QObject::connect(actExport,SIGNAL(triggered()),this,SLOT(slot_export()));
    QObject::connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_resetView()));

    QObject::connect(actRandomSubSample,SIGNAL(triggered()),this,SLOT(slot_randomSubSamples()));
    QObject::connect(actSave,SIGNAL(triggered()),this,SLOT(slot_saveImage()));
    QObject::connect(actSaveRevolution,SIGNAL(triggered()),this,SLOT(slot_saveRevolution()));
    QObject::connect(sb_size, SIGNAL(valueChanged(double)), this, SLOT(slot_setPointSize(double)));
    QObject::connect(cb_mode, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setPrimitiveType(int)));

    QObject::connect(actProjectMesh, SIGNAL(triggered() ), this, SLOT(slot_projectCustomMesh()));
    QObject::connect(actProjectPlan, SIGNAL(triggered() ), this, SLOT(slot_projectPlan()));
    QObject::connect(actProjectSphere, SIGNAL(triggered() ), this, SLOT(slot_projectSphere()));

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

    QScreen* screen=QGuiApplication::primaryScreen();
    if (!filename.isEmpty())
    {
        this->current_filename=filename;

        screen->grabWindow(customContainer->winId()).save(filename);
    }

}

void View3D::slot_saveRevolution()
{
    Qt3DRender::QRenderCapture renderCapture(rootEntity);

    QFileInfo info(current_filename);
    QString where=info.path();

    QString filename=QFileDialog::getSaveFileName(nullptr,"Save Images",where,"(*.png)");
    if (!filename.isEmpty())
    {
        this->current_filename=filename;

        int N=QInputDialog::getInt(nullptr,"Number of frames","Number of frames=",1,1,360*10,1);
        //QScreen* screen=QGuiApplication::primaryScreen();

        QFileInfo info(filename);

        Qt3DRender::QRenderCapture* capture = new Qt3DRender::QRenderCapture;
        this->activeFrameGraph()->setParent(capture);
        this->setActiveFrameGraph(capture);

        MyCapture myCap(capture);
        float a0=camera_params->getAlpha();
        QScreen* screen=QGuiApplication::primaryScreen();

        for (int k=0; k<N; k++)
        {
            QString image_filename=info.dir().path()+QString("/%1_").arg(k)+info.baseName()+QString(".png");
            std::cout<<image_filename.toLocal8Bit().data()<<std::endl;
            camera_params->moveTo(a0+float(2.0*M_PI/N*k),camera_params->getBeta(),camera_params->getRadius());
            updateGridAndLabels();
            //this->customContainer->update();
            //QApplication::processEvents();

            myCap.capture();
            screen->grabWindow(customContainer->winId()).save(image_filename);
            //myCap.capture().save(image_filename);
        }
    }

}


void View3D::slot_fitSphere()
{
    Sphere sphere(cloud->getBarycenter(),cloud->getBoundingRadius()/2.0);
    cloud->fit(&sphere);

    Eigen::Vector3d C=sphere.getCenter();

    addSphere(&sphere,QColor(64,64,64));


    //addSphere(QPosAtt(C,Eigen::Quaterniond(1,0,0,0)),1.0,QColor(64,64,64),sphere.getRadius());

    emit sig_displayResults( QString("Fit Sphere:\nCenter=(%1 , %2 , %3) Radius=%4\nRms=%5\n").arg(C[0]).arg(C[1]).arg(C[2]).arg(sphere.getRadius()).arg(sphere.getRMS()) );
    emit sig_newColumn("Err_Sphere",sphere.getErrNorm());
}

void View3D::slot_fitPlan()
{
    Eigen::Vector3d barycenter=cloud->getBarycenter();

    Plan* plan;
    Plan planA(Eigen::Vector3d(1,0,0),barycenter);
    Plan planB(Eigen::Vector3d(0,1,0),barycenter);
    Plan planC(Eigen::Vector3d(0,0,1),barycenter);
    cloud->fit(&planA);
    cloud->fit(&planB);
    cloud->fit(&planC);

    if (planA.getRMS()<planB.getRMS() && planA.getRMS()<planC.getRMS())
    {
        plan=&planA;
    }
    else if (planB.getRMS()<planA.getRMS() && planB.getRMS()<planC.getRMS())
    {
        plan=&planB;
    }
    else
    {
        plan=&planC;
    }

    Eigen::Vector3d N=plan->getNormal();

    addPlan(plan,cloud->getBoundingRadius()*2,QColor(128,128,128));

    emit sig_displayResults( QString("Fit Plan:\nNormal=+-(%1 , %2 , %3)\nRms=%4\n").arg(N[0]).arg(N[1]).arg(N[2]).arg(plan->getRMS()));
    emit sig_newColumn("Err_Plan",plan->getErrNorm());
}


void View3D::slot_projectPlan()
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("project on a plan");
    dialog->setMinimumWidth(400);
    QGridLayout* gbox = new QGridLayout();
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QDoubleSpinBox* sb_nx=new QDoubleSpinBox(dialog);
    sb_nx->setValue(1);
    sb_nx->setPrefix("nx=");
    sb_nx->setRange(-1,1);
    QDoubleSpinBox* sb_ny=new QDoubleSpinBox(dialog);
    sb_ny->setValue(0);
    sb_ny->setPrefix("ny=");
    sb_ny->setRange(-1,1);
    QDoubleSpinBox* sb_nz=new QDoubleSpinBox(dialog);
    sb_nz->setValue(0);
    sb_nz->setPrefix("nz=");
    sb_nz->setRange(-1,1);

    Eigen::Vector3d barycenter=cloud->getBarycenter();
    QDoubleSpinBox* sb_bx=new QDoubleSpinBox(dialog);
    sb_bx->setValue(barycenter.x());
    sb_bx->setPrefix("bx=");
    sb_bx->setRange(-1e12,1e12);
    QDoubleSpinBox* sb_by=new QDoubleSpinBox(dialog);
    sb_by->setValue(barycenter.y());
    sb_by->setPrefix("by=");
    sb_by->setRange(-1e12,1e12);
    QDoubleSpinBox* sb_bz=new QDoubleSpinBox(dialog);
    sb_bz->setValue(barycenter.z());
    sb_bz->setPrefix("bz=");
    sb_bz->setRange(-1e12,1e12);

    gbox->addWidget(new QLabel("Normal"),0,0);
    gbox->addWidget(sb_nx,0,1);
    gbox->addWidget(sb_ny,0,2);
    gbox->addWidget(sb_nz,0,3);

    gbox->addWidget(new QLabel("Barycenter"),1,0);
    gbox->addWidget(sb_bx,1,1);
    gbox->addWidget(sb_by,1,2);
    gbox->addWidget(sb_bz,1,3);

    gbox->addWidget(buttonBox,2,0,1,2);

    dialog->setLayout(gbox);
    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        Plan plan(Eigen::Vector3d(sb_nx->value(),sb_ny->value(),sb_nz->value()),
                  Eigen::Vector3d(sb_bx->value(),sb_by->value(),sb_bz->value()));

        cloud->project(&plan);
        cloud3D->buffer->setData(cloud->getBuffer(customContainer->getColorScale()->dataRange()) );
    }
}

void View3D::slot_projectSphere()
{//todo dialog
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("project on a Sphere");
    dialog->setMinimumWidth(400);
    QGridLayout* gbox = new QGridLayout();
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QDoubleSpinBox* sb_radius=new QDoubleSpinBox(dialog);
    sb_radius->setValue(cloud->getBoundingRadius());
    sb_radius->setPrefix("radius=");
    sb_radius->setRange(-10000000,10000000);

    Eigen::Vector3d barycenter=cloud->getBarycenter();
    QDoubleSpinBox* sb_bx=new QDoubleSpinBox(dialog);
    sb_bx->setValue(barycenter.x());
    sb_bx->setPrefix("bx=");
    sb_bx->setRange(-1e12,1e12);
    QDoubleSpinBox* sb_by=new QDoubleSpinBox(dialog);
    sb_by->setValue(barycenter.y());
    sb_by->setPrefix("by=");
    sb_by->setRange(-1e12,1e12);
    QDoubleSpinBox* sb_bz=new QDoubleSpinBox(dialog);
    sb_bz->setValue(barycenter.z());
    sb_bz->setPrefix("bz=");
    sb_bz->setRange(-1e12,1e12);

    gbox->addWidget(new QLabel("Radius"),0,0);
    gbox->addWidget(sb_radius,0,1);

    gbox->addWidget(new QLabel("Barycenter"),1,0);
    gbox->addWidget(sb_bx,1,1);
    gbox->addWidget(sb_by,1,2);
    gbox->addWidget(sb_bz,1,3);

    gbox->addWidget(buttonBox,2,0,1,2);

    dialog->setLayout(gbox);
    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        Sphere sphere(Eigen::Vector3d(sb_bx->value(),sb_by->value(),sb_bz->value()),
                      sb_radius->value());

        cloud->project(&sphere);
        cloud3D->buffer->setData(cloud->getBuffer(customContainer->getColorScale()->dataRange()) );
    }
}

void View3D::slot_projectCustomMesh()
{
    QString filename=QFileDialog::getOpenFileName(nullptr,"Project on 3D Mesh","./obj","Object (*.obj)");
    std::cout<<filename.toLocal8Bit().data()<<std::endl;
    if(filename.isEmpty())return;

    Object obj(filename,QPosAtt());
    cloud->project(&obj);

    cloud3D->buffer->setData(cloud->getBuffer(customContainer->getColorScale()->dataRange()) );
    //setCloudScalar(cloud,PrimitiveMode::MODE_POINTS);
}

void View3D::slot_randomSubSamples()
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Random Sub Samples");
    dialog->setMinimumWidth(400);
    QGridLayout* gbox = new QGridLayout();
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QSpinBox* sb_subSamples=new QSpinBox(dialog);
    sb_subSamples->setValue(cloud->getBoundingRadius());
    sb_subSamples->setPrefix("Samples=");
    sb_subSamples->setRange(0,cloud->size());

    QSlider* sb_slideSubSamples=new QSlider(Qt::Horizontal,dialog);
    sb_slideSubSamples->setValue(cloud->getBoundingRadius());
    sb_slideSubSamples->setRange(0,cloud->size());

    QObject::connect(sb_slideSubSamples, SIGNAL(valueChanged(int)), sb_subSamples, SLOT(setValue(int)));
    QObject::connect(sb_subSamples, SIGNAL(valueChanged(int)), sb_slideSubSamples, SLOT(setValue(int)));
    sb_subSamples->setValue(cloud->size());

    gbox->addWidget(sb_subSamples,0,0);
    gbox->addWidget(sb_slideSubSamples,0,1,1,2);
    gbox->addWidget(buttonBox,1,0,1,3);

    dialog->setLayout(gbox);
    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        int nbPoints=sb_subSamples->value();
        cloud3D->positionAttribute   ->setCount(static_cast<unsigned int>(nbPoints));
        cloud3D->cloudColorsAttribute->setCount(static_cast<unsigned int>(nbPoints));
        cloud->subSample(nbPoints);

        if(nbPoints!=cloud->size())
        {
            std::cout<<"error : "<<nbPoints<<" "<<cloud->size()<<std::endl;
        }

        cloud3D->buffer->setData(cloud->getBuffer(customContainer->getColorScale()->dataRange()) );
    }
}

void View3D::slot_fitCustomMesh()
{
    QString filename=QFileDialog::getOpenFileName(nullptr,"3D Mesh","./obj","Object (*.obj)");
    std::cout<<filename.toLocal8Bit().data()<<std::endl;
    if(filename.isEmpty())return;

    QElapsedTimer timer;
    timer.start();

    Object obj(filename,QPosAtt());


    Vector3d obj_center=obj.getBox().middle();
    obj.setScalePosAtt(QPosAtt(cloud->getBarycenter()-obj_center,Eigen::Quaterniond(cloud->getBoundingRadius()/obj.getRadius(obj_center),0,0,0)));

    cloud->fit(&obj,100);

    QFileInfo info(filename);

    obj.save(info.path()+"/tmp.obj");

    auto* m_obj = new Qt3DRender::QMesh();
    m_obj->setSource(QUrl(QString("file:///")+info.path()+"/tmp.obj"));

    addObj(m_obj,QPosAtt(),1.0,QColor(64,64,64));

    QPosAtt posatt=obj.getPosAtt();
    emit sig_displayResults( QString("Fit Mesh :\nScale=%1\nPosition=(%2,%3,%4)\nQ=(%5,%6,%7,%8)\nRms=%9\ndt=%10 ms")
                             .arg(obj.getScale())
                             .arg(posatt.P[0]).arg(posatt.P[1]).arg(posatt.P[2])
            .arg(posatt.Q.w()).arg(posatt.Q.x()).arg(posatt.Q.y()).arg(posatt.Q.w())
            .arg(obj.getRMS())
            .arg(timer.nsecsElapsed()*1e-6));

    emit sig_newColumn("Err_Mesh",obj.getErrNorm());
}

//----------------------------




void View3D::slot_ColorScaleChanged(const QCPRange& range)
{
    if (cloud && cloud3D->buffer)
    {
        cloud3D->buffer->setData(cloud->getBuffer(range));
    }
}

void dispMat(QMatrix4x4 m)
{
    float* data=m.data();
    Eigen::Matrix3d M;
    Eigen::Matrix4d M4;

    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (i<3 && j<3)
            {
                M(i,j)=data[i*4+j];
            }
            M4(i,j)=data[i*4+j];
        }
    }

    std::cout<<M4<<" "<<M.determinant()<<std::endl;
}

void View3D::slot_ScaleChanged()
{
    customContainer->getScale();

    Qt3DCore::QTransform T,S;
    T.setTranslation(-customContainer->getTranslation());
    S.setScale3D(customContainer->getScaleInv());
    cloud3D->cloudTransform->setMatrix( S.matrix()*T.matrix() );//->setMatrix(t.matrix().inverted());

    //dispMat(T.matrix()*S.matrix());
    //dispMat(S.matrix()*T.matrix());

    for (int i=0; i<transforms.size(); i++)
    {
        transforms[i]->setMatrix( S.matrix()*T.matrix()*baseT[i]*baseR[i] );
    }

    for (int i=0; i<objects_list.size(); i++)
    {
        objects_list[i]->transform->setMatrix( S.matrix()*T.matrix() );
    }
}

void View3D::slot_setGradient(int preset)
{
    if (cloud && cloud3D->buffer)
    {
        cloud->setGradientPreset(static_cast<QCPColorGradient::GradientPreset>(preset));
        customContainer->getColorScale()->setGradient(cloud->getGradient());
        cloud3D->buffer->setData(cloud->getBuffer(customContainer->getColorScale()->dataRange()));
        customContainer->getColorScalePlot()->rescaleAxes();
        customContainer->getColorScalePlot()->replot();
    }
}

void View3D::setCloudScalar(Cloud* cloud, PrimitiveMode primitiveMode)
{
    this->cloud=cloud;

    camera_params->setBarycenter( QVector3D(0,0,0) );
    camera_params->setBoundingRadius( 1.20f );

    //Set Data Buffers
    cloud3D->positionAttribute->setCount(static_cast<unsigned int>(cloud->size()));
    cloud3D->cloudColorsAttribute->setCount(static_cast<unsigned int>(cloud->size()));

    // mesh
    slot_setPrimitiveType(primitiveMode);

    //points size
    slot_setPointSize(cloud3D->pointSize->value());

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
    cloud3D->pointSize->setValue(value);
    cloud3D->lineWidth->setValue(value);

    auto effect = cloud3D->cloudMaterial->effect();
    for (auto t : effect->techniques())
    {
        for (auto rp : t->renderPasses())
        {
            rp->addRenderState(cloud3D->pointSize);
            rp->addRenderState(cloud3D->lineWidth);
        }
    }
}

void View3D::slot_setPrimitiveType(int type)
{
    if (type==0)
    {
        mode=MODE_POINTS;
        cloud3D->geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    }
    else if (type==1)
    {
        mode=MODE_LINES;
        cloud3D->geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    }
    else if (type==2)
    {
        mode=MODE_LINE_STRIP;
        cloud3D->geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::LineStrip);
    }
    else if (type==3)
    {
        mode=MODE_TRIANGLE;
        cloud3D->geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    }
    else if (type==3)
    {
        mode=MODE_TRIANGLE_STRIP;
        cloud3D->geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::TriangleStrip);
    }
}

void View3D::addSphere(QPosAtt posatt,float scale,QColor color,double radius)
{
    auto* m_obj = new Qt3DExtras::QSphereMesh();
    m_obj->setRadius(radius);

    addObj(reinterpret_cast<Qt3DRender::QMesh*>(m_obj),posatt,scale,color);
}

void View3D::addSphere(Sphere * sphere,QColor color)
{
    Sphere3D* sphere3D=new Sphere3D(rootEntity,sphere,color);
    objects_list.push_back(sphere3D);
    slot_ScaleChanged();
}

void View3D::addPlan(QPosAtt posatt,float scale,QColor color,double width,double height)
{
    auto* m_obj = new Qt3DExtras::QPlaneMesh();
    m_obj->setWidth(width);
    m_obj->setHeight(height);

    addObj(reinterpret_cast<Qt3DRender::QMesh*>(m_obj),posatt,scale,color);
}

void View3D::addPlan(Plan* plan,float radius,QColor color)
{
    Plan3D* plan3D=new Plan3D(rootEntity,plan,radius,color);
    objects_list.push_back(plan3D);
    slot_ScaleChanged();
}

void View3D::addObj(Qt3DRender::QMesh* m_obj, QPosAtt posatt,float scale,QColor color)
{
    Object3D* obj=new Object3D(rootEntity,m_obj,posatt,scale,color);

    Qt3DRender::QCullFace* culling = new Qt3DRender::QCullFace();
    culling->setMode(Qt3DRender::QCullFace::NoCulling);
    auto effect = obj->material->effect();
    for (auto t : effect->techniques())
    {
        for (auto rp : t->renderPasses())
        {
            rp->addRenderState(culling);
        }
    }

    Qt3DCore::QTransform tR;
    tR.setRotation(toQQuaternion(posatt.Q));

    Qt3DCore::QTransform tT;
    tT.setTranslation(toQVector3D(posatt.P));

    baseT.push_back(tT.matrix());
    baseR.push_back(tR.matrix());
    transforms.push_back(obj->transform);
    materials.push_back(obj->material);

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
    double s=1.1;

    Eigen::Vector3d Px(0,(xy_reversed?-1:1),(xz_reversed)?1:-1);
    Eigen::Vector3d Py(yz_reversed?1:-1,0,(xz_reversed?-1:1));
    Eigen::Vector3d Pz((yz_reversed?-1:1),(xy_reversed?1:-1),0);

    Eigen::Vector3d Plx(0,(xy_reversed?-s:s),(xz_reversed)?1:-1);
    Eigen::Vector3d Ply(yz_reversed?1:-1,0,(xz_reversed?-s:s));
    Eigen::Vector3d Plz((yz_reversed?-s:s),(xy_reversed?1:-1),0);

    labelx->setPosRot(toQVector3D(Plx),(xy_reversed?180:0),(xz_reversed)?-180:0,0);

    labely->setPosRot(toQVector3D(Ply),(xz_reversed?180:0)+(yz_reversed?180:0),90+(xz_reversed?180:0),90);

    labelz->setPosRot(toQVector3D(Plz),
                      -90+(xy_reversed?180:0),
                      (yz_reversed?-270:-90),
                      (xy_reversed?180:0));

    objArrowX->setPosAtt(QPosAtt(Px,Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(1,0,0),Eigen::Vector3d(0,-1,0))));
    objArrowY->setPosAtt(QPosAtt(Py,Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,1,0),Eigen::Vector3d(0, 1,0))));
    objArrowZ->setPosAtt(QPosAtt(Pz,Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,0,1),Eigen::Vector3d(0,-1,0))));
}

void View3D::updateGridAndLabels()
{
    if (camera_params->getBeta()>0 && xy_reversed==false)
    {
        xy_reversed=true;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }
    else if (camera_params->getBeta()<0 && xy_reversed==true)
    {
        xy_reversed=false;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }

    if ( cos(camera_params->getAlpha())>0 && yz_reversed==false)
    {
        yz_reversed=true;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }
    else if ( cos(camera_params->getAlpha())<0 && yz_reversed==true)
    {
        yz_reversed=false;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }

    if ( sin(camera_params->getAlpha())>0 && xz_reversed==false)
    {
        xz_reversed=true;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        updateLabels();
    }
    else if ( sin(camera_params->getAlpha())<0 && xz_reversed==true)
    {
        xz_reversed=false;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
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

        updateGridAndLabels();

        xp=event->x();
        yp=event->y();
    }
}

void View3D::slot_export()
{
    QString filename=QFileDialog::getSaveFileName(nullptr,"Cloud export data","Cloud.graphy","*.graphy");

    if(!filename.isEmpty())
    {
        QFile file(filename);

        if(file.open(QIODevice::WriteOnly | QIODevice::Text ))
        {
            QTextStream ts(&file);

            const std::vector<Eigen::Vector4d> & data=cloud->data();
            ts<<"<header>\n";
            ts<<"X;Y;Z;S;\n";
            ts<<"</header>\n";
            for(int i=0;i<cloud->size();i++)
            {
                ts<<data[i][0]<<";"<<data[i][1]<<";"<<data[i][2]<<";"<<data[i][3]<<";\n";
            }
            file.close();
        }
    }
}

void View3D::slot_addMesh()
{
    QStringList filenames=QFileDialog::getOpenFileNames(nullptr,"3D Mesh","./obj","Object (*.obj)");

    if(filenames.size()==0)return;

    for(int i=0;i<filenames.size();i++)
    {
        Object obj(filenames[i],QPosAtt());
        BoundingBox bb=obj.getBox();

        QCPRange rangex=customContainer->getXAxis()->range();
        QCPRange rangey=customContainer->getYAxis()->range();
        QCPRange rangez=customContainer->getZAxis()->range();

        if(i!=0)
        {
            customContainer->getXAxis()->setRange(QCPRange(std::min(rangex.lower,bb.Pmin[0]),std::max(rangex.upper,bb.Pmax[0])));
            customContainer->getYAxis()->setRange(QCPRange(std::min(rangey.lower,bb.Pmin[1]),std::max(rangey.upper,bb.Pmax[1])));
            customContainer->getZAxis()->setRange(QCPRange(std::min(rangez.lower,bb.Pmin[2]),std::max(rangez.upper,bb.Pmax[2])));
        }
        else
        {
            customContainer->getXAxis()->setRange(QCPRange(bb.Pmin[0],bb.Pmax[0]));
            customContainer->getYAxis()->setRange(QCPRange(bb.Pmin[1],bb.Pmax[1]));
            customContainer->getZAxis()->setRange(QCPRange(bb.Pmin[2],bb.Pmax[2]));
        }



        customContainer->adjustSize();
        customContainer->replot();

        auto* m_obj = new Qt3DRender::QMesh();
        m_obj->setSource(QUrl(QString("file:///")+filenames[i]));
        addObj(m_obj,QPosAtt(),1.0,QColor(64,64,64));
    }

}

void View3D::slot_createRotegrity()
{
    QString filename=QFileDialog::getOpenFileName(nullptr,"3D Mesh","./obj","Object (*.obj)");
    std::cout<<filename.toLocal8Bit().data()<<std::endl;
    if(filename.isEmpty())return;


    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Generate rotegrity shape");
    dialog->setMinimumWidth(400);
    QGridLayout* gbox = new QGridLayout();
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QDoubleSpinBox* sb_angle=new QDoubleSpinBox(dialog);
    sb_angle->setValue(25);
    sb_angle->setPrefix("angle=");
    sb_angle->setSuffix(" °");
    sb_angle->setRange(-90,90);
    QSpinBox* sb_subdivisions=new QSpinBox(dialog);
    sb_subdivisions->setValue(100);
    sb_subdivisions->setPrefix("subdivisions=");
    sb_subdivisions->setRange(0,1000);
    QDoubleSpinBox* sb_strech=new QDoubleSpinBox(dialog);
    sb_strech->setValue(1.5);
    sb_strech->setPrefix("strech=");
    sb_strech->setRange(0,10);
    QDoubleSpinBox* sb_radius_int=new QDoubleSpinBox(dialog);
    sb_radius_int->setValue(60);
    sb_radius_int->setPrefix("radius_int=");
    sb_radius_int->setSuffix(" mm");
    sb_radius_int->setRange(0,1000);
    QDoubleSpinBox* sb_radius_dr=new QDoubleSpinBox(dialog);
    sb_radius_dr->setValue(10);
    sb_radius_dr->setPrefix("radius_dr=");
    sb_radius_dr->setSuffix(" mm");
    sb_radius_dr->setRange(0,1000);
    QDoubleSpinBox* sb_width=new QDoubleSpinBox(dialog);
    sb_width->setValue(3);
    sb_width->setPrefix("width=");
    sb_width->setSuffix(" mm");
    sb_width->setRange(0,100);
    QDoubleSpinBox* sb_encA=new QDoubleSpinBox(dialog);
    sb_encA->setDecimals(3);
    sb_encA->setValue(0.9);
    sb_encA->setPrefix("encA=");
    sb_encA->setSuffix(" ");
    sb_encA->setRange(0,1);
    QDoubleSpinBox* sb_encB=new QDoubleSpinBox(dialog);
    sb_encB->setDecimals(3);
    sb_encB->setValue(0.673);
    sb_encB->setPrefix("encB=");
    sb_encB->setSuffix(" ");
    sb_encB->setRange(0,1);



    gbox->addWidget(sb_angle,0,0);
    gbox->addWidget(sb_subdivisions,0,1);
    gbox->addWidget(sb_strech,1,0);
    gbox->addWidget(sb_width,1,1);
    gbox->addWidget(sb_radius_int,2,0);
    gbox->addWidget(sb_radius_dr,2,1);
    gbox->addWidget(sb_encA,3,0);
    gbox->addWidget(sb_encB,3,1);

    gbox->addWidget(buttonBox,4,0,1,2);

    dialog->setLayout(gbox);
    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {

        QFileInfo info(filename);
        Object obj(filename,QPosAtt());
        QString filename=info.path()+"/"+info.baseName()+"_rotegrity.obj";

        obj.rotegrity(sb_angle->value(),
                      sb_subdivisions->value(),
                      sb_strech->value(),
                      sb_radius_int->value(),
                      sb_radius_dr->value(),
                      sb_width->value(),
                      sb_encA->value(),
                      sb_encB->value(),
                      filename,
                      true);

        BoundingBox bb=obj.getBox();
        customContainer->getXAxis()->setRange(QCPRange(bb.Pmin[0],bb.Pmax[0]));
        customContainer->getYAxis()->setRange(QCPRange(bb.Pmin[1],bb.Pmax[1]));
        customContainer->getZAxis()->setRange(QCPRange(bb.Pmin[2],bb.Pmax[2]));

        auto* m_obj = new Qt3DRender::QMesh();
        m_obj->setSource(QUrl(QString("file:///")+filename));

        addObj(m_obj,QPosAtt(),1.0,QColor(64,64,64));

        customContainer->adjustSize();
        customContainer->replot();
    }
}

void View3D::slot_resetView()
{
    camera_params->reset();

    customContainer->getXAxis()->setRange(cloud->getXRange());
    customContainer->getYAxis()->setRange(cloud->getYRange());
    customContainer->getZAxis()->setRange(cloud->getZRange());
    customContainer->getColorScale()->setDataRange(cloud->getScalarFieldRange());
    customContainer->getColorScalePlot()->rescaleAxes();
    customContainer->replot();


    updateGridAndLabels();
}

void View3D::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        slot_resetView();
    }
}

void View3D::configurePopup()
{
    sb_size->blockSignals(true);
    cb_mode->blockSignals(true);
    c_gradient->blockSignals(true);


    sb_size->setValue(static_cast<double>(cloud3D->pointSize->value()));
    cb_mode->setCurrentIndex(static_cast<int>(mode));
    c_gradient->setCurrentIndex(static_cast<int>(cloud->getGradientPreset()));

    sb_size->blockSignals(false);
    cb_mode->blockSignals(false);
    c_gradient->blockSignals(false);


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
        configurePopup();
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


