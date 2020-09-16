#include <QAbstractItemModel>
#include <QModelIndex>

#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

const int COLS= 3;
const int ROWS= 2;

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;



private:
    QString m_gridData[ROWS][COLS];  //holds text entered into QTableView
};

#endif // MYTABLEMODEL_H
