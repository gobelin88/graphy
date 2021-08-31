#pragma once
#include "shapes/Shape.hpp"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

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
        dialog->setWindowTitle("R+L+C : Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        QLabel* label_eqn=new QLabel(dialog);
        label_eqn->setPixmap(QPixmap(":/imp/imp/(R+L)-C.png"));
        label_eqn->setAlignment(Qt::AlignHCenter);

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

        gbox->addWidget(label_eqn,0,2,4,1);
        gbox->addWidget(buttonBox,4,0,2,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    double getFc()const
    {
        return 1.0/(2*M_PI*std::sqrt(getL()*getC()*1e-12));
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)
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

    void at(Eigen::VectorXd f,Eigen::VectorXd& g,Eigen::VectorXd& p)
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
        dialog->setWindowTitle("(Ra+L)|C|Rb : Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        QLabel* label_eqn=new QLabel(dialog);
        label_eqn->setPixmap(QPixmap(":/imp/imp/(Ra+L)-C-Rb.png"));
        label_eqn->setAlignment(Qt::AlignHCenter);

        getRa=new QDoubleSpinBox(dialog);
        getRa->setRange(0.001,1e8);
        getRa->setDecimals(4);
        getRa->setPrefix("Ra=");
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
        getRb->setPrefix("Rb=");
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

        gbox->addWidget(label_eqn,0,2,5,1);
        gbox->addWidget(buttonBox,5,0,3,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)
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

    void at(Eigen::VectorXd f,Eigen::VectorXd& g,Eigen::VectorXd& phi)
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
        dialog->setWindowTitle("(R+L)|Ca+Cb : Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        QLabel* label_eqn=new QLabel(dialog);
        label_eqn->setPixmap(QPixmap(":/imp/imp/(R+L)-Ca+Cb.png"));
        label_eqn->setAlignment(Qt::AlignHCenter);

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

        gbox->addWidget(label_eqn,0,2,5,1);
        gbox->addWidget(buttonBox,5,0,2,1);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)
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

    void at(Eigen::VectorXd f,Eigen::VectorXd& g,Eigen::VectorXd& phi)
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
        dialog->setWindowTitle("R+L : Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        QLabel* label_eqn=new QLabel(dialog);
        label_eqn->setPixmap(QPixmap(":/imp/imp/R+L.png"));
        label_eqn->setAlignment(Qt::AlignHCenter);

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

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(label_eqn,0,0,1,2);
        gbox->addWidget(getR,1,0);
        gbox->addWidget(getL,2,0);
        gbox->addWidget(fixedR,1,1);
        gbox->addWidget(fixedL,2,1);
        gbox->addWidget(buttonBox,3,0,1,2);


        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)
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

    void at(Eigen::VectorXd f,Eigen::VectorXd& g,Eigen::VectorXd& p)
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

        QLabel* label_eqn=new QLabel(dialog);
        label_eqn->setPixmap(QPixmap(":/imp/imp/R+L+C.png"));
        label_eqn->setAlignment(Qt::AlignHCenter);

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


        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(label_eqn,0,0,1,2);
        gbox->addWidget(getR,1,0);
        gbox->addWidget(getL,2,0);
        gbox->addWidget(getC,3,0);
        gbox->addWidget(fixedR,1,1);
        gbox->addWidget(fixedL,2,1);
        gbox->addWidget(fixedC,3,1);
        gbox->addWidget(buttonBox,4,0,1,2);

        dialog->setLayout(gbox);

        return dialog;
    }

    double getFc()const
    {
        return 1.0/(2*M_PI*std::sqrt(getL()*getC()*1e-12));
    }

    Eigen::Vector3d delta(const Eigen::Vector3d& pt)
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

    void at(Eigen::VectorXd f,Eigen::VectorXd& g,Eigen::VectorXd& p)
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

