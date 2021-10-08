#include "BrushStyleComboBox.h"

#include <QBrush>
#include <QPainter>

BrushStyleComboBox::BrushStyleComboBox(QWidget *parent):QComboBox(parent)
{
    addItem(QString("NoBrush"),int(Qt::NoBrush));
    addItem(QString("SolidPattern"),int(Qt::SolidPattern));
    addItem(QString("Dense1Pattern"),int(Qt::Dense1Pattern));
    addItem(QString("Dense2Pattern"),int(Qt::Dense2Pattern));
    addItem(QString("Dense3Pattern"),int(Qt::Dense3Pattern));
    addItem(QString("Dense4Pattern"),int(Qt::Dense4Pattern));
    addItem(QString("Dense5Pattern"),int(Qt::Dense5Pattern));
    addItem(QString("Dense6Pattern"),int(Qt::Dense6Pattern));
    addItem(QString("Dense7Pattern"),int(Qt::Dense7Pattern));
    addItem(QString("HorPattern"),int(Qt::HorPattern));
    addItem(QString("VerPattern"),int(Qt::VerPattern));
    addItem(QString("CrossPattern"),int(Qt::CrossPattern));
    addItem(QString("BDiagPattern"),int(Qt::BDiagPattern));
    addItem(QString("DiagCrossPattern"),int(Qt::DiagCrossPattern));

    QSize icon_size(64,32);
    this->setIconSize(icon_size);

    for(int k=0;k<this->count();k++)
    {
        QIcon icon(getPixmap(icon_size,k));
        this->setItemIcon(k,icon);
    }
}

QBrush BrushStyleComboBox::getCurrentBrush()
{
    QBrush brush(currentData().value<Qt::BrushStyle>());
    brush.setColor(Qt::black);
    return brush;
}

QPixmap BrushStyleComboBox::getPixmap(QSize size,int index)
{
    int w=size.width();
    int h=size.height();
    int s=4;
    QPixmap pixmap(size);
    pixmap.fill();
    QPainter painter(&pixmap);
    setCurrentIndex(index);
    painter.setBrush(getCurrentBrush());
    painter.drawRect(QRect(s,s,w-s*2,h-s*2));

    return pixmap;
}

