#include <QAbstractItemModel>
#include <QHeaderView>
#include <QModelIndex>
#include <Eigen/Dense>
#include <QScrollBar>
#include <iostream>

#include "Register.h"
#include "MyValueContainer.h"

#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
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

    MyModel(int nbRows,int nbCols,int rowSpan,QObject *parent=nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index_logical, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index_logical, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int logicalIndex, Qt::Orientation orientation, int role) const override;

    //I/O
    void createEmpty(int nbRows, int nbCols);
    bool open(QString filename);
    bool save(QString filename);
    void exportLatex(QString filename);

    //Data
    void colourizeCol(unsigned int visualColIndex, const std::vector<QRgb> &colors);
    void colourizeCol(unsigned int visualColIndex, QRgb color);

    QVector<QString> getColLogicalDataString(int logicalIndex)const;
    QVector<QString> getColVisualDataString(int visualIndex)const;

    QString getLogicalColName(int logicalIndex);
    QString getVisualColName(int visualIndex);
    Eigen::VectorXd getColLogicalDataDouble(int logicalIndex)const;
    Eigen::VectorXd getColVisualDataDouble(int visualIndex)const;
    bool asColumnStrings(int idCol);
    void applyFilters(const QModelIndexList & selectedColsIndexes);
    void removeLogicalIndexesRows(const QModelIndexList & selectedIndexesRows);
    void removeLogicalIndexesCols(const QModelIndexList & selectedIndexesCols);

    void clearLogicalIndexes(const QModelIndexList & selectedIndexes);
    void clearLogicalIndexesCols(const QModelIndexList & selectedIndexesCols);

    const MatrixXv &tableData();
    VectorXv eval(int visualIndex);
    QString copy(int x0,int y0,int nrows,int ncols);
    void paste(int x0,int y0,QString buffer);

    //Gui
    QHeaderView * horizontalHeader();
    QHeaderView * verticalHeader();
    QScrollBar * verticalScrollBar();

    int getRowOffset(){return m_rowOffset;}
    int getRowSpan(){return m_rowSpan;}
    void setRowSpan(int rowSpan);
    int getRowOffsetMax();

    //register
    const Register & getRegister(){return reg;}

    //copy/paste

public slots:
    void setRowOffset(int rowOffset);

    void slot_newColumn(QString varName, Eigen::VectorXd dataCol);

    void slot_editColumn(int logicalIndex);
    void slot_newRow();
    void slot_newRows();
    void slot_updateColumns();

    void slot_vSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex);
    void slot_hSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex);

signals:
    void sig_dataChanged();

private:
    void contentResized();
    void create(int nbRows, int nbCols, int rowSpan);
    MyValueContainer & at(QModelIndex indexLogical);

    int m_rowOffset;
    int m_rowSpan;

    MatrixXv m_data;

    QHeaderView * h_header;
    QHeaderView * v_header;
    QScrollBar * v_scrollBar;


    //Variables---------------------------------------
    Register reg;

    //Data management
    void dataSwapColumns(MatrixXv &matrix, int ida, int idb);
    void dataMoveColumn(MatrixXv & matrix,int ida,int idb);
    void dataSwapRows(MatrixXv &matrix, int ida, int idb);
    void dataMoveRow(MatrixXv & matrix,int ida,int idb);
    void dataAddRow(MatrixXv & matrix, VectorXv rowToAdd);
    void dataAddRows(MatrixXv & matrix, int n);
    void dataAddColumn(MatrixXv & matrix, VectorXv colToAdd);
    void dataRemoveRows(MatrixXv& matrix, unsigned int rowToRemove, unsigned int nbRow);
    void dataRemoveColumns(MatrixXv& matrix, unsigned int colToRemove,unsigned int nbCol);
    void dataSortBy(MatrixXv& matrix, int colId, SortMode mode);
    void dataThresholdBy(MatrixXv & matrix, int colId,ThresholdMode mode,double thresholdValue);

    QModelIndex toVisualIndex(const QModelIndex &index) const;
    QModelIndex toLogicalIndex(const QModelIndex &index) const;

    void error(QString title,QString msg);


};

#endif // MYTABLEMODEL_H
