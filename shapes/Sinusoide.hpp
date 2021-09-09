#pragma once

#include "shapes/Shape.hpp"

#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

class Sinusoide: public Shape<Eigen::Vector2d>
{
public:

    Sinusoide(double A,double F,double P,double C,
              bool fixedA,bool fixedF,bool fixedP,bool fixedC
              )
    {
        this->fixedA=fixedA;
        this->fixedF=fixedF;
        this->fixedP=fixedP;
        this->fixedC=fixedC;

        int N=0;
        if(!fixedA)
        {
            idA=N;
            N++;
        }
        if(!fixedF)
        {
            idF=N;
            N++;
        }
        if(!fixedP)
        {
            idP=N;
            N++;
        }
        if(!fixedC)
        {
            idC=N;
            N++;
        }

        std::cout<<"Sinusoide N="<<N<<std::endl;
        std::cout<<"idA="<<idA<<std::endl;
        std::cout<<"idF="<<idF<<std::endl;
        std::cout<<"idP="<<idP<<std::endl;
        std::cout<<"idC="<<idC<<std::endl;

        p.resize(N);
        p_fixed.resize(4);

        setA(A);
        setF(F);
        setP(P);
        setC(C);
    }

    static QDialog* createDialog(QDoubleSpinBox*& getA,
                                 QDoubleSpinBox*& getF,
                                 QDoubleSpinBox*& getP,
                                 QDoubleSpinBox*& getC,
                                 QCheckBox *& fixedA,
                                 QCheckBox *& fixedF,
                                 QCheckBox *& fixedP,
                                 QCheckBox *& fixedC
                                 )
    {
        QDialog* dialog=new QDialog;
        dialog->setLocale(QLocale("C"));
        dialog->setWindowTitle("Initials parameters");
        QGridLayout* gbox = new QGridLayout();

        QLabel* label_eqn=new QLabel(dialog);
        label_eqn->setPixmap(QPixmap(":/eqn/eqn/sin.gif"));
        label_eqn->setAlignment(Qt::AlignHCenter);

        getA=new QDoubleSpinBox(dialog);
        getA->setRange(0.0001,1e8);
        getA->setDecimals(6);
        getA->setPrefix("A=");
        getF=new QDoubleSpinBox(dialog);
        getF->setRange(0,1e8);
        getF->setDecimals(6);
        getF->setPrefix("F=");
        getP=new QDoubleSpinBox(dialog);
        getP->setRange(-1e8,1e8);
        getP->setDecimals(6);
        getP->setPrefix("Phi=");
        getP->setValue(0.0);
        getC=new QDoubleSpinBox(dialog);
        getC->setRange(-1e8,1e8);
        getC->setDecimals(6);
        getC->setPrefix("C=");
        getC->setValue(0.0);

        fixedA=new QCheckBox(dialog);
        fixedF=new QCheckBox(dialog);
        fixedP=new QCheckBox(dialog);
        fixedC=new QCheckBox(dialog);
        fixedA->setChecked(false);
        fixedF->setChecked(false);
        fixedP->setChecked(false);
        fixedC->setChecked(false);
        fixedA->setText("Fixed?");
        fixedF->setText("Fixed?");
        fixedP->setText("Fixed?");
        fixedC->setText("Fixed?");


        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                           | QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

        gbox->addWidget(label_eqn,0,0);
        gbox->addWidget(getA,1,0);
        gbox->addWidget(getF,2,0);
        gbox->addWidget(getP,3,0);
        gbox->addWidget(getC,4,0);
        gbox->addWidget(fixedA,1,1);
        gbox->addWidget(fixedF,2,1);
        gbox->addWidget(fixedP,3,1);
        gbox->addWidget(fixedC,4,1);
        gbox->addWidget(buttonBox,5,0);

        dialog->setLayout(gbox);

        return dialog;
    }

    Eigen::Vector2d delta(const Eigen::Vector2d& pt)
    {
        return Eigen::Vector2d (pt[1]-at(pt[0]),0);
    }

    double getA()const
    {
        return (fixedA)?p_fixed[0]: p[idA];
    }
    double getF()const
    {
        return (fixedF)?p_fixed[1]: p[idF];
    }
    double getP()const
    {
        return (fixedP)?p_fixed[2]: p[idP];
    }
    double getC()const
    {
        return (fixedC)?p_fixed[3]: p[idC];
    }

    void  setA(double A)
    {
        if(fixedA){p_fixed[0]=A;}else{p[idA]=A;}
    }
    void  setF(double F)
    {
        if(fixedF){p_fixed[1]=F;}else{p[idF]=F;}
    }
    void  setP(double P)
    {
        if(fixedP){p_fixed[2]=P;}else{p[idP]=P;}
    }
    void  setC(double C)
    {
        if(fixedC){p_fixed[3]=C;}else{p[idC]=C;}
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
        if(P<0){P+=2*M_PI;}

        setA(A);
        setF(F);
        setP(P);
    }

    double at(double t)const
    {
        double A=getA();
        double F=getF();
        double P=getP();
        double C=getC();
        //std::cout<<t<<" "<<A<<" "<<F<<" "<<P<<" "<<A* std::sin(2*M_PI*F*t+P)<<std::endl;
        return C+A*std::sin(2*M_PI*F*t+P);
    }

    Eigen::VectorXd at(Eigen::VectorXd f)
    {
        Eigen::VectorXd y(f.size());

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
    void setParams(const Eigen::VectorXd& _p)
    {
        this->p=_p;
    }
    const Eigen::VectorXd& getParams()
    {
        return p;
    }

private:
    Eigen::VectorXd p;
    Eigen::VectorXd p_fixed;
    int idA,idF,idP,idC;
    bool fixedA,fixedF,fixedP,fixedC;
};
