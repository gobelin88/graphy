#ifndef DATACONVERT_H
#define DATACONVERT_H

#include <QVector>
#include <iostream>
#include <Eigen/Dense>
#include <QDataStream>

std::vector<double> toStdVector(const Eigen::VectorXd& v);
Eigen::VectorXd fromStdVector(const std::vector<double>& v_std);

QVector<double> toQVector(const Eigen::VectorXd& v);
Eigen::VectorXd fromQVector(const QVector<double>& v);

QString toString(const Eigen::MatrixXd& v);

QDataStream & operator<<(QDataStream & ds,const Eigen::VectorXd & v);
QDataStream & operator>>(QDataStream & ds,Eigen::VectorXd & v);
QDataStream & operator<<(QDataStream & ds,const QVector<QString> & v);
QDataStream & operator>>(QDataStream & ds,QVector<QString> & v);

#endif // TABLEDATA_H
