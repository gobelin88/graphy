#ifndef FFTDIALOG_H
#define FFTDIALOG_H

#include <QDialog>
#include <Eigen/Dense>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>

class MyFFT
{
public:
    enum FFTWindowsType
    {
        RECTANGLE,
        BLACKMAN,
        HANN,
        FLAT_TOP,
    };

    static Eigen::VectorXcd MyFFT::getFFT(Eigen::VectorXcd s_in,
                                     FFTWindowsType fft_mode,
                                     bool normalize_flag,
                                     bool halfspectrum,
                                     bool inverse);
};

//-------------------------------------------------------------------
class FFTDialog:public QDialog
{
    Q_OBJECT
public:
    FFTDialog();

    MyFFT::FFTWindowsType getWindowsType();
    bool isNormalized();
    bool isInverse();
    double getFe();

public slots:
    void setFormula(bool inverse);
    void setWindowsFormula(int type);
    void setWindowsFormula(MyFFT::FFTWindowsType type);
    void setNormalizedFormula(bool normalize);

public:
    QCheckBox* cb_normalize;
    QCheckBox* cb_inverse;
    QDoubleSpinBox* sb_fe;
    QComboBox* cb_windowType;

    QLabel* labelFormula;
    QLabel* labelWindowFormula;
    QLabel* labelNormalized;
};

#endif // FFTDIALOG_H
