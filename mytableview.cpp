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
