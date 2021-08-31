#include "shapes/Gaussian.h"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

Gaussian::Gaussian(double S,double M,double K)
{
    p.resize(3);
    setS(S);
    setM(M);
    setK(K);
}

QDialog* Gaussian::createDialog(QDoubleSpinBox*& getS,
                             QDoubleSpinBox*& getM,
                             QDoubleSpinBox*& getK)
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Gaussian : Initials parameters");
    QGridLayout* gbox = new QGridLayout();

    QLabel* label_eqn=new QLabel(dialog);
    label_eqn->setPixmap(QPixmap(":/eqn/eqn/gaussian.gif"));
    label_eqn->setAlignment(Qt::AlignHCenter);

    getS=new QDoubleSpinBox(dialog);
    getS->setRange(0.001,1e8);
    getS->setPrefix("Sigma=");
    getM=new QDoubleSpinBox(dialog);
    getM->setRange(-1e8,1e8);
    getM->setPrefix("M=");
    getK=new QDoubleSpinBox(dialog);
    getK->setRange(0.001,1e8);
    getK->setPrefix("K=");
    getK->setValue(1.0);


    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));


    gbox->addWidget(label_eqn,0,0);
    gbox->addWidget(getS,1,0);
    gbox->addWidget(getM,2,0);
    gbox->addWidget(getK,3,0);
    gbox->addWidget(buttonBox,4,0);

    dialog->setLayout(gbox);

    return dialog;
}

Eigen::Vector2d Gaussian::delta(const Eigen::Vector2d& pt)
{
    return Eigen::Vector2d (pt[1]-at(pt[0]),pt[1]-at(pt[0]));
}

double Gaussian::getS()const
{
    return p[0];
}
double Gaussian::getM()const
{
    return p[1];
}
double Gaussian::getK()const
{
    return p[2];
}

void  Gaussian::setS(double R)
{
    p[0]=R;
}
void  Gaussian::setM(double L)
{
    p[1]=L;
}
void  Gaussian::setK(double K)
{
    p[2]=K;
}

double Gaussian::at(double x)const
{
    double S=getS();
    double M=getM();
    double K=getK();
    double I=(x-M)/(S);
    return K/(S*sqrt(2*M_PI))*exp(-0.5*I*I);
}

Eigen::VectorXd Gaussian::at(Eigen::VectorXd f)
{
    Eigen::VectorXd y(f.size());

    for (int i=0; i<f.size(); i++)
    {
        y[i]=at(f[i]);
    }
    return y;
}

int Gaussian::nb_params()
{
    return 3;
}
void Gaussian::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}
const Eigen::VectorXd& Gaussian::getParams()
{
    return p;
}
