#include <Functor.h>
#include <QVector>
#include <iostream>

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#ifndef SHAPE_FIT_H
#define SHAPE_FIT_H

/////////////////////////////////////////////////////////////////////////////////////
#define EPS (sqrt(Eigen::NumTraits<double>::epsilon()))


/**
 * @class Shape
 * @brief Interface pour une forme quelconque.
 */
template<typename T>
class Shape:public Functor<double>
{
public:

    virtual T delta(const T& p)const=0;
    virtual int nb_params()=0;

    virtual void setParams(const Eigen::VectorXd& p)=0;
    virtual const Eigen::VectorXd& getParams()=0;

    int operator()(const Eigen::VectorXd& p, Eigen::VectorXd& h)
    {
        setParams(p);

        T dn;
        int n=dn.rows();

        for (int i=0; i<values()/n; i++)
        {
            T d=delta(points[i]);

            for (int k=0; k<n; k++)
            {
                h[n*i+k]=d[k];
            }
        }

        return 0;
    }

    int df(const Eigen::VectorXd& p, Eigen::MatrixXd& J)
    {
        for (int j=0; j< inputs(); j++)
        {
            Eigen::VectorXd dp=Eigen::VectorXd::Zero(inputs());
            dp[j]=EPS;
            Eigen::VectorXd h_df_p(values());
            this->operator ()(p+dp,h_df_p);
            Eigen::VectorXd h_df_m(values());
            this->operator ()(p-dp,h_df_m);

            J.col(j)=(h_df_p-h_df_m)/(2*EPS);
        }

        return 0;
    }

    void setPointsList(std::vector<T> points_list)
    {
        m_values=(int)points_list.size()*points_list[0].rows();
        m_inputs=nb_params();
        points=points_list;
    }

    void fit(std::vector<T> points, int it)
    {
        Eigen::VectorXd p=this->getParams();
        this->setPointsList(points);
        Eigen::LevenbergMarquardt<Shape<T>,double> lm(*this);
        lm.parameters.maxfev = it;
        lm.parameters.factor = 2;
        lm.minimize(p);
    }

    double getRMS()
    {
        double rms=0.0;
        for (unsigned int i=0; i< points.size(); i++)
        {
            T delta=this->delta(points[i]);
            rms+=delta.squaredNorm();
        }
        return rms;
    }

    std::vector<T> getErr()
    {
        std::vector<Eigen::Vector3d> err_dist(points.size());
        for (unsigned int i=0; i< points.size(); i++)
        {
            err_dist[i]=this->delta(points[i]);
        }
        return err_dist;
    }

private:
    std::vector<T> points;
};

/////////////////////////////////////////////////////////////////////////////////////
//Some standard Shapes

/**
 * @class Sphere
 * @brief Décrit une forme sphérique.
 */
class Sphere: public Shape<Eigen::Vector3d>
{
public:
    Sphere();
    Sphere(Eigen::Vector3d center, double radius);
    inline Eigen::Vector3d getCenter()const
    {
        return Eigen::Vector3d(p[0],p[1],p[2]);
    }
    inline double getRadius()const
    {
        return p[3];
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)const;
    int nb_params();
    void setParams(const Eigen::VectorXd& p);
    const Eigen::VectorXd& getParams();

private:
    Eigen::VectorXd p;
};

/**
 * @class Plan
 * @brief Décrit un plan.
 */
class Plan: public Shape<Eigen::Vector3d>
{
public:
    Plan();
    inline Eigen::Vector3d getNormal()const
    {
        return Eigen::Vector3d(sin(p[0])*sin(p[1]),cos(p[1]),cos(p[0])*sin(p[1]));
    }
    inline double getOffset()const
    {
        return p[2];
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)const;
    int nb_params();
    void setParams(const Eigen::VectorXd& p);
    const Eigen::VectorXd& getParams();

private:
    Eigen::VectorXd p;
};

/**
 * @class Circle
 * @brief Décrit un cercle.
 */
class Circle3D: public Shape<Eigen::Vector3d>
{
public:
    Circle3D();
    Circle3D(Eigen::Vector3d center,double radius);
    inline Eigen::Vector3d getNormal()const
    {
        return Eigen::Vector3d(sin(p[0])*sin(p[1]),cos(p[1]),cos(p[0])*sin(p[1]));
    }
    inline Eigen::Vector3d getCenter()const
    {
        return Eigen::Vector3d(p[2],p[3],p[4]);
    }
    inline double getRadius()const
    {
        return p[5];
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)const;
    int nb_params();
    void setParams(const Eigen::VectorXd& p);
    const Eigen::VectorXd& getParams();

private:
    Eigen::VectorXd p;
};

/**
 * @brief The Gaussian class
 */
class Sinusoide: public Shape<Eigen::Vector2d>
{
public:

    Sinusoide(double A,double F,double P)
    {
        p.resize(3);
        setA(A);
        setF(F);
        setP(P);
    }

