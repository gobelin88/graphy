#include "1d_viewer.h"

Viewer1D::Viewer1D(Curve2D * sharedBuf)
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

    connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress(QMouseEvent*)));
    connect(this, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoublePress(QMouseEvent*)));

    pen_select.setColor(colors[2]);
    pen_select.setStyle(Qt::SolidLine);

    colorScale=nullptr;
}

Viewer1D::~Viewer1D()
{

}

unsigned int Viewer1D::getId()
{
    unsigned int id=(this->plottableCount()-1)%colors.size();
    return id;
}

void Viewer1D::newGraph(QString name)
{
    this->addGraph();
    QPen pen_model(colors[getId()]);

    pen_model.setStyle(Qt::SolidLine);
    this->graph()->setPen(pen_model);
    this->graph()->setLineStyle(QCPGraph::lsLine);
    this->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 10));
    this->rescaleAxes();
    legend->setVisible(true);

    this->graph()->setSelectable(QCP::stWhole);

    QCPSelectionDecorator * decorator_select=new QCPSelectionDecorator;
    decorator_select->setPen(pen_select);
    this->graph()->setSelectionDecorator(decorator_select);
    graph()->setName(name);
}

void Viewer1D::slot_add_data_graph(const Curve2D & datacurve)
{
    newGraph(datacurve.name());

    graph()->setData(datacurve.getX(), datacurve.getY());

    rescaleAxes();
    replot();
}

void Viewer1D::colorScaleChanged(const QCPRange & range)
{
    QList<QCPCurve*> listcurves=getQCPCurves();

    if(listcurves.size()>0)
    {
        listcurves[0]->setGradientRange(range);
    }
}

