#include <QVector>
#include <QVector3D>
#include <QQuaternion>
#include <Eigen/Dense>

#include "MyCustomPlot.h"
#include "shapes/Shape.hpp"

#ifndef CLOUD_H
#define CLOUD_H

/////////////////////////////////////////////////////////////////////////////////////////////
struct DataCloudNode
{
    Eigen::Vector3d position;
    Eigen::Quaterniond attitude;
    double scalar;
};

QDataStream & operator>>(QDataStream & ds,std::vector<DataCloudNode> & nodes);
QDataStream & operator<<(QDataStream & ds,std::vector<DataCloudNode> & nodes);

class Cloud
{

public:

    enum Type
    {
        TYPE_POINTS,
        TYPE_TRANSFORMS,
    };

    Cloud();

    Cloud(std::vector<Eigen::Vector3d> plist,
          QString labelX,
          QString labelY,
          QString labelZ);

    Cloud(std::vector<Eigen::Vector3d> plist,
          const Eigen::VectorXd& scalarField,
          QString labelX,
          QString labelY,
          QString labelZ,
          QString labelS);

    Cloud(std::vector<Eigen::Vector4d> plist,
          QString labelX,
          QString labelY,
          QString labelZ,
          QString labelS);

    Cloud(const Eigen::VectorXd& x,
          const Eigen::VectorXd& y,
          const Eigen::VectorXd& z,
          QString labelX,
          QString labelY,
          QString labelZ);

    Cloud(const Eigen::VectorXd& x,
          const Eigen::VectorXd& y,
          const Eigen::VectorXd& z,
          const Eigen::VectorXd& scalarField,
          QString labelX,
          QString labelY,
          QString labelZ,
          QString labelS);

    Cloud(const Eigen::VectorXd& x,
          const Eigen::VectorXd& y,
          const Eigen::VectorXd& z,
          const std::vector<Eigen::Quaterniond> & attitudes,
          QString labelX,
          QString labelY,
          QString labelZ);

    Cloud(const Eigen::VectorXd& x,
          const Eigen::VectorXd& y,
          const Eigen::VectorXd& z,
          const Eigen::VectorXd& qw,
          const Eigen::VectorXd& qx,
          const Eigen::VectorXd& qy,
          const Eigen::VectorXd& qz,
          QString labelX,
          QString labelY,
          QString labelZ);

    ~Cloud();

    void setName(QString name);
    QString getName();

    void operator=(const Cloud& other);

    //Data
    std::vector<Eigen::Vector3d> positions()const;
    std::vector<Eigen::Vector4d> positionsAndScalarField()const;
    const std::vector<DataCloudNode> & data()const;

    int size() const;

    QCPRange getXRange();
    QCPRange getYRange();
    QCPRange getZRange();
    QCPRange getScalarFieldRange();
    QRgb getCustomColor(){return customColor;}
    void setCustomColor(QRgb _customColor){this->customColor=_customColor;}

    bool isCustomColorUsed(){return useCustomColor;}
    void setUseCustomColor(bool value){useCustomColor=value;}
    Type type(){return cloudType;}

    void setGradientPreset(QCPColorGradient::GradientPreset preset);
    const QCPColorGradient& getGradient();
    QCPColorGradient::GradientPreset getGradientPreset();
    Eigen::Vector3d getCenter();

    QByteArray getBuffer(QCPRange range);
    QByteArray getBufferTransfos(double size=1.0);

    QString getLabelX();
    QString getLabelY();
    QString getLabelZ();
    QString getLabelS();

    //Fit a model
    void fit(Shape<Eigen::Vector3d>* model, int it=10000,double xtol=-1);
    void fit(Shape<Eigen::Vector4d>* model, int it=10000,double xtol=-1);
    void project(Shape<Eigen::Vector3d>* model);

    //Modificateurs
    void subSample(unsigned int nbPoints);
    void move(const Eigen::Matrix3d & R, const Eigen::Vector3d & T, double scale);

    //Get
    void getBarycenterAndBoundingRadius(Eigen::Vector3d & Barycenter,double & boundingRadius);
    Eigen::Vector3d getBarycenter();

    //I/O interface
    QByteArray toByteArray();
    void fromByteArray(QByteArray data);

private:

    //QCPRange getRange(const Eigen::VectorXd& v);
    std::vector<DataCloudNode> nodes;

    QCPColorGradient gradient;

    QCPRange rangeX,rangeY,rangeZ,rangeS;
    QString labelX ;
    QString labelY ;
    QString labelZ ;
    QString labelS ;
    QString name;


    QCPColorGradient::GradientPreset gradientPreset;

    QRgb customColor;
    bool useCustomColor;
    Type cloudType;

    void init();
    void calcRanges();
};

#endif
