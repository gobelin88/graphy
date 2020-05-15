#include "3d_viewer.h"

ViewerScatter3D::ViewerScatter3D()
{
    QtDataVisualization::Q3DScatter* graph = new QtDataVisualization::Q3DScatter();
    QWidget* container = QWidget::createWindowContainer(graph);

    container->setMinimumSize(QSize(512,512));

    this->setLocale(QLocale("C"));

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    QVBoxLayout* vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    scatterSize=new QDoubleSpinBox();
    scatterSize->setRange(0,1e3);
    scatterSize->setSingleStep(0.01);
    scatterSize->setPrefix("Size=");

    QComboBox* themeList = new QComboBox(this);
    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));
    themeList->setCurrentIndex(4);

    QCheckBox* smoothCheckBox = new QCheckBox(this);
    smoothCheckBox->setText(QStringLiteral("Smooth dots"));
    smoothCheckBox->setChecked(true);

    QComboBox* itemStyleList = new QComboBox(this);
    itemStyleList->addItem(QStringLiteral("Sphere"),      int(QAbstract3DSeries::MeshSphere));
    itemStyleList->addItem(QStringLiteral("Cube"),        int(QAbstract3DSeries::MeshCube));
    itemStyleList->addItem(QStringLiteral("Minimal"),     int(QAbstract3DSeries::MeshMinimal));
    itemStyleList->addItem(QStringLiteral("Point"),       int(QAbstract3DSeries::MeshPoint));
    itemStyleList->addItem(QStringLiteral("Arrow"),       int(QAbstract3DSeries::MeshArrow));
    itemStyleList->addItem(QStringLiteral("Bar"),         int(QAbstract3DSeries::MeshBar));
    itemStyleList->addItem(QStringLiteral("Minimal"),     int(QAbstract3DSeries::MeshMinimal));
    itemStyleList->addItem(QStringLiteral("User-Defined"),int(QAbstract3DSeries::MeshUserDefined));
    itemStyleList->setCurrentIndex(0);

    QPushButton* cameraButton = new QPushButton(this);
    cameraButton->setText(QStringLiteral("Change camera preset"));

    QPushButton* loadMesh = new QPushButton(this);
    loadMesh->setText(QStringLiteral("Load custom Mesh"));

    QCheckBox* backgroundCheckBox = new QCheckBox(this);
    backgroundCheckBox->setText(QStringLiteral("Show background"));
    backgroundCheckBox->setChecked(true);

    QCheckBox* gridCheckBox = new QCheckBox(this);
    gridCheckBox->setText(QStringLiteral("Show grid"));
    gridCheckBox->setChecked(true);

    QComboBox* shadowQuality = new QComboBox(this);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(0);

    QFontComboBox* fontList = new QFontComboBox(this);
    fontList->setCurrentFont(QFont("Arial"));

    sb_xmin=new QDoubleSpinBox(this);
    sb_xmin->setRange(-1e100,1e100);
    sb_xmin->setSingleStep(0.1);
    sb_xmax=new QDoubleSpinBox(this);
    sb_xmax->setRange(-1e100,1e100);
    sb_xmax->setSingleStep(0.1);
    sb_ymin=new QDoubleSpinBox(this);
    sb_ymin->setRange(-1e100,1e100);
    sb_ymin->setSingleStep(0.1);
    sb_ymax=new QDoubleSpinBox(this);
    sb_ymax->setRange(-1e100,1e100);
    sb_ymax->setSingleStep(0.1);
    sb_zmin=new QDoubleSpinBox(this);
    sb_zmin->setRange(-1e100,1e100);
    sb_zmin->setSingleStep(0.1);
    sb_zmax=new QDoubleSpinBox(this);
    sb_zmax->setRange(-1e100,1e100);
    sb_zmax->setSingleStep(0.1);

    QGridLayout* glayout=new QGridLayout(this);
    glayout->addWidget(sb_xmin,0,0);
    glayout->addWidget(sb_xmax,0,1);
    glayout->addWidget(sb_ymin,1,0);
    glayout->addWidget(sb_ymax,1,1);
    glayout->addWidget(sb_zmin,2,0);
    glayout->addWidget(sb_zmax,2,1);

    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
    vLayout->addWidget(backgroundCheckBox);
    vLayout->addWidget(gridCheckBox);
    vLayout->addWidget(smoothCheckBox, 0, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Change dot style")));
    vLayout->addWidget(itemStyleList);
    vLayout->addWidget(new QLabel(QStringLiteral("Change theme")));
    vLayout->addWidget(themeList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout->addWidget(shadowQuality);
    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout->addWidget(fontList, 1, Qt::AlignTop);
    vLayout->addWidget(scatterSize, 1, Qt::AlignTop);
    vLayout->addWidget(loadMesh, 1, Qt::AlignTop);

    vLayout->addLayout(glayout,2);

    modifier = new ScatterDataModifier(graph);

    QObject::connect(cameraButton, &QPushButton::clicked, modifier,&ScatterDataModifier::changePresetCamera);
    QObject::connect(backgroundCheckBox, &QCheckBox::stateChanged, modifier,&ScatterDataModifier::setBackgroundEnabled);
    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,&ScatterDataModifier::setGridEnabled);
    QObject::connect(smoothCheckBox, &QCheckBox::stateChanged, modifier,&ScatterDataModifier::setSmoothDots);
    QObject::connect(modifier, &ScatterDataModifier::backgroundEnabledChanged,backgroundCheckBox, &QCheckBox::setChecked);
    QObject::connect(modifier, &ScatterDataModifier::gridEnabledChanged,gridCheckBox, &QCheckBox::setChecked);
    QObject::connect(itemStyleList, SIGNAL(currentIndexChanged(int)), modifier,SLOT(changeStyle(int)));
    QObject::connect(themeList, SIGNAL(currentIndexChanged(int)), modifier,SLOT(changeTheme(int)));
    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,SLOT(changeShadowQuality(int)));
    QObject::connect(modifier, &ScatterDataModifier::shadowQualityChanged, shadowQuality,&QComboBox::setCurrentIndex);
    QObject::connect(graph, &Q3DScatter::shadowQualityChanged, modifier,&ScatterDataModifier::shadowQualityUpdatedByVisual);
    QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,&ScatterDataModifier::changeFont);
    QObject::connect(modifier, &ScatterDataModifier::fontChanged, fontList,&QFontComboBox::setCurrentFont);
    QObject::connect(scatterSize, SIGNAL(valueChanged(double)), modifier,SLOT(setSize(double)));
    QObject::connect(loadMesh, SIGNAL(clicked()), modifier,SLOT(setUserDefinedMesh()));

    QObject::connect(sb_xmin,SIGNAL(valueChanged(double)), modifier,SLOT(setXMin(double)));
    QObject::connect(sb_xmax,SIGNAL(valueChanged(double)), modifier,SLOT(setXMax(double)));
    QObject::connect(sb_ymin,SIGNAL(valueChanged(double)), modifier,SLOT(setYMin(double)));
    QObject::connect(sb_ymax,SIGNAL(valueChanged(double)), modifier,SLOT(setYMax(double)));
    QObject::connect(sb_zmin,SIGNAL(valueChanged(double)), modifier,SLOT(setZMin(double)));
    QObject::connect(sb_zmax,SIGNAL(valueChanged(double)), modifier,SLOT(setZMax(double)));
}

