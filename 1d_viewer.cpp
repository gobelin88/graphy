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

    connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress(QMouseEvent*)));
    connect(this, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDoublePress(QMouseEvent*)));

    pen_select.setColor(colors[2]);
    pen_select.setStyle(Qt::SolidLine);

    applyShortcuts(shortcuts_map);

    top_bottom=Qt::AlignTop;
    left_right=Qt::AlignRight;
}

Viewer1D::~Viewer1D()
{

}

unsigned int Viewer1D::getId()
{
    unsigned int id=(this->plottableCount()-1)%colors.size();
    return id;
}

QCPGraph* Viewer1D::newGraph(const Curve2D& datacurve)
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

    QCPSelectionDecorator* decorator_select=new QCPSelectionDecorator;
    decorator_select->setPen(pen_select);
    this->graph()->setSelectionDecorator(decorator_select);
    graph()->setName(datacurve.getLegend());
    graph()->setData(toQVector(datacurve.getX()),toQVector(datacurve.getY()));

    return this->graph();
}

QCPCurve* Viewer1D::newCurve(const Curve2D& datacurve)
{
    QCPCurve* pcurve = new QCPCurve(this->xAxis, this->yAxis);

    pcurve->setScalarField(datacurve.getQScalarField());

    pcurve->setName(datacurve.getLegend());
    pcurve->setData(datacurve.getQX(), datacurve.getQY());
    QPen pen_model(colors[getId()]);
    pen_model.setStyle(Qt::SolidLine);
    pcurve->setPen(pen_model);

    QCPSelectionDecorator* decorator_select=new QCPSelectionDecorator;
    decorator_select->setPen(pen_select);
    pcurve->setSelectionDecorator(decorator_select);
    legend->setVisible(true);

    if (datacurve.getScalarField().size()>0)
    {
        pcurve->setColorScale(new QCPColorScale(this));
        pcurve->getColorScale()->setType(QCPAxis::atRight);
        pcurve->getColorScale()->setDataRange(pcurve->getScalarFieldRange());
        pcurve->getColorScale()->setGradient(pcurve->getGradient());

        pcurve->setLineStyle(QCPCurve::lsNone);
        pcurve->setScatterStyle(QCPScatterStyle::ScatterShape::ssDisc);

        this->plotLayout()->addElement(0, this->plotLayout()->columnCount(), pcurve->getColorScale());
    }

    return pcurve;
}

void Viewer1D::slot_add_data(const Curve2D& datacurve)
{
    if (datacurve.getType()==Curve2D::GRAPH)
    {
        newGraph(datacurve);
    }
    else
    {
        newCurve(datacurve);
        axisRect()->setupFullAxesBox();
    }

    rescaleAxes();
    replot();
}

