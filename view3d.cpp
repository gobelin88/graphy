#include "view3d.h"

View3D::View3D(const QMap<QString, QKeySequence>& shortcuts_map)
{
    objects3D.clear();
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
    cameraParams=new CameraParams(camera(),0,0,0);
    cameraParams->entity()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.0001f, 100000.0f);
    cameraParams->reset();
    cameraParams->setBarycenter( QVector3D(0,0,0) );
    cameraParams->setBoundingRadius( 1.20f );

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

    applyShortcuts(shortcuts_map);
}

void View3D::init3D()
{
    grid3D=new Grid3D(rootEntity,10,QColor(255,255,255));

    auto* meshArrow = new Qt3DRender::QMesh();
    meshArrow->setSource(QUrl( QUrl::fromLocalFile(":/obj/obj/axis.obj") ) );

    objArrowX=new Object3D(rootEntity,meshArrow,nullptr,QPosAtt(Eigen::Vector3d(-0.0,1,-1),Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(1,0,0),Eigen::Vector3d(0,-1,0))),0.1f,QColor(255,0,0));
    objArrowY=new Object3D(rootEntity,meshArrow,nullptr,QPosAtt(Eigen::Vector3d(-1,-0.0,1),Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,1,0),Eigen::Vector3d(0,1,0))),0.1f,QColor(0,255,0));
    objArrowZ=new Object3D(rootEntity,meshArrow,nullptr,QPosAtt(Eigen::Vector3d(1,-1,-0.0),Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,0,1),Eigen::Vector3d(0,-1,0))),0.1f,QColor(0,0,255));

    labelx=new Label3D(rootEntity,"X",QVector3D(0,1,-1),0.1f,0,0,0);
    labely=new Label3D(rootEntity,"Y",QVector3D(-1,0,1),0.1f,0,90,90);
    labelz=new Label3D(rootEntity,"Z",QVector3D(1,-1,0),0.1f,-90,-90,0);
    slot_updateLabels();
    slot_resetView();

}



