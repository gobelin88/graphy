#include "AppearanceDialog.h"

AppearanceDialog::AppearanceDialog(QWidget *parent)
    :QDialog(parent)
{
    QWidget* widget=new QWidget(this);
    widget->setMinimumWidth(400);

    QGridLayout* gbox = new QGridLayout();

    cb_itemLineStyleList = new QComboBox(widget);
    cb_itemLineStyleList->addItem(QStringLiteral("None"),        int(QCPGraph::LineStyle::lsNone));
    cb_itemLineStyleList->addItem(QStringLiteral("Line"),        int(QCPGraph::LineStyle::lsLine));
    cb_itemLineStyleList->addItem(QStringLiteral("StepLeft"),    int(QCPGraph::LineStyle::lsStepLeft));
    cb_itemLineStyleList->addItem(QStringLiteral("StepRight"),   int(QCPGraph::LineStyle::lsStepRight));
    cb_itemLineStyleList->addItem(QStringLiteral("StepCenter"),  int(QCPGraph::LineStyle::lsStepCenter));
    cb_itemLineStyleList->addItem(QStringLiteral("Impulse"),     int(QCPGraph::LineStyle::lsImpulse));

    cb_ScatterShapes = new ScatterComboBox(widget);
    sb_ScatterSize=new QDoubleSpinBox(widget);
    sb_ScatterSize->setRange(1,100);

    cb_penStyle = new PenStyleComboBox(widget);

    cb_brushStyle = new QComboBox(widget);
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
    cw_pen_color = new ColorWheel(widget);
    cw_brush_color = new ColorWheel(widget);
    //cd_pen_color->setOptions(QColorDialog::DontUseNativeDialog| QColorDialog::NoButtons);
    cw_pen_color->setMinimumSize(QSize(128,128));
    cw_brush_color->setMinimumSize(QSize(128,128));

    cb_gradient=new MyGradientComboBox(widget);

    sb_penWidth=new QDoubleSpinBox(widget);
    s_pen_alpha=new QDoubleSpinBox(widget);
    s_pen_alpha->setRange(0,1.0);
    s_pen_alpha->setSingleStep(0.1);
    s_pen_alpha->setPrefix("alpha=");
    s_brush_alpha=new QDoubleSpinBox(widget);
    s_brush_alpha->setRange(0,1.0);
    s_brush_alpha->setSingleStep(0.1);
    s_brush_alpha->setPrefix("alpha=");

    QGridLayout* g_styleCurve = new QGridLayout();

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

    widget->setLayout(g_styleCurve);

    //-------------------------------------
    this->setLocale(QLocale("C"));
    this->setWindowTitle("Appearance");

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    gbox->addWidget(widget,0,0);
    gbox->addWidget(buttonBox,1,0);

    this->setLayout(gbox);
}

void AppearanceDialog::configure(QList<QCPAbstractPlottable*> plottables)
{
    cb_gradient->hide();

    if (plottables.size()>0)
    {
        setWindowTitle("Appearance of : "+plottables[0]->name());

        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(plottables[0]);
        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(plottables[0]);
        QCPColorMap* currentmap=dynamic_cast<QCPColorMap*>(plottables[0]);
        if (currentmap)
        {
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
