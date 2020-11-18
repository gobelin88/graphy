#include "MyVariant.h"

MyVariant::MyVariant()
{
    *this=QVariant("");
    background=qRgb(255,255,255);
}

MyVariant::MyVariant(const QVariant & value)
{
    *dynamic_cast<QVariant*>(this)=value;
    background=qRgb(255,255,255);
}

void MyVariant::operator=(const MyVariant & other)
{
    *dynamic_cast<QVariant*>(this)=other;
    background=other.background;
}

void MyVariant::operator=(const QString & other)
{
    *dynamic_cast<QVariant*>(this)=other;
}

void MyVariant::operator=(const std::complex<double> other)
{
    dynamic_cast<QVariant*>(this)->setValue(other);
}

void MyVariant::operator=(double other)
{
    *dynamic_cast<QVariant*>(this)=other;
}

std::ostream& operator<< (std::ostream &out,const MyVariant & v)
{
    //std::cout<<v.toString().toLocal8Bit().data()<<" "<<v.canConvert<double>()<<std::endl;
    if( v.isDouble() )
    {
        out<<v.toDouble()<<"(d)";
    }
    else if( v.isComplex() )
    {
        out<<v.toComplex()<<"(c)";
    }
    else if( v.isString() )
    {
        QString str=v.toString();
        out<<str.toStdString()<<"(s)";
    }
    return out;
}

QString MyVariant::saveToString()const
{
    if( isDouble() )
    {
        return QString::number(this->toDouble());
    }
    else if( isComplex() )
    {
        std::complex<double> value= toComplex();
        return QString("(%1,%2)").arg( value.real() ).arg( value.imag() );
    }
    else
    {
        return this->toString();
    }
}

void MyVariant::loadFromString(QString string)
{
    bool canDouble=false;
    double value_double=string.toDouble(&canDouble);
    if(canDouble)
    {
        *this=value_double;
        return ;
    }

    QStringList args=string.split(",");
    bool canComplex=string.startsWith("(") && string.endsWith(")") && args.size()==2;
    if(canComplex)
    {
        args[0].remove(0,1);
        args[1].chop(1);
        *this=std::complex<double>(args[0].toDouble(),args[1].toDouble());
        return ;
    }

    *this=string;
}

double log10(const MyVariant &v)
{
    if( v.isDouble() )
    {
        return std::log10(v.toDouble());
    }
    else if( v.isComplex() )
    {
        return std::log10( std::abs(v.toComplex()) );
    }
    else
    {
        return 0.0;
    }
}

bool MyVariant::isDouble() const
{
    return type()==QVariant::Type::Double;
}

bool MyVariant::isString() const
{
    return type()==QVariant::Type::String;
}

bool MyVariant::isComplex() const
{
    return this->canConvert<std::complex<double>>();
}

std::complex<double> MyVariant::toComplex() const
{
    if(isComplex())
    {
        return this->value<std::complex<double> >();
    }
    else if(isDouble())
    {
        return std::complex<double>(toDouble(),0.0);
    }
}
/////////////////////////

Eigen::VectorXd toDouble(const VectorXv & v)
{
    Eigen::VectorXd vo(v.rows());
    for(int i=0;i<v.rows();i++)
    {
        vo[i]=v[i].toDouble();
    }
    return vo;
}
QVector<QString> toString(const VectorXv & v)
{
    QVector<QString> vo(v.rows());
    for(int i=0;i<v.rows();i++)
    {
        vo[i]=v[i].toString();
    }
    return vo;
}
Eigen::VectorXcd toComplex(const VectorXv & v)
{
    Eigen::VectorXcd vo(v.rows());
    for(int i=0;i<v.rows();i++)
    {
        vo[i]=v[i].toComplex();
    }
    return vo;
}

VectorXv fromDouble(const Eigen::VectorXd & v)
{
    VectorXv vo(v.rows());
    for(int i=0;i<v.rows();i++)
    {
        vo[i]=v[i];
    }
    return vo;
}
VectorXv fromString(const QVector<QString> & v)
{
    VectorXv vo(v.size());
    for(int i=0;i<v.size();i++)
    {
        vo[i]=v[i];
    }
    return vo;
}
VectorXv fromComplex(const Eigen::VectorXcd & v)
{
    VectorXv vo(v.rows());
    for(int i=0;i<v.rows();i++)
    {
        vo[i]=v[i];
    }
    return vo;
}
