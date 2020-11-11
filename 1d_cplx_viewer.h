#include <qcustomplot.h>
#include <QVector>
#include <iostream>

#include "box.h"
#include "curve2dComplex.h"

#ifndef VIEWER1DCPLX_H
#define VIEWER1DCPLX_H


class Viewer1DCPLX:public QCustomPlot
{
    Q_OBJECT
public:
    Viewer1DCPLX(const QMap<QString,QKeySequence>& shortcuts_map);
    ~Viewer1DCPLX();

    void newGraph(QString name);
    void newSubGraph(QString name);
    void createPopup();

    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);


public slots:
    void mousePress(QMouseEvent* event);
    void mouseDoublePress(QMouseEvent* event);

    void slot_save_image();
    void slot_add_data_graph(const Curve2D_GainPhase& curve);

    void slot_rescale();
    void slot_show_legend(bool value);
    void slot_top_legend(bool value);
    void slot_left_legend(bool value);

    void slot_fit_ralpcprb();
    void slot_fit_rlpc();
    void slot_fit_rl();
    void slot_fit_rlc();
    void slot_fit_rlcapcb();

    void slot_set_color();
    void slot_set_style();
    void slot_clear_marks();

signals:
    void pick(double p0);

protected:
    QList<Curve2D_GainPhase> getCurves();

    QList<QColor> colors;

    QMenu* popup_menu;
    QAction* actSave;
    QAction* actRescale;
    QAction* actClearMarks;
    QAction* actLegendShowHide;
    QAction* actLegendTopBottom;
    QAction* actLegendLeftRight;
    QAction* actColor;
    QAction* actStyle;

    QMenu* menu_legend;
    QMenu* menu_fit;

    QAction* actFitRaLpCpRb;
    QAction* actFitRLpC;
    QAction* actFitRL;
    QAction* actFitRLC;
    QAction* actFitRLCapCb;

    QString current_filename;

    unsigned int getId();

    QPen pen_select;

    Qt::AlignmentFlag left_right;
    Qt::AlignmentFlag top_bottom;
};

#endif // VIEWER1D_H
