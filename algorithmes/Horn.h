#include <Eigen/Dense>
#include <vector>

namespace algorithms
{

Eigen::Vector3d getHornCentroid(std::vector<Eigen::Vector3d> R);
std::vector<Eigen::Vector3d> substractHornCentroid(const std::vector<Eigen::Vector3d> &R, const Eigen::Vector3d &C);
Eigen::Matrix3d getHornM(const std::vector<Eigen::Vector3d> & Rr_prime, const std::vector<Eigen::Vector3d> & Rl_prime);
Eigen::Matrix4d getHornN(const Eigen::Matrix3d & M);
Eigen::Quaterniond getHornQuaternion(const Eigen::Matrix4d & N);
double getHornScaleSym(const std::vector<Eigen::Vector3d> & Rr_prime,const std::vector<Eigen::Vector3d> & Rl_prime);
Eigen::Vector3d getHornTranslation(const Eigen::Vector3d &Cr, const Eigen::Vector3d &Cl, double s, const Eigen::Quaterniond & q);

//Fonction principale
void computeHornTranform(const std::vector<Eigen::Vector3d> & Rr, const std::vector<Eigen::Vector3d> & Rl, Eigen::Quaterniond & q, Eigen::Vector3d & t, double &s);

//Test & resultats
void testHorn();
Eigen::VectorXd getHornError(const std::vector<Eigen::Vector3d> & Rr, const std::vector<Eigen::Vector3d> & Rl,const Eigen::Quaterniond & q,const Eigen::Vector3d & t,double s);
double getHornRMS(const std::vector<Eigen::Vector3d> & P_prime, const std::vector<Eigen::Vector3d> & P,const Eigen::Quaterniond & r,const Eigen::Vector3d & t,double s);

}
