#include <QVector>
#include "qcustomplot.h"
#include <boxplot.h>
#include <iostream>
#include <Eigen/Dense>

#include "kdtree_eigen.h"

#ifndef TABLEDATA_H
#define TABLEDATA_H

typedef QVector< QVector<double> > TableData;

void create(TableData & data, uint nbC, uint nbL);

double getNearest(const TableData & data, const BoxPlot & box, Eigen::Vector2d p);

void interpolate(const TableData & data,const BoxPlot & box,QCPColorMap * map,size_t knn);
double getMin(const TableData & data,int id);
double getMax(const TableData & data,int id);
QCPRange getRange(const TableData & data,int id);

#endif // TABLEDATA_H
