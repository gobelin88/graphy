#include "1d_viewer.h"
#include "kdtree_eigen.h"

#include "shapes/Sinusoide.hpp"
#include "shapes/CustomExpression.h"
#include "shapes/Circle2D.h"
#include "shapes/Ellipse2D.h"
#include "shapes/Sigmoid.h"
#include "shapes/Gaussian.h"
#include "shapes/Impedances.hpp"

Viewer1D::Viewer1D(const QMap<QString,QKeySequence>& shortcuts_map, QWidget* parent):QCustomPlot(parent)
{
    colors<<QColor(160,0,0);
    colors<<QColor(0,160,0);
    colors<<QColor(0,0,160);
    colors<<QColor(255,160,0);
    colors<<QColor(255,0,255);
    colors<<QColor(0,160,128);
    colors<<QColor(128,255,0);
    colors<<QColor(128,0,255);
    colors<<QColor(0,128,255);
    colors<<QColor(0,0,0);

    createPopup();

    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                          QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectItems);

    connect(this, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(slot_axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(this, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(slot_legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
    connect(this, SIGNAL(itemDoubleClick(QCPAbstractItem*,QMouseEvent*)), this, SLOT(slot_itemDoubleClick(QCPAbstractItem*,QMouseEvent*)));
    connect(this, SIGNAL(plottableDoubleClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(slot_plottableDoubleClick(QCPAbstractPlottable*,int,QMouseEvent*)));
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    applyShortcuts(shortcuts_map);

    top_bottom=Qt::AlignTop;
    left_right=Qt::AlignRight;

    modifiers=Qt::NoModifier;
    state_label.clear();
    state_arrow.clear();
    state_mark.clear();
    state_tracer.clear();
    state_line.clear();

    deltaArrowItem=nullptr;
    deltaLabel=nullptr;
    arrowItem=nullptr;
    tracerItem=nullptr;
    lineItem=nullptr;
    deltaLineItemA=nullptr;
    deltaLineItemB=nullptr;

    legend->setVisible(true);

//    plotLayout()->elementAt(0)->setMinimumSize(QSize(512,512));
//    plotLayout()->elementAt(0)->setMaximumSize(QSize(512,512));
//    plotLayout()->elementAt(0)->setSizeConstraintRect(QCPLayoutElement::scrInnerRect);
}

Viewer1D::~Viewer1D()
{

}

unsigned int Viewer1D::getId()
{
    unsigned int id=(this->plottableCount()-1)%colors.size();
    return id;
}



void Viewer1D::selectionChanged()
{
    for (int i=0; i<plottableCount(); ++i)
    {
        auto* ptr_plottable = plottable(i);
        QCPPlottableLegendItem* item = legend->itemWithPlottable(ptr_plottable);
        if (item->selected() || ptr_plottable->selected())
        {
            item->setSelected(true);

            QCPGraph* ptr_graph=dynamic_cast<QCPGraph*>(ptr_plottable);
            if (ptr_graph)
            {
                ptr_graph->setSelection(QCPDataSelection(ptr_graph->data()->dataRange()));
            }
            QCPCurve* ptr_curve=dynamic_cast<QCPCurve*>(ptr_plottable);
            if (ptr_curve)
            {
                ptr_curve->setSelection(QCPDataSelection(ptr_curve->data()->dataRange()));
            }

            QCPColorMap* ptr_map=dynamic_cast<QCPColorMap*>(ptr_plottable);
            if (ptr_map)
            {
                ptr_map->setSelection(QCPDataSelection(QCPDataRange(0,ptr_map->data()->keySize())));
            }
        }
    }
}

void Viewer1D::slot_add_data(const Curve2D& datacurve)
{
    if (datacurve.getType()==Curve2D::GRAPH)
    {
        std::cout<<"graph"<<std::endl;
        datacurve.toQCP<QCPGraph>(this);
    }
    else if (datacurve.getType()==Curve2D::CURVE)
    {
        std::cout<<"curve"<<std::endl;
        datacurve.toQCP<QCPCurve>(this);
        axisRect()->setupFullAxesBox();
    }
    else if (datacurve.getType()==Curve2D::MAP)
    {
        std::cout<<"map"<<std::endl;
        datacurve.toQCPMap(this);
    }

    rescaleAxes();
    replot();
}

void Viewer1D::configurePopup()
{
    QList<QCPAbstractPlottable*> plottables=selectedPlottables();

    cb_gradient->hide();

    actSaveMap->setVisible(false);

    if (plottables.size()>0)
    {
        parameterWidget->setWindowTitle("Appearance of : "+plottables[0]->name());

        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(plottables[0]);
        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(plottables[0]);
        QCPColorMap* currentmap=dynamic_cast<QCPColorMap*>(plottables[0]);
        if (currentmap)
        {
            actSaveMap->setVisible(true);
            cb_gradient->setCurrentIndex(currentmap->colorScale()->gradient().getPreset());
            cb_gradient->show();
        }
        else if (currentcurve)
        {
            cb_itemLineStyleList->setCurrentIndex(currentcurve->lineStyle());
            cb_ScatterShapes->setCurrentIndex(static_cast<int>(currentcurve->scatterStyle().shape()));
            sb_ScatterSize->setValue(currentcurve->scatterStyle().size());
            if (currentcurve->getScalarField().size()>0)
            {
                cb_gradient->setCurrentIndex(currentcurve->getScalarFieldGradientType());
                cb_gradient->show();
            }
        }
        else if (currentgraph)
        {
            cb_itemLineStyleList->setCurrentIndex(currentgraph->lineStyle());
            cb_ScatterShapes->setCurrentIndex(static_cast<int>(currentgraph->scatterStyle().shape()));
            sb_ScatterSize->setValue(currentgraph->scatterStyle().size());
            if (currentgraph->getScalarField().size()>0)
            {
                cb_gradient->setCurrentIndex(currentgraph->getScalarFieldGradientType());
                cb_gradient->show();
            }
        }


        s_pen_alpha->setValue(plottables[0]->pen().color().alphaF());
        s_brush_alpha->setValue(plottables[0]->brush().color().alphaF());
        cb_brushStyle->setCurrentIndex(plottables[0]->brush().style());
        cb_penStyle->setCurrentIndex(plottables[0]->pen().style()-1);
        cw_pen_color->setColor(plottables[0]->pen().color());
        cw_brush_color->setColor(plottables[0]->brush().color());
        sb_penWidth->setValue(plottables[0]->pen().widthF());

    }
}

QComboBox * Viewer1D::createScatterComboBox()
{
    QComboBox * comboBox=new QComboBox;
    comboBox->addItem(QStringLiteral("ssNone"),             int(QCPScatterStyle::ScatterShape::ssNone));
    comboBox->addItem(QStringLiteral("ssDot"),              int(QCPScatterStyle::ScatterShape::ssDot));
    comboBox->addItem(QStringLiteral("ssCross"),            int(QCPScatterStyle::ScatterShape::ssCross));
    comboBox->addItem(QStringLiteral("ssPlus"),             int(QCPScatterStyle::ScatterShape::ssPlus));
    comboBox->addItem(QStringLiteral("ssCircle"),           int(QCPScatterStyle::ScatterShape::ssCircle));
    comboBox->addItem(QStringLiteral("ssDisc"),             int(QCPScatterStyle::ScatterShape::ssDisc));
    comboBox->addItem(QStringLiteral("ssSquare"),           int(QCPScatterStyle::ScatterShape::ssSquare));
    comboBox->addItem(QStringLiteral("ssDiamond"),          int(QCPScatterStyle::ScatterShape::ssDiamond));
    comboBox->addItem(QStringLiteral("ssStar"),             int(QCPScatterStyle::ScatterShape::ssStar));
    comboBox->addItem(QStringLiteral("ssTriangle"),         int(QCPScatterStyle::ScatterShape::ssTriangle));
    comboBox->addItem(QStringLiteral("ssTriangleInverted"), int(QCPScatterStyle::ScatterShape::ssTriangleInverted));
    comboBox->addItem(QStringLiteral("ssCrossSquare"),      int(QCPScatterStyle::ScatterShape::ssCrossSquare));
    comboBox->addItem(QStringLiteral("ssPlusSquare"),       int(QCPScatterStyle::ScatterShape::ssPlusSquare));
    comboBox->addItem(QStringLiteral("ssCrossCircle"),      int(QCPScatterStyle::ScatterShape::ssCrossCircle));
    comboBox->addItem(QStringLiteral("ssPlusCircle"),       int(QCPScatterStyle::ScatterShape::ssPlusCircle));
    comboBox->addItem(QStringLiteral("ssPeace"),            int(QCPScatterStyle::ScatterShape::ssPeace));
    comboBox->addItem(QStringLiteral("ssArrow"),            int(QCPScatterStyle::ScatterShape::ssArrow));
    return comboBox;
}

QWidget* Viewer1D::createParametersWidget()
{

    QWidget* widget=new QWidget;
    widget->setMinimumWidth(400);

    QGridLayout* gbox = new QGridLayout();

    cb_itemLineStyleList = new QComboBox;
    cb_itemLineStyleList->addItem(QStringLiteral("lsNone"),        int(QCPGraph::LineStyle::lsNone));
    cb_itemLineStyleList->addItem(QStringLiteral("lsLine"),        int(QCPGraph::LineStyle::lsLine));
    cb_itemLineStyleList->addItem(QStringLiteral("lsStepLeft"),    int(QCPGraph::LineStyle::lsStepLeft));
    cb_itemLineStyleList->addItem(QStringLiteral("lsStepRight"),   int(QCPGraph::LineStyle::lsStepRight));
    cb_itemLineStyleList->addItem(QStringLiteral("lsStepCenter"),  int(QCPGraph::LineStyle::lsStepCenter));
    cb_itemLineStyleList->addItem(QStringLiteral("lsImpulse"),     int(QCPGraph::LineStyle::lsImpulse));

    cb_ScatterShapes = createScatterComboBox();
    sb_ScatterSize=new QDoubleSpinBox;
    sb_ScatterSize->setRange(1,100);

    cb_penStyle = new QComboBox;
    cb_penStyle->addItem(QStringLiteral("SolidLine"));
    cb_penStyle->addItem(QStringLiteral("DashLine"));
    cb_penStyle->addItem(QStringLiteral("DotLine"));
    cb_penStyle->addItem(QStringLiteral("DashDotLine"));
    cb_penStyle->addItem(QStringLiteral("DashDotDotLine"));
    cb_penStyle->addItem(QStringLiteral("CustomDashLine"));

    cb_brushStyle = new QComboBox;
    cb_brushStyle->addItem(QStringLiteral("NoBrush"));
    cb_brushStyle->addItem(QStringLiteral("SolidPattern"));
    cb_brushStyle->addItem(QStringLiteral("Dense1Pattern"));
    cb_brushStyle->addItem(QStringLiteral("Dense2Pattern"));
    cb_brushStyle->addItem(QStringLiteral("Dense3Pattern"));
    cb_brushStyle->addItem(QStringLiteral("Dense4Pattern"));
    cb_brushStyle->addItem(QStringLiteral("Dense5Pattern"));
    cb_brushStyle->addItem(QStringLiteral("Dense6Pattern"));
    cb_brushStyle->addItem(QStringLiteral("Dense7Pattern"));
    cb_brushStyle->addItem(QStringLiteral("HorPattern"));
    cb_brushStyle->addItem(QStringLiteral("VerPattern"));
    cb_brushStyle->addItem(QStringLiteral("CrossPattern"));
    cb_brushStyle->addItem(QStringLiteral("BDiagPattern"));
    cb_brushStyle->addItem(QStringLiteral("DiagCrossPattern"));

    //QPushButton* pb_pen_color=new  QPushButton("Pen");
    cw_pen_color = new ColorWheel;
    cw_brush_color = new ColorWheel;
    //cd_pen_color->setOptions(QColorDialog::DontUseNativeDialog| QColorDialog::NoButtons);

    cb_gradient=new MyGradientComboBox(nullptr);

    sb_penWidth=new QDoubleSpinBox();
    s_pen_alpha=new QDoubleSpinBox();
    s_pen_alpha->setRange(0,1.0);
    s_pen_alpha->setSingleStep(0.1);
    s_pen_alpha->setPrefix("alpha=");
    s_brush_alpha=new QDoubleSpinBox();
    s_brush_alpha->setRange(0,1.0);
    s_brush_alpha->setSingleStep(0.1);
    s_brush_alpha->setPrefix("alpha=");

    QGridLayout* g_styleCurve = new QGridLayout();
    QGroupBox* gb_styleCurve=new QGroupBox("Curve Style");
    gb_styleCurve->setLayout(g_styleCurve);

    g_styleCurve->addWidget(new QLabel("Scatter shape : "),0,0);
    g_styleCurve->addWidget(new QLabel("Scatter size : "),1,0);
    g_styleCurve->addWidget(new QLabel("Line style : "),2,0);
    g_styleCurve->addWidget(new QLabel("Pen style : "),3,0);
    g_styleCurve->addWidget(new QLabel("Pen size : "),4,0);
    g_styleCurve->addWidget(new QLabel("Brush style : "),5,0);

    g_styleCurve->addWidget(cb_ScatterShapes,0,1);
    g_styleCurve->addWidget(sb_ScatterSize,1,1);
    g_styleCurve->addWidget(cb_itemLineStyleList,2,1);
    g_styleCurve->addWidget(cb_penStyle,3,1);
    g_styleCurve->addWidget(sb_penWidth,4,1);
    g_styleCurve->addWidget(cb_brushStyle,5,1);

    QLabel* l_pen_color=new QLabel("Pen color");
    QLabel* l_brush_color=new QLabel("Brush color");
    l_pen_color->setAlignment(Qt::AlignHCenter);
    l_brush_color->setAlignment(Qt::AlignHCenter);

    g_styleCurve->addWidget(l_pen_color,6,0);
    g_styleCurve->addWidget(l_brush_color,6,1);
    g_styleCurve->addWidget(cw_pen_color,7,0);
    g_styleCurve->addWidget(cw_brush_color,7,1);
    g_styleCurve->addWidget(s_pen_alpha,8,0);
    g_styleCurve->addWidget(s_brush_alpha,8,1);
    g_styleCurve->addWidget(cb_gradient,9,0,1,2);

    //gbox->addWidget(gb_axis,0,0);
    gbox->addWidget(gb_styleCurve,0,0);
    widget->setLayout(gbox);

    QObject::connect(s_pen_alpha, SIGNAL(valueChanged(double)), this, SLOT(slot_setPenAlpha(double)));
    QObject::connect(s_brush_alpha, SIGNAL(valueChanged(double)), this, SLOT(slot_setBrushAlpha(double)));

    QObject::connect(cw_brush_color, SIGNAL(colorChanged(QColor)), this, SLOT(slot_setBrushColor(QColor)));
    QObject::connect(sb_penWidth, SIGNAL(valueChanged(double)), this, SLOT(slot_setPenWidth(double)));
    QObject::connect(cw_pen_color, SIGNAL(colorChanged(QColor)), this, SLOT(slot_setPenColor(QColor)));
    QObject::connect(cb_penStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setPenStyle(int)));
    QObject::connect(cb_brushStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setBrushStyle(int)));
    QObject::connect(cb_ScatterShapes, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setScatterShape(int)));
    QObject::connect(sb_ScatterSize, SIGNAL(valueChanged(double) ), this, SLOT(slot_setScatterSize(double)));
    QObject::connect(cb_itemLineStyleList, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setStyle(int)));

    QObject::connect(cb_gradient, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setScalarFieldGradientType(int)));

    return widget;
}

