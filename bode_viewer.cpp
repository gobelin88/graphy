#include "bode_viewer.h"

ViewerBode::ViewerBode(const QMap<QString,QKeySequence>& shortcuts_map,
                       QWidget* parent):QWidget(parent)
{
    QGridLayout * layout=new QGridLayout(this);

    tab=new QTabWidget();
    modules_viewer=new Viewer1D(shortcuts_map,parent);
    phases_viewer=new Viewer1D(shortcuts_map,parent);
    real_viewer=new Viewer1D(shortcuts_map,parent);
    imag_viewer=new Viewer1D(shortcuts_map,parent);

    connect(modules_viewer,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),this,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)));
    connect(modules_viewer,SIGNAL(sig_displayResults(QString)),this,SIGNAL(sig_displayResults(QString)));
    connect(phases_viewer,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),this,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)));
    connect(phases_viewer,SIGNAL(sig_displayResults(QString)),this,SIGNAL(sig_displayResults(QString)));
    connect(real_viewer,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),this,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)));
    connect(real_viewer,SIGNAL(sig_displayResults(QString)),this,SIGNAL(sig_displayResults(QString)));
    connect(imag_viewer,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)),this,SIGNAL(sig_newColumn(QString,Eigen::VectorXd)));
    connect(imag_viewer,SIGNAL(sig_displayResults(QString)),this,SIGNAL(sig_displayResults(QString)));

    tab->addTab(modules_viewer,"Module");
    tab->addTab(phases_viewer,"Phase");
    tab->addTab(real_viewer,"Real");
    tab->addTab(imag_viewer,"Imaginary");

    layout->addWidget(tab,0,0);

    this->setLayout(layout);

    //connect(modules_viewer,SIGNAL())
    createFitImpedanceMenu();
    modules_viewer->addSubMenu(menuFitImpedance);

    setMinimumSize(600,400);
}

void ViewerBode::createFitImpedanceMenu()
{
    menuFitImpedance=new QMenu("Fit impedance");
    actFitRL= new QAction("R+L",  this);
    actFitRLC= new QAction("R+L+C",  this);
    actFitRLpC= new QAction("(R+L)|C",  this);
    actFitRaLpCpRb= new QAction("(Ra+L)|C|Rb",  this);
    actFitRLCapCb= new QAction("(R+L)|Ca+Cb",  this);

    menuFitImpedance->addAction(actFitRL);
    menuFitImpedance->addAction(actFitRLC);
    menuFitImpedance->addAction(actFitRLpC);
    menuFitImpedance->addAction(actFitRaLpCpRb);
    menuFitImpedance->addAction(actFitRLCapCb);

    connect(actFitRL,SIGNAL(triggered()),this,SLOT(slot_fit_rl()));
    connect(actFitRLC,SIGNAL(triggered()),this,SLOT(slot_fit_rlc()));
    connect(actFitRLpC,SIGNAL(triggered()),this,SLOT(slot_fit_rlpc()));
    connect(actFitRaLpCpRb,SIGNAL(triggered()),this,SLOT(slot_fit_ralpcprb()));
    connect(actFitRLCapCb,SIGNAL(triggered()),this,SLOT(slot_fit_rlcapcb()));
}

void ViewerBode::slot_add_data(const Curve2DComplex& curve)
{
    modules_viewer->slot_add_data(curve.getModulesCurve());
    phases_viewer->slot_add_data(curve.getArgumentsCurve());
    real_viewer->slot_add_data(curve.getRealCurve());
    imag_viewer->slot_add_data(curve.getImagCurve());
    curves.push_back(curve);
}

QVector<Curve2DComplex> ViewerBode::getCurves()
{
    return curves;
}

void ViewerBode::slot_fit_rl()
{
    QVector<Curve2DComplex> curves=getCurves();
    QDoubleSpinBox* R,*L;
    QCheckBox* fixedR,*fixedL;

    QDialog* dialog=RL_cplx::createDialog(R,L,fixedR,fixedL);
    R->setValue(200);
    L->setValue(10);

    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        RL_cplx rl_cplx(R->value(),L->value(),fixedR->isChecked(),fixedL->isChecked());

        for (int i=0; i<curves.size(); i++)
        {
            curves[i].fit(&rl_cplx,Curve2DComplex::POLAR);
            Eigen::VectorXd F=curves[i].getModulesCurve().getLinX(1000);
            Eigen::VectorXd G,P;
            rl_cplx.at(F,G,P);

            QString legend=QString("Fit R=%1Ohm L=%2mH").arg(rl_cplx.getR()).arg(rl_cplx.getL());


            slot_add_data(Curve2DComplex(F,G,P,Curve2DComplex::POLAR,legend));
        }
    }
}

