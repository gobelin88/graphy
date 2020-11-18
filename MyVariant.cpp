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

void MyVariant::operator=(const std::complex<double> & other)
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
    else if (value.real()==0.0)
    {
        if(value.imag()==1)
        {
            return QString("i");
        }
        else if(value.imag()==-1)
        {
            return QString("-i");
        }
        else
        {
            return doubleToString(value.imag())+QString("i");
        }
    }
    else
    {
        if(value.imag()==1)
        {
            return doubleToString( value.real() )+QString("+i");
        }
        else if(value.imag()==-1)
        {
            return doubleToString( value.real() )+QString("-i");
        }
        else if(value.imag()>0)
        {
            return doubleToString( value.real() )+QString("+")+doubleToString(value.imag())+QString("i");
        }
        else
        {
            return doubleToString( value.real() )+doubleToString(value.imag())+QString("i");
        }
    }
}

bool MyVariant::complexFromString(QString string,std::complex<double> & value)const
{
    if(string.endsWith("i"))
    {
        if(string==QString("i"))//i
        {
            value=std::complex<double>(0,1);
            return true;
        }

        if(string==QString("-i"))//-i
        {
            value=std::complex<double>(0,-1);
            return true;
        }

        //Remove the i
        string.chop(1);

        bool isPureImaginary;
        double imaginaryPart=string.toDouble(&isPureImaginary);
        if(isPureImaginary)//Yi
        {
            value=std::complex<double>(0,imaginaryPart);
        }
        else//A+Yi
        {
            bool realneg=false,imagneg=false;
            if(string.startsWith("-"))
            {
                realneg=true;
                string.remove(0,1);
            }

            QStringList args;
            if(string.contains("-"))
            {
                args=string.split("-");
                imagneg=true;
            }
            else if(string.contains("+"))
            {
                args=string.split("+");
                imagneg=false;
            }

            if(args.size()==2)
            {
                if(args[1].isEmpty())
                {
                    value=std::complex<double>( (realneg)?-args[0].toDouble():args[0].toDouble(),
                                                (imagneg)?-1:1);
                }
                else
                {
                    value=std::complex<double>( (realneg)?-args[0].toDouble():args[0].toDouble(),
                                                (imagneg)?-args[1].toDouble():args[1].toDouble());
                }
                return true;
            }
            else
            {
//                std::cout<<string.toStdString()<<std::endl;
//                std::cout<<argsP.size()<<" "<<argsN.size()<<std::endl;
//                std::cout<<argsP[0].toStdString()<<" "<<argsN[0].toStdString()<<std::endl;
                return false;
            }
        }
    }
    else
    {
        bool ok;
        double valuedouble=string.toDouble(&ok);

        if(ok)
        {
            value=std::complex<double>(valuedouble,0);
            return true;
        }
        else
        {
            return false;
        }

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

void MyVariant::loadFromString(QString string)
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
    testComplex(std::complex<double>(0,1));
    testComplex(std::complex<double>(0,-1));
    testComplex(std::complex<double>(1,0));
    testComplex(std::complex<double>(-1,0));
    testComplex(std::complex<double>(-1,-1));
    testComplex(std::complex<double>(-1,+1));
    testComplex(std::complex<double>(1,-1));

    testComplex(std::complex<double>(0,2));
    testComplex(std::complex<double>(0,-2));
    testComplex(std::complex<double>(2,0));
    testComplex(std::complex<double>(-2,0));
    testComplex(std::complex<double>(-2,-2));
    testComplex(std::complex<double>(-2,+2));
    testComplex(std::complex<double>(2,-2));
}