void Viewer1D::createPopup()
{
    popup_menu=new QMenu(this);
    menuGadgets=new QMenu("Gadgets",this);
    menuFit=new QMenu("Fit",this);
    menuLegend=new QMenu("Legend",this);
    menuScalarField=new QMenu("Scalar Field",this);
    menuScalarFieldFit=new QMenu("Fit",this);
    menuAppearance=new QMenu("Appearance",this);
    menuAnalyse=new QMenu("Analyse",this);
    menuFilters=new QMenu("Filters",this);
    menuMisc=new QMenu("Miscellaneous",this);
    menuThemes=new QMenu("Themes",this);

    actCopy   = new QAction("Copy",  this);
    actPaste   = new QAction("Paste",  this);
    actSave   = new QAction("Save",  this);
    actSaveMap= new QAction("Save Map",  this);
    actRescale= new QAction("Rescale",  this);
    actDelete= new QAction("Delete",  this);
    actClearGadgets   = new QAction("Clear gadgets",  this);
    actGadgetArrow= new QAction("Arrow",  this);
    actGadgetText= new QAction("Text",  this);
    actGadgetMark= new QAction("Mark",  this);
    actGadgetTracer= new QAction("Tracer",  this);
    actGadgetLine= new QAction("Line",  this);
    actGadgetDeltaLine= new QAction("Delta Line",  this);
    actLegendShowHide= new QAction("Hide",  this);
    actLegendTopBottom= new QAction("Move bottom",  this);
    actLegendLeftRight= new QAction("Move left",  this);
    actStatistiques= new QAction("Statistiques",  this);
    actSvd= new QAction("Singular values decomposition (SVD)",  this);
    actCovariance= new QAction("Covariance matrix",  this);
    actFitPolynomial= new QAction("Polynomial",  this);
    actFitGaussian= new QAction("Gaussian",  this);
    actFitSigmoid= new QAction("Sigmoid",  this);
    actFitSinusoide= new QAction("Sinusoide",  this);
    actFitCircle= new QAction("Circle",  this);
    actFitEllipse= new QAction("Ellipse",  this);
    actFitCustomExp= new QAction("Custom Expression",  this);

    actFitPolynomial2V= new QAction("Polynomial in two variables",  this);
    actDistance= new QAction("K-nearest neighbors distances",  this);


    actAutoColor1= new QAction("Theme 1 : Colors"                 ,  this);
    actAutoColor2= new QAction("Theme 2 : Colors pairs "          ,  this);
    actAutoColor3= new QAction("Theme 3",  this);
    actAutoColor4= new QAction("Theme 4",  this);
    actAutoColor5= new QAction("Theme 5",  this);
    actAutoColorClear= new QAction("Clear",  this);

    actFilterMedian=new QAction("Median",this);
    actFilterMean=new QAction("Mean",this);

    actDecreasePenWidth=new QAction("Decrease pen width",this);
    actIncreasePenWidth=new QAction("Increase pen width",this);
    actSetScatters=new QAction("Set scatters",this);

    actAppearance=new QAction("Appearance",this);

    this->addAction(actDecreasePenWidth);
    this->addAction(actIncreasePenWidth);
    this->addAction(actSetScatters);

    this->addAction(actCopy);
    this->addAction(actPaste);
    this->addAction(actSaveMap);
    this->addAction(actSave);
    this->addAction(actRescale);
    this->addAction(actDelete);
    this->addAction(actClearGadgets);
    this->addAction(actGadgetArrow);
    this->addAction(actGadgetText);
    this->addAction(actGadgetMark);
    this->addAction(actGadgetTracer);
    this->addAction(actGadgetLine);
    this->addAction(actGadgetDeltaLine);
    this->addAction(actLegendShowHide);
    this->addAction(actLegendTopBottom);
    this->addAction(actLegendLeftRight);
    this->addAction(actStatistiques);
    this->addAction(actAutoColor1);
    this->addAction(actAutoColor2);
    this->addAction(actAutoColor3);
    this->addAction(actAutoColor4);
    this->addAction(actAutoColor5);
    this->addAction(actAutoColorClear);
    this->addAction(actAppearance);

    auto customContainerActions=this->actions();
    for(auto act:customContainerActions)
    {
        act->setShortcutVisibleInContextMenu(true);
    }

    actLegendShowHide->setCheckable(true);
    actLegendShowHide->setChecked(true);
    actLegendTopBottom->setCheckable(true);
    actLegendTopBottom->setChecked(true);
    actLegendLeftRight->setCheckable(true);
    actLegendLeftRight->setChecked(true);

    popup_menu->addMenu(menuAppearance);
    popup_menu->addSeparator();
    popup_menu->addAction(actSave);
    popup_menu->addAction(actSaveMap);
    popup_menu->addAction(actCopy);
    popup_menu->addAction(actPaste);
    popup_menu->addAction(actDelete);
    popup_menu->addAction(actRescale);
    popup_menu->addSeparator();
    popup_menu->addMenu(menuGadgets);
    popup_menu->addSeparator();
    popup_menu->addMenu(menuAnalyse);
    popup_menu->addSeparator();

    menuFilters->addAction(actFilterMedian);
    menuFilters->addAction(actFilterMean);

    menuGadgets->addAction(actGadgetArrow);
    menuGadgets->addAction(actGadgetMark);
    menuGadgets->addAction(actGadgetTracer);
    menuGadgets->addAction(actGadgetLine);
    menuGadgets->addAction(actGadgetDeltaLine);
    menuGadgets->addAction(actGadgetText);
    menuGadgets->addSeparator();
    menuGadgets->addAction(actClearGadgets);

    menuFit->addAction(actFitPolynomial);
    menuFit->addAction(actFitGaussian);
    menuFit->addAction(actFitSigmoid);
    menuFit->addAction(actFitSinusoide);
    menuFit->addAction(actFitCircle);
    menuFit->addAction(actFitEllipse);
    menuFit->addSeparator();
    menuFit->addAction(actFitCustomExp);


    menuLegend->addAction(actLegendShowHide);
    menuLegend->addAction(actLegendTopBottom);
    menuLegend->addAction(actLegendLeftRight);

    menuScalarField->addMenu(menuScalarFieldFit);
    menuScalarField->addAction(actDistance);
    menuScalarFieldFit->addAction(actFitPolynomial2V);

    menuAppearance->addAction(actAppearance);
    menuAppearance->addMenu(menuThemes);
    menuThemes->addAction(actAutoColor1);
    menuThemes->addAction(actAutoColor2);
    menuThemes->addAction(actAutoColor3);
    menuThemes->addAction(actAutoColor4);
    menuThemes->addAction(actAutoColor5);
    menuThemes->addAction(actAutoColorClear);
    menuThemes->addSeparator();
    menuThemes->addAction(actIncreasePenWidth);
    menuThemes->addAction(actDecreasePenWidth);
    menuThemes->addSeparator();
    menuThemes->addAction(actSetScatters);
    menuAppearance->addMenu(menuLegend);

    parameterWidget=createParametersWidget();

    //QWidgetAction* actWidget=new QWidgetAction(popup_menu);
    //actWidget->setDefaultWidget(parameterWidget);
    //menuAppearance->addAction(actWidget);

    menuAnalyse->addMenu(menuFit);
    menuAnalyse->addMenu(menuScalarField);
    menuAnalyse->addMenu(menuFilters);
    menuAnalyse->addMenu(menuMisc);
    menuMisc->addAction(actStatistiques);
    menuMisc->addAction(actSvd);
    menuMisc->addAction(actCovariance);



    connect(actIncreasePenWidth,SIGNAL(triggered()),this,SLOT(slot_increasePenWidth()));
    connect(actDecreasePenWidth,SIGNAL(triggered()),this,SLOT(slot_decreasePenWidth()));

    connect(actGadgetDeltaLine,SIGNAL(triggered()),this,SLOT(slot_gadgetDeltaLine()));
    connect(actGadgetLine,SIGNAL(triggered()),this,SLOT(slot_gadgetLine()));
    connect(actGadgetTracer,SIGNAL(triggered()),this,SLOT(slot_gadgetTracer()));
    connect(actGadgetMark,SIGNAL(triggered()),this,SLOT(slot_gadgetMark()));
    connect(actGadgetText,SIGNAL(triggered()),this,SLOT(slot_gadgetText()));
    connect(actGadgetArrow,SIGNAL(triggered()),this,SLOT(slot_gadgetArrow()));
    connect(actClearGadgets,SIGNAL(triggered()),this,SLOT(slot_clear_marks()));
    connect(actStatistiques,SIGNAL(triggered()),this,SLOT(slot_statistiques()));
    connect(actSvd,SIGNAL(triggered()),this,SLOT(slot_svd()));
    connect(actCovariance,SIGNAL(triggered()),this,SLOT(slot_covariance()));

    connect(actDistance,SIGNAL(triggered()),this,SLOT(slot_Distance()));

    connect(actSaveMap,SIGNAL(triggered()),this,SLOT(slot_save_image_current_map()));
    connect(actSave,SIGNAL(triggered()),this,SLOT(slot_save_image()));
    connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_rescale()));
    connect(actFitPolynomial,SIGNAL(triggered()),this,SLOT(slot_fit_polynomial()));
    connect(actFitPolynomial2V,SIGNAL(triggered()),this,SLOT(slot_fit_2var_polynomial()));
    connect(actFitGaussian,SIGNAL(triggered()),this,SLOT(slot_fit_gaussian()));
    connect(actFitSinusoide,SIGNAL(triggered()),this,SLOT(slot_fit_sinusoide()));
    connect(actFitSigmoid,SIGNAL(triggered()),this,SLOT(slot_fit_sigmoid()));
    connect(actFitCircle,SIGNAL(triggered()),this,SLOT(slot_fit_circle()));
    connect(actFitEllipse,SIGNAL(triggered()),this,SLOT(slot_fit_ellipse()));
    connect(actFitCustomExp,SIGNAL(triggered()),this,SLOT(slot_fit_custom()));

    connect(actCopy,SIGNAL(triggered()),this,SLOT(slot_copy()));
    connect(actPaste,SIGNAL(triggered()),this,SLOT(slot_paste()));
    connect(actDelete,SIGNAL(triggered()),this,SLOT(slot_delete()));
    connect(actLegendShowHide,SIGNAL(toggled(bool)),this,SLOT(slot_show_legend(bool)));
    connect(actLegendTopBottom,SIGNAL(toggled(bool)),this,SLOT(slot_top_legend(bool)));
    connect(actLegendLeftRight,SIGNAL(toggled(bool)),this,SLOT(slot_left_legend(bool)));
    connect(actAutoColor1,SIGNAL(triggered()),this,SLOT(slot_auto_color1()));
    connect(actAutoColor2,SIGNAL(triggered()),this,SLOT(slot_auto_color2()));
    connect(actAutoColor3,SIGNAL(triggered()),this,SLOT(slot_auto_color3()));
    connect(actAutoColor4,SIGNAL(triggered()),this,SLOT(slot_auto_color4()));
    connect(actAutoColor5,SIGNAL(triggered()),this,SLOT(slot_auto_color5()));
    connect(actAutoColorClear,SIGNAL(triggered()),this,SLOT(slot_auto_clear()));
    connect(actSetScatters,SIGNAL(triggered()),this,SLOT(slot_setScatters()));

    connect(actFilterMean,SIGNAL(triggered()),this,SLOT(slot_meanFilter()));
    connect(actFilterMedian,SIGNAL(triggered()),this,SLOT(slot_medianFilter()));
    connect(actAppearance,SIGNAL(triggered()),this,SLOT(slot_appearance()));


}