void View3D::createPopup()
{
    popupMenu=new QMenu();

    menuParameters= new QMenu("Parameters");
    menuData= new QMenu("Point cloud");
    menuFit= new QMenu("Fit");
    menuProject= new QMenu("Project");
    menuSubSample= new QMenu("Subsample");
    menuView= new QMenu("View");
    menuMesh= new QMenu("Mesh");

    actSave   = new QAction("Save",  this);
    actSaveRevolution   = new QAction("Save a revolution",  this);
    actExport= new QAction("Export",  this);
    actFitSphere= new QAction("Sphere",  this);
    actFitPlan= new QAction("Plan",  this);
    actFitMesh= new QAction("Custom mesh",  this);
    actProjectSphere= new QAction("Sphere",  this);
    actProjectPlan= new QAction("Plan",  this);
    actProjectMesh= new QAction("Custom mesh",  this);
    actRandomSubSample= new QAction("Random",  this);
    actRescale= new QAction("Rescale",  this);
    actRescaleSelected= new QAction("Rescale selected",  this);
    actRemoveSelected= new QAction("Remove selected",  this);
    actMeshLoad= new QAction("Load",  this);
    actMeshCreateRotegrity= new QAction("Rotegrity",  this);
    actFullscreen= new QAction("Fullscreen",  this);
    actFullscreen->setCheckable(true);

    ///////////////////////////////////////////////
    QWidgetAction* actWidget=new QWidgetAction(popupMenu);
    QWidget* widget=new QWidget;
    actWidget->setDefaultWidget(widget);

    QGridLayout* gbox = new QGridLayout();

    cb_show_hide_grid=new QCheckBox("Show grid");
    cb_show_hide_grid->setChecked(grid3D->entity->isEnabled());

    cb_show_hide_axis=new QCheckBox("Show axis");
    cb_show_hide_axis->setChecked(true);

    cb_show_hide_labels=new QCheckBox("Show labels");
    cb_show_hide_labels->setChecked(true);

    cb_use_custom_color=new QCheckBox("Use custom color");
    cw_custom_color=new Color_Wheel();

    sb_size=new QDoubleSpinBox;
    sb_size->setRange(0.1,100);
    sb_size->setSingleStep(0.1);

    cb_mode=new QComboBox;
    cb_mode->addItem("POINTS");
    cb_mode->addItem("LINES");
    cb_mode->addItem("LINES_LOOP");
    cb_mode->addItem("LINE_STRIP");
    cb_mode->addItem("TRIANGLES");
    cb_mode->addItem("TRIANGLE_STRIP");

    c_gradient=new QGradientComboBox(nullptr);

    gbox->addWidget(cb_mode,0,0);
    gbox->addWidget(sb_size,0,1);
    gbox->addWidget(c_gradient,1,0,1,2);
    gbox->addWidget(cb_show_hide_grid,2,0,1,2);
    gbox->addWidget(cb_show_hide_axis,3,0,1,2);
    gbox->addWidget(cb_show_hide_labels,4,0,1,2);
    gbox->addWidget(cb_use_custom_color,5,0);
    gbox->addWidget(cw_custom_color,5,1);


    widget->setLayout(gbox);

    ///////////////////////////////////////////////Action context
    customContainer->addAction(actRescale);
    customContainer->addAction(actRescaleSelected);
    customContainer->addAction(actRemoveSelected);
    customContainer->addAction(actFullscreen);

    auto customContainerActions=customContainer->actions();
    for(auto act:customContainerActions)
    {
        act->setShortcutVisibleInContextMenu(true);
    }

    ///////////////////////////////////////////////
    popupMenu->addMenu(menuParameters);
    popupMenu->addMenu(menuView);
    popupMenu->addSeparator();
    popupMenu->addMenu(menuData);
    popupMenu->addMenu(menuMesh);
    popupMenu->addSeparator();
    popupMenu->addAction(actSave);
    popupMenu->addAction(actSaveRevolution);
    menuView->addAction(actRescale);
    menuView->addAction(actRescaleSelected);
    menuView->addAction(actRemoveSelected);
    menuView->addAction(actFullscreen);
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

    QObject::connect(cb_show_hide_labels,SIGNAL(stateChanged(int)),this,SLOT(slot_showHideLabels(int)));
    QObject::connect(cb_show_hide_grid,SIGNAL(stateChanged(int)),this,SLOT(slot_showHideGrid(int)));
    QObject::connect(cb_show_hide_axis,SIGNAL(stateChanged(int)),this,SLOT(slot_showHideAxis(int)));
    QObject::connect(actMeshLoad,SIGNAL(triggered()),this,SLOT(slot_addMesh()));
    QObject::connect(actMeshCreateRotegrity,SIGNAL(triggered()),this,SLOT(slot_createRotegrity()));
    QObject::connect(actExport,SIGNAL(triggered()),this,SLOT(slot_export()));
    QObject::connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_resetView()));
    QObject::connect(actRescaleSelected,SIGNAL(triggered()),this,SLOT(slot_resetViewOnSelected()));
    QObject::connect(actRemoveSelected,SIGNAL(triggered()),this,SLOT(slot_removeSelected()));
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
    QObject::connect(cb_use_custom_color, SIGNAL(stateChanged(int)), this, SLOT(slot_useCustomColor(int)));
    QObject::connect(cw_custom_color, SIGNAL(colorChanged(QColor)), this, SLOT(slot_setCustomColor(QColor)));
    QObject::connect(actFullscreen, SIGNAL(triggered(bool)), customContainer, SLOT(slot_fullscreen(bool)));

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
        float a0=cameraParams->getAlpha();
        QScreen* screen=QGuiApplication::primaryScreen();

        for (int k=0; k<N; k++)
        {
            QString image_filename=info.dir().path()+QString("/%1_").arg(k)+info.baseName()+QString(".png");
            std::cout<<image_filename.toLocal8Bit().data()<<std::endl;
            cameraParams->moveTo(a0+float(2.0*M_PI/N*k),cameraParams->getBeta(),cameraParams->getRadius());
            slot_updateGridAndLabels();
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
    std::vector<Cloud3D *> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;

        Sphere sphere(currentCloud->getBarycenter(),currentCloud->getBoundingRadius()/2.0);
        currentCloud->fit(&sphere);

        Eigen::Vector3d C=sphere.getCenter();

        addSphere(&sphere,QColor(64,64,64));


        //addSphere(QPosAtt(C,Eigen::Quaterniond(1,0,0,0)),1.0,QColor(64,64,64),sphere.getRadius());

        emit sig_displayResults( QString("Fit Sphere:\nCenter=(%1 , %2 , %3) Radius=%4\nRms=%5\n").arg(C[0]).arg(C[1]).arg(C[2]).arg(sphere.getRadius()).arg(sphere.getRMS()) );
        emit sig_newColumn("Err_Sphere",sphere.getErrNorm());
    }
}

