#include <QTableView>
#include "mytablemodel.h"
#include "myitemdelegate.h"
#include "qcustomplot.h"

#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H


class MyTableView : public QTableView
{
    Q_OBJECT
public:
    MyTableView(int nbRow,
                int nbCols,
                int rowsSpan,
                QWidget * parent);

    void createNew(int nbRow,int nbCols,int rowsSpan);

    MyModel * model() {return m_model;}
    QWidget * getContainer(){return container;}

    Eigen::VectorXd getLogicalColDataDouble(int logicalColId);
    QVector<QString> getLogicalColDataString(int logicalColId);
    QString getLogicalColName(int logicalColId);

    QString getSelectionPattern();
    void setSelectionPattern(QString pattern);

public slots:
    void slot_deleteSelected();
    void slot_removeSelectedRowsAndCols();
    void slot_filter();
    void slot_copy();
    void slot_paste();
    void slot_newColumn(QString varName,Eigen::VectorXd data);

protected:
    //void wheelEvent(QWheelEvent * event);

private:
    void getVisualRowColSelectedRanges(QCPRange &range_row,QCPRange &range_col);

    MyModel * m_model;
    MyItemDelegate * m_delegate;
    QWidget * container;
};

#endif // MYTABLEVIEW_H
