#include <QVector>
#include <iostream>
#include <Eigen/Dense>
#include "MyCustomPlot.h"

#ifndef TABLEDATA_H
#define TABLEDATA_H

enum InterpolationMode
{
    MODE_NEAREST,
    MODE_WEIGHTED
};

enum SortMode
{
    ASCENDING,
    DECENDING
};

enum ThresholdMode
{
    KEEP_GREATER,
    KEEP_LOWER
};

//Build KdTree in order to interpolate data
void interpolate(const Eigen::VectorXd &dataX, const Eigen::VectorXd &dataY, const Eigen::VectorXd &dataZ, const Eigen::Vector2d & box, QCPColorMap* map, size_t knn, InterpolationMode mode);

std::vector<double> toStdVector(const Eigen::VectorXd& v);
Eigen::VectorXd fromStdVector(const std::vector<double>& v_std);

QVector<double> toQVector(const Eigen::VectorXd& v);
Eigen::VectorXd fromQVector(const QVector<double>& v);

QString toString(const Eigen::MatrixXd& v);

QDataStream & operator<<(QDataStream & ds,const Eigen::VectorXd & v);
QDataStream & operator>>(QDataStream & ds,Eigen::VectorXd & v);
QDataStream & operator<<(QDataStream & ds,const QVector<QString> & v);
QDataStream & operator>>(QDataStream & ds,QVector<QString> & v);

#endif // TABLEDATA_H
