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
    if( v.canConvert<double>() )
    {
        out<<v.toDouble();
    }
    else if( v.canConvert< std::complex<double> >() )
    {
        out<<v.value<std::complex<double>>();
    }
    else if( v.canConvert< QString >() )
    {
        out<<v.toString().data();
    }
    return out;
}

QString MyVariant::saveToString()const
{
    if( canConvert<double>() )
    {
        return QString::number(this->toDouble());
    }
    else if( canConvert< std::complex<double> >() )
    {
        std::complex<double> value= this->value<std::complex<double>>();
        return QString("(%1,%2)").arg( value.real() ).arg( value.imag() );
    }
    else if( canConvert< QString >() )
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
    if( v.canConvert<double>() )
    {
        return std::log10(v.toDouble());
    }
    else if( v.canConvert< std::complex<double> >() )
    {
        return std::log10( std::abs(v.value<std::complex<double>>()) );
    }
    else
    {
        0.0;
    }
}
