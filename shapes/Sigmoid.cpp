#include "Sigmoid.h"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

Sigmoid::Sigmoid(double A,double B,double C,double P)
{
    p.resize(4);
    setA(A);
    setB(B);
    setC(C);
    setP(P);
}

QDialog* Sigmoid::createDialog(QDoubleSpinBox*& getA,
                             QDoubleSpinBox*& getB,
                             QDoubleSpinBox*& getC,
                             QDoubleSpinBox*& getP)
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Sigmoid : Initials parameters");
    QGridLayout* gbox = new QGridLayout();

    QLabel* label_eqn=new QLabel(dialog);
    label_eqn->setPixmap(QPixmap(":/eqn/eqn/sigmoid.gif"));
    label_eqn->setAlignment(Qt::AlignHCenter);

    getA=new QDoubleSpinBox(dialog);
    getA->setRange(-1e8,1e8);
    getA->setDecimals(4);
    getA->setPrefix("A=");
    getB=new QDoubleSpinBox(dialog);
    getB->setRange(-1e8,1e8);
    getB->setDecimals(4);
    getB->setPrefix("B=");
    getC=new QDoubleSpinBox(dialog);
    getC->setRange(-1e8,1e8);
    getC->setDecimals(4);
    getC->setPrefix("C=");
    getP=new QDoubleSpinBox(dialog);
    getP->setRange(-1e8,1e8);
    getP->setDecimals(4);
    getP->setPrefix("Phi=");


    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));


    gbox->addWidget(label_eqn,0,0);
    gbox->addWidget(getA,1,0);
    gbox->addWidget(getB,2,0);
    gbox->addWidget(getC,3,0);
    gbox->addWidget(getP,4,0);
    gbox->addWidget(buttonBox,6,0);

    dialog->setLayout(gbox);

    return dialog;
}

Eigen::Vector2d Sigmoid::delta(const Eigen::Vector2d& pt)
{
    return Eigen::Vector2d (pt[1]-at(pt[0]),0);
}

double Sigmoid::getA()const
{
    return p[0];
}
double Sigmoid::getB()const
{
    return p[1];
}
double Sigmoid::getC()const
{
    return p[2];
}
double Sigmoid::getP()const
{
    return p[3];
}

void  Sigmoid::setA(double A)
{
    p[0]=A;
}
void  Sigmoid::setB(double B)
{
    p[1]=B;
}
void  Sigmoid::setC(double C)
{
    p[2]=C;
}
void  Sigmoid::setP(double P)
{
    p[3]=P;
}

double Sigmoid::at(double t)const
{
    double A=getA();
    double B=getB();
    double C=getC();
    double P=getP();

    return ((B-A)/(1+std::exp((C-t)*P*(B-A)))+A);//
}

Eigen::VectorXd Sigmoid::at(Eigen::VectorXd t)
{
    Eigen::VectorXd y(t.size());

    for (int i=0; i<t.size(); i++)
    {
        y[i]=at(t[i]);
    }
    return y;
}

int Sigmoid::nb_params()
{
    return this->p.rows();
}
void Sigmoid::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}
const Eigen::VectorXd& Sigmoid::getParams()
{
    return p;
}
