#include "3d_viewer.h"

#include "algorithmes/Arun.h"
#include "algorithmes/Horn.h"
#include "shapes/SolidHarmonicsDecomposition.h"

Viewer3D::Viewer3D(const QMap<QString, QKeySequence>& shortcuts_map,QWidget * parent)
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
    //customContainer=new CustomViewContainer(container,nullptr,Qt::SubWindow);
    customContainer=new CustomViewContainer(container,parent,Qt::WindowFlags());
    customContainer->setAttribute(Qt::WA_DeleteOnClose);
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
    connect(customContainer,&CustomViewContainer::sig_itemDoubleClicked,this,&Viewer3D::slot_itemDoubleClicked);

    //addSphere(QPosAtt(Eigen::Vector3d(0.05,0.05,0.05),Eigen::Quaterniond(1,0,0,0)),1.0,QColor(128,128,128),0.01);

    createPopup();

    applyShortcuts(shortcuts_map);
}

Viewer3D::~Viewer3D()
{
    std::cout<<"Delete Viewer3D"<<std::endl;

    for(unsigned int i=0;i<objects3D.size();i++)
    {
        Cloud3D* ptrCloud=dynamic_cast<Cloud3D*>(objects3D[i]);
        Plan3D * ptrPlan3D=dynamic_cast<Plan3D*>(objects3D[i]);
        Sphere3D * ptrSphere3D=dynamic_cast<Sphere3D*>(objects3D[i]);
        Object3D * ptrObject3D=dynamic_cast<Object3D*>(objects3D[i]);
        Ellipsoid3D * ptrEllipsoid3D=dynamic_cast<Ellipsoid3D*>(objects3D[i]);

        if(ptrCloud){delete ptrCloud;}
        if(ptrPlan3D){delete ptrPlan3D;}
        if(ptrSphere3D){delete ptrSphere3D;}
        if(ptrObject3D){delete ptrObject3D;}
        if(ptrEllipsoid3D){delete ptrEllipsoid3D;}
    }

    std::cout<<"Delete rootEntity"<<std::endl;
    delete rootEntity;
}

void Viewer3D::init3D()
{
    grid3D=new Grid3D(rootEntity,10,QColor(255,255,255));

    auto* meshArrow = new Qt3DRender::QMesh();
    meshArrow->setSource(QUrl( QUrl::fromLocalFile(":/obj/obj/axis.obj") ) );

    if(meshArrow->status()==Qt3DRender::QMesh::Status::Error)
    {
        std::cout<<"Failed to load mesh:"<<meshArrow->meshName().toStdString()<<std::endl;
    }

    objArrowX=new Object3D(rootEntity,meshArrow,nullptr,PosAtt(Eigen::Vector3d(-0.0,1,-1),Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(1,0,0),Eigen::Vector3d(0,-1,0))),0.1f,QColor(0,0,0));
    objArrowY=new Object3D(rootEntity,meshArrow,nullptr,PosAtt(Eigen::Vector3d(-1,-0.0,1),Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,1,0),Eigen::Vector3d(0,1,0))),0.1f,QColor(0,0,0));
    objArrowZ=new Object3D(rootEntity,meshArrow,nullptr,PosAtt(Eigen::Vector3d(1,-1,-0.0),Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,0,1),Eigen::Vector3d(0,-1,0))),0.1f,QColor(0,0,0));

    labelx=new Label3D(rootEntity,"X",QVector3D(0,1,-1),0.1f,0,0,0);
    labely=new Label3D(rootEntity,"Y",QVector3D(-1,0,1),0.1f,0,90,90);
    labelz=new Label3D(rootEntity,"Z",QVector3D(1,-1,0),0.1f,-90,-90,0);
    slot_updateLabels();
    slot_resetView();

}



void Viewer3D::createPopup()
{
    popupMenu=new QMenu();

    menuParameters= new QMenu("Parameters");
    menuData= new QMenu("Point cloud");
    menuFit= new QMenu("Fit");
    menuProject= new QMenu("Project");
    menuScalarField= new QMenu("ScalarField");
    menuTools= new QMenu("Tools");
    menuView= new QMenu("View");
    menuMesh= new QMenu("Mesh");

    actSave   = new QAction("Save",  this);
    actSaveRevolution   = new QAction("Save a revolution",  this);
    actExport= new QAction("Export",  this);
    actFitEllipsoid= new QAction("Ellipsoid",  this);
    actFitSphere= new QAction("Sphere",  this);
    actFitPlan= new QAction("Plan",  this);
    actFitMesh= new QAction("Custom mesh",  this);
    actFitPointCloud= new QAction("Point cloud",  this);
    actProjectSphere= new QAction("Sphere",  this);
    actProjectPlan= new QAction("Plan",  this);
    actProjectMesh= new QAction("Custom mesh",  this);
    actRandomSubSample= new QAction("Random subsample",  this);
    actMove= new QAction("Move",  this);

    actComputeSolidHarmonics= new QAction("Solid Harmonics",  this);
    actRescale= new QAction("Rescale",  this);
    actRescaleSelected= new QAction("Rescale selected",  this);
    actRescaleSelectedSameRanges= new QAction("Rescale selected same ranges",  this);
    actRemoveSelected= new QAction("Remove selected",  this);
    actMeshLoad= new QAction("Load",  this);
    actMeshCreateRotegrity= new QAction("Rotegrity",  this);
    actFullscreen= new QAction("Fullscreen",  this);
    actFullscreen->setCheckable(true);

    actCopy= new QAction("Copy",  this);
    actCopy->setShortcut(QKeySequence("Ctrl+C"));
    actPaste= new QAction("Paste",  this);
    actPaste->setShortcut(QKeySequence("Ctrl+V"));

    ///////////////////////////////////////////////
    QWidgetAction* parametersActionWidget=new QWidgetAction(popupMenu);
    QWidget* parametersWidget=new QWidget;
    parametersActionWidget->setDefaultWidget(parametersWidget);

    QGridLayout* gbox = new QGridLayout();
    cb_show_hide_grid=new QCheckBox("Show grid");
    cb_show_hide_grid->setChecked(grid3D->entity->isEnabled());
    cb_show_hide_axis=new QCheckBox("Show axis");
    cb_show_hide_axis->setChecked(true);
    cb_show_hide_labels=new QCheckBox("Show labels");
    cb_show_hide_labels->setChecked(true);

    gbox->addWidget(cb_show_hide_grid,1,0);
    gbox->addWidget(cb_show_hide_axis,2,0);
    gbox->addWidget(cb_show_hide_labels,3,0);
    parametersWidget->setLayout(gbox);

    ///////////////////////////////////////////////Action context
    customContainer->addAction(actCopy);
    customContainer->addAction(actPaste);
    customContainer->addAction(actRescale);
    customContainer->addAction(actRescaleSelected);
    customContainer->addAction(actRescaleSelectedSameRanges);
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
    menuView->addAction(actRescaleSelectedSameRanges);
    menuView->addAction(actRemoveSelected);
    menuView->addAction(actFullscreen);
    menuParameters->addAction(parametersActionWidget);
    menuData->addMenu(menuFit);
    menuData->addMenu(menuProject);
    menuData->addMenu(menuScalarField);
    menuData->addMenu(menuTools);
    menuData->addAction(actExport);
    menuFit->addAction(actFitSphere);
    menuFit->addAction(actFitEllipsoid);
    menuFit->addAction(actFitPlan);
    menuFit->addAction(actFitMesh);
    menuFit->addAction(actFitPointCloud);
    menuProject->addAction(actProjectSphere);
    menuProject->addAction(actProjectPlan);
    menuProject->addAction(actProjectMesh);
    menuMesh->addAction(actMeshLoad);
    menuMesh->addAction(actMeshCreateRotegrity);
    menuTools->addAction(actRandomSubSample);
    menuTools->addAction(actMove);
    menuScalarField->addAction(actComputeSolidHarmonics);

    QObject::connect(cb_show_hide_labels,SIGNAL(stateChanged(int)),this,SLOT(slot_showHideLabels(int)));
    QObject::connect(cb_show_hide_grid,SIGNAL(stateChanged(int)),this,SLOT(slot_showHideGrid(int)));
    QObject::connect(cb_show_hide_axis,SIGNAL(stateChanged(int)),this,SLOT(slot_showHideAxis(int)));
    QObject::connect(actMeshLoad,SIGNAL(triggered()),this,SLOT(slot_addMesh()));
    QObject::connect(actMeshCreateRotegrity,SIGNAL(triggered()),this,SLOT(slot_createRotegrity()));
    QObject::connect(actExport,SIGNAL(triggered()),this,SLOT(slot_export()));
    QObject::connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_resetView()));
    QObject::connect(actRescaleSelectedSameRanges,SIGNAL(triggered()),this,SLOT(slot_resetViewOnSelectedSameRanges()));
    QObject::connect(actRescaleSelected,SIGNAL(triggered()),this,SLOT(slot_resetViewOnSelected()));
    QObject::connect(actFitPointCloud,SIGNAL(triggered()),this,SLOT(slot_fitPointCloud()));
    QObject::connect(actComputeSolidHarmonics,SIGNAL(triggered()),this,SLOT(slot_computeSolidHarmonics()));
    QObject::connect(actRemoveSelected,SIGNAL(triggered()),this,SLOT(slot_removeSelected()));
    QObject::connect(actRandomSubSample,SIGNAL(triggered()),this,SLOT(slot_randomSubSamples()));
    QObject::connect(actMove,SIGNAL(triggered()),this,SLOT(slot_movePointCloud()));
    QObject::connect(actSave,SIGNAL(triggered()),this,SLOT(slot_saveImage()));
    QObject::connect(actSaveRevolution,SIGNAL(triggered()),this,SLOT(slot_saveRevolution()));
    QObject::connect(actProjectMesh, SIGNAL(triggered() ), this, SLOT(slot_projectCustomMesh()));
    QObject::connect(actProjectPlan, SIGNAL(triggered() ), this, SLOT(slot_projectPlan()));
    QObject::connect(actProjectSphere, SIGNAL(triggered() ), this, SLOT(slot_projectSphere()));
    QObject::connect(actFitEllipsoid, SIGNAL(triggered() ), this, SLOT(slot_fitEllipsoid()));
    QObject::connect(actFitSphere, SIGNAL(triggered() ), this, SLOT(slot_fitSphere()));
    QObject::connect(actFitPlan, SIGNAL(triggered() ), this, SLOT(slot_fitPlan()));
    QObject::connect(actFitMesh, SIGNAL(triggered() ), this, SLOT(slot_fitCustomMesh()));
    QObject::connect(actFullscreen, SIGNAL(triggered(bool)), customContainer, SLOT(slot_fullscreen(bool)));
    QObject::connect(actCopy, SIGNAL(triggered()), this, SLOT(slot_copy()));
    QObject::connect(actPaste, SIGNAL(triggered()), this, SLOT(slot_paste()));

}