void Viewer1D::createPopup()
{
    popup_menu=new QMenu(this);

    actCopy   = new QAction("Copy",  this);
    actPaste   = new QAction("Paste",  this);

    actClearMarks   = new QAction("Clear Marks",  this);
    actSave   = new QAction("Save",  this);
    actRescale= new QAction("Rescale",  this);
    actDelete= new QAction("Delete",  this);
    actColor= new QAction("Color",  this);
    actStyle= new QAction("Style",  this);
    actLegendShowHide= new QAction("Hide",  this);
    actLegendTopBottom= new QAction("Move bottom",  this);
    actLegendLeftRight= new QAction("Move left",  this);


    actCopy->setShortcutVisibleInContextMenu(true);
    actPaste->setShortcutVisibleInContextMenu(true);
    actClearMarks->setShortcutVisibleInContextMenu(true);
    actSave->setShortcutVisibleInContextMenu(true);
    actRescale->setShortcutVisibleInContextMenu(true);
    actDelete->setShortcutVisibleInContextMenu(true);
    actColor->setShortcutVisibleInContextMenu(true);
    actStyle->setShortcutVisibleInContextMenu(true);
    actLegendShowHide->setShortcutVisibleInContextMenu(true);
    actLegendTopBottom->setShortcutVisibleInContextMenu(true);
    actLegendLeftRight->setShortcutVisibleInContextMenu(true);

    actLegendShowHide->setCheckable(true);
    actLegendShowHide->setChecked(true);
    actLegendTopBottom->setCheckable(true);
    actLegendTopBottom->setChecked(true);
    actLegendLeftRight->setCheckable(true);
    actLegendLeftRight->setChecked(true);

    this->addAction(actCopy);
    this->addAction(actPaste);
    this->addAction(actSave);
    this->addAction(actRescale);
    this->addAction(actDelete);
    this->addAction(actLegendShowHide);
    this->addAction(actClearMarks);
    this->addAction(actColor);
    this->addAction(actStyle);
    this->addAction(actLegendTopBottom);
    this->addAction(actLegendLeftRight);

    popup_menu->addAction(actCopy);
    popup_menu->addAction(actPaste);
    popup_menu->addAction(actSave);
    popup_menu->addAction(actDelete);
    popup_menu->addSeparator();
    popup_menu->addAction(actRescale);
    popup_menu->addAction(actClearMarks);
    popup_menu->addSeparator();

    menu_fit=new QMenu("Fit",this);
    menu_legend=new QMenu("Legend",this);
    menu_scalarField=new QMenu("Scalar Field",this);
    menu_scalarField_fit=new QMenu("Fit",this);

    actFitPolynomial= new QAction("Polynomial",  this);
    actFitGaussian= new QAction("Gaussian",  this);
    actFitSigmoid= new QAction("Sigmoid",  this);
    actFitSinusoide= new QAction("Sinusoide",  this);

    actFitPolynomial2V= new QAction("Polynomial XY",  this);

    menu_fit->addAction(actFitPolynomial);
    menu_fit->addAction(actFitGaussian);
    menu_fit->addAction(actFitSigmoid);
    menu_fit->addAction(actFitSinusoide);

    menu_legend->addAction(actLegendShowHide);
    menu_legend->addAction(actLegendTopBottom);
    menu_legend->addAction(actLegendLeftRight);
    menu_legend->addAction(actStyle);

    menu_scalarField->addMenu(menu_scalarField_fit);

    menu_scalarField_fit->addAction(actFitPolynomial2V);

    popup_menu->addMenu(menu_fit);
    popup_menu->addMenu(menu_scalarField);
    popup_menu->addMenu(menu_legend);

    connect(actSave,SIGNAL(triggered()),this,SLOT(slot_save_image()));
    connect(actRescale,SIGNAL(triggered()),this,SLOT(slot_rescale()));
    connect(actFitPolynomial,SIGNAL(triggered()),this,SLOT(slot_fit_polynomial()));
    connect(actFitPolynomial2V,SIGNAL(triggered()),this,SLOT(slot_fit_2var_polynomial()));
    connect(actFitGaussian,SIGNAL(triggered()),this,SLOT(slot_fit_gaussian()));
    connect(actFitSinusoide,SIGNAL(triggered()),this,SLOT(slot_fit_sinusoide()));
    connect(actFitSigmoid,SIGNAL(triggered()),this,SLOT(slot_fit_sigmoid()));
    connect(actColor,SIGNAL(triggered()),this,SLOT(slot_set_color()));

    connect(actCopy,SIGNAL(triggered()),this,SLOT(slot_copy()));
    connect(actPaste,SIGNAL(triggered()),this,SLOT(slot_paste()));

    connect(actDelete,SIGNAL(triggered()),this,SLOT(slot_delete()));
    connect(actLegendShowHide,SIGNAL(toggled(bool)),this,SLOT(slot_show_legend(bool)));
    connect(actLegendTopBottom,SIGNAL(toggled(bool)),this,SLOT(slot_top_legend(bool)));
    connect(actLegendLeftRight,SIGNAL(toggled(bool)),this,SLOT(slot_left_legend(bool)));
    connect(actClearMarks,SIGNAL(triggered()),this,SLOT(slot_clear_marks()));
    connect(actStyle,SIGNAL(triggered()),this,SLOT(slot_set_style()));
}

