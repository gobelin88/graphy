#include "SolidHarmonicsDecomposition.h"

SolidHarmonicsDecomposition::SolidHarmonicsDecomposition(const std::vector<Eigen::Vector4d> & _dataPoints,
                                                         int n,
                                                         SolidHarmonicsDecomposition::Mode mode,
                                                         bool cart)
    :dataPoints(_dataPoints)
{
    // C(i,j)=(l,m)
    //        0 0  1 1  2 2
    //        1-1  1 0  2 1
    //        2-2  2-1  2 0

    this->mode=mode;
    this->n=n;
    this->cart=cart;
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

int SolidHarmonicsDecomposition::getLMIndex(int l,int m)const
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

    return i*n+j;
}

double SolidHarmonicsDecomposition::getC(int l,int m)const
{
    if(std::abs(m)<=l && l<n)
    {
        return p[getLMIndex(l,m)];
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

//Eigen::Vector4d SolidHarmonicsDecomposition::delta(const Eigen::Vector4d& pt)
//{
//    //std::cout<<p.transpose()<<std::endl;
//    return Eigen::Vector4d(0,0,0,at(pt[0],pt[1],pt[2])-pt[3]);
//}

//double SolidHarmonicsDecomposition::at(double x,double y,double z)
//{
//    double sum=0.0;
//    for(int l=0;l<n;l++)
//    {
//        for(int m=-l;m<=l;m++)
//        {
//            double sh;

//            if(cart)
//            {
//                sh=(mode==MODE_REGULAR)?
//                    cart_solidHarmonicsR(x,y,z,l,m):
//                    cart_solidHarmonicsI(x,y,z,l,m);
//            }
//            else
//            {
//                sh=(mode==MODE_REGULAR)?
//                    sph_solidHarmonicsR(x,y,z,l,m):
//                    sph_solidHarmonicsI(x,y,z,l,m);
//            }

//            sum+=getC(l,m)*sh;
//        }
//    }
//    return sum;
//}

Eigen::Vector2d SolidHarmonicsDecomposition::delta(const Eigen::Vector2d & pt)
{
    return Eigen::Vector2d(0,at((int)pt[0])-pt[1]);
}

double SolidHarmonicsDecomposition::at(int index)
{
    double sum=0.0;
    for(int l=0;l<n;l++)
    {
        for(int m=-l;m<=l;m++)
        {
            double sh;

            if(precalYs.size()>0)
            {
                sh=precalYs[index][getLMIndex(l,m)];
            }
            else
            {
                sh=getYs(index,l,m);
            }

            sum+=getC(l,m)*sh;
        }
    }
    return sum;
}

double SolidHarmonicsDecomposition::getYs(int index,int l,int m)
{
    double Ys;

    double x=dataPoints[index][0];
    double y=dataPoints[index][1];
    double z=dataPoints[index][2];

    if(cart)
    {
        Ys=(mode==MODE_REGULAR)?
            cart_solidHarmonicsR(x,y,z,l,m):
            cart_solidHarmonicsI(x,y,z,l,m);
    }
    else
    {
        Ys=(mode==MODE_REGULAR)?
            sph_solidHarmonicsR(x,y,z,l,m):
            sph_solidHarmonicsI(x,y,z,l,m);
    }

    return Ys;
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



void SolidHarmonicsDecomposition::preCalcSolidsHarmonics()
{
    precalYs.resize(dataPoints.size(),Eigen::VectorXd::Zero(n*n));

    for(int l=0;l<n;l++)
    {
        for(int m=-l;m<=l;m++)
        {
            for(int index=0;index<dataPoints.size();index++)
            {
                precalYs[index][getLMIndex(l,m)]=getYs(index,l,m);
            }
        }
    }

}

void SolidHarmonicsDecomposition::guessDecomposition()
{
    for(int l=0;l<n;l++)
    {
        for(int m=-l;m<=l;m++)
        {
            double sum_lm=0.0,ny=0.0;
            int lmIndex=getLMIndex(l,m);
            for(int index=0;index<dataPoints.size();index++)
            {
                double sy;

                if(precalYs.size()>index)
                {
                    sy=precalYs[index][lmIndex];
                }
                else
                {
                    sy=getYs(index,l,m);
                }

                double sx=dataPoints[index][3];

                sum_lm+=sx*sy;
                ny+=sy*sy;
            }
            setC(l,m,sum_lm/ny);
        }
    }
}
