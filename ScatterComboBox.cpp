#include "ScatterComboBox.h"
#include "MyCustomPlot.h"

ScatterComboBox::ScatterComboBox(QWidget * parent):QComboBox(parent)
{
    addItem(QStringLiteral("None"),             int(QCPScatterStyle::ScatterShape::ssNone));
    addItem(QStringLiteral("Dot"),              int(QCPScatterStyle::ScatterShape::ssDot));
    addItem(QStringLiteral("Cross"),            int(QCPScatterStyle::ScatterShape::ssCross));
    addItem(QStringLiteral("Plus"),             int(QCPScatterStyle::ScatterShape::ssPlus));
    addItem(QStringLiteral("Circle"),           int(QCPScatterStyle::ScatterShape::ssCircle));
    addItem(QStringLiteral("Disc"),             int(QCPScatterStyle::ScatterShape::ssDisc));
    addItem(QStringLiteral("Square"),           int(QCPScatterStyle::ScatterShape::ssSquare));
    addItem(QStringLiteral("Diamond"),          int(QCPScatterStyle::ScatterShape::ssDiamond));
    addItem(QStringLiteral("Star"),             int(QCPScatterStyle::ScatterShape::ssStar));
    addItem(QStringLiteral("Triangle"),         int(QCPScatterStyle::ScatterShape::ssTriangle));
    addItem(QStringLiteral("TriangleInverted"), int(QCPScatterStyle::ScatterShape::ssTriangleInverted));
    addItem(QStringLiteral("CrossSquare"),      int(QCPScatterStyle::ScatterShape::ssCrossSquare));
    addItem(QStringLiteral("PlusSquare"),       int(QCPScatterStyle::ScatterShape::ssPlusSquare));
    addItem(QStringLiteral("CrossCircle"),      int(QCPScatterStyle::ScatterShape::ssCrossCircle));
    addItem(QStringLiteral("PlusCircle"),       int(QCPScatterStyle::ScatterShape::ssPlusCircle));
    addItem(QStringLiteral("Peace"),            int(QCPScatterStyle::ScatterShape::ssPeace));
    addItem(QStringLiteral("Arrow"),            int(QCPScatterStyle::ScatterShape::ssArrow));
}