    static QDialog* createDialog(QDoubleSpinBox*& getA,
                                 QDoubleSpinBox*& getF,
                                 QDoubleSpinBox*& getP)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        getA=new QDoubleSpinBox(dialog);
        getA->setRange(0.0001,1e8);
        getA->setDecimals(4);
        getA->setPrefix("A=");
        getF=new QDoubleSpinBox(dialog);
        getF->setRange(0.0001,1e8);
        getF->setDecimals(4);
        getF->setPrefix("F=");
        getP=new QDoubleSpinBox(dialog);
        getP->setRange(0.0001,1e8);
        getP->setDecimals(4);
        getP->setPrefix("Phi=");
        getP->setValue(0.0);

        gbox->addWidget(getA,0,0);
        gbox->addWidget(getF,1,0);
        gbox->addWidget(getP,2,0);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,3,0);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector2d delta(const Eigen::Vector2d& pt)const
    {
        return Eigen::Vector2d (pt[1]-at(pt[0]),0);
    }

    double getA()const
    {
        return p[0];
    }
    double getF()const
    {
        return p[1];
    }
    double getP()const
    {
        return p[2];
    }

    void  setA(double A)
    {
        p[0]=A;
    }
    void  setF(double F)
    {
        p[1]=F;
    }
    void  setP(double P)
    {
        p[2]=P;
    }

    void regularized()
    {
        double A=getA();
        double F=getF();
        double P=getP();

        if (A<0)
        {
            A=-A;
            P+=M_PI;
        }
        if (F<0)
        {
            F=-F;
            P+=M_PI;
        }
        P=std::fmod(P,2*M_PI);

        setA(A);
        setF(F);
        setP(P);
    }

    double at(double t)const
    {
        double A=getA();
        double F=getF();
        double P=getP();
        std::cout<<t<<" "<<A<<" "<<F<<" "<<P<<" "<<A* std::sin(2*M_PI*F*t+P)<<std::endl;
        return A*std::sin(2*M_PI*F*t+P);
    }

    QVector<double> at(QVector<double> f)
    {
        QVector<double> y(f.size());

        for (int i=0; i<f.size(); i++)
        {
            y[i]=at(f[i]);
        }
        return y;
    }

    int nb_params()
    {
        return 3;
    }
    void setParams(const Eigen::VectorXd& p)
    {
        this->p=p;
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
};

/**
 * @brief The Gaussian class
 */
class Sigmoid: public Shape<Eigen::Vector2d>
{
public:

    Sigmoid(double A,double B,double C,double P,double Asymp)
    {
        p.resize(5);
        setA(A);
        setB(B);
        setC(C);
        setP(P);
        setAsymp(Asymp);
    }

    static QDialog* createDialog(QDoubleSpinBox*& getA,
                                 QDoubleSpinBox*& getB,
                                 QDoubleSpinBox*& getC,
                                 QDoubleSpinBox*& getP,
                                 QDoubleSpinBox*& getAsymp)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

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
        getAsymp=new QDoubleSpinBox(dialog);
        getAsymp->setRange(-1e8,1e8);
        getAsymp->setDecimals(4);
        getAsymp->setPrefix("Asymp=");


        gbox->addWidget(getA,0,0);
        gbox->addWidget(getB,1,0);
        gbox->addWidget(getC,2,0);
        gbox->addWidget(getP,3,0);
        gbox->addWidget(getAsymp,4,0);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,5,0);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector2d delta(const Eigen::Vector2d& pt)const
    {
        return Eigen::Vector2d (pt[1]-at(pt[0]),0);
    }

    double getA()const
    {
        return p[0];
    }
    double getB()const
    {
        return p[1];
    }
    double getC()const
    {
        return p[2];
    }
    double getP()const
    {
        return p[3];
    }
    double getAsymp()const
    {
        return p[4];
    }

    void  setA(double A)
    {
        p[0]=A;
    }
    void  setB(double B)
    {
        p[1]=B;
    }
    void  setC(double C)
    {
        p[2]=C;
    }
    void  setP(double P)
    {
        p[3]=P;
    }
    void  setAsymp(double Asymp)
    {
        p[4]=Asymp;
    }

    double at(double t)const
    {
        double A=getA();
        double B=getB();
        double C=getC();
        double P=getP();
        double Asymp=getAsymp();

        return ((B-A)/(1+std::exp((C-t)*P*(B-A)))+A)*(std::abs(Asymp*(C-t))+1);//
    }

    QVector<double> at(QVector<double> t)
    {
        QVector<double> y(t.size());

        for (int i=0; i<t.size(); i++)
        {
            y[i]=at(t[i]);
        }
        return y;
    }

    int nb_params()
    {
        return this->p.rows();
    }
    void setParams(const Eigen::VectorXd& p)
    {
        this->p=p;
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
};

/**
 * @brief The Gaussian class
 */
class Gaussian: public Shape<Eigen::Vector2d>
{
public:

    Gaussian(double S,double M,double K)
    {
        p.resize(3);
        setS(S);
        setM(M);
        setK(K);
    }

