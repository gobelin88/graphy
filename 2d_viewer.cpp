#include "2d_viewer.h"
#include "kdtree_eigen.h"

Viewer2D::Viewer2D()
{
    createPopup();

    this->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    this->axisRect()->setupFullAxesBox(true);

    colorMap = new QCPColorMap(this->xAxis, this->yAxis);
    colorScale = new QCPColorScale(this);

    this->plotLayout()->addElement(0, 1, colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorMap->setColorScale(colorScale);
    slot_setGradient(QCPColorGradient::gpPolar);

    marginGroup = new QCPMarginGroup(this);
    this->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    this->addGraph();
    QPen pen_model(QColor(255,1,0));
    pen_model.setStyle(Qt::SolidLine);
    this->graph()->setPen(pen_model);
    this->graph()->setLineStyle(QCPGraph::lsLine);
    this->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 10));
    this->rescaleAxes();
    this->legend->setFont(QFont("Helvetica", 9));

    std::cout<<this->xAxis->range().lower<<std::endl;
    std::cout<<this->xAxis->range().upper<<std::endl;

    //default param
    mode=MODE_NEAREST;
    knn=5;
}

Viewer2D::~Viewer2D()
{

}

QVector<double> Viewer2D::extract(std::vector<QVector<double>> v,int id)
{
    QVector<double> v_out(static_cast<int>(v.size()));
    for (unsigned int i=0; i<v.size(); i++)
    {
        v_out[static_cast<int>(i)]=v[i][id];
    }
    return v_out;
}

void Viewer2D::createPopup()
{
    popup_menu=new QMenu(this);

    actSave   = new QAction("Save",  this);
    actSave->setShortcut(QKeySequence("Ctrl+S"));
    actRescale= new QAction("Rescale",  this);
    actRescale->setShortcut(QKeySequence("R"));
    actParameters= new QAction("Parameters",  this);
    actParameters->setShortcut(QKeySequence("!"));

    this->addAction(actSave);
    this->addAction(actParameters);
    this->addAction(actRescale);

    popup_menu->addAction(actSave);
    popup_menu->addAction(actRescale);
    popup_menu->addAction(actParameters);

    connect(actSave,SIGNAL(triggered()),this,SLOT(slot_saveImage()));
    connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_rescale()));
    connect(actParameters,SIGNAL(triggered()),this,SLOT(slot_setParameters()));
}

void Viewer2D::mousePressEvent(QMouseEvent* event)
{
    QCustomPlot::mousePressEvent(event);
    if (event->button() == Qt::RightButton)
    {
        popup_menu->exec(mapToGlobal(event->pos()));
    }
}

void Viewer2D::slot_setGradient(int preset)
{
    colorMap->setGradient(static_cast<QCPColorGradient::GradientPreset>(preset));
    this->replot();
    this->currentgradient=static_cast<QCPColorGradient::GradientPreset>(preset);
}

void Viewer2D::slot_rescale()
{
    resetRange();
}

void Viewer2D::slot_setParameters()
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Style Parameters");
    QGridLayout* gbox = new QGridLayout();

    QComboBox* c_mode=new QComboBox(dialog);
    c_mode->addItem("NEAREST",MODE_NEAREST);
    c_mode->addItem("WEIGHTED",MODE_WEIGHTED);
    c_mode->setCurrentIndex(mode);

    QSpinBox* sb_knn=new QSpinBox(dialog);
    sb_knn->setRange(1,10000);
    sb_knn->setValue(knn);

    MyGradientComboBox * c_gradient=new MyGradientComboBox(dialog);
    c_gradient->setCurrentIndex(static_cast<int>(currentgradient));

    QObject::connect(sb_knn, SIGNAL(valueChanged(int)), this, SLOT(slot_setKnn(int)));
    QObject::connect(c_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setMode(int)));
    QObject::connect(c_gradient, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setGradient(int)));


    gbox->addWidget(c_mode,0,0);
    gbox->addWidget(sb_knn,0,1);
    gbox->addWidget(c_gradient,1,0,1,2);

    dialog->setLayout(gbox);

    dialog->exec();
}

