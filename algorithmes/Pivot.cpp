#include "Pivot.h"

namespace algorithms
{

Pivot::Pivot(PosAttList Tsw)
{
    this->Tsw=Tsw;
}

void Pivot::computeSVD()
{
    Eigen::MatrixXd A(Tsw.size()*3,6);
    Eigen::VectorXd b(Tsw.size()*3);

    for (unsigned int i=0;i<Tsw.size();i++)
    {
        // The transform
        Eigen::Matrix4d currentTransf = Tsw[i].toMatrice(  );

        // Set the R matrix
        A(i*3+0,0) = 1.;
        A(i*3+0,1) = 0.;
        A(i*3+0,2) = 0.;

        A(i*3+0,3) = -currentTransf(0,0);
        A(i*3+0,4) = -currentTransf(0,1);
        A(i*3+0,5) = -currentTransf(0,2);

        A(i*3+1,0) = 0.;
        A(i*3+1,1) = 1.;
        A(i*3+1,2) = 0.;

        A(i*3+1,3) = -currentTransf(1,0);
        A(i*3+1,4) = -currentTransf(1,1);
        A(i*3+1,5) = -currentTransf(1,2);

        A(i*3+2,0) = 0.;
        A(i*3+2,1) = 0.;
        A(i*3+2,2) = 1.;

        A(i*3+2,3) = -currentTransf(2,0);
        A(i*3+2,4) = -currentTransf(2,1);
        A(i*3+2,5) = -currentTransf(2,2);

        b(i*3+0) = currentTransf(0,3);
        b(i*3+1) = currentTransf(1,3);
        b(i*3+2) = currentTransf(2,3);

    }

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);

    Eigen::VectorXd pivotMat(6);
    pivotMat = svd.solve(b);

    pivot_W[0] = pivotMat[0];
    pivot_W[1] = pivotMat[1];
    pivot_W[2] = pivotMat[2];

    pivot_S[0] = pivotMat[3];
    pivot_S[1] = pivotMat[4];
    pivot_S[2] = pivotMat[5];


}

Eigen::Vector3d Pivot::getPivot_W(){return pivot_W;}
Eigen::Vector3d Pivot::getPivot_S(){return pivot_S;}

PosList Pivot::getErr()
{
    PosList err(Tsw.size());

    Eigen::VectorXd dist(Tsw.size());

    for (unsigned int i=0;i<Tsw.size();i++)
    {
        //Calcul du point dans le referenciel de l'emetteur ( Pe = Q Pr + T )
        Eigen::Vector3d Pe=Tsw[i].Q._transformVector(pivot_S)+Tsw[i].P;

        //On compare au point pivot dans E calculé
        Eigen::Vector3d err_vec=Pe-pivot_W;

        err[i]=err_vec;
        dist[i]=err_vec.norm();
    }

    return err;
}

double Pivot::getRMS()
{
    Eigen::VectorXd dist(Tsw.size());

    for (unsigned int i=0;i<Tsw.size();i++)
    {
        //Calcul du point dans le referenciel de l'emetteur ( Pe = Q Pr + T )
        Eigen::Vector3d Pe=Tsw[i].Q._transformVector(pivot_S)+Tsw[i].P;

        //On compare au point pivot dans E calculé
        Eigen::Vector3d err_vec=Pe-pivot_W;

        dist[i]=err_vec.norm();
    }

    return 1e3*dist.norm()/sqrt(Tsw.size());
}

}
