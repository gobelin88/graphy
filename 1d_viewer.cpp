#include "1d_viewer.h"

Viewer1D::Viewer1D(Curve2D* sharedBuf,const QMap<QString,QKeySequence>& shortcuts_map,QWidget* parent):QCustomPlot(parent)
{
    this->sharedBuf=sharedBuf;

    colors.append(QColor(255,0,0));
    colors.append(QColor(0,128,0));
    colors.append(QColor(0,0,255));
    colors.append(QColor(255,128,0));
    colors.append(QColor(0,128,255));
    colors.append(QColor(255,0,255));
    colors.append(QColor(128,255,0));
    colors.append(QColor(0,255,128));
    colors.append(QColor(128,0,255));
    colors.append(QColor(255,0,128));

    createPopup();

    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                          QCP::iSelectLegend | QCP::iSelectPlottables);

    connect(this, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(slot_axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(this, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(slot_legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    applyShortcuts(shortcuts_map);

    top_bottom=Qt::AlignTop;
    left_right=Qt::AlignRight;

    modifiers=Qt::NoModifier;
    state_label.clear();
    state_arrow.clear();
    state_mark.clear();
    arrowItem=nullptr;

    legend->setVisible(true);
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
        }
    }
}

//QCPGraph* Viewer1D::newGraph(const Curve2D& datacurve)
//{

//    this->rescaleAxes();
//    return pgraph;
//}

//QCPCurve* Viewer1D::newCurve(const Curve2D& datacurve)
//{
//    QCPCurve* pcurve =
//    this->rescaleAxes();
//    return pcurve;
//}

void Viewer1D::slot_add_data(const Curve2D& datacurve)
{
    if (datacurve.getType()==Curve2D::GRAPH)
    {
        std::cout<<"graph"<<std::endl;
        datacurve.toQCP<QCPGraph>(this);
    }
    else
    {
        std::cout<<"curve"<<std::endl;
        datacurve.toQCP<QCPCurve>(this);
        axisRect()->setupFullAxesBox();
    }

    rescaleAxes();
    replot();
}

void Viewer1D::configurePopup()
{
    QList<QCPAbstractPlottable*> plottables=selectedPlottables();

    if (plottables.size()>0)
    {
        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(plottables[0]);
        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(plottables[0]);

        if (currentcurve)
        {
            cb_itemLineStyleList->setCurrentIndex(currentcurve->lineStyle());
            cb_ScatterShapes->setCurrentIndex(static_cast<int>(currentcurve->scatterStyle().shape()));
            sb_ScatterSize->setValue(currentcurve->scatterStyle().size());
            if (currentcurve->getScalarField().size()>0)
            {
                cb_gradient->setCurrentIndex(currentcurve->getScalarFieldGradientType());
                cb_gradient->show();
            }
            else
            {
                cb_gradient->hide();
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
            else
            {
                cb_gradient->hide();
            }
        }


        s_pen_alpha->setValue(plottables[0]->pen().color().alphaF());
        s_brush_alpha->setValue(plottables[0]->brush().color().alphaF());
        cb_brushstyle->setCurrentIndex(plottables[0]->brush().style());
        cb_penstyle->setCurrentIndex(plottables[0]->pen().style()-1);
        cw_pen_color->setColor(plottables[0]->pen().color());
        cw_brush_color->setColor(plottables[0]->brush().color());
        sb_pen_width->setValue(plottables[0]->pen().widthF());

    }
}

QWidgetAction* Viewer1D::createParametersWidget()
{
    QWidgetAction* actWidget=new QWidgetAction(popup_menu);
    QWidget* widget=new QWidget;
    actWidget->setDefaultWidget(widget);

    QGridLayout* gbox = new QGridLayout();

    cb_scale_mode_x=new QComboBox;
    cb_scale_mode_x->addItem("Linear");
    cb_scale_mode_x->addItem("Logarithmic");
    cb_scale_mode_x->setCurrentIndex(0);

    cb_scale_mode_y=new QComboBox;
    cb_scale_mode_y->addItem("Linear");
    cb_scale_mode_y->addItem("Logarithmic");
    cb_scale_mode_y->setCurrentIndex(0);

    cb_itemLineStyleList = new QComboBox;
    cb_itemLineStyleList->addItem(QStringLiteral("lsNone"),        int(QCPGraph::LineStyle::lsNone));
    cb_itemLineStyleList->addItem(QStringLiteral("lsLine"),        int(QCPGraph::LineStyle::lsLine));
    cb_itemLineStyleList->addItem(QStringLiteral("lsStepLeft"),    int(QCPGraph::LineStyle::lsStepLeft));
    cb_itemLineStyleList->addItem(QStringLiteral("lsStepRight"),   int(QCPGraph::LineStyle::lsStepRight));
    cb_itemLineStyleList->addItem(QStringLiteral("lsStepCenter"),  int(QCPGraph::LineStyle::lsStepCenter));
    cb_itemLineStyleList->addItem(QStringLiteral("lsImpulse"),     int(QCPGraph::LineStyle::lsImpulse));

    cb_ScatterShapes = new QComboBox;
    cb_ScatterShapes->addItem(QStringLiteral("ssNone"),             int(QCPScatterStyle::ScatterShape::ssNone));
    cb_ScatterShapes->addItem(QStringLiteral("ssDot"),              int(QCPScatterStyle::ScatterShape::ssDot));
    cb_ScatterShapes->addItem(QStringLiteral("ssCross"),            int(QCPScatterStyle::ScatterShape::ssCross));
    cb_ScatterShapes->addItem(QStringLiteral("ssPlus"),             int(QCPScatterStyle::ScatterShape::ssPlus));
    cb_ScatterShapes->addItem(QStringLiteral("ssCircle"),           int(QCPScatterStyle::ScatterShape::ssCircle));
    cb_ScatterShapes->addItem(QStringLiteral("ssDisc"),             int(QCPScatterStyle::ScatterShape::ssDisc));
    cb_ScatterShapes->addItem(QStringLiteral("ssSquare"),           int(QCPScatterStyle::ScatterShape::ssSquare));
    cb_ScatterShapes->addItem(QStringLiteral("ssDiamond"),          int(QCPScatterStyle::ScatterShape::ssDiamond));
    cb_ScatterShapes->addItem(QStringLiteral("ssStar"),             int(QCPScatterStyle::ScatterShape::ssStar));
    cb_ScatterShapes->addItem(QStringLiteral("ssTriangle"),         int(QCPScatterStyle::ScatterShape::ssTriangle));
    cb_ScatterShapes->addItem(QStringLiteral("ssTriangleInverted"), int(QCPScatterStyle::ScatterShape::ssTriangleInverted));
    cb_ScatterShapes->addItem(QStringLiteral("ssCrossSquare"),      int(QCPScatterStyle::ScatterShape::ssCrossSquare));
    cb_ScatterShapes->addItem(QStringLiteral("ssPlusSquare"),       int(QCPScatterStyle::ScatterShape::ssPlusSquare));
    cb_ScatterShapes->addItem(QStringLiteral("ssCrossCircle"),      int(QCPScatterStyle::ScatterShape::ssCrossCircle));
    cb_ScatterShapes->addItem(QStringLiteral("ssPlusCircle"),       int(QCPScatterStyle::ScatterShape::ssPlusCircle));
    cb_ScatterShapes->addItem(QStringLiteral("ssPeace"),            int(QCPScatterStyle::ScatterShape::ssPeace));
    cb_ScatterShapes->addItem(QStringLiteral("ssArrow"),            int(QCPScatterStyle::ScatterShape::ssArrow));

    sb_ScatterSize=new QDoubleSpinBox;
    sb_ScatterSize->setRange(1,100);

    cb_penstyle = new QComboBox;
    cb_penstyle->addItem(QStringLiteral("SolidLine"));
    cb_penstyle->addItem(QStringLiteral("DashLine"));
    cb_penstyle->addItem(QStringLiteral("DotLine"));
    cb_penstyle->addItem(QStringLiteral("DashDotLine"));
    cb_penstyle->addItem(QStringLiteral("DashDotDotLine"));
    cb_penstyle->addItem(QStringLiteral("CustomDashLine"));

    cb_brushstyle = new QComboBox;
    cb_brushstyle->addItem(QStringLiteral("NoBrush"));
    cb_brushstyle->addItem(QStringLiteral("SolidPattern"));
    cb_brushstyle->addItem(QStringLiteral("Dense1Pattern"));
    cb_brushstyle->addItem(QStringLiteral("Dense2Pattern"));
    cb_brushstyle->addItem(QStringLiteral("Dense3Pattern"));
    cb_brushstyle->addItem(QStringLiteral("Dense4Pattern"));
    cb_brushstyle->addItem(QStringLiteral("Dense5Pattern"));
    cb_brushstyle->addItem(QStringLiteral("Dense6Pattern"));
    cb_brushstyle->addItem(QStringLiteral("Dense7Pattern"));
    cb_brushstyle->addItem(QStringLiteral("HorPattern"));
    cb_brushstyle->addItem(QStringLiteral("VerPattern"));
    cb_brushstyle->addItem(QStringLiteral("CrossPattern"));
    cb_brushstyle->addItem(QStringLiteral("BDiagPattern"));
    cb_brushstyle->addItem(QStringLiteral("DiagCrossPattern"));

    //QPushButton* pb_pen_color=new  QPushButton("Pen");
    cw_pen_color = new Color_Wheel;
    cw_brush_color = new Color_Wheel;
    //cd_pen_color->setOptions(QColorDialog::DontUseNativeDialog| QColorDialog::NoButtons);

    cb_gradient=new QGradientComboBox(nullptr);

    sb_pen_width=new QDoubleSpinBox();
    s_pen_alpha=new QDoubleSpinBox();
    s_pen_alpha->setRange(0,1.0);
    s_pen_alpha->setSingleStep(0.1);
    s_pen_alpha->setPrefix("alpha=");
    s_brush_alpha=new QDoubleSpinBox();
    s_brush_alpha->setRange(0,1.0);
    s_brush_alpha->setSingleStep(0.1);
    s_brush_alpha->setPrefix("alpha=");

    QGridLayout* g_style = new QGridLayout();
    QGroupBox* gb_style=new QGroupBox("Graphic Style");
    gb_style->setLayout(g_style);

    g_style->addWidget(new QLabel("Scatter shape : "),0,0);
    g_style->addWidget(new QLabel("Scatter size : "),1,0);
    g_style->addWidget(new QLabel("Line style : "),2,0);
    g_style->addWidget(new QLabel("Pen style : "),3,0);
    g_style->addWidget(new QLabel("Pen size : "),4,0);
    g_style->addWidget(new QLabel("Brush style : "),5,0);

    g_style->addWidget(cb_ScatterShapes,0,1);
    g_style->addWidget(sb_ScatterSize,1,1);
    g_style->addWidget(cb_itemLineStyleList,2,1);
    g_style->addWidget(cb_penstyle,3,1);
    g_style->addWidget(sb_pen_width,4,1);
    g_style->addWidget(cb_brushstyle,5,1);

    QLabel* l_pen_color=new QLabel("Pen color");
    QLabel* l_brush_color=new QLabel("Brush color");
    l_pen_color->setAlignment(Qt::AlignHCenter);
    l_brush_color->setAlignment(Qt::AlignHCenter);

    g_style->addWidget(l_pen_color,6,0);
    g_style->addWidget(l_brush_color,6,1);
    g_style->addWidget(cw_pen_color,7,0);
    g_style->addWidget(cw_brush_color,7,1);
    g_style->addWidget(s_pen_alpha,8,0);
    g_style->addWidget(s_brush_alpha,8,1);
    g_style->addWidget(cb_gradient,9,0,1,2);

    QGridLayout* g_axis = new QGridLayout();
    QGroupBox* gb_axis=new QGroupBox("Axis");
    gb_axis->setLayout(g_axis);

    g_axis->addWidget(new QLabel("X Type"),0,0);
    g_axis->addWidget(new QLabel("Y Type"),1,0);
    g_axis->addWidget(cb_scale_mode_x,0,1);
    g_axis->addWidget(cb_scale_mode_y,1,1);

    gbox->addWidget(gb_axis,0,0);
    gbox->addWidget(gb_style,1,0);

    widget->setLayout(gbox);

    QObject::connect(s_pen_alpha, SIGNAL(valueChanged(double)), this, SLOT(slot_setPenAlpha(double)));
    QObject::connect(s_brush_alpha, SIGNAL(valueChanged(double)), this, SLOT(slot_setBrushAlpha(double)));

    QObject::connect(cw_brush_color, SIGNAL(colorChanged(QColor)), this, SLOT(slot_setBrushColor(QColor)));
    QObject::connect(sb_pen_width, SIGNAL(valueChanged(double)), this, SLOT(slot_setPenWidth(double)));
    QObject::connect(cw_pen_color, SIGNAL(colorChanged(QColor)), this, SLOT(slot_setPenColor(QColor)));
    QObject::connect(cb_penstyle, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setPenStyle(int)));
    QObject::connect(cb_brushstyle, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setBrushStyle(int)));
    QObject::connect(cb_ScatterShapes, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setScatterShape(int)));
    QObject::connect(sb_ScatterSize, SIGNAL(valueChanged(double) ), this, SLOT(slot_setScatterSize(double)));
    QObject::connect(cb_itemLineStyleList, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setStyle(int)));
    QObject::connect(cb_scale_mode_x, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setAxisXType(int)));
    QObject::connect(cb_scale_mode_y, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setAxisYType(int)));

    QObject::connect(cb_gradient, SIGNAL(currentIndexChanged(int) ), this, SLOT(slot_setScalarFieldGradientType(int)));

    return actWidget;
}

