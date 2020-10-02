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

    MyModel * model(){return m_model;}

public slots:
    void slot_deleteSelected();

protected:
    void wheelEvent(QWheelEvent * event);

private:
    MyModel * m_model;
};

#endif // MYTABLEVIEW_H
