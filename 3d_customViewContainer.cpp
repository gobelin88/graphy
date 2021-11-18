#include "3d_customViewContainer.h"

#include "QStackedLayout.h"
#include <iostream>

CustomViewContainer::CustomViewContainer(QWidget* container,QWidget*parent, Qt::WindowFlags flags)
    :QWidget(parent,flags)
{
    container->setParent(this);

    axisSize=100;
    createColorAxisPlot();
    createXAxisPlot();
    createYAxisPlot();
    createZAxisPlot();    
    axisList<<axisX<<axisY<<axisZ<<scale->axis();

    selectionView=new QListWidget(this);
    selectionView->setSelectionMode(QListWidget::ExtendedSelection);
    selectionView->setMaximumWidth(200);
    selectionView->setStyleSheet("QListWidget { border: none; }");

    glayout=new QGridLayout(this);
    glayout->addWidget(axisX_plot,2,1);
    glayout->addWidget(axisY_plot,1,0);
    glayout->addWidget(axisZ_plot,0,1);
    glayout->addWidget(container,1,1);
    glayout->addWidget(color_plot,1,2);
    glayout->addWidget(selectionView,0,3,3,1);

//    QStackedLayout * stackedLayout=new QStackedLayout(this);
//    stackedLayout->addWidget(axisX_plot);
//    stackedLayout->addWidget(selectionView);
//    stackedLayout->setStackingMode(QStackedLayout::StackAll);
//    this->setLayout(stackedLayout);
//    stackedLayout->setCurrentIndex(1);

    //white
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    this->container=container;

    connect(selectionView,&QListWidget::itemDoubleClicked,this,&CustomViewContainer::slot_onItemDoubleClicked);
    connect(axisX_plot,&QCustomPlot::axisDoubleClick,this,&CustomViewContainer::slot_onAxisDoubleClicked);
    connect(axisY_plot,&QCustomPlot::axisDoubleClick,this,&CustomViewContainer::slot_onAxisDoubleClicked);
    connect(axisZ_plot,&QCustomPlot::axisDoubleClick,this,&CustomViewContainer::slot_onAxisDoubleClicked);
    connect(color_plot,&QCustomPlot::axisDoubleClick,this,&CustomViewContainer::slot_onAxisDoubleClicked);
}

void CustomViewContainer::slot_onItemDoubleClicked(QListWidgetItem *item)
{
    if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
    {        
        emit sig_itemDoubleClicked(selectionView->row(item));
    }
}

void CustomViewContainer::slot_onAxisDoubleClicked(QCPAxis* axis)
{
    if (axis) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
    {
        emit sig_axisDoubleClicked(axisList.indexOf(axis));
    }
}

CustomViewContainer::~CustomViewContainer()
{
    std::cout<<"Delete CustomViewContainer"<<std::endl;
    delete container;
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

    axisX_plot->setBackground(Qt::GlobalColor::transparent);
    axisX_plot->setAttribute(Qt::WA_OpaquePaintEvent, false);

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

    axisX->setUpperEnding(QCPLineEnding::esFlatArrow);
    axisX->setOffset(0);

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

    axisY->setUpperEnding(QCPLineEnding::esFlatArrow);
    axisY->setOffset(0);

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

    axisZ->setUpperEnding(QCPLineEnding::esFlatArrow);
    axisZ->setOffset(0);

    axisZ_plot->setMaximumHeight(axisSize);
}

QWidget* CustomViewContainer::getContainer()
{
    return container;
}

QCPColorScale* CustomViewContainer::getColorScale()
{
    return scale;
}
QCustomPlot* CustomViewContainer::getColorScalePlot()
{
    return color_plot;
}

QCPAxis* CustomViewContainer::getXAxis()
{
    return axisX;
}
QCPAxis* CustomViewContainer::getYAxis()
{
    return axisY;
}
QCPAxis* CustomViewContainer::getZAxis()
{
    return axisZ;
}
QCPAxis* CustomViewContainer::getAxis(int axisIndex)
{
    if(axisIndex<axisList.size() && axisIndex>=0)
    {
        return axisList[axisIndex];
    }
    return nullptr;
}

QVector3D CustomViewContainer::getTranslation()
{
    return QVector3D(float(axisX->range().center()),
                     float(axisY->range().center()),
                     float(axisZ->range().center()));
}

QVector3D CustomViewContainer::getScale()
{
    return QVector3D(float(0.5*(axisX->range().upper-axisX->range().lower)),
                     float(0.5*(axisY->range().upper-axisY->range().lower)),
                     float(0.5*(axisZ->range().upper-axisZ->range().lower)));
}

QVector3D CustomViewContainer::getScaleInv()
{
    return QVector3D(1.0f/float(0.5*(axisX->range().upper-axisX->range().lower)),
                     1.0f/float(0.5*(axisY->range().upper-axisY->range().lower)),
                     1.0f/float(0.5*(axisZ->range().upper-axisZ->range().lower)));
}

QListWidget *CustomViewContainer::getSelectionView()
{
    return selectionView;
}

void CustomViewContainer::replot()
{
    axisX_plot->replot();
    axisY_plot->replot();
    axisZ_plot->replot();
    color_plot->replot();
}

void CustomViewContainer::slot_fullscreen(bool checked)
{
    std::cout<<"slot_fullscreen :"<<checked<<std::endl;
    if(checked)
    {
        container->setParent(nullptr);
        container->showFullScreen();
    }
    else
    {
        glayout->addWidget(container,1,1);
        container->showNormal();
    }
}
