#include "2d_viewer.h"

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
    colorMap->setGradient(QCPColorGradient::gpPolar);

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
}

Viewer2D::~Viewer2D()
{

}

QVector<double> Viewer2D::extract(std::vector<QVector<double>> v,int id)
{
    QVector<double> v_out(v.size());
    for(unsigned int i=0;i<v.size();i++)
    {
        v_out[i]=v[i][id];
    }
    return v_out;
}

void Viewer2D::createPopup()
{
    popup_menu=new QMenu(this);

    actSave   = new QAction("Save",  this);
    actSave->setShortcut(QKeySequence("S"));
    actRescale= new QAction("Rescale",  this);
    actRescale->setShortcut(QKeySequence("R"));

    this->addAction(actSave);
    this->addAction(actRescale);

    popup_menu->addAction(actRescale);
    popup_menu->addAction(actSave);
    connect(actSave,SIGNAL(triggered()),this,SLOT(slot_save_image()));
    connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_rescale()));
}

//void Viewer2D::mousePressEvent(QMouseEvent * event)
//{
//    if(event->button() == Qt::RightButton)
//    {
//        popup_menu->exec(mapToGlobal(event->pos()));
//    }
//}

void Viewer2D::setGradient(int preset)
{
    colorMap->setGradient((QCPColorGradient::GradientPreset)preset);
    this->replot();
}

void Viewer2D::slot_rescale()
{
    resetRange();
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

void Viewer2D::slot_set_data(const TableData & data,const BoxPlot & box,int knn)
{
    std::cout<<box.pX_res<<" "<<box.pY_res<<" "<<
               getRange(data,box.idX).lower<<" "<<
               getRange(data,box.idX).upper<<" "<<
               getRange(data,box.idY).lower<<" "<<
               getRange(data,box.idY).upper<<std::endl;

    colorMap->data()->clear();
    colorMap->data()->setSize(box.pX_res,box.pY_res);
    setRange(getRange(data,box.idX),getRange(data,box.idY));

    interpolate(data,box,colorMap,knn);

    resetRange();
}

void Viewer2D::slot_save_image()
{
    QFileInfo info(current_filename);
    QString where=info.path();

    QString filename=QFileDialog::getSaveFileName(this,"Save Image",where,"(*.png)");

    if(!filename.isEmpty())
    {
        this->current_filename=filename;

        this->savePng(current_filename,512,512,1);
    }
}
