#include "MyVariant.h"

MyVariant::MyVariant()
{
    color=qRgb(255,255,255);
}

MyVariant::MyVariant(const QString & value)
{
    *this=value;
    color=qRgb(255,255,255);
}
MyVariant::MyVariant(const std::complex<double> & value)
{
    *this=value;
    color=qRgb(255,255,255);
}
MyVariant::MyVariant(const double value)
{
    *this=value;
    color=qRgb(255,255,255);
}

MyVariant::MyVariant(const QVariant & value)
{
    *dynamic_cast<QVariant*>(this)=value;
    color=qRgb(255,255,255);
}

void MyVariant::operator=(const MyVariant & other)
{
    *dynamic_cast<QVariant*>(this)=other;
    color=other.color;
}

void MyVariant::operator=(const QString & other)
{
    *dynamic_cast<QVariant*>(this)=other;
}

void MyVariant::operator=(const std::complex<double> & other)
{
    if(other.imag()==0)
    {
        *dynamic_cast<QVariant*>(this)=other.real();
    }
    else
    {
        dynamic_cast<QVariant*>(this)->setValue(other);
    }
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

QString MyVariant::doubleToString(double value)const
{
    return QString::number(value,'g',myVariantDoubleToStringPrecision);
}

QString MyVariant::complexToString(std::complex<double> value)const
{
    if (value.imag()==0.0)
    {
        return doubleToString( value.real());
    }
    else
    {
        return doubleToString( value.real())+QString(",")+doubleToString( value.imag());
    }
}

bool MyVariant::complexFromStringRef(const QStringRef & string,std::complex<double> & value)const
{
    QVector<QStringRef> args=string.split(',');
    if(args.size()==2)
    {
        bool okR,okI;
        double valuedoubleR=args[0].toDouble(&okR);
        double valuedoubleI=args[1].toDouble(&okI);

        if(okR && okI)
        {
            value=std::complex<double>(valuedoubleR,valuedoubleI);
        }

        return (okR && okI);
    }
    else if(args.size()==1)
    {
        bool okR;
        double valuedoubleR=args[0].toDouble(&okR);
        value=std::complex<double>(valuedoubleR,0);

        return okR;
    }
    else
    {
        return false;
    }

}

bool MyVariant::complexFromString(const QString & string,std::complex<double> & value)const
{
    QStringList args=string.split(",");
    if(args.size()==2)
    {
        bool okR,okI;
        double valuedoubleR=args[0].toDouble(&okR);
        double valuedoubleI=args[1].toDouble(&okI);

        if(okR && okI)
        {
            value=std::complex<double>(valuedoubleR,valuedoubleI);
        }

        return (okR && okI);
    }
    else if(args.size()==1)
    {
        bool okR;
        double valuedoubleR=args[0].toDouble(&okR);
        value=std::complex<double>(valuedoubleR,0);

        return okR;
    }
    else
    {
        return false;
    }

}

bool MyVariant::complexFromByteArray(const QByteArray & string,std::complex<double> & value)const
{
    QList<QByteArray> args=string.split(',');
    if(args.size()==2)
    {
        bool okR,okI;
        double valuedoubleR=args[0].toDouble(&okR);
        double valuedoubleI=args[1].toDouble(&okI);

        if(okR && okI)
        {
            value=std::complex<double>(valuedoubleR,valuedoubleI);
        }

        return (okR && okI);
    }
    else if(args.size()==1)
    {
        bool okR;
        double valuedoubleR=args[0].toDouble(&okR);
        value=std::complex<double>(valuedoubleR,0);

        return okR;
    }
    else
    {
        return false;
    }

}

QString MyVariant::saveToString()const
{
    if( isDouble() )
    {
        return doubleToString(this->toDouble());
    }
    else if( isComplex() )
    {
        return complexToString(toComplex());
    }
    else
    {
        return this->toString();
    }
}

void MyVariant::loadFromByteArray(const QByteArray & string)
{
    bool canDouble=false;
    double value_double=string.toDouble(&canDouble);
    if(canDouble)
    {
        *this=value_double;
        return ;
    }

    std::complex<double> value_cplx;
    if(complexFromByteArray(string,value_cplx))
    {
        *this=value_cplx;
        return;
    }

    *this=QString(string);
}

void MyVariant::loadFromStringRef(const QStringRef & string)
{
    bool canDouble=false;
    *this=string.toDouble(&canDouble);
    if(canDouble)
    {
        return;
    }

    std::complex<double> value_cplx;
    if(complexFromStringRef(string,value_cplx))
    {
        *this=value_cplx;
        return;
    }

    *this=string.toString();
}

void MyVariant::loadFromString(const QString & string)
{
    bool canDouble=false;
    double value_double=string.toDouble(&canDouble);
    if(canDouble)
    {
        *this=value_double;
        return ;
    }

    std::complex<double> value_cplx;
    if(complexFromString(string,value_cplx))
    {
        *this=value_cplx;
        return;
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
    else
    {
        return std::complex<double>(0.0,0.0);
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

void testComplex(std::complex<double> ci)
{
     MyVariant v;
    std::complex<double> co;

    std::complex<double>(0,1);
    bool ok=v.complexFromString(v.complexToString(ci),co);
    std::cout<<ci<<" "<<co<<" "<<ok<<" ";

    if(ci.real()!=co.real())
    {
        std::cout<<"failed"<<std::endl;
        return;
    }

    if(ci.imag()!=co.imag())
    {
        std::cout<<"failed"<<std::endl;
        return;
    }

    std::cout<<"ok"<<std::endl;
}

void testVariant()
{
    double s=1.0;
    testComplex(std::complex<double>(0,s));
    testComplex(std::complex<double>(0,-s));
    testComplex(std::complex<double>(s,0));
    testComplex(std::complex<double>(-s,0));
    testComplex(std::complex<double>(-s,-s));
    testComplex(std::complex<double>(-s,+s));
    testComplex(std::complex<double>(s,-s));

    s=2.0;
    testComplex(std::complex<double>(0,s));
    testComplex(std::complex<double>(0,-s));
    testComplex(std::complex<double>(s,0));
    testComplex(std::complex<double>(-s,0));
    testComplex(std::complex<double>(-s,-s));
    testComplex(std::complex<double>(-s,+s));
    testComplex(std::complex<double>(s,-s));

    s=1.1234567890;
    testComplex(std::complex<double>(0,s));
    testComplex(std::complex<double>(0,-s));
    testComplex(std::complex<double>(s,0));
    testComplex(std::complex<double>(-s,0));
    testComplex(std::complex<double>(-s,-s));
    testComplex(std::complex<double>(-s,+s));
    testComplex(std::complex<double>(s,-s));

    s=1.1234567890e40;
    testComplex(std::complex<double>(0,s));
    testComplex(std::complex<double>(0,-s));
    testComplex(std::complex<double>(s,0));
    testComplex(std::complex<double>(-s,0));
    testComplex(std::complex<double>(-s,-s));
    testComplex(std::complex<double>(-s,+s));
    testComplex(std::complex<double>(s,-s));
}
