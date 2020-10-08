#include <QString>
#include <QVariant>
#include <iostream>
#include <Eigen/Dense>
#include <QRgb>

#ifndef VALUE_H
#define VALUE_H

class ValueContainer
{
public:
    ValueContainer();
    void operator=(const ValueContainer & other);
    void operator=(const QVariant & other);
    void operator=(const QString & other);

    QString toString()const;

    double num;
    QString str;
    bool isDouble;
    QRgb background;
};

std::ostream& operator<< (std::ostream &out,const ValueContainer & value);
double log10(const ValueContainer & value);

using MatrixXv=Eigen::Matrix<ValueContainer,Eigen::Dynamic,Eigen::Dynamic>;
using VectorXv=Eigen::Matrix<ValueContainer,Eigen::Dynamic,1>;
using VectorXs=Eigen::Matrix<QString,Eigen::Dynamic,1>;

#endif // VALUE_H
