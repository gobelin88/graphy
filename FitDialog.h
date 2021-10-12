#ifndef FITDIALOG_H
#define FITDIALOG_H

#include <QString>
#include <QDoubleSpinBox>
#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

#include "ModelCurveInterface.h"


class FitDialog : public QDialog
{
    Q_OBJECT
public:
    FitDialog(QString title);

    void addPixmap(QString pixmapName);
    void addParameter(QString parameterName,
                      double value,
                      double min,
                      double max,
                      int decimals=6,
                      bool fixable=true);

    void setValueOf(QString parameterName,double value);
    double valueOf(QString parameterName);
    bool isFixed(QString parameterName);

    void setModelCurve(ModelCurveInterface * model);

public slots:
    void slot_parameterValueChanged(QString parameterName,double value);
    void slot_parameterFixedChanged(QString parameterName);

signals:
    void sig_modelChanged(ModelCurveInterface * model);

private:
    struct Parameter
    {
        QString name;
        QDoubleSpinBox * sb_value;
        QCheckBox * cb_isFixed;
    };

    QList<Parameter*> parameters;
    QVBoxLayout * vlayout;
    QVBoxLayout * mainlayout;

    ModelCurveInterface * model;
};

#endif // FITDIALOG_H
