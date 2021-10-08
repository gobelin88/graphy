#ifndef BRUSHSTYLECOMBOBOX_H
#define BRUSHSTYLECOMBOBOX_H

#include <QComboBox>

class BrushStyleComboBox: public QComboBox
{
public:
    BrushStyleComboBox(QWidget * parent);

    QPixmap getPixmap(QSize size, int index);
    QBrush getCurrentBrush();
};

#endif // BRUSHSTYLECOMBOBOX_H
