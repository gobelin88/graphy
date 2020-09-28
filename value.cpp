#include "value.h"

Value::Value()
{
    val=0.0;
    str.clear();
    isDouble=true;
}

void Value::operator=(const Value & other)
{
    val=other.val;
    str=other.str;
    isDouble=other.isDouble;
}

std::ostream& operator<< (std::ostream &out,const Value & value)
{
    if(value.isDouble)
    {
        out<<value.val;
    }
    else
    {
        out<<value.str.toLocal8Bit().data();
    }
    return out;
}

double log10(const Value &value)
{
    return log10(value.val);
}
