#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QGridLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDialog>
#include <QDialogButtonBox>

#include "MyTableModel.h"

//-----------------------------------------------------------------
MyModel::MyModel(int nbRows, int nbCols, int rowSpan, QObject *parent): QAbstractTableModel(parent)
{
   create(nbRows, nbCols,rowSpan) ;
}
//-----------------------------------------------------------------
void MyModel::create(int nbRows, int nbCols,int rowSpan)
{

    v_scrollBar=new QScrollBar();
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

    createEmpty(nbRows,nbCols);

    setRowSpan(rowSpan);
    setRowOffset(0);
    v_scrollBar->setRange(0,getRowOffsetMax());

    connect(h_header,&QHeaderView::sectionDoubleClicked,this,&MyModel::slot_editColumn);
    connect(h_header,&QHeaderView::sectionMoved        ,this,&MyModel::slot_hSectionMoved);
    connect(v_header,&QHeaderView::sectionMoved        ,this,&MyModel::slot_vSectionMoved);
    connect(v_scrollBar,&QScrollBar::valueChanged      ,this,&MyModel::setRowOffset);

}

void MyModel::createEmpty(int nbRows, int nbCols)
{
    reg.clear();
    m_data=MatrixXv(nbRows,nbCols);
    for(int i=0;i<nbCols;i++)
    {
        QString value = QString("C%1").arg(i+1);
        reg.newVariable(value,"");
    }

    contentResized();
    emit sig_dataChanged();
}

void MyModel::exportLatex(QString filename)
{
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString textData;

        textData+= QString("\\begin{tabular}{|*{%1}{c|}} \n").arg(m_data.cols());
        textData += "\\hline \n";

        for (int j = 0; j < m_data.cols(); j++)
        {
            textData+="$"+reg.variablesNames()[j]+"$";
            if (j!=m_data.cols()-1)
            {
                textData += "&";
            }
        }
        textData += "\\\\ \\hline \n";

        for (int i = 0; i < m_data.rows(); i++)
        {
            for (int j = 0; j < m_data.cols(); j++)
            {
                textData += m_data(i,j).toString();

                if (j!=m_data.cols()-1)
                {
                    textData += "&";
                }
            }
            textData += "\\\\ \\hline \n";             // (optional: for new line segmentation)
        }

        textData+=QString("\\end{tabular}");

        QTextStream out(&file);
        out << textData;
    }
    else
    {
        error("Export Latex",QString("Impossible d'ouvrir le fichier : ")+filename);
    }
}