void View3D::slot_fitPlan()
{
    std::vector<Cloud3D *> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;

        Eigen::Vector3d barycenter=currentCloud->getBarycenter();

        Plan* plan;
        Plan planA(Eigen::Vector3d(1,0,0),barycenter);
        Plan planB(Eigen::Vector3d(0,1,0),barycenter);
        Plan planC(Eigen::Vector3d(0,0,1),barycenter);
        currentCloud->fit(&planA);
        currentCloud->fit(&planB);
        currentCloud->fit(&planC);

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

        addPlan(plan,currentCloud->getBoundingRadius()*2,QColor(128,128,128));

        emit sig_displayResults( QString("Fit Plan:\nNormal=+-(%1 , %2 , %3)\nRms=%4\n").arg(N[0]).arg(N[1]).arg(N[2]).arg(plan->getRMS()));
        emit sig_newColumn("Err_Plan",plan->getErrNorm());
    }
}


void View3D::slot_projectPlan()
{
    std::vector<Cloud3D *> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

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

        Eigen::Vector3d barycenter=currentCloud->getBarycenter();
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

            currentCloud->project(&plan);
            currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()) );
        }
    }
}

void View3D::slot_projectSphere()
{
    std::vector<Cloud3D *> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("project on a Sphere");
        dialog->setMinimumWidth(400);
        QGridLayout* gbox = new QGridLayout();
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        QDoubleSpinBox* sb_radius=new QDoubleSpinBox(dialog);
        sb_radius->setValue(currentCloud->getBoundingRadius());
        sb_radius->setPrefix("radius=");
        sb_radius->setRange(-10000000,10000000);

        Eigen::Vector3d barycenter=currentCloud->getBarycenter();
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

            currentCloud->project(&sphere);
            currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()) );
        }
    }
}

void View3D::slot_projectCustomMesh()
{
    std::vector<Cloud3D *> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        QString filename=QFileDialog::getOpenFileName(nullptr,"Project on 3D Mesh","./obj","Object (*.obj)");
        std::cout<<filename.toLocal8Bit().data()<<std::endl;
        if(filename.isEmpty())return;

        Object obj(filename,QPosAtt());
        currentCloud->project(&obj);

        currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()) );
        //setCloudScalar(cloud,PrimitiveMode::MODE_POINTS);
    }
}

void View3D::slot_randomSubSamples()
{
    std::vector<Cloud3D *> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Random Sub Samples");
        dialog->setMinimumWidth(400);
        QGridLayout* gbox = new QGridLayout();
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        QSpinBox* sb_subSamples=new QSpinBox(dialog);
        sb_subSamples->setValue(currentCloud->getBoundingRadius());
        sb_subSamples->setPrefix("Samples=");
        sb_subSamples->setRange(0,currentCloud->size());

        QSlider* sb_slideSubSamples=new QSlider(Qt::Horizontal,dialog);
        sb_slideSubSamples->setValue(currentCloud->getBoundingRadius());
        sb_slideSubSamples->setRange(0,currentCloud->size());

        QObject::connect(sb_slideSubSamples, SIGNAL(valueChanged(int)), sb_subSamples, SLOT(setValue(int)));
        QObject::connect(sb_subSamples, SIGNAL(valueChanged(int)), sb_slideSubSamples, SLOT(setValue(int)));
        sb_subSamples->setValue(currentCloud->size());

        gbox->addWidget(sb_subSamples,0,0);
        gbox->addWidget(sb_slideSubSamples,0,1,1,2);
        gbox->addWidget(buttonBox,1,0,1,3);

        dialog->setLayout(gbox);
        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            int nbPoints=sb_subSamples->value();
            currentCloud3D->positionAttribute   ->setCount(static_cast<unsigned int>(nbPoints));
            currentCloud3D->indexAttribute->setCount(static_cast<unsigned int>(nbPoints));
            currentCloud->subSample(nbPoints);

            if(nbPoints!=currentCloud->size())
            {
                std::cout<<"error : "<<nbPoints<<" "<<currentCloud->size()<<std::endl;
            }

            currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()) );
        }
    }
}