void Viewer1D::slot_auto_color1()//Diff colors
{
    QList<QCPAbstractPlottable*> plottables=this->plottables();

    for (int i=0; i<plottables.size(); i++)
    {
        int index=i%colors.size();

        //Pen
        QPen pen=plottables[i]->pen();
        pen.setStyle(Qt::SolidLine);
        colors[index].setAlphaF(1.0);
        pen.setColor(colors[index]);

        //Brush
        QBrush brush;
        brush.setStyle(Qt::NoBrush);

        plottables[i]->setPen(pen);
        plottables[i]->setBrush(brush);
    }

    replot();
}

void Viewer1D::slot_auto_color2()//diff color pair
{
    QList<QCPAbstractPlottable*> plottables=this->plottables();

    for (int i=0; i<plottables.size(); i++)
    {
        int index=(i/2)%colors.size();

        //Pen
        QPen pen=plottables[i]->pen();
        colors[index].setAlphaF(1.0);
        pen.setColor(colors[index]);
        if(i%2==1){pen.setStyle(Qt::DotLine);}else{pen.setStyle(Qt::SolidLine);}

        //Brush
        QBrush brush;
        brush.setStyle(Qt::NoBrush);

        plottables[i]->setPen(pen);
        plottables[i]->setBrush(brush);
    }

    replot();
}

void Viewer1D::slot_auto_color3()//brush diag
{
    QList<QCPAbstractPlottable*> plottables=this->plottables();

    for (int i=0; i<plottables.size(); i++)
    {
        int index=i%colors.size();

        //Pen
        QPen pen=plottables[i]->pen();
        pen.setStyle(Qt::SolidLine);
        pen.setColor(colors[index]);

        //Brush
        QBrush brush=plottables[i]->brush();
        brush.setStyle(Qt::BDiagPattern);
        colors[index].setAlphaF(0.5);
        brush.setColor(colors[index]);

        plottables[i]->setPen(pen);
        plottables[i]->setBrush(brush);
    }

    replot();
}

void Viewer1D::slot_auto_color4()//brush solid
{
    QList<QCPAbstractPlottable*> plottables=this->plottables();

    for (int i=0; i<plottables.size(); i++)
    {
        int index=i%colors.size();

        //Pen
        QPen pen=plottables[i]->pen();
        pen.setStyle(Qt::SolidLine);
        pen.setColor(Qt::black);

        //Brush
        QBrush brush=plottables[i]->brush();
        brush.setStyle(Qt::SolidPattern);
        colors[index].setAlphaF(0.5);
        brush.setColor(colors[index]);

        plottables[i]->setPen(pen);
        plottables[i]->setBrush(brush);
    }

    replot();
}

void Viewer1D::slot_auto_color5()
{
    QList<QCPAbstractPlottable*> plottables=this->plottables();

    for (int i=0; i<plottables.size(); i++)
    {
        int index=i%colors.size();

        //Pen
        QPen pen=plottables[i]->pen();
        pen.setStyle(Qt::DashLine);
        pen.setColor(colors[index]);

        //Brush
        QBrush brush=plottables[i]->brush();
        brush.setStyle(Qt::NoBrush);
        brush.setColor(colors[index]);

        plottables[i]->setPen(pen);
        plottables[i]->setBrush(brush);
    }

    replot();
}

void Viewer1D::slot_setScatters()
{
    QVector<int> scattersList=getScattersList();
    QVector<int> scattersSizeList=getScattersSizeList();

    if(scattersList.size()>0 && scattersSizeList.size()>0)
    {
        QDialog* dialog=new QDialog;
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        QComboBox * cb_scattersShape=createScatterComboBox();
        cb_scattersShape->setCurrentIndex(scattersList[0]);
        QObject::connect(cb_scattersShape, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setScattersShape(int)));

        QSpinBox * sb_scattersSize=new QSpinBox();
        sb_scattersSize->setValue(scattersSizeList[0]);
        QObject::connect(sb_scattersSize, SIGNAL(valueChanged(int)), this, SLOT(slot_setScattersSize(int)));

        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Set scatters");
        QGridLayout* gbox = new QGridLayout();
        gbox->addWidget(cb_scattersShape,0,0);
        gbox->addWidget(sb_scattersSize,0,1);
        gbox->addWidget(buttonBox,1,0);
        dialog->setLayout(gbox);

        int result=dialog->exec();
        if (result != QDialog::Accepted)
        {
            setScattersList(scattersList);
            setScattersSizeList(scattersSizeList);
            replot();
        }
    }
}

void Viewer1D::slot_auto_clear()
{
    QList<QCPAbstractPlottable*> plottables=this->plottables();

    for (int i=0; i<plottables.size(); i++)
    {
        //Pen
        QPen pen=plottables[i]->pen();
        pen.setStyle(Qt::SolidLine);
        pen.setColor(Qt::black);

        //Brush
        QBrush brush=plottables[i]->brush();
        brush.setStyle(Qt::NoBrush);
        brush.setColor(Qt::black);

        plottables[i]->setPen(pen);
        plottables[i]->setBrush(brush);
    }

    replot();
}


void Viewer1D::slot_setStyle(int style)
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();
    QList<QCPCurve*> curveslist=getQCPListOf<QCPCurve>(true);

    for (int i=0; i<graphslist.size(); i++)
    {
        graphslist[i]->setLineStyle( QCPGraph::LineStyle (style) );
    }
    for (int i=0; i<curveslist.size(); i++)
    {
        curveslist[i]->setLineStyle( QCPCurve::LineStyle (style) );
    }
    replot();
}

QVector<int> Viewer1D::getScattersList()
{
    QList<QCPAbstractPlottable*> list=this->getCurvesOrGraphs(false);
    QVector<int> scattersList(list.size());
    for (int i=0; i<list.size(); i++)
    {
        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(list[i]);
        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(list[i]);

        if(currentcurve)
        {
            scattersList[i]=currentcurve->scatterStyle().shape();
        }
        else if(currentgraph)
        {
            scattersList[i]=currentgraph->scatterStyle().shape();
        }
    }
    return scattersList;
}

void Viewer1D::setScattersList(QVector<int> scattersList)
{
    QList<QCPAbstractPlottable*> list=this->getCurvesOrGraphs(false);

    if(scattersList.size()==list.size())
    {
        for (int i=0; i<list.size(); i++)
        {
            QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(list[i]);
            QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(list[i]);

            QCPScatterStyle::ScatterShape scatter_shape=QCPScatterStyle::ScatterShape(scattersList[i]);

            if(currentcurve)
            {
                currentcurve->setScatterStyle( QCPScatterStyle(scatter_shape,currentcurve->scatterStyle().size()) );
            }
            else if(currentgraph)
            {
                currentgraph->setScatterStyle( QCPScatterStyle(scatter_shape,currentgraph->scatterStyle().size()) );
            }
        }
    }
}

QVector<int> Viewer1D::getScattersSizeList()
{
    QList<QCPAbstractPlottable*> list=this->getCurvesOrGraphs(false);
    QVector<int> scattersSizeList(list.size());
    for (int i=0; i<list.size(); i++)
    {
        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(list[i]);
        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(list[i]);

        if(currentcurve)
        {
            scattersSizeList[i]=currentcurve->scatterStyle().size();
        }
        else if(currentgraph)
        {
            scattersSizeList[i]=currentgraph->scatterStyle().size();
        }
    }
    return scattersSizeList;
}

void Viewer1D::setScattersSizeList(QVector<int> scattersSizeList)
{
    QList<QCPAbstractPlottable*> list=this->getCurvesOrGraphs(false);

    if(scattersSizeList.size()==list.size())
    {
        for (int i=0; i<list.size(); i++)
        {
            QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(list[i]);
            QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(list[i]);


            if(currentcurve)
            {
                currentcurve->setScatterStyle( QCPScatterStyle(currentcurve->scatterStyle().shape(),scattersSizeList[i]) );
            }
            else if(currentgraph)
            {
                currentgraph->setScatterStyle( QCPScatterStyle(currentgraph->scatterStyle().shape(),scattersSizeList[i]) );
            }
        }
    }
}

void Viewer1D::slot_setScattersShape(int scatter_shapei)
{
    QList<QCPAbstractPlottable*> list=this->getCurvesOrGraphs(false);
    QCPScatterStyle::ScatterShape scatter_shape=QCPScatterStyle::ScatterShape(scatter_shapei);
    for (int i=0; i<list.size(); i++)
    {
        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(list[i]);
        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(list[i]);

        if(currentcurve)
        {
            currentcurve->setScatterStyle( QCPScatterStyle(scatter_shape,currentcurve->scatterStyle().size()) );
        }
        else if(currentgraph)
        {
            currentgraph->setScatterStyle( QCPScatterStyle(scatter_shape,currentgraph->scatterStyle().size()) );
        }
    }
    replot();
}

void Viewer1D::slot_setScattersSize(int scatter_size)
{
    QList<QCPAbstractPlottable*> list=this->getCurvesOrGraphs(false);
    for (int i=0; i<list.size(); i++)
    {
        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(list[i]);
        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(list[i]);

        if(currentcurve)
        {
            currentcurve->setScatterStyle( QCPScatterStyle(currentcurve->scatterStyle().shape(),scatter_size) );
        }
        else if(currentgraph)
        {
            currentgraph->setScatterStyle( QCPScatterStyle(currentgraph->scatterStyle().shape(),scatter_size) );
        }
    }
    replot();
}

void Viewer1D::slot_setScatterShape(int scatter_shape)
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();
    QList<QCPCurve*> curveslist=getQCPListOf<QCPCurve>(true);

    for (int i=0; i<graphslist.size(); i++)
    {
        graphslist[0]->setScatterStyle( QCPScatterStyle(QCPScatterStyle::ScatterShape (scatter_shape),graphslist[0]->scatterStyle().size()) );
    }
    for (int i=0; i<curveslist.size(); i++)
    {
        curveslist[0]->setScatterStyle( QCPScatterStyle(QCPScatterStyle::ScatterShape (scatter_shape),curveslist[0]->scatterStyle().size()) );
    }
    replot();
}

void Viewer1D::slot_setScatterSize(double scatter_size)
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();
    QList<QCPCurve*> curveslist=getQCPListOf<QCPCurve>(true);

    for (int i=0; i<graphslist.size(); i++)
    {
        graphslist[0]->setScatterStyle( QCPScatterStyle(graphslist[0]->scatterStyle().shape(),scatter_size) );
    }
    for (int i=0; i<curveslist.size(); i++)
    {
        curveslist[0]->setScatterStyle( QCPScatterStyle(curveslist[0]->scatterStyle().shape(),scatter_size) );
    }
    replot();
}

