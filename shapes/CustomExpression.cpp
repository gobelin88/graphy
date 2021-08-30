#include "CustomExpression.h"

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