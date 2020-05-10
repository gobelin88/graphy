#include "tabledata.h"

void create(TableData& data,uint nbC,uint nbL)
{
    data=QVector< QVector<double> > (int(nbC),QVector<double>(int(nbL)));
}

void interpolate(const TableData& data,const BoxPlot& box,QCPColorMap* map,size_t knn,InterpolationMode mode)
{
    Eigen::MatrixXd datapoints(2,data[box.idX].size());

    for (int i=0; i<data[box.idX].size(); i++)
    {
        datapoints(0,i)=data[box.idX][i];
        datapoints(1,i)=data[box.idY][i];
    }

    std::cout<<"data[box.idX].size()="<<data[box.idX].size()<<std::endl;
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
                        value+=(1.0/dists(k,0))* data[box.idZ][idx(k,0)];
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
                    value=data[box.idZ][idx(0,0)];
                }
            }

            map->data()->setCell(i,j,value);
        }
    }
}

double getMin(const TableData& data,int id)
{
    return  *std::min_element(data[id].constBegin(), data[id].constEnd());
}

double getMax(const TableData& data,int id)
{
    return  *std::max_element(data[id].constBegin(), data[id].constEnd());
}

QCPRange getRange(const TableData& data,int id)
{
    std::cout<<getMin(data,id)<<" "<<getMax(data,id)<<std::endl;

    return  QCPRange(getMin(data,id),getMax(data,id));
}