void Viewer1D::slot_clear_marks()
{
    std::cout<<"clear items "<<this->itemCount()<<std::endl;
    this->clearItems();

    this->replot();
}

void Viewer1D::addMark(double cx,double cy,QString markstr)
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

void Viewer1D::mousePress(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        popup_menu->exec(mapToGlobal(event->pos()));
    }
}

void Viewer1D::mouseDoublePress(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        double cx=this->xAxis->pixelToCoord(event->x());
        double cy=this->yAxis->pixelToCoord(event->y());

        QString str=QInputDialog::getText(this,"Add Mark","Text=");
        addMark(cx,cy,str);
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

            Curve2D fit_curve(X,Y,QString("Fit Sinusoid : %1").arg(result_str),Curve2D::GRAPH);

            slot_add_data(fit_curve);
        }
    }
}

void Viewer1D::slot_fit_sigmoid()
{
    QList<Curve2D> curves=getSelectedCurves();

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

        for (int i=0; i<curves.size(); i++)
        {
            curves[i].fit(&sigmoid);

            Eigen::VectorXd X=curves[i].getLinX(1000);
            Eigen::VectorXd Y=sigmoid.at(X);

            QString result_str=QString("A=%1 B=%2 C=%3 P=%4 ").arg(sigmoid.getA()).arg(sigmoid.getB()).arg(sigmoid.getC()).arg(sigmoid.getP());
            Curve2D fit_curve(X,Y,QString("Fit Sigmoid : %1").arg(result_str),Curve2D::GRAPH);

            slot_add_data(fit_curve);
        }
    }
}

