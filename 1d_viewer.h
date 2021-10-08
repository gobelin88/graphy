#ifndef VIEWER1D_H
#define VIEWER1D_H

#include <MyCustomPlot.h>
#include <QVector>
#include <iostream>
#include "2d_curve.h"
#include "DataConvert.h"
#include "ColorWheel.hpp"
#include "MyGradientComboBox.h"
#include "MySciDoubleSpinBox.h"
#include "MyLabel.h"
#include "AppearanceDialog.h"

class Viewer1D:public QCustomPlot
{
    Q_OBJECT
public:
    Viewer1D(const QMap<QString, QKeySequence>& shortcuts_map, QWidget* parent);
    ~Viewer1D();

    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);
    void addSubMenu(QMenu * sub_menu);


    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

public slots:
    void slot_add_data(const Curve2D& datacurve);
    void slot_fit_polynomial();
    void slot_fit_gaussian();
    void slot_fit_sinusoide();
    void slot_fit_sigmoid();
    void slot_fit_circle();
    void slot_fit_ellipse();
    void slot_fit_custom();
    void slot_fit_2var_polynomial();
    void slot_save_image_current_map();
    void slot_save_image();
    void slot_rescale();
    void slot_delete();
    void slot_show_legend(bool value);
    void slot_top_legend(bool value);
    void slot_left_legend(bool value);
    void slot_histogram(Eigen::VectorXd _data, QString name, int nbbins);
    void slot_copy();
    void slot_paste();
    void slot_statistiques();
    void slot_svd();
    void slot_covariance();
    void slot_appearance();
    void slot_increasePenWidth();
    void slot_decreasePenWidth();
    void slot_setAxisXType(int mode);
    void slot_setAxisYType(int mode);
    void slot_setBrushColor(QColor color);
    void slot_setPenColor(QColor color);
    void slot_setPenWidth(double width);
    void slot_setStyle(int style);
    void slot_setScattersShape(int scatter_shape);
    void slot_setScatterShape(int scatter_shape);
    void slot_setScattersSize(int scatter_shape);
    void slot_setScatterSize(double scatter_size);
    void slot_setPenStyle(int style);
    void slot_setBrushStyle(int style);
    void slot_setPenAlpha(double alpha);
    void slot_setBrushAlpha(double alpha);
    void slot_plottableDoubleClick(QCPAbstractPlottable*plottable, int n, QMouseEvent*event);
    void slot_itemDoubleClick(QCPAbstractItem* item,QMouseEvent* event);
    void slot_axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void slot_legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void slot_clear_marks();
    void slot_gadgetLine();
    void slot_gadgetTracer();
    void slot_gadgetMark();
    void slot_gadgetText();
    void slot_gadgetArrow();
    void slot_gadgetAddArrow(Eigen::Vector2d A,Eigen::Vector2d B);
    void slot_gadgetDeltaLine();
    void slot_setScalarFieldGradientType(int type);
    void slot_showSubGridType(int state);
    void slot_auto_color1();
    void slot_auto_color2();
    void slot_auto_color3();
    void slot_auto_color4();
    void slot_auto_color5();
    void slot_auto_clear();
    void slot_setScatters();
    void slot_meanFilter();
    void slot_medianFilter();
    void slot_Distance();
    void slot_SetSbAxisMax_Min(double min);
    void slot_SetSbAxisMin_Max(double max);

signals:
    void pick(double p0);
    void sig_displayResults(QString results);
    void sig_newColumn(QString varName,Eigen::VectorXd data);

protected:
    QVector<int> getScattersList();
    void setScattersList(QVector<int> scattersList);
    QVector<int> getScattersSizeList();
    void setScattersSizeList(QVector<int> scattersSizeList);

    template <typename T>
    QList<T*> getQCPListOf(bool selected)
    {
        QList<QCPAbstractPlottable*> plottableslist=(selected)?this->selectedPlottables():this->plottables();
        QList<T*> list;
        for (int i=0; i<plottableslist.size(); i++)
        {
            T* current=dynamic_cast<T*>(plottableslist[i]);
            if (current)
            {
                list.push_back(current);
            }
        }
        return list;
    }

    QList<Curve2D> getSelectedCurves();
    QList<QCPAbstractPlottable*> getCurvesOrGraphs(bool selected);

    QString current_filename;
    unsigned int getColorId();

    //status
    Qt::AlignmentFlag left_right;
    Qt::AlignmentFlag top_bottom;
    Qt::KeyboardModifiers modifiers;
    QString state_arrow,state_mark,state_tracer,state_line,state_deltaline;
    QCPItemCurve* arrowItem;
    QCPItemTracer* tracerItem;
    QCPItemText * textItem;
    MyLabel* tracerLabel;
    QCPItemLine* lineItem;
    QCPItemLine* deltaLineItemA,*deltaLineItemB;
    QCPItemLine* deltaArrowItem;
    QCPItemText* deltaLabel;

    //Popup
    void createPopup();

    QMenu* popup_menu;
    QAction* actSaveMap;
    QAction* actSave;
    QAction* actRescale;
    QAction* actDelete;
    QAction* actCopy;
    QAction* actPaste;
    QAction* actLegendShowHide;
    QAction* actLegendTopBottom;
    QAction* actLegendLeftRight;
    QAction* actAutoColor1;
    QAction* actAutoColor2;
    QAction* actAutoColor3;
    QAction* actAutoColor4;
    QAction* actAutoColor5;
    QAction* actAutoColorClear;
    QAction* actIncreasePenWidth;
    QAction* actDecreasePenWidth;
    QAction* actSetScatters;
    QAction* actAppearance;
    QMenu* menuAppearance;
    QMenu* menuLegend;
    QMenu* menuFit;
    QMenu* menuScalarField;
    QMenu* menuScalarFieldFit;
    QMenu* menuAnalyse;
    QMenu* menuFilters;
    QMenu* menuMisc;
    QMenu* menuThemes;
    QAction* actFilterMedian;
    QAction* actFilterMean;
    QAction* actFitPolynomial;
    QAction* actFitPolynomial2V;
    QAction* actDistance;
    QAction* actFitGaussian;
    QAction* actFitSigmoid;
    QAction* actFitSinusoide;
    QAction* actFitCircle;
    QAction* actFitEllipse;
    QAction* actFitCustomExp;
    QAction* actStatistiques;
    QAction* actSvd;
    QAction* actCovariance;
    QMenu* menuGadgets;
    QAction* actClearGadgets;
    QAction* actGadgetArrow;
    QAction* actGadgetText;
    QAction* actGadgetMark;
    QAction* actGadgetTracer;
    QAction* actGadgetLine;
    QAction* actGadgetDeltaLine;   
    QVector<QMenu*> sub_menus;
    MySciDoubleSpinBox * sb_axis_min;
    MySciDoubleSpinBox * sb_axis_max;
    QVector<QColor> colors;

    //Appearance --> AppearanceDialog
    AppearanceDialog * appearanceDialog;
    void connectAppearance(AppearanceDialog*);
    void configurePopup();

    QCPItemTracer * createTracer(double cx,QCPGraph * graph);
    MyLabel *createLabel(double cx, double cy);
    QCPItemText * createTextItem(double cx,double cy,QString markstr);
};

#endif // VIEWER1D_H
