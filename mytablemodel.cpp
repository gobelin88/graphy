#include "mytablemodel.h"

MyModel::MyModel(int nbRows, int nbCols, QObject *parent): QAbstractTableModel(parent)
{
   create(nbRows, nbCols) ;
}
void MyModel::create(int nbRows, int nbCols)
{
    m_dataDouble=MatrixXd(nbRows,nbCols);
    m_dataString=MatrixXs(nbRows,nbCols);
    m_isDataString=MatrixXb(nbRows,nbCols);

    m_dataDouble.setZero();
    m_dataString.setConstant(QString(""));
    m_isDataString.setConstant(false);
}

//-----------------------------------------------------------------
int MyModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_dataDouble.rows();
}
//-----------------------------------------------------------------
int MyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_dataDouble.cols();
}
//-----------------------------------------------------------------
QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && checkIndex(index))
    {
        if(m_isDataString(index.row(),index.column()))
        {
            return m_dataString(index.row(),index.column());
        }
        else
        {
            return m_dataDouble(index.row(),index.column());
        }
    }
    return QVariant();
}
//-----------------------------------------------------------------
//! [quoting mymodel_e]
bool MyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        if (!checkIndex(index))
        {
            return false;
        }
        bool isd=false;
        double datad=value.toDouble(&isd);
        if(isd)
        {
            m_dataDouble(index.row(),index.column()) = datad;
        }
        else
        {
            m_dataString(index.row(),index.column()) = value.toString();
        }

        std::cout<<"---------------"<<std::endl;
        std::cout<<m_dataDouble<<std::endl;

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