void ViewerScatter3D::setCloudScalar(const CloudScalar& cloud)
{
    modifier->setData(cloud);

    scatterSize->setValue(modifier->getSize());
    sb_xmin->setValue(double(modifier->getXMin()));
    sb_xmax->setValue(double(modifier->getXMax()));
    sb_ymin->setValue(double(modifier->getYMin()));
    sb_ymax->setValue(double(modifier->getYMax()));
    sb_zmin->setValue(double(modifier->getZMin()));
    sb_zmax->setValue(double(modifier->getZMax()));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ViewerSurface3D::ViewerSurface3D()
{
    QtDataVisualization::Q3DSurface* graph = new QtDataVisualization::Q3DSurface();
    QWidget* container = QWidget::createWindowContainer(graph);

    container->setMinimumSize(QSize(512,512));

    this->setLocale(QLocale("C"));

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    QVBoxLayout* vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    QComboBox* themeList = new QComboBox(this);
    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));
    themeList->setCurrentIndex(4);

    QCheckBox* smoothCheckBox = new QCheckBox(this);
    smoothCheckBox->setText(QStringLiteral("Smooth dots"));
    smoothCheckBox->setChecked(true);

    QComboBox* itemStyleList = new QComboBox(this);
    itemStyleList->addItem(QStringLiteral("Sphere"), int(QAbstract3DSeries::MeshSphere));
    itemStyleList->addItem(QStringLiteral("Cube"), int(QAbstract3DSeries::MeshCube));
    itemStyleList->addItem(QStringLiteral("Minimal"), int(QAbstract3DSeries::MeshMinimal));
    itemStyleList->addItem(QStringLiteral("Point"), int(QAbstract3DSeries::MeshPoint));
    itemStyleList->setCurrentIndex(0);

    QPushButton* cameraButton = new QPushButton(this);
    cameraButton->setText(QStringLiteral("Change camera preset"));

    QCheckBox* backgroundCheckBox = new QCheckBox(this);
    backgroundCheckBox->setText(QStringLiteral("Show background"));
    backgroundCheckBox->setChecked(true);

    QCheckBox* gridCheckBox = new QCheckBox(this);
    gridCheckBox->setText(QStringLiteral("Show grid"));
    gridCheckBox->setChecked(true);

    QComboBox* shadowQuality = new QComboBox(this);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(0);

    QFontComboBox* fontList = new QFontComboBox(this);
    fontList->setCurrentFont(QFont("Arial"));

    sb_xmin=new QDoubleSpinBox(this);
    sb_xmin->setRange(-1e100,1e100);
    sb_xmin->setSingleStep(0.1);
    sb_xmax=new QDoubleSpinBox(this);
    sb_xmax->setRange(-1e100,1e100);
    sb_xmax->setSingleStep(0.1);
    sb_ymin=new QDoubleSpinBox(this);
    sb_ymin->setRange(-1e100,1e100);
    sb_ymin->setSingleStep(0.1);
    sb_ymax=new QDoubleSpinBox(this);
    sb_ymax->setRange(-1e100,1e100);
    sb_ymax->setSingleStep(0.1);
    sb_zmin=new QDoubleSpinBox(this);
    sb_zmin->setRange(-1e100,1e100);
    sb_zmin->setSingleStep(0.1);
    sb_zmax=new QDoubleSpinBox(this);
    sb_zmax->setRange(-1e100,1e100);
    sb_zmax->setSingleStep(0.1);

    QGridLayout* glayout=new QGridLayout(this);
    glayout->addWidget(sb_xmin,0,0);
    glayout->addWidget(sb_xmax,0,1);
    glayout->addWidget(sb_ymin,1,0);
    glayout->addWidget(sb_ymax,1,1);
    glayout->addWidget(sb_zmin,2,0);
    glayout->addWidget(sb_zmax,2,1);

    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
    vLayout->addWidget(backgroundCheckBox);
    vLayout->addWidget(gridCheckBox);
    vLayout->addWidget(smoothCheckBox, 0, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Change dot style")));
    vLayout->addWidget(itemStyleList);
    vLayout->addWidget(new QLabel(QStringLiteral("Change theme")));
    vLayout->addWidget(themeList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout->addWidget(shadowQuality);
    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout->addWidget(fontList, 1, Qt::AlignTop);
    vLayout->addLayout(glayout,2);

    modifier = new SurfaceDataModifier(graph);

    QObject::connect(cameraButton, &QPushButton::clicked, modifier,&SurfaceDataModifier::changePresetCamera);
    QObject::connect(backgroundCheckBox, &QCheckBox::stateChanged, modifier,&SurfaceDataModifier::setBackgroundEnabled);
    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,&SurfaceDataModifier::setGridEnabled);
    QObject::connect(smoothCheckBox, &QCheckBox::stateChanged, modifier,&SurfaceDataModifier::setSmoothDots);
    QObject::connect(modifier, &SurfaceDataModifier::backgroundEnabledChanged,backgroundCheckBox, &QCheckBox::setChecked);
    QObject::connect(modifier, &SurfaceDataModifier::gridEnabledChanged,gridCheckBox, &QCheckBox::setChecked);
    QObject::connect(itemStyleList, SIGNAL(currentIndexChanged(int)), modifier,SLOT(changeStyle(int)));
    QObject::connect(themeList, SIGNAL(currentIndexChanged(int)), modifier,SLOT(changeTheme(int)));
    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,SLOT(changeShadowQuality(int)));
    QObject::connect(modifier, &SurfaceDataModifier::shadowQualityChanged, shadowQuality,&QComboBox::setCurrentIndex);
    QObject::connect(graph, &Q3DScatter::shadowQualityChanged, modifier,&SurfaceDataModifier::shadowQualityUpdatedByVisual);
    QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,&SurfaceDataModifier::changeFont);
    QObject::connect(modifier, &SurfaceDataModifier::fontChanged, fontList,&QFontComboBox::setCurrentFont);

    QObject::connect(sb_xmin,SIGNAL(valueChanged(double)), modifier,SLOT(setXMin(double)));
    QObject::connect(sb_xmax,SIGNAL(valueChanged(double)), modifier,SLOT(setXMax(double)));
    QObject::connect(sb_ymin,SIGNAL(valueChanged(double)), modifier,SLOT(setYMin(double)));
    QObject::connect(sb_ymax,SIGNAL(valueChanged(double)), modifier,SLOT(setYMax(double)));
    QObject::connect(sb_zmin,SIGNAL(valueChanged(double)), modifier,SLOT(setZMin(double)));
    QObject::connect(sb_zmax,SIGNAL(valueChanged(double)), modifier,SLOT(setZMax(double)));
}

void ViewerSurface3D::set_data(const CloudScalar& cloud)
{
    modifier->setData(cloud);

    sb_xmin->setValue(double(modifier->getXMin()));
    sb_xmax->setValue(double(modifier->getXMax()));
    sb_ymin->setValue(double(modifier->getYMin()));
    sb_ymax->setValue(double(modifier->getYMax()));
    sb_zmin->setValue(double(modifier->getZMin()));
    sb_zmax->setValue(double(modifier->getZMax()));
}
