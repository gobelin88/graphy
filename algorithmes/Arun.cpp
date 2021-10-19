#include "Arun.h"

namespace algorithms
{



bool computeArunTranform(const std::vector<Eigen::Vector3d> & P_prime, const std::vector<Eigen::Vector3d> & P, Eigen::Matrix3d & r, Eigen::Vector3d & t)
{
    //Step 1 Calulate centroid and substract
    Eigen::Vector3d C=getArunCentroid(P);
    Eigen::Vector3d C_prime=getArunCentroid(P_prime);
    std::vector<Eigen::Vector3d> Q=substractArunCentroid(P,C);
    std::vector<Eigen::Vector3d> Q_prime=substractArunCentroid(P_prime,C_prime);

    //Step 2 calculate H
    Eigen::Matrix3d H=getArunH(Q,Q_prime);

    //Step 3 Find the SVD of H
    Eigen::JacobiSVD<Eigen::Matrix3d> svd(H,Eigen::ComputeFullU | Eigen::ComputeFullV);

    //Step 4 Calculate X
    Eigen::Matrix3d U=svd.matrixU();
    Eigen::Matrix3d V=svd.matrixV();
    Eigen::Matrix3d X= V*U.transpose();

    //Step 5 calculate det(X)
    double det=X.determinant();
    if(det>0)
    {
        r=X;
    }
    else if(det<0)//not a rotation
    {
        if(svd.singularValues().prod()==0)//If one of the singular values is zero
        {
            std::cout<<"Reflect"<<std::endl;
            Eigen::Matrix3d V_prime=V;
            V_prime(0,2)=-V(0,2);
            V_prime(1,2)=-V(1,2);
            V_prime(2,2)=-V(2,2);
            r=V_prime*U.transpose();
            return true;
        }
        else
        {
            return false;
        }
    }

    //Step 6 find the translation
    t=C_prime-r*C;

    return true;
}

Eigen::VectorXd getArunError(const std::vector<Eigen::Vector3d> & P_prime, const std::vector<Eigen::Vector3d> & P,const Eigen::Matrix3d & r,const Eigen::Vector3d & t)
{
    Eigen::VectorXd err(P_prime.size());

    for(unsigned int i=0;i<P_prime.size();i++)
    {
        Eigen::Vector3d err_vec=P_prime[i]-(r*P[i]+t);
        err[i]=err_vec.norm();
    }

    return err;
}

double getArunRMS(const std::vector<Eigen::Vector3d> & P_prime, const std::vector<Eigen::Vector3d> & P,const Eigen::Matrix3d & r,const Eigen::Vector3d & t)
{
    Eigen::VectorXd E=getArunError(P_prime,P,r,t);
    return E.norm()/sqrt(E.rows());
}

double getArunRMSQuat(const std::vector<Eigen::Vector3d> & P_prime, const std::vector<Eigen::Vector3d> & P,const Eigen::Quaterniond & q,const Eigen::Vector3d & t)
{
    double err=0.0;

    for(unsigned int i=0;i<P_prime.size();i++)
    {
        Eigen::Vector3d err_vec=P_prime[i]-(q._transformVector(P[i])+t);
        err+=err_vec.squaredNorm();
    }

    return sqrt(err/P_prime.size());
}

Eigen::Vector3d getArunCentroid(std::vector<Eigen::Vector3d> P)
{
    Eigen::Vector3d centroid(0,0,0);
    if(P.size()>0)
    {
        for(unsigned int i=0;i<P.size();i++)
        {
            centroid+=P[i];
        }
        centroid/=P.size();
    }
    return centroid;
}

std::vector<Eigen::Vector3d> substractArunCentroid(const std::vector<Eigen::Vector3d> &P, const Eigen::Vector3d &C)
{
    std::vector<Eigen::Vector3d> R_sub(P.size());
    for(unsigned int i=0;i<P.size();i++){R_sub[i]=P[i]-C;}
    return R_sub;
}

Eigen::Matrix3d getArunH(const std::vector<Eigen::Vector3d> & Q,const std::vector<Eigen::Vector3d> & Q_prime)
{
    Eigen::Matrix3d M;
    M.setZero();
    if(Q.size()==Q_prime.size())
    {
        for(unsigned int i=0;i<Q_prime.size();i++)
        {
            M+=Q[i]*Q_prime[i].transpose();
        }
    }
    return M;
}

void testArun()
{
//    std::cout<<"-----------------------------"<<std::endl;
//    std::cout<<"Test Arun:"<<std::endl;
//    Eigen::Quaterniond q=Eigen::AngleAxisd(10,Eigen::Vector3d(1,0,0))*Eigen::AngleAxisd(20,Eigen::Vector3d(0,1,0))*Eigen::AngleAxisd(30,Eigen::Vector3d(0,0,1)),q_hat;
//    Eigen::Matrix3d q_hat_mat;
//    Eigen::Vector3d t(1,2,3),t_hat;

//    std::vector<Eigen::Vector3d> Rl;
//    for(int i=0;i<1000;i++)
//    {
//        Rl.push_back( Eigen::Vector3d(Ru(),Ru(),Ru()) );
//    }

//    std::vector<Eigen::Vector3d> Rr;
//    for(unsigned int i=0;i<Rl.size();i++)
//    {
//        Eigen::Vector3d noise(Rg(0,0.05),Rg(0,0.05),Rg(0,0.05));
//        Rr.push_back( q._transformVector(Rl[i])+t + noise );
//    }


//    computeArunTranform(Rr,Rl,q_hat_mat,t_hat);
//    q_hat=Eigen::Quaterniond(q_hat_mat);

//    std::cout<<"q="<<q.w()<<" "<<q.x()<<" "<<q.y()<<" "<<q.z()<<std::endl;
//    std::cout<<"q_hat="<<q_hat.w()<<" "<<q_hat.x()<<" "<<q_hat.y()<<" "<<q_hat.z()<<std::endl;
//    std::cout<<"t="<<t.transpose()<<std::endl;
//    std::cout<<"t_hat="<<t_hat.transpose()<<std::endl;
//    std::cout<<"RMS="<<getArunRMS(Rr,Rl,q_hat_mat,t_hat)<<" avec le quaternion:"<<getArunRMSQuat(Rr,Rl,q_hat,t_hat)<<std::endl;
//    std::cout<<"-----------------------------"<<std::endl;
}

}
