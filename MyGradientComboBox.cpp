#include "MyGradientComboBox.h"

QGradientComboBox::QGradientComboBox(QWidget * parent):QComboBox(parent)
{
    addItem("Grayscale",QCPColorGradient::GradientPreset::gpGrayscale);
    addItem("GreenRed",QCPColorGradient::GradientPreset::gpGreenRed);
    addItem("BlueWhiteRed",QCPColorGradient::GradientPreset::gpBlueWhiteRed);
    addItem("Hot",QCPColorGradient::GradientPreset::gpHot);
    addItem("Cold",QCPColorGradient::GradientPreset::gpCold);
    addItem("Night",QCPColorGradient::GradientPreset::gpNight);
    addItem("Candy",QCPColorGradient::GradientPreset::gpCandy);
    addItem("Geography",QCPColorGradient::GradientPreset::gpGeography);
    addItem("Ion",QCPColorGradient::GradientPreset::gpIon);
    addItem("Thermal",QCPColorGradient::GradientPreset::gpThermal);
    addItem("Polar",QCPColorGradient::GradientPreset::gpPolar);
    addItem("Spectrum",QCPColorGradient::GradientPreset::gpSpectrum);
    addItem("Jet",QCPColorGradient::GradientPreset::gpJet);
    addItem("Hues",QCPColorGradient::GradientPreset::gpHues);

    QSize icon_size(256,16);
    this->setIconSize(icon_size);

    for(int k=0;k<this->count();k++)
    {
        QIcon icon(getPixmap(icon_size,k));
        this->setItemIcon(k,icon);
    }
}

QPixmap QGradientComboBox::getPixmap(QSize size,int preset_index)
{
    int w=size.width();
    int h=size.height();

    QImage image(w,h, QImage::Format_RGB32);

    Eigen::VectorXd v=Eigen::VectorXd::LinSpaced(w,1,w);
    std::vector<QRgb> colors=colourize(v,preset_index);

    for(int i=0;i<w;i++)
    {
        for(int j=0;j<h;j++)
        {
            image.setPixel(i,j,colors[i]);
        }
    }

    return QPixmap::fromImage(image);
}

std::vector<QRgb> QGradientComboBox::colourize(const Eigen::VectorXd & v)
{
    QCPRange range(v.minCoeff(),v.maxCoeff());
    std::vector<QRgb> colors;
    QCPColorGradient gradient(static_cast<QCPColorGradient::GradientPreset>(this->currentIndex()));

    colors.resize(v.rows());
    gradient.colorize(v.data(),range,colors.data(),v.size());

    return colors;
}

std::vector<QRgb> QGradientComboBox::colourize(const Eigen::VectorXd & v,const QCPRange & range)
{
    std::vector<QRgb> colors;
    QCPColorGradient gradient(static_cast<QCPColorGradient::GradientPreset>(this->currentIndex()));

    colors.resize(v.rows());
    gradient.colorize(v.data(),range,colors.data(),v.size());

    return colors;
}

std::vector<QRgb> QGradientComboBox::colourize(const Eigen::VectorXd & v,int preset_index)
{
    QCPRange range(v.minCoeff(),v.maxCoeff());
    std::vector<QRgb> colors;
    QCPColorGradient gradient(static_cast<QCPColorGradient::GradientPreset>(preset_index));

    colors.resize(v.rows());
    gradient.colorize(v.data(),range,colors.data(),v.size());

    return colors;
}