    static QDialog* createDialog(QDoubleSpinBox*& getS,
                                 QDoubleSpinBox*& getM,
                                 QDoubleSpinBox*& getK)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        getS=new QDoubleSpinBox(dialog);
        getS->setRange(0.001,1e8);
        getS->setPrefix("Sigma=");
        getM=new QDoubleSpinBox(dialog);
        getM->setRange(0.001,1e8);
        getM->setPrefix("Mean=");
        getK=new QDoubleSpinBox(dialog);
        getK->setRange(0.001,1e8);
        getK->setPrefix("K=");
        getK->setValue(1.0);

        gbox->addWidget(getS,0,0);
        gbox->addWidget(getM,1,0);
        gbox->addWidget(getK,2,0);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,3,0);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector2d delta(const Eigen::Vector2d& pt)const
    {
        return Eigen::Vector2d (pt[1]-at(pt[0]),pt[1]-at(pt[0]));
    }

    double getS()const
    {
        return p[0];
    }
    double getM()const
    {
        return p[1];
    }
    double getK()const
    {
        return p[2];
    }

    void  setS(double R)
    {
        p[0]=R;
    }
    void  setM(double L)
    {
        p[1]=L;
    }
    void  setK(double K)
    {
        p[2]=K;
    }

    double at(double x)const
    {
        double S=getS();
        double M=getM();
        double K=getK();
        double I=(x-M)/(S);
        return K/(S*sqrt(2*M_PI))*exp(-0.5*I*I);
    }

    QVector<double> at(QVector<double> f)
    {
        QVector<double> y(f.size());

        for (int i=0; i<f.size(); i++)
        {
            y[i]=at(f[i]);
        }
        return y;
    }

    int nb_params()
    {
        return 3;
    }
    void setParams(const Eigen::VectorXd& p)
    {
        this->p=p;
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
};

/**
 * @class RLpC
 */
class RLpC: public Shape<Eigen::Vector2d>
{
public:

    RLpC(double R,double L,double C,bool fixedR,bool fixedL,bool fixedC)
    {
        fixed[0]=fixedR;
        fixed[1]=fixedL;
        fixed[2]=fixedC;

        p.resize(3-(int(fixedR)+int(fixedL)+int(fixedC)));

        idr=0;
        idl=(fixed[0])?0:1;
        idc=(fixed[1])?0:idl+1;

        std::cout<<idr<<" "<<idl<<" "<<idc<<" "<<p.size()<<std::endl;

        setR(R);
        setL(L);
        setC(C);

        std::cout<<idr<<" "<<idl<<" "<<idc<<std::endl;
    }

    static QDialog* createDialog(QDoubleSpinBox*& getR,
                                 QDoubleSpinBox*& getL,
                                 QDoubleSpinBox*& getC,
                                 QCheckBox*& fixedR,
                                 QCheckBox*& fixedL,
                                 QCheckBox*& fixedC)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        getR=new QDoubleSpinBox(dialog);
        getR->setRange(0.001,1e8);
        getR->setDecimals(4);
        getR->setPrefix("R=");
        getR->setSuffix("Ohm");
        getL=new QDoubleSpinBox(dialog);
        getL->setRange(0.001,1e8);
        getL->setDecimals(4);
        getL->setPrefix("L=");
        getL->setSuffix("mH");
        getC=new QDoubleSpinBox(dialog);
        getC->setRange(0.001,1e8);
        getC->setDecimals(4);
        getC->setPrefix("C=");
        getC->setSuffix("nF");
        fixedR=new QCheckBox("fixed?");
        fixedL=new QCheckBox("fixed?");
        fixedC=new QCheckBox("fixed?");

        gbox->addWidget(getR,0,0);
        gbox->addWidget(getL,1,0);
        gbox->addWidget(getC,2,0);

        gbox->addWidget(fixedR,0,1);
        gbox->addWidget(fixedL,1,1);
        gbox->addWidget(fixedC,2,1);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,4,0,2,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    double getFc()const
    {
        return 1.0/(2*M_PI*std::sqrt(getL()*getC()*1e-12));
    }

    Eigen::Vector2d delta(const Eigen::Vector2d& pt)const
    {
        return Eigen::Vector2d (pt[1]-at(pt[0]),pt[1]-at(pt[0]));
    }

    double getR()const
    {
        return (fixed[0])? fixedvalue[0]: p[idr];
    }
    double getL()const
    {
        return (fixed[1])? fixedvalue[1]: p[idl];
    }
    double getC()const
    {
        return (fixed[2])? fixedvalue[2]: p[idc];
    }

    void  setR(double R)
    {
        if (fixed[0])
        {
            fixedvalue[0]=R;
        }
        else
        {
            p[idr]=R;
        }
    }
    void  setL(double L)
    {
        if (fixed[1])
        {
            fixedvalue[1]=L;
        }
        else
        {
            p[idl]=L;
        }
    }
    void  setC(double C)
    {
        if (fixed[2])
        {
            fixedvalue[2]=C;
        }
        else
        {
            p[idc]=C;
        }
    }

