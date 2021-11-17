#include <QVector>
#include <QVector3D>
#include <QQuaternion>
#include <Eigen/Dense>

#include "MyCustomPlot.h"
#include "shapes/Shape.hpp"

#ifndef CLOUD_H
#define CLOUD_H

/////////////////////////////////////////////////////////////////////////////////////////////
class Cloud
{
public:
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

    ~Cloud();

    void setName(QString name);
    QString getName();

    void operator=(const Cloud& other);

    //Data
    std::vector<Eigen::Vector3d> positions()const;
    const std::vector<Eigen::Vector4d> & data()const;
    int size() const;

    QCPRange getXRange();
    QCPRange getYRange();
    QCPRange getZRange();
    QCPRange getScalarFieldRange();
    std::vector<QRgb>& getColors();
    QRgb getCustomColor(){return customColor;}
    void setCustomColor(QRgb _customColor){this->customColor=_customColor;}
    bool isCustomColorUsed(){return useCustomColor;}
    void setUseCustomColor(bool value){useCustomColor=value;}
    void setGradientPreset(QCPColorGradient::GradientPreset preset);
    const QCPColorGradient& getGradient();
    QCPColorGradient::GradientPreset getGradientPreset();
    Eigen::Vector3d getCenter();
    QByteArray getBuffer(QCPRange range);

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

    std::vector<Eigen::Vector4d> pts;
    std::vector<QRgb> colors;

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

    void init();
    void calcRanges();
};

#endif