void Viewer1D::slot_clear_marks()
{
    std::cout<<"clear items "<<this->itemCount()<<std::endl;
    this->clearItems();

    this->replot();
}

void Viewer1D::slot_gadgetMark()
{
    state_mark = QString("mark");
    this->setCursor(Qt::PointingHandCursor);
}

void Viewer1D::slot_gadgetLine()
{
    state_line = QString("first");

    lineItem= new QCPItemLine(this);
    lineItem->start->setType(QCPItemPosition::ptPlotCoords);
    lineItem->end->setType(QCPItemPosition::ptPlotCoords);
    lineItem->start->setCoords(0,0);
    lineItem->end->setCoords(0,0);

    this->setCursor(Qt::PointingHandCursor);
}

void Viewer1D::slot_gadgetDeltaLine()
{
    if(state_deltaline.isEmpty())
    {
        state_deltaline = QString("first");

        deltaLineItemA= new QCPItemLine(this);
        deltaLineItemA->start->setType(QCPItemPosition::ptPlotCoords);
        deltaLineItemA->end->setType(QCPItemPosition::ptPlotCoords);
        deltaLineItemA->start->setCoords(0,0);
        deltaLineItemA->end->setCoords(0,0);

        deltaLineItemB= new QCPItemLine(this);
        deltaLineItemB->start->setType(QCPItemPosition::ptPlotCoords);
        deltaLineItemB->end->setType(QCPItemPosition::ptPlotCoords);
        deltaLineItemB->start->setCoords(0,0);
        deltaLineItemB->end->setCoords(0,0);

        this->setCursor(Qt::PointingHandCursor);
    }
}

void Viewer1D::slot_gadgetTracer()
{
    QList<QCPGraph*> list=getQCPListOf<QCPGraph>(true);

    if(list.size()>0)
    {
        tracerItem=createTracer(0.0,list[0]);
        tracerLabel=addLabel(0,tracerItem->position->value());

        state_tracer = QString("first");
        this->setCursor(Qt::PointingHandCursor);
    }
}

void Viewer1D::slot_gadgetText()
{
    state_label = QInputDialog::getText(this, "New label", "New label :", QLineEdit::Normal,"");
    this->setCursor(Qt::PointingHandCursor);
}

void Viewer1D::slot_gadgetArrow()
{
    state_arrow = QString("first");
    this->setCursor(Qt::PointingHandCursor);
}

void Viewer1D::slot_gadgetAddArrow(Eigen::Vector2d A,Eigen::Vector2d B)
{
    arrowItem = new QCPItemCurve(this);
    arrowItem->start->setCoords(A.x(), A.y());
    arrowItem->startDir->setCoords(B.x(), B.y());
    arrowItem->endDir->setCoords(A.x(), A.y());
    arrowItem->end->setCoords(B.x(), B.y());
    arrowItem->setHead(QCPLineEnding::esSpikeArrow);
    arrowItem=nullptr;
}

QCPItemTracer * Viewer1D::createTracer(double cx,QCPGraph * graph)
{
    QCPItemTracer *groupTracer = new QCPItemTracer(this);
    groupTracer->setGraph(graph);
    groupTracer->setGraphKey(cx);
    groupTracer->setInterpolating(true);
    groupTracer->setStyle(QCPItemTracer::tsCircle);
    groupTracer->setPen(QPen(Qt::black));
    groupTracer->setBrush(Qt::black);
    groupTracer->setSize(5);

    return groupTracer;
}

MyLabel * Viewer1D::addLabel(double cx, double cy)
{
    return new MyLabel(this,cx,cy);
}

void Viewer1D::addTextLabel(double cx,double cy,QString markstr)
{
    // add the text label at the top:
    QCPItemText* coordtextStr = new QCPItemText(this);
    coordtextStr->position->setType(QCPItemPosition::ptPlotCoords);
    coordtextStr->setPositionAlignment(Qt::AlignLeft|Qt::AlignBottom);
    coordtextStr->position->setCoords(cx, cy); // lower right corner of axis rect
    coordtextStr->setText(markstr);
    coordtextStr->setTextAlignment(Qt::AlignLeft);
    coordtextStr->setFont(QFont(font().family(), 9));
    coordtextStr->setPadding(QMargins(8, 0, 0, 0));

    this->replot();
}

//void Viewer1D::keyPressEvent(QKeyEvent* event)
//{
//    QCustomPlot::keyPressEvent(event);
//    modifiers |= event->modifiers();

//    if (modifiers&Qt::ControlModifier && event->key()==Qt::Key_M)
//    {
//        state_mark = QString("mark");
//        this->setCursor(Qt::PointingHandCursor);
//    }

//    if (modifiers&Qt::ControlModifier && event->key()==Qt::Key_L)
//    {
//        state_label = QInputDialog::getText(this, "New label", "New label :", QLineEdit::Normal,"");
//        this->setCursor(Qt::PointingHandCursor);
//    }

//    if (modifiers&Qt::ControlModifier && event->key()==Qt::Key_F)
//    {
//        state_arrow = QString("first");
//        this->setCursor(Qt::PointingHandCursor);
//    }
//}

//void Viewer1D::keyReleaseEvent(QKeyEvent* event)
//{
//    QCustomPlot::keyReleaseEvent(event);
//    modifiers &= event->modifiers();
//}

void Viewer1D::mouseMoveEvent(QMouseEvent* event)
{
    QCustomPlot::mouseMoveEvent(event);
    double cx=this->xAxis->pixelToCoord(event->x());
    double cy=this->yAxis->pixelToCoord(event->y());

    if (arrowItem)
    {
        if (state_arrow=="second")
        {
            arrowItem->startDir->setCoords(cx, cy);
            arrowItem->endDir->setCoords(cx, cy);
            arrowItem->end->setCoords(cx, cy);
        }
        else if (state_arrow=="third")
        {
            arrowItem->endDir->setCoords(cx, cy);
            arrowItem->end->setCoords(cx, cy);
        }
        else if (state_arrow=="four")
        {
            arrowItem->end->setCoords(cx, cy);
        }
        replot();
    }

    if(tracerItem)
    {
        if(modifiers & Qt::ControlModifier)
        {
            double dx=tracerLabel->ccx+(cx-tracerLabel->ccx)/10;
            tracerItem->setGraphKey(dx);
            tracerLabel->setCoord(dx,tracerItem->position->value());
            replot();
            std::cout<<"dx="<<dx<<std::endl;
        }
        else
        {
            tracerItem->setGraphKey(cx);
            tracerLabel->setCoord(cx,tracerItem->position->value());
            tracerLabel->ccx=cx;
            replot();
            std::cout<<"cx="<<cx<<std::endl;
        }
    }

    if(lineItem)
    {
        double min_x=this->xAxis->range().lower;
        double min_y=this->yAxis->range().lower;
        double max_x=this->xAxis->range().upper;
        double max_y=this->yAxis->range().upper;

        if(modifiers & Qt::ControlModifier)
        {
            lineItem->start->setCoords(min_x,cy);
            lineItem->end->setCoords(max_x,cy);
        }
        else
        {
            lineItem->start->setCoords(cx,min_y);
            lineItem->end->setCoords(cx,max_y);
        }
        replot();
    }

    if(deltaLineItemA && deltaLineItemB)
    {
        double min_x=this->xAxis->range().lower;
        double min_y=this->yAxis->range().lower;
        double max_x=this->xAxis->range().upper;
        double max_y=this->yAxis->range().upper;

        if(modifiers & Qt::ControlModifier)
        {
            if (state_deltaline==QString("first"))
            {
                deltaLineItemA->start->setCoords(min_x,cy);
                deltaLineItemA->end->setCoords(max_x,cy);
                deltaLineItemB->start->setCoords(min_x,cy);
                deltaLineItemB->end->setCoords(max_x,cy);
            }
        }
        else
        {
            if (state_deltaline==QString("first"))
            {
                deltaLineItemA->start->setCoords(cx,min_y);
                deltaLineItemA->end->setCoords(cx,max_y);
                deltaLineItemB->start->setCoords(cx,min_y);
                deltaLineItemB->end->setCoords(cx,max_y);
            }
        }

        if (state_deltaline==QString("second_v"))
        {
            deltaLineItemB->start->setCoords(min_x,cy);
            deltaLineItemB->end->setCoords(max_x,cy);
        }
        else if (state_deltaline==QString("second_h"))
        {
            deltaLineItemB->start->setCoords(cx,min_y);
            deltaLineItemB->end->setCoords(cx,max_y);
        }

        if(deltaArrowItem && deltaLabel)
        {
            if(state_deltaline==QString("second_v"))
            {
                double delta=deltaLineItemA->start->coords().y()-deltaLineItemB->start->coords().y();
                deltaLabel->position->setCoords(cx,deltaLineItemA->start->coords().y()-delta/2);
                deltaLabel->setText(QString("%1").arg(std::abs(delta)));
                deltaArrowItem->start->setCoords(cx, deltaLineItemA->start->coords().y());
                deltaArrowItem->end->setCoords(cx, deltaLineItemB->start->coords().y());
            }
            else
            {
                double delta=deltaLineItemA->start->coords().x()-deltaLineItemB->start->coords().x();
                deltaLabel->position->setCoords(deltaLineItemA->start->coords().x()-delta/2,cy);
                deltaLabel->setText(QString("%1").arg(std::abs(delta)));
                deltaArrowItem->start->setCoords( deltaLineItemA->start->coords().x(),cy);
                deltaArrowItem->end->setCoords( deltaLineItemB->start->coords().x(),cy);
            }
        }

        replot();
    }
}

void Viewer1D::keyPressEvent(QKeyEvent* event)
{
    QCustomPlot::keyPressEvent(event);

    modifiers=event->modifiers();

    std::cout<<modifiers<<" "<<event->modifiers()<<std::endl;
}

void Viewer1D::keyReleaseEvent(QKeyEvent* event)
{
    QCustomPlot::keyReleaseEvent(event);

    modifiers=event->modifiers();

    std::cout<<modifiers<<" "<<event->modifiers()<<std::endl;
}


