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

    virtual T delta(const T & p)const=0;
    virtual int nb_params()=0;

    virtual void setParams(const Eigen::VectorXd & p)=0;
    virtual const Eigen::VectorXd & getParams()=0;

    int operator()(const Eigen::VectorXd &p, Eigen::VectorXd &h)
    {
        setParams(p);

        T dn;
        int n=dn.rows();

        for (int i=0; i<values()/n; i++)
        {
            T d=delta(points[i]);

            for(int k=0;k<n;k++)
            {
                h[n*i+k]=d[k];
            }
        }

        return 0;
    }

    int df(const Eigen::VectorXd &p, Eigen::MatrixXd & J)
    {
        for(int j=0;j< inputs();j++)
        {
            Eigen::VectorXd dp=Eigen::VectorXd::Zero(inputs());
            dp[j]=EPS;
            Eigen::VectorXd h_df_p(values());this->operator ()(p+dp,h_df_p);
            Eigen::VectorXd h_df_m(values());this->operator ()(p-dp,h_df_m);

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
        for(unsigned int i=0;i< points.size();i++)
        {
            T delta=this->delta(points[i]);
            rms+=delta.squaredNorm();
        }
        return rms;
    }

    std::vector<T> getErr()
    {
        std::vector<Eigen::Vector3d> err_dist(points.size());
        for(unsigned int i=0;i< points.size();i++)
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
    inline Eigen::Vector3d getCenter()const{return Eigen::Vector3d(p[0],p[1],p[2]);}
    inline double getRadius()const{return p[3];}

    Eigen::Vector3d delta(const Eigen::Vector3d & pt)const;
    int nb_params();
    void setParams(const Eigen::VectorXd & p);
    const Eigen::VectorXd & getParams();

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
    inline Eigen::Vector3d getNormal()const{return Eigen::Vector3d(sin(p[0])*sin(p[1]),cos(p[1]),cos(p[0])*sin(p[1]));}
    inline double getOffset()const{return p[2];}

    Eigen::Vector3d delta(const Eigen::Vector3d & pt)const;
    int nb_params();
    void setParams(const Eigen::VectorXd & p);
    const Eigen::VectorXd & getParams();

private:
    Eigen::VectorXd p;
};

/**
 * @class Circle
 * @brief Décrit un cercle.
 */
class Circle: public Shape<Eigen::Vector3d>
{
public:
    Circle();
    Circle(Eigen::Vector3d center,double radius);
    inline Eigen::Vector3d getNormal()const{return Eigen::Vector3d(sin(p[0])*sin(p[1]),cos(p[1]),cos(p[0])*sin(p[1]));}
    inline Eigen::Vector3d getCenter()const{return Eigen::Vector3d(p[2],p[3],p[4]);}
    inline double getRadius()const{return p[5];}

    Eigen::Vector3d delta(const Eigen::Vector3d & pt)const;
    int nb_params();
    void setParams(const Eigen::VectorXd & p);
    const Eigen::VectorXd & getParams();

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
        setA(A);setF(F);setP(P);
    }

    static QDialog * createDialog(QDoubleSpinBox *& getA,
                                  QDoubleSpinBox *& getF,
                                  QDoubleSpinBox *& getP)
    {
        QDialog * dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout * gbox = new QGridLayout();

        getA=new QDoubleSpinBox(dialog);getA->setRange(0.0001,1e8);getA->setDecimals(4);
        getA->setPrefix("A=");
        getF=new QDoubleSpinBox(dialog);getF->setRange(0.0001,1e8);getF->setDecimals(4);
        getF->setPrefix("F=");
        getP=new QDoubleSpinBox(dialog);getP->setRange(0.0001,1e8);getP->setDecimals(4);
        getP->setPrefix("Phi=");
        getP->setValue(0.0);

        gbox->addWidget(getA,0,0);
        gbox->addWidget(getF,1,0);
        gbox->addWidget(getP,2,0);

        QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                            | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,3,0);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector2d delta(const Eigen::Vector2d & pt)const
    {
        return Eigen::Vector2d (pt[1]-at(pt[0]),0);
    }

    double getA()const{return p[0];}
    double getF()const{return p[1];}
    double getP()const{return p[2];}

    void  setA(double A){p[0]=A;}
    void  setF(double F){p[1]=F;}
    void  setP(double P){p[2]=P;}

    void regularized()
    {
        double A=getA();
        double F=getF();
        double P=getP();

        if(A<0){A=-A;P+=M_PI;}
        if(F<0){F=-F;P+=M_PI;}
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
        std::cout<<t<<" "<<A<<" "<<F<<" "<<P<<" "<<A*std::sin(2*M_PI*F*t+P)<<std::endl;
        return A*std::sin(2*M_PI*F*t+P);
    }

    QVector<double> at(QVector<double> f)
    {
        QVector<double> y(f.size());

        for(int i=0;i<f.size();i++)
        {
            y[i]=at(f[i]);
        }
        return y;
    }

    int nb_params(){return 3;}
    void setParams(const Eigen::VectorXd & p)
    {
        this->p=p;
    }
    const Eigen::VectorXd & getParams(){return p;}

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
        setS(S);setM(M);setK(K);
    }

    static QDialog * createDialog(QDoubleSpinBox *& getS,
                                  QDoubleSpinBox *& getM,
                                  QDoubleSpinBox *& getK)
    {
        QDialog * dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout * gbox = new QGridLayout();

        getS=new QDoubleSpinBox(dialog);getS->setRange(0.001,1e8);
        getS->setPrefix("Sigma=");
        getM=new QDoubleSpinBox(dialog);getM->setRange(0.001,1e8);
        getM->setPrefix("Mean=");
        getK=new QDoubleSpinBox(dialog);getK->setRange(0.001,1e8);
        getK->setPrefix("K=");
        getK->setValue(1.0);

        gbox->addWidget(getS,0,0);
        gbox->addWidget(getM,1,0);
        gbox->addWidget(getK,2,0);

        QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                            | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,3,0);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector2d delta(const Eigen::Vector2d & pt)const
    {
        return Eigen::Vector2d (pt[1]-at(pt[0]),pt[1]-at(pt[0]));
    }

    double getS()const{return p[0];}
    double getM()const{return p[1];}
    double getK()const{return p[2];}

    void  setS(double R){p[0]=R;}
    void  setM(double L){p[1]=L;}
    void  setK(double K){p[2]=K;}

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

        for(int i=0;i<f.size();i++)
        {
            y[i]=at(f[i]);
        }
        return y;
    }

    int nb_params(){return 3;}
    void setParams(const Eigen::VectorXd & p)
    {
        this->p=p;
    }
    const Eigen::VectorXd & getParams(){return p;}

