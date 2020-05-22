#include <qcustomplot.h>
#include <QVector>
#include <iostream>

#include "box.h"
#include "curve2d.h"
#include "tabledata.h"
#include "color_wheel.hpp"

#ifndef VIEWER1D_H
#define VIEWER1D_H


class Viewer1D:public QCustomPlot
{
    Q_OBJECT
public:
    Viewer1D(Curve2D* sharedBuf, const QMap<QString, QKeySequence>& shortcuts_map, QWidget* parent);
    ~Viewer1D();

    QCPGraph* newGraph(const Curve2D& datacurve);
    QCPCurve* newCurve(const Curve2D& datacurve);



    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);

public slots:
    void slot_add_data(const Curve2D& datacurve);

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mousePress(QMouseEvent* event);
    //void mouseDoublePress(QMouseEvent* event);

    void slot_fit_polynomial();
    void slot_fit_gaussian();
    void slot_fit_sinusoide();
    void slot_fit_sigmoid();

    void slot_fit_2var_polynomial();

    void slot_save_image();
    void slot_rescale();
    void slot_delete();
    void slot_show_legend(bool value);
    void slot_top_legend(bool value);
    void slot_left_legend(bool value);
    void slot_histogram(Eigen::VectorXd data, QString name, int nbbins);
    void slot_copy();
    void slot_paste();
    void slot_statistiques();

    void slot_clear_marks();
    void addLabel(double cx,double cy);
    void addTextLabel(double cx,double cy,QString markstr);

    void slot_setAxisXType(int mode);
    void slot_setAxisYType(int mode);
    void slot_setBrushColor(QColor color);
    void slot_setPenColor(QColor color);
    void slot_setPenWidth(double width);
    void slot_setStyle(int style);
    void slot_setScatter(int style);
    void slot_setPenStyle(int style);
    void slot_setBrushStyle(int style);
    void slot_setPenAlpha(double alpha);
    void slot_setBrushAlpha(double alpha);

    void slot_axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void slot_legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();

signals:
    void pick(double p0);
    void sig_displayResults(QString results);
    void sig_newColumn(QString varName,Eigen::VectorXd data);

protected:
    QList<QCPCurve*> getQCPCurves();
    QList<QCPCurve*> getSelectedQCPCurves();
    QList<Curve2D> getSelectedCurves();
    QList<QCPAbstractPlottable*> getSelectedCurvesOrGraphs();
    QList<QColor> colors;

    QString current_filename;
    unsigned int getId();
    Curve2D* sharedBuf;

    //status
    Qt::AlignmentFlag left_right;
    Qt::AlignmentFlag top_bottom;
    Qt::KeyboardModifiers modifiers;
    QString label;

    //Popup
    void createPopup();
    void configurePopup();
    QMenu* popup_menu;
    QAction* actSave;
    QAction* actRescale;
    QAction* actDelete;
    QAction* actClearMarks;
    QAction* actCopy;
    QAction* actPaste;
    QAction* actLegendShowHide;
    QAction* actLegendTopBottom;
    QAction* actLegendLeftRight;
    QMenu* menuParameters;
    QMenu* menuLegend;
    QMenu* menuFit;
    QMenu* menuScalarField;
    QMenu* menuScalarFieldFit;
    QAction* actFitPolynomial;
    QAction* actFitPolynomial2V;
    QAction* actFitGaussian;
    QAction* actFitSigmoid;
    QAction* actFitSinusoide;
    QAction* actStatistiques;

    QComboBox* cb_scale_mode_x;
    QComboBox* cb_scale_mode_y;
    QComboBox* cb_itemLineStyleList;
    QComboBox* cb_itemScatterStyleList;
    QComboBox* cb_penstyle;
    QComboBox* cb_brushstyle;
    Color_Wheel* cw_pen_color;
    Color_Wheel* cw_brush_color;
    QDoubleSpinBox* sb_pen_width;
    QDoubleSpinBox* s_pen_alpha;
    QDoubleSpinBox* s_brush_alpha;


};

#endif // VIEWER1D_H
