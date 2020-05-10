#include <QVector>
#include "qcustomplot.h"
#include <boxplot.h>
#include <iostream>
#include <Eigen/Dense>

#include "kdtree_eigen.h"

#ifndef TABLEDATA_H
#define TABLEDATA_H

enum InterpolationMode
{
    MODE_NEAREST,
    MODE_WEIGHTED
};

typedef QVector< QVector<double> > TableData;

void create(TableData& data, uint nbC, uint nbL);

void interpolate(const TableData& data,const BoxPlot& box,QCPColorMap* map,size_t knn,InterpolationMode mode);
double getMin(const TableData& data,int id);
double getMax(const TableData& data,int id);
QCPRange getRange(const TableData& data,int id);

#endif // TABLEDATA_H