void Viewer3D::slot_saveImage()
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

void Viewer3D::slot_copy()
{
    std::vector<Cloud3D*> list=getClouds(true);

    if (list.size()>0)
    {
        QMimeData * mimeData=new QMimeData();
        QByteArray rawData=list[0]->cloud->toByteArray();
        mimeData->setData("Cloud",rawData);
        QApplication::clipboard()->setMimeData(mimeData);
    }
}

void Viewer3D::slot_paste()
{
    const QMimeData * mimeData=QApplication::clipboard()->mimeData();

    if(mimeData)
    {
        if(mimeData->hasFormat("Cloud"))
        {
            Cloud * cloud=new Cloud();
            QByteArray _data=mimeData->data("Cloud");
            cloud->fromByteArray(_data);

            if(cloud->type()==Cloud::Type::TYPE_TRANSFORMS)
            {
                addCloudScalar(cloud,Qt3DRender::QGeometryRenderer::PrimitiveType::Lines);
            }
            else
            {
                addCloudScalar(cloud,Qt3DRender::QGeometryRenderer::PrimitiveType::Points);
            }
        }
    }

}

void Viewer3D::slot_saveRevolution()
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

        QFileInfo infoFilename(filename);

        Qt3DRender::QRenderCapture* capture = new Qt3DRender::QRenderCapture;
        this->activeFrameGraph()->setParent(capture);
        this->setActiveFrameGraph(capture);

        MyCapture myCap(capture);
        float a0=cameraParams->getAlpha();
        QScreen* screen=QGuiApplication::primaryScreen();

        for (int k=0; k<N; k++)
        {
            QString image_filename=infoFilename.dir().path()+QString("/%1_").arg(k)+infoFilename.baseName()+QString(".png");
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

void Viewer3D::slot_fitEllipsoid()
{
    std::vector<Cloud3D *> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;

        double Ra=(currentCloud->getXRange().upper-currentCloud->getXRange().lower)/2;
        double Rb=(currentCloud->getYRange().upper-currentCloud->getXRange().lower)/2;
        double Rc=(currentCloud->getZRange().upper-currentCloud->getXRange().lower)/2;
        bool searchRotation=false;
        Eigen::Vector3d B=currentCloud->getBarycenter();
        double Cx=B[0];
        double Cy=B[1];
        double Cz=B[2];

        //dialog------------------------------
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Fit Ellipsoid");
        QGridLayout* gbox = new QGridLayout();
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);
        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        QDoubleSpinBox * sb_Ra=new QDoubleSpinBox(dialog);
        sb_Ra->setRange(0,1e100);sb_Ra->setValue(Ra);
        sb_Ra->setPrefix("Ra=");
        QDoubleSpinBox * sb_Rb=new QDoubleSpinBox(dialog);
        sb_Rb->setRange(0,1e100);sb_Rb->setValue(Rb);
        sb_Rb->setPrefix("Rb=");
        QDoubleSpinBox * sb_Rc=new QDoubleSpinBox(dialog);
        sb_Rc->setRange(0,1e100);sb_Rc->setValue(Rc);
        sb_Rc->setPrefix("Rc=");

        QDoubleSpinBox * sb_Cx=new QDoubleSpinBox(dialog);
        sb_Cx->setRange(0,1e100);sb_Cx->setValue(Cx);
        sb_Cx->setPrefix("Cx=");
        QDoubleSpinBox * sb_Cy=new QDoubleSpinBox(dialog);
        sb_Cy->setRange(0,1e100);sb_Cy->setValue(Cy);
        sb_Cy->setPrefix("Cy=");
        QDoubleSpinBox * sb_Cz=new QDoubleSpinBox(dialog);
        sb_Cz->setRange(0,1e100);sb_Cz->setValue(Cz);
        sb_Cz->setPrefix("Cz=");

        QCheckBox * cb_searchRotation=new QCheckBox(dialog);
        cb_searchRotation->setChecked(searchRotation);
        cb_searchRotation->setText("Search a rotation");

        gbox->addWidget(sb_Ra,0,0);
        gbox->addWidget(sb_Rb,1,0);
        gbox->addWidget(sb_Rc,2,0);
        gbox->addWidget(sb_Cx,0,1);
        gbox->addWidget(sb_Cy,1,1);
        gbox->addWidget(sb_Cz,2,1);
        gbox->addWidget(cb_searchRotation,3,0,1,2);
        gbox->addWidget(buttonBox,4,0,1,2);
        dialog->setLayout(gbox);

        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            Ra=sb_Ra->value();
            Rb=sb_Rb->value();
            Rc=sb_Rc->value();
            Cx=sb_Cx->value();
            Cy=sb_Cx->value();
            Cz=sb_Cx->value();
            searchRotation=cb_searchRotation->isChecked();

            //------------------------------

            Ellipsoid * ellipsoid=new Ellipsoid(Eigen::Vector3d(Cx,Cy,Cz),Ra,Rb,Rc,searchRotation);
            currentCloud->fit(ellipsoid);

            Eigen::Vector3d C=ellipsoid->getCenter();
            Eigen::Vector3d Ax=ellipsoid->getR().col(0);
            Eigen::Vector3d Ay=ellipsoid->getR().col(1);
            Eigen::Vector3d Az=ellipsoid->getR().col(2);

            addEllipsoid(ellipsoid,QColor(64,64,64));


            //addSphere(QPosAtt(C,Eigen::Quaterniond(1,0,0,0)),1.0,QColor(64,64,64),sphere.getRadius());
            QString results;
            results+=QString("\nCenter=(%1 , %2 , %3)").arg(C[0]).arg(C[1]).arg(C[2]);
            results+=QString("\nRadius=(%1 , %2 , %3)").arg(ellipsoid->getA()).arg(ellipsoid->getB()).arg(ellipsoid->getC());
            results+=QString("\nAxis:");
            results+=QString("\nAx=(%1 , %2 , %3)").arg(Ax[0]).arg(Ax[1]).arg(Ax[2]);
            results+=QString("\nAy=(%1 , %2 , %3)").arg(Ay[0]).arg(Ay[1]).arg(Ay[2]);
            results+=QString("\nAz=(%1 , %2 , %3)").arg(Az[0]).arg(Az[1]).arg(Az[2]);
            results+=QString("\nRms=%7").arg(ellipsoid->getRMS());

            emit sig_displayResults( QString("Fit Ellipsoid:")+results);
            emit sig_newColumn("Err_Ellipsoid",ellipsoid->getErrNorm());
        }
    }
}

