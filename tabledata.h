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

void removeRows(Eigen::MatrixXd& matrix, unsigned int rowToRemove,unsigned int nbRow=1);
void removeColumns(Eigen::MatrixXd& matrix, unsigned int colToRemove, unsigned int nbCol=1);
void addRow(Eigen::MatrixXd& matrix, Eigen::VectorXd rowToAdd);
void addRows(Eigen::MatrixXd& matrix, int n);
void addColumn(Eigen::MatrixXd& matrix, Eigen::VectorXd colToAdd);
void swapColumns(Eigen::MatrixXd& matrix,int ida,int idb);
void moveColumn(Eigen::MatrixXd& matrix,int ida,int idb);
void swapRows(Eigen::MatrixXd& matrix,int ida,int idb);
void moveRow(Eigen::MatrixXd& matrix,int ida,int idb);

void interpolate(const Eigen::MatrixXd& data,const BoxPlot& box,QCPColorMap* map,size_t knn,InterpolationMode mode);
double getMin(const Eigen::MatrixXd& data,int id);
double getMax(const Eigen::MatrixXd& data,int id);
QCPRange getRange(const Eigen::MatrixXd& data,int id);

std::vector<double> toStdVector(const Eigen::VectorXd& v);
QVector<double> toQVector(const Eigen::VectorXd& v);
QVector<QString> toQVectorStr(const Eigen::VectorXd& v);

Eigen::VectorXd fromStdVector(const std::vector<double>& v_std);
Eigen::VectorXd fromQVector(const QVector<double>& v);

QString toString(const Eigen::MatrixXd& v);

#endif // TABLEDATA_H
