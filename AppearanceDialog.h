#ifndef APPEARANCEDIALOG_H
#define APPEARANCEDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QComboBox>
#include "MyGradientComboBox.h"
#include <ColorWheel.hpp>
#include "MyCustomPlot.h"
#include "ScatterComboBox.h"
#include "PenStyleComboBox.h"
#include "BrushStyleComboBox.h"

class AppearanceDialog:public QDialog
{
public:
    AppearanceDialog(QWidget * parent);

    void configure(QList<QCPAbstractPlottable*> plottables);

    MyGradientComboBox * cb_gradient;
    QComboBox* cb_itemLineStyleList;
    ScatterComboBox * cb_ScatterShapes;
    QDoubleSpinBox* sb_ScatterSize;
    PenStyleComboBox * cb_penStyle;
    BrushStyleComboBox * cb_brushStyle;
    ColorWheel* cw_pen_color;
    ColorWheel* cw_brush_color;
    QDoubleSpinBox* sb_penWidth;
    QDoubleSpinBox* s_pen_alpha;
    QDoubleSpinBox* s_brush_alpha;
};

#endif // APPEARANCEDIALOG_H