void Viewer3D::slot_fitSphere()
{
    std::vector<Cloud3D *> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;

        Eigen::Vector3d barycenter;
        double boundingRadius;
        currentCloud->getBarycenterAndBoundingRadius(barycenter,boundingRadius);
        Sphere * sphere=new Sphere(barycenter,boundingRadius/2.0);
        currentCloud->fit(sphere);

        Eigen::Vector3d C=sphere->getCenter();

        addSphere(sphere,QColor(64,64,64));


        //addSphere(QPosAtt(C,Eigen::Quaterniond(1,0,0,0)),1.0,QColor(64,64,64),sphere.getRadius());

        emit sig_displayResults( QString("Fit Sphere:\nCenter=(%1 , %2 , %3) Radius=%4\nRms=%5\n").arg(C[0]).arg(C[1]).arg(C[2]).arg(sphere->getRadius()).arg(sphere->getRMS()) );
        emit sig_newColumn("Err_Sphere",sphere->getErrNorm());
    }
}

void Viewer3D::slot_fitPlan()
{
    std::vector<Cloud3D *> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;

        Eigen::Vector3d barycenter;
        double boundingRadius;
        currentCloud->getBarycenterAndBoundingRadius(barycenter,boundingRadius);

        Plan * plan;
        Plan * planA=new Plan(Eigen::Vector3d(1,0,0),barycenter);
        Plan * planB=new Plan(Eigen::Vector3d(0,1,0),barycenter);
        Plan * planC=new Plan(Eigen::Vector3d(0,0,1),barycenter);
        currentCloud->fit(planA);
        currentCloud->fit(planB);
        currentCloud->fit(planC);

        if (planA->getRMS()<planB->getRMS() && planA->getRMS()<planC->getRMS())
        {
            plan=planA;
            delete planB;
            delete planC;
        }
        else if (planB->getRMS()<planA->getRMS() && planB->getRMS()<planC->getRMS())
        {
            plan=planB;
            delete planA;
            delete planC;
        }
        else
        {
            plan=planC;
            delete planA;
            delete planB;
        }

        Eigen::Vector3d N=plan->getNormal();

        addPlan(plan,boundingRadius*2,QColor(128,128,128));

        emit sig_displayResults( QString("Fit Plan:\nNormal=+-(%1 , %2 , %3)\nRms=%4\n").arg(N[0]).arg(N[1]).arg(N[2]).arg(plan->getRMS()));
        emit sig_newColumn("Err_Plan",plan->getErrNorm());
    }
}


void Viewer3D::slot_projectPlan()
{
    std::vector<Cloud3D *> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
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

void Viewer3D::slot_projectSphere()
{
    std::vector<Cloud3D *> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;
        Eigen::Vector3d barycenter;
        double boundingRadius;
        currentCloud->getBarycenterAndBoundingRadius(barycenter,boundingRadius);

        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("project on a Sphere");
        dialog->setMinimumWidth(400);
        QGridLayout* gbox = new QGridLayout();
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        QDoubleSpinBox* sb_radius=new QDoubleSpinBox(dialog);
        sb_radius->setValue(boundingRadius);
        sb_radius->setPrefix("radius=");
        sb_radius->setRange(-10000000,10000000);

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

void Viewer3D::slot_projectCustomMesh()
{
    std::vector<Cloud3D *> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        QString filename=QFileDialog::getOpenFileName(nullptr,"Project on 3D Mesh","./obj","Object (*.obj)");
        std::cout<<filename.toLocal8Bit().data()<<std::endl;
        if(filename.isEmpty())return;

        Object obj(filename,PosAtt());
        currentCloud->project(&obj);

        currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()) );
        //setCloudScalar(cloud,PrimitiveMode::MODE_POINTS);
    }
}

void Viewer3D::slot_randomSubSamples()
{
    std::vector<Cloud3D *> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Random Subsamples");
        dialog->setMinimumWidth(400);
        QGridLayout* gbox = new QGridLayout();
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        QSpinBox* sb_subSamples=new QSpinBox(dialog);
        sb_subSamples->setPrefix("Samples=");
        sb_subSamples->setRange(0,currentCloud->size());

        QSlider* sb_slideSubSamples=new QSlider(Qt::Horizontal,dialog);
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

            currentCloud->subSample(nbPoints);

            if(nbPoints!=currentCloud->size())
            {
                std::cout<<"error : "<<nbPoints<<" "<<currentCloud->size()<<std::endl;
            }

            currentCloud3D->update(customContainer->getColorScale()->dataRange());
        }
    }
}

