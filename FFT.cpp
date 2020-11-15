#include "FFT.h"
#include <unsupported/Eigen/FFT>
#include <QGridLayout>
#include <QDialogButtonBox>
#include "FIR.h"

Eigen::VectorXcd MyFFT::getFFT(Eigen::VectorXcd s_in,
                               WindowsType fft_mode,
                               bool normalize_flag,
                               bool halfspectrum,
                               bool inverse)
{
    uint N=s_in.size();                //Number of inputs samples
    uint outN=(halfspectrum)?N/2:N; //Number of outputs samples depending on mode
    double sqrtN=sqrt(static_cast<double>(N));

    FIR win;
    if (fft_mode==HANN)
    {
        win.getHannCoef(N);
    }
    else if (fft_mode==BLACKMAN)
    {
        win.getBlackmanCoef(N);
    }
    else if (fft_mode==FLAT_TOP)
    {
        win.getFlatTopCoef(N);
    }
    else if (fft_mode==RECTANGLE)
    {
        win.getRectCoef(N);
    }

    if (normalize_flag==true)
    {
        win.normalizeCoefs();
    }

    //Convention symétrique pour la fft
    if(inverse)
    {
        win.mul(sqrtN);
    }
    else
    {
        win.mul(1.0/sqrtN);
    }

    //apply windows
    for (unsigned int i=0; i<N; i++){s_in[i]=s_in[i]*win.at(i);}

    Eigen::FFT<double> fft;
    if(halfspectrum)
    {
        fft.SetFlag(Eigen::FFT<double>::HalfSpectrum);
    }

    Eigen::VectorXcd s_out(outN);

    if(inverse)
    {
        fft.inv(s_out,s_in);
    }
    else
    {
        fft.fwd(s_out,s_in);
    }

    return s_out;
}

FFTDialog::FFTDialog()
{
    this->setLocale(QLocale("C"));
    this->setWindowTitle("FFT : Fast Fourier Transform parameters");
    QGridLayout* gbox = new QGridLayout();

    labelFormula=new QLabel(this);
    labelFormula->setAlignment(Qt::AlignCenter);

    labelWindowFormula=new QLabel(this);
    labelWindowFormula->setAlignment(Qt::AlignCenter );

    labelNormalized=new QLabel(this);

    labelNormalized->setAlignment(Qt::AlignCenter);

    cb_windowType=new QComboBox(this);
    cb_windowType->addItem("RECTANGLE");
    cb_windowType->addItem("BLACKMAN");
    cb_windowType->addItem("HANN");
    cb_windowType->addItem("FLAT_TOP");
    cb_windowType->setCurrentText("RECTANGLE");
    setWindowsFormula(getWindowsType());

    sb_fe=new QDoubleSpinBox(this);
    sb_fe->setPrefix("Fe=");
    sb_fe->setValue(1.0);
    sb_fe->setRange(0.0,1e100);
    sb_fe->setSuffix(" [Hz]");

    cb_normalize=new QCheckBox("Normalized");
    cb_normalize->setToolTip("Parseval theorem don't apply if normalized");
    cb_normalize->setChecked(false);
    setNormalizedFormula(cb_normalize->isChecked());

    //        QCheckBox* cb_halfspectrum=new QCheckBox("Half Spectrum");
    //        cb_halfspectrum->setToolTip("In case of reals entries spectrum is symetrical");
    //        cb_halfspectrum->setChecked(true);

    cb_inverse=new QCheckBox("Inverse");
    cb_inverse->setToolTip("Compute inverse FFT");
    cb_inverse->setChecked(false);
    setFormula(cb_inverse->isChecked());

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout* gboxFormula = new QGridLayout();

    gboxFormula->addWidget(labelFormula, 0,0);
    gboxFormula->addWidget(labelNormalized, 0,1);
    gboxFormula->addWidget(labelWindowFormula, 1,0,1,2);

    gbox->addLayout(gboxFormula, 0,0,1,2);
    gbox->addWidget(new QLabel("Windows type : "),1,0);
    gbox->addWidget(cb_windowType,1,1);

    gbox->addWidget(new QLabel("Sample frequency : "),2,0);
    gbox->addWidget(sb_fe,2,1);
    gbox->addWidget(cb_normalize,3,0);
    //gbox->addWidget(cb_halfspectrum,2,1);
    gbox->addWidget(cb_inverse,3,1);
    gbox->addWidget(buttonBox,4,0,1,2);

    this->setLayout(gbox);

    QObject::connect(cb_inverse, SIGNAL(toggled(bool)), this, SLOT(setFormula(bool)));
    QObject::connect(cb_normalize, SIGNAL(toggled(bool)), this, SLOT(setNormalizedFormula(bool)));
    QObject::connect(cb_windowType, SIGNAL(currentIndexChanged(int)), this, SLOT(setWindowsFormula(int)));
}

MyFFT::WindowsType FFTDialog::getWindowsType()
{
    return static_cast<MyFFT::WindowsType>(cb_windowType->currentIndex());
}
bool FFTDialog::isNormalized()
{
    return cb_normalize->isChecked();
}
bool FFTDialog::isInverse()
{
    return cb_inverse->isChecked();
}

double FFTDialog::getFe()
{
    return sb_fe->value();
}

void FFTDialog::setFormula(bool inverse)
{
    if(inverse)
    {
        labelFormula->setPixmap(QPixmap(":/eqn/eqn/invfft.gif"));
    }
    else
    {
        labelFormula->setPixmap(QPixmap(":/eqn/eqn/fft.gif"));
    }
}

void FFTDialog::setWindowsFormula(MyFFT::WindowsType type)
{
    if(type==MyFFT::RECTANGLE)
    {
        labelWindowFormula->setPixmap(QPixmap(":/eqn/eqn/window_rectangle.gif"));
    }
    else if(type==MyFFT::BLACKMAN)
    {
        labelWindowFormula->setPixmap(QPixmap(":/eqn/eqn/window_blackman.gif"));
    }
    else if(type==MyFFT::HANN)
    {
        labelWindowFormula->setPixmap(QPixmap(":/eqn/eqn/window_hann.gif"));
    }
    else if(type==MyFFT::FLAT_TOP)
    {
        labelWindowFormula->setPixmap(QPixmap(":/eqn/eqn/window_flattop.gif"));
    }
}

void FFTDialog::setWindowsFormula(int type)
{
    setWindowsFormula(static_cast<MyFFT::WindowsType>(type));
}

void FFTDialog::setNormalizedFormula(bool normalize)
{
    if(normalize)
    {
        labelNormalized->setPixmap(QPixmap(":/eqn/eqn/window_normalize.gif"));
    }
    else
    {
        labelNormalized->setPixmap(QPixmap(":/eqn/eqn/window_not_normalize.gif"));
    }
}