void Viewer1D::mousePressEvent(QMouseEvent* event)
{
    QCustomPlot::mousePressEvent(event);
    if (event->button() == Qt::RightButton)
    {
        configurePopup();
        popup_menu->exec(mapToGlobal(event->pos()));
    }

    double cx=this->xAxis->pixelToCoord(event->x());
    double cy=this->yAxis->pixelToCoord(event->y());

    if (!state_tracer.isEmpty() && event->button() == Qt::LeftButton)
    {
        //tracerItem->setGraphKey(cx);
        //addLabel(cx,tracerItem->position->value());
        //tracerLabel->setCoord(cx,tracerItem->position->value());
        state_tracer.clear();
        tracerItem=nullptr;
        this->setCursor(Qt::ArrowCursor);
        replot();
    }

    if (!state_line.isEmpty() && event->button() == Qt::LeftButton)
    {
        double min_x=this->xAxis->range().lower;
        double min_y=this->yAxis->range().lower;
        double max_x=this->xAxis->range().upper;
        double max_y=this->yAxis->range().upper;

        if(modifiers & Qt::ControlModifier)
        {
            lineItem->start->setCoords(min_x,cy);
            lineItem->end->setCoords(max_x,cy);
        }
        else
        {
            lineItem->start->setCoords(cx,min_y);
            lineItem->end->setCoords(cx,max_y);
        }

        state_line.clear();
        lineItem=nullptr;
        this->setCursor(Qt::ArrowCursor);
        replot();
    }

    if (!state_deltaline.isEmpty() && event->button() == Qt::LeftButton)
    {
        double min_x=this->xAxis->range().lower;
        double min_y=this->yAxis->range().lower;
        double max_x=this->xAxis->range().upper;
        double max_y=this->yAxis->range().upper;

        if (state_deltaline==QString("first"))
        {
            deltaLabel= new QCPItemText(this);
            deltaLabel->position->setType(QCPItemPosition::ptPlotCoords);
            deltaLabel->setTextAlignment(Qt::AlignCenter);
            deltaLabel->setPositionAlignment(Qt::AlignHCenter|Qt::AlignBottom);

            deltaArrowItem = new QCPItemLine(this);
            deltaArrowItem->setHead(QCPLineEnding::esSpikeArrow);
            deltaArrowItem->setTail(QCPLineEnding::esSpikeArrow);

            if(modifiers & Qt::ControlModifier)
            {
                deltaLabel->setRotation(-90);
                deltaLineItemA->start->setCoords(min_x,cy);
                deltaLineItemA->end->setCoords(max_x,cy);
                deltaArrowItem->start->setCoords(cx, deltaLineItemA->start->coords().y());
                deltaArrowItem->end->setCoords(cx, deltaLineItemB->start->coords().y());

                state_deltaline=QString("second_v");
            }
            else
            {
                deltaLineItemA->start->setCoords(cx,min_y);
                deltaLineItemA->end->setCoords(cx,max_y);
                deltaArrowItem->start->setCoords( deltaLineItemA->start->coords().x(),cy);
                deltaArrowItem->end->setCoords( deltaLineItemB->start->coords().x(),cy);

                state_deltaline=QString("second_h");
            }

            replot();
        }
        else if (state_deltaline.contains("second"))
        {
            if(state_deltaline==QString("second_v"))
            {
                deltaLineItemB->start->setCoords(min_x,cy);
                deltaLineItemB->end->setCoords(max_x,cy);

                deltaArrowItem->start->setCoords(cx, deltaLineItemA->start->coords().y());
                deltaArrowItem->end->setCoords(cx, deltaLineItemB->start->coords().y());
            }
            else if(state_deltaline==QString("second_h"))
            {
                deltaLineItemB->start->setCoords(cx,min_y);
                deltaLineItemB->end->setCoords(cx,max_y);

                deltaArrowItem->start->setCoords( deltaLineItemA->start->coords().x(),cy);
                deltaArrowItem->end->setCoords( deltaLineItemB->start->coords().x(),cy);
            }

            deltaLineItemA=nullptr;
            deltaLineItemB=nullptr;
            deltaArrowItem=nullptr;
            deltaLabel=nullptr;
            state_deltaline.clear();
            this->setCursor(Qt::ArrowCursor);
            replot();
        }

    }


    if (!state_mark.isEmpty() && event->button() == Qt::LeftButton)
    {
        addLabel(cx,cy);

        state_mark.clear();
        this->setCursor(Qt::ArrowCursor);
        replot();
    }

    if (!state_label.isEmpty() && event->button() == Qt::LeftButton)
    {
        addTextLabel(cx,cy,state_label);
        state_label.clear();
        this->setCursor(Qt::ArrowCursor);
        replot();
    }

    if (!state_arrow.isEmpty() && event->button() == Qt::LeftButton)
    {
        if (state_arrow=="first")
        {
            arrowItem = new QCPItemCurve(this);
            arrowItem->start->setCoords(cx, cy);
            arrowItem->startDir->setCoords(cx, cy);
            arrowItem->endDir->setCoords(cx, cy);
            arrowItem->end->setCoords(cx, cy);
            state_arrow=QString("second");
            replot();
        }
        else if (state_arrow=="second")
        {
            arrowItem->startDir->setCoords(cx, cy);
            arrowItem->endDir->setCoords(cx, cy);
            arrowItem->end->setCoords(cx, cy);
            state_arrow=QString("third");
            replot();
        }
        else if (state_arrow=="third")
        {
            arrowItem->endDir->setCoords(cx, cy);
            arrowItem->end->setCoords(cx, cy);
            state_arrow=QString("four");
            arrowItem->setHead(QCPLineEnding::esSpikeArrow);
            replot();
        }
        else if (state_arrow=="four")
        {
            arrowItem->end->setCoords(cx, cy);
            arrowItem=nullptr;
            state_arrow.clear();
            this->setCursor(Qt::ArrowCursor);
            replot();
        }
    }
}

void Viewer1D::slot_SetSbAxisMax_Min(double min)
{
    sb_axis_max->setMinimum(min);
}

void Viewer1D::slot_SetSbAxisMin_Max(double max)
{
    sb_axis_min->setMaximum(max);
}

void Viewer1D::slot_axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part)
{
    // Set an axis label by double clicking on it
    if (part == QCPAxis::spAxisLabel || part == QCPAxis::spAxis || part == QCPAxis::spTickLabels) // only react when the actual axis label is clicked, not tick label or axis backbone
    {

        QDialog* dialog=new QDialog;

        QCPRange currentRange=axis->range();
        QCPAxis::ScaleType currentScaleType=axis->scaleType();
        QString currentLabel=axis->label();
        uint currentPrecision=axis->numberPrecision();
        QString currentNumberFormat=axis->numberFormat();
        double currentLabelRotation=axis->tickLabelRotation();

        QComboBox* cb_scale_mode=new QComboBox(dialog);
        cb_scale_mode->addItem("Linear");
        cb_scale_mode->addItem("Logarithmic");
        cb_scale_mode->setCurrentIndex(currentScaleType==QCPAxis::ScaleType::stLogarithmic);
        sb_axis_min=new   MySciDoubleSpinBox(dialog);
        //sb_axis_min->setDecimals(DBL_MAX_10_EXP + DBL_DIG);
        sb_axis_min->setPrefix("min=");
        sb_axis_min->setRange(-1e100,currentRange.upper);
        sb_axis_min->setValue(currentRange.lower);
        sb_axis_max=new   MySciDoubleSpinBox(dialog);
        //sb_axis_max->setDecimals(DBL_MAX_10_EXP + DBL_DIG);
        sb_axis_max->setPrefix("max=");
        sb_axis_max->setRange(currentRange.lower,1e100);
        sb_axis_max->setValue(currentRange.upper);
        QLineEdit * le_format=new QLineEdit(currentNumberFormat,dialog);
        le_format->setToolTip("gb : If number is small, fixed format is used, if number is large, scientific format is used with beautifully typeset decimal powers and a dot as multiplication sign\n"\
                              "ebc : All numbers are in scientific format with beautifully typeset decimal power and a cross multiplication sign\n"\
                              "g : normal format code behaviour. If number is small, fixed format is used, if number is large, normal scientific format is used\n"\
                              "f : floating point");

        QObject::connect(le_format,  SIGNAL(textChanged(QString)), axis, SLOT(setNumberFormat(QString)));
        QObject::connect(le_format,  SIGNAL(textChanged(QString)), this, SLOT(replot()));

        QSpinBox * sb_precision=new QSpinBox(dialog);
        sb_precision->setValue(currentPrecision);

        QDoubleSpinBox * sb_rotation=new QDoubleSpinBox(dialog);
        sb_rotation->setValue(currentLabelRotation);
        sb_rotation->setRange(-90,90);

        QCheckBox * cb_subGrid=new QCheckBox("Show sub-grid");
        cb_subGrid->setChecked(axis->grid()->subGridVisible());
        QObject::connect(sb_rotation,  SIGNAL(valueChanged(double)), axis, SLOT(setTickLabelRotation(double)));
        QObject::connect(sb_rotation,  SIGNAL(valueChanged(double)), this, SLOT(replot()));
        QObject::connect(sb_precision,  SIGNAL(valueChanged(int)), axis, SLOT(setNumberPrecision(int)));
        QObject::connect(sb_precision,  SIGNAL(valueChanged(int)), this, SLOT(replot()));
        QObject::connect(sb_axis_min,  SIGNAL(valueChanged(double)), axis, SLOT(setRangeLower(double)));
        QObject::connect(sb_axis_max,  SIGNAL(valueChanged(double)), axis, SLOT(setRangeUpper(double)));
        QObject::connect(sb_axis_min,  SIGNAL(valueChanged(double)), this, SLOT(slot_SetSbAxisMax_Min(double)));
        QObject::connect(sb_axis_max,  SIGNAL(valueChanged(double)), this, SLOT(slot_SetSbAxisMin_Max(double)));
        QObject::connect(cb_scale_mode,SIGNAL(currentIndexChanged(int)), axis, SLOT(setScaleType(int)));
        QObject::connect(sb_axis_min,  SIGNAL(valueChanged(double)), this, SLOT(replot()));
        QObject::connect(sb_axis_max,  SIGNAL(valueChanged(double)), this, SLOT(replot()));
        QObject::connect(cb_scale_mode,SIGNAL(currentIndexChanged(int)), this, SLOT(replot()));
        QObject::connect(cb_subGrid,SIGNAL(stateChanged(int)), axis, SLOT(setSubGridVisible(int)));
        QObject::connect(cb_subGrid,SIGNAL(stateChanged(int)), this, SLOT(replot()));

        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Axis");
        QGridLayout* gbox = new QGridLayout();

        QLineEdit * le_legend=new QLineEdit(currentLabel,dialog);
        QObject::connect(le_legend,  SIGNAL(textChanged(QString)), axis, SLOT(setLabel(QString)));
        QObject::connect(le_legend,  SIGNAL(textChanged(QString)), this, SLOT(replot()));

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(new QLabel("Label :"),0,0);
        gbox->addWidget(le_legend,0,1,1,2);

        gbox->addWidget(new QLabel("Range :"),1,0);
        gbox->addWidget(sb_axis_min,1,1);
        gbox->addWidget(sb_axis_max,1,2);

        gbox->addWidget(new QLabel("Scale type :"),2,0);
        gbox->addWidget(cb_scale_mode,2,1,1,2);

        gbox->addWidget(new QLabel("Format :"),3,0);
        gbox->addWidget(le_format,3,1,1,2);

        gbox->addWidget(new QLabel("Precision :"),4,0);
        gbox->addWidget(sb_precision,4,1,1,2);

        gbox->addWidget(new QLabel("Label rotation :"),5,0);
        gbox->addWidget(sb_rotation,5,1,1,2);

        gbox->addWidget(cb_subGrid,6,0,1,3);

        gbox->addWidget(buttonBox,7,0,1,3);

//        if(dynamic_cast<QCPColorScale*>(axis))
//        {
//            MyGradientComboBox * cb_gradient2=new MyGradientComboBox(this);
//            gbox->addWidget(cb_gradient2,8,0,1,3);
//            QObject::connect(cb_gradient2, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setScalarFieldGradientType(int)));
//        }


        dialog->setLayout(gbox);

        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            replot();
        }
        else
        {
            axis->setTickLabelRotation(currentLabelRotation);
            axis->setNumberFormat(currentNumberFormat);
            sb_precision->setValue(currentPrecision);
            axis->setLabel(currentLabel);
            axis->setScaleType(currentScaleType);
            axis->setRange(currentRange);
            replot();
        }
    }
}

void Viewer1D::slot_plottableDoubleClick(QCPAbstractPlottable* plottable, int n, QMouseEvent* event)
{
    Q_UNUSED(plottable);
    Q_UNUSED(event);
    Q_UNUSED(n);

    //parameterWidget->setWindowFlags(Qt::WindowStaysOnTopHint);
    configurePopup();
    parameterWidget->raise();
    parameterWidget->show();
}

void Viewer1D::slot_itemDoubleClick(QCPAbstractItem* item,QMouseEvent* event)
{
    Q_UNUSED(event);



    //If text
    QCPItemText * ptextItem=dynamic_cast<QCPItemText *>(item);
    if(ptextItem)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Edit label");
        QGridLayout* gbox = new QGridLayout();

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);

        ColorWheel * colorWheel=new ColorWheel(dialog);
        colorWheel->setColor(ptextItem->color());

        QLineEdit * le_label=new QLineEdit(dialog);
        le_label->setText(ptextItem->text());

        QSpinBox * fontSize=new QSpinBox(dialog);
        fontSize->setPrefix("Size=");
        fontSize->setSuffix(" pt");
        fontSize->setValue(ptextItem->font().pointSize());

        gbox->addWidget(le_label,0,0);
        gbox->addWidget(colorWheel,1,0);
        gbox->addWidget(fontSize,2,0);
        gbox->addWidget(buttonBox,3,0);
        dialog->setLayout(gbox);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            ptextItem->setText(le_label->text());
            ptextItem->setColor(colorWheel->color());
            ptextItem->setFont(QFont(ptextItem->font().family(),fontSize->value()));
            ptextItem->setSelectedFont(QFont(ptextItem->font().family(),fontSize->value()));
        }
    }

    //If tracer
    QCPItemTracer * ptracerItem=dynamic_cast<QCPItemTracer *>(item);
    if(ptracerItem)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Edit tracer");
        QGridLayout* gbox = new QGridLayout();

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);

        QDoubleSpinBox * sb_size=new QDoubleSpinBox(dialog);
        sb_size->setPrefix("Size=");
        sb_size->setValue(ptracerItem->size());        

        ColorWheel * colorWheel=new ColorWheel(dialog);
        colorWheel->setColor(ptracerItem->brush().color());

        gbox->addWidget(sb_size,0,0);
        gbox->addWidget(colorWheel,1,0);
        gbox->addWidget(buttonBox,2,0);
        dialog->setLayout(gbox);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            ptracerItem->setSize(sb_size->value());
            ptracerItem->setBrush(QBrush(colorWheel->color()));
        }
    }

    //If line
    QCPItemLine * plineItem=dynamic_cast<QCPItemLine *>(item);
    if(plineItem)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Edit line");
        QGridLayout* gbox = new QGridLayout();

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);

        QDoubleSpinBox * sb_size=new QDoubleSpinBox(dialog);
        sb_size->setPrefix("Size=");
        sb_size->setValue(plineItem->pen().widthF());

        ColorWheel * colorWheel=new ColorWheel(dialog);
        colorWheel->setColor(plineItem->pen().color());

        gbox->addWidget(sb_size,0,0);
        gbox->addWidget(colorWheel,1,0);
        gbox->addWidget(buttonBox,2,0);
        dialog->setLayout(gbox);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            QPen pen;
            pen.setWidthF(sb_size->value());
            pen.setColor(colorWheel->color());
            plineItem->setPen(pen);
        }
    }

    replot();
    update();
}

