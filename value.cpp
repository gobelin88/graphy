#include "value.h"

ValueContainer::ValueContainer()
{
    num=0.0;
    str.clear();
    isDouble=false;
}

void ValueContainer::operator=(const ValueContainer & other)
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

void ValueContainer::operator=(const QVariant & other)
{
    num=other.toDouble(&isDouble);

    if(!isDouble)
    {
        str=other.toString();
    }
}

void ValueContainer::operator=(const QString & other)
{
    isDouble=false;
    str=other;
}

std::ostream& operator<< (std::ostream &out,const ValueContainer & value)
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

QString ValueContainer::toString()const
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

double log10(const ValueContainer &value)
{
    return log10(value.num);
}
