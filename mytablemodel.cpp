#include "mytablemodel.h"

//-----------------------------------------------------------------
MyModel::MyModel(int nbRows, int nbCols, QObject *parent): QAbstractTableModel(parent)
{
   create(nbRows, nbCols) ;
}
//-----------------------------------------------------------------
void MyModel::create(int nbRows, int nbCols)
{
    m_data=MatrixXv(nbRows,nbCols);

    h_header=new QHeaderView(Qt::Horizontal);
    v_header=new QHeaderView(Qt::Vertical);
    h_header->setSectionsMovable(true);
    v_header->setSectionsMovable(true);

    //connect(h_header,&QHeaderView::sectionDoubleClicked,this,&MainWindow::slot_sectionDoubleClicked);
    connect(h_header,SIGNAL(sectionMoved(int,int,int)),this,SLOT(slot_hSectionMoved(int,int,int)));
    connect(v_header,SIGNAL(sectionMoved(int,int,int)),this,SLOT(slot_vSectionMoved(int,int,int)));
}
//-----------------------------------------------------------------
void MyModel::slot_vSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    dataMoveRow(m_data,oldVisualIndex,newVisualIndex);

    std::cout<<"---------------"<<std::endl;
    std::cout<<m_data<<std::endl;
}

//-----------------------------------------------------------------
void MyModel::slot_hSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    //reg.moveVariable(oldVisualIndex,newVisualIndex);
    dataMoveColumn(m_data,oldVisualIndex,newVisualIndex);

    std::cout<<"---------------"<<std::endl;
    std::cout<<m_data<<std::endl;

    //reg.dispVariables();
}
//-----------------------------------------------------------------
QHeaderView * MyModel::horizontalHeader()
{
    return h_header;
}
//-----------------------------------------------------------------
QHeaderView * MyModel::verticalHeader()
{
    return v_header;
}
//-----------------------------------------------------------------
int MyModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_data.rows();
}
//-----------------------------------------------------------------
int MyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_data.cols();
}
//-----------------------------------------------------------------
QVariant MyModel::data(const QModelIndex &index_logical, int role) const
{
    std::cout<<"data"<<std::endl;
    QModelIndex index=toVisualIndex(index_logical);

    if (role == Qt::DisplayRole && checkIndex(index))
    {
        if(m_data(index.row(),index.column()).isDouble)
        {
            return m_data(index.row(),index.column()).val;
        }
        else
        {
            return m_data(index.row(),index.column()).str;
        }
    }
    return QVariant();
}
//-----------------------------------------------------------------
bool MyModel::setData(const QModelIndex &index_logical, const QVariant &value, int role)
{
    std::cout<<"setData"<<std::endl;
    QModelIndex index=toVisualIndex(index_logical);

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
            m_data(index.row(),index.column()).val = datad;
        }
        else
        {
            m_data(index.row(),index.column()).str = value.toString();
        }
        m_data(index.row(),index.column()).isDouble=isd;

        std::cout<<"---------------"<<std::endl;
        std::cout<<m_data<<std::endl;

        return true;
    }
    return false;
}
//-----------------------------------------------------------------
Qt::ItemFlags MyModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

//-----------------------------------------------------------------
void MyModel::dataSwapColumns(MatrixXv &matrix, int ida, int idb)
{
    matrix.col(ida).swap(matrix.col(idb));
}

//-----------------------------------------------------------------
void MyModel::dataMoveColumn(MatrixXv & matrix,int ida,int idb)
{
    if (ida<idb)
    {
        for (int k=ida; k<idb; k++)
        {
            dataSwapColumns(matrix,k,k+1);
        }
    }
    else if (ida>idb)
    {
        for (int k=ida; k>idb; k--)
        {
            dataSwapColumns(matrix,k,k-1);
        }
    }
}

//-----------------------------------------------------------------
void MyModel::dataSwapRows(MatrixXv &matrix, int ida, int idb)
{
    matrix.row(ida).swap(matrix.row(idb));
}

//-----------------------------------------------------------------
void MyModel::dataMoveRow(MatrixXv & matrix,int ida,int idb)
{
    if (ida<idb)
    {
        for (int k=ida; k<idb; k++)
        {
            dataSwapRows(matrix,k,k+1);
        }
    }
    else if (ida>idb)
    {
        for (int k=ida; k>idb; k--)
        {
            dataSwapRows(matrix,k,k-1);
        }
    }
}

QModelIndex MyModel::toVisualIndex(const QModelIndex &index)const
{
    return createIndex(v_header->visualIndex(index.row()),h_header->visualIndex(index.column()),nullptr);
    //return Index2D(v_header->visualIndex(index.row()),h_header->visualIndex(index.column()));
}

QModelIndex MyModel::toLogicalIndex(const QModelIndex &index)const
{
    return createIndex(v_header->logicalIndex(index.row()),h_header->logicalIndex(index.column()),nullptr);
    //return Index2D(v_header->logicalIndex(index.row()),h_header->logicalIndex(index.column()));
}

