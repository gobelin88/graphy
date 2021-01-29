#include "FFT.h"
#include <unsupported/Eigen/FFT>
#include <QGridLayout>
#include <QDialogButtonBox>
#include "FIR.h"
#include <QGroupBox>

Eigen::VectorXcd MyFFT::getFFT(Eigen::VectorXcd s_in,
                               WindowsType fft_mode,
                               bool normalize_flag,
                               bool symetrical_convention,
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

    //Convention symétrique pour la fft so that energy is conserved between representations.
    double factor=1.0;
    if(symetrical_convention)
    {
        if(inverse)
        {
            factor=sqrtN;     //conventionA 1/N ---> 1/sqrt(N)
        }
        else
        {
            factor=1.0/sqrtN ; //conventionA 1   ---> 1/sqrt(N)
        }
    }

    Eigen::FFT<double> fft;
    if(halfspectrum)
    {
        fft.SetFlag(Eigen::FFT<double>::HalfSpectrum);
    }

    Eigen::VectorXcd s_out(outN);

    //apply windows
    if(inverse)
    {
        s_in*=factor;
        fft.inv(s_out,s_in);
        for (unsigned int i=0; i<N; i++)
        {
            if(std::abs(win.at(i))>DBL_EPSILON)
            {
                s_out[i]=s_out[i]/win.at(i);
            }
            else
            {
                s_out[i]=0.0;
            }
        }
    }
    else
    {
        for (unsigned int i=0; i<N; i++){s_in[i]=s_in[i]*factor*win.at(i);}
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
    cb_normalize->setToolTip("Does nothing in case of RECTANGLE windows. Preserve signal dynamics.");
    cb_normalize->setChecked(false);
    setNormalizedFormula(cb_normalize->isChecked());

    cb_symetrical=new QCheckBox("Symetrical");
    cb_symetrical->setToolTip("If non-symetrical convention is choosed, energy of representations is not conserved.\n"\
                              "And FFT is not a unitary operator.");
    cb_symetrical->setChecked(true);

    cb_inverse=new QCheckBox("Inverse");
    cb_inverse->setToolTip("Compute inverse FFT");
    cb_inverse->setChecked(false);
    setFormula(cb_inverse->isChecked());

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    //Formulas
    QGridLayout* gboxFormula = new QGridLayout();
    gboxFormula->addWidget(labelFormula, 0,0);
    gboxFormula->addWidget(labelWindowFormula, 1,0);
    gboxFormula->addWidget(labelNormalized, 2,0);

    //Params
    QGroupBox *gb_fft_parameters=new QGroupBox("Parameters");
    QGridLayout * layout_parameters=new QGridLayout;
    layout_parameters->addWidget(new QLabel("Windows type : "),0,0);
    layout_parameters->addWidget(cb_windowType,0,1,1,2);
    layout_parameters->addWidget(new QLabel("Sample frequency : "),1,0);
    layout_parameters->addWidget(sb_fe,1,1,1,2);
    layout_parameters->addWidget(cb_normalize,2,0);
    layout_parameters->addWidget(cb_inverse,2,1);
    layout_parameters->addWidget(cb_symetrical,2,2);
    gb_fft_parameters->setLayout(layout_parameters);

    gbox->addLayout(gboxFormula, 0,0);
    gbox->addWidget(gb_fft_parameters,1,0);
    gbox->addWidget(buttonBox,2,0);

    this->setLayout(gbox);

    QObject::connect(cb_inverse, SIGNAL(toggled(bool)), this, SLOT(setFormula(bool)));
    QObject::connect(cb_symetrical, SIGNAL(toggled(bool)), this, SLOT(setFormula(bool)));

    QObject::connect(cb_normalize, SIGNAL(toggled(bool)), this, SLOT(setNormalizedFormula(bool)));
    QObject::connect(cb_windowType, SIGNAL(currentIndexChanged(int)), this, SLOT(setWindowsFormula(int)));
}

MyFFT::WindowsType FFTDialog::getWindowsType()
{
    return static_cast<MyFFT::WindowsType>(cb_windowType->currentIndex());
}

bool FFTDialog::isSymetrical()
{
    return cb_symetrical->isChecked();
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

void FFTDialog::setFormula(bool value)
{
    Q_UNUSED(value);
    if(cb_symetrical->isChecked())
    {
        if(cb_inverse->isChecked())
        {
            labelFormula->setPixmap(QPixmap(":/eqn/eqn/invfft.gif"));
        }
        else
        {
            labelFormula->setPixmap(QPixmap(":/eqn/eqn/fft.gif"));
        }
    }
    else
    {
        if(cb_inverse->isChecked())
        {
            labelFormula->setPixmap(QPixmap(":/eqn/eqn/invfft_asym.gif"));
        }
        else
        {
            labelFormula->setPixmap(QPixmap(":/eqn/eqn/fft_asym.gif"));
        }
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