void Viewer1D::slot_add_data_cloud(const Curve2D & datacurve)
{
    QCPCurve *newCurve = new QCPCurve(this->xAxis, this->yAxis);

    newCurve->setScalarField(datacurve.getScalarField());

    newCurve->setName(datacurve.name());
    newCurve->setData(datacurve.getX(), datacurve.getY());
    QPen pen_model(colors[getId()]);
    pen_model.setStyle(Qt::SolidLine);
    newCurve->setPen(pen_model);
    newCurve->setLineStyle(QCPCurve::lsNone);
    newCurve->setScatterStyle(QCPScatterStyle::ScatterShape::ssDisc);

    QCPSelectionDecorator * decorator_select=new QCPSelectionDecorator;
    decorator_select->setPen(pen_select);
    newCurve->setSelectionDecorator(decorator_select);
    legend->setVisible(true);

    colorScale = new QCPColorScale(this);
    this->plotLayout()->addElement(0, 1, colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorScale->setDataRange(newCurve->getScalarFieldRange());
    colorScale->setGradient(newCurve->getGradient());
    //colorScale->setRangeDrag(false);
    //colorScale->setRangeZoom(false);

    connect(colorScale,SIGNAL(dataRangeChanged(const QCPRange &)),this,SLOT(colorScaleChanged(const QCPRange &)));

    QCPMarginGroup * marginGroup = new QCPMarginGroup(this);
    this->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    axisRect()->setupFullAxesBox();
    rescaleAxes();
    replot();
}

void Viewer1D::slot_add_data_curve(const Curve2D & datacurve)
{
    QCPCurve *newCurve = new QCPCurve(this->xAxis, this->yAxis);

    newCurve->setName(datacurve.name());
    newCurve->setData(datacurve.getX(), datacurve.getY());
    QPen pen_model(colors[getId()]);
    pen_model.setStyle(Qt::SolidLine);
    newCurve->setPen(pen_model);

    QCPSelectionDecorator * decorator_select=new QCPSelectionDecorator;
    decorator_select->setPen(pen_select);
    newCurve->setSelectionDecorator(decorator_select);
    legend->setVisible(true);

    axisRect()->setupFullAxesBox();
    rescaleAxes();
    replot();
}


void Viewer1D::createPopup()
{
    popup_menu=new QMenu(this);

    actCopy   = new QAction("Copy",  this);
    actCopy->setShortcut(QKeySequence("Ctrl+C"));
    actPaste   = new QAction("Paste",  this);
    actPaste->setShortcut(QKeySequence("Ctrl+V"));

    actClearMarks   = new QAction("Clear Marks",  this);
    actClearMarks->setShortcut(QKeySequence("Space"));

    actSave   = new QAction("Save",  this);
    actSave->setShortcut(QKeySequence("S"));
    actRescale= new QAction("Rescale",  this);
    actRescale->setShortcut(QKeySequence("R"));
    actDelete= new QAction("Delete",  this);
    actDelete->setShortcut(QKeySequence("Del"));
    actColor= new QAction("Color",  this);
    actColor->setShortcut(QKeySequence("C"));
    actStyle= new QAction("Style",  this);
    actStyle->setShortcut(QKeySequence("!"));

    actLegendShow= new QAction("Hide",  this);
    actLegendShow->setCheckable(true);
    actLegendShow->setChecked(true);
    actLegendShow->setShortcut(QKeySequence("L"));

    actLegendTop= new QAction("Move bottom",  this);
    actLegendTop->setShortcut(QKeySequence("Ctrl+L"));
    actLegendTop->setCheckable(true);
    actLegendTop->setChecked(true);

    this->addAction(actCopy);
    this->addAction(actPaste);
    this->addAction(actSave);
    this->addAction(actRescale);
    this->addAction(actDelete);
    this->addAction(actLegendShow);
    this->addAction(actClearMarks);
    this->addAction(actColor);
    this->addAction(actStyle);
    this->addAction(actLegendTop);

    popup_menu->addAction(actCopy);
    popup_menu->addAction(actPaste);
    popup_menu->addAction(actSave);
    popup_menu->addAction(actDelete);
    popup_menu->addSeparator();
    popup_menu->addAction(actRescale);
    popup_menu->addAction(actClearMarks);
    popup_menu->addSeparator();

    menu_fit=new QMenu("Fit",popup_menu);
    menu_mathematics=new QMenu("Mathematics",menu_fit);
    menu_electronics=new QMenu("Electronics",menu_fit);
    menu_legend=new QMenu("Legend",menu_fit);

    actFitLinear= new QAction("Linear",  this);
    actFitPolynome= new QAction("Polynome",  this);
    actFitGaussian= new QAction("Gaussian",  this);
    actFitSigmoid= new QAction("Sigmoid",  this);

    actFitRLC= new QAction("RLC",  this);
    actFitSinusoide= new QAction("Sinusoide",  this);

    menu_mathematics->addAction(actFitLinear);
    menu_mathematics->addAction(actFitPolynome);
    menu_mathematics->addAction(actFitGaussian);
    menu_mathematics->addAction(actFitSigmoid);

    menu_electronics->addAction(actFitRLC);
    menu_electronics->addAction(actFitSinusoide);

    menu_legend->addAction(actLegendShow);
    menu_legend->addAction(actLegendTop);
    menu_legend->addAction(actStyle);

    popup_menu->addMenu(menu_fit);
    popup_menu->addMenu(menu_legend);
    menu_fit->addMenu(menu_mathematics);
    menu_fit->addMenu(menu_electronics);

    connect(actSave,SIGNAL(triggered()),this,SLOT(slot_save_image()));
    connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_rescale()));
    connect(actFitLinear,SIGNAL(triggered()),this,SLOT(slot_fit_linear()));
    connect(actFitPolynome,SIGNAL(triggered()),this,SLOT(slot_fit_polynome()));
    connect(actFitRLC,SIGNAL(triggered()),this,SLOT(slot_fit_rlc()));
    connect(actFitGaussian,SIGNAL(triggered()),this,SLOT(slot_fit_gaussian()));
    connect(actFitSinusoide,SIGNAL(triggered()),this,SLOT(slot_fit_sinusoide()));
    connect(actFitSigmoid,SIGNAL(triggered()),this,SLOT(slot_fit_sigmoid()));
    connect(actColor,SIGNAL(triggered()),this,SLOT(slot_set_color()));

    connect(actCopy,SIGNAL(triggered()),this,SLOT(slot_copy()));
    connect(actPaste,SIGNAL(triggered()),this,SLOT(slot_paste()));

    connect(actDelete,SIGNAL(triggered()),this,SLOT(slot_delete()));
    connect(actLegendShow,SIGNAL(toggled(bool)),this,SLOT(slot_show_legend(bool)));
    connect(actLegendTop,SIGNAL(toggled(bool)),this,SLOT(slot_top_legend(bool)));
    connect(actClearMarks,SIGNAL(triggered()),this,SLOT(slot_clear_marks()));
    connect(actStyle,SIGNAL(triggered()),this,SLOT(slot_set_style()));
}

