#include "ShapeFit.h"

/////////////////////////////////////////////////////////////////////////////////////
//Some standard Shapes
Plan::Plan(Eigen::Vector3d n0, Eigen::Vector3d barycenter)
{
    p=n0;
    this->barycenter=barycenter;
}

Eigen::Vector3d Plan::delta(const Eigen::Vector3d& pt)
{
    Eigen::Vector3d n=getNormal();
    return (pt-barycenter).dot(n)*n;
}

Eigen::Vector3d Plan::proj(const Eigen::Vector3d& P,const Eigen::Vector3d& V)const
{
    Eigen::Vector3d n=getNormal();
    if (n.dot(V)!=0)
    {
        double k=n.dot(barycenter-P)/n.dot(V);
        return P+k*V;
    }
    else
    {
        return P;
    }
}

int Plan::nb_params()
{
    return 3;
}

void Plan::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}

const Eigen::VectorXd& Plan::getParams()
{
    return p;
}

/////////////////////////////////////////////////////////////////////////////////////
//Some standard Shapes
Sphere::Sphere()
{
    p.resize(4);
    p[0]=0;
    p[1]=0;
    p[2]=0;
    p[3]=0;
}

Sphere::Sphere(Eigen::Vector3d center, double radius)
{
    p.resize(4);
    p[0]=center[0];
    p[1]=center[1];
    p[2]=center[2];
    p[3]=radius>0 ? radius : 0.0 ;
}

Eigen::Vector3d Sphere::delta(const Eigen::Vector3d& pt)
{
    Eigen::Vector3d u=pt-getCenter();
    return  u-u*getRadius()/u.norm();
}

int Sphere::nb_params()
{
    return 4;
}

void Sphere::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}

const Eigen::VectorXd& Sphere::getParams()
{
    return p;
}

/////////////////////////////////////////////////////////////////////////////////////
//Some standard Shapes
Ellipsoid::Ellipsoid()
{
    p.resize(6);
    p[0]=0;
    p[1]=0;
    p[2]=0;
    p[3]=0;
    p[4]=0;
    p[5]=0;
}

Ellipsoid::Ellipsoid(Eigen::Vector3d center, double A, double B, double C)
{
    p.resize(6);
    p[0]=center[0];
    p[1]=center[1];
    p[2]=center[2];
    p[3]=A;
    p[4]=B;
    p[5]=C;
}

Eigen::Vector3d Ellipsoid::delta(const Eigen::Vector3d& pt)
{
    Eigen::Vector3d u=(pt-getCenter()).cwiseProduct(Eigen::Vector3d(1/getA(),1/getB(),1/getC()));
    return  (u-u/u.norm()).cwiseProduct(Eigen::Vector3d(getA(),getB(),getC()));
}

int Ellipsoid::nb_params()
{
    return p.size();
}

void Ellipsoid::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}

const Eigen::VectorXd& Ellipsoid::getParams()
{
    return p;
}


