#include "mytablemodel.h"

//-----------------------------------------------------------------
MyModel::MyModel(int nbRows, int nbCols, int rowSpan, QObject *parent): QAbstractTableModel(parent)
{
   create(nbRows, nbCols,rowSpan) ;
}
//-----------------------------------------------------------------
void MyModel::create(int nbRows, int nbCols,int rowSpan)
{
    m_data=MatrixXv(nbRows,nbCols);

    h_header=new QHeaderView(Qt::Horizontal);
    v_header=new QHeaderView(Qt::Vertical);
    h_header->setAccessibleName("Variables");
    h_header->setSectionsMovable(true);
    v_header->setSectionsMovable(true);
    h_header->setHighlightSections(true);
    v_header->setHighlightSections(true);
    h_header->setVisible(true);
    v_header->setVisible(true);

    connect(h_header,&QHeaderView::sectionDoubleClicked,this,&MyModel::slot_editColumn);
    connect(h_header,&QHeaderView::sectionMoved        ,this,&MyModel::slot_hSectionMoved);
    connect(v_header,&QHeaderView::sectionMoved        ,this,&MyModel::slot_vSectionMoved);

    for(int i=0;i<nbCols;i++)
    {
        QString value = QString("C%1").arg(i+1);
        reg.newVariable(value,"");
    }

    setRowSpan(rowSpan);
    setRowOffset(0);
}

bool MyModel::open(QString filename)
{
    bool ok=true;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        reg.clear();

        QStringList content=QString(file.readAll()).split("\n",QString::SkipEmptyParts);

        if(content.size()>0)
        {

            //Parse Header-----------------------------------
            int headerSize=0;

            std::cout<<content[0].toLocal8Bit().data()<<std::endl;

            if(content[0]==QString("<header>"))
            {
                if(content[2]==QString("</header>"))
                {
                    QStringList variablesNames=content[1].split(";");
                    for(int i=0;i<variablesNames.size();i++)
                    {
                        reg.newVariable(variablesNames[i],"");
                    }
                    headerSize=3;
                }
                else if(content[3]==QString("</header>"))
                {
                    QStringList variablesNames=content[1].split(";");
                    QStringList variablesExpressions=content[2].split(";");

                    if(variablesNames.size()==variablesExpressions.size())
                    {
                        for(int i=0;i<variablesNames.size();i++)
                        {
                            reg.newVariable(variablesNames[i],variablesExpressions[i]);
                        }
                    }
                    else
                    {
                        error("Open",QString("Number of expressions (%1) and number of variables (%2) are different.").arg(variablesExpressions.size()).arg(variablesNames.size()));
                        ok=false;
                    }
                    headerSize=4;
                }
                else
                {
                    error("Open",QString("Bad Header."));
                    ok=false;
                }
            }
            else
            {
                QStringList data=content[0].split(";");
                for(int i=0;i<data.size();i++)
                {
                    reg.newVariable(QString("C%1").arg(i),"");
                }
            }

            //Parse Data-----------------------------------
            if(ok)
            {
                int nbCols=reg.size(),nbRows=content.size()-headerSize;
                m_data=MatrixXv(nbRows,nbCols);

                for(int i=headerSize;i<content.size();i++)
                {
                    QStringList valueList=content[i].split(";");

                    if(valueList.size()>=nbCols)
                    {
                        for(int j=0;j<nbCols;j++)
                        {
                            bool isd=false;
                            double datad=valueList[j].toDouble(&isd);
                            if(isd)
                            {
                                m_data(i-headerSize,j).val = datad;
                            }
                            else
                            {
                                m_data(i-headerSize,j).str = valueList[j];
                            }
                            m_data(i-headerSize,j).isDouble=isd;
                        }
                    }
                    else
                    {
                        error("Open",QString("Bad data size at line : %1").arg(i));
                        ok=false;
                        break;
                    }
                }
            }

        }
        else
        {
            error("Open",QString("Empty file."));
            ok=false;
        }
        file.close();
    }
    else
    {
        error("Open",QString("Unable to load file : %1").arg(filename));
        ok=false;
    }

    emit layoutChanged();

    return ok;
}

