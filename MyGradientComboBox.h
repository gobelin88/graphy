#ifndef QGRADIENTCOMBOBOX_H
#define QGRADIENTCOMBOBOX_H

#include <QComboBox>
#include "MyCustomPlot.h"
#include <Eigen/Dense>

class MyGradientComboBox : public QComboBox
{
public:
    MyGradientComboBox(QWidget * parent);
    std::vector<QRgb> colourize(const Eigen::VectorXd & v);
    std::vector<QRgb> colourize(const Eigen::VectorXd & v,const QCPRange & range);
    std::vector<QRgb> colourize(const Eigen::VectorXd & v,int preset_index);

    QPixmap getPixmap(QSize size, int preset_index);
};

#endif // QGRADIENTCOMBOBOX_H
