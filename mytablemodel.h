#include <QAbstractItemModel>
#include <QHeaderView>
#include <QModelIndex>
#include <Eigen/Dense>
#include <QMessageBox>
#include <QTableView>
#include <iostream>

#include "register.h"
#include "value.h"

#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

using namespace Eigen;

using MatrixXv=Matrix<Value,Eigen::Dynamic,Eigen::Dynamic>;

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(int nbRows,int nbCols,int rowSpan,QObject *parent=nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index_logical, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index_logical, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    //
    void create(int nbRows, int nbCols, int rowSpan);
    bool open(QString filename);
    const MatrixXv & tableData();

    QHeaderView * horizontalHeader();
    QHeaderView * verticalHeader();

    int getRowOffset(){return m_rowOffset;}
    int getRowSpan(){return m_rowSpan;}
    void setRowOffset(int rowOffset);
    void setRowSpan(int rowSpan);
    int getRowOffsetMax();

public slots:
    void slot_editColumn(int logicalIndex);
    void slot_vSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex);
    void slot_hSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex);

private:
    int m_rowOffset;
    int m_rowSpan;

    MatrixXv m_data;

    QHeaderView * h_header;
    QHeaderView * v_header;

    //Variables---------------------------------------
    Register reg;

    //
    void dataSwapColumns(MatrixXv &matrix, int ida, int idb);
    void dataMoveColumn(MatrixXv & matrix,int ida,int idb);
    void dataSwapRows(MatrixXv &matrix, int ida, int idb);
    void dataMoveRow(MatrixXv & matrix,int ida,int idb);

    //
    QModelIndex toVisualIndex(const QModelIndex &index) const;
    QModelIndex toLogicalIndex(const QModelIndex &index) const;

    void error(QString title,QString msg);
};

#endif // MYTABLEMODEL_H
