#include "FilterDialog.h"

#include <QDialogButtonBox>

FilterDialog::FilterDialog(QString defaultValue,QString varName):QDialog()
{
    setLocale(QLocale("C"));
    setWindowTitle(QString("Filter by : %1").arg(varName));
    gbox = new QGridLayout();

    cb_mode=new QComboBox(this);
    cb_mode->addItem(QIcon(":/img/icons/SortAscending.png"),"Ascending sort");
    cb_mode->addItem(QIcon(":/img/icons/SortDescending.png"),"Descending sort");
    cb_mode->addItem(QIcon(":/img/icons/SortGreaterThan.png"),"Keep greater than value");
    cb_mode->addItem(QIcon(":/img/icons/SortLowerThan.png"),"Keep lower than value");
    cb_mode->addItem(QIcon(":/img/icons/SortEqual.png"),"Keep equal to value");
    cb_mode->addItem(QIcon(":/img/icons/SortNotEqual.png"),"Keep not equal to value");

    sb_value=new QLineEdit(this);
    sb_value->setToolTip("Value");
    sb_value->setText(defaultValue);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(cb_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_modeChanged(int)));


    gbox->addWidget(cb_mode,0,0);
    gbox->addWidget(sb_value,1,0);
    gbox->addWidget(buttonBox,2,0);

    setLayout(gbox);
    setMinimumWidth(300);
    adjustSize();

    slot_modeChanged(0);
}

QString FilterDialog::getValue()
{
    return sb_value->text();
}

int FilterDialog::getMode()
{
    return cb_mode->currentIndex();
}

void FilterDialog::slot_modeChanged(int mode)
{
    if(mode>=2 && mode<=5)// 2345
    {
        //sb_value->setEnabled(true);
        sb_value->setVisible(true);
    }
    else
    {
        //sb_value->setEnabled(false);
        sb_value->setVisible(false);
    }
}