bool MyModel::open(QString filename)
{
    bool ok=true;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        reg.clear();

        QStringList content=QString(file.readAll()).split("\n",QString::SkipEmptyParts);

        for(int i=0;i<content.size();)
        {
            if(content[i].startsWith("#"))
            {
                content.removeAt(i);
            }
            else
            {
                i++;
            }
        }

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

    contentResized();

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

QString MyModel::copy(int x0,int y0,int nrows,int ncols)
{
    QString clipboardBuffer;
    Eigen::Matrix<QString,Eigen::Dynamic,Eigen::Dynamic> content(nrows,ncols);

    for (int i = 0; i < nrows; ++i)
    {
        for (int j = 0; j < ncols; ++j)
        {
            content(i,j)=m_data(i+x0,j+y0).toString();
        }
    }

    for(int i=0;i<nrows;i++)
    {
        for(int j=0;j<ncols;j++)
        {
            clipboardBuffer+=content(i,j);
            if(j!=ncols-1)
            {
                    clipboardBuffer+=QString(";");
            }
        }
        clipboardBuffer+="\n";
    }

    return clipboardBuffer;
}

void MyModel::paste(int x0,int y0,QString buffer)
{
    QStringList lines=buffer.split("\n",QString::SkipEmptyParts);
    for(int i=0.0;i<lines.size();i++)
    {
        QStringList valuesToken=lines[i].split(";");
        for(int j=0;j<valuesToken.size();j++)
        {
            int indexRow=i+x0;
            int indexCol=j+y0;

            if(indexRow<m_data.rows() && indexCol<m_data.cols())
            {
                m_data(indexRow,indexCol)=valuesToken[j];
            }
        }
    }

    emit layoutChanged();
    emit sig_dataChanged();
}

MyValueContainer & MyModel::at(QModelIndex indexLogical)
{
    QModelIndex indexVisual= toVisualIndex(indexLogical);
    return m_data(indexVisual.row()+m_rowOffset,indexVisual.column());
}

bool MyModel::asColumnStrings(int idCol)
{
    for(int i=0;i<m_data.rows();i++)
    {
        if(m_data(i,idCol).isDouble==false)
        {
            return true;
        }
    }
    return false;
}

QString MyModel::getLogicalColName(int logicalIndex)
{
    return getVisualColName(h_header->visualIndex(logicalIndex));
}

QString MyModel::getVisualColName(int visualIndex)
{
    return reg.variablesNames()[visualIndex];
}

Eigen::VectorXd MyModel::getColLogicalDataDouble(int logicalIndex)const
{
    return getColVisualDataDouble(h_header->visualIndex(logicalIndex));
}

QVector<QString> MyModel::getColLogicalDataString(int logicalIndex)const
{
    return getColVisualDataString(h_header->visualIndex(logicalIndex));
}

QVector<QString> MyModel::getColVisualDataString(int visualIndex)const
{
    //something better to be done here

    QVector<QString> v(m_data.rows());

    for(int i=0;i<v.size();i++)
    {
        v[i]=m_data(i,visualIndex).str;
    }

    return v;
}

Eigen::VectorXd MyModel::getColVisualDataDouble(int visualIndex)const
{
    //something better to be done here

    Eigen::VectorXd v(m_data.rows());

    for(int i=0;i<v.rows();i++)
    {
        v[i]=m_data(i,visualIndex).num;
    }

    return v;
}

void MyModel::applyFilters(const QModelIndexList & selectedColsIndexes)
{
    if (selectedColsIndexes.size()>0)
    {
        int visualIndex=h_header->visualIndex(selectedColsIndexes[0].column());

        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle(QString("Filter by : %1").arg(reg.variablesNames()[visualIndex]));
        QGridLayout* gbox = new QGridLayout();

        QComboBox* cb_mode=new QComboBox(dialog);
        cb_mode->addItem("Ascending sort");
        cb_mode->addItem("Decending sort");
        cb_mode->addItem("Keep greater than threshold");
        cb_mode->addItem("Keep lower than threshold");

        QDoubleSpinBox* sb_threshold=new QDoubleSpinBox(dialog);
        sb_threshold->setPrefix("Threshold=");
        sb_threshold->setRange(-1e100,1e100);

        double defaultThresholdValue=getColVisualDataDouble(visualIndex).mean();
        if(std::isnan(defaultThresholdValue))
        {
            sb_threshold->setValue(0);
        }
        else
        {
            sb_threshold->setValue(defaultThresholdValue);
        }
        //sb_threshold->setEnabled(false);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));


        gbox->addWidget(cb_mode,0,0);
        gbox->addWidget(sb_threshold,0,1);
        gbox->addWidget(buttonBox,2,0,1,2);

        dialog->setLayout(gbox);
        dialog->setMinimumWidth(300);
        dialog->adjustSize();
        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            if(cb_mode->currentIndex()==0)
            {
                dataSortBy(m_data,visualIndex,MyModel::SortMode::ASCENDING);
            }
            else if(cb_mode->currentIndex()==1)
            {
                dataSortBy(m_data,visualIndex,MyModel::SortMode::DECENDING);
            }
            else if(cb_mode->currentIndex()==2)
            {
                dataThresholdBy(m_data,visualIndex,MyModel::ThresholdMode::KEEP_GREATER,sb_threshold->value());
            }
            else if(cb_mode->currentIndex()==3)
            {
                dataThresholdBy(m_data,visualIndex,MyModel::ThresholdMode::KEEP_LOWER,sb_threshold->value());
            }
        }


        contentResized();
        emit sig_dataChanged();
    }
}

