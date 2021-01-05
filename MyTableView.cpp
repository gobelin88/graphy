#include <QWheelEvent>
#include "MyTableView.h"

MyTableView::MyTableView(int nbRow,
                         int nbCols,
                         int rowsSpan,
                         QWidget *parent):QTableView(parent)
{
    createNew(nbRow,nbCols,rowsSpan);

    createPopup();
    //this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectColumns);
    resizeColumnsToContents();
}

MyTableView::MyTableView(int rowsSpan,QWidget * parent):QTableView(parent)
{
    createNew(1,1,rowsSpan);
    createPopup();

}

void MyTableView::createNew(int nbRow,int nbCols,int rowsSpan)
{
    m_model = new MyModel(nbRow,nbCols,rowsSpan);
    setHorizontalHeader(m_model->horizontalHeader());
    setVerticalHeader(m_model->verticalHeader());

    m_delegate=new MyItemDelegate(this);
    setItemDelegate(m_delegate);

    container=new QWidget();
    QHBoxLayout * layout=new QHBoxLayout(container);
    layout->addWidget(this);
    layout->addWidget(m_model->verticalScrollBar());

    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setModel(m_model);
}

void MyTableView::wheelEvent(QWheelEvent * event)
{
    if(event->delta()>0)
    {
        m_model->setRowOffset(m_model->getRowOffset()-1);
    }
    else
    {
        m_model->setRowOffset(m_model->getRowOffset()+1);
    }
}

