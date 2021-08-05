#ifndef MYLABEL_H
#define MYLABEL_H

#include "MyCustomPlot.h"

class MyLabel
{
public:
    MyLabel(QCustomPlot *parentPlot, double cx, double cy);

    void setCoord(double cx, double cy);

    QCPItemLine* lineX;
    QCPItemLine* lineY;
    QCPItemText* coordtextX;
    QCPItemText* coordtextY;
    QCustomPlot* parentPlot;

    double ccx;
};

#endif // MYLABEL_H