void MyModel::contentResized()
{
    v_scrollBar->setRange(0,getRowOffsetMax());
    emit layoutChanged();
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

     contentResized();
     emit sig_dataChanged();
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

    contentResized();
    emit sig_dataChanged();
}

void MyModel::clearLogicalIndexes(const QModelIndexList & selectedIndexes)
{
    for (int i = 0; i < selectedIndexes.count(); ++i)
    {
        at(selectedIndexes[i])=MyValueContainer();
    }
    emit layoutChanged();
    emit sig_dataChanged();
}

void MyModel::clearLogicalIndexesCols(const QModelIndexList & selectedIndexesCols)
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

    for(int j=0;j<visualIndexsCols.size();j++)
    {
        for (int i = 0; i < m_data.rows(); ++i)
        {
            m_data(i,visualIndexsCols[j])=MyValueContainer();
        }
    }

    emit layoutChanged();
    emit sig_dataChanged();
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
                if(!reg.customExpressionParse2(m_data,visualIndex,colResults[i],i))
                {
                    break;
                }
            }
        }

        return colResults;
    }
}

QVariant MyModel::headerData(int logicalIndex, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
             return QVariant();

    if (orientation == Qt::Horizontal)
    {
        //Les columnes ont les noms des variables.
        if ( (logicalIndex>=0) && (logicalIndex < reg.variablesNames().size()) )
        {
            //return QVariant();
            return reg.variablesNames()[h_header->visualIndex(logicalIndex)];
        }
        else
        {
            return QVariant();
        }
    }
    else if (orientation == Qt::Vertical)
    {
        //Les numéro des lignes sont toujours dans l'ordre.
        return QString("R%1").arg(v_header->visualIndex(logicalIndex)+1+m_rowOffset);
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

        if(!reg.variablesExpressions()[visualIndex].isEmpty())
        {
            m_data.col(visualIndex)=eval(visualIndex);
        }

        emit layoutChanged();
        emit sig_dataChanged();
    }
}

void MyModel::slot_newRow()
{
    dataAddRows(m_data,1);
    contentResized();
}

void MyModel::slot_newRows()
{
    bool ok=false;
    int N=QInputDialog::getInt(nullptr,"Number of rows","Number of rows to add",1,1,10000000,1,&ok);

    if (ok)
    {
        dataAddRows(m_data,N);
        contentResized();
    }
}

void MyModel::slot_updateColumns()
{
    for (int i=0; i<m_data.cols(); i++)
    {
        if(!reg.variablesExpressions()[i].isEmpty())
        {
            m_data.col(i)=eval(i);
        }
    }
    emit layoutChanged();
    emit sig_dataChanged();
}

//-----------------------------------------------------------------
void MyModel::slot_vSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    dataMoveRow(m_data,oldVisualIndex+m_rowOffset,newVisualIndex+m_rowOffset);

//    std::cout<<"---------------"<<std::endl;
//    std::cout<<m_data<<std::endl;
    emit sig_dataChanged();
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
    emit sig_dataChanged();
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
QScrollBar * MyModel::verticalScrollBar()
{
    return v_scrollBar;
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
    if(role==Qt::EditRole)
    {
        QModelIndex index=toVisualIndex(index_logical);
        return m_data(index.row()+m_rowOffset,index.column()).toString();
    }
    else if (role == Qt::DisplayRole)
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
    if (role == Qt::BackgroundRole)
    {
        QModelIndex index=toVisualIndex(index_logical);
        QRgb backgroundColor=m_data(index.row()+m_rowOffset,index.column()).background;
        return QBrush(QColor(backgroundColor));
    }
    if (role == Qt::ForegroundRole)
    {
        QModelIndex index=toVisualIndex(index_logical);
        QRgb backgroundColor=m_data(index.row()+m_rowOffset,index.column()).background;
        if(qGray(backgroundColor)>100)
        {
            return QBrush(QColor(Qt::black));
        }
        else
        {
            return QBrush(QColor(Qt::white));
        }
    }

    return QVariant();
}