void Viewer1D::slot_clear_marks()
{
    std::cout<<"clear items "<<this->itemCount()<<std::endl;
    this->clearItems();

    this->replot();
}


void Viewer1D::mousePress(QMouseEvent * event)
{
    if(event->button() == Qt::RightButton)
    {
        popup_menu->exec(mapToGlobal(event->pos()));
    }
}

void Viewer1D::mouseDoublePress(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
    {
        double cx=this->xAxis->pixelToCoord(event->x());
        double cy=this->yAxis->pixelToCoord(event->y());

        double mx=this->xAxis->range().lower;
        double my=this->yAxis->range().lower;

        std::cout<<cx<<" "<<cy<<std::endl;

        QPen linePen(QColor(200,200,200));

        QCPItemLine * lineX= new QCPItemLine(this);
        lineX->start->setType(QCPItemPosition::ptPlotCoords);
        lineX->end->setType(QCPItemPosition::ptPlotCoords);
        lineX->start->setCoords(cx, my);
        lineX->end->setCoords(cx, cy);
        lineX->setPen(linePen);

        QCPItemLine * lineY= new QCPItemLine(this);
        lineY->start->setType(QCPItemPosition::ptPlotCoords);
        lineY->end->setType(QCPItemPosition::ptPlotCoords);
        lineY->start->setCoords(mx, cy);
        lineY->end->setCoords(cx, cy);
        lineY->setPen(linePen);

        // add the text label at the top:
        QCPItemText *coordtextX = new QCPItemText(this);
        coordtextX->position->setType(QCPItemPosition::ptPlotCoords);
        coordtextX->setPositionAlignment(Qt::AlignLeft);
        coordtextX->position->setCoords(cx, my); // lower right corner of axis rect
        coordtextX->setRotation(-90);
        coordtextX->setText(QString("%1").arg(cx));
        coordtextX->setTextAlignment(Qt::AlignLeft);
        coordtextX->setFont(QFont(font().family(), 9));
        coordtextX->setPadding(QMargins(8, 0, 0, 0));

        QCPItemText *coordtextY = new QCPItemText(this);
        coordtextY->position->setType(QCPItemPosition::ptPlotCoords);
        coordtextY->setPositionAlignment(Qt::AlignLeft|Qt::AlignBottom);
        coordtextY->position->setCoords(mx, cy); // lower right corner of axis rect
        coordtextY->setText(QString("%1").arg(cy));
        coordtextY->setTextAlignment(Qt::AlignLeft);
        coordtextY->setFont(QFont(font().family(), 9));
        coordtextY->setPadding(QMargins(8, 0, 0, 0));

        this->replot();
    }
}


void Viewer1D::slot_fit_linear()
{
    QList<Curve2D> curves=getSelectedCurves();

    for(int i=0;i<curves.size();i++)
    {
        Eigen::VectorXd C=curves[i].fit(2);

        QVector<double> X=curves[i].getX();
        QVector<double> Y=curves[i].at(C,X);

        slot_add_data_graph(Curve2D(X,Y,QString("Fit Linear Y=%1 X + %2").arg(C[1]).arg(C[0])));
    }
}

