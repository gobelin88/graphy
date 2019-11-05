#include "1d_cplx_viewer.h"

Viewer1DCPLX::Viewer1DCPLX()
{
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

    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );

    connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress(QMouseEvent*)));
    connect(this, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoublePress(QMouseEvent*)));

    pen_select.setColor(colors[2]);
    pen_select.setStyle(Qt::SolidLine);
}

Viewer1DCPLX::~Viewer1DCPLX()
{

}

unsigned int Viewer1DCPLX::getId()
{
    unsigned int id=(this->plottableCount()-1)%colors.size();
    return id;
}

void Viewer1DCPLX::newGraph(QString name)
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

    this->yAxis->setLabel("Gain");
}

void Viewer1DCPLX::newSubGraph(QString name)
{
    this->addGraph(this->xAxis, this->yAxis2);
    QPen pen_model(colors[getId()]);

    pen_model.setStyle(Qt::SolidLine);
    this->graph()->setPen(pen_model);
    this->graph()->setLineStyle(QCPGraph::lsLine);
    this->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 10));
    this->rescaleAxes();
    legend->setVisible(true);

    this->graph()->setSelectable(QCP::stWhole);

    this->yAxis2->setVisible(true);

    this->yAxis2->setLabel("Phase");

    QCPSelectionDecorator * decorator_select=new QCPSelectionDecorator;
    decorator_select->setPen(pen_select);
    this->graph()->setSelectionDecorator(decorator_select);
    graph()->setName(name);
}

void Viewer1DCPLX::slot_add_data_graph(const Curve2D_GainPhase & curve)
{
    std::cout<<"slot_add_data_graph1"<<std::endl;

    newGraph(curve.gname());
    graph()->setData(curve.getF(), curve.getG());

    newSubGraph(curve.pname());
    graph()->setData(curve.getF(), curve.getP());

    rescaleAxes();
    replot();
}

void Viewer1DCPLX::createPopup()
{
    popup_menu=new QMenu(this);

    actClearMarks   = new QAction("Clear Marks",  this);
    actClearMarks->setShortcut(QKeySequence("Space"));

    actSave   = new QAction("Save",  this);
    actSave->setShortcut(QKeySequence("S"));
    actRescale= new QAction("Rescale",  this);
    actRescale->setShortcut(QKeySequence("R"));
    actColor= new QAction("Color",  this);
    actColor->setShortcut(QKeySequence("C"));
    actStyle= new QAction("Style",  this);
    actStyle->setShortcut(QKeySequence("!"));

    actLegendShow= new QAction("Hide",  this);
    actLegendShow->setCheckable(true);
    actLegendShow->setChecked(true);
    actLegendShow->setShortcut(QKeySequence("L"));

    actLegendTop= new QAction("Move bottom",  this);
    actLegendTop->setCheckable(true);
    actLegendTop->setChecked(true);

    this->addAction(actSave);
    this->addAction(actRescale);
    this->addAction(actLegendShow);
    this->addAction(actClearMarks);
    this->addAction(actColor);
    this->addAction(actStyle);

    popup_menu->addAction(actSave);
    popup_menu->addSeparator();
    popup_menu->addAction(actRescale);
    popup_menu->addAction(actClearMarks);
    popup_menu->addSeparator();

    menu_fit=new QMenu("Fit",popup_menu);
    menu_legend=new QMenu("Legend",menu_fit);

    actFitRaLpCpRb= new QAction("RaL|C|Rb",  this);
    actFitRLpC= new QAction("RL|C",  this);
    actFitRL= new QAction("RL",  this);
    actFitRLC= new QAction("RLC",  this);
    actFitRLCapCb= new QAction("RL|CaCb",  this);

    menu_fit->addAction(actFitRL);
    menu_fit->addAction(actFitRLpC);
    menu_fit->addAction(actFitRaLpCpRb);
    menu_fit->addAction(actFitRLC);
    menu_fit->addAction(actFitRLCapCb);

    menu_legend->addAction(actLegendShow);
    menu_legend->addAction(actLegendTop);
    menu_legend->addAction(actStyle);

    popup_menu->addMenu(menu_fit);
    popup_menu->addMenu(menu_legend);

    connect(actSave,SIGNAL(triggered()),this,SLOT(slot_save_image()));
    connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_rescale()));
    connect(actFitRLpC,SIGNAL(triggered()),this,SLOT(slot_fit_rlpc()));
    connect(actFitRaLpCpRb,SIGNAL(triggered()),this,SLOT(slot_fit_ralpcprb()));
    connect(actFitRL,SIGNAL(triggered()),this,SLOT(slot_fit_rl()));

    connect(actFitRLC,SIGNAL(triggered()),this,SLOT(slot_fit_rlc()));
    connect(actFitRLCapCb,SIGNAL(triggered()),this,SLOT(slot_fit_rlcapcb()));

    connect(actColor,SIGNAL(triggered()),this,SLOT(slot_set_color()));

    connect(actLegendShow,SIGNAL(toggled(bool)),this,SLOT(slot_show_legend(bool)));
    connect(actLegendTop,SIGNAL(toggled(bool)),this,SLOT(slot_top_legend(bool)));
    connect(actClearMarks,SIGNAL(triggered()),this,SLOT(slot_clear_marks()));
    connect(actStyle,SIGNAL(triggered()),this,SLOT(slot_set_style()));
}

