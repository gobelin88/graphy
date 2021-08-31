#include "shapes/Circle2D.h"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

Circle2D::Circle2D()
{
    p.resize(3);
    p[0]=0;
    p[1]=0;
    p[2]=1;
}

Circle2D::Circle2D(Eigen::Vector2d center,double radius)
{
    p.resize(3);
    p[0]=center[0];
    p[1]=center[1];
    p[2]=radius;
}

Eigen::Vector2d Circle2D::delta(const Eigen::Vector2d& P)
{
    Eigen::Vector2d CP=P-getCenter();
    return  CP*(1-getRadius()/CP.norm());
}

int Circle2D::nb_params()
{
    return 3;
}
void Circle2D::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}
const Eigen::VectorXd& Circle2D::getParams()
{
    return p;
}

QDialog* Circle2D::createDialog(QDoubleSpinBox*& getR,
                             QDoubleSpinBox*& getA,
                             QDoubleSpinBox*& getB)
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Circle2D : Initials parameters");
    QGridLayout* gbox = new QGridLayout();

    QLabel* label_eqn=new QLabel(dialog);
    label_eqn->setPixmap(QPixmap(":/eqn/eqn/circle.gif"));
    label_eqn->setAlignment(Qt::AlignHCenter);

    getR=new QDoubleSpinBox(dialog);
    getR->setRange(0.0,1e100);
    getR->setDecimals(4);
    getR->setPrefix("r=");
    getR->setValue(1.0);
    getA=new QDoubleSpinBox(dialog);
    getA->setRange(-1e100,1e100);
    getA->setDecimals(4);
    getA->setPrefix("a=");
    getA->setValue(0.0);
    getB=new QDoubleSpinBox(dialog);
    getB->setRange(-1e100,1e100);
    getB->setDecimals(4);
    getB->setPrefix("b=");
    getB->setValue(0.0);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    gbox->addWidget(label_eqn,0,0);
    gbox->addWidget(getR,1,0);
    gbox->addWidget(getA,2,0);
    gbox->addWidget(getB,3,0);
    gbox->addWidget(buttonBox,4,0);

    dialog->setLayout(gbox);

    return dialog;
}

void Circle2D::create(Eigen::VectorXd & x,Eigen::VectorXd & y,int N)
{
    double step=2*M_PI/N;
    x.resize(N);
    y.resize(N);
    for(int i=0;i<N;i++)
    {
        x[i]=getRadius()*cos(i*step)+p[0];
        y[i]=getRadius()*sin(i*step)+p[1];
    }
}

Eigen::Vector2d Circle2D::getCenter()const
{
    return Eigen::Vector2d(p[0],p[1]);
}

double Circle2D::getRadius()const
{
    return p[2];
}
