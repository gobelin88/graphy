#include "tabledata.h"

void create(Eigen::MatrixXd& data, uint nbL,uint nbC)
{
    data=Eigen::MatrixXd(nbL,nbC);
}

void removeRows(Eigen::MatrixXd& matrix, unsigned int rowToRemove, unsigned int nbRow)
{
    unsigned int numRows = matrix.rows()-nbRow;
    unsigned int numCols = matrix.cols();

    if ( rowToRemove < numRows )
    {
        matrix.block(rowToRemove,0,numRows-rowToRemove,numCols) = matrix.block(rowToRemove+nbRow,0,numRows-rowToRemove,numCols);
    }
    matrix.conservativeResize(numRows,numCols);
}

void removeColumns(Eigen::MatrixXd& matrix, unsigned int colToRemove,unsigned int nbCol)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols()-nbCol;

    if ( colToRemove < numCols )
    {
        matrix.block(0,colToRemove,numRows,numCols-colToRemove) = matrix.block(0,colToRemove+nbCol,numRows,numCols-colToRemove);
    }
    matrix.conservativeResize(numRows,numCols);
}

void addRow(Eigen::MatrixXd& matrix, Eigen::VectorXd rowToAdd)
{
    unsigned int numRows = matrix.rows()+1;
    unsigned int numCols = matrix.cols();
    matrix.conservativeResize(numRows,numCols);
    matrix.row(numRows-1)=rowToAdd;
}

void addRows(Eigen::MatrixXd& matrix, int n)
{
    unsigned int numRows = matrix.rows()+n;
    unsigned int numCols = matrix.cols();
    matrix.conservativeResize(numRows,numCols);
    matrix.block(numRows-n,0,n,numCols);
}

void addColumn(Eigen::MatrixXd& matrix, Eigen::VectorXd colToAdd)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols()+1;
    matrix.conservativeResize(numRows,numCols);
    matrix.col(numCols-1)=colToAdd;
}

void swapColumns(Eigen::MatrixXd& matrix,int ida,int idb)
{
    matrix.col(ida).swap(matrix.col(idb));
}

void moveColumn(Eigen::MatrixXd& matrix,int ida,int idb)
{
    if (ida<idb)
    {
        for (int k=ida; k<idb; k++)
        {
            swapColumns(matrix,k,k+1);
        }
    }
    else if (ida>idb)
    {
        for (int k=ida; k>idb; k--)
        {
            swapColumns(matrix,k,k-1);
        }
    }
}

void swapRows(Eigen::MatrixXd& matrix,int ida,int idb)
{
    matrix.row(ida).swap(matrix.row(idb));
}

void moveRow(Eigen::MatrixXd& matrix,int ida,int idb)
{
    if (ida<idb)
    {
        for (int k=ida; k<idb; k++)
        {
            swapRows(matrix,k,k+1);
        }
    }
    else if (ida>idb)
    {
        for (int k=ida; k>idb; k--)
        {
            swapRows(matrix,k,k-1);
        }
    }
}


void interpolate(const Eigen::MatrixXd& data,const BoxPlot& box,QCPColorMap* map,size_t knn,InterpolationMode mode)
{
    Eigen::MatrixXd datapoints(2,data.col(box.idX).size());

    for (int i=0; i<datapoints.cols(); i++)
    {
        datapoints(0,i)=data.col(box.idX)[i];
        datapoints(1,i)=data.col(box.idY)[i];
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
                        value+=(1.0/dists(k,0))* data.col(box.idZ)[idx(k,0)];
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
                    value=data.col(box.idZ)[idx(0,0)];
                }
            }

            map->data()->setCell(i,j,value);
        }
    }
}

double getMin(const Eigen::MatrixXd& data,int id)
{
    return  data.col(id).minCoeff();
}

double getMax(const Eigen::MatrixXd& data,int id)
{
    return  data.col(id).maxCoeff();
}

QCPRange getRange(const Eigen::MatrixXd& data, int id)
{
    std::cout<<getMin(data,id)<<" "<<getMax(data,id)<<std::endl;

    return  QCPRange(getMin(data,id),getMax(data,id));
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

QVector<QString> toQVectorStr(const Eigen::VectorXd& v)
{
    QVector<QString> v_str(v.size());
    for (int i=0; i<v.size(); i++)
    {
        v_str[i]=QString::number(v[i]);
    }
    return v_str;
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

void sortBy(Eigen::MatrixXd& matrix, int colId,SortMode mode)
{
    std::vector<Eigen::VectorXd> vec;
    for (int64_t i = 0; i < matrix.rows(); ++i)
        vec.push_back(matrix.row(i));

    if(mode==ASCENDING)
    {
        std::sort(vec.begin(), vec.end(), [&colId](Eigen::VectorXd const& t1, Eigen::VectorXd const& t2){ return t1(colId) < t2(colId); } );
    }
    else if(mode==DECENDING)
    {
        std::sort(vec.begin(), vec.end(), [&colId](Eigen::VectorXd const& t1, Eigen::VectorXd const& t2){ return t1(colId) > t2(colId); } );
    }

    for (int64_t i = 0; i < matrix.rows(); ++i)
        matrix.row(i) = vec[i];
}

void thresholdBy(Eigen::MatrixXd& matrix, int colId,ThresholdMode mode,double value)
{
    std::vector<Eigen::VectorXd> vec;
    for (int64_t i = 0; i < matrix.rows(); ++i)
    {
        if(mode==KEEP_GREATER)
        {
            if(matrix(i,colId)>value)//KEEP_GREATER
            {
                vec.push_back(matrix.row(i));
            }
        }
        else
        {
            if(matrix(i,colId)<value)//KEEP_LOWER
            {
                vec.push_back(matrix.row(i));
            }
        }
    }

    for (int64_t i = 0; i < vec.size(); ++i)
    {
        matrix.row(i) = vec[i];
    }

    matrix.conservativeResize(vec.size(),matrix.cols());
}