void Viewer3D::slot_fitCustomMesh()
{
    std::vector<Cloud3D *> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;

        QString filename=QFileDialog::getOpenFileName(nullptr,"3D Mesh","./obj","Object (*.obj)");
        std::cout<<filename.toLocal8Bit().data()<<std::endl;
        if(filename.isEmpty())return;

        QElapsedTimer timer;
        timer.start();

        Object * obj=new Object(filename,PosAtt());

        Eigen::Vector3d barycenter;
        double boundingRadius;
        currentCloud->getBarycenterAndBoundingRadius(barycenter,boundingRadius);

        Vector3d obj_center=obj->getBox().middle();
        obj->setScalePosAtt(PosAtt(barycenter-obj_center,Eigen::Quaterniond(boundingRadius/obj->getRadius(obj_center),0,0,0)));

        currentCloud->fit(obj,100);

        QFileInfo info(filename);

        obj->save(info.path()+"/tmp.obj");

        auto* m_obj = new Qt3DRender::QMesh();
        m_obj->setSource(QUrl(QString("file:///")+info.path()+"/tmp.obj"));

        addObject(m_obj,obj,PosAtt(),1.0,QColor(64,64,64));

        PosAtt posatt=obj->getPosAtt();
        emit sig_displayResults( QString("Fit Mesh :\nScale=%1\nPosition=(%2,%3,%4)\nQ=(%5,%6,%7,%8)\nRms=%9\ndt=%10 ms")
                                 .arg(obj->getScale())
                                 .arg(posatt.P[0]).arg(posatt.P[1]).arg(posatt.P[2])
                .arg(posatt.Q.w()).arg(posatt.Q.x()).arg(posatt.Q.y()).arg(posatt.Q.w())
                .arg(obj->getRMS())
                .arg(timer.nsecsElapsed()*1e-6));

        emit sig_newColumn("Err_Mesh",obj->getErrNorm());
    }
}

//----------------------------




void Viewer3D::slot_ColorScaleChanged(const QCPRange& range)
{
    std::vector<Cloud3D *> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        if (currentCloud)
        {
            currentCloud3D->update(range);
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

void Viewer3D::slot_ScaleChanged()
{
    Qt3DCore::QTransform T,S;
    T.setTranslation(-customContainer->getTranslation());
    S.setScale3D(customContainer->getScaleInv());

    for(unsigned int i=0;i<objects3D.size();i++)
    {
        objects3D[i]->transform->setMatrix( S.matrix()*T.matrix()*objects3D[i]->tT*objects3D[i]->tR );//->setMatrix(t.matrix().inverted());
    }
}

void Viewer3D::slot_setGradient(int preset)
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        if (currentCloud)
        {
            currentCloud->setGradientPreset(static_cast<QCPColorGradient::GradientPreset>(preset));
            customContainer->getColorScale()->setGradient(currentCloud->getGradient());
            //currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()));
            //currentCloud3D->update(customContainer->getColorScale()->dataRange());
            customContainer->getColorScalePlot()->rescaleAxes();
            customContainer->getColorScalePlot()->replot();
        }
    }
}

void Viewer3D::slot_useCustomColor(int value)
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        if (currentCloud)
        {
            currentCloud->setUseCustomColor(value);
            //currentCloud3D->buffer->setData(currentCloud->getBuffer(customContainer->getColorScale()->dataRange()));
            currentCloud3D->update(customContainer->getColorScale()->dataRange());
        }
    }
}

void Viewer3D::slot_setCustomColor(QColor color)
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud3D * currentCloud3D=selectedClouds[i];
        Cloud * currentCloud=currentCloud3D->cloud;

        if (currentCloud)
        {
            currentCloud->setCustomColor(color.rgb());
            currentCloud3D->update(customContainer->getColorScale()->dataRange());
        }
    }
}

void Viewer3D::addCloudScalar(Cloud* cloudData,
                              Qt3DRender::QGeometryRenderer::PrimitiveType primitiveMode)
{
    Cloud3D * currentCloud3D=new Cloud3D(cloudData,rootEntity);

    currentCloud3D->geometryRenderer->setPrimitiveType(primitiveMode);


    {
        customContainer->getColorScale()->setGradient(currentCloud3D->cloud->getGradient());

        customContainer->getXAxis()->setLabel(cloudData->getLabelX());
        customContainer->getYAxis()->setLabel(cloudData->getLabelY());
        customContainer->getZAxis()->setLabel(cloudData->getLabelZ());
        customContainer->getColorScale()->axis()->setLabel(cloudData->getLabelS());

        labelx->setText(customContainer->getXAxis()->label());
        labely->setText(customContainer->getYAxis()->label());
        labelz->setText(customContainer->getZAxis()->label());
    }

    referenceObjectEntity(currentCloud3D,cloudData->getName());

    setCloudPointSize(currentCloud3D,currentCloud3D->pointSize->value());

    slot_resetViewOnSameRanges();

    customContainer->adjustSize();
    customContainer->replot();
}

std::vector<Base3D*> Viewer3D::getMeshs(bool selected)
{
    std::vector<Base3D*> meshList;

    for(unsigned int i=0;i<objects3D.size();i++)
    {
        QListWidgetItem * currentItem= customContainer->getSelectionView()->item(i);

        Base3D * ptrPlan3D=dynamic_cast<Plan3D*>(objects3D[i]);
        Base3D * ptrSphere3D=dynamic_cast<Sphere3D*>(objects3D[i]);
        Base3D * ptrObject3D=dynamic_cast<Object3D*>(objects3D[i]);
        Base3D * ptrEllipsoid3D=dynamic_cast<Ellipsoid3D*>(objects3D[i]);

        if(currentItem->isSelected() || selected==false)
        {
            if(ptrPlan3D){meshList.push_back(ptrPlan3D);}
            if(ptrSphere3D){meshList.push_back(ptrSphere3D);}
            if(ptrObject3D){meshList.push_back(ptrObject3D);}
            if(ptrEllipsoid3D){meshList.push_back(ptrEllipsoid3D);}
        }
    }

    return meshList;
}

std::vector<Cloud3D*> Viewer3D::getClouds(bool selected)
{
    std::vector<Cloud3D*> selectedClouds;

    unsigned int itemsCount=customContainer->getSelectionView()->count();

    if(objects3D.size()!=itemsCount)
    {
        std::cout<<"Error"<<std::endl;
        return selectedClouds;
    }

    for(unsigned int i=0;i<itemsCount;i++)
    {
        QListWidgetItem * currentItem= customContainer->getSelectionView()->item(i);

        Cloud3D* ptrCloud=dynamic_cast<Cloud3D*>(objects3D[i]);
        if(ptrCloud)
        {
            if(currentItem->isSelected() || selected==false)
            {
                selectedClouds.push_back(ptrCloud);
            }
        }
    }
    return selectedClouds;
}

std::vector<Base3D*> Viewer3D::getObjects(bool selected)
{
    std::vector<Base3D*> selectedObjects;

    unsigned int itemsCount=customContainer->getSelectionView()->count();

    if(objects3D.size()!=itemsCount)
    {
        std::cout<<"Error"<<std::endl;
        return selectedObjects;
    }

    for(unsigned int i=0;i<itemsCount;i++)
    {
        QListWidgetItem * currentItem= customContainer->getSelectionView()->item(i);

        if(currentItem->isSelected() || selected==false)
        {
            selectedObjects.push_back(objects3D[i]);
        }

    }
    return selectedObjects;
}

