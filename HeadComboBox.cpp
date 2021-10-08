#include "HeadComboBox.h"
#include "MyCustomPlot.h"
#include <QString>

HeadComboBox::HeadComboBox(QWidget * parent):QComboBox(parent)
{
    QStringList headNameList;
    headNameList.append(QString("None"));
    headNameList.append(QString("FlatArrow"));
    headNameList.append(QString("SpikeArrow"));
    headNameList.append(QString("LineArrow"));
    headNameList.append(QString("Disc"));
    headNameList.append(QString("Square"));
    headNameList.append(QString("Diamond"));
    headNameList.append(QString("Bar"));
    headNameList.append(QString("HalfBar"));
    headNameList.append(QString("SkewedBar"));

    QList<int> headValueList;
    headValueList.append(int(QCPLineEnding::esNone));
    headValueList.append(int(QCPLineEnding::esFlatArrow));
    headValueList.append(int(QCPLineEnding::esSpikeArrow));
    headValueList.append(int(QCPLineEnding::esLineArrow));
    headValueList.append(int(QCPLineEnding::esDisc));
    headValueList.append(int(QCPLineEnding::esSquare));
    headValueList.append(int(QCPLineEnding::esDiamond));
    headValueList.append(int(QCPLineEnding::esBar));
    headValueList.append(int(QCPLineEnding::esHalfBar));
    headValueList.append(int(QCPLineEnding::esSkewedBar));

    QPixmap pixmap(64,64);

    QCPPainter * painter= new QCPPainter(&pixmap);
    QCPLineEnding lineEnding;
    lineEnding.setLength(40);
    lineEnding.setWidth(32);

    for(int i=0;i<headValueList.size();i++)
    {
        pixmap.fill();
        lineEnding.setStyle(static_cast<QCPLineEnding::EndingStyle>(headValueList[i]));
        if(i<4)
        {
            lineEnding.draw(painter,QCPVector2D(32+lineEnding.length()/2,32),0.0);
        }
        else
        {
            lineEnding.draw(painter,QCPVector2D(32,32),0.0);
        }
        QIcon icon(pixmap);
        this->addItem(icon,headNameList[i],headValueList[i]);
    }

    delete painter;
}
