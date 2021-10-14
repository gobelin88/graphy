#include "CustomExpression.h"

CustomRealExp::CustomRealExp()
{
    parser=new exprtk::parser<double>;
    current_compiled_expression=new exprtk::expression<double>;
    symbolsTable=new exprtk::symbol_table<double>;
    current_compiled_expression->register_symbol_table(*symbolsTable);


}

bool CustomRealExp::setExpression(QString exp)
{
    this->exp=exp;
    registerParams(exp);
    bool ok=parser->compile(exp.toStdString(),*current_compiled_expression);
    return ok;
}

QString CustomRealExp::getExpression()
{
    return exp;
}

void CustomRealExp::registerParams(QString exp)
{
    if(current_compiled_expression)
    {
        delete current_compiled_expression;
        current_compiled_expression=new exprtk::expression<double>;
    }

    symbolsTable->clear();
    symbolsTable->add_variable("x",x);

    QStringList paramsList;
    paramsList<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H";
    int id=0;
    paramsNames.clear();

    for(int i=0;i<paramsList.size();i++)
    {
        if(exp.contains(QRegExp(QString(R"abcd((^|[^A-Za-z0-9])%1(?=\$|[^A-Za-z0-9]))abcd").arg(paramsList[i]))))
        {
            paramsNames.append(paramsList[i]);
            id++;
        }
    }
    p.resize(id);
    p.setZero();

    for(int i=0;i<paramsNames.size();i++)
    {
        symbolsTable->add_variable(paramsNames[i].toStdString(),p[i]);
    }

    current_compiled_expression->register_symbol_table(*symbolsTable);
}

QString CustomRealExp::getError()
{
    return QString::fromStdString(parser->error());
}

double CustomRealExp::at(double x)
{
    this->x=x;
    return current_compiled_expression->value();
}

Eigen::VectorXd CustomRealExp::at(Eigen::VectorXd v)
{
    Eigen::VectorXd y(v.size());

    for (int i=0; i<v.size(); i++)
    {
        y[i]=at(v[i]);
    }
    return y;
}

Eigen::Vector2d CustomRealExp::delta(const Eigen::Vector2d& P)
{
    return Eigen::Vector2d(0,P[1]-at(P[0]));
}

int CustomRealExp::nb_params(){return p.rows();}
void CustomRealExp::setParams(const Eigen::VectorXd& _p)
{
    this->p=_p;
}
const Eigen::VectorXd& CustomRealExp::getParams(){return p;}

const QStringList & CustomRealExp::getParamsNames()
{
    return paramsNames;
}

//-------------------------------------------------------------------
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

CustomRealExp * CustomExpDialog::getCustomRealExpression()
{
    return &customRealExp;
}