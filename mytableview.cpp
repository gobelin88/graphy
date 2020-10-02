#include <QWheelEvent>

#include "mytableview.h"

MyTableView::MyTableView(int nbRow,
                         int nbCols,
                         int rowsSpan,
                         QWidget *parent):QTableView(parent)
{
    createNew(nbRow,nbCols,rowsSpan);
}

void MyTableView::createNew(int nbRow,int nbCols,int rowsSpan)
{
    model = new MyModel(nbRow,nbCols,rowsSpan);
    setHorizontalHeader(model->horizontalHeader());
    setVerticalHeader(model->verticalHeader());
    setModel(model);
}

bool MyTableView::directOpen(QString filename)
{
    return model->open(filename);
}

const MatrixXv & MyTableView::tableData()
{
    return model->tableData();
}

void MyTableView::wheelEvent(QWheelEvent * event)
{
    if(event->delta()>0)
    {
        model->setRowOffset(model->getRowOffset()+1);
    }
    else
    {
        model->setRowOffset(model->getRowOffset()-1);
    }


   //this->model->submit();
   //this->update();
}