void Viewer1D::slot_fit_sinusoide()
{
    QList<Curve2D> curves=getSelectedCurves();

    QDoubleSpinBox *A,*F,*P;
    QDialog * dialog=Sinusoide::createDialog(A,F,P);
    A->setValue(0.0001);
    F->setValue(0.0001);
    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        Sinusoide sinusoide(A->value(),F->value(),P->value());

        for(int i=0;i<curves.size();i++)
        {
            curves[i].fit(&sinusoide);

            sinusoide.regularized();

            QVector<double> X=curves[i].getLinX(1000);
            QVector<double> Y=sinusoide.at(X);

            slot_add_data_graph(Curve2D(X,Y,QString("Fit A=%1 F=%2[Hz] P=%3[°]").arg(sinusoide.getA()).arg(sinusoide.getF()).arg(sinusoide.getP()*180/M_PI)) );
        }
    }
}

void Viewer1D::slot_fit_sigmoid()
{
    QList<Curve2D> curves=getSelectedCurves();

    QDoubleSpinBox *A,*B,*C,*P,*As;
    QDialog * dialog=Sigmoid::createDialog(A,B,C,P,As);
    A->setValue(-1);
    B->setValue(1);
    C->setValue(0);
    P->setValue(1);
    As->setValue(0);
    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        Sigmoid sigmoid(A->value(),B->value(),C->value(),P->value(),As->value());

        for(int i=0;i<curves.size();i++)
        {
            curves[i].fit(&sigmoid);

            QVector<double> X=curves[i].getLinX(1000);
            QVector<double> Y=sigmoid.at(X);

            slot_add_data_graph(Curve2D(X,Y,QString("Fit A=%1 B=%2 C=%3 P=%4 As=%5").arg(sigmoid.getA()).arg(sigmoid.getB()).arg(sigmoid.getC()).arg(sigmoid.getP()).arg(sigmoid.getAsymp())) );
        }
    }
}

void Viewer1D::slot_fit_gaussian()
{
    QList<Curve2D> curves=getSelectedCurves();

    QDoubleSpinBox *S,*M,*K;
    QDialog * dialog=Gaussian::createDialog(S,M,K);
    S->setValue(10);
    M->setValue(0);
    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        Gaussian gaussian(S->value(),M->value(),K->value());

        for(int i=0;i<curves.size();i++)
        {
            curves[i].fit(&gaussian);
            QVector<double> X=curves[i].getLinX(1000);
            QVector<double> Y=gaussian.at(X);

            slot_add_data_graph(Curve2D(X,Y,QString("Fit Sigma=%1 Mean=%2 K=%3").arg(gaussian.getS()).arg(gaussian.getM()).arg(gaussian.getK())));
        }
    }
}

void Viewer1D::slot_fit_rlc()
{
    QList<Curve2D> curves=getSelectedCurves();

    QDoubleSpinBox *R,*L,*C;
    QCheckBox *fixedR,*fixedL,*fixedC;

    QDialog * dialog=RLpC::createDialog(R,L,C,fixedR,fixedL,fixedC);
    R->setValue(200);
    L->setValue(10);
    C->setValue(2);
    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        RLpC rlpc(R->value(),L->value(),C->value(),fixedR->isChecked(),fixedL->isChecked(),fixedC->isChecked());

        for(int i=0;i<curves.size();i++)
        {
            curves[i].fit(&rlpc);
            QVector<double> X=curves[i].getLinX(1000);
            QVector<double> Y=rlpc.at(X);

            slot_add_data_graph(Curve2D(X,Y,QString("Fit R=%1Ohm L=%2mH C=%3nF Fc=%4Hz").arg(rlpc.getR()).arg(rlpc.getL()).arg(rlpc.getC()).arg(rlpc.getFc())));
        }
    }
}