void Viewer1D::createPopup()
{
    popup_menu=new QMenu(this);
    menuGadgets=new QMenu("Gadgets",this);
    menuFit=new QMenu("Fit",this);
    menuLegend=new QMenu("Legend",this);
    menuScalarField=new QMenu("Scalar Field",this);
    menuScalarFieldFit=new QMenu("Fit",this);
    menuParameters=new QMenu("Parameters",this);
    menuAnalyse=new QMenu("Analyse",this);
    menuFilters=new QMenu("Filter",this);

    actCopy   = new QAction("Copy",  this);
    actPaste   = new QAction("Paste",  this);
    actSave   = new QAction("Save",  this);
    actRescale= new QAction("Rescale",  this);
    actDelete= new QAction("Delete",  this);
    actClearGadgets   = new QAction("Clear gadgets",  this);
    actGadgetArrow= new QAction("Arrow",  this);
    actGadgetText= new QAction("Text",  this);
    actGadgetMark= new QAction("Mark",  this);
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
    actFitPolynomial2V= new QAction("Polynomial in two variables",  this);
    actDistance= new QAction("Distances",  this);

    actAutoColor= new QAction("Auto color",  this);
    actFilterMedian=new QAction("Median Filter",this);
    actFilterMean=new QAction("Mean Filter",this);

    this->addAction(actCopy);
    this->addAction(actPaste);
    this->addAction(actSave);
    this->addAction(actRescale);
    this->addAction(actDelete);
    this->addAction(actClearGadgets);
    this->addAction(actGadgetArrow);
    this->addAction(actGadgetText);
    this->addAction(actGadgetMark);
    this->addAction(actLegendShowHide);
    this->addAction(actLegendTopBottom);
    this->addAction(actLegendLeftRight);
    this->addAction(actStatistiques);
    this->addAction(actAutoColor);

    actAutoColor->setShortcutVisibleInContextMenu(true);
    actCopy->setShortcutVisibleInContextMenu(true);
    actPaste->setShortcutVisibleInContextMenu(true);
    actSave->setShortcutVisibleInContextMenu(true);
    actRescale->setShortcutVisibleInContextMenu(true);
    actDelete->setShortcutVisibleInContextMenu(true);
    actClearGadgets->setShortcutVisibleInContextMenu(true);
    actGadgetArrow->setShortcutVisibleInContextMenu(true);
    actGadgetText->setShortcutVisibleInContextMenu(true);
    actGadgetMark->setShortcutVisibleInContextMenu(true);
    actLegendShowHide->setShortcutVisibleInContextMenu(true);
    actLegendTopBottom->setShortcutVisibleInContextMenu(true);
    actLegendLeftRight->setShortcutVisibleInContextMenu(true);

    actLegendShowHide->setCheckable(true);
    actLegendShowHide->setChecked(true);
    actLegendTopBottom->setCheckable(true);
    actLegendTopBottom->setChecked(true);
    actLegendLeftRight->setCheckable(true);
    actLegendLeftRight->setChecked(true);

    popup_menu->addMenu(menuParameters);
    popup_menu->addSeparator();
    popup_menu->addAction(actSave);
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

    menuGadgets->addAction(actGadgetMark);
    menuGadgets->addAction(actGadgetText);
    menuGadgets->addAction(actGadgetArrow);
    menuGadgets->addSeparator();
    menuGadgets->addAction(actClearGadgets);

    menuFit->addAction(actFitPolynomial);
    menuFit->addAction(actFitGaussian);
    menuFit->addAction(actFitSigmoid);
    menuFit->addAction(actFitSinusoide);

    menuLegend->addAction(actLegendShowHide);
    menuLegend->addAction(actLegendTopBottom);
    menuLegend->addAction(actLegendLeftRight);

    menuScalarField->addMenu(menuScalarFieldFit);
    menuScalarField->addAction(actDistance);
    menuScalarFieldFit->addAction(actFitPolynomial2V);

    menuParameters->addAction(actAutoColor);
    menuParameters->addMenu(menuLegend);
    menuParameters->addAction(createParametersWidget());

    menuAnalyse->addMenu(menuFilters);
    menuAnalyse->addMenu(menuFit);
    menuAnalyse->addMenu(menuScalarField);
    menuAnalyse->addAction(actStatistiques);
    menuAnalyse->addAction(actSvd);
    menuAnalyse->addAction(actCovariance);



    connect(actGadgetMark,SIGNAL(triggered()),this,SLOT(slot_gadgetMark()));
    connect(actGadgetText,SIGNAL(triggered()),this,SLOT(slot_gadgetText()));
    connect(actGadgetArrow,SIGNAL(triggered()),this,SLOT(slot_gadgetArrow()));
    connect(actClearGadgets,SIGNAL(triggered()),this,SLOT(slot_clear_marks()));
    connect(actStatistiques,SIGNAL(triggered()),this,SLOT(slot_statistiques()));
    connect(actSvd,SIGNAL(triggered()),this,SLOT(slot_svd()));
    connect(actCovariance,SIGNAL(triggered()),this,SLOT(slot_covariance()));

    connect(actDistance,SIGNAL(triggered()),this,SLOT(slot_Distance()));

    connect(actSave,SIGNAL(triggered()),this,SLOT(slot_save_image()));
    connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_rescale()));
    connect(actFitPolynomial,SIGNAL(triggered()),this,SLOT(slot_fit_polynomial()));
    connect(actFitPolynomial2V,SIGNAL(triggered()),this,SLOT(slot_fit_2var_polynomial()));
    connect(actFitGaussian,SIGNAL(triggered()),this,SLOT(slot_fit_gaussian()));
    connect(actFitSinusoide,SIGNAL(triggered()),this,SLOT(slot_fit_sinusoide()));
    connect(actFitSigmoid,SIGNAL(triggered()),this,SLOT(slot_fit_sigmoid()));
    connect(actCopy,SIGNAL(triggered()),this,SLOT(slot_copy()));
    connect(actPaste,SIGNAL(triggered()),this,SLOT(slot_paste()));
    connect(actDelete,SIGNAL(triggered()),this,SLOT(slot_delete()));
    connect(actLegendShowHide,SIGNAL(toggled(bool)),this,SLOT(slot_show_legend(bool)));
    connect(actLegendTopBottom,SIGNAL(toggled(bool)),this,SLOT(slot_top_legend(bool)));
    connect(actLegendLeftRight,SIGNAL(toggled(bool)),this,SLOT(slot_left_legend(bool)));
    connect(actAutoColor,SIGNAL(triggered()),this,SLOT(slot_auto_color()));

    connect(actFilterMean,SIGNAL(triggered()),this,SLOT(slot_meanFilter()));
    connect(actFilterMedian,SIGNAL(triggered()),this,SLOT(slot_medianFilter()));


}

