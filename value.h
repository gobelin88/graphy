#include <QString>
#include <iostream>

#ifndef VALUE_H
#define VALUE_H

class Value
{
public:
    Value();
    void operator=(const Value & other);

    double val;
    QString str;
    bool isDouble;
};

std::ostream& operator<< (std::ostream &out,const Value & value);
double log10(const Value & value);

#endif // VALUE_H
