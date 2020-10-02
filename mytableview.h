#include <QTableView>
#include "mytablemodel.h"

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
    bool directOpen(QString filename);
    const MatrixXv & tableData();

protected:
    void wheelEvent(QWheelEvent * event);

private:
    MyModel * model;
};

#endif // MYTABLEVIEW_H
