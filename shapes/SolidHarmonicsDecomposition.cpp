#include "SolidHarmonicsDecomposition.h"

SolidHarmonicsDecomposition::SolidHarmonicsDecomposition(int n,SolidHarmonicsDecomposition::Mode mode)
{
    // C(i,j)=(l,m)
    //        0 0  1 1  2 2
    //        1-1  1 0  2 1
    //        2-2  2-1  2 0

    this->mode=mode;
    this->n=n;
    p=Eigen::VectorXd::Zero(n*n);
}

SolidHarmonicsDecomposition::~SolidHarmonicsDecomposition()
{
    std::cout<<"Delete SolidHarmonicsDecomposition"<<std::endl;
}

SolidHarmonicsDecomposition::Mode SolidHarmonicsDecomposition::getMode()
{
    return mode;
}

int SolidHarmonicsDecomposition::maxOrder()
{
    return 20;
}

int SolidHarmonicsDecomposition::order()
{
    return n;
}

void SolidHarmonicsDecomposition::setC(int l,int m,double value)
{
    if(std::abs(m)<=l && l<n)
    {
        int i=l;
        int j=l;

        if(m>0)
        {
            i-=m;
        }
        else if(m<0)
        {
            j+=m;
        }

        p[i*n+j]=value;
    }
}

double SolidHarmonicsDecomposition::getC(int l,int m)const
{
    if(std::abs(m)<=l && l<n)
    {
        int i=l;
        int j=l;

        if(m>0)
        {
            i-=m;
        }
        else if(m<0)
        {
            j+=m;
        }

        return p[i*n+j];
    }
    else
    {
        return 0.0;
    }
}

Eigen::MatrixXd SolidHarmonicsDecomposition::getC()
{
    Eigen::MatrixXd C(n,n);
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            C(i,j)=p[i*n+j];
        }
    }
    return C;
}

Eigen::Vector4d SolidHarmonicsDecomposition::delta(const Eigen::Vector4d& pt)
{
    //std::cout<<p.transpose()<<std::endl;
    return Eigen::Vector4d(0,0,0,at(pt[0],pt[1],pt[2])-pt[3]);
}

double SolidHarmonicsDecomposition::at(double x,double y,double z)
{
    double sum=0.0;
    for(int l=0;l<n;l++)
    {
        for(int m=-l;m<=l;m++)
        {
            double sh=(mode==MODE_REGULAR)?
                      solidHarmonicsR(x,y,z,l,m):
                      solidHarmonicsI(x,y,z,l,m);

            sum+=getC(l,m)*sh;
        }
    }
    return sum;
}

int SolidHarmonicsDecomposition::nb_params()
{
    return p.rows();
}

void SolidHarmonicsDecomposition::setParams(const Eigen::VectorXd& _p)
{
    this->p=_p;
}

const Eigen::VectorXd& SolidHarmonicsDecomposition::getParams()
{
    return p;
}

void SolidHarmonicsDecomposition::guessDecomposition(const std::vector<Eigen::Vector4d> & pt)
{
    for(int l=0;l<n;l++)
    {
        for(int m=-l;m<=l;m++)
        {
            double sum_lm=0.0,nh=0.0,ni=0.0;
            for(int i=0;i<pt.size();i++)
            {
                double sh=(mode==MODE_REGULAR)?
                          solidHarmonicsR(pt[i][0],pt[i][1],pt[i][2],l,m):
                          solidHarmonicsI(pt[i][0],pt[i][1],pt[i][2],l,m);
                double si=pt[i][3];

                sum_lm+=si*sh;
                nh+=sh*sh;
                ni+=si*si;
            }
            setC(l,m,sum_lm/sqrt(nh*ni));
        }
    }
}