void Viewer1D::slot_fit_polynome()
{
    int order=1+QInputDialog::getInt(this,"Order of the polynome","Order=",2,1,20,1);
    QList<Curve2D> curves=getSelectedCurves();

    std::cout<<curves.size()<<std::endl;

    for(int i=0;i<curves.size();i++)
    {
        Eigen::VectorXd C=curves[i].fit(order);

        QVector<double> X=curves[i].getLinX(1000);
        QVector<double> Y=curves[i].at(C,X);

        QString name("Fit Polynome Coeffs=");
        for(int k=0;k<order;k++){name+=QString("%1 ").arg(C[order-k-1]);}

        slot_add_data_graph(Curve2D(X,Y,name));
    }
}

void Viewer1D::slot_show_legend(bool value)
{
    legend->setVisible(value);
    replot();

    if(value)
    {
        actLegendShow->setText("Hide");
    }
    else
    {
        actLegendShow->setText("Show");
    }
}

void Viewer1D::slot_top_legend(bool value)
{
    if(value)
    {
        this->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
        actLegendTop->setText("Move bottom");
    }
    else
    {
        this->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
        actLegendTop->setText("Move top");
    }

    replot();
}

void Viewer1D::slot_set_color()
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();

    if(graphslist.size()>0)
    {
        QColor color=QColorDialog::getColor(graphslist[0]->pen().color(),this,"Get Color");

        graphslist[0]->setPen(QPen(color));
    }
}

void Viewer1D::slot_set_style()
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();
    QList<QCPCurve*> curveslist=this->getSelectedQCPCurves();

    if(graphslist.size()==0 && curveslist.size()==0)return;

    QDialog * dialog=new QDialog;
    QComboBox *itemLineStyleList = new QComboBox(dialog);

    if(curveslist.size()>0)
    {
        itemLineStyleList->addItem(QStringLiteral("lsNone"),        int(QCPCurve::LineStyle::lsNone));
        itemLineStyleList->addItem(QStringLiteral("lsLine"),        int(QCPCurve::LineStyle::lsLine));
    }
    else
    {
        itemLineStyleList->addItem(QStringLiteral("lsNone"),        int(QCPGraph::LineStyle::lsNone));
        itemLineStyleList->addItem(QStringLiteral("lsLine"),        int(QCPGraph::LineStyle::lsLine));
        itemLineStyleList->addItem(QStringLiteral("lsStepLeft"),    int(QCPGraph::LineStyle::lsStepLeft));
        itemLineStyleList->addItem(QStringLiteral("lsStepRight"),   int(QCPGraph::LineStyle::lsStepRight));
        itemLineStyleList->addItem(QStringLiteral("lsStepCenter"),  int(QCPGraph::LineStyle::lsStepCenter));
        itemLineStyleList->addItem(QStringLiteral("lsImpulse"),     int(QCPGraph::LineStyle::lsImpulse));
    }

    QComboBox *itemScatterStyleList = new QComboBox(dialog);
    itemScatterStyleList->addItem(QStringLiteral("ssNone"),             int(QCPScatterStyle::ScatterShape::ssNone));
    itemScatterStyleList->addItem(QStringLiteral("ssDot"),              int(QCPScatterStyle::ScatterShape::ssDot));
    itemScatterStyleList->addItem(QStringLiteral("ssCross"),            int(QCPScatterStyle::ScatterShape::ssCross));
    itemScatterStyleList->addItem(QStringLiteral("ssPlus"),             int(QCPScatterStyle::ScatterShape::ssPlus));
    itemScatterStyleList->addItem(QStringLiteral("ssCircle"),           int(QCPScatterStyle::ScatterShape::ssCircle));
    itemScatterStyleList->addItem(QStringLiteral("ssDisc"),             int(QCPScatterStyle::ScatterShape::ssDisc));
    itemScatterStyleList->addItem(QStringLiteral("ssSquare"),           int(QCPScatterStyle::ScatterShape::ssSquare));
    itemScatterStyleList->addItem(QStringLiteral("ssDiamond"),          int(QCPScatterStyle::ScatterShape::ssDiamond));
    itemScatterStyleList->addItem(QStringLiteral("ssStar"),             int(QCPScatterStyle::ScatterShape::ssStar));
    itemScatterStyleList->addItem(QStringLiteral("ssTriangle"),         int(QCPScatterStyle::ScatterShape::ssTriangle));
    itemScatterStyleList->addItem(QStringLiteral("ssTriangleInverted"), int(QCPScatterStyle::ScatterShape::ssTriangleInverted));
    itemScatterStyleList->addItem(QStringLiteral("ssCrossSquare"),      int(QCPScatterStyle::ScatterShape::ssCrossSquare));
    itemScatterStyleList->addItem(QStringLiteral("ssPlusSquare"),       int(QCPScatterStyle::ScatterShape::ssPlusSquare));
    itemScatterStyleList->addItem(QStringLiteral("ssCrossCircle"),      int(QCPScatterStyle::ScatterShape::ssCrossCircle));
    itemScatterStyleList->addItem(QStringLiteral("ssPlusCircle"),       int(QCPScatterStyle::ScatterShape::ssPlusCircle));
    itemScatterStyleList->addItem(QStringLiteral("ssPeace"),            int(QCPScatterStyle::ScatterShape::ssPeace));


    if(graphslist.size()>0)
    {
        itemLineStyleList->setCurrentIndex(graphslist[0]->lineStyle());
        itemScatterStyleList->setCurrentIndex(graphslist[0]->scatterStyle().shape());
    }

    if(curveslist.size()>0)
    {
        itemLineStyleList->setCurrentIndex(curveslist[0]->lineStyle());
        itemScatterStyleList->setCurrentIndex(curveslist[0]->scatterStyle().shape());
    }

    QPushButton * pb_color=new  QPushButton("Color",dialog);
    QObject::connect(pb_color, SIGNAL(clicked()), this, SLOT(slot_set_color()));

    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Initials parameters");
    QGridLayout * gbox = new QGridLayout();


    gbox->addWidget(itemLineStyleList,0,0);
    gbox->addWidget(itemScatterStyleList,1,0);
    gbox->addWidget(pb_color,2,0);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                        | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    gbox->addWidget(buttonBox,3,0);

    dialog->setLayout(gbox);

    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        if(graphslist.size()>0)
        {
            graphslist[0]->setLineStyle( QCPGraph::LineStyle (itemLineStyleList->currentData().toInt()) );
            graphslist[0]->setScatterStyle( QCPScatterStyle::ScatterShape (itemScatterStyleList->currentData().toInt()) );
        }

        if(curveslist.size()>0)
        {
            curveslist[0]->setLineStyle( QCPCurve::LineStyle (itemLineStyleList->currentData().toInt()) );
            curveslist[0]->setScatterStyle( QCPScatterStyle::ScatterShape (itemScatterStyleList->currentData().toInt()) );
        }
        replot();
    }
}