void MyTableView::resizeEvent(QResizeEvent *event)
{
    if(event)
    {
        QTableView::resizeEvent(event);
    }

    this->model()->setRowSpan((this->size().height()-this->horizontalHeader()->size().height())/
                                   (this->verticalHeader()->defaultSectionSize())-1);
}
void MyTableView::slot_deleteSelected()
{
    QModelIndexList selectedIndexes=selectionModel()->selectedIndexes();
    QModelIndexList selectedColsIndexes=selectionModel()->selectedColumns();

    m_model->clearLogicalIndexes(selectedIndexes);
    m_model->clearLogicalIndexesCols(selectedColsIndexes);
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

void MyTableView::slot_newColumn(QString varName,VectorXv data)
{
    m_model->slot_newColumn(varName,data);
}

void MyTableView::slot_newColumn(QString varName,Eigen::VectorXd data)
{
    m_model->slot_newColumn(varName,fromDouble(data));
}
void MyTableView::slot_newColumn(QString varName,Eigen::VectorXcd data)
{
    m_model->slot_newColumn(varName,fromComplex(data));
}
void MyTableView::slot_newColumn(QString varName,QVector<QString> data)
{
    m_model->slot_newColumn(varName,fromString(data));
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

Eigen::VectorXd MyTableView::getLogicalColDataDouble(int logicalColId) const
{
    return m_model->getColLogicalDataDouble(logicalColId);
}

Eigen::VectorXcd MyTableView::getLogicalColDataComplex(int logicalColId)const
{
    return m_model->getColLogicalDataComplex(logicalColId);
}

QVector<QString> MyTableView::getLogicalColDataString(int logicalColId)const
{
    return m_model->getColLogicalDataString(logicalColId);
}

QString MyTableView::getLogicalColName(int logicalColId)
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

void MyTableView::applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map)
{
    QMap<QString,QAction*> shortcuts_links;
    shortcuts_links.insert(QString("Update"),actUpdateColumns);
    shortcuts_links.insert(QString("Edit/Add-variable"),actNewColumn);
    shortcuts_links.insert(QString("Delete"),actDelete);
    shortcuts_links.insert(QString("Delete-cols-rows"),actRemoveColumnsRows);

    QMapIterator<QString, QKeySequence> i(shortcuts_map);
    while (i.hasNext())
    {
        i.next();

        if (shortcuts_links.contains(i.key()))
        {
            shortcuts_links[i.key()]->setShortcut(i.value());
        }
    }
}

void MyTableView::createPopup()
{
    popup_menu=new QMenu(this);
    menuRows=new QMenu("Rows",this);
    menuColumns=new QMenu("Columns",this);

    actCopy=new QAction("Copy",this);
    actPaste=new QAction("Paste",this);

    actDelete=new QAction("Delete",this);
    actRemoveColumnsRows=new QAction("Remove",this);

    actNewColumn=new QAction("New",this);
    actUpdateColumns=new QAction("Update",this);

    actNewRowBelow = new QAction("Insert below" ,  this);
    actNewRowAbove = new QAction("Insert above" ,  this);
    actNewRowBegin = new QAction("Add begin" ,  this);
    actNewRowEnd   = new QAction("Add end"   ,  this);
    actNewRowsBegin= new QAction("Add begin",  this);
    actNewRowsEnd  = new QAction("Add end"  ,  this);

    actNewRowBelow ->setShortcut(QKeySequence("PgDown"));
    actNewRowAbove ->setShortcut(QKeySequence("PgUp"));
    actNewRowBegin ->setShortcut(QKeySequence("Alt+PgUp"));
    actNewRowEnd   ->setShortcut(QKeySequence("Alt+PgDown"));
    actNewRowsBegin->setShortcut(QKeySequence("Ctrl+Alt+PgUp"));
    actNewRowsEnd  ->setShortcut(QKeySequence("Ctrl+Alt+PgDown"));
    actCopy ->setShortcut(QKeySequence("Ctrl+C"));
    actPaste->setShortcut(QKeySequence("Ctrl+V"));

    this->addAction(actCopy );
    this->addAction(actPaste );
    this->addAction(actDelete);
    this->addAction(actNewColumn);
    this->addAction(actRemoveColumnsRows);
    this->addAction(actUpdateColumns);
    this->addAction(actNewRowBelow );
    this->addAction(actNewRowAbove );
    this->addAction(actNewRowBegin );
    this->addAction(actNewRowEnd   );
    this->addAction(actNewRowsBegin);
    this->addAction(actNewRowsEnd  );

    popup_menu->addAction(actCopy);
    popup_menu->addAction(actPaste);
    popup_menu->addAction(actDelete);
    popup_menu->addAction(actRemoveColumnsRows);
    popup_menu->addMenu(menuColumns);
    menuColumns->addAction(actNewColumn);
    menuColumns->addAction(actUpdateColumns);
    popup_menu->addMenu(menuRows);
    menuRows->addAction(actNewRowBelow );
    menuRows->addAction(actNewRowAbove );
    menuRows->addAction(actNewRowBegin );
    menuRows->addAction(actNewRowEnd   );
    menuRows->addAction(actNewRowsBegin);
    menuRows->addAction(actNewRowsEnd  );

    auto customContainerActions=this->actions();
    for(auto act:customContainerActions)
    {
        act->setShortcutVisibleInContextMenu(true);
    }

    connect(actNewRowEnd    ,&QAction::triggered,m_model,&MyModel::slot_newRowEnd);
    connect(actNewRowsEnd   ,&QAction::triggered,m_model,&MyModel::slot_newRowsEnd);
    connect(actNewRowBegin  ,&QAction::triggered,m_model,&MyModel::slot_newRowBegin);
    connect(actNewRowsBegin ,&QAction::triggered,m_model,&MyModel::slot_newRowsBegin);

    connect(actNewRowBelow  ,&QAction::triggered,this   ,&MyTableView::slot_newRowBelow);
    connect(actNewRowAbove  ,&QAction::triggered,this   ,&MyTableView::slot_newRowAbove);
    connect(actCopy         ,&QAction::triggered,this   ,&MyTableView::slot_copy);
    connect(actPaste        ,&QAction::triggered,this   ,&MyTableView::slot_paste);

    connect(actNewColumn,&QAction::triggered,m_model,&MyModel::slot_editColumn);
    connect(actDelete,&QAction::triggered,this,&MyTableView::slot_deleteSelected);
    connect(actRemoveColumnsRows,&QAction::triggered,this,&MyTableView::slot_removeSelectedRowsAndCols);
    connect(actUpdateColumns,&QAction::triggered,m_model,&MyModel::slot_updateColumns);
}

void MyTableView::mousePressEvent(QMouseEvent* event)
{
    QTableView::mousePressEvent(event);
    if (event->button() == Qt::RightButton)
    {
        popup_menu->exec(mapToGlobal(event->pos()));
    }
}

void MyTableView::slot_newRowBelow()
{
    QModelIndexList selectedRows=selectionModel()->selectedRows();
    if(selectedRows.size()>0)
    {
        for(int i=0;i<selectedRows.size();i++)
        {
            int index=verticalHeader()->visualIndex(selectedRows[i].row());
            m_model->slot_newRowBelow(index);
        }
    }
}

void MyTableView::slot_newRowAbove()
{
    QModelIndexList selectedRows=selectionModel()->selectedRows();
    if(selectedRows.size()>0)
    {
        for(int i=0;i<selectedRows.size();i++)
        {
            int index=verticalHeader()->visualIndex(selectedRows[i].row());
            m_model->slot_newRowAbove(index);
        }
    }
}