void View3D::slot_fitCustomMesh()
{
    std::vector<Cloud3D *> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;

        QString filename=QFileDialog::getOpenFileName(nullptr,"3D Mesh","./obj","Object (*.obj)");
        std::cout<<filename.toLocal8Bit().data()<<std::endl;
        if(filename.isEmpty())return;

        QElapsedTimer timer;
        timer.start();

        Object obj(filename,QPosAtt());


        Vector3d obj_center=obj.getBox().middle();
        obj.setScalePosAtt(QPosAtt(currentCloud->getBarycenter()-obj_center,Eigen::Quaterniond(currentCloud->getBoundingRadius()/obj.getRadius(obj_center),0,0,0)));

        currentCloud->fit(&obj,100);

        QFileInfo info(filename);

        obj.save(info.path()+"/tmp.obj");

        auto* m_obj = new Qt3DRender::QMesh();
        m_obj->setSource(QUrl(QString("file:///")+info.path()+"/tmp.obj"));

        addObject(m_obj,nullptr,QPosAtt(),1.0,QColor(64,64,64));

        QPosAtt posatt=obj.getPosAtt();
        emit sig_displayResults( QString("Fit Mesh :\nScale=%1\nPosition=(%2,%3,%4)\nQ=(%5,%6,%7,%8)\nRms=%9\ndt=%10 ms")
                                 .arg(obj.getScale())
                                 .arg(posatt.P[0]).arg(posatt.P[1]).arg(posatt.P[2])
                .arg(posatt.Q.w()).arg(posatt.Q.x()).arg(posatt.Q.y()).arg(posatt.Q.w())
                .arg(obj.getRMS())
                .arg(timer.nsecsElapsed()*1e-6));

        emit sig_newColumn("Err_Mesh",obj.getErrNorm());
    }
}

//----------------------------




void View3D::slot_ColorScaleChanged(const QCPRange& range)
{
    std::vector<Cloud3D *> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        if (currentCloud && currentCloud3D->buffer)
        {
            currentCloud3D->buffer->setData(currentCloud->getBuffer(range));
        }
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
    Qt3DCore::QTransform T,S;
    T.setTranslation(-customContainer->getTranslation());
    S.setScale3D(customContainer->getScaleInv());

    for(int i=0;i<objects3D.size();i++)
    {
        objects3D[i]->transform->setMatrix( S.matrix()*T.matrix()*objects3D[i]->tT*objects3D[i]->tR );//->setMatrix(t.matrix().inverted());
    }
}

void View3D::slot_setGradient(int preset)
{
    std::vector<Cloud3D*> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        if (currentCloud && currentCloud3D->buffer)
        {
            currentCloud->setGradientPreset(static_cast<QCPColorGradient::GradientPreset>(preset));
            customContainer->getColorScale()->setGradient(currentCloud->getGradient());
            currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()));
            customContainer->getColorScalePlot()->rescaleAxes();
            customContainer->getColorScalePlot()->replot();
        }
    }
}

void View3D::slot_useCustomColor(int value)
{
    std::vector<Cloud3D*> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        if (currentCloud && currentCloud3D->buffer)
        {
            currentCloud->setUseCustomColor(value);
            currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()));
        }
    }
}

void View3D::slot_setCustomColor(QColor color)
{
    std::vector<Cloud3D*> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        if (currentCloud && currentCloud3D->buffer)
        {
            currentCloud->setCustomColor(color.rgb());
            currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()));
        }
    }
}

void View3D::addCloudScalar(Cloud* cloudData, Qt3DRender::QGeometryRenderer::PrimitiveType primitiveMode)
{
    Cloud3D * currentCloud3D=new Cloud3D(cloudData,rootEntity);

    referenceObjectEntity(currentCloud3D,"Cloud");


    currentCloud3D->geometryRenderer->setPrimitiveType(primitiveMode);

    customContainer->getColorScale()->axis()->setLabel(currentCloud3D->cloud->getLabelS());
    customContainer->getColorScale()->setGradient(currentCloud3D->cloud->getGradient());

    customContainer->getXAxis()->setLabel(currentCloud3D->cloud->getLabelX());
    customContainer->getYAxis()->setLabel(currentCloud3D->cloud->getLabelY());
    customContainer->getZAxis()->setLabel(currentCloud3D->cloud->getLabelZ());

    labelx->setText(currentCloud3D->cloud->getLabelX());
    labely->setText(currentCloud3D->cloud->getLabelY());
    labelz->setText(currentCloud3D->cloud->getLabelZ());

    slot_setPointSize(currentCloud3D->pointSize->value());
    slot_resetView();

    customContainer->adjustSize();
    customContainer->replot();
}

