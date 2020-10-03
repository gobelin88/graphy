#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>

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
    v_header->setSectionsClickable(true);
    h_header->setSectionsClickable(true);
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
                                m_data(i-headerSize,j).num = datad;
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

bool MyModel::save(QString filename)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString textData;

        textData += "<header>\n";
        for (int j = 0; j < reg.variablesNames().size(); j++)
        {
            textData+=reg.variablesNames()[j];
            textData += ";";
        }
        textData += "\n";
        for (int j = 0; j < reg.variablesExpressions().size(); j++)
        {
            textData+=reg.variablesExpressions()[j];
            textData += ";";
        }
        textData += "\n";
        textData += "</header>\n";

        for (int i = 0; i < m_data.rows(); i++)
        {
            for (int j = 0; j < m_data.cols(); j++)
            {

                textData += m_data(i,j).toString();
                textData += ";";      // for .csv file format
            }
            textData += "\n";             // (optional: for new line segmentation)
        }

        QTextStream out(&file);
        out << textData;

        file.close();
        return true;
    }
    else
    {
        error("Save",QString("Unable to save the file : ")+filename);
        return false;
    }
}

const MatrixXv & MyModel::tableData()
{
    return m_data;
}

ValueContainer & MyModel::at(QModelIndex indexLogical)
{
    QModelIndex indexVisual= toVisualIndex(indexLogical);
    return m_data(indexVisual.row()+m_rowOffset,indexVisual.column());
}

void MyModel::removeLogicalIndexesRows(const QModelIndexList & selectedIndexesRows)
{
     if(selectedIndexesRows.size()==0)
     {
        return;
     }

     std::vector<unsigned int> visualIndexsRows(selectedIndexesRows.size());
     for(int i=0;i<visualIndexsRows.size();i++)
     {
        visualIndexsRows[i]=v_header->visualIndex(selectedIndexesRows[i].row())+m_rowOffset;
     }
     std::sort(visualIndexsRows.begin(),visualIndexsRows.end());

     //remove consecutives rows
     for (int i=0; i<visualIndexsRows.size();)
     {
         int indexa=visualIndexsRows[i];

         int n=1;
         if ((i+n)<visualIndexsRows.size())
         {
             int indexb=visualIndexsRows[i+n];
             while ((i+n)<visualIndexsRows.size() && indexb==indexa+n)
             {
                 n++;
                 if ((i+n)<visualIndexsRows.size())
                 {
                     indexb=visualIndexsRows[i+n];
                 }
             }
         }

         dataRemoveRows(m_data,indexa-i,n);

         i+=n;
     }

     emit layoutChanged();
}

void MyModel::removeLogicalIndexesCols(const QModelIndexList & selectedIndexesCols)
{
    if(selectedIndexesCols.size()==0)
    {
       return;
    }

    std::vector<unsigned int> visualIndexsCols(selectedIndexesCols.size());
    for(int i=0;i<visualIndexsCols.size();i++)
    {
       visualIndexsCols[i]=h_header->visualIndex(selectedIndexesCols[i].column());
    }
    std::sort(visualIndexsCols.begin(),visualIndexsCols.end());

    //remove consecutives columns
    for (int i=0; i<visualIndexsCols.size();)
    {
        int indexa=visualIndexsCols[i];

        int n=1;
        if ((i+n)<visualIndexsCols.size())
        {
            int indexb=visualIndexsCols[i+n];
            while ((i+n)<visualIndexsCols.size() && indexb==indexa+n)
            {
                n++;
                if ((i+n)<visualIndexsCols.size())
                {
                    indexb=visualIndexsCols[i+n];
                }
            }
        }

        for (int k=0; k<n; k++)
        {
            reg.delVariable(reg.variablesNames()[indexa-i]);
        }

        dataRemoveColumns(m_data,indexa-i,n);

        i+=n;
    }

    emit layoutChanged();
}

void MyModel::clearLogicalIndexes(const QModelIndexList & selectedIndexes)
{
    for (int i = 0; i < selectedIndexes.count(); ++i)
    {
        at(selectedIndexes[i])=ValueContainer();
    }
    emit layoutChanged();
}

