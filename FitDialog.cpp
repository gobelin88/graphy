#include "FitDialog.h"

#include <QDialogButtonBox>
#include <iostream>

FitDialog::FitDialog(QString title)
{
    setLocale(QLocale("C"));
    setWindowTitle(title);

    vlayout=new QVBoxLayout;
    mainlayout=new QVBoxLayout(this);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    mainlayout->addLayout(vlayout);
    mainlayout->addWidget(buttonBox);
    setLayout(mainlayout);
}

void FitDialog::addPixmap(QString pixmapName)
{
    QLabel* label_eqn=new QLabel(this);
    label_eqn->setPixmap(QPixmap(pixmapName));
    label_eqn->setAlignment(Qt::AlignHCenter);

    vlayout->addWidget(label_eqn);
}

void FitDialog::addParameter(QString parameterName,
                             double value,
                             double min,
                             double max,
                             int decimals,
                             bool fixable)
{
    Parameter * p=new Parameter;
    p->name=parameterName;
    p->sb_value=new QDoubleSpinBox(this);
    p->sb_value->setRange(min,max);
    p->sb_value->setPrefix(QString("%1=").arg(parameterName));
    p->sb_value->setDecimals(decimals);
    p->sb_value->setValue(value);

    if(fixable)
    {
        p->cb_isFixed=new QCheckBox(this);
        p->cb_isFixed->setChecked(false);
        p->cb_isFixed->setText("Fixed");
        connect(p->cb_isFixed,&QCheckBox::stateChanged,this,[this,p]{slot_parameterFixedChanged(p->name);});
    }

    parameters.append(p);

    connect(p->sb_value,static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),this,[this,p](double value){slot_parameterValueChanged(p->name,value);});

    QHBoxLayout * playout=new QHBoxLayout(this);
    playout->addWidget(p->sb_value);
    playout->addWidget(p->cb_isFixed);

    vlayout->addLayout(playout);
}

void FitDialog::setValueOf(QString parameterName,double value)
{
    for(int i=0;i<parameters.size();i++)
    {
        if(parameters[i]->name==parameterName)
        {
            return parameters[i]->sb_value->setValue(value);
        }
    }
    std::cout<<"Bad parameter:"<<parameterName.toStdString()<<std::endl;
}

double FitDialog::valueOf(QString parameterName)
{
    for(int i=0;i<parameters.size();i++)
    {
        if(parameters[i]->name==parameterName)
        {
            return parameters[i]->sb_value->value();
        }
    }
    std::cout<<"Bad parameter:"<<parameterName.toStdString()<<std::endl;
    return 0.0;
}

bool FitDialog::isFixed(QString parameterName)
{
    for(int i=0;i<parameters.size();i++)
    {
        if(parameters[i]->name==parameterName)
        {
            return parameters[i]->cb_isFixed->isChecked();
        }
    }
    std::cout<<"Bad parameter:"<<parameterName.toStdString()<<std::endl;
    return false;
}

void FitDialog::slot_parameterValueChanged(QString parameterName,double value)
{
    if(model)
    {
        std::cout<<"slot_parameterValueChanged : "<<parameterName.toStdString()<<std::endl;
        //affect all parameters values to the model
        for(int i=0;i<parameters.size();i++)
        {
            if(parameters[i]->name==parameterName)
            {
                model->setParameter(parameters[i]->name,value);
            }
        }
        emit sig_modelChanged(model);
    }
}

void FitDialog::slot_parameterFixedChanged(QString parameterName)
{
    if(model)
    {
        //affect all parameters values to the model
        for(int i=0;i<parameters.size();i++)
        {
            if(parameters[i]->name==parameterName)
            {
                model->setFixedParameter(parameters[i]->name,parameters[i]->cb_isFixed->isChecked());
            }
        }
    }
}

void FitDialog::setModelCurve(ModelCurveInterface * model)
{
    this->model=model;
}