void Viewer1D::slot_legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item)
{
    // Rename a graph by double clicking on its legend item
    Q_UNUSED(legend)
    if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
    {
        QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText(this,"Set legend", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
        if (ok)
        {
            plItem->plottable()->setName(newName);
            replot();
        }
    }
}

void Viewer1D::slot_fit_sinusoide()
{
    QList<Curve2D> curves=getSelectedCurves();

    if (curves.size()==1)
    {
        QDoubleSpinBox* A,*F,*P,*C;
        QCheckBox * fixedA,*fixedF,*fixedP,*fixedC;
        QDialog* dialog=Sinusoide::createDialog(A,F,P,C,
                                                fixedA,fixedF,fixedP,fixedC);

        double frequencyGuess,a0;
        std::complex<double> pm=curves[0].guessMainFrequencyPhaseModule(frequencyGuess,a0);

        A->setValue(std::abs(pm));
        F->setValue(frequencyGuess);
        P->setValue(std::arg(pm)+M_PI/2);
        C->setValue(a0);
        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            Sinusoide sinusoide(A->value(),F->value(),P->value(),C->value(),
                                fixedA->isChecked(),fixedF->isChecked(),fixedP->isChecked(),fixedC->isChecked());

            curves[0].fit(&sinusoide);
            sinusoide.regularized();
            Eigen::VectorXd X=curves[0].getLinX(1000);
            Eigen::VectorXd Y=sinusoide.at(X);

            QString result_str=QString("A=%1 F=%2[Hz] P=%3[°] C=%4").arg(sinusoide.getA()).arg(sinusoide.getF()).arg(sinusoide.getP()*180/M_PI).arg(sinusoide.getC());
            QString expression=QString("%1*sin(2*pi*%2*X+%3)+%4").arg(sinusoide.getA()).arg(sinusoide.getF()).arg(sinusoide.getP()).arg(sinusoide.getC());
            Curve2D fit_curve(X,Y,QString("Fit Sinusoid : %1").arg(result_str),Curve2D::GRAPH);

            slot_add_data(fit_curve);

            emit sig_displayResults(QString("Fit Sinusoid :\n%1\nF(X)=%2\nRms=%3").arg(result_str).arg(expression).arg(sinusoide.getRMS()));
            emit sig_newColumn(QString("Err_Sinusoid"),sinusoide.getErrNorm());
        }
    }
}

void Viewer1D::slot_fit_circle()
{
    QList<Curve2D> curves=getSelectedCurves();

    for (int i=0; i<curves.size(); i++)
    {
        QDoubleSpinBox* A,*B,*R;
        QDialog* dialog=Circle2D::createDialog(R,A,B);

        Eigen::Vector2d center=curves[i].getBarycenter();
        A->setValue(center[0]);
        B->setValue(center[1]);
        R->setValue(1.0 );

        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            Circle2D circle(Eigen::Vector2d(A->value(),B->value()),R->value());

            curves[i].fit(&circle);

            Eigen::VectorXd X,Y;
            circle.create(X,Y,200);

            QString result_str=QString("A=%1 B=%2 R=%3").arg(circle.getCenter()[0]).arg(circle.getCenter()[1]).arg(circle.getRadius());
            Curve2D fit_curve(X,Y,QString("Fit Circle : %1").arg(result_str),Curve2D::CURVE);

            slot_add_data(fit_curve);

            emit sig_displayResults(QString("Fit Circle :\n%1\nRms=%3").arg(result_str).arg(circle.getRMS()));
            emit sig_newColumn(QString("Err_Circle"),circle.getErrNorm());
        }
    }
}

void Viewer1D::slot_fit_custom()
{
    std::cout<<"Custom fit"<<std::endl;

    QList<Curve2D> curves=getSelectedCurves();

    for (int i=0; i<curves.size(); i++)
    {
        CustomExpDialog dialog;

        int result=dialog.exec();

        if (result == QDialog::Accepted)
        {
            if(dialog.isValid())
            {
                //init
                CustomRealExp * p_exp=dialog.getCustomRealExpression();

                p_exp->setParams(dialog.getP0());

                //Minimize
                curves[i].fit(p_exp);

                //Results
                QString result_str;
                VectorXd p=p_exp->getParams();
                for(int i=0;i<p_exp->getParamsNames().size();i++)
                {
                    result_str.append(QString("%1=%2 ").arg(p_exp->getParamsNames()[i]).arg(p[i]));
                }

                Eigen::VectorXd X=curves[0].getLinX(1000);
                Eigen::VectorXd Y=p_exp->at(X);

                Curve2D fit_curve(X,Y,QString("Fit Custom : %1").arg(result_str),Curve2D::CURVE);

                slot_add_data(fit_curve);

                emit sig_displayResults(QString("Fit Custom real expression :%1\n%2\nRms=%3").arg(p_exp->getExpression()).arg(result_str).arg(p_exp->getRMS()));
                emit sig_newColumn(QString("Err_Custom"),p_exp->getErrNorm());

            }
        }

    }
}

void Viewer1D::slot_fit_ellipse()
{
    QList<Curve2D> curves=getSelectedCurves();

    for (int i=0; i<curves.size(); i++)
    {
        QDoubleSpinBox* A,*B,*Ra,*Rb,*Theta;
        QDialog* dialog=Ellipse2D::createDialog(Ra,Rb,A,B,Theta);

        Eigen::Vector2d center=curves[i].getBarycenter();
        A->setValue(center[0]);
        B->setValue(center[1]);
        Ra->setValue(1.0 );
        Rb->setValue(1.0 );
        Theta->setValue(0.0 );

        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            Ellipse2D ellipse(Eigen::Vector2d(A->value(),B->value()),Ra->value(),Rb->value(),Theta->value()/180*M_PI);

            curves[i].fit(&ellipse);

            Eigen::VectorXd X,Y;
            ellipse.create(X,Y,200);

            QString result_str=QString("A=%1 B=%2 Ra=%3 Rb=%4 Theta=%5").arg(ellipse.getCenter()[0]).arg(ellipse.getCenter()[1]).arg(ellipse.getRa()).arg(ellipse.getRb()).arg(ellipse.getTheta()*180/M_PI);
            Curve2D fit_curve(X,Y,QString("Fit Ellipse : %1").arg(result_str),Curve2D::CURVE);

            slot_add_data(fit_curve);

            emit sig_displayResults(QString("Fit Ellipse :\n%1\nRms=%3").arg(result_str).arg(ellipse.getRMS()));
            emit sig_newColumn(QString("Err_Ellipse"),ellipse.getErrNorm());
        }
    }
}

void Viewer1D::slot_fit_sigmoid()
{
    QList<Curve2D> curves=getSelectedCurves();

    for (int i=0; i<curves.size(); i++)
    {
        QDoubleSpinBox* A,*B,*C,*P;
        QDialog* dialog=Sigmoid::createDialog(A,B,C,P);
        A->setValue(-1);
        B->setValue(1);
        C->setValue(0);
        P->setValue(1);
        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            Sigmoid sigmoid(A->value(),B->value(),C->value(),P->value());


            curves[i].fit(&sigmoid);

            Eigen::VectorXd X=curves[i].getLinX(1000);
            Eigen::VectorXd Y=sigmoid.at(X);

            QString result_str=QString("A=%1 B=%2 C=%3 P=%4 ").arg(sigmoid.getA()).arg(sigmoid.getB()).arg(sigmoid.getC()).arg(sigmoid.getP());
            Curve2D fit_curve(X,Y,QString("Fit Sigmoid : %1").arg(result_str),Curve2D::GRAPH);

            QString expression=QString("(%2-%1)/(1+exp((%3-X)*%4*(%2-%1)))+%1").arg(sigmoid.getA()).arg(sigmoid.getB()).arg(sigmoid.getC()).arg(sigmoid.getP());

            slot_add_data(fit_curve);

            emit sig_displayResults(QString("Fit Sigmoid :\n%1\nF(X)=%2\nRms=%3").arg(result_str).arg(expression).arg(sigmoid.getRMS()));
            emit sig_newColumn(QString("Err_Sigmoid"),sigmoid.getErrNorm());
        }
    }
}

void Viewer1D::slot_fit_gaussian()
{
    QList<Curve2D> curves=getSelectedCurves();

    for (int i=0; i<curves.size(); i++)
    {

        QDoubleSpinBox* S,*M,*K;
        QDialog* dialog=Gaussian::createDialog(S,M,K);
        S->setValue(1);
        M->setValue(curves[i].getMeanXWeightedByY());
        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            Gaussian gaussian(S->value(),M->value(),K->value());

            curves[i].fit(&gaussian);
            Eigen::VectorXd X=curves[i].getLinX(1000);
            Eigen::VectorXd Y=gaussian.at(X);

            QString result_str=QString("Sigma=%1 Mean=%2 K=%3").arg(gaussian.getS()).arg(gaussian.getM()).arg(gaussian.getK());

            Curve2D fit_curve(X,Y,QString("Fit Gaussian : %1").arg(result_str),Curve2D::GRAPH);

            slot_add_data(fit_curve);

            QString expression=QString("%3/(%1*sqrt(2*pi))*exp(-0.5*((X-%2)/%1)^2)").arg(gaussian.getS()).arg(gaussian.getM()).arg(gaussian.getK());

            emit sig_displayResults(QString("Fit Gaussian :\n%1 \nF(X)==%2 \n").arg(result_str).arg(expression));
            emit sig_newColumn(QString("Err_Gaussian"),gaussian.getErrNorm());

        }

    }
}

void Viewer1D::slot_fit_2var_polynomial()
{
    QList<Curve2D> curves=getSelectedCurves();
    if (curves.size()>0)
    {
        if (curves[0].getScalarField().size()>0)
        {
            //------------------------------------------------Dialog
            QDialog* dialog=new QDialog;
            dialog->setLocale(QLocale("C"));
            dialog->setWindowTitle("Polynomial of 2 Variables on scalar field: Initials parameters");
            QGridLayout* gbox = new QGridLayout();

            QLabel* label_eqn=new QLabel(dialog);
            label_eqn->setPixmap(QPixmap(":/eqn/eqn/polynome_2var.gif"));
            label_eqn->setAlignment(Qt::AlignHCenter);

            QSpinBox* getOrder=new QSpinBox(dialog);
            getOrder->setRange(2,10);
            getOrder->setPrefix("Order=");

            QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                               | QDialogButtonBox::Cancel);

            QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
            QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

            gbox->addWidget(label_eqn,0,0);
            gbox->addWidget(getOrder,1,0);
            gbox->addWidget(buttonBox,3,0);

            dialog->setLayout(gbox);

            //------------------------------------------------
            int result=dialog->exec();
            if (result == QDialog::Accepted)
            {
                unsigned int order=static_cast<unsigned int>(getOrder->value());
                Eigen::VectorXd C=curves[0].fit2d(order);
                Eigen::VectorXd X,Y,S;
                curves[0].getLinXY(100,X,Y);
                S=curves[0].at(C,X,Y,order);

                QString result_str=Curve2D::getPolynome2VString(C,order);

                Curve2D fit_curve(X,Y,QString("Fit Polynome : %1").arg(result_str),Curve2D::CURVE);
                fit_curve.getStyle().mScatterShape=static_cast<int>(QCPScatterStyle::ssDisc);
                fit_curve.getStyle().mLineStyle=static_cast<int>(QCPCurve::lsNone);
                fit_curve.setScalarField(S);
                slot_add_data(fit_curve);
            }
        }
        else
        {
            QMessageBox::information(this,"Oups","Selected curve must have scalar field");
        }
    }
    else
    {
        QMessageBox::information(this,"Oups","Please select a curve");
    }
}