void Viewer3D::slot_removeSelected()
{
    std::cout<<"Remove Selected"<<std::endl;

    std::vector<Base3D*> selectedObjects=getObjects(true);

    for(unsigned int i=0;i<selectedObjects.size();i++)
    {
        for(unsigned int k=0;k<objects3D.size();k++)
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

void Viewer3D::setCloudPointSize(Cloud3D * currentCloud3D,double value)
{
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

void Viewer3D::slot_setPointSize(double value)
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        setCloudPointSize(selectedClouds[i],value);
    }
}

void Viewer3D::slot_setPrimitiveType(int type)
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
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

void Viewer3D::addEllipsoid(Ellipsoid * ellipsoid,QColor color)
{
    Ellipsoid3D* ellipsoid3D=new Ellipsoid3D(rootEntity,ellipsoid,color);
    referenceObjectEntity(ellipsoid3D,"Ellipsoid");
    slot_ScaleChanged();
}

void Viewer3D::addSphere(Sphere * sphere,QColor color)
{
    Sphere3D* sphere3D=new Sphere3D(rootEntity,sphere,color);
    referenceObjectEntity(sphere3D,"Sphere");
    slot_ScaleChanged();
}

void Viewer3D::addPlan(Plan* plan,float radius,QColor color)
{
    Plan3D* plan3D=new Plan3D(rootEntity,plan,radius,color);
    referenceObjectEntity(plan3D,"Plan");
    slot_ScaleChanged();
}

void Viewer3D::referenceObjectEntity(Base3D * base3D,QString name)
{
    objects3D.push_back(base3D);

    QListWidgetItem * item=new QListWidgetItem(name);

    if(dynamic_cast<Cloud3D*>(base3D)!=nullptr)
    {
        Cloud3D* pcloud=dynamic_cast<Cloud3D*>(base3D);
        if(pcloud->cloud->type()==Cloud::Type::TYPE_TRANSFORMS)
        {
            item->setIcon(QIcon(QPixmap::fromImage(QImage(":/img/icons/transforms_cloud.png"))));
        }
        else
        {
            item->setIcon(QIcon(QPixmap::fromImage(QImage(":/img/icons/points_cloud.png"))));
        }
    }
    else //Todo differents icon here
    {
        item->setIcon(QIcon(QPixmap::fromImage(QImage(":/img/icons/shape_icosahedron.gif"))));
    }

    customContainer->getSelectionView()->addItem(item);
    item->setSelected(true);
}

void Viewer3D::addObject(Qt3DRender::QMesh* mesh_object,Object * object, PosAtt posatt,float scale,QColor color)
{
    Object3D* object3D=new Object3D(rootEntity,mesh_object,object,posatt,scale,color);

    referenceObjectEntity(object3D,QString("Mesh : %1").arg(QFileInfo(mesh_object->source().fileName()).baseName()));

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

CustomViewContainer* Viewer3D::getContainer()
{
    return customContainer;
}

void Viewer3D::applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map)
{
    QMap<QString,QAction*> shortcuts_links;
    shortcuts_links.insert(QString("Graph-Delete"),actRemoveSelected);
    shortcuts_links.insert(QString("Graph-Rescale"),actRescale);
    shortcuts_links.insert(QString("Graph-RescaleSelected"),actRescaleSelected);
    shortcuts_links.insert(QString("Graph-RescaleSelectedSameRanges"),actRescaleSelectedSameRanges);
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

void Viewer3D::slot_movePointCloud()
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);

    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Move points clouds");
    QGridLayout* gbox = new QGridLayout();
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QDoubleSpinBox * sb_Rx=new QDoubleSpinBox(dialog);
    QDoubleSpinBox * sb_Ry=new QDoubleSpinBox(dialog);
    QDoubleSpinBox * sb_Rz=new QDoubleSpinBox(dialog);
    QDoubleSpinBox * sb_Tx=new QDoubleSpinBox(dialog);
    QDoubleSpinBox * sb_Ty=new QDoubleSpinBox(dialog);
    QDoubleSpinBox * sb_Tz=new QDoubleSpinBox(dialog);
    QDoubleSpinBox * sb_scale=new QDoubleSpinBox(dialog);
    sb_Rx->setRange(-360,360); sb_Rx->setDecimals(5);sb_Rx->setPrefix("Rx=");sb_Rx->setSuffix("°");
    sb_Ry->setRange(-360,360); sb_Ry->setDecimals(5);sb_Ry->setPrefix("Ry=");sb_Ry->setSuffix("°");
    sb_Rz->setRange(-360,360); sb_Rz->setDecimals(5);sb_Rz->setPrefix("Rz=");sb_Rz->setSuffix("°");
    sb_Tx->setRange(-1e100,1e100); sb_Tx->setDecimals(5);sb_Tx->setPrefix("Tx=");
    sb_Ty->setRange(-1e100,1e100); sb_Ty->setDecimals(5);sb_Ty->setPrefix("Ty=");
    sb_Tz->setRange(-1e100,1e100); sb_Tz->setDecimals(5);sb_Tz->setPrefix("Tz=");
    sb_scale->setRange(-1e100,1e100); sb_scale->setDecimals(5);sb_scale->setPrefix("Scale=");
    sb_scale->setValue(1.0);

    gbox->addWidget(sb_Rx,0,0);
    gbox->addWidget(sb_Ry,1,0);
    gbox->addWidget(sb_Rz,2,0);
    gbox->addWidget(sb_Tx,0,1);
    gbox->addWidget(sb_Ty,1,1);
    gbox->addWidget(sb_Tz,2,1);
    gbox->addWidget(sb_scale,3,0,1,2);
    gbox->addWidget(buttonBox,4,0,1,2);
    dialog->setLayout(gbox);

    int result=dialog->exec();
    if (result == QDialog::Accepted)
    {
        for(int i=0;i<selectedClouds.size();i++)
        {
            Eigen::Matrix3d R= Eigen::Matrix3d(Eigen::AngleAxisd(sb_Rx->value()*M_PI/180, Eigen::Vector3d::UnitX())
                             * Eigen::AngleAxisd(sb_Ry->value()*M_PI/180, Eigen::Vector3d::UnitY())
                             * Eigen::AngleAxisd(sb_Rz->value()*M_PI/180, Eigen::Vector3d::UnitZ()));

            Eigen::Vector3d T(sb_Tx->value(),sb_Ty->value(),sb_Tz->value());

            selectedClouds[i]->cloud->move(R,T,sb_scale->value());
            selectedClouds[i]->update(customContainer->getColorScale()->dataRange());
        }
    }
}

void Viewer3D::slot_fitPointCloud()
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);
    std::vector<Cloud3D*> cloudsList=getClouds(false);

    if(selectedClouds.size()==1)
    {
        Cloud3D* pcA=selectedClouds[0];
        if(cloudsList.size()>=2)
        {
            QDialog* dialog=new QDialog;
            dialog->setLocale(QLocale("C"));
            dialog->setWindowTitle("Point cloud registration");
            QGridLayout* gbox = new QGridLayout();

            QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);

            QComboBox * cb_algorithm=new QComboBox(dialog);
            cb_algorithm->addItem("ARUN");
            cb_algorithm->addItem("HORN");

            QComboBox * cb_cloudsList=new QComboBox(dialog);
            for(int i=0;i<cloudsList.size();i++)
            {
                if(cloudsList[i]!=pcA)
                {
                    cb_cloudsList->addItem(cloudsList[i]->cloud->getName(),QVariant::fromValue(static_cast<void*>(cloudsList[i])));
                }
            }

            QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
            QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

            gbox->addWidget(cb_algorithm,0,0,1,3);
            gbox->addWidget(new QLabel(QString("Project ")),1,0);
            gbox->addWidget(cb_cloudsList,1,1);
            gbox->addWidget(new QLabel(QString(" on %1").arg(pcA->cloud->getName())),1,2);
            gbox->addWidget(buttonBox,2,0,1,3);
            dialog->setLayout(gbox);

            int result=dialog->exec();
            if (result == QDialog::Accepted)
            {
                Cloud3D* pcB=static_cast<Cloud3D*>(cb_cloudsList->currentData().value<void*>());

                if(cb_algorithm->currentIndex()==0)
                {
                    if(!computeArun(pcA->cloud,pcB->cloud))
                    {
                        QMessageBox::information(nullptr,"Information Arun","Point cloud must have the same number of points");
                    }
                }
                else if(cb_algorithm->currentIndex()==1)
                {
                    if(!computeHorn(pcA->cloud,pcB->cloud))
                    {
                        QMessageBox::information(nullptr,"Information Arun","Point cloud must have the same number of points");
                    }
                }
            }
        }
        else
        {
            QMessageBox::information(nullptr,"Information fit point cloud","You dont have enough point cloud");
        }
    }
    else
    {
        QMessageBox::information(nullptr,"Information fit point cloud","Please select a cloud to project");
    }
}

