#include "ScatterComboBox.h"
#include "MyCustomPlot.h"

ScatterComboBox::ScatterComboBox(QWidget * parent):QComboBox(parent)
{
    QStringList entriesNames;
    entriesNames.append(QString("None"));
    entriesNames.append(QString("Dot"));
    entriesNames.append(QString("Cross"));
    entriesNames.append(QString("Plus"));
    entriesNames.append(QString("Circle"));
    entriesNames.append(QString("Disc"));
    entriesNames.append(QString("Square"));
    entriesNames.append(QString("Diamond"));
    entriesNames.append(QString("Star"));
    entriesNames.append(QString("Triangle"));
    entriesNames.append(QString("TriangleInverted"));
    entriesNames.append(QString("CrossSquare"));
    entriesNames.append(QString("PlusSquare"));
    entriesNames.append(QString("CrossCircle"));
    entriesNames.append(QString("PlusCircle"));
    entriesNames.append(QString("Peace"));
    entriesNames.append(QString("Arrow"));

    QList<int> entriesValues;
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssNone));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssDot));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssCross));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssPlus));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssCircle));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssDisc));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssSquare));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssDiamond));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssStar));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssTriangle));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssTriangleInverted));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssCrossSquare));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssPlusSquare));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssCrossCircle));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssPlusCircle));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssPeace));
    entriesValues.append(int(QCPScatterStyle::ScatterShape::ssArrow));

    QPixmap pixmap(64,64);

    QCPPainter * painter= new QCPPainter(&pixmap);
    QCPScatterStyle shape;
    shape.setSize(40);

    for(int i=0;i<entriesValues.size();i++)
    {
        pixmap.fill();
        shape.setShape(static_cast<QCPScatterStyle::ScatterShape>(entriesValues[i]));

        shape.drawShape(painter,QPointF(32,32),0.0);

        QIcon icon(pixmap);
        this->addItem(icon,entriesNames[i],entriesValues[i]);
    }

    delete painter;
}