    double at(double f)const
    {
        double w=2*M_PI*f;//

        double R=getR();
        double L=getL()*1e-3;
        double C=getC()*1e-9;

        return std::sqrt(R*R+L*L*w*w)/std::sqrt( (1-L*C*w*w)*(1-L*C*w*w)+(R*C*w)*(R*C*w) );
    }

    QVector<double> at(QVector<double> f)
    {
        QVector<double> y(f.size());

        for (int i=0; i<f.size(); i++)
        {
            y[i]=at(f[i]);
        }
        return y;
    }

    int nb_params()
    {
        return p.rows();
    }
    void setParams(const Eigen::VectorXd& p)
    {
        this->p=p;
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
    bool fixed[3];
    double fixedvalue[3];

    int idr,idl,idc;
};

/**
 * @class RLpC
 */
class RLpC_cplx: public Shape<Eigen::Vector3d>
{
public:

    RLpC_cplx(double R,double L,double C,bool fixedR,bool fixedL,bool fixedC)
    {
        fixed[0]=fixedR;
        fixed[1]=fixedL;
        fixed[2]=fixedC;

        p.resize(3-(int(fixedR)+int(fixedL)+int(fixedC)));

        idr=0;
        idl=(fixed[0])?0:1;
        idc=(fixed[1])?0:idl+1;

        setR(R);
        setL(L);
        setC(C);
    }

    static QDialog* createDialog(QDoubleSpinBox*& getR,
                                 QDoubleSpinBox*& getL,
                                 QDoubleSpinBox*& getC,
                                 QCheckBox*& fixedR,
                                 QCheckBox*& fixedL,
                                 QCheckBox*& fixedC)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        getR=new QDoubleSpinBox(dialog);
        getR->setRange(0.001,1e8);
        getR->setDecimals(4);
        getR->setPrefix("R=");
        getR->setSuffix("Ohm");
        getL=new QDoubleSpinBox(dialog);
        getL->setRange(0.001,1e8);
        getL->setDecimals(4);
        getL->setPrefix("L=");
        getL->setSuffix("mH");
        getC=new QDoubleSpinBox(dialog);
        getC->setRange(0.001,1e8);
        getC->setDecimals(4);
        getC->setPrefix("C=");
        getC->setSuffix("nF");
        fixedR=new QCheckBox("fixed?");
        fixedL=new QCheckBox("fixed?");
        fixedC=new QCheckBox("fixed?");

        gbox->addWidget(getR,0,0);
        gbox->addWidget(getL,1,0);
        gbox->addWidget(getC,2,0);

        gbox->addWidget(fixedR,0,1);
        gbox->addWidget(fixedL,1,1);
        gbox->addWidget(fixedC,2,1);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,4,0,2,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    double getFc()const
    {
        return 1.0/(2*M_PI*std::sqrt(getL()*getC()*1e-12));
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)const
    {
        Eigen::Vector2d res=at(pt[0]);

//            std::complex<double> Zt(res[0]*cos(res[1]*M_PI/180),res[0]*sin(res[1]*M_PI/180));
//            std::complex<double> Zm(pt[1]*cos(pt[2]*M_PI/180),pt[1]*sin(pt[2]*M_PI/180));
//            return Eigen::Vector3d (Zt.real()-Zm.real(),Zt.imag()-Zm.imag(),0);

        return Eigen::Vector3d (pt[1]-res[0],pt[2]-res[1],0);
    }

    double getR()const
    {
        return (fixed[0])? fixedvalue[0]: p[idr];
    }
    double getL()const
    {
        return (fixed[1])? fixedvalue[1]: p[idl];
    }
    double getC()const
    {
        return (fixed[2])? fixedvalue[2]: p[idc];
    }

    void  setR(double R)
    {
        if (fixed[0])
        {
            fixedvalue[0]=R;
        }
        else
        {
            p[idr]=R;
        }
    }
    void  setL(double L)
    {
        if (fixed[1])
        {
            fixedvalue[1]=L;
        }
        else
        {
            p[idl]=L;
        }
    }
    void  setC(double C)
    {
        if (fixed[2])
        {
            fixedvalue[2]=C;
        }
        else
        {
            p[idc]=C;
        }
    }

    Eigen::Vector2d at(double f)const
    {
        double w=2*M_PI*f;//

        std::complex<double> R(getR(),0.0);
        std::complex<double> Lp(0.0,w*getL()*1e-3);
        std::complex<double> Cp(0.0,w*getC()*1e-9);
        std::complex<double> G=(R+Lp)/(1.0+R*Cp+Lp*Cp);

        return Eigen::Vector2d(std::abs(G), std::arg(G)*180.0/M_PI );
    }

    void at(QVector<double> f,QVector<double>& g,QVector<double>& p)
    {
        g.resize(f.size());
        p.resize(f.size());

        for (int i=0; i<f.size(); i++)
        {
            Eigen::Vector2d res=at(f[i]);
            g[i]=res[0];
            p[i]=res[1];
        }
    }