void ViewerBode::slot_fit_rlc()
{
    QVector<Curve2DComplex> curves=getCurves();
    QDoubleSpinBox* R,*L,*C;
    QCheckBox* fixedR,*fixedL,*fixedC;

    QDialog* dialog=RLC_cplx::createDialog(R,L,C,fixedR,fixedL,fixedC);
    R->setValue(200);
    L->setValue(10);

    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        RLC_cplx rlc_cplx(R->value(),L->value(),C->value(),fixedR->isChecked(),fixedL->isChecked(),fixedC->isChecked());

        for (int i=0; i<curves.size(); i++)
        {
            curves[i].fit(&rlc_cplx,Curve2DComplex::POLAR);
            Eigen::VectorXd F=curves[i].getModulesCurve().getLinX(1000);
            Eigen::VectorXd G,P;
            rlc_cplx.at(F,G,P);

            QString legend=QString("Fit R=%1Ohm L=%2mH C=%3nF Fc=%4Hz").arg(rlc_cplx.getR()).arg(rlc_cplx.getL()).arg(rlc_cplx.getC()).arg(rlc_cplx.getFc());

            slot_add_data(Curve2DComplex(F,G,P,Curve2DComplex::POLAR,legend));
        }
    }
}

void ViewerBode::slot_fit_rlpc()
{
    QVector<Curve2DComplex> curves=getCurves();
    QDoubleSpinBox* R,*L,*C;
    QCheckBox* fixedR,*fixedL,*fixedC;
    QDialog* dialog=RLpC_cplx::createDialog(R,L,C,fixedR,fixedL,fixedC);
    R->setValue(200);
    L->setValue(10);
    C->setValue(2);

    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        RLpC_cplx rlpc_cplx(R->value(),L->value(),C->value(),fixedR->isChecked(),fixedL->isChecked(),fixedC->isChecked());

        for (int i=0; i<curves.size(); i++)
        {
            curves[i].fit(&rlpc_cplx,Curve2DComplex::POLAR);
            Eigen::VectorXd F=curves[i].getModulesCurve().getLinX(1000);
            Eigen::VectorXd G,P;
            rlpc_cplx.at(F,G,P);

            QString legend=QString("Fit R=%1Ohm L=%2mH C=%3nF Fc=%4Hz").arg(rlpc_cplx.getR()).arg(rlpc_cplx.getL()).arg(rlpc_cplx.getC()).arg(rlpc_cplx.getFc());

            slot_add_data(Curve2DComplex(F,G,P,Curve2DComplex::POLAR,legend));
        }
    }
}

void ViewerBode::slot_fit_ralpcprb()
{
    QVector<Curve2DComplex> curves=getCurves();
    QDoubleSpinBox* Ra,*L,*C,*Rb;
    QCheckBox* fixedRa,*fixedL,*fixedC,*fixedRb;
    QDialog* dialog=RaLpCpRb_cplx::createDialog(Ra,L,C,Rb,fixedRa,fixedL,fixedC,fixedRb);
    Ra->setValue(200);
    L->setValue(10);
    C->setValue(2);
    Rb->setValue(100);

    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        RaLpCpRb_cplx rlpc_cplx(Ra->value(),L->value(),C->value(),Rb->value(),fixedRa->isChecked(),fixedL->isChecked(),fixedC->isChecked(),fixedRb->isChecked());

        for (int i=0; i<curves.size(); i++)
        {
            curves[i].fit(&rlpc_cplx,Curve2DComplex::POLAR);

            Eigen::VectorXd F=curves[i].getModulesCurve().getLinX(1000),G,P;
            rlpc_cplx.at(F,G,P);

            QString legend=QString("Fit R=%1Ohm L=%2mH C=%3nF Rb=%4kOhm").arg(rlpc_cplx.getRa()).arg(rlpc_cplx.getL()).arg(rlpc_cplx.getC()).arg(rlpc_cplx.getRb());

            slot_add_data(Curve2DComplex(F,G,P,Curve2DComplex::POLAR,legend));
        }
    }
}

void ViewerBode::slot_fit_rlcapcb()
{
    QVector<Curve2DComplex> curves=getCurves();
    QDoubleSpinBox* R,*L,*Ca,*Cb;
    QCheckBox* fixedR,*fixedL,*fixedCa,*fixedCb;
    QDialog* dialog=RLpCaCb_cplx::createDialog(R,L,Ca,Cb,fixedR,fixedL,fixedCa,fixedCb);
    R->setValue(200);
    L->setValue(10);
    Ca->setValue(2);
    Cb->setValue(0);

    int result=dialog->exec();

    if (result == QDialog::Accepted)
    {
        RLpCaCb_cplx rlpcacb_cplx(R->value(),L->value(),Ca->value(),Cb->value(),fixedR->isChecked(),fixedL->isChecked(),fixedCa->isChecked(),fixedCb->isChecked());

        for (int i=0; i<curves.size(); i++)
        {
            curves[i].fit(&rlpcacb_cplx,Curve2DComplex::POLAR);

            Eigen::VectorXd G,P;
            Eigen::VectorXd F=curves[i].getModulesCurve().getLinX(1000);
            rlpcacb_cplx.at(F,G,P);

            QString legend=QString("Fit R=%1Ohm L=%2mH Ca=%3nF Cb=%4nF").arg(rlpcacb_cplx.getR()).arg(rlpcacb_cplx.getL()).arg(rlpcacb_cplx.getCa()).arg(rlpcacb_cplx.getCb());

            slot_add_data(Curve2DComplex(F,G,P,Curve2DComplex::POLAR,legend));
        }
    }
}
