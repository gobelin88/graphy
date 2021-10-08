#include "PenStyleComboBox.h"
#include <QPen>
#include <QPainter>

PenStyleComboBox::PenStyleComboBox(QWidget *parent):QComboBox(parent)
{
    addItem("SolidLine",int(Qt::PenStyle::SolidLine));
    addItem("DashLine",int(Qt::PenStyle::DashLine));
    addItem("DotLine",int(Qt::PenStyle::DotLine));
    addItem("DashDotLine",int(Qt::PenStyle::DashDotLine));
    addItem("DashDotDotLine",int(Qt::PenStyle::DashDotDotLine));
    addItem("CustomDashLine",int(Qt::PenStyle::CustomDashLine));

    QSize icon_size(64,16);
    this->setIconSize(icon_size);

    for(int k=0;k<this->count();k++)
    {
        QIcon icon(getPixmap(icon_size,k));
        this->setItemIcon(k,icon);
    }
}

QPen PenStyleComboBox::getCurrentPen()
{
    QPen pen(currentData().value<Qt::PenStyle>());
    pen.setWidth(2);
    return pen;
}

QPixmap PenStyleComboBox::getPixmap(QSize size,int index)
{
    int w=size.width();
    int h=size.height();

    QPixmap pixmap(size);
    pixmap.fill();
    QPainter painter(&pixmap);
    setCurrentIndex(index);
    painter.setPen( getCurrentPen() );
    painter.drawLine(QPointF(0,h/2),QPointF(w,h/2));

    return pixmap;
}