void Viewer1DCPLX::slot_clear_marks()
{
    std::cout<<"clear items "<<this->itemCount()<<std::endl;
    this->clearItems();

    this->replot();
}


void Viewer1DCPLX::mousePress(QMouseEvent * event)
{
    if(event->button() == Qt::RightButton)
    {
        popup_menu->exec(mapToGlobal(event->pos()));
    }
}

void Viewer1DCPLX::mouseDoublePress(QMouseEvent * event)
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

void Viewer1DCPLX::slot_fit_ralpcprb()
{
    QList<Curve2D_GainPhase> curves=getCurves();
    QDoubleSpinBox *Ra,*L,*C,*Rb;
    QCheckBox *fixedRa,*fixedL,*fixedC,*fixedRb;
    QDialog * dialog=RaLpCpRb_cplx::createDialog(Ra,L,C,Rb,fixedRa,fixedL,fixedC,fixedRb);
    Ra->setValue(200);
    L->setValue(10);
    C->setValue(2);
    Rb->setValue(100);

    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        RaLpCpRb_cplx rlpc_cplx(Ra->value(),L->value(),C->value(),Rb->value(),fixedRa->isChecked(),fixedL->isChecked(),fixedC->isChecked(),fixedRb->isChecked());

        for(int i=0;i<curves.size();i++)
        {
            std::cout<<"fit..."<<std::endl;
            curves[i].fit(&rlpc_cplx);
            std::cout<<"fit ok"<<std::endl;

            QVector<double> F=curves[i].getLinF(1000),G,P;
            rlpc_cplx.at(F,G,P);

            slot_add_data_graph(Curve2D_GainPhase(F,G,P,
                                                  QString("Fit R=%1Ohm L=%2mH C=%3nF Rb=%4kOhm").arg(rlpc_cplx.getRa()).arg(rlpc_cplx.getL()).arg(rlpc_cplx.getC()).arg(rlpc_cplx.getRb()),
                                                  QString("Fit R=%1Ohm L=%2mH C=%3nF Rb=%4kOhm").arg(rlpc_cplx.getRa()).arg(rlpc_cplx.getL()).arg(rlpc_cplx.getC()).arg(rlpc_cplx.getRb())));
        }
    }
}

