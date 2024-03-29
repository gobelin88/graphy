#include <QString>
#include <QVector>
#include <QVariant>
#include <iostream>
#include <Eigen/Dense>
#include <QRgb>

#ifndef VALUE_H
#define VALUE_H

Q_DECLARE_METATYPE(std::complex<double>);

const unsigned int myVariantDoubleToStringPrecision=15;//presque 16

class MyVariant:public QVariant
{
public:
    MyVariant();

    MyVariant(const QString & value);
    MyVariant(const std::complex<double> & value);
    MyVariant(const double value);
    MyVariant(const QVariant & value);

    bool operator>(const MyVariant & other)const;
    bool operator<(const MyVariant & other)const;
    bool operator==(const MyVariant & other)const;

    void operator=(const QString & other);
    void operator=(const std::complex<double> & other);
    void operator=(double other);
    void operator=(const MyVariant & other);

    void saveToString(QString & str)const;
    QString saveToString()const;
    QByteArray saveToByteArray()const;
    void loadFromStringRef(const QStringRef & string);
    void loadFromString(const QString &string);
    void loadFromByteArray(const QByteArray & string);

    bool isDouble() const;
    bool isString() const;
    bool isComplex() const;

    std::complex<double> toComplex()const ;


    //
    QByteArray doubleToByteArray(double value) const;
    QString doubleToString(double value) const;
    void doubleToString(double value,QString & str) const;
    QByteArray complexToByteArray(std::complex<double> value)const;
    QString complexToString(std::complex<double> value)const;
    void complexToString(std::complex<double> value,QString & str)const;

    bool complexFromStringRef(const QStringRef & string,std::complex<double> & value)const;
    bool complexFromString(const QString & string,std::complex<double> & value)const;
    bool complexFromByteArray(const QByteArray &string, std::complex<double> & value)const;

    QRgb color;
};

void testVariant();

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