bool Viewer3D::computeHorn(Cloud * pcA,Cloud * pcB)
{
    std::vector<Eigen::Vector3d> Pa=pcA->positions();
    std::vector<Eigen::Vector3d> Pb=pcB->positions();
    if(Pa.size()!=Pb.size())
    {
        return false;
    }

    Eigen::Quaterniond Q(1,0,0,0);
    Eigen::Vector3d T=Eigen::Vector3d::Zero();
    double S=1.0;

    QString PaName=pcA->getName();
    QString PbName=pcB->getName();

    std::cout<<"Compute Horn"<<std::endl;

    algorithms::computeHornTranform(Pa,Pb,Q,T,S);
    Eigen::VectorXd HornE=algorithms::getHornError(Pa,Pb,Q,T,S);
    double rms=HornE.norm()/sqrt(HornE.rows());

    std::vector<Eigen::Vector3d> Pc(Pa.size());
    Eigen::Matrix3d R=Q.toRotationMatrix();
    for(int i=0;i<Pc.size();i++)
    {
        Pc[i]=S*(R*Pb[i]+T);
    }
    Cloud * pcC=new Cloud(Pc,HornE,"X","Y","Z","Error");
    pcC->setName(QString("Pc (%1 projected on %2) [Horn]").arg(pcB->getName()).arg(pcA->getName()));
    addCloudScalar(pcC);

    emit sig_displayResults( QString("Compute Horn (Pa=S*(R*Pb[i]+T))\n"
                                     "Pa=%15\n"
                                     "Pb=%16\n"
                                     "Q=(%1 , %2 , %3 , %4)\n"
                                     "T=(%10 , %11 , %12)\n"
                                     "S=%13\n"
                                     "Rms=%14\n")
                             .arg(Q.w()).arg(Q.x()).arg(Q.y()).arg(Q.z())
                             .arg(T[0]).arg(T[1]).arg(T[2])
                             .arg(S)
                             .arg(rms).arg(PaName).arg(PbName));

    emit sig_newColumn("Err_Horn",HornE);

    return true;
}

bool Viewer3D::computeArun(Cloud * pcA,Cloud * pcB)
{
    std::vector<Eigen::Vector3d> Pa=pcA->positions();
    std::vector<Eigen::Vector3d> Pb=pcB->positions();

    if(Pa.size()!=Pb.size())
    {
        return false;
    }

    Eigen::Matrix3d R=Eigen::Matrix3d::Identity();
    Eigen::Vector3d T=Eigen::Vector3d::Zero();

    QString PaName=pcA->getName();
    QString PbName=pcB->getName();

    std::cout<<"Compute Arun :"<<std::endl;

    algorithms::computeArunTranform(Pa,Pb,R,T);
    Eigen::VectorXd ArunE=algorithms::getArunError(Pa,Pb,R,T);
    double rms=ArunE.norm()/sqrt(ArunE.rows());

    std::vector<Eigen::Vector3d> Pc(Pa.size());
    for(int i=0;i<Pc.size();i++)
    {
        Pc[i]=R*Pb[i]+T;
    }
    Cloud * pcC=new Cloud(Pc,ArunE,"X","Y","Z","Error");
    pcC->setName(QString("Pc (%1 projected on %2) [Arun]").arg(pcB->getName()).arg(pcA->getName()));
    addCloudScalar(pcC);

    emit sig_displayResults( QString("Compute Arun (Pa=R Pb+ T):\n"
                                     "Pa=%14\n"
                                     "Pb=%15\n"
                                     "R=\n"
                                     "%1 , %2 , %3\n"
                                     "%4 , %5 , %6\n"
                                     "%7 , %8 , %9\n"
                                     "T=(%10 , %11 , %12)\n"
                                     "Rms=%13\n")
                             .arg(R(0,0)).arg(R(0,1)).arg(R(0,2))
                             .arg(R(1,0)).arg(R(1,1)).arg(R(1,2))
                             .arg(R(2,0)).arg(R(2,1)).arg(R(2,2))
                             .arg(T[0]).arg(T[1]).arg(T[2])
                             .arg(rms).arg(PaName).arg(PbName));

    emit sig_newColumn("Err_Arun",ArunE);

    return true;
}

void Viewer3D::slot_itemDoubleClicked(int index)
{
    std::vector<Base3D*> selectedObjects=getObjects(true);

    for(int i=0;i<selectedObjects.size();i++)
    {
        //Edit Cloud
        Cloud3D * pcloud3D=dynamic_cast<Cloud3D *>(selectedObjects[i]);
        if(pcloud3D)
        {
            ConfigurationCloudDialog dialog(nullptr,pcloud3D,this);
            int result=dialog.exec();
            if (result == QDialog::Accepted)
            {
                dialog.apply();
                customContainer->getSelectionView()->item(index)->setText(pcloud3D->cloud->getName());
            }
        }

        //Edit Mesh

    }
}

void Viewer3D::slot_computeSolidHarmonics()
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);

    if(selectedClouds.size()==1)
    {
        Cloud * pcl=selectedClouds[0]->cloud;

        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Solid Harmonics decomposition");
        QGridLayout* gbox = new QGridLayout();

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);

        QSpinBox * sb_order=new QSpinBox(dialog);
        sb_order->setPrefix("Order=");
        sb_order->setValue(3);
        sb_order->setRange(1,SolidHarmonicsDecomposition::maxOrder());

        QComboBox * cb_mode=new QComboBox(dialog);
        cb_mode->addItem("REGULAR",int(SolidHarmonicsDecomposition::MODE_REGULAR));
        cb_mode->addItem("IRREGULAR",int(SolidHarmonicsDecomposition::MODE_IRREGULAR));

        QLabel * l_eqn=new QLabel;
        l_eqn->setPixmap(QPixmap(":/eqn/eqn/SolidHarmonics_Regular.gif"));

        QCheckBox * cb_optimized=new QCheckBox("Optimized",dialog);
        cb_optimized->setChecked(false);
        cb_optimized->setToolTip("For non-uniformly set of points basic correlation may be not sufficient "
                                 "in order to estimate the harmonics decomposition. Enable this option will perform"
                                 "Levenberg-Marquardt optimisation on data starting with basic correlation result.");

        gbox->addWidget(l_eqn,0,0,1,2);
        gbox->addWidget(sb_order,1,0);
        gbox->addWidget(cb_mode,1,1);
        gbox->addWidget(cb_optimized,2,1);

        gbox->addWidget(buttonBox,3,0,1,2);
        dialog->setLayout(gbox);

