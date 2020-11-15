#include <QString>
#include <QVariant>
#include <iostream>
#include <Eigen/Dense>
#include <QRgb>

#ifndef VALUE_H
#define VALUE_H

Q_DECLARE_METATYPE(std::complex<double>);

class MyVariant:public QVariant
{
public:
    MyVariant();
    MyVariant(const QVariant & value);

    void operator=(const QString & other);
    void operator=(const std::complex<double> other);
    void operator=(double other);

    void operator=(const MyVariant & other);

    QString saveToString()const;
    void loadFromString(QString string);


    QRgb background;
};

std::ostream& operator<< (std::ostream &out,const MyVariant & value);
double log10(const MyVariant & v);

using MatrixXv=Eigen::Matrix<MyVariant,Eigen::Dynamic,Eigen::Dynamic>;
using VectorXv=Eigen::Matrix<MyVariant,Eigen::Dynamic,1>;
using VectorXs=Eigen::Matrix<QString,Eigen::Dynamic,1>;

#endif // VALUE_H
