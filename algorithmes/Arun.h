#include <Eigen/Dense>
#include <vector>
#include <iostream>

namespace algorithms
{

Eigen::Vector3d getArunCentroid(std::vector<Eigen::Vector3d> P);
std::vector<Eigen::Vector3d> substractArunCentroid(const std::vector<Eigen::Vector3d> &P, const Eigen::Vector3d &C);
Eigen::Matrix3d getArunH(const std::vector<Eigen::Vector3d> & Q,const std::vector<Eigen::Vector3d> & Q_prime);

//Fonction principale :  mininimize E=P_prime[i]-(r*P[i]+t);
bool computeArunTranform(const std::vector<Eigen::Vector3d> & P_prime, const std::vector<Eigen::Vector3d> & P, Eigen::Matrix3d & r, Eigen::Vector3d & t);

//Test & resultats
Eigen::VectorXd getArunError(const std::vector<Eigen::Vector3d> & P_prime, const std::vector<Eigen::Vector3d> & P,const Eigen::Matrix3d & r,const Eigen::Vector3d & t);
double getArunRMS(const std::vector<Eigen::Vector3d> & P_prime, const std::vector<Eigen::Vector3d> & P, const Eigen::Matrix3d &r, const Eigen::Vector3d &t);
double getArunRMSQuat(const std::vector<Eigen::Vector3d> & P_prime, const std::vector<Eigen::Vector3d> & P, const Eigen::Quaterniond &q, const Eigen::Vector3d &t);
void testArun();

}