std::vector<Cloud3D*> View3D::getClouds()
{
    std::vector<Cloud3D*> cloudsList;

    for(int i=0;i<objects3D.size();i++)
    {
        Cloud3D* ptrCloud=dynamic_cast<Cloud3D*>(objects3D[i]);
        if(ptrCloud)
        {
                cloudsList.push_back(ptrCloud);
        }
    }
    return cloudsList;
}

std::vector<Base3D*> View3D::getMeshs()
{
    std::vector<Base3D*> meshList;

    for(int i=0;i<objects3D.size();i++)
    {
        Base3D * ptrPlan3D=dynamic_cast<Plan3D*>(objects3D[i]);
        Base3D * ptrSphere3D=dynamic_cast<Sphere3D*>(objects3D[i]);
        Base3D * ptrObject3D=dynamic_cast<Object3D*>(objects3D[i]);

        if(ptrPlan3D){meshList.push_back(ptrPlan3D);}
        if(ptrSphere3D){meshList.push_back(ptrSphere3D);}
        if(ptrObject3D){meshList.push_back(ptrObject3D);}
    }

    return meshList;
}

std::vector<Cloud3D*> View3D::getSelectedClouds()
{
    std::vector<Cloud3D*> selectedClouds;

    int itemsCount=customContainer->getSelectionView()->count();

    if(objects3D.size()!=itemsCount)
    {
        std::cout<<"Error"<<std::endl;
        return selectedClouds;
    }

    for(int i=0;i<itemsCount;i++)
    {
        QListWidgetItem * currentItem= customContainer->getSelectionView()->item(i);

        Cloud3D* ptrCloud=dynamic_cast<Cloud3D*>(objects3D[i]);
        if(ptrCloud)
        {
            if(currentItem->isSelected())
            {
                selectedClouds.push_back(ptrCloud);
            }
        }
    }
    return selectedClouds;
}

std::vector<Base3D*> View3D::getSelectedObjects()
{
    std::vector<Base3D*> selectedObjects;

    int itemsCount=customContainer->getSelectionView()->count();

    if(objects3D.size()!=itemsCount)
    {
        std::cout<<"Error"<<std::endl;
        return selectedObjects;
    }

    for(int i=0;i<itemsCount;i++)
    {
        QListWidgetItem * currentItem= customContainer->getSelectionView()->item(i);

        if(currentItem->isSelected())
        {
            selectedObjects.push_back(objects3D[i]);
        }

    }
    return selectedObjects;
}

void View3D::slot_removeSelected()
{
    std::cout<<"Remove Selected"<<std::endl;

    std::vector<Base3D*> selectedObjects=getSelectedObjects();

    for(int i=0;i<selectedObjects.size();i++)
    {
        for(int k=0;k<objects3D.size();k++)
        {
            if(objects3D[k]==selectedObjects[i])
            {
                objects3D.erase(objects3D.begin()+k);
                delete customContainer->getSelectionView()->takeItem(k);
            }
        }

        delete selectedObjects[i];
    }

}

void View3D::slot_setPointSize(double value)
{
    std::vector<Cloud3D*> selectedClouds=getSelectedClouds();



    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        currentCloud3D->pointSize->setValue(value);
        currentCloud3D->lineWidth->setValue(value);
        currentCloud3D->lineWidth->setSmooth(true);

        auto effect = currentCloud3D->material->effect();
        for (auto t : effect->techniques())
        {
            for (auto rp : t->renderPasses())
            {
                rp->addRenderState(currentCloud3D->pointSize);
                rp->addRenderState(currentCloud3D->lineWidth);
            }
        }

    }
}

void View3D::slot_setPrimitiveType(int type)
{
    std::vector<Cloud3D*> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];

        //        Points = 0x0000,
        //        Lines = 0x0001,
        //        LineLoop = 0x0002,
        //        LineStrip = 0x0003,
        //        Triangles = 0x0004,
        //        TriangleStrip = 0x0005,
        //        TriangleFan = 0x0006,
        currentCloud3D->geometryRenderer->setPrimitiveType(static_cast<Qt3DRender::QGeometryRenderer::PrimitiveType>(type));
    }
}

