#include "tabledata.h"

void create(TableData & data,uint nbC,uint nbL)
{
    data=QVector< QVector<double> > (int(nbC),QVector<double>(int(nbL)));
}

double getNearest(const TableData & data,const BoxPlot & box,Eigen::Vector2d p)
{
    double dist=DBL_MAX,value=0.0;

    for(int i=0;i<data[box.idX].size();i++)
    {
        Eigen::Vector2d m(data[box.idX][i],data[box.idY][i]);
        double d=(p-m).norm();

        if(d<dist)
        {
            dist=d;
            value=data[box.idZ][i];
        }

    }

    return value;
}

void interpolate(const TableData & data,const BoxPlot & box,QCPColorMap * map,size_t knn)
{
    Eigen::MatrixXd datapoints(2,data[box.idX].size());

    for(int i=0;i<data[box.idX].size();i++)
    {
        datapoints(0,i)=data[box.idX][i];
        datapoints(1,i)=data[box.idY][i];
    }

    kdt::KDTreed kdtree(datapoints);
    kdtree.build();

    kdt::KDTreed::Matrix dists; // basically Eigen::MatrixXd
    kdt::KDTreed::MatrixI idx; // basically Eigen::Matrix<Eigen::Index>

    for(uint i=0;i<box.pX_res;i++)
    {
        for(uint j=0;j<box.pY_res;j++)
        {
            kdt::KDTreed::Matrix queryPoints(2,1);
            map->data()->cellToCoord(int(i),int(j),&queryPoints(0,0),&queryPoints(1,0));
            kdtree.query(queryPoints, knn, idx, dists);

            double value=0,weight=0;
            for(int k=0;k<knn;k++)
            {
                if(idx(k,0)>0)
                {
                    value+=(1.0/dists(k,0))* data[box.idZ][idx(k,0)];
                    weight+=(1.0/dists(k,0));
                }
            }
            value/=weight;

            map->data()->setCell(i,j,value);
        }
    }
}

double getMin(const TableData & data,int id)
{
    return  *std::min_element(data[id].constBegin(), data[id].constEnd());
}

double getMax(const TableData & data,int id)
{
    return  *std::max_element(data[id].constBegin(), data[id].constEnd());
}

QCPRange getRange(const TableData & data,int id)
{
    std::cout<<getMin(data,id)<<" "<<getMax(data,id)<<std::endl;

    return  QCPRange(getMin(data,id),getMax(data,id));
}
