#include "MyItemDelegate.h"

MyItemDelegate::MyItemDelegate(QObject *parent): QItemDelegate(parent)
{

}

//void MyItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index)const
//{
//    QVariant value = index.data(Qt::BackgroundRole);
//    if (value.canConvert<QBrush>())
//    {
//        QPointF oldBO = painter->brushOrigin();
//        painter->setBrushOrigin(option.rect.topLeft());
//        painter->fillRect(option.rect, qvariant_cast<QBrush>(value));
//        painter->setBrushOrigin(oldBO);
//    }
//}