void Viewer2D::slot_setKnn(int value)
{
    this->knn=value;
    slot_updateData();
}

void Viewer2D::slot_setMode(int mode)
{
    this->mode=static_cast<InterpolationMode>(mode);
    slot_updateData();
}

void Viewer2D::setRange(QCPRange range_x, QCPRange range_y)
{
    colorMap->data()->setRange(range_x,range_y);
    this->xAxis->setRange(range_x);
    this->yAxis->setRange(range_y);
}

void Viewer2D::resetRange()
{
    this->xAxis->setRange(colorMap->data()->keyRange());
    this->yAxis->setRange(colorMap->data()->valueRange());

    colorScale->rescaleDataRange(true);
    colorMap->rescaleDataRange();
    this->rescaleAxes();
    this->replot();
}

void Viewer2D::slot_setData(const Eigen::VectorXd & dataX,
                            const Eigen::VectorXd & dataY,
                            const Eigen::VectorXd & dataZ,
                            const Eigen::Vector2d & box)
{
    this->dataX=dataX;
    this->dataY=dataY;
    this->dataZ=dataZ;
    this->box=box;
    slot_updateData();
}

void Viewer2D::interpolate(const Eigen::VectorXd& dataX,
                 const Eigen::VectorXd& dataY,
                 const Eigen::VectorXd& dataZ,
                 const Eigen::Vector2d &box,
                 QCPColorMap* map,
                 size_t knn,
                 InterpolationMode mode)
{
    Eigen::MatrixXd datapoints(2,dataX.size());

    for (int i=0; i<datapoints.cols(); i++)
    {
        datapoints(0,i)=dataX[i];
        datapoints(1,i)=dataY[i];
    }

    //std::cout<<"data[box.idX].size()="<<datapoints.cols()<<std::endl;
    //std::cout<<datapoints.transpose()<<std::endl;

    kdt::KDTreed kdtree(datapoints);
    kdtree.build();

    kdt::KDTreed::Matrix dists; // basically Eigen::MatrixXd
    kdt::KDTreed::MatrixI idx; // basically Eigen::Matrix<Eigen::Index>

    for (uint i=0; i<box[0]; i++)
    {
        for (uint j=0; j<box[1]; j++)
        {
            kdt::KDTreed::Matrix queryPoints(2,1);
            map->data()->cellToCoord(int(i),int(j),&queryPoints(0,0),&queryPoints(1,0));

            double value=0;
            if (mode==MODE_WEIGHTED)
            {
                kdtree.query(queryPoints, knn, idx, dists);
                double weight_sum=0;
                for (int k=0; k<knn; k++)
                {
                    if (idx(k,0)>=0)
                    {
                        value+=(1.0/dists(k,0))* dataZ[idx(k,0)];
                        weight_sum+=(1.0/dists(k,0));
                    }
                }
                value/=weight_sum;
            }
            else if (mode==MODE_NEAREST)
            {
                kdtree.query(queryPoints, 1, idx, dists);
                if (idx(0,0)>=0)
                {
                    value=dataZ[idx(0,0)];
                }
            }

            map->data()->setCell(i,j,value);
        }
    }
}

void Viewer2D::slot_updateData()
{
    colorMap->data()->clear();
    colorMap->data()->setSize(box[0],box[1]);
    setRange(QCPRange(dataX.minCoeff(),dataX.maxCoeff()),QCPRange(dataY.minCoeff(),dataY.maxCoeff()));
    interpolate(dataX,dataY,dataZ,box,colorMap,knn,mode);
    resetRange();
}

void Viewer2D::slot_saveImage()
{
    QFileInfo info(current_filename);
    QString where=info.path();

    QString filename=QFileDialog::getSaveFileName(this,"Save Image",where,"(*.png)");

    if (!filename.isEmpty())
    {
        this->current_filename=filename;

        this->savePng(current_filename,0,0,1);
    }
}