void Viewer1D::slot_auto_color()
{
    QVector<QColor> color;
    color<<QColor(160,0,0);
    color<<QColor(0,160,0);
    color<<QColor(0,0,160);
    color<<QColor(255,160,0);
    color<<QColor(255,0,255);
    color<<QColor(0,160,128);
    color<<QColor(128,255,0);
    color<<QColor(128,0,255);
    color<<QColor(0,128,255);
    color<<QColor(0,0,0);

    QList<QCPAbstractPlottable*> plottables=this->plottables();

    for (int i=0; i<plottables.size(); i++)
    {
        int index=i%color.size();
        QPen pen=plottables[i]->pen();
        color[index].setAlphaF(pen.color().alphaF());
        pen.setColor(color[index]);
        plottables[i]->setPen(pen);
    }

    replot();
}

void Viewer1D::slot_setStyle(int style)
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();
    QList<QCPCurve*> curveslist=this->getSelectedQCPCurves();

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


void Viewer1D::slot_setScatterShape(int scatter_shape)
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();
    QList<QCPCurve*> curveslist=this->getSelectedQCPCurves();

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
    QList<QCPCurve*> curveslist=this->getSelectedQCPCurves();

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
void Viewer1D::addLabel(double cx, double cy)
{
    double mx=this->xAxis->range().lower;
    double my=this->yAxis->range().lower;

    QPen linePen(QColor(200,200,200));

    QCPItemLine* lineX= new QCPItemLine(this);
    lineX->start->setType(QCPItemPosition::ptPlotCoords);
    lineX->end->setType(QCPItemPosition::ptPlotCoords);
    lineX->start->setCoords(cx, my);
    lineX->end->setCoords(cx, cy);
    lineX->setPen(linePen);

    QCPItemLine* lineY= new QCPItemLine(this);
    lineY->start->setType(QCPItemPosition::ptPlotCoords);
    lineY->end->setType(QCPItemPosition::ptPlotCoords);
    lineY->start->setCoords(mx, cy);
    lineY->end->setCoords(cx, cy);
    lineY->setPen(linePen);

    // add the text label at the top:
    QCPItemText* coordtextX = new QCPItemText(this);
    coordtextX->position->setType(QCPItemPosition::ptPlotCoords);
    coordtextX->setPositionAlignment(Qt::AlignLeft);
    coordtextX->position->setCoords(cx, my); // lower right corner of axis rect
    coordtextX->setRotation(-90);
    coordtextX->setText(QString("%1").arg(cx));
    coordtextX->setTextAlignment(Qt::AlignLeft);
    coordtextX->setFont(QFont(font().family(), 9));
    coordtextX->setPadding(QMargins(8, 0, 0, 0));

    QCPItemText* coordtextY = new QCPItemText(this);
    coordtextY->position->setType(QCPItemPosition::ptPlotCoords);
    coordtextY->setPositionAlignment(Qt::AlignLeft|Qt::AlignBottom);
    coordtextY->position->setCoords(mx, cy); // lower right corner of axis rect
    coordtextY->setText(QString("%1").arg(cy));
    coordtextY->setTextAlignment(Qt::AlignLeft);
    coordtextY->setFont(QFont(font().family(), 9));
    coordtextY->setPadding(QMargins(8, 0, 0, 0));

    this->replot();
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


void Viewer1D::slot_axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part)
{
    // Set an axis label by double clicking on it
    if (part == QCPAxis::spAxisLabel || part == QCPAxis::spAxis || part == QCPAxis::spTickLabels) // only react when the actual axis label is clicked, not tick label or axis backbone
    {
        bool ok;
        QString newLabel = QInputDialog::getText(this, "Set legend", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
        if (ok)
        {
            axis->setLabel(newLabel);
            replot();
        }
    }
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
        QDoubleSpinBox* A,*F,*P;
        QDialog* dialog=Sinusoide::createDialog(A,F,P);
        A->setValue(curves[0].getRms()*sqrt(2.0));
        F->setValue(curves[0].guessMainFrequency());
        int result=dialog->exec();

        if (result == QDialog::Accepted)
        {
            Sinusoide sinusoide(A->value(),F->value(),P->value());
            curves[0].fit(&sinusoide);
            sinusoide.regularized();
            Eigen::VectorXd X=curves[0].getLinX(1000);
            Eigen::VectorXd Y=sinusoide.at(X);

            QString result_str=QString("A=%1 F=%2[Hz] P=%3[°]").arg(sinusoide.getA()).arg(sinusoide.getF()).arg(sinusoide.getP()*180/M_PI);
            QString expression=QString("%1*sin(2*pi*%2*X+%3)").arg(sinusoide.getA()).arg(sinusoide.getF()).arg(sinusoide.getP());
            Curve2D fit_curve(X,Y,QString("Fit Sinusoid : %1").arg(result_str),Curve2D::GRAPH);

            slot_add_data(fit_curve);

            emit sig_displayResults(QString("Fit Sinusoid :\n%1\nF(X)=%2\nRms=%3").arg(result_str).arg(expression).arg(sinusoide.getRMS()));
            emit sig_newColumn(QString("Err_Sinusoid"),sinusoide.getErrNorm());
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
        int index=static_cast<int>(std::round((data[k]-min)/delta));

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
    hist_curve.getStyle().mLineStyle=QCPGraph::lsImpulse;
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

    this->replot();
}

QList<QCPCurve*> Viewer1D::getSelectedQCPCurves()
{
    QList<QCPAbstractPlottable*> plottableslist=this->selectedPlottables();
    QList<QCPCurve*> listcurves;
    for (int i=0; i<plottableslist.size(); i++)
    {
        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if (currentcurve)
        {
            listcurves.push_back(currentcurve);
        }
    }
    return listcurves;
}

QList<QCPCurve*> Viewer1D::getQCPCurves()
{
    QList<QCPAbstractPlottable*> plottableslist=this->plottables();
    QList<QCPCurve*> listcurves;
    for (int i=0; i<plottableslist.size(); i++)
    {
        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if (currentcurve)
        {
            listcurves.push_back(currentcurve);
        }
    }
    return listcurves;
}

QList<QCPAbstractPlottable*> Viewer1D::getSelectedCurvesOrGraphs()
{
    QList<QCPAbstractPlottable*> plottableslist=this->selectedPlottables();
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

    for (int i=0,id=0; i<plottableslist.size(); i++)
    {
        QVector<double> xv,yv;

        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if (currentcurve)
        {
            Curve2D curve;
            curve.fromQCP(currentcurve);
            curvelist.push_back(curve);
            id++;
        }

        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(plottableslist[i]);
        if (currentgraph)
        {
            Curve2D curve;
            curve.fromQCP(currentgraph);
            curvelist.push_back(curve);
            id++;
        }

    }

    return curvelist;
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
    QList<QCPCurve*> listcurves=getQCPCurves();
    for (int i=0; i<listcurves.size(); i++)
    {
        if (listcurves[i]->getColorScale())
        {
            listcurves[i]->getColorScale()->setDataRange(listcurves[0]->getScalarFieldRange());
        }
    }

    this->rescaleAxes();
    this->replot();
}

void Viewer1D::slot_copy()
{
    QList<Curve2D> list=getSelectedCurves();

    if (list.size()>0 && sharedBuf!=nullptr)
    {
        //*sharedBuf=list[0];
        //sharedBuf


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
    shortcuts_links.insert(QString("Graph-Save"),actSave);
    shortcuts_links.insert(QString("Graph-Rescale"),actRescale);
    shortcuts_links.insert(QString("Graph-Delete"),actDelete);
    shortcuts_links.insert(QString("Graph-Legend-Show/Hide"),actLegendShowHide);
    shortcuts_links.insert(QString("Graph-Legend-Top/Bottom"),actLegendTopBottom);
    shortcuts_links.insert(QString("Graph-Legend-Left/Right"),actLegendLeftRight);
    shortcuts_links.insert(QString("Graph-AutoColor"),actAutoColor);

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
    rescaleAxes();
    replot();
}

void Viewer1D::slot_setScalarFieldGradientType(int type)
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();
    QList<QCPCurve*> curveslist=this->getSelectedQCPCurves();

    for (int i=0; i<graphslist.size(); i++)
    {
        graphslist[0]->setScalarFieldGradientType(type);
    }
    for (int i=0; i<curveslist.size(); i++)
    {
        curveslist[0]->setScalarFieldGradientType(type);
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
    QList<QCPCurve*> qcp_curves=getSelectedQCPCurves();
    if (curves.size()>0)
    {
        bool ok;
        double knn=QInputDialog::getInt(this,"Distances","Evaluate the mean distance of K nearest neighbors.\n\n K=",20,0,1e100,1,&ok);
        if(ok)
        {
            curves[0].distance(knn);
            qcp_curves[0]->setScalarField(curves[0].getQScalarField());
            replot();

            emit sig_newColumn(QString("Distances"),curves[0].getScalarField());
        }
    }

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
