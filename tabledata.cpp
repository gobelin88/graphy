#include "tabledata.h"
#include "kdtree_eigen.h"

void create(Eigen::MatrixXd& data, uint nbL,uint nbC)
{
    data=Eigen::MatrixXd(nbL,nbC);
}






void interpolate(const Eigen::VectorXd& dataX,
                 const Eigen::VectorXd& dataY,
                 const Eigen::VectorXd& dataZ,
                 const Resolution& box,
                 QCPColorMap* map,
                 size_t knn,
                 InterpolationMode mode)
{
    Eigen::MatrixXd datapoints(2,dataX.size());

    for (int i=0; i<datapoints.cols(); i++)
    {
        datapoints(0,i)=dataX[i];
        datapoints(1,i)=dataY[i];
    }

    std::cout<<"data[box.idX].size()="<<datapoints.cols()<<std::endl;
    std::cout<<datapoints.transpose()<<std::endl;

    kdt::KDTreed kdtree(datapoints);
    kdtree.build();

    kdt::KDTreed::Matrix dists; // basically Eigen::MatrixXd
    kdt::KDTreed::MatrixI idx; // basically Eigen::Matrix<Eigen::Index>

    for (uint i=0; i<box.pX_res; i++)
    {
        for (uint j=0; j<box.pY_res; j++)
        {
            kdt::KDTreed::Matrix queryPoints(2,1);
            map->data()->cellToCoord(int(i),int(j),&queryPoints(0,0),&queryPoints(1,0));

            double value=0;
            if (mode==MODE_WEIGHTED)
            {
                kdtree.query(queryPoints, knn, idx, dists);
                double weight_sum=0;
                for (int k=0; k<knn; k++)
                {
                    if (idx(k,0)>=0)
                    {
                        value+=(1.0/dists(k,0))* dataZ[idx(k,0)];
                        weight_sum+=(1.0/dists(k,0));
                    }
                }
                value/=weight_sum;
            }
            else if (mode==MODE_NEAREST)
            {
                kdtree.query(queryPoints, 1, idx, dists);
                if (idx(0,0)>=0)
                {
                    value=dataZ[idx(0,0)];
                }
            }

            map->data()->setCell(i,j,value);
        }
    }
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
    QVector<double> v_q(v.size());
    memcpy(v_q.data(),v.data(),v_q.size()*sizeof(double));
    return v_q;
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
