#include <QTableView>
#include "MyTableModel.h"
#include "MyItemDelegate.h"
#include "MyCustomPlot.h"

#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H


class MyTableView : public QTableView
{
    Q_OBJECT
public:
    MyTableView(int rowsSpan,
                QWidget * parent,
                int nbRow=1,
                int nbCols=1);

    void createNew(int nbRow,int nbCols,int rowsSpan);

    MyModel * model() {return m_model;}
    QWidget * getContainer(){return container;}

    Eigen::VectorXd getLogicalColDataDouble(int logicalColId)const;
    QVector<QString> getLogicalColDataString(int logicalColId)const;
    Eigen::VectorXcd getLogicalColDataComplex(int logicalColId) const;
    QString getLogicalColName(int logicalColId);

    QString getSelectionPattern();
    void setSelectionPattern(QString pattern);

    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);

    void save(QString filename);
    void open(QString filename);
    bool isOpen(){return is_open;}
    bool isSave(){return is_save;}

public slots:
    void slot_deleteSelected();
    void slot_removeSelectedRowsAndCols();
    void slot_complexify();
    void slot_filter();
    void slot_copy();
    void slot_paste();
    void slot_newColumn(QString varName, VectorXv data);
    void slot_newColumn(QString varName,Eigen::VectorXd data);
    void slot_newColumn(QString varName,Eigen::VectorXcd data);
    void slot_newColumn(QString varName,QVector<QString> data);
    void slot_newRowBelow();
    void slot_newRowAbove();
    void slot_startUpdateColumns();
    void slot_finishUpdateColumns();

signals:
    void sig_opened(MyTableView * newtable);
    void sig_saved(MyTableView * newtable);

protected:
    void wheelEvent(QWheelEvent * event);
    void resizeEvent(QResizeEvent *event);

private:
    void getVisualRowColSelectedRanges(QCPRange &range_row,QCPRange &range_col);

    MyModel * m_model;
    MyItemDelegate * m_delegate;
    QWidget * container;

    //Popup
    void createPopup();
    QMenu* popup_menu;
    QMenu* menuRows;
    QMenu* menuColumns;

    QAction * actNewRowBelow ;
    QAction * actNewRowAbove ;
    QAction * actNewRowBegin ;
    QAction * actNewRowEnd   ;
    QAction * actNewRowsBegin;
    QAction * actNewRowsEnd  ;
    QAction * actSetNumberOfRows;
    QAction * actCopy;
    QAction * actPaste;
    QAction * actNewColumn;
    QAction * actDelete;
    QAction * actRemoveColumnsRows;
    QAction * actUpdateColumns;
    QAction * actComplexify;

    bool is_open,is_save;

protected :
    void mousePressEvent(QMouseEvent* event);
};

#endif // MYTABLEVIEW_H
