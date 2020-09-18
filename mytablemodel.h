#include <QAbstractItemModel>
#include <QModelIndex>
#include <Eigen/Dense>
#include <iostream>

#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

using namespace Eigen;

using MatrixXs=Matrix<QString,Eigen::Dynamic,Eigen::Dynamic>;
using MatrixXb=Matrix<bool,Eigen::Dynamic,Eigen::Dynamic>;

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(int nbRows,int nbCols,QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    //
    void create(int nbRows, int nbCols);

private:
    MatrixXd m_dataDouble;      //holds double
    MatrixXs m_dataString;      //holds text
    MatrixXb m_isDataString;    //isText
};

#endif // MYTABLEMODEL_H