//        connect(cb_mode, &QComboBox::currentIndexChanged,[=](int index) { l_eqn->setPixmap(QPixmap(":/eqn/eqn/SolidHarmonics_Regular.gif")); });

        connect(cb_mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]( int index )
        {
            if(index==0){l_eqn->setPixmap(QPixmap(":/eqn/eqn/SolidHarmonics_Regular.gif"));}
            else if(index==1){l_eqn->setPixmap(QPixmap(":/eqn/eqn/SolidHarmonics_Irregular.gif"));}
        } );

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            std::cout<<"slot_computeSolidHarmonics...fit"<<std::endl;
            SolidHarmonicsDecomposition shd(sb_order->value()+1,static_cast<SolidHarmonicsDecomposition::Mode>(cb_mode->currentData().toInt()));
            std::cout<<"Number of harmonics="<<shd.nb_params()<<std::endl;

            shd.guessDecomposition(pcl->positionsAndScalarField());
            Eigen::MatrixXd C=shd.getC();
            std::cout<<"C="<<C<<std::endl;

            if(cb_optimized->isChecked())
            {
                pcl->fit(&shd);
            }

            QString str;
            for(int l=0;l<shd.order();l++)
            {
                for(int m=-l;m<=l;m++)
                {
                    str+=QString("C(%1,%2)=%3 \n").arg(l).arg(m).arg(shd.getC(l,m));
                }
//                str+=QString("\n");
            }

            str+=QString("\n");
            str+=QString("Expression\n");
            for(int l=0;l<shd.order();l++)
            {
                for(int m=-l;m<=l;m++)
                {
                    if(shd.getMode()==SolidHarmonicsDecomposition::MODE_IRREGULAR)
                    {
                        str+=QString("%3*solidHarmonicsI(x,y,z,%1,%2)+\n").arg(l).arg(m).arg(shd.getC(l,m));
                    }
                    else if(shd.getMode()==SolidHarmonicsDecomposition::MODE_REGULAR)
                    {
                        str+=QString("%3*solidHarmonicsR(x,y,z,%1,%2)+\n").arg(l).arg(m).arg(shd.getC(l,m));
                    }
                }
            }
            str+=QString("\n");

            emit sig_displayResults( QString("Fit SolidHarmonicsDecomposition:\n\nC(l,m)=\n")+str);
            emit sig_newColumn("Err_SolidHarmonicsDecomposition",shd.getErrNorm());
        }
    }
}



void Viewer3D::slot_updateLabels()
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

    objArrowX->setPosAtt(PosAtt(Px,Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(1,0,0),Eigen::Vector3d(0,-1,0))));
    objArrowY->setPosAtt(PosAtt(Py,Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,1,0),Eigen::Vector3d(0, 1,0))));
    objArrowZ->setPosAtt(PosAtt(Pz,Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d(0,0,1),Eigen::Vector3d(0,-1,0))));
}

void Viewer3D::slot_updateGridAndLabels()
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

void Viewer3D::mouseMoveEvent(QMouseEvent* event)
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

void Viewer3D::keyPressEvent(QKeyEvent * event)
{
    QList<QAction*> actions=customContainer->actions();
    for(auto act:actions)
    {
        if (isEquiv(event,act->shortcut())) {act->trigger();}
    }
}

void Viewer3D::slot_export()
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);

    for(unsigned int i=0;i<selectedClouds.size();i++)
    {
        Cloud * currentCloud=selectedClouds[i]->cloud;

        QString filename=QFileDialog::getSaveFileName(nullptr,"Cloud export data","Cloud.graphy","*.graphy");

        if(!filename.isEmpty())
        {
            QFile file(filename);

            if(file.open(QIODevice::WriteOnly | QIODevice::Text ))
            {
                QTextStream ts(&file);

                const std::vector<Eigen::Vector4d> & data=currentCloud->positionsAndScalarField();
                ts<<"<header>\n";
                ts<<"X;Y;Z;S;\n";
                ts<<"</header>\n";
                for(int k=0;k<currentCloud->size();k++)
                {
                    ts<<data[k][0]<<";"<<data[k][1]<<";"<<data[k][2]<<";"<<data[k][3]<<";\n";
                }
                file.close();
            }
        }
    }
}

void Viewer3D::slot_showHideGrid(int value)
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

void Viewer3D::slot_showHideAxis(int value)
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

void Viewer3D::slot_showHideLabels(int value)
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

void Viewer3D::slot_addMesh()
{
    QStringList filenames=QFileDialog::getOpenFileNames(nullptr,"3D Mesh","./obj","Object (*.obj)");

    if(filenames.size()==0)return;

    for(int i=0;i<filenames.size();i++)
    {
        Object * obj=new Object(filenames[i],PosAtt());
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
        //m_obj->setMeshName(QFileInfo(filenames[i]).baseName());
        addObject(m_obj,obj,PosAtt(),1.0,QColor(64,64,64));
    }

}

void Viewer3D::slot_createRotegrity()
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
        Object * objet=new Object(filename,PosAtt());
        QString filenameRotegrity=info.path()+"/"+info.baseName()+"_rotegrity.obj";

        objet->rotegrity(sb_angle->value(),
                      sb_subdivisions->value(),
                      sb_strech->value(),
                      sb_radius_int->value(),
                      sb_radius_dr->value(),
                      sb_width->value(),
                      sb_encA->value(),
                      sb_encB->value(),
                      filenameRotegrity,
                      true);

        BoundingBox bb=objet->getBox();
        customContainer->getXAxis()->setRange(QCPRange(bb.Pmin[0],bb.Pmax[0]));
        customContainer->getYAxis()->setRange(QCPRange(bb.Pmin[1],bb.Pmax[1]));
        customContainer->getZAxis()->setRange(QCPRange(bb.Pmin[2],bb.Pmax[2]));

        auto* mesh_object = new Qt3DRender::QMesh();
        mesh_object->setSource(QUrl(QString("file:///")+filenameRotegrity));

        addObject(mesh_object,objet,PosAtt(),1.0,QColor(64,64,64));

        customContainer->adjustSize();
        customContainer->replot();
    }
}

QCPRange regularized(const QCPRange & range)
{
    if (range.lower==range.upper)
    {
        return QCPRange(range.lower-1,range.lower+1);
    }
    else
    {
        return range;
    }
}

void Viewer3D::extendSameRanges(QCPRange itemRangeX,QCPRange itemRangeY,QCPRange itemRangeZ,int i)
{

    QCPRange rangex=customContainer->getXAxis()->range();
    QCPRange rangey=customContainer->getYAxis()->range();
    QCPRange rangez=customContainer->getZAxis()->range();

    QCPRange rangeMinMax=QCPRange(std::min(std::min(itemRangeX.lower,itemRangeY.lower),itemRangeZ.lower),
                                  std::max(std::max(itemRangeX.upper,itemRangeY.upper),itemRangeZ.upper));

    if(i!=0)
    {
        customContainer->getXAxis()->setRange(regularized(QCPRange(std::min(rangeMinMax.lower,rangex.lower),std::max(rangeMinMax.upper,rangex.upper))));
        customContainer->getYAxis()->setRange(regularized(QCPRange(std::min(rangeMinMax.lower,rangey.lower),std::max(rangeMinMax.upper,rangey.upper))));
        customContainer->getZAxis()->setRange(regularized(QCPRange(std::min(rangeMinMax.lower,rangez.lower),std::max(rangeMinMax.upper,rangez.upper))));
    }
    else
    {
        customContainer->getXAxis()->setRange(regularized(rangeMinMax));
        customContainer->getYAxis()->setRange(regularized(rangeMinMax));
        customContainer->getZAxis()->setRange(regularized(rangeMinMax));
    }
}



