#include <qcustomplot.h>
#include <QVector>
#include <iostream>

#include "box.h"
#include "curve2d.h"
#include "tabledata.h"

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

    void createPopup();

    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);

public slots:
    void slot_add_data(const Curve2D& datacurve);

    void mousePress(QMouseEvent* event);
    void mouseDoublePress(QMouseEvent* event);

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

    void slot_set_color();
    void slot_set_style();
    void slot_clear_marks();
    void addMark(double cx,double cy,QString markstr);

    void slot_setAxisXType(int mode);
    void slot_setAxisYType(int mode);


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
    QAction* actColor;
    QAction* actStyle;

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

    QString current_filename;

    unsigned int getId();

    Curve2D* sharedBuf;
    QPen pen_select;

    Qt::AlignmentFlag left_right;
    Qt::AlignmentFlag top_bottom;
};

#endif // VIEWER1D_H