    int nb_params()
    {
        return int(p.rows());
    }
    void setParams(const Eigen::VectorXd& p)
    {
        for (int i=0; i<p.rows(); i++)
        {
            this->p[i]=std::abs(p[i]);
        }
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
    bool fixed[3];
    double fixedvalue[3];

    int idr,idl,idc;
};

class RLC_cplx: public Shape<Eigen::Vector3d>
{
public:

    RLC_cplx(double R,double L,double C,bool fixedR,bool fixedL,bool fixedC)
    {
        fixed[0]=fixedR;
        fixed[1]=fixedL;
        fixed[2]=fixedC;

        p.resize(3-(int(fixedR)+int(fixedL)+int(fixedC)));

        idr=0;
        idl=(fixed[0])?0:1;
        idc=(fixed[1])?0:idl+1;

        setR(R);
        setL(L);
        setC(C);
    }

    static QDialog* createDialog(QDoubleSpinBox*& getR,
                                 QDoubleSpinBox*& getL,
                                 QDoubleSpinBox*& getC,
                                 QCheckBox*& fixedR,
                                 QCheckBox*& fixedL,
                                 QCheckBox*& fixedC)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        getR=new QDoubleSpinBox(dialog);
        getR->setRange(0.001,1e8);
        getR->setDecimals(4);
        getR->setPrefix("R=");
        getR->setSuffix("Ohm");
        getL=new QDoubleSpinBox(dialog);
        getL->setRange(0.001,1e8);
        getL->setDecimals(4);
        getL->setPrefix("L=");
        getL->setSuffix("mH");
        getC=new QDoubleSpinBox(dialog);
        getC->setRange(0.001,1e8);
        getC->setDecimals(4);
        getC->setPrefix("C=");
        getC->setSuffix("nF");
        fixedR=new QCheckBox("fixed?");
        fixedL=new QCheckBox("fixed?");
        fixedC=new QCheckBox("fixed?");

        gbox->addWidget(getR,0,0);
        gbox->addWidget(getL,1,0);
        gbox->addWidget(getC,2,0);

        gbox->addWidget(fixedR,0,1);
        gbox->addWidget(fixedL,1,1);
        gbox->addWidget(fixedC,2,1);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,4,0,2,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    double getFc()const
    {
        return 1.0/(2*M_PI*std::sqrt(getL()*getC()*1e-12));
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)const
    {
        Eigen::Vector2d res=at(pt[0]);

//            std::complex<double> Zt(res[0]*cos(res[1]*M_PI/180),res[0]*sin(res[1]*M_PI/180));
//            std::complex<double> Zm(pt[1]*cos(pt[2]*M_PI/180),pt[1]*sin(pt[2]*M_PI/180));
//            return Eigen::Vector3d (Zt.real()-Zm.real(),Zt.imag()-Zm.imag(),0);

        return Eigen::Vector3d (pt[1]-res[0],pt[2]-res[1],0);
    }

    double getR()const
    {
        return (fixed[0])? fixedvalue[0]: p[idr];
    }
    double getL()const
    {
        return (fixed[1])? fixedvalue[1]: p[idl];
    }
    double getC()const
    {
        return (fixed[2])? fixedvalue[2]: p[idc];
    }

    void  setR(double R)
    {
        if (fixed[0])
        {
            fixedvalue[0]=R;
        }
        else
        {
            p[idr]=R;
        }
    }
    void  setL(double L)
    {
        if (fixed[1])
        {
            fixedvalue[1]=L;
        }
        else
        {
            p[idl]=L;
        }
    }
    void  setC(double C)
    {
        if (fixed[2])
        {
            fixedvalue[2]=C;
        }
        else
        {
            p[idc]=C;
        }
    }

    Eigen::Vector2d at(double f)const
    {
        double w=2*M_PI*f;//

        std::complex<double> R(getR(),0.0);
        std::complex<double> Lp(0.0,w*getL()*1e-3);
        std::complex<double> Cp(0.0,w*getC()*1e-9);
        std::complex<double> G=(R+Lp+1.0/Cp);

        return Eigen::Vector2d(std::abs(G), std::arg(G)*180.0/M_PI );
    }

    void at(QVector<double> f,QVector<double>& g,QVector<double>& p)
    {
        g.resize(f.size());
        p.resize(f.size());

        for (int i=0; i<f.size(); i++)
        {
            Eigen::Vector2d res=at(f[i]);
            g[i]=res[0];
            p[i]=res[1];
        }
    }

    int nb_params()
    {
        return int(p.rows());
    }
    void setParams(const Eigen::VectorXd& p)
    {
        for (int i=0; i<p.rows(); i++)
        {
            this->p[i]=std::abs(p[i]);
        }
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
    bool fixed[3];
    double fixedvalue[3];

    int idr,idl,idc;
};


/**
 * @class RLpC
 */
class RaLpCpRb_cplx: public Shape<Eigen::Vector3d>
{
public:

