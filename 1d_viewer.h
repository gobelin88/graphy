#include <MyCustomPlot.h>
#include <QVector>
#include <iostream>

#include "2d_curve.h"
#include "DataConvert.h"
#include "ColorWheel.hpp"
#include "MyGradientComboBox.h"

#ifndef VIEWER1D_H
#define VIEWER1D_H


class Viewer1D:public QCustomPlot
{
    Q_OBJECT
public:


    Viewer1D(const QMap<QString, QKeySequence>& shortcuts_map, QWidget* parent);
    ~Viewer1D();


    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);
    void addSubMenu(QMenu * sub_menu);

public slots:
    void slot_add_data(const Curve2D& datacurve);

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    //void mouseDoublePress(QMouseEvent* event);

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
    void slot_histogram(Eigen::VectorXd data, QString name, int nbbins);
    void slot_copy();
    void slot_paste();
    void slot_statistiques();
    void slot_svd();
    void slot_covariance();

    QCPItemTracer * createTracer(double cx,QCPGraph * graph);
    void addLabel(double cx, double cy);
    void addTextLabel(double cx,double cy,QString markstr);

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

    //void slot_addXaxisLabel();

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
    unsigned int getId();

    //status
    Qt::AlignmentFlag left_right;
    Qt::AlignmentFlag top_bottom;
    Qt::KeyboardModifiers modifiers;
    QString state_label,state_arrow,state_mark,state_tracer,state_line,state_deltaline;
    QCPItemCurve* arrowItem;
    QCPItemTracer* tracerItem;
    QCPItemLine* lineItem;

    QCPItemLine* deltaLineItemA,*deltaLineItemB;
    QCPItemLine* deltaArrowItem;
    QCPItemText* deltaLabel;

    //Popup
    void createPopup();
    void configurePopup();
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
    QWidget *createParametersWidget();

    MyGradientComboBox * cb_gradient;

    QComboBox* cb_itemLineStyleList;
    QComboBox* cb_ScatterShapes;
    QDoubleSpinBox* sb_ScatterSize;
    QComboBox* cb_penStyle;
    QComboBox* cb_brushStyle;
    ColorWheel* cw_pen_color;
    ColorWheel* cw_brush_color;
    QDoubleSpinBox* sb_penWidth;
    QDoubleSpinBox* s_pen_alpha;
    QDoubleSpinBox* s_brush_alpha;

    QVector<QMenu*> sub_menus;

    QDoubleSpinBox * sb_axis_min;
    QDoubleSpinBox * sb_axis_max;

    QVector<QColor> colors;

    QComboBox * createScatterComboBox();

    QWidget * parameterWidget;
};

#endif // VIEWER1D_H