void View3D::addSphere(Sphere * sphere,QColor color)
{
    Sphere3D* sphere3D=new Sphere3D(rootEntity,sphere,color);
    referenceObjectEntity(sphere3D,"Sphere");
    slot_ScaleChanged();
}

void View3D::addPlan(Plan* plan,float radius,QColor color)
{
    Plan3D* plan3D=new Plan3D(rootEntity,plan,radius,color);
    referenceObjectEntity(plan3D,"Plan");
    slot_ScaleChanged();
}

void View3D::referenceObjectEntity(Base3D * base3D,QString name)
{
    objects3D.push_back(base3D);

    QListWidgetItem * item=new QListWidgetItem(name);

    if(dynamic_cast<Cloud3D*>(base3D)!=nullptr)
    {
        item->setIcon(QIcon(QPixmap::fromImage(QImage(":/img/icons/points_cloud.png"))));
    }
    else //Todo differents icon here
    {
        item->setIcon(QIcon(QPixmap::fromImage(QImage(":/img/icons/shape_icosahedron.gif"))));
    }

    customContainer->getSelectionView()->addItem(item);
}

void View3D::addObject(Qt3DRender::QMesh* mesh_object,Object * object, QPosAtt posatt,float scale,QColor color)
{
    Object3D* object3D=new Object3D(rootEntity,mesh_object,object,posatt,scale,color);

    referenceObjectEntity(object3D,"Mesh");

    Qt3DRender::QCullFace* culling = new Qt3DRender::QCullFace();
    culling->setMode(Qt3DRender::QCullFace::NoCulling);
    auto effect = object3D->material->effect();
    for (auto t : effect->techniques())
    {
        for (auto rp : t->renderPasses())
        {
            rp->addRenderState(culling);
        }
    }

    Qt3DCore::QTransform tR;
    tR.setRotation(toQQuaternion(posatt.Q));
    object3D->tR=tR.matrix();

    Qt3DCore::QTransform tT;
    tT.setTranslation(toQVector3D(posatt.P));
    object3D->tT=tT.matrix();

    slot_ScaleChanged();
}

CustomViewContainer* View3D::getContainer()
{
    return customContainer;
}

void View3D::applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map)
{
    QMap<QString,QAction*> shortcuts_links;
    shortcuts_links.insert(QString("Graph-Delete"),actRemoveSelected);
    shortcuts_links.insert(QString("Graph-Rescale"),actRescale);
    shortcuts_links.insert(QString("Graph-RescaleSelected"),actRescaleSelected);
    shortcuts_links.insert(QString("Graph-Fullscreen"),actFullscreen);

    QMapIterator<QString, QKeySequence> i(shortcuts_map);
    while (i.hasNext())
    {
        i.next();

        if (shortcuts_links.contains(i.key()))
        {
            shortcuts_links[i.key()]->setShortcut(i.value());
        }
    }
}

void View3D::slot_updateLabels()
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

void View3D::slot_updateGridAndLabels()
{
    if (cameraParams->getBeta()>0 && xy_reversed==false)
    {
        xy_reversed=true;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        slot_updateLabels();
    }
    else if (cameraParams->getBeta()<0 && xy_reversed==true)
    {
        xy_reversed=false;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        slot_updateLabels();
    }

    if ( cos(cameraParams->getAlpha())>0 && yz_reversed==false)
    {
        yz_reversed=true;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        slot_updateLabels();
    }
    else if ( cos(cameraParams->getAlpha())<0 && yz_reversed==true)
    {
        yz_reversed=false;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        slot_updateLabels();
    }

    if ( sin(cameraParams->getAlpha())>0 && xz_reversed==false)
    {
        xz_reversed=true;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        slot_updateLabels();
    }
    else if ( sin(cameraParams->getAlpha())<0 && xz_reversed==true)
    {
        xz_reversed=false;
        grid3D->buffer->setData(grid3D->getGridBuffer(xz_reversed,xy_reversed,yz_reversed,10));
        slot_updateLabels();
    }
}