    RaLpCpRb_cplx(double Ra,double L,double C,double Rb,bool fixedRa,bool fixedL,bool fixedC,bool fixedRb)
    {
        fixed[0]=fixedRa;
        fixed[1]=fixedL;
        fixed[2]=fixedC;
        fixed[3]=fixedRb;

        int nbp=4-(int(fixedRa)+int(fixedL)+int(fixedC)+int(fixedRb));
        p.resize(nbp);

        idra=(nbp-1);
        idl =(nbp-1)-(int(!fixedRa));
        idc =(nbp-1)-(int(!fixedRa)+int(!fixedL));
        idrb=(nbp-1)-(int(!fixedRa)+int(!fixedL)+int(!fixedC));

        std::cout<<idra<<" "<<idl<<" "<<idc<<" "  <<idrb<<" "<<std::endl;

        setRa(Ra);
        setL(L);
        setC(C);
        setRb(Rb);
    }

    static QDialog* createDialog(QDoubleSpinBox*& getRa,
                                 QDoubleSpinBox*& getL,
                                 QDoubleSpinBox*& getC,
                                 QDoubleSpinBox*& getRb,
                                 QCheckBox*& fixedRa,
                                 QCheckBox*& fixedL,
                                 QCheckBox*& fixedC,
                                 QCheckBox*& fixedRb)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        getRa=new QDoubleSpinBox(dialog);
        getRa->setRange(0.001,1e8);
        getRa->setDecimals(4);
        getRa->setPrefix("R=");
        getRa->setSuffix("Ohm");
        getL=new QDoubleSpinBox(dialog);
        getL->setRange(0.001,1e8);
        getL->setDecimals(4);
        getL->setPrefix("L=");
        getL->setSuffix("mH");
        getC=new QDoubleSpinBox(dialog);
        getC->setRange(0.001,1e8);
        getC->setDecimals(4);
        getC->setPrefix("C=");
        getC->setSuffix("nF");
        getRb=new QDoubleSpinBox(dialog);
        getRb->setRange(0.001,1e8);
        getRb->setDecimals(4);
        getRb->setPrefix("R=");
        getRb->setSuffix("kOhm");
        fixedRa=new QCheckBox("fixed?");
        fixedL=new QCheckBox("fixed?");
        fixedC=new QCheckBox("fixed?");
        fixedRb=new QCheckBox("fixed?");

        gbox->addWidget(getRa,0,0);
        gbox->addWidget(getL,1,0);
        gbox->addWidget(getC,2,0);
        gbox->addWidget(getRb,3,0);

        gbox->addWidget(fixedRa,0,1);
        gbox->addWidget(fixedL,1,1);
        gbox->addWidget(fixedC,2,1);
        gbox->addWidget(fixedRb,3,1);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,4,0,2,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)const
    {
        Eigen::Vector2d res=at(pt[0]);

//            std::complex<double> Zt(res[0]*cos(res[1]*M_PI/180),res[0]*sin(res[1]*M_PI/180));
//            std::complex<double> Zm(pt[1]*cos(pt[2]*M_PI/180),pt[1]*sin(pt[2]*M_PI/180));
//            return Eigen::Vector3d (Zt.real()-Zm.real(),Zt.imag()-Zm.imag(),0);

        return Eigen::Vector3d (pt[1]-res[0],pt[2]-res[1],0);
    }

    double getRa()const
    {
        return std::abs( (fixed[0])? fixedvalue[0]: p[idra] );
    }
    double getL() const
    {
        return std::abs( (fixed[1])? fixedvalue[1]: p[idl]  );
    }
    double getC() const
    {
        return std::abs( (fixed[2])? fixedvalue[2]: p[idc]  );
    }
    double getRb()const
    {
        return std::abs( (fixed[3])? fixedvalue[3]: p[idrb] );
    }

    void  setRa(double Ra)
    {
        if (fixed[0])
        {
            fixedvalue[0]=Ra;
        }
        else
        {
            p[idra]=Ra;
        }
    }
    void  setL(double L)
    {
        if (fixed[1])
        {
            fixedvalue[1]=L ;
        }
        else
        {
            p[idl]=L;
        }
    }
    void  setC(double C)
    {
        if (fixed[2])
        {
            fixedvalue[2]=C ;
        }
        else
        {
            p[idc]=C;
        }
    }
    void  setRb(double Rb)
    {
        if (fixed[3])
        {
            fixedvalue[3]=Rb;
        }
        else
        {
            p[idrb]=Rb;
        }
    }

    Eigen::Vector2d at(double f)const
    {
        double w=2*M_PI*f;//

        std::complex<double> Ra(getRa(),0.0),Rb(getRb()*1e3,0.0);
        std::complex<double> Lp(0.0,w*getL()*1e-3);
        std::complex<double> Cp(0.0,w*getC()*1e-9);
        std::complex<double> G=1.0/(1.0/(Ra+Lp)+Cp+1.0/Rb);

        return Eigen::Vector2d(std::abs(G), std::arg(G)*180.0/M_PI );
    }