void Viewer1D::slot_fit_polynomial()
{
    QList<Curve2D> curves=getSelectedCurves();
    if (curves.size()>0)
    {
        //------------------------------------------------Dialog
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Polynomial : Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        QLabel* label_eqn=new QLabel(dialog);
        label_eqn->setPixmap(QPixmap(":/eqn/eqn/polynome.gif"));
        label_eqn->setAlignment(Qt::AlignHCenter);

        QSpinBox* getOrder=new QSpinBox(dialog);
        getOrder->setRange(1,40);
        getOrder->setValue(1);
        getOrder->setPrefix("Order=");

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(label_eqn,0,0);
        gbox->addWidget(getOrder,1,0);
        gbox->addWidget(buttonBox,3,0);

        dialog->setLayout(gbox);

        //------------------------------------------------
        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            int order=getOrder->value();
            for (int i=0; i<curves.size(); i++)
            {
                Eigen::VectorXd C=curves[i].fit(order);
                Eigen::VectorXd X=curves[i].getLinX(1000);
                Eigen::VectorXd Y=curves[i].at(C,X);

                Eigen::VectorXd Yhat=curves[i].at(C,curves[i].getX());

                QString result_str=Curve2D::getPolynomeString(C,order);

                Curve2D fit_curve(X,Y,QString("Fit Polynome : %1").arg(result_str),Curve2D::GRAPH);

                Eigen::VectorXd E=curves[i].getY()-Yhat;
                double rms=sqrt(E.dot(E)/E.size());

                slot_add_data(fit_curve);
                emit sig_displayResults(QString("Fit Polynome :\nP(X)=%1\nRms=%2").arg(result_str).arg(rms));
                emit sig_newColumn(QString("Err_Polynome"),E);
            }
        }
    }
    else
    {
        QMessageBox::information(this,"Oups","Please select a curve");
    }

}

void Viewer1D::slot_show_legend(bool value)
{
    legend->setVisible(value);
    replot();

    if (value)
    {
        actLegendShowHide->setText("Hide");
    }
    else
    {
        actLegendShowHide->setText("Show");
    }
}

void Viewer1D::slot_top_legend(bool value)
{
    if (value)
    {
        top_bottom=Qt::AlignTop;
        actLegendTopBottom->setText("Move bottom");
    }
    else
    {
        top_bottom=Qt::AlignBottom;
        actLegendTopBottom->setText("Move top");
    }

    this->axisRect()->insetLayout()->setInsetAlignment(0,left_right|top_bottom);

    replot();
}

void Viewer1D::slot_left_legend(bool value)
{
    if (value)
    {
        left_right=Qt::AlignRight;
        actLegendLeftRight->setText("Move left");
    }
    else
    {
        left_right=Qt::AlignLeft;
        actLegendLeftRight->setText("Move right");
    }

    this->axisRect()->insetLayout()->setInsetAlignment(0,left_right|top_bottom);

    replot();
}

void Viewer1D::slot_setBrushColor(QColor color)
{
    QList<QCPAbstractPlottable*> plottables=selectedPlottables();

    for (int i=0; i<plottables.size(); i++)
    {
        QBrush brush=plottables[i]->brush();
        color.setAlphaF(brush.color().alphaF());
        brush.setColor(color);
        plottables[i]->setBrush(brush);
    }

    replot();
}

void Viewer1D::slot_setPenColor(QColor color)
{
    QList<QCPAbstractPlottable*> plottables=this->selectedPlottables();

    for (int i=0; i<plottables.size(); i++)
    {
        QPen pen=plottables[i]->pen();
        color.setAlphaF(pen.color().alphaF());
        pen.setColor(color);
        plottables[i]->setPen(pen);
    }

    replot();
}

void Viewer1D::slot_setPenWidth(double width)
{
    QList<QCPAbstractPlottable*> plottables=this->selectedPlottables();

    for (int i=0; i<plottables.size(); i++)
    {
        QPen pen=plottables[i]->pen();
        pen.setWidthF(width);
        plottables[i]->setPen(pen);
    }
    replot();
}

void Viewer1D::slot_increasePenWidth()
{
    QList<QCPAbstractPlottable*> plottables=this->plottables();

    for (int i=0; i<plottables.size(); i++)
    {
        QPen pen=plottables[i]->pen();
        pen.setWidthF(pen.widthF()+1.0);
        plottables[i]->setPen(pen);
    }
    replot();
}

void Viewer1D::slot_decreasePenWidth()
{
    QList<QCPAbstractPlottable*> plottables=this->plottables();

    for (int i=0; i<plottables.size(); i++)
    {
        QPen pen=plottables[i]->pen();
        float pW=pen.widthF()-1.0;
        if(pW>1)
        {
            pen.setWidthF(pW);
        }
        else
        {
            pen.setWidthF(1.0);
        }
        plottables[i]->setPen(pen);
    }
    replot();
}

void Viewer1D::slot_setPenAlpha(double alpha)
{
    QList<QCPAbstractPlottable*> plottables=this->selectedPlottables();

    for (int i=0; i<plottables.size(); i++)
    {
        QPen pen=plottables[i]->pen();
        QColor color=pen.color();
        color.setAlphaF(alpha);
        pen.setColor(color);
        plottables[i]->setPen(pen);
    }
    replot();
}

void Viewer1D::slot_setBrushAlpha(double alpha)
{
    QList<QCPAbstractPlottable*> plottables=this->selectedPlottables();

    for (int i=0; i<plottables.size(); i++)
    {
        QBrush brush=plottables[i]->brush();
        QColor color=brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        plottables[i]->setBrush(brush);
    }
    replot();
}

void Viewer1D::slot_setPenStyle(int style)
{
    QList<QCPAbstractPlottable*> plottables=this->selectedPlottables();

    for (int i=0; i<plottables.size(); i++)
    {
        QPen pen=plottables[i]->pen();
        pen.setStyle(Qt::PenStyle(style+1));
        plottables[i]->setPen(pen);
    }
    replot();
}

void Viewer1D::slot_setBrushStyle(int style)
{
    QList<QCPAbstractPlottable*> plottables=this->selectedPlottables();

    for (int i=0; i<plottables.size(); i++)
    {
        QBrush brush=plottables[i]->brush();
        brush.setStyle(Qt::BrushStyle(style));
        plottables[i]->setBrush(brush);
    }
    replot();
}

void Viewer1D::slot_histogram(Eigen::VectorXd data,QString name,int nbbins)
{
    Eigen::VectorXd labels=Eigen::VectorXd::Zero(nbbins);
    Eigen::VectorXd hist=Eigen::VectorXd::Zero(nbbins);

    double min=data.minCoeff();
    double max=data.maxCoeff();

    double delta= (max-min)/nbbins;

    for (int k=0; k<data.size(); k++)
    {
        int index=static_cast<int>(std::floor((data[k]-min)/delta));

        if (index>=0 && index<data.size())
        {
            hist[index]+=1;
        }
    }

    for (int i=0; i<labels.size(); i++)
    {
        labels[i]=delta*i+min;
    }

    Curve2D hist_curve(labels,hist,name,Curve2D::GRAPH);
    hist_curve.getStyle().mLineStyle=QCPGraph::lsStepCenter;
    slot_add_data(hist_curve);
}

void Viewer1D::slot_delete()
{
    QList<QCPAbstractPlottable*> plottableslist=this->selectedPlottables();

    for (int i=0; i<plottableslist.size(); i++)
    {
        this->removePlottable(plottableslist[i]);
        this->plotLayout()->simplify();
    }


    QList<QCPAbstractItem*> itemslist=this->selectedItems();
    for (int i=0; i<itemslist.size(); i++)
    {
        this->removeItem(itemslist[i]);
    }

    this->replot();
}

QList<QCPAbstractPlottable*> Viewer1D::getCurvesOrGraphs(bool selected)
{
    QList<QCPAbstractPlottable*> plottableslist=(selected)?this->selectedPlottables():this->plottables();
    QList<QCPAbstractPlottable*> list;
    for (int i=0; i<plottableslist.size(); i++)
    {
        QCPAbstractPlottable* currentp=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if (currentp)
        {
            list.push_back(currentp);
        }

        currentp=dynamic_cast<QCPGraph*>(plottableslist[i]);
        if (currentp)
        {
            list.push_back(currentp);
        }
    }
    return list;
}

QList<Curve2D> Viewer1D::getSelectedCurves()
{
    QList<Curve2D> curvelist;
    QList<QCPAbstractPlottable*> plottableslist=this->selectedPlottables();

    for (int i=0; i<plottableslist.size(); i++)
    {
        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if (currentcurve)
        {
            Curve2D curve;
            curve.fromQCP(currentcurve);
            curvelist.push_back(curve);
        }

        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(plottableslist[i]);
        if (currentgraph)
        {
            Curve2D curve;
            curve.fromQCP(currentgraph);
            curvelist.push_back(curve);
        }

        QCPColorMap* currentmap=dynamic_cast<QCPColorMap*>(plottableslist[i]);
        if (currentmap)
        {
            Curve2D curve;
            curve.fromQCPMap(currentmap);
            curvelist.push_back(curve);
        }

    }

    return curvelist;
}

void Viewer1D::slot_save_image_current_map()
{
    std::cout<<"slot_save_image_current_map"<<std::endl;

    QList<QCPColorMap*> listMaps=getQCPListOf<QCPColorMap>(true);

    for(int i=0;i<listMaps.size();i++)
    {
        QFileInfo info(current_filename);
        QString where=info.path();
        QString filename=QFileDialog::getSaveFileName(this,"Save Image",where,"(*.png)");
        if (!filename.isEmpty())
        {
            listMaps[i]->getImage().save(filename);
        }
    }
}

void Viewer1D::slot_save_image()
{
    QFileInfo info(current_filename);
    QString where=info.path();

    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Resolution");
    QGridLayout* gbox = new QGridLayout();
    QSpinBox* getWidth=new QSpinBox(dialog);
    QSpinBox* getHeight=new QSpinBox(dialog);
    getWidth->setRange(32,1024);
    getWidth->setPrefix("Width=");
    getWidth->setSuffix(" px");
    getWidth->setValue(width());
    getHeight->setRange(32,1024);
    getHeight->setPrefix("Height=");
    getHeight->setSuffix(" px");
    getHeight->setValue(height());
    gbox->addWidget(getWidth,0,0);
    gbox->addWidget(getHeight,0,1);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
    gbox->addWidget(buttonBox,4,0,2,1);
    dialog->setLayout(gbox);

    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        QString filename=QFileDialog::getSaveFileName(this,"Save Image",where,"(*.png)");
        if (!filename.isEmpty())
        {
            this->current_filename=filename;

            std::cout<<"Save "<<getWidth->value()<<"x"<<getHeight->value()<<std::endl;

            this->savePng(current_filename,getWidth->value(),getHeight->value(),1);
        }
    }
}

void Viewer1D::slot_rescale()
{
    QList<QCPCurve*> listcurves=getQCPListOf<QCPCurve>(false);
    for (int i=0; i<listcurves.size(); i++)
    {
        if (listcurves[i]->getColorScale())
        {
            listcurves[i]->getColorScale()->setDataRange(listcurves[0]->getScalarFieldRange());
        }
    }

    QList<QCPColorMap*> listmaps=getQCPListOf<QCPColorMap>(false);
    std::cout<<"listmaps.size()="<<listmaps.size()<<std::endl;
    for (int i=0; i<listmaps.size(); i++)
    {
        if (listmaps[i]->colorScale())
        {
            std::cout<<"range="<<listmaps[i]->dataRange().lower<<" "<<listmaps[i]->dataRange().upper<<std::endl;
            //listmaps[i]->colorScale()->setDataRange( listmaps[i]->dataRange() );
            listmaps[i]->colorScale()->rescaleDataRange(true);
        }
    }

    this->rescaleAxes();
    this->replot();
}

void Viewer1D::slot_copy()
{
    std::cout<<"slot_copy"<<std::endl;

    QList<Curve2D> list=getSelectedCurves();

    std::cout<<"slot_copy "<<list.size()<<std::endl;

    if (list.size()>0)
    {
        QMimeData * mimeData=new QMimeData();
        QByteArray rawData=list[0].toByteArray();
        mimeData->setData("Curve",rawData);
        QApplication::clipboard()->setMimeData(mimeData);
    }
}