void Viewer3D::extendRanges(QCPRange itemRangeX,QCPRange itemRangeY,QCPRange itemRangeZ,int i)
{
    QCPRange rangex=customContainer->getXAxis()->range();
    QCPRange rangey=customContainer->getYAxis()->range();
    QCPRange rangez=customContainer->getZAxis()->range();

    if(i!=0)
    {
        customContainer->getXAxis()->setRange(regularized(QCPRange(std::min(itemRangeX.lower,rangex.lower),std::max(itemRangeX.upper,rangex.upper))));
        customContainer->getYAxis()->setRange(regularized(QCPRange(std::min(itemRangeY.lower,rangey.lower),std::max(itemRangeY.upper,rangey.upper))));
        customContainer->getZAxis()->setRange(regularized(QCPRange(std::min(itemRangeZ.lower,rangez.lower),std::max(itemRangeZ.upper,rangez.upper))));
    }
    else
    {
        customContainer->getXAxis()->setRange(regularized(itemRangeX));
        customContainer->getYAxis()->setRange(regularized(itemRangeY));
        customContainer->getZAxis()->setRange(regularized(itemRangeZ));
    }
}



void Viewer3D::extendScalarRange(QCPRange itemRangeS,int i)
{
    QCPRange ranges=customContainer->getColorScale()->dataRange();

    if(i!=0)
    {
        customContainer->getColorScale()->setDataRange(regularized(QCPRange(std::min(itemRangeS.lower,ranges.lower),std::max(itemRangeS.upper,ranges.upper))));
    }
    else
    {
        customContainer->getColorScale()->setDataRange(regularized(itemRangeS));
    }
}



void Viewer3D::resetRanges()
{
    QCPRange rangeReset(0,0);
    customContainer->getXAxis()->setRange(rangeReset);
    customContainer->getYAxis()->setRange(rangeReset);
    customContainer->getZAxis()->setRange(rangeReset);
}

void Viewer3D::dispRanges()
{
    std::cout<<"Ranges:"<<std::endl;
    std::cout<<customContainer->getXAxis()->range().lower<<" "<<customContainer->getXAxis()->range().upper<<std::endl;
    std::cout<<customContainer->getYAxis()->range().lower<<" "<<customContainer->getYAxis()->range().upper<<std::endl;
    std::cout<<customContainer->getZAxis()->range().lower<<" "<<customContainer->getZAxis()->range().upper<<std::endl;
}

void Viewer3D::updateScalarRanges(std::vector<Cloud3D*> & cloudsList)
{
    for(unsigned int i=0;i<cloudsList.size();i++)
    {
        //cloudsList[i]->buffer->setData(cloudsList[i]->cloud->getBuffer(customContainer->getColorScale()->dataRange()));
        cloudsList[i]->update(customContainer->getColorScale()->dataRange());
    }
}

void Viewer3D::updateRanges(std::vector<Cloud3D*> & cloudsList,bool same)
{
    resetRanges();
    for(unsigned int i=0;i<cloudsList.size();i++)
    {
        Cloud * currentCloud=cloudsList[i]->cloud;
        if(same)
        {
            extendSameRanges(currentCloud->getXRange(),currentCloud->getYRange(),currentCloud->getZRange(),i);
        }
        else
        {
            extendRanges(currentCloud->getXRange(),currentCloud->getYRange(),currentCloud->getZRange(),i);
        }
        extendScalarRange(currentCloud->getScalarFieldRange(),i);
    }
    updateScalarRanges(cloudsList);

    customContainer->getColorScalePlot()->rescaleAxes();
    customContainer->replot();

    slot_ScaleChanged();
}

void Viewer3D::slot_resetView()
{
    std::vector<Cloud3D*> cloudsList=getClouds(false);
    updateRanges(cloudsList,false);

    cameraParams->reset();
    slot_updateGridAndLabels();
}

void Viewer3D::slot_resetViewOnSameRanges()
{
    std::vector<Cloud3D*> cloudsList=getClouds(false);
    updateRanges(cloudsList,true);
}

void Viewer3D::slot_resetViewOnSelectedSameRanges()
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);
    updateRanges(selectedClouds,true);
}

void Viewer3D::slot_resetViewOnSelected()
{
    std::vector<Cloud3D*> selectedClouds=getClouds(true);
    updateRanges(selectedClouds,false);
}

void Viewer3D::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        cameraParams->reset();
        slot_updateGridAndLabels();
    }
}

void Viewer3D::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        xp=event->x();
        yp=event->y();
    }

    if (event->button() == Qt::RightButton)
    {
        //configurePopup();
        popupMenu->exec(mapToGlobal(event->pos()));
    }
}
void Viewer3D::wheelEvent(QWheelEvent* event)
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


//-------------------
ConfigurationCloudDialog::ConfigurationCloudDialog(QWidget * parent,
                                                   Cloud3D* pcloud3D,
                                                   Viewer3D * pviewer3D)
    :QDialog(parent)
{
    this->pcloud3D=pcloud3D;
    setLocale(QLocale("C"));
    setWindowTitle("Configure cloud");

    le_name=new QLineEdit();

    cb_use_custom_color=new QCheckBox("Use custom color");
    cw_custom_color=new ColorWheel();
    cw_custom_color->setMinimumSize(128,128);

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

    c_gradient=new MyGradientComboBox(nullptr);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout* gbox = new QGridLayout();
    gbox->addWidget(le_name,0,0,1,2);
    gbox->addWidget(c_gradient,1,0,1,2);
    gbox->addWidget(cb_use_custom_color,2,0);
    gbox->addWidget(cw_custom_color,2,1);
    gbox->addWidget(sb_size,3,0);
    gbox->addWidget(cb_mode,3,1);
    gbox->addWidget(buttonBox,4,0,1,2);
    setLayout(gbox);

    configure();
    connect(pviewer3D);
}

void ConfigurationCloudDialog::configure()
{
    le_name->setText(pcloud3D->cloud->getName());
    le_name->setFocus();

    sb_size->setValue(static_cast<double>(pcloud3D->pointSize->value()));
    cb_mode->setCurrentIndex(static_cast<int>(pcloud3D->geometryRenderer->primitiveType()));
    c_gradient->setCurrentIndex(static_cast<int>(pcloud3D->cloud->getGradientPreset()));

    sb_size->setEnabled(true);
    cb_mode->setEnabled(true);
    cb_use_custom_color->setEnabled(true);

    cb_use_custom_color->setChecked(pcloud3D->cloud->isCustomColorUsed());
    cw_custom_color->setColor(pcloud3D->cloud->getCustomColor());
}

void ConfigurationCloudDialog::connect(Viewer3D * pviewer3D)
{
    QObject::connect(sb_size, SIGNAL(valueChanged(double)), pviewer3D, SLOT(slot_setPointSize(double)));
    QObject::connect(cb_mode, SIGNAL(currentIndexChanged(int) ), pviewer3D, SLOT(slot_setPrimitiveType(int)));
    QObject::connect(c_gradient, SIGNAL(currentIndexChanged(int)), pviewer3D, SLOT(slot_setGradient(int)));
    QObject::connect(cb_use_custom_color, SIGNAL(stateChanged(int)), pviewer3D, SLOT(slot_useCustomColor(int)));
    QObject::connect(cw_custom_color, SIGNAL(colorChanged(QColor)), pviewer3D, SLOT(slot_setCustomColor(QColor)));
}

void ConfigurationCloudDialog::apply()
{
    this->pcloud3D->cloud->setName(le_name->text());
}