    void at(QVector<double> f,QVector<double>& g,QVector<double>& phi)
    {
        g.resize(f.size());
        phi.resize(f.size());

        for (int i=0; i<f.size(); i++)
        {
            Eigen::Vector2d res=at(f[i]);
            g[i]=res[0];
            phi[i]=res[1];
        }
    }

    int nb_params()
    {
        return int(p.rows());
    }
    void setParams(const Eigen::VectorXd& p)
    {
        for (int i=0; i<p.rows(); i++)
        {
            this->p[i]=std::abs(p[i]);
        }
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
    bool fixed[4];
    double fixedvalue[4];

    int idra,idl,idc,idrb;
};

/**
 * @class RLpC
 */
class RLpCaCb_cplx: public Shape<Eigen::Vector3d>
{
public:

    RLpCaCb_cplx(double R,double L,double Ca,double Cb,bool fixedR,bool fixedL,bool fixedCa,bool fixedCb)
    {
        fixed[0]=fixedR;
        fixed[1]=fixedL;
        fixed[2]=fixedCa;
        fixed[3]=fixedCb;

        int nbp=4-(int(fixedR)+int(fixedL)+int(fixedCa)+int(fixedCb));
        p.resize(nbp);

        idr=(nbp-1);
        idl =(nbp-1)-(int(!fixedR));
        idca =(nbp-1)-(int(!fixedR)+int(!fixedL));
        idcb=(nbp-1)-(int(!fixedR)+int(!fixedL)+int(!fixedCa));

        std::cout<<idr<<" "<<idl<<" "<<idca<<" "  <<idcb<<" "<<std::endl;

        setR(R);
        setL(L);
        setCa(Ca);
        setCb(Cb);
    }

    static QDialog* createDialog(QDoubleSpinBox*& getR,
                                 QDoubleSpinBox*& getL,
                                 QDoubleSpinBox*& getCa,
                                 QDoubleSpinBox*& getCb,
                                 QCheckBox*& fixedR,
                                 QCheckBox*& fixedL,
                                 QCheckBox*& fixedCa,
                                 QCheckBox*& fixedCb)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        getR=new QDoubleSpinBox(dialog);
        getR->setRange(0.001,1e8);
        getR->setDecimals(4);
        getR->setPrefix("R=");
        getR->setSuffix("Ohm");
        getL=new QDoubleSpinBox(dialog);
        getL->setRange(0.001,1e8);
        getL->setDecimals(4);
        getL->setPrefix("L=");
        getL->setSuffix("mH");
        getCa=new QDoubleSpinBox(dialog);
        getCa->setRange(0.001,1e8);
        getCa->setDecimals(4);
        getCa->setPrefix("Ca=");
        getCa->setSuffix("nF");
        getCb=new QDoubleSpinBox(dialog);
        getCb->setRange(0.001,1e8);
        getCb->setDecimals(4);
        getCb->setPrefix("Cb=");
        getCb->setSuffix("nF");
        fixedR=new QCheckBox("fixed?");
        fixedL=new QCheckBox("fixed?");
        fixedCa=new QCheckBox("fixed?");
        fixedCb=new QCheckBox("fixed?");

        gbox->addWidget(getR,0,0);
        gbox->addWidget(getL,1,0);
        gbox->addWidget(getCa,2,0);
        gbox->addWidget(getCb,3,0);

        gbox->addWidget(fixedR,0,1);
        gbox->addWidget(fixedL,1,1);
        gbox->addWidget(fixedCa,2,1);
        gbox->addWidget(fixedCb,3,1);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,4,0,2,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)const
    {
        Eigen::Vector2d res=at(pt[0]);

//            std::complex<double> Zt(res[0]*cos(res[1]*M_PI/180),res[0]*sin(res[1]*M_PI/180));
//            std::complex<double> Zm(pt[1]*cos(pt[2]*M_PI/180),pt[1]*sin(pt[2]*M_PI/180));
//            return Eigen::Vector3d (Zt.real()-Zm.real(),Zt.imag()-Zm.imag(),0);

        return Eigen::Vector3d (pt[1]-res[0],pt[2]-res[1],0);
    }

    double getR()const
    {
        return std::abs( (fixed[0])? fixedvalue[0]: p[idr] );
    }
    double getL() const
    {
        return std::abs( (fixed[1])? fixedvalue[1]: p[idl]  );
    }
    double getCa() const
    {
        return std::abs( (fixed[2])? fixedvalue[2]: p[idca]  );
    }
    double getCb()const
    {
        return std::abs( (fixed[3])? fixedvalue[3]: p[idcb] );
    }

    void  setR(double R)
    {
        if (fixed[0])
        {
            fixedvalue[0]=R;
        }
        else
        {
            p[idr]=R;
        }
    }
    void  setL(double L)
    {
        if (fixed[1])
        {
            fixedvalue[1]=L ;
        }
        else
        {
            p[idl]=L;
        }
    }
    void  setCa(double Ca)
    {
        if (fixed[2])
        {
            fixedvalue[2]=Ca ;
        }
        else
        {
            p[idca]=Ca;
        }
    }
    void  setCb(double Cb)
    {
        if (fixed[3])
        {
            fixedvalue[3]=Cb;
        }
        else
        {
            p[idcb]=Cb;
        }
    }

