#include <QString>
#include <QVector>
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

    bool isDouble() const;
    bool isString() const;
    bool isComplex() const;

    std::complex<double> toComplex()const ;

    QRgb background;
};

std::ostream& operator<< (std::ostream &out,const MyVariant & value);
double log10(const MyVariant & v);

using MatrixXv=Eigen::Matrix<MyVariant,Eigen::Dynamic,Eigen::Dynamic>;
using VectorXv=Eigen::Matrix<MyVariant,Eigen::Dynamic,1>;
using VectorXs=Eigen::Matrix<QString,Eigen::Dynamic,1>;

Eigen::VectorXd toDouble(const VectorXv & v);
QVector<QString> toString(const VectorXv & v);
Eigen::VectorXcd toComplex(const VectorXv & v);

VectorXv fromDouble(const Eigen::VectorXd & v);
VectorXv fromString(const QVector<QString> & v);
VectorXv fromComplex(const Eigen::VectorXcd & v);

#endif // VALUE_H