VectorXv MyModel::eval(int visualIndex)
{
    reg.setActiveCol(visualIndex);

    if (reg.variablesExpressions()[visualIndex].isEmpty())
    {
        return VectorXv(m_data.rows());
    }
    else
    {
        VectorXv colResults(m_data.rows());

        if (reg.compileExpression(visualIndex))
        {
            for (int i=0; i<m_data.rows(); i++)
            {
                reg.setActiveRow(i);
                for (int j=0; j<m_data.cols(); j++)
                {
                    reg.setVariable(j,m_data(i,j).num);
                }

                colResults[i].num=reg.currentCompiledExpressionValue();
                colResults[i].isDouble=true;
            }
        }
        else
        {
            for (int i=0; i<m_data.rows(); i++)
            {
                reg.setActiveRow(i);
                for (int j=0; j<m_data.cols(); j++)
                {
                    reg.setVariable(j,m_data(i,j).num);
                }

                // store a call to a member function and object ptr
                reg.customExpressionParse2(m_data,visualIndex,colResults[i],i);
            }
        }

        return colResults;
    }
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
        if(visualIndex==-1)//new col
        {
            visualIndex=m_data.cols();
            dataAddColumn(m_data,VectorXv(m_data.rows()));
        }
        m_data.col(visualIndex)=eval(visualIndex);
        emit layoutChanged();
    }
}

void MyModel::slot_newRow()
{
    dataAddRows(m_data,1);
    emit layoutChanged();
}

void MyModel::slot_newRows()
{
    bool ok=false;
    int N=QInputDialog::getInt(nullptr,"Number of rows","Number of rows to add",1,1,10000000,1,&ok);

    if (ok)
    {
        dataAddRows(m_data,N);
        emit layoutChanged();
    }
}

void MyModel::slot_updateColumns()
{
    for (int i=0; i<m_data.cols(); i++)
    {
        m_data.col(i)=eval(i);
    }
}

//-----------------------------------------------------------------
void MyModel::slot_vSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    dataMoveRow(m_data,oldVisualIndex+m_rowOffset,newVisualIndex+m_rowOffset);

//    std::cout<<"---------------"<<std::endl;
//    std::cout<<m_data<<std::endl;
}

//-----------------------------------------------------------------
void MyModel::slot_hSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    reg.moveVariable(oldVisualIndex,newVisualIndex);
    dataMoveColumn(m_data,oldVisualIndex,newVisualIndex);

//    std::cout<<"---------------"<<std::endl;
//    std::cout<<m_data<<std::endl;

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
                return m_data(index.row()+m_rowOffset,index.column()).num;
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
            m_data(index.row()+m_rowOffset,index.column()).num = datad;
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
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index) | Qt::ItemIsSelectable ;
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

void MyModel::dataAddRow(MatrixXv& matrix, VectorXv rowToAdd)
{
    unsigned int numRows = matrix.rows()+1;
    unsigned int numCols = matrix.cols();
    matrix.conservativeResize(numRows,numCols);
    matrix.row(numRows-1)=rowToAdd;
}

void MyModel::dataAddRows(MatrixXv& matrix, int n)
{
    unsigned int numRows = matrix.rows()+n;
    unsigned int numCols = matrix.cols();
    matrix.conservativeResize(numRows,numCols);
    matrix.block(numRows-n,0,n,numCols);
}

void MyModel::dataAddColumn(MatrixXv& matrix, VectorXv colToAdd)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols()+1;
    matrix.conservativeResize(numRows,numCols);
    matrix.col(numCols-1)=colToAdd;
}

void MyModel::dataRemoveRows(MatrixXv& matrix, unsigned int rowToRemove, unsigned int nbRow)
{
    unsigned int numRows = matrix.rows()-nbRow;
    unsigned int numCols = matrix.cols();

    if ( rowToRemove < numRows )
    {
        matrix.block(rowToRemove,0,numRows-rowToRemove,numCols) = matrix.block(rowToRemove+nbRow,0,numRows-rowToRemove,numCols);
    }
    matrix.conservativeResize(numRows,numCols);
}

void MyModel::dataRemoveColumns(MatrixXv& matrix, unsigned int colToRemove,unsigned int nbCol)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols()-nbCol;

    if ( colToRemove < numCols )
    {
        matrix.block(0,colToRemove,numRows,numCols-colToRemove) = matrix.block(0,colToRemove+nbCol,numRows,numCols-colToRemove);
    }
    matrix.conservativeResize(numRows,numCols);
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