void Viewer1D::slot_paste()
{
    //    if (sharedBuf!=nullptr)
    //    {
    //        if (sharedBuf->getX().size()>0)
    //        {
    //            slot_add_data(*sharedBuf);
    //        }
    //    }
    const QMimeData * mimeData=QApplication::clipboard()->mimeData();

    if(mimeData)
    {
        if(mimeData->hasFormat("Curve"))
        {
            Curve2D curve;
            QByteArray data=mimeData->data("Curve");
            curve.fromByteArray(data);
            slot_add_data(curve);
        }
    }

    slot_rescale();
}

void Viewer1D::slot_svd()
{
    QList<Curve2D> curves=getSelectedCurves();

    for (int i=0; i<curves.size(); i++)
    {
        Eigen::MatrixXd M(curves[i].getX().size(),2);
        M.col(0)=curves[i].getX();
        M.col(1)=curves[i].getY();

        //Centroid
        Eigen::VectorXd C=M.colwise().mean();

        //Substract centroid
        for (int i=0; i<M.rows(); i++)
        {
            M(i,0)-=C[0];
            M(i,1)-=C[1];
        }

        //Svd
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(M, Eigen::ComputeFullU | Eigen::ComputeFullV);
        Eigen::MatrixXd U=svd.matrixU();
        Eigen::VectorXd S=svd.singularValues();
        Eigen::MatrixXd V=svd.matrixV();

        slot_gadgetAddArrow(C,C+V.row(0).transpose()*sqrt(S[0]));
        slot_gadgetAddArrow(C,C+V.row(1).transpose()*sqrt(S[1]));

        emit sig_displayResults(QString("Singular value decomposition M=USV:\n-Centroid:\n%4\n-Singulars values S:\n%1\n-Right singular vectors V:\n%3-Left Singular vectors U:\n%2").arg(toString(S)).arg(toString(U)).arg(toString(V)).arg(toString(C)) );
    }
    replot();
}

void Viewer1D::slot_covariance()
{
    QList<Curve2D> curves=getSelectedCurves();

    for (int i=0; i<curves.size(); i++)
    {
        Eigen::MatrixXd M(curves[i].getX().size(),2);
        M.col(0)=curves[i].getX();
        M.col(1)=curves[i].getY();

        //Centroid
        Eigen::VectorXd C=M.colwise().mean();

        //Substract centroid
        for (int i=0; i<M.rows(); i++)
        {
            M(i,0)-=C[0];
            M(i,1)-=C[1];
        }

        Eigen::MatrixXd Cov=(M.transpose()*M)*1.0/M.rows();

        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(Cov);
        Eigen::MatrixXd U=eigenSolver.eigenvectors();
        Eigen::VectorXd S=eigenSolver.eigenvalues();

        slot_gadgetAddArrow(C,C+U.col(0)*sqrt(S[0]));
        slot_gadgetAddArrow(C,C+U.col(1)*sqrt(S[1]));

        emit sig_displayResults(QString("Covariance matrix :\n%1\n Eigen value decomposition M=USU^*:\n-Centroid:\n%2\n-Eigen values S:\n%3\n-Eigen vectors U:\n%4\n").arg(toString(Cov)).arg(toString(C)).arg(toString(S)).arg(toString(U)) );
    }
}

void Viewer1D::slot_statistiques()
{
    QList<Curve2D> curves=getSelectedCurves();

    for (int i=0; i<curves.size(); i++)
    {
        double mean=curves[i].getY().mean();

        Eigen::VectorXd Ysorted=(curves[i].getY()-Eigen::VectorXd::Constant(curves[i].getY().size(),mean)).cwiseAbs();
        std::sort(Ysorted.data(), Ysorted.data()+Ysorted.size());

        QString CIstr;
        int rate_prev=0;
        for (int k=0; k<Ysorted.size(); k++)
        {
            int rate=static_cast<int>(floor(double(k)/Ysorted.size()*100.0));

            if ( rate%5==0 && rate_prev!=rate)
            {
                rate_prev=rate;
                CIstr+=QString("CI%2%=%1\n").arg(Ysorted[k]).arg(rate);
            }
        }

        double min=curves[i].getY().minCoeff();
        double max=curves[i].getY().maxCoeff();

        QString stat_str=QString("Statistiques : %1\nMin=%2\nMax=%3\nSpan=%4\nMean=%5\nStandard-Deviation=%6\nRoot-Mean-Squares=%7\n%8")
                .arg(curves[i].getLegend())
                .arg(min)
                .arg(max)
                .arg(max-min)
                .arg(mean)
                .arg( sqrt( (curves[i].getY()-Eigen::VectorXd::Constant(curves[i].getY().size(),mean)).cwiseAbs2().mean() ) )
                .arg( curves[i].getRms() )
                .arg(CIstr);

        emit sig_displayResults(stat_str);
    }
}

void Viewer1D::applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map)
{
    QMap<QString,QAction*> shortcuts_links;
    shortcuts_links.insert(QString("Graph-Copy"),actCopy);
    shortcuts_links.insert(QString("Graph-Paste"),actPaste);
    shortcuts_links.insert(QString("Graph-ClearGadgets"),actClearGadgets);
    shortcuts_links.insert(QString("Graph-GadgetText"),actGadgetText);
    shortcuts_links.insert(QString("Graph-GadgetArrow"),actGadgetArrow);
    shortcuts_links.insert(QString("Graph-GadgetMark"),actGadgetMark);
    shortcuts_links.insert(QString("Graph-GadgetTracer"),actGadgetTracer);
    shortcuts_links.insert(QString("Graph-GadgetLine"),actGadgetLine);
    shortcuts_links.insert(QString("Graph-GadgetDeltaLine"),actGadgetDeltaLine);
    shortcuts_links.insert(QString("Graph-Save"),actSave);
    shortcuts_links.insert(QString("Graph-Rescale"),actRescale);
    shortcuts_links.insert(QString("Graph-Delete"),actDelete);
    shortcuts_links.insert(QString("Graph-Legend-Show/Hide"),actLegendShowHide);
    shortcuts_links.insert(QString("Graph-Legend-Top/Bottom"),actLegendTopBottom);
    shortcuts_links.insert(QString("Graph-Legend-Left/Right"),actLegendLeftRight);
    shortcuts_links.insert(QString("Graph-AutoColor1"),actAutoColor1);
    shortcuts_links.insert(QString("Graph-AutoColor2"),actAutoColor2);
    shortcuts_links.insert(QString("Graph-AutoColor3"),actAutoColor3);
    shortcuts_links.insert(QString("Graph-AutoColor4"),actAutoColor4);
    shortcuts_links.insert(QString("Graph-AutoColor5"),actAutoColor5);
    shortcuts_links.insert(QString("Graph-AutoColorClear"),actAutoColorClear);
    shortcuts_links.insert(QString("Graph-IncreasePenWidth"),actIncreasePenWidth);
    shortcuts_links.insert(QString("Graph-DecreasePenWidth"),actDecreasePenWidth);
    shortcuts_links.insert(QString("Graph-SetScatters"),actSetScatters);

    QMapIterator<QString, QKeySequence> i(shortcuts_map);
    while (i.hasNext())
    {
        i.next();

        if (shortcuts_links.contains(i.key()))
        {
            shortcuts_links[i.key()]->setShortcut(i.value());
        }
    }
}

void Viewer1D::addSubMenu(QMenu * sub_menu)
{
    sub_menus.push_back(sub_menu);
    popup_menu->addMenu(sub_menu);
}

void Viewer1D::slot_setAxisXType(int mode)
{
    if (mode==1)
    {
        this->xAxis->setScaleType(QCPAxis::stLogarithmic);
    }
    else
    {
        this->xAxis->setScaleType(QCPAxis::stLinear);
    }
    rescaleAxes();
    replot();
}

void Viewer1D::slot_setAxisYType(int mode)
{
    if (mode==1)
    {
        this->yAxis->setScaleType(QCPAxis::stLogarithmic);
    }
    else
    {
        this->yAxis->setScaleType(QCPAxis::stLinear);
    }
    replot();
}

void Viewer1D::slot_showSubGridType(int state)
{
    xAxis->grid()->setSubGridVisible(state);
    yAxis->grid()->setSubGridVisible(state);
    replot();
}

void Viewer1D::slot_setScalarFieldGradientType(int type)
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();
    QList<QCPCurve*> curveslist=this->getQCPListOf<QCPCurve>(true);
    QList<QCPColorMap*> mapslist=this->getQCPListOf<QCPColorMap>(true);

    for (int i=0; i<graphslist.size(); i++)
    {
        graphslist[0]->setScalarFieldGradientType(type);
    }
    for (int i=0; i<curveslist.size(); i++)
    {
        curveslist[0]->setScalarFieldGradientType(type);
    }
    for (int i=0; i<mapslist.size(); i++)
    {
        mapslist[0]->colorScale()->setGradient(static_cast<QCPColorGradient::GradientPreset>(type));
    }
    replot();
}

void Viewer1D::slot_meanFilter()
{
    QList<Curve2D> curves=getSelectedCurves();
    if (curves.size()>0)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Mean Filter : Parameters");
        QGridLayout* gbox = new QGridLayout();

        QSpinBox* getRadius=new QSpinBox(dialog);
        getRadius->setRange(1,1000);
        getRadius->setValue(1);
        getRadius->setPrefix(QString("Radius="));

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(getRadius,0,0);
        gbox->addWidget(buttonBox,1,0);

        dialog->setLayout(gbox);

        //------------------------------------------------
        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            for(int k=0;k<curves.size();k++)
            {
                Eigen::VectorXd Y=curves[k].getY(),Ymean(Y.rows());
                int radius=getRadius->value();
                for(int i=0;i<Y.rows();i++)
                {
                    double sum=0.0;
                    int cpt=0;
                    for(int j=i-radius;j<=i+radius;j++)
                    {
                        if(j>0 && j<Y.rows())
                        {
                            sum+=Y[j];
                            cpt++;
                        }
                    }
                    sum/=cpt;
                    Ymean[i]=sum;
                }

                Curve2D fit_curve(curves[k].getX(),Ymean,QString("Mean Filter: %1").arg(curves[k].getLegend()),Curve2D::GRAPH);
                slot_add_data(fit_curve);
                emit sig_newColumn(QString("Mean"),Ymean);
            }
        }
    }
}

void Viewer1D::slot_Distance()
{
    QList<Curve2D> curves=getSelectedCurves();
    if (curves.size()>0)
    {
        bool ok;
        double knn=QInputDialog::getInt(this,"K-nearest neighbors distances",
                                        "Evaluate the mean distance of K-nearest neighbors .\n\n K=",20,0,2147483647,1,&ok);
        if(ok)
        {
            curves[0].knnMeanDistance(knn);

            curves[0].updateLinkedScalarfield(this);

            replot();

            emit sig_newColumn(QString("Distances"),curves[0].getScalarField());
        }
    }

}

void Viewer1D::slot_appearance()
{
    configurePopup();
    parameterWidget->raise();
    parameterWidget->show();
}

void Viewer1D::slot_medianFilter()
{
    QList<Curve2D> curves=getSelectedCurves();
    if (curves.size()>0)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Median Filter : Parameters");
        QGridLayout* gbox = new QGridLayout();

        QSpinBox* getRadius=new QSpinBox(dialog);
        getRadius->setRange(1,1000);
        getRadius->setValue(1);
        getRadius->setPrefix(QString("Radius="));

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(getRadius,0,0);
        gbox->addWidget(buttonBox,1,0);

        dialog->setLayout(gbox);

        //------------------------------------------------
        int result=dialog->exec();
        if (result == QDialog::Accepted)
        {
            for(int k=0;k<curves.size();k++)
            {
                Eigen::VectorXd Y=curves[k].getY(),Ymean(Y.rows());
                int radius=getRadius->value();
                for(int i=0;i<Y.rows();i++)
                {
                    std::vector<double> values;
                    for(int j=i-radius;j<=i+radius;j++)
                    {
                        if(j>0 && j<Y.rows())
                        {
                            values.push_back(Y[j]);
                        }
                    }
                    qSort(values);
                    Ymean[i]=values[values.size()/2];
                }

                Curve2D fit_curve(curves[k].getX(),Ymean,QString("Median Filter").arg(curves[k].getLegend()),Curve2D::GRAPH);
                slot_add_data(fit_curve);
                emit sig_newColumn(QString("Median"),Ymean);
            }
        }
    }
}