private:
    Eigen::VectorXd p;
};

/**
 * @class RLC
 */
class RLC: public Shape<Eigen::Vector2d>
{
public:

    RLC(double R,double L,double C,double K,bool fixedR,bool fixedL,bool fixedC,bool fixedK)
    {
        p.resize(4);
        setR(R);setL(L);setC(C);setK(K);
        fixed[0]=fixedR;
        fixed[1]=fixedL;
        fixed[2]=fixedC;
        fixed[3]=fixedK;
    }

    static QDialog * createDialog(QDoubleSpinBox *& getR,
                                  QDoubleSpinBox *& getL,
                                  QDoubleSpinBox *& getC,
                                  QDoubleSpinBox *& getK,
                                  QCheckBox *& fixedR,
                                  QCheckBox *& fixedL,
                                  QCheckBox *& fixedC,
                                  QCheckBox *& fixedK)
    {
        QDialog * dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout * gbox = new QGridLayout();

        getR=new QDoubleSpinBox(dialog);getR->setRange(0.001,1e8);
        getR->setPrefix("R=");getR->setSuffix("Ohm");
        getL=new QDoubleSpinBox(dialog);getL->setRange(0.001,1e8);
        getL->setPrefix("L=");getL->setSuffix("mH");
        getC=new QDoubleSpinBox(dialog);getC->setRange(0.001,1e8);
        getC->setPrefix("C=");getC->setSuffix("nC");
        getK=new QDoubleSpinBox(dialog);getK->setRange(0.001,1e8);
        getK->setPrefix("K=");getK->setSuffix("");
        getK->setValue(1.0);
        fixedR=new QCheckBox("fixed?");
        fixedL=new QCheckBox("fixed?");
        fixedC=new QCheckBox("fixed?");
        fixedK=new QCheckBox("fixed?");
        fixedK->setChecked(true);

        gbox->addWidget(getR,0,0);
        gbox->addWidget(getL,1,0);
        gbox->addWidget(getC,2,0);
        gbox->addWidget(getK,3,0);

        gbox->addWidget(fixedR,0,1);
        gbox->addWidget(fixedL,1,1);
        gbox->addWidget(fixedC,2,1);
        gbox->addWidget(fixedK,3,1);

        QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                            | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(buttonBox,4,0,2,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    double getFc()const{return 1.0/(2*M_PI*std::sqrt(getL()*getC()*1e-12));}

    Eigen::Vector2d delta(const Eigen::Vector2d & pt)const
    {
        return Eigen::Vector2d (pt[1]-at(pt[0]),pt[1]-at(pt[0]));
    }

    double getR()const{return p[0];}
    double getL()const{return p[1];}
    double getC()const{return p[2];}
    double getK()const{return p[3];}

    void  setR(double R){p[0]=R;}
    void  setL(double L){p[1]=L;}
    void  setC(double C){p[2]=C;}
    void  setK(double K){p[3]=K;}

    double at(double f)const
    {
        double w=2*M_PI*f;//
        double R=getR();
        double L=getL()*1e-3;
        double C=getC()*1e-9;
        double I=(L*w-1.0/(C*w));
        return p[3]/std::sqrt(R*R+I*I);
    }

    QVector<double> at(QVector<double> f)
    {
        QVector<double> y(f.size());

        for(int i=0;i<f.size();i++)
        {
            y[i]=at(f[i]);
        }
        return y;
    }

    int nb_params(){return 4;}
    void setParams(const Eigen::VectorXd & p)
    {
        if(fixed[0]==false)this->p[0]=p[0];
        if(fixed[1]==false)this->p[1]=p[1];
        if(fixed[2]==false)this->p[2]=p[2];
        if(fixed[3]==false)this->p[3]=p[3];
    }
    const Eigen::VectorXd & getParams(){return p;}

private:
    Eigen::VectorXd p;
    bool fixed[4];
};


/////////////////////////////////////////////////////////////////////////////////////
double Ru();
double Rg(double mean,double sigma);
void testShapeFit();
#endif
