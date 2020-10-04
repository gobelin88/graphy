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

void removeRows(Eigen::MatrixXd& matrix, unsigned int rowToRemove,unsigned int nbRow=1);
void removeColumns(Eigen::MatrixXd& matrix, unsigned int colToRemove, unsigned int nbCol=1);
void addRow(Eigen::MatrixXd& matrix, Eigen::VectorXd rowToAdd);
void addRows(Eigen::MatrixXd& matrix, int n);
void addColumn(Eigen::MatrixXd& matrix, Eigen::VectorXd colToAdd);

void interpolate(const Eigen::VectorXd &dataX, const Eigen::VectorXd &dataY, const Eigen::VectorXd &dataZ, const Resolution& box, QCPColorMap* map, size_t knn, InterpolationMode mode);
QCPRange getRange(const Eigen::VectorXd &data);

std::vector<double> toStdVector(const Eigen::VectorXd& v);
QVector<double> toQVector(const Eigen::VectorXd& v);
QVector<QString> toQVectorStr(const Eigen::VectorXd& v);

Eigen::VectorXd fromStdVector(const std::vector<double>& v_std);
Eigen::VectorXd fromQVector(const QVector<double>& v);

QString toString(const Eigen::MatrixXd& v);

void sortBy(Eigen::MatrixXd& matrix, int colId,SortMode mode);
void thresholdBy(Eigen::MatrixXd& matrix, int colId, ThresholdMode mode, double value);


template<typename Derived>
void swapColumns(Eigen::MatrixBase<Derived> &matrix, int ida, int idb)
{
    matrix.col(ida).swap(matrix.col(idb));
}

template<typename Derived>
void moveColumn(Eigen::MatrixBase<Derived> & matrix,int ida,int idb)
{
    if (ida<idb)
    {
        for (int k=ida; k<idb; k++)
        {
            swapColumns(matrix,k,k+1);
        }
    }
    else if (ida>idb)
    {
        for (int k=ida; k>idb; k--)
        {
            swapColumns(matrix,k,k-1);
        }
    }
}

template<typename Derived>
void swapRows(Eigen::MatrixBase<Derived> &matrix, int ida, int idb)
{
    matrix.row(ida).swap(matrix.row(idb));
}

template<typename Derived>
void moveRow(Eigen::MatrixBase<Derived> & matrix,int ida,int idb)
{
    if (ida<idb)
    {
        for (int k=ida; k<idb; k++)
        {
            swapRows(matrix,k,k+1);
        }
    }
    else if (ida>idb)
    {
        for (int k=ida; k>idb; k--)
        {
            swapRows(matrix,k,k-1);
        }
    }
}
#endif // TABLEDATA_H
