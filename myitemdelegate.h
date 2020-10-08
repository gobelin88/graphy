#ifndef MYITEMDELEGATE_H
#define MYITEMDELEGATE_H
#include <QItemDelegate>
#include <QPainter>

class MyItemDelegate: public QItemDelegate
{
public:
    MyItemDelegate(QObject *parent);

    //void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

};

#endif // MYITEMDELEGATE_H