void Viewer1DCPLX::slot_fit_rlpc()
{
    QList<Curve2D_GainPhase> curves=getCurves();
    QDoubleSpinBox *R,*L,*C;
    QCheckBox *fixedR,*fixedL,*fixedC;
    QDialog * dialog=RLpC_cplx::createDialog(R,L,C,fixedR,fixedL,fixedC);
    R->setValue(200);
    L->setValue(10);
    C->setValue(2);

    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        RLpC_cplx rlpc_cplx(R->value(),L->value(),C->value(),fixedR->isChecked(),fixedL->isChecked(),fixedC->isChecked());

        for(int i=0;i<curves.size();i++)
        {
            curves[i].fit(&rlpc_cplx);
            QVector<double> F=curves[i].getLinF(1000);
            QVector<double> G,P;
            rlpc_cplx.at(F,G,P);

            slot_add_data_graph(Curve2D_GainPhase(F,G,P,
                                                  QString("Fit R=%1Ohm L=%2mH C=%3nF Fc=%4Hz").arg(rlpc_cplx.getR()).arg(rlpc_cplx.getL()).arg(rlpc_cplx.getC()).arg(rlpc_cplx.getFc()),
                                                  QString("Fit R=%1Ohm L=%2mH C=%3nF Fc=%4Hz").arg(rlpc_cplx.getR()).arg(rlpc_cplx.getL()).arg(rlpc_cplx.getC()).arg(rlpc_cplx.getFc())));
        }
    }
}

void Viewer1DCPLX::slot_fit_rlc()
{
    QList<Curve2D_GainPhase> curves=getCurves();
    QDoubleSpinBox *R,*L,*C;
    QCheckBox *fixedR,*fixedL,*fixedC;

    QDialog * dialog=RLC_cplx::createDialog(R,L,C,fixedR,fixedL,fixedC);
    R->setValue(200);
    L->setValue(10);

    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        RLC_cplx rlc_cplx(R->value(),L->value(),C->value(),fixedR->isChecked(),fixedL->isChecked(),fixedC->isChecked());

        for(int i=0;i<curves.size();i++)
        {
            curves[i].fit(&rlc_cplx);
            QVector<double> F=curves[i].getLinF(1000);
            QVector<double> G,P;
            rlc_cplx.at(F,G,P);

            slot_add_data_graph(Curve2D_GainPhase(F,G,P,
                                                  QString("Fit R=%1Ohm L=%2mH C=%3nF Fc=%4Hz").arg(rlc_cplx.getR()).arg(rlc_cplx.getL()).arg(rlc_cplx.getC()).arg(rlc_cplx.getFc()),
                                                  QString("Fit R=%1Ohm L=%2mH C=%3nF Fc=%4Hz").arg(rlc_cplx.getR()).arg(rlc_cplx.getL()).arg(rlc_cplx.getC()).arg(rlc_cplx.getFc())));
        }
    }
}

void Viewer1DCPLX::slot_fit_rlcapcb()
{
    QList<Curve2D_GainPhase> curves=getCurves();
    QDoubleSpinBox *R,*L,*Ca,*Cb;
    QCheckBox *fixedR,*fixedL,*fixedCa,*fixedCb;
    QDialog * dialog=RLpCaCb_cplx::createDialog(R,L,Ca,Cb,fixedR,fixedL,fixedCa,fixedCb);
    R->setValue(200);
    L->setValue(10);
    Ca->setValue(2);
    Cb->setValue(0);

    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        RLpCaCb_cplx rlpcacb_cplx(R->value(),L->value(),Ca->value(),Cb->value(),fixedR->isChecked(),fixedL->isChecked(),fixedCa->isChecked(),fixedCb->isChecked());

        for(int i=0;i<curves.size();i++)
        {
            std::cout<<"fit..."<<std::endl;
            curves[i].fit(&rlpcacb_cplx);
            std::cout<<"fit ok"<<std::endl;

            QVector<double> F=curves[i].getLinF(1000),G,P;
            rlpcacb_cplx.at(F,G,P);

            slot_add_data_graph(Curve2D_GainPhase(F,G,P,
                                                  QString("Fit R=%1Ohm L=%2mH Ca=%3nF Cb=%4nF").arg(rlpcacb_cplx.getR()).arg(rlpcacb_cplx.getL()).arg(rlpcacb_cplx.getCa()).arg(rlpcacb_cplx.getCb()),
                                                  QString("Fit R=%1Ohm L=%2mH Ca=%3nF Cb=%4nF").arg(rlpcacb_cplx.getR()).arg(rlpcacb_cplx.getL()).arg(rlpcacb_cplx.getCa()).arg(rlpcacb_cplx.getCb())));
        }
    }
}

