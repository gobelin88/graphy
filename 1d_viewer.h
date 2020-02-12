#include <qcustomplot.h>
#include <QVector>
#include <iostream>

#include "box.h"
#include "curve2d.h"

#ifndef VIEWER1D_H
#define VIEWER1D_H


class Viewer1D:public QCustomPlot
{
    Q_OBJECT
public:
    Viewer1D(Curve2D * sharedBuf);
    ~Viewer1D();

    void newGraph(QString name);
    void createPopup();

public slots:
    void mousePress(QMouseEvent * event);
    void mouseDoublePress(QMouseEvent * event);

    void slot_save_image();
    void colorScaleChanged(const QCPRange & range);
    void slot_add_data_cloud(const Curve2D & datacurve);
    void slot_add_data_curve(const Curve2D & datacurve);
    void slot_add_data_graph(const Curve2D & datacurve);
    void slot_fit_linear();
    void slot_rescale();
    void slot_fit_polynome();
    void slot_delete();
    void slot_show_legend(bool value);
    void slot_top_legend(bool value);
    void slot_fit_gaussian();
    void slot_fit_sinusoide();
    void slot_fit_sigmoid();
    void slot_fit_rlc();
    void slot_histogram(QVector<double> data,QString name);

    void slot_set_color();
    void slot_set_style();
    void slot_clear_marks();

    void slot_copy();
    void slot_paste();

signals:
    void pick(double p0);

protected:
    QList<QCPCurve*> getQCPCurves();
    QList<QCPCurve*> getSelectedQCPCurves();
    QList<Curve2D> getSelectedCurves();
    QList<QCPAbstractPlottable *> getSelectedCurvesOrGraphs();

    QList<QColor> colors;

    QMenu * popup_menu;
    QAction * actSave;
    QAction * actRescale;
    QAction * actDelete;
    QAction * actClearMarks;

    QAction * actCopy;
    QAction * actPaste;

    QAction * actLegendShow;
    QAction * actLegendTop;

    QMenu * menu_legend;
    QMenu * menu_fit;
    QMenu * menu_mathematics;
    QMenu * menu_electronics;

    QAction * actFitLinear;
    QAction *actFitPolynome;
    QAction *actFitGaussian;
    QAction *actFitSigmoid;
    QAction *actFitRLC;
    QAction *actFitSinusoide;
    QAction *actColor;
    QAction *actStyle;

    QString current_filename;

    unsigned int getId();

    Curve2D * sharedBuf;
    QPen pen_select;
};

#endif // VIEWER1D_H
