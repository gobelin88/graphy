#include "MyValueContainer.h"

MyValueContainer::MyValueContainer()
{
    num=0.0;
    str.clear();
    isDouble=false;
    background=qRgb(255,255,255);
}

void MyValueContainer::operator=(const MyValueContainer & other)
{
    isDouble=other.isDouble;

    if(isDouble)
    {
        num=other.num;
    }
    else
    {
        str=other.str;
    }
}

void MyValueContainer::operator=(const QVariant & other)
{
    num=other.toDouble(&isDouble);

    if(!isDouble)
    {
        str=other.toString();
    }
}

void MyValueContainer::operator=(const QString & other)
{
    num=other.toDouble(&isDouble);

    if(!isDouble)
    {
        str=other;
    }
}

std::ostream& operator<< (std::ostream &out,const MyValueContainer & value)
{
    if(value.isDouble)
    {
        out<<value.num;
    }
    else
    {
        out<<value.str.toLocal8Bit().data();
    }
    return out;
}

QString MyValueContainer::toString()const
{
    if(isDouble)
    {
        return QString::number(num);
    }
    else
    {
        return str;
    }
}

double log10(const MyValueContainer &value)
{
    return log10(value.num);
}