void View3D::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        float dx=xp-event->x();
        float dy=yp-event->y();

        cameraParams->move(dx,dy);

        slot_updateGridAndLabels();

        xp=event->x();
        yp=event->y();
    }
}

bool isEquiv(const QKeyEvent* event, const QKeySequence& seq)
{
    if (seq.count() != 1)
        return false;
    return seq[0] == (event->key() | event->modifiers());
}

void View3D::keyPressEvent(QKeyEvent * event)
{
    QList<QAction*> actions=customContainer->actions();
    for(auto act:actions)
    {
        if (isEquiv(event,act->shortcut())) {act->trigger();}
    }
}

void View3D::slot_export()
{
    std::vector<Cloud3D*> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;

        QString filename=QFileDialog::getSaveFileName(nullptr,"Cloud export data","Cloud.graphy","*.graphy");

        if(!filename.isEmpty())
        {
            QFile file(filename);

            if(file.open(QIODevice::WriteOnly | QIODevice::Text ))
            {
                QTextStream ts(&file);

                const std::vector<Eigen::Vector4d> & data=currentCloud->data();
                ts<<"<header>\n";
                ts<<"X;Y;Z;S;\n";
                ts<<"</header>\n";
                for(int i=0;i<currentCloud->size();i++)
                {
                    ts<<data[i][0]<<";"<<data[i][1]<<";"<<data[i][2]<<";"<<data[i][3]<<";\n";
                }
                file.close();
            }
        }
    }
}

void View3D::slot_showHideGrid(int value)
{
    if(value>0)
    {
        grid3D->entity->setEnabled(true);
    }
    else
    {
        grid3D->entity->setEnabled(false);
    }
}

void View3D::slot_showHideAxis(int value)
{
    if(value>0)
    {
        objArrowX->entity->setEnabled(true);
        objArrowY->entity->setEnabled(true);
        objArrowZ->entity->setEnabled(true);
    }
    else
    {
        objArrowX->entity->setEnabled(false);
        objArrowY->entity->setEnabled(false);
        objArrowZ->entity->setEnabled(false);
    }
}

void View3D::slot_showHideLabels(int value)
{
    if(value>0)
    {
        labelx->entity->setEnabled(true);
        labely->entity->setEnabled(true);
        labelz->entity->setEnabled(true);
    }
    else
    {
        labelx->entity->setEnabled(false);
        labely->entity->setEnabled(false);
        labelz->entity->setEnabled(false);
    }
}

void View3D::slot_addMesh()
{
    QStringList filenames=QFileDialog::getOpenFileNames(nullptr,"3D Mesh","./obj","Object (*.obj)");

    if(filenames.size()==0)return;

    for(int i=0;i<filenames.size();i++)
    {
        Object * obj=new Object(filenames[i],QPosAtt());
        BoundingBox bb=obj->getBox();

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
        addObject(m_obj,obj,QPosAtt(),1.0,QColor(64,64,64));
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
        Object * objet=new Object(filename,QPosAtt());
        QString filename=info.path()+"/"+info.baseName()+"_rotegrity.obj";

        objet->rotegrity(sb_angle->value(),
                      sb_subdivisions->value(),
                      sb_strech->value(),
                      sb_radius_int->value(),
                      sb_radius_dr->value(),
                      sb_width->value(),
                      sb_encA->value(),
                      sb_encB->value(),
                      filename,
                      true);

        BoundingBox bb=objet->getBox();
        customContainer->getXAxis()->setRange(QCPRange(bb.Pmin[0],bb.Pmax[0]));
        customContainer->getYAxis()->setRange(QCPRange(bb.Pmin[1],bb.Pmax[1]));
        customContainer->getZAxis()->setRange(QCPRange(bb.Pmin[2],bb.Pmax[2]));

        auto* mesh_object = new Qt3DRender::QMesh();
        mesh_object->setSource(QUrl(QString("file:///")+filename));

        addObject(mesh_object,objet,QPosAtt(),1.0,QColor(64,64,64));

        customContainer->adjustSize();
        customContainer->replot();
    }
}