void Viewer1D::slot_fit_gaussian()
{
    QList<Curve2D> curves=getSelectedCurves();

    QDoubleSpinBox* S,*M,*K;
    QDialog* dialog=Gaussian::createDialog(S,M,K);
    S->setValue(10);
    M->setValue(0);
    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        Gaussian gaussian(S->value(),M->value(),K->value());

        for (int i=0; i<curves.size(); i++)
        {
            curves[i].fit(&gaussian);
            Eigen::VectorXd X=curves[i].getLinX(1000);
            Eigen::VectorXd Y=gaussian.at(X);

            QString result_str=QString("Sigma=%1 Mean=%2 K=%3").arg(gaussian.getS()).arg(gaussian.getM()).arg(gaussian.getK());

            Curve2D fit_curve(X,Y,QString("Fit Gaussian : %1").arg(result_str),Curve2D::GRAPH);

            slot_add_data(fit_curve);
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
            int order=getOrder->value();
            for (int i=0; i<curves.size(); i++)
            {
                Eigen::VectorXd C=curves[i].fit(order);
                Eigen::VectorXd X=curves[i].getLinX(1000);
                Eigen::VectorXd Y=curves[i].at(C,X);


                QString result_str=Curve2D::getPolynomeString(C,order);

                Curve2D fit_curve(X,Y,QString("Fit Polynome : %1").arg(result_str),Curve2D::GRAPH);

                slot_add_data(fit_curve);
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

void Viewer1D::slot_set_color()
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();

    if (graphslist.size()>0)
    {
        QColor color=QColorDialog::getColor(graphslist[0]->pen().color(),this,"Get Color");

        graphslist[0]->setPen(QPen(color));
    }
}

void Viewer1D::slot_set_style()
{
    QList<QCPGraph*> graphslist=this->selectedGraphs();
    QList<QCPCurve*> curveslist=this->getSelectedQCPCurves();

    if (graphslist.size()==0 && curveslist.size()==0)
    {
        return;
    }

    QDialog* dialog=new QDialog;
    QComboBox* itemLineStyleList = new QComboBox(dialog);

    if (curveslist.size()>0)
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

    QComboBox* itemScatterStyleList = new QComboBox(dialog);
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


    if (graphslist.size()>0)
    {
        itemLineStyleList->setCurrentIndex(graphslist[0]->lineStyle());
        itemScatterStyleList->setCurrentIndex(graphslist[0]->scatterStyle().shape());
    }

    if (curveslist.size()>0)
    {
        itemLineStyleList->setCurrentIndex(curveslist[0]->lineStyle());
        itemScatterStyleList->setCurrentIndex(curveslist[0]->scatterStyle().shape());
    }

    QPushButton* pb_color=new  QPushButton("Color",dialog);
    QObject::connect(pb_color, SIGNAL(clicked()), this, SLOT(slot_set_color()));

    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Style Options");
    QGridLayout* gbox = new QGridLayout();


    gbox->addWidget(itemLineStyleList,0,0);
    gbox->addWidget(itemScatterStyleList,1,0);
    gbox->addWidget(pb_color,2,0);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    gbox->addWidget(buttonBox,3,0);

    dialog->setLayout(gbox);

    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        if (graphslist.size()>0)
        {
            graphslist[0]->setLineStyle( QCPGraph::LineStyle (itemLineStyleList->currentData().toInt()) );
            graphslist[0]->setScatterStyle( QCPScatterStyle::ScatterShape (itemScatterStyleList->currentData().toInt()) );
        }

        if (curveslist.size()>0)
        {
            curveslist[0]->setLineStyle( QCPCurve::LineStyle (itemLineStyleList->currentData().toInt()) );
            curveslist[0]->setScatterStyle( QCPScatterStyle::ScatterShape (itemScatterStyleList->currentData().toInt()) );
        }
        replot();
    }
}

void Viewer1D::slot_histogram(Eigen::VectorXd data,QString name,int nbbins)
{
    Eigen::VectorXd labels;
    Eigen::VectorXd hist;
    labels.resize(nbbins);
    hist.resize(nbbins);

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
        QVector<double> x,y;

        QCPCurve* currentcurve=dynamic_cast<QCPCurve*>(plottableslist[i]);
        if (currentcurve)
        {
            auto it =currentcurve->data()->begin();

            while (it!=currentcurve->data()->end())
            {
                x.push_back(it->key);
                y.push_back(it->value);
                it++;
            }
            Curve2D curve(fromQVector(x),fromQVector(y),currentcurve->name(),Curve2D::CURVE);
            curve.setScalarField(fromQVector(currentcurve->getScalarField()));
            curvelist.push_back(curve);
            id++;
        }

        QCPGraph* currentgraph=dynamic_cast<QCPGraph*>(plottableslist[i]);
        if (currentgraph)
        {
            auto it =currentgraph->data()->begin();

            while (it!=currentgraph->data()->end())
            {
                x.push_back(it->key);
                y.push_back(it->value);
                it++;
            }
            curvelist.push_back(Curve2D(fromQVector(x),fromQVector(y),currentgraph->name(),Curve2D::GRAPH));
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
        *sharedBuf=list[0];
    }
}

void Viewer1D::slot_paste()
{
    if (sharedBuf!=nullptr)
    {
        if (sharedBuf->getX().size()>0)
        {
            slot_add_data(*sharedBuf);
        }
    }
    slot_rescale();
}

void Viewer1D::slot_statistiques()
{
    QList<Curve2D> curves=getSelectedCurves();

    for (int i=0; i<curves.size(); i++)
    {
        //todo
    }
}

void Viewer1D::applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map)
{
    QMap<QString,QAction*> shortcuts_links;
    shortcuts_links.insert(QString("Graph-Copy"),actCopy);
    shortcuts_links.insert(QString("Graph-Paste"),actPaste);
    shortcuts_links.insert(QString("Graph-ClearMarks"),actClearMarks);
    shortcuts_links.insert(QString("Graph-Save"),actSave);
    shortcuts_links.insert(QString("Graph-Rescale"),actRescale);
    shortcuts_links.insert(QString("Graph-Delete"),actDelete);
    shortcuts_links.insert(QString("Graph-Color"),actColor);
    shortcuts_links.insert(QString("Graph-Style"),actStyle);
    shortcuts_links.insert(QString("Graph-Legend-Show/Hide"),actLegendShowHide);
    shortcuts_links.insert(QString("Graph-Legend-Top/Bottom"),actLegendTopBottom);
    shortcuts_links.insert(QString("Graph-Legend-Left/Right"),actLegendLeftRight);

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
