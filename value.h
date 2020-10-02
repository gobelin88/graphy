#include <QString>
#include <iostream>
#include <Eigen/Dense>

#ifndef VALUE_H
#define VALUE_H

class ValueContainer
{
public:
    ValueContainer();
    void operator=(const ValueContainer & other);

    QString toString()const;

    double num;
    QString str;
    bool isDouble;
};

std::ostream& operator<< (std::ostream &out,const ValueContainer & value);
double log10(const ValueContainer & value);

using MatrixXv=Eigen::Matrix<ValueContainer,Eigen::Dynamic,Eigen::Dynamic>;
using VectorXv=Eigen::Matrix<ValueContainer,Eigen::Dynamic,1>;

#endif // VALUE_H
