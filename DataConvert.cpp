#include "DataConvert.h"
#include "kdtree_eigen.h"

void create(Eigen::MatrixXd& data, uint nbL,uint nbC)
{
    data=Eigen::MatrixXd(nbL,nbC);
}

std::vector<double> toStdVector(const Eigen::VectorXd& v)
{
    std::vector<double> v_std(v.size());
    memcpy(v_std.data(),v.data(),v_std.size()*sizeof(double));
    return v_std;
}

QString toString(const Eigen::MatrixXd& m)
{
    QString str;
    for (int i=0; i<m.rows(); i++)
    {
        for (int j=0; j<m.cols(); j++)
        {
            str+=QString::number(m(i,j));
            if (j!=m.cols()-1)
            {
                str+=" ";
            }
        }
        str.append("\n");
    }
    return str;
}

QVector<double> toQVector(const Eigen::VectorXd& v)
{
    if(v.size()>0)
    {
        QVector<double> v_q(v.size());
        memcpy(v_q.data(),v.data(),v_q.size()*sizeof(double));
        return v_q;
    }
    else
    {
        return QVector<double>();
    }
}

Eigen::VectorXd fromStdVector(const std::vector<double>& v_std)
{
    Eigen::VectorXd v(v_std.size());
    memcpy(v.data(),v_std.data(),v.size()*sizeof(double));
    return v;
}

Eigen::VectorXd fromQVector(const QVector<double>& v_q)
{
    Eigen::VectorXd v(v_q.size());
    memcpy(v.data(),v_q.data(),v.size()*sizeof(double));
    return v;
}

QDataStream & operator<<(QDataStream & ds,const Eigen::VectorXd & v)
{
    ds<<v.rows();
    for(int i=0;i<v.rows();i++)
    {
        ds<<v[i];
    }
    return ds;
}

QDataStream & operator>>(QDataStream & ds,Eigen::VectorXd & v)
{
    Eigen::Index nbRows;
    ds>>nbRows;
    v.resize(nbRows);
    for(int i=0;i<v.rows();i++)
    {
        ds>>v[i];
    }
    return ds;
}

QDataStream & operator<<(QDataStream & ds,const QVector<QString> & v)
{
    ds<<v.size();
    for(int i=0;i<v.size();i++)
    {
        ds<<v[i];
    }
    return ds;
}

QDataStream & operator>>(QDataStream & ds,QVector<QString> & v)
{
    int size;
    ds>>size;
    v.resize(size);
    for(int i=0;i<v.size();i++)
    {
        ds>>v[i];
    }
    return ds;
}

QDataStream & operator<<(QDataStream & ds,const std::vector<Eigen::Vector4d> & v)
{
    ds<<v.size();
    for(int i=0;i<v.size();i++)
    {
        ds<<v[i][0]<<v[i][1]<<v[i][2]<<v[i][3];
    }
    return ds;
}
QDataStream & operator>>(QDataStream & ds,std::vector<Eigen::Vector4d> & v)
{
    size_t size_v;
    ds>>size_v;
    v.resize(size_v);
    for(int i=0;i<size_v;i++)
    {
        ds>>v[i][0]>>v[i][1]>>v[i][2]>>v[i][3];
    }
    return ds;
}
