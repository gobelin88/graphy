#include "HeadComboBox.h"
#include "MyCustomPlot.h"
#include <QString>

HeadComboBox::HeadComboBox(QWidget * parent):QComboBox(parent)
{
    QStringList entriesNames;
    entriesNames.append(QString("None"));
    entriesNames.append(QString("FlatArrow"));
    entriesNames.append(QString("SpikeArrow"));
    entriesNames.append(QString("LineArrow"));
    entriesNames.append(QString("Disc"));
    entriesNames.append(QString("Square"));
    entriesNames.append(QString("Diamond"));
    entriesNames.append(QString("Bar"));
    entriesNames.append(QString("HalfBar"));
    entriesNames.append(QString("SkewedBar"));

    QList<int> entriesValues;
    entriesValues.append(int(QCPLineEnding::esNone));
    entriesValues.append(int(QCPLineEnding::esFlatArrow));
    entriesValues.append(int(QCPLineEnding::esSpikeArrow));
    entriesValues.append(int(QCPLineEnding::esLineArrow));
    entriesValues.append(int(QCPLineEnding::esDisc));
    entriesValues.append(int(QCPLineEnding::esSquare));
    entriesValues.append(int(QCPLineEnding::esDiamond));
    entriesValues.append(int(QCPLineEnding::esBar));
    entriesValues.append(int(QCPLineEnding::esHalfBar));
    entriesValues.append(int(QCPLineEnding::esSkewedBar));

    QPixmap pixmap(64,64);

    QCPPainter * painter= new QCPPainter(&pixmap);
    QCPLineEnding lineEnding;
    lineEnding.setLength(40);
    lineEnding.setWidth(32);

    for(int i=0;i<entriesValues.size();i++)
    {
        pixmap.fill();
        lineEnding.setStyle(static_cast<QCPLineEnding::EndingStyle>(entriesValues[i]));
        if(i<4)
        {
            lineEnding.draw(painter,QCPVector2D(32+lineEnding.length()/2,32),0.0);
        }
        else
        {
            lineEnding.draw(painter,QCPVector2D(32,32),0.0);
        }
        QIcon icon(pixmap);
        this->addItem(icon,entriesNames[i],entriesValues[i]);
    }

    delete painter;
}