void MyModel::colourizeCol(unsigned int visualColIndex,const std::vector<QRgb> &colors)
{
    if(m_data.rows()==colors.size() && visualColIndex<m_data.cols())
    {
        for(int i=0;i<m_data.rows();i++)
        {
            m_data(i,visualColIndex).background=colors[i];
        }
    }
}

void MyModel::colourizeCol(unsigned int visualColIndex,QRgb color)
{
    for(int i=0;i<m_data.rows();i++)
    {
        m_data(i,visualColIndex).background=color;
    }
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

//        std::cout<<"---------------"<<std::endl;
//        std::cout<<m_data<<std::endl;

        emit sig_dataChanged();

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
    if(numRows==colToAdd.rows())
    {
        matrix.conservativeResize(numRows,numCols);
        matrix.col(numCols-1)=colToAdd;
    }
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

void MyModel::dataSortBy(MatrixXv & matrix, int colId,SortMode mode)
{
    std::vector<VectorXv> vec;
    for (int64_t i = 0; i < matrix.rows(); ++i)
        vec.push_back(matrix.row(i));

    if(mode==ASCENDING)
    {
        std::sort(vec.begin(), vec.end(), [&colId](VectorXv const& t1, VectorXv const& t2){ return t1(colId).num < t2(colId).num; } );
    }
    else if(mode==DECENDING)
    {
        std::sort(vec.begin(), vec.end(), [&colId](VectorXv const& t1, VectorXv const& t2){ return t1(colId).num > t2(colId).num; } );
    }

    for (int64_t i = 0; i < matrix.rows(); ++i)
        matrix.row(i) = vec[i];
}

void MyModel::dataThresholdBy(MatrixXv & matrix, int colId,ThresholdMode mode,double thresholdValue)
{
    std::vector<VectorXv> vec;
    for (unsigned int i = 0; i < matrix.rows(); ++i)
    {
        if(mode==KEEP_GREATER)
        {
            if(matrix(i,colId).num>thresholdValue)//KEEP_GREATER
            {
                vec.push_back(matrix.row(i));
            }
        }
        else
        {
            if(matrix(i,colId).num<thresholdValue)//KEEP_LOWER
            {
                vec.push_back(matrix.row(i));
            }
        }
    }

    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        matrix.row(i) = vec[i];
    }

    matrix.conservativeResize(vec.size(),matrix.cols());
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

void MyModel::slot_newColumn(QString varName,Eigen::VectorXd dataCol)
{
    if(dataCol.rows()!=m_data.rows())
    {
        std::cout<<"dataCol.rows()="<<dataCol.rows()<<" m_data.rows()="<<m_data.rows()<<std::endl;
        return;
    }

    if(!reg.existVariable(varName))
    {
        if(reg.newVariable(varName,""))
        {
            VectorXv dataColv(dataCol.rows());

            for(int i=0;i<dataCol.rows();i++)
            {
                dataColv[i].num=dataCol[i];
                dataColv[i].isDouble=true;
            }

            dataAddColumn(m_data,dataColv);


            emit layoutChanged();
            emit sig_dataChanged();
        }
        else
        {
            bool ok;
            QString newVarName=QInputDialog::getText(nullptr,"Variable Name",varName,QLineEdit::Normal,varName,&ok);

            if(ok)
            {
                slot_newColumn(newVarName,dataCol);
            }
        }
    }
    else
    {
        slot_newColumn(varName+QString("_%1").arg(reg.countVariable(varName)),dataCol);
    }
    std::cout<<"slot_newColumn end"<<std::endl;
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
