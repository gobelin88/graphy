#ifndef PENSTYLECOMBOBOX_H
#define PENSTYLECOMBOBOX_H

#include <QComboBox>

class PenStyleComboBox : public QComboBox
{
public:
    PenStyleComboBox(QWidget * parent);

    QPixmap getPixmap(QSize size, int index);
    QPen getCurrentPen();
};

#endif // PENSTYLECOMBOBOX_H
