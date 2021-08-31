#include "shapes/Ellipse2D.h"

Ellipse2D::Ellipse2D()
{
    p.resize(5);
    p[0]=0;
    p[1]=0;
    p[2]=1;
    p[3]=1;
    p[4]=0;
}

Ellipse2D::Ellipse2D(Eigen::Vector2d center,double radiusA,double radiusB,double theta)
{
    p.resize(5);
    p[0]=center[0];
    p[1]=center[1];
    p[2]=radiusA;
    p[3]=radiusB;
    p[4]=theta;
}

QDialog* Ellipse2D::createDialog(QDoubleSpinBox*& getRa,
                             QDoubleSpinBox*& getRb,
                             QDoubleSpinBox*& getA,
                             QDoubleSpinBox*& getB,
                             QDoubleSpinBox*& getT)
{
    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle("Ellipse2D : Initials parameters");
    QGridLayout* gbox = new QGridLayout();

    QLabel* label_eqn=new QLabel(dialog);
    label_eqn->setPixmap(QPixmap(":/eqn/eqn/ellipse.gif"));
    label_eqn->setAlignment(Qt::AlignHCenter);

    getRa=new QDoubleSpinBox(dialog);
    getRa->setRange(0.0,1e100);
    getRa->setDecimals(4);
    getRa->setPrefix("ra=");
    getRa->setValue(1.0);
    getRb=new QDoubleSpinBox(dialog);
    getRb->setRange(0.0,1e100);
    getRb->setDecimals(4);
    getRb->setPrefix("rb=");
    getRb->setValue(1.0);
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
    getT=new QDoubleSpinBox(dialog);
    getT->setRange(-1e100,1e100);
    getT->setDecimals(4);
    getT->setPrefix("theta=");
    getT->setValue(0.0);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    gbox->addWidget(label_eqn,0,0,1,2);
    gbox->addWidget(getRa,1,0);
    gbox->addWidget(getRb,1,1);
    gbox->addWidget(getA,2,0);
    gbox->addWidget(getB,2,1);
    gbox->addWidget(getT,3,0,1,2);
    gbox->addWidget(buttonBox,4,0,1,2);

    dialog->setLayout(gbox);

    return dialog;
}

void Ellipse2D::create(Eigen::VectorXd & x,Eigen::VectorXd & y,int N)
{
    double step=2*M_PI/N;
    x.resize(N);
    y.resize(N);
    for(int i=0;i<N;i++)
    {
        double theta=i*step;
        x[i]=getR(theta)*cos(theta)+p[0];
        y[i]=getR(theta)*sin(theta)+p[1];
    }
}

Eigen::Vector2d Ellipse2D::getCenter()const
{
    return Eigen::Vector2d(p[0],p[1]);
}
double Ellipse2D::getRa()const
{
    return p[2];
}
double Ellipse2D::getRb()const
{
    return p[3];
}
double Ellipse2D::getTheta()const
{
    return fmod(p[4],2*M_PI);
}

double Ellipse2D::getR(double theta)const
{
    double t=theta+getTheta();
    double st=getRb()*sin(t);
    double ct=getRa()*cos(t);
    return getRa()*getRb()/sqrt(ct*ct+st*st);
}

Eigen::Vector2d Ellipse2D::delta(const Eigen::Vector2d& P)
{
    Eigen::Vector2d CP=P-getCenter();
    double t=atan2(CP[1],CP[0]);

    return CP*(1.0-getR(t)/CP.norm());
}

int Ellipse2D::nb_params(){return 5;}
void Ellipse2D::setParams(const Eigen::VectorXd& p){this->p=p;}
const Eigen::VectorXd& Ellipse2D::getParams(){return p;}
