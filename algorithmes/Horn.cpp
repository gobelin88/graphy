#include "Horn.h"

namespace algorithms
{

void computeHornTranform(const std::vector<Eigen::Vector3d> & Rr,const std::vector<Eigen::Vector3d> & Rl, Eigen::Quaterniond & q,Eigen::Vector3d & t, double & s)
{
    //1 Calculs des centroids
    Eigen::Vector3d Cr=getHornCentroid(Rr);
    Eigen::Vector3d Cl=getHornCentroid(Rl);

    //2 Soustraction des centroids
    std::vector<Eigen::Vector3d> Rr_prime=substractHornCentroid(Rr,Cr);
    std::vector<Eigen::Vector3d> Rl_prime=substractHornCentroid(Rl,Cl);

    //3 Minimisation de la rotation
    q=getHornQuaternion(getHornN(getHornM(Rr_prime,Rl_prime)));

    //4 Calcul du facteur d'echelle
    s=getHornScaleSym(Rr_prime,Rl_prime);

    //5 Calcul de la translation
    t=getHornTranslation(Cr,Cl,s,q);
}

Eigen::Vector3d getHornCentroid(std::vector<Eigen::Vector3d> R)
{
    Eigen::Vector3d centroid(0,0,0);
    if(R.size()>0)
    {
        for(unsigned int i=0;i<R.size();i++)
        {
            centroid+=R[i];
        }
        centroid/=R.size();
    }
    return centroid;
}

std::vector<Eigen::Vector3d> substractHornCentroid(const std::vector<Eigen::Vector3d> & R,const Eigen::Vector3d & C)
{
    std::vector<Eigen::Vector3d> R_sub(R.size());
    for(unsigned int i=0;i<R.size();i++){R_sub[i]=R[i]-C;}
    return R_sub;
}

Eigen::Matrix3d getHornM(const std::vector<Eigen::Vector3d> & Rr_prime,const std::vector<Eigen::Vector3d> & Rl_prime)
{
    Eigen::Matrix3d M;
    M.setZero();
    if(Rr_prime.size()==Rl_prime.size())
    {
        for(std::size_t i=0;i<Rl_prime.size();i++)
        {
            M+=Rl_prime[i]*Rr_prime[i].transpose();
        }
    }
    return M;
}

Eigen::Matrix4d getHornN(const Eigen::Matrix3d & M)
{
    Eigen::Matrix4d N;

    double N00= M(0,0)+M(1,1)+M(2,2);
    double N11= M(0,0)-M(1,1)-M(2,2);
    double N22=-M(0,0)+M(1,1)-M(2,2);
    double N33=-M(0,0)-M(1,1)+M(2,2);

    double N01= M(1,2)-M(2,1);
    double N02= M(2,0)-M(0,2);
    double N03= M(0,1)-M(1,0);
    double N12= M(0,1)+M(1,0);
    double N13= M(2,0)+M(0,2);
    double N23= M(1,2)+M(2,1);

    N<< N00 , N01 , N02 , N03 ,
        N01 , N11 , N12 , N13 ,
        N02 , N12 , N22 , N23 ,
        N03 , N13 , N23 , N33 ;

    return N;
}

Eigen::Quaterniond getHornQuaternion(const Eigen::Matrix4d & N)
{
    Eigen::EigenSolver<Eigen::Matrix4d> eigen_solver(N,true);

    Eigen::Vector4d eigen_values=eigen_solver.pseudoEigenvalueMatrix().diagonal();
    Eigen::Matrix4d eigen_vectors=eigen_solver.pseudoEigenvectors();

    int index_max;
    eigen_values.maxCoeff(&index_max);

    Eigen::Vector4d q_mat=eigen_vectors.col(index_max);

    return Eigen::Quaterniond(q_mat[0],q_mat[1],q_mat[2],q_mat[3]);
}

double getHornScaleSym(const std::vector<Eigen::Vector3d> & Rr_prime, const std::vector<Eigen::Vector3d> & Rl_prime)
{
    double scale_r=0.0,scale_l=0.0;
    for(unsigned int i=0;i<Rr_prime.size();i++)
    {
        scale_r+=Rr_prime[i].squaredNorm();
        scale_l+=Rl_prime[i].squaredNorm();
    }
    return sqrt(scale_r/scale_l);
}

Eigen::Vector3d getHornTranslation(const Eigen::Vector3d & Cr,const Eigen::Vector3d & Cl, double s, const Eigen::Quaterniond &q)
{
    return Cr-s*q._transformVector(Cl);
}

void testHorn()
{
//    std::cout<<"-----------------------------"<<std::endl;
//    std::cout<<"Test Horn:"<<std::endl;
//    Eigen::Quaterniond q=Eigen::AngleAxisd(10,Eigen::Vector3d(1,0,0))*Eigen::AngleAxisd(20,Eigen::Vector3d(0,1,0))*Eigen::AngleAxisd(30,Eigen::Vector3d(0,0,1)),q_hat;
//    Eigen::Vector3d t(1,2,3),t_hat;
//    double s=0.5,s_hat;

//    std::vector<Eigen::Vector3d> Rl;
//    for(int i=0;i<1000;i++)
//    {
//        Rl.push_back( Eigen::Vector3d(Ru(),Ru(),Ru()) );
//    }

//    std::vector<Eigen::Vector3d> Rr;
//    for(unsigned int i=0;i<Rl.size();i++)
//    {
//        Eigen::Vector3d noise(Rg(0,0.05),Rg(0,0.05),Rg(0,0.05));
//        Rr.push_back( s*q._transformVector(Rl[i])+t + noise );
//    }


//    computeHornTranform(Rr,Rl,q_hat,t_hat,s_hat);


//    std::cout<<"q="<<q.w()<<" "<<q.x()<<" "<<q.y()<<" "<<q.z()<<std::endl;
//    std::cout<<"q_hat="<<q_hat.w()<<" "<<q_hat.x()<<" "<<q_hat.y()<<" "<<q_hat.z()<<std::endl;
//    std::cout<<"t="<<t.transpose()<<std::endl;
//    std::cout<<"t_hat="<<t_hat.transpose()<<std::endl;
//    std::cout<<"s="<<s<<std::endl;
//    std::cout<<"s_hat="<<s_hat<<std::endl;
//    std::cout<<"RMS="<<getHornRMS(Rr,Rl,q_hat,t_hat,s_hat)<<std::endl;
//    std::cout<<"-----------------------------"<<std::endl;
}

Eigen::VectorXd getHornError(const std::vector<Eigen::Vector3d> & Rr, const std::vector<Eigen::Vector3d> & Rl,const Eigen::Quaterniond & q,const Eigen::Vector3d & t,double s)
{
    Eigen::VectorXd err(Rr.size());

    for(unsigned int i=0;i<Rr.size();i++)
    {
        Eigen::Vector3d err_vec=Rr[i]-(s*q._transformVector(Rl[i])+t);
        err[i]=err_vec.norm();
    }

    return err;
}

double getHornRMS(const std::vector<Eigen::Vector3d> & Rr, const std::vector<Eigen::Vector3d> & Rl,const Eigen::Quaterniond & q,const Eigen::Vector3d & t,double s)
{
    Eigen::VectorXd E=getHornError(Rr,Rl,q,t,s);
    return E.norm()/sqrt(E.size());
}

}
