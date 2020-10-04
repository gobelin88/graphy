#include <QWheelEvent>
#include "mytableview.h"

MyTableView::MyTableView(int nbRow,
                         int nbCols,
                         int rowsSpan,
                         QWidget *parent):QTableView(parent)
{
    createNew(nbRow,nbCols,rowsSpan);

    //this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectColumns);
}

void MyTableView::createNew(int nbRow,int nbCols,int rowsSpan)
{
    m_model = new MyModel(nbRow,nbCols,rowsSpan);
    setHorizontalHeader(m_model->horizontalHeader());
    setVerticalHeader(m_model->verticalHeader());
    setModel(m_model);
}

void MyTableView::wheelEvent(QWheelEvent * event)
{
    if(event->delta()>0)
    {
        m_model->setRowOffset(m_model->getRowOffset()+1);
    }
    else
    {
        m_model->setRowOffset(m_model->getRowOffset()-1);
    }
}

void MyTableView::slot_deleteSelected()
{
    QModelIndexList selectedIndexes=selectionModel()->selectedIndexes();
    m_model->clearLogicalIndexes(selectedIndexes);
}

void MyTableView::slot_removeSelectedRowsAndCols()
{
    QModelIndexList selectedColsIndexes=selectionModel()->selectedColumns();
    QModelIndexList selectedRowsIndexes=selectionModel()->selectedRows();

    m_model->removeLogicalIndexesCols(selectedColsIndexes);
    m_model->removeLogicalIndexesRows(selectedRowsIndexes);
}

void MyTableView::slot_filter()
{
    QModelIndexList selectedColsIndexes=selectionModel()->selectedColumns();
    m_model->applyFilters(selectedColsIndexes);
}

void MyTableView::slot_copy()
{
    QCPRange range_row,range_col;
    getVisualRowColSelectedRanges(range_row,range_col);

    int nrows=range_row.upper-range_row.lower+1;
    int ncols=range_col.upper-range_col.lower+1;

    QApplication::clipboard()->setText(m_model->copy(range_row.lower,range_col.lower,nrows,ncols));
}

void MyTableView::slot_paste()
{
    QString clipboardbuffer=QApplication::clipboard()->text();

    if(!clipboardbuffer.isEmpty())
    {
        QCPRange range_row,range_col;
        getVisualRowColSelectedRanges(range_row,range_col);

        //---------------------------------------------------------------------------------
        m_model->paste(range_row.lower,range_col.lower,clipboardbuffer);
    }
}

void MyTableView::getVisualRowColSelectedRanges(QCPRange &range_row,QCPRange &range_col)
{
    QModelIndexList selectedIndexes    = selectionModel()->selectedIndexes();
    QModelIndexList selectedColsIndexes= selectionModel()->selectedColumns();
    QModelIndexList selectedRowsIndexes= selectionModel()->selectedRows();

    for (int i = 0; i < selectedIndexes.count(); ++i)
    {
        QModelIndex current = selectedIndexes[i];
        int visualIndexRows=verticalHeader()->visualIndex(current.row())+m_model->getRowOffset();
        int visualIndexCols=horizontalHeader()->visualIndex(current.column());

        if(i==0)
        {
            range_row.lower=visualIndexRows;
            range_row.upper=visualIndexRows;
            range_col.lower=visualIndexCols;
            range_col.upper=visualIndexCols;
        }
        else
        {
            if(visualIndexRows<range_row.lower)range_row.lower=visualIndexRows;
            else if(visualIndexRows>range_row.upper)range_row.upper=visualIndexRows;
            if(visualIndexCols<range_col.lower)range_col.lower=visualIndexCols;
            else if(visualIndexCols>range_col.upper)range_col.upper=visualIndexCols;
        }
    }

    if(selectedRowsIndexes.size()>0)
    {
        range_col.lower=0;
        range_col.upper=model()->tableData().cols()-1;
    }

    if(selectedColsIndexes.size()>0)
    {
        range_row.lower=0;
        range_row.upper=model()->tableData().rows()-1;
    }
}

Eigen::VectorXd MyTableView::getColDataDouble(int logicalColId)
{
    return m_model->getColLogicalDataDouble(logicalColId);
}

QString MyTableView::getColName(int logicalColId)
{
    return m_model->getLogicalColName(logicalColId);
}

QString MyTableView::getSelectionPattern()
{
    QString patterns;
    QModelIndexList selectedCols=selectionModel()->selectedColumns();
    QModelIndexList selectedRows=selectionModel()->selectedRows();

    for(int i=0;i<selectedCols.size();i++)
    {
        int index=horizontalHeader()->visualIndex(selectedCols[i].column());
        patterns+=QString("C%1,").arg(index+1);
    }

    for(int i=0;i<selectedRows.size();i++)
    {
        int index=verticalHeader()->visualIndex(selectedRows[i].row());
        patterns+=QString("R%1,").arg(index+1);
    }

    return patterns;
}

void MyTableView::setSelectionPattern(QString pattern)
{
    clearSelection();
    QAbstractItemView::SelectionMode currentSelectionMode=selectionMode();
    setSelectionMode(QAbstractItemView::MultiSelection);
    QStringList patterns=pattern.split(",",QString::SkipEmptyParts);

    for(int i=0;i<patterns.size();i++)
    {
        if(patterns[i].startsWith('R'))
        {
            patterns[i].remove('R');
            bool ok=false;
            unsigned int index=patterns[i].toUInt(&ok);
            if(ok && index>0)
            {
                selectRow(index-1);
            }
        }
        else if(patterns[i].startsWith('C'))
        {
            patterns[i].remove('C');
            bool ok=false;
            unsigned int index=patterns[i].toUInt(&ok);
            if(ok && index>0)
            {
                selectColumn(index-1);
            }
        }
    }
    setSelectionMode(currentSelectionMode);
}