void Viewer1D::slot_histogram(QVector<double> data,QString name)
{
    int nbbins=QInputDialog::getInt(this,"Number of bins","Nb bins=",100,2,10000,1);
    QVector<double> labels;
    QVector<double> hist;
    labels.resize(nbbins);
    hist.resize(nbbins);

    double min=*std::min_element(data.constBegin(), data.constEnd());
    double max=*std::max_element(data.constBegin(), data.constEnd());

    double delta= (max-min)/nbbins;

    for(int k=0;k<data.size();k++)
    {
        unsigned int index=std::round((data[k]-min)/delta);

        if(index>0 && index<data.size())
        {
            hist[index]+=1;
        }
    }

    for(int i=0;i<labels.size();i++)
    {
        labels[i]=delta*i+min;
    }

    std::cout<<"slot_set_data_graph "<<labels.size()<<" "<<hist.size()<<std::endl;

    slot_add_data_graph(Curve2D(labels,hist,name));
}

void Viewer1D::slot_delete()
{
    QList<QCPAbstractPlottable*> plottableslist=this->selectedPlottables();

    for(int i=0;i<plottableslist.size();i++)
    {
        this->removePlottable(plottableslist[i]);
    }

    this->replot();
}

QList<QCPCurve*> Viewer1D::getSelectedQCPCurves()
{
    QList<QCPAbstractPlottable*> plottableslist=this->selectedPlottables();
    QList<QCPCurve*> listcurves;
    for(int i=0;i<plottableslist.size();i++)
    {
        QCPCurve * currentcurve=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if(currentcurve)
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
    for(int i=0;i<plottableslist.size();i++)
    {
        QCPCurve * currentcurve=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if(currentcurve)
        {
            listcurves.push_back(currentcurve);
        }
    }
    return listcurves;
}

QList<QCPAbstractPlottable *> Viewer1D::getSelectedCurvesOrGraphs()
{
    QList<QCPAbstractPlottable*> plottableslist=this->selectedPlottables();
    QList<QCPAbstractPlottable*> list;
    for(int i=0;i<plottableslist.size();i++)
    {
        QCPAbstractPlottable * currentp=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if(currentp)
        {
            list.push_back(currentp);
        }

        currentp=dynamic_cast<QCPGraph*>(plottableslist[i]);
        if(currentp)
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

    for(int i=0,id=0;i<plottableslist.size();i++)
    {
        QVector<double> x,y;

        QCPCurve * currentcurve=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if(currentcurve)
        {
            auto it =currentcurve->data()->begin();

            while(it!=currentcurve->data()->end())
            {
                x.push_back(it->key);
                y.push_back(it->value);
                it++;
            }
            curvelist.push_back(Curve2D(x,y,currentcurve->name()));
            id++;
        }

        QCPGraph * currentgraph=dynamic_cast<QCPGraph*>(plottableslist[i]);
        if(currentgraph)
        {
            auto it =currentgraph->data()->begin();

            while(it!=currentgraph->data()->end())
            {
                x.push_back(it->key);
                y.push_back(it->value);
                it++;
            }
            curvelist.push_back(Curve2D(x,y,currentgraph->name()));
            id++;
        }

    }

    return curvelist;
}


void Viewer1D::slot_save_image()
{
    QFileInfo info(current_filename);
    QString where=info.path();

    QDialog * dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Resolution");
    QGridLayout * gbox = new QGridLayout();
    QSpinBox * getWidth=new QSpinBox(dialog);
    QSpinBox * getHeight=new QSpinBox(dialog);
    getWidth->setRange(32,1024);getWidth->setPrefix("Width=");getWidth->setSuffix(" px");getWidth->setValue(width());
    getHeight->setRange(32,1024);getHeight->setPrefix("Height=");getHeight->setSuffix(" px");getHeight->setValue(height());
    gbox->addWidget(getWidth,0,0);
    gbox->addWidget(getHeight,0,1);
    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
    gbox->addWidget(buttonBox,4,0,2,1);
    dialog->setLayout(gbox);

    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        QString filename=QFileDialog::getSaveFileName(this,"Save Image",where,"(*.png)");
        if(!filename.isEmpty())
        {
            this->current_filename=filename;

            std::cout<<"Save "<<getWidth->value()<<"x"<<getHeight->value()<<std::endl;

            this->savePng(current_filename,getWidth->value(),getHeight->value(),1);
        }
    }
}

void Viewer1D::slot_rescale()
{
    if(colorScale)
    {
        QList<QCPCurve*> listcurves=getQCPCurves();
        if(listcurves.size()>0)
        {
            colorScale->setDataRange(listcurves[0]->getScalarFieldRange());
        }
    }

    this->rescaleAxes();
    this->replot();
}

void Viewer1D::slot_copy()
{
    QList<Curve2D> list=getSelectedCurves();

    if(list.size()>0 && sharedBuf!=nullptr)
    {
        *sharedBuf=list[0];
    }
}

void Viewer1D::slot_paste()
{
    if(sharedBuf!=nullptr)
    {
        if(sharedBuf->getX().size()>0)
        {
            if(sharedBuf->name().contains("Graph"))
            {
                slot_add_data_graph(*sharedBuf);
            }
            else if(sharedBuf->name().contains("Curve"))
            {
                slot_add_data_curve(*sharedBuf);
            }
        }
    }
    slot_rescale();
}