void View3D::extendRanges(QCPRange itemRangeX,QCPRange itemRangeY,QCPRange itemRangeZ,int i)
{
    QCPRange rangex=customContainer->getXAxis()->range();
    QCPRange rangey=customContainer->getYAxis()->range();
    QCPRange rangez=customContainer->getZAxis()->range();

    if(i!=0)
    {
        customContainer->getXAxis()->setRange(QCPRange(std::min(itemRangeX.lower,rangex.lower),std::max(itemRangeX.upper,rangex.upper)));
        customContainer->getYAxis()->setRange(QCPRange(std::min(itemRangeY.lower,rangey.lower),std::max(itemRangeY.upper,rangey.upper)));
        customContainer->getZAxis()->setRange(QCPRange(std::min(itemRangeZ.lower,rangez.lower),std::max(itemRangeZ.upper,rangez.upper)));
    }
    else
    {
        customContainer->getXAxis()->setRange(itemRangeX);
        customContainer->getYAxis()->setRange(itemRangeY);
        customContainer->getZAxis()->setRange(itemRangeZ);
    }
}

void View3D::extendScalarRange(QCPRange itemRangeS,int i)
{
    QCPRange ranges=customContainer->getColorScale()->dataRange();

    if(i!=0)
    {
        customContainer->getColorScale()->setDataRange(QCPRange(std::min(itemRangeS.lower,ranges.lower),std::max(itemRangeS.upper,ranges.upper)));
    }
    else
    {
        customContainer->getColorScale()->setDataRange(itemRangeS);
    }
}

void View3D::slot_resetView()
{
    cameraParams->reset();

    std::vector<Cloud3D*> cloudsList=getClouds();

    for(int i=0;i<cloudsList.size();i++)
    {
        Cloud * currentCloud=cloudsList[i]->cloud;
        extendRanges(currentCloud->getXRange(),currentCloud->getYRange(),currentCloud->getZRange(),i);
        extendScalarRange(currentCloud->getScalarFieldRange(),i);
    }

    customContainer->getColorScalePlot()->rescaleAxes();
    customContainer->getColorScalePlot()->replot();
    customContainer->replot();

    slot_updateGridAndLabels();
}



void View3D::slot_resetViewOnSelected()
{
    cameraParams->reset();

    std::vector<Cloud3D*> selectedClouds=getSelectedClouds();

    for(int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;
        extendRanges(currentCloud->getXRange(),currentCloud->getYRange(),currentCloud->getZRange(),i);
        extendScalarRange(currentCloud->getScalarFieldRange(),i);
    }

    customContainer->getColorScalePlot()->rescaleAxes();
    customContainer->replot();

    slot_updateGridAndLabels();
}

void View3D::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {

    }
}

void View3D::configurePopup()
{
    sb_size->blockSignals(true);
    cb_mode->blockSignals(true);
    c_gradient->blockSignals(true);

    std::vector<Cloud3D*> selectedClouds=getSelectedClouds();

    if(selectedClouds.size()>0)
    {
        Cloud3D * currentCloud3D=selectedClouds[0];
        Cloud * currentCloud=currentCloud3D->cloud;

        sb_size->setValue(static_cast<double>(currentCloud3D->pointSize->value()));
        cb_mode->setCurrentIndex(static_cast<int>(currentCloud3D->geometryRenderer->primitiveType()));
        c_gradient->setCurrentIndex(static_cast<int>(currentCloud->getGradientPreset()));

        sb_size->setEnabled(true);
        cb_mode->setEnabled(true);
        cb_use_custom_color->setEnabled(true);
        cw_custom_color->setEnabled(currentCloud3D->cloud->isCustomColorUsed());

        cb_use_custom_color->setChecked(currentCloud3D->cloud->isCustomColorUsed());
        cw_custom_color->setColor(currentCloud3D->cloud->getCustomColor());
    }
    else
    {
        sb_size->setEnabled(false);
        cb_mode->setEnabled(false);
        cb_use_custom_color->setEnabled(false);
        cw_custom_color->setEnabled(false);
    }

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
        popupMenu->exec(mapToGlobal(event->pos()));
    }
}
void View3D::wheelEvent(QWheelEvent* event)
{
    float dw=event->delta();
    if (dw>0)
    {
        if (cameraParams->getRadius()<10*cameraParams->getBoundingRadius())
        {
            cameraParams->setRadius(cameraParams->getRadius()*dw*0.01f);
        }
    }
    else
    {
        if (cameraParams->getRadius()>cameraParams->getBoundingRadius())
        {
            cameraParams->setRadius(cameraParams->getRadius()/(std::abs(dw)*0.01f));
        }
    }
}