    Eigen::Vector2d at(double f)const
    {
        double w=2*M_PI*f;//

        std::complex<double> Zl(0.0,w*getL()*1e-3);
        std::complex<double> Zca(0.0,-1/(w*getCa()*1e-9));
        std::complex<double> Zcb(0.0,-1/(w*getCb()*1e-9));
        std::complex<double> G=((Zl+getR())*Zca)/(Zl+getR()+Zca)+Zcb;

        return Eigen::Vector2d(std::abs(G), std::arg(G)*180.0/M_PI );
    }

    void at(QVector<double> f,QVector<double>& g,QVector<double>& phi)
    {
        g.resize(f.size());
        phi.resize(f.size());

        for (int i=0; i<f.size(); i++)
        {
            Eigen::Vector2d res=at(f[i]);
            g[i]=res[0];
            phi[i]=res[1];
        }
    }

    int nb_params()
    {
        return int(p.rows());
    }
    void setParams(const Eigen::VectorXd& p)
    {
        for (int i=0; i<p.rows(); i++)
        {
            this->p[i]=std::abs(p[i]);
        }
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
    bool fixed[4];
    double fixedvalue[4];

    int idr,idl,idca,idcb;
};

/**
 * @class RL
 */
class RL_cplx: public Shape<Eigen::Vector3d>
{
public:

    RL_cplx(double R,double L,bool fixedR,bool fixedL)
    {
        fixed[0]=fixedR;
        fixed[1]=fixedL;

        p.resize(2-(int(fixedR)+int(fixedL)));

        idr=0;
        idl=(fixed[0])?0:1;

        setR(R);
        setL(L);
    }

    static QDialog* createDialog(QDoubleSpinBox*& getR,
                                 QDoubleSpinBox*& getL,
                                 QCheckBox*& fixedR,
                                 QCheckBox*& fixedL)
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        getR=new QDoubleSpinBox(dialog);
        getR->setRange(0.001,1e8);
        getR->setDecimals(4);
        getR->setPrefix("R=");
        getR->setSuffix("Ohm");
        getL=new QDoubleSpinBox(dialog);
        getL->setRange(0.001,1e8);
        getL->setDecimals(4);
        getL->setPrefix("L=");
        getL->setSuffix("mH");
        fixedR=new QCheckBox("fixed?");
        fixedL=new QCheckBox("fixed?");

        gbox->addWidget(getR,0,0);
        gbox->addWidget(getL,1,0);

        gbox->addWidget(fixedR,0,1);
        gbox->addWidget(fixedL,1,1);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,4,0,2,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)const
    {
        Eigen::Vector2d res=at(pt[0]);

//            std::complex<double> Zt(res[0]*cos(res[1]*M_PI/180),res[0]*sin(res[1]*M_PI/180));
//            std::complex<double> Zm(pt[1]*cos(pt[2]*M_PI/180),pt[1]*sin(pt[2]*M_PI/180));
//            return Eigen::Vector3d (Zt.real()-Zm.real(),Zt.imag()-Zm.imag(),0);

        return Eigen::Vector3d (pt[1]-res[0],pt[2]-res[1],0);
    }

    double getR()const
    {
        return (fixed[0])? fixedvalue[0]: p[idr];
    }
    double getL()const
    {
        return (fixed[1])? fixedvalue[1]: p[idl];
    }

    void  setR(double R)
    {
        if (fixed[0])
        {
            fixedvalue[0]=R;
        }
        else
        {
            p[idr]=R;
        }
    }
    void  setL(double L)
    {
        if (fixed[1])
        {
            fixedvalue[1]=L;
        }
        else
        {
            p[idl]=L;
        }
    }

    Eigen::Vector2d at(double f)const
    {
        double w=2*M_PI*f;//

        std::complex<double> R(getR(),0.0);
        std::complex<double> Lp(0.0,w*getL()*1e-3);

        std::complex<double> G=(R+Lp);

        return Eigen::Vector2d(std::abs(G), std::arg(G)*180.0/M_PI );
    }

    void at(QVector<double> f,QVector<double>& g,QVector<double>& p)
    {
        g.resize(f.size());
        p.resize(f.size());

        for (int i=0; i<f.size(); i++)
        {
            Eigen::Vector2d res=at(f[i]);
            g[i]=res[0];
            p[i]=res[1];
        }
    }

    int nb_params()
    {
        return int(p.rows());
    }
    void setParams(const Eigen::VectorXd& p)
    {
        for (int i=0; i<p.rows(); i++)
        {
            this->p[i]=std::abs(p[i]);
        }
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
    bool fixed[2];
    double fixedvalue[2];

    int idr,idl;
};

/////////////////////////////////////////////////////////////////////////////////////
double Ru();
double Rg(double mean,double sigma);
void testShapeFit();
#endif
