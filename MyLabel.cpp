#include "MyLabel.h"

MyLabel::MyLabel(QCustomPlot* parentPlot,double cx, double cy)
{
    this->parentPlot=parentPlot;
    double mx=parentPlot->xAxis->range().lower;
    double my=parentPlot->yAxis->range().lower;

    QPen linePen(QColor(200,200,200));

    lineX= new QCPItemLine(parentPlot);
    lineX->start->setType(QCPItemPosition::ptPlotCoords);
    lineX->end->setType(QCPItemPosition::ptPlotCoords);
    lineX->setPen(linePen);

    lineY= new QCPItemLine(parentPlot);
    lineY->start->setType(QCPItemPosition::ptPlotCoords);
    lineY->end->setType(QCPItemPosition::ptPlotCoords);
    lineY->setPen(linePen);

    // add the text label at the top:
    coordtextX = new QCPItemText(parentPlot);
    coordtextX->position->setType(QCPItemPosition::ptPlotCoords);
    coordtextX->setPositionAlignment(Qt::AlignLeft);
    coordtextX->setRotation(-90);
    coordtextX->setTextAlignment(Qt::AlignLeft);
    coordtextX->setFont(QFont(parentPlot->font().family(), 9));
    coordtextX->setPadding(QMargins(8, 0, 0, 0));

    coordtextY = new QCPItemText(parentPlot);
    coordtextY->position->setType(QCPItemPosition::ptPlotCoords);
    coordtextY->setPositionAlignment(Qt::AlignLeft|Qt::AlignBottom);
    coordtextY->setTextAlignment(Qt::AlignLeft);
    coordtextY->setFont(QFont(parentPlot->font().family(), 9));
    coordtextY->setPadding(QMargins(8, 0, 0, 0));

    setCoord(cx,cy);
    ccx=cx;
    parentPlot->replot();
}

void MyLabel::setCoord(double cx, double cy)
{
    double mx=parentPlot->xAxis->range().lower;
    double my=parentPlot->yAxis->range().lower;

    lineX->start->setCoords(cx, my);
    lineX->end->setCoords(cx, cy);

    lineY->start->setCoords(mx, cy);
    lineY->end->setCoords(cx, cy);

    coordtextY->position->setCoords(mx, cy); // lower right corner of axis rect
    coordtextY->setText(QString("%1").arg(cy,0,'g',3));

    coordtextX->position->setCoords(cx, my); // lower right corner of axis rect
    coordtextX->setText(QString("%1").arg(cx,0,'g',3));
}