void Viewer1DCPLX::slot_fit_rl()
{
    QList<Curve2D_GainPhase> curves=getCurves();
    QDoubleSpinBox *R,*L;
    QCheckBox *fixedR,*fixedL;

    QDialog * dialog=RL_cplx::createDialog(R,L,fixedR,fixedL);
    R->setValue(200);
    L->setValue(10);

    std::cout<<"slot_fit_rlc3"<<std::endl;
    int result=dialog->exec();

    if(result == QDialog::Accepted)
    {
        RL_cplx rl_cplx(R->value(),L->value(),fixedR->isChecked(),fixedL->isChecked());

        for(int i=0;i<curves.size();i++)
        {
            curves[i].fit(&rl_cplx);
            QVector<double> F=curves[i].getLinF(1000);
            QVector<double> G,P;
            rl_cplx.at(F,G,P);

            slot_add_data_graph(Curve2D_GainPhase(F,G,P,
                                                  QString("Fit R=%1Ohm L=%2mH").arg(rl_cplx.getR()).arg(rl_cplx.getL()),
                                                  QString("Fit R=%1Ohm L=%2mH").arg(rl_cplx.getR()).arg(rl_cplx.getL())));
        }
    }
}

void Viewer1DCPLX::slot_show_legend(bool value)
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

void Viewer1DCPLX::slot_top_legend(bool value)
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

void Viewer1DCPLX::slot_set_color()
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();

    if(graphslist.size()>0)
    {
        QColor color=QColorDialog::getColor(graphslist[0]->pen().color(),this,"Get Color");

        graphslist[0]->setPen(QPen(color));
    }
}

void Viewer1DCPLX::slot_set_style()
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();

    if(graphslist.size()>0)
    {
        QDialog * dialog=new QDialog;

        QComboBox *itemLineStyleList = new QComboBox(dialog);
        itemLineStyleList->addItem(QStringLiteral("lsNone"),        int(QCPGraph::LineStyle::lsNone));
        itemLineStyleList->addItem(QStringLiteral("lsLine"),        int(QCPGraph::LineStyle::lsLine));
        itemLineStyleList->addItem(QStringLiteral("lsStepLeft"),    int(QCPGraph::LineStyle::lsStepLeft));
        itemLineStyleList->addItem(QStringLiteral("lsStepRight"),   int(QCPGraph::LineStyle::lsStepRight));
        itemLineStyleList->addItem(QStringLiteral("lsStepCenter"),  int(QCPGraph::LineStyle::lsStepCenter));
        itemLineStyleList->addItem(QStringLiteral("lsImpulse"),     int(QCPGraph::LineStyle::lsImpulse));
        itemLineStyleList->setCurrentIndex(graphslist[0]->lineStyle());

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
        itemScatterStyleList->setCurrentIndex(graphslist[0]->scatterStyle().shape());

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
            graphslist[0]->setLineStyle( QCPGraph::LineStyle (itemLineStyleList->currentData().toInt()) );
            graphslist[0]->setScatterStyle( QCPScatterStyle::ScatterShape (itemScatterStyleList->currentData().toInt()) );
            replot();
        }
    }
}

QList<Curve2D_GainPhase> Viewer1DCPLX::getCurves()
{
    QList<Curve2D_GainPhase> curvelist;

    for(int i=0;i<this->graphCount();i+=2)
    {
        QVector<double> f,g,p;

        QCPGraph * currentgraph_g,*currentgraph_p;

        currentgraph_g=this->graph(i);
        currentgraph_p=this->graph(i+1);

        if(currentgraph_g && currentgraph_p)
        {
            auto itg =currentgraph_g->data()->begin();
            auto itp =currentgraph_p->data()->begin();

            while(itg!=currentgraph_g->data()->end())
            {
                f.push_back(itg->key);
                g.push_back(itg->value);
                p.push_back(itp->value);
                itg++;
                itp++;
            }
            curvelist.push_back(Curve2D_GainPhase(f,g,p,currentgraph_g->name(),currentgraph_p->name()));
        }

    }

    return curvelist;
}


void Viewer1DCPLX::slot_save_image()
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

void Viewer1DCPLX::slot_rescale()
{
    this->rescaleAxes();
    this->replot();
}