/////////////////////////////////////////////////////////////////////////////////////
void testShapeFit()
{
    std::cout<<"Test Fit Sphere"<<std::endl;

    Eigen::Vector3d center(1,2,3);
    double rayon=1.123456789;

    std::vector<Eigen::Vector3d> points;
    for (int i=0; i<1000; i++)
    {
        double alpha=Ru()*M_PI*2,beta=Ru()*M_PI*2;
        Eigen::Vector3d U(cos(alpha)*cos(beta),sin(beta),sin(alpha)*cos(beta));
        points.push_back( center + rayon*U + Eigen::Vector3d(Rg(0,.05),Rg(0,.05),Rg(0,.05)));
    }

    Sphere sphere(Eigen::Vector3d(0,0,0),0);

    sphere.fit(points,500);

    std::cout<<"center="<<center.transpose()<<std::endl;
    std::cout<<"center_hat="<<sphere.getCenter().transpose()<<std::endl;
    std::cout<<"radius="<<rayon<<std::endl;
    std::cout<<"radius_hat="<<sphere.getRadius()<<std::endl;
    std::cout<<"RMS="<<sphere.getRMS()<<std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////
Circle3D::Circle3D()
{
    p.resize(6);
    p[0]=0;
    p[1]=0;
    p[2]=0;
    p[3]=0;
    p[4]=0;
    p[5]=0.5;
}

Circle3D::Circle3D(Eigen::Vector3d center,double radius)
{
    p.resize(6);
    p[0]=0;
    p[1]=0;
    p[2]=center[0];
    p[3]=center[1];
    p[4]=center[2];
    p[5]=radius;
}

Eigen::Vector3d Circle3D::delta(const Eigen::Vector3d& pt)
{
    Eigen::Vector3d u=pt-getCenter();
    return  Eigen::Vector3d( u.dot(getNormal()), (u-u.dot(getNormal())*getNormal()).norm()-getRadius(), 0 );
}

int Circle3D::nb_params()
{
    return 6;
}
void Circle3D::setParams(const Eigen::VectorXd& p)
{
    this->p=p;
}
const Eigen::VectorXd& Circle3D::getParams()
{
    return p;
}
/////////////////////////////////////////////////////////////////////////////////////
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

double Ru()
{
    return (((double)(rand()%RAND_MAX))/RAND_MAX-0.5)*2;
}

double Rg(double mean,double sigma)
{

    double x=0;
    double y=0;
    while (x==0)
    {
        x=(((double)(rand()%RAND_MAX))/(RAND_MAX-1));
    }
    while (y==0)
    {
        y=(((double)(rand()%RAND_MAX))/(RAND_MAX-1));
    }
    double w=sqrt(-2*log(x))*cos(2*M_PI*y);
    return mean+(sigma*w);
}

/////////////////////////////////////////
CustomExpDialog::CustomExpDialog():QDialog()
{
    this->setMinimumWidth(400);
    this->setLocale(QLocale("C"));
    this->setWindowTitle("CustomExp : Initials parameters");
    QGridLayout* gbox = new QGridLayout();
    vbox=new QVBoxLayout;

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    le_expr=new QLineEdit(this);
    le_expr->setToolTip("ex: A*x^2+B*x+C\nParameters ares specified by capitals letters.");

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(le_expr, SIGNAL(textChanged(QString)), this, SLOT(slot_setExp(QString)));

    l_error=new QLabel();

    gbox->addWidget(new QLabel("Expression f(x)="),0,0);
    gbox->addWidget(le_expr,0,1);
    gbox->addWidget(l_error,1,0,1,2);
    gbox->addLayout(vbox,2,0,1,2);
    gbox->addWidget(buttonBox,3,0,1,2);

    setLayout(gbox);

    valid=false;
}

const QVector<QDoubleSpinBox*> & CustomExpDialog::getSpinBoxParams()
{
    return sp_params;
}

bool CustomExpDialog::isValid()
{
    return valid;
}

void CustomExpDialog::slot_p0Changed()
{
    if(isValid())
    {
        customRealExp.setParams(getP0());
        l_error->setText(QString("Ok f(0)=%1").arg(customRealExp.at(0)));
    }
}

void CustomExpDialog::slot_setExp(QString expStr)
{
    if(customRealExp.setExpression(expStr))
    {
        std::cout<<customRealExp.getParamsNames().size()<<std::endl;

        for(int i=0;i<sp_params.size();i++)
        {
            delete sp_params[i];
        }
        sp_params.clear();

        for(int i=0;i<customRealExp.getParamsNames().size();i++)
        {
            QDoubleSpinBox * sp_newParams=new QDoubleSpinBox(this);
            sp_newParams->setPrefix(customRealExp.getParamsNames()[i]+QString("="));
            sp_newParams->setToolTip(QString("Initial value for parameter %1").arg(customRealExp.getParamsNames()[i]));
            sp_params.append(sp_newParams);
            vbox->addWidget(sp_newParams);
            connect(sp_newParams,SIGNAL(valueChanged(double)),this,SLOT(slot_p0Changed()));
        }

        l_error->setStyleSheet("color: #003F00");
        customRealExp.setParams(getP0());
        l_error->setText(QString("Ok f(0)=%1").arg(customRealExp.at(0)));
        valid=true;
    }
    else
    {
        for(int i=0;i<sp_params.size();i++)
        {
            delete sp_params[i];
        }
        sp_params.clear();

        l_error->setText(customRealExp.getError());
        l_error->setStyleSheet("color: red");
        valid=false;
    }
}

VectorXd CustomExpDialog::getP0()
{
    VectorXd p0(sp_params.size());
    for(int i=0;i<sp_params.size();i++)
    {
        p0[i]=sp_params[i]->value();
    }
    return p0;
}
