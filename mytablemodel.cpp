#include "mytablemodel.h"

MyModel::MyModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}
//-----------------------------------------------------------------
int MyModel::rowCount(const QModelIndex & /*parent*/) const
{
    return ROWS;
}
//-----------------------------------------------------------------
int MyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return COLS;
}
//-----------------------------------------------------------------
QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && checkIndex(index))
            return m_gridData[index.row()][index.column()];
    return QVariant();
}
//-----------------------------------------------------------------
//! [quoting mymodel_e]
bool MyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        if (!checkIndex(index))
            return false;
        //save value from editor to member m_gridData
        m_gridData[index.row()][index.column()] = value.toString();
        return true;
    }
    return false;
}
//! [quoting mymodel_e]
//-----------------------------------------------------------------
//! [quoting mymodel_f]
Qt::ItemFlags MyModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}