const MatrixXv & MyModel::tableData()
{
    return m_data;
}

QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
             return QVariant();

    if (orientation == Qt::Horizontal)
    {
        //Les columnes ont les noms des variables.
        if ( (section>=0) && (section < reg.variablesNames().size()) )
        {
            //return QVariant();
            return reg.variablesNames()[h_header->visualIndex(section)];
        }
        else
        {
            return QVariant();
        }
    }
    else if (orientation == Qt::Vertical)
    {
        //Les numéro des lignes sont toujours dans l'ordre.
        return QString("R%1 ").arg(v_header->visualIndex(section)+1+m_rowOffset);
    }
    else
    {
        return QVariant();
    }
}

void MyModel::slot_editColumn(int logicalIndex)
{
    int visualIndex=h_header->visualIndex( logicalIndex );

    if (reg.editVariableAndExpression(visualIndex))
    {

    }
}

//-----------------------------------------------------------------
void MyModel::slot_vSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    dataMoveRow(m_data,oldVisualIndex+m_rowOffset,newVisualIndex+m_rowOffset);

    std::cout<<"---------------"<<std::endl;
    std::cout<<m_data<<std::endl;
}

//-----------------------------------------------------------------
void MyModel::slot_hSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    reg.moveVariable(oldVisualIndex,newVisualIndex);
    dataMoveColumn(m_data,oldVisualIndex,newVisualIndex);

    std::cout<<"---------------"<<std::endl;
    std::cout<<m_data<<std::endl;

    reg.dispVariables();
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
    int rowRemainder=m_data.rows()-m_rowOffset;
    if( rowRemainder>=m_rowSpan )
    {
        return m_rowSpan ;
    }
    else
    {
        return rowRemainder;
    }
}
//-----------------------------------------------------------------
int MyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_data.cols();
}
//-----------------------------------------------------------------
QVariant MyModel::data(const QModelIndex &index_logical, int role) const
{
    if (role == Qt::DisplayRole)
    {
        QModelIndex index=toVisualIndex(index_logical);

        if (checkIndex(index))
        {
            if(m_data(index.row()+m_rowOffset,index.column()).isDouble)
            {
                return m_data(index.row()+m_rowOffset,index.column()).val;
            }
            else
            {
                return m_data(index.row()+m_rowOffset,index.column()).str;
            }
        }
    }
    return QVariant();
}
//-----------------------------------------------------------------
bool MyModel::setData(const QModelIndex &index_logical, const QVariant &value, int role)
{

    if (role == Qt::EditRole)
    {
        QModelIndex index=toVisualIndex(index_logical);
        if (!checkIndex(index))
        {
            return false;
        }
        bool isd=false;
        double datad=value.toDouble(&isd);
        if(isd)
        {
            m_data(index.row()+m_rowOffset,index.column()).val = datad;
        }
        else
        {
            m_data(index.row()+m_rowOffset,index.column()).str = value.toString();
        }
        m_data(index.row()+m_rowOffset,index.column()).isDouble=isd;

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

int MyModel::getRowOffsetMax()
{
    return (m_data.rows()-1);
}

void MyModel::setRowOffset(int rowOffset)
{
    m_rowOffset=rowOffset;
    if(m_rowOffset<0)m_rowOffset=0;
    if(m_rowOffset>getRowOffsetMax())m_rowOffset=getRowOffsetMax();

    emit layoutChanged();
}

void MyModel::setRowSpan(int rowSpan)
{
    this->m_rowSpan=rowSpan;

    emit layoutChanged();
}

void MyModel::error(QString title,QString msg)
{
    QMessageBox::information(nullptr,QString("Error : ")+title,msg);
    std::cout<<"Error : "<<msg.toLocal8Bit().data()<<std::endl;
}
