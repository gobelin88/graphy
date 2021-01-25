#include "FIR.h"

FIR::FIR()
{
    reset();
}

void FIR::process(double sample)
{
    hist.push_back(sample);
    hist.pop_front();
}

double FIR::get()
{
    double sum=0.0;
    for (unsigned int i=0; i<coefs.size(); i++)
    {
        sum+=hist[i]*coefs[i];
    }
    return sum;
}

void FIR::reset()
{
    for (unsigned int i=0; i<hist.size(); i++)
    {
        hist[i]=0;
    }
}

////////////////////////////////////////////////////SAVE/LOAD
void FIR::save(char* filename)
{
    std::ofstream os(filename);
    os<<coefs.size()<<std::endl;
    for (unsigned int k=0; k<coefs.size(); k++)
    {
        os<<coefs[k]<<std::endl;
    }
    os.close();
}

void FIR::load(char* filename)
{
    int s=0;
    std::ifstream os(filename);
    os>>s;
    coefs.resize(s,0.0);
    hist.resize(s,0.0);
    for (unsigned int k=0; k<coefs.size(); k++)
    {
        os>>coefs[k];
    }
    os.close();
}

std::vector<std::complex<double> > FIR::getFFT()
{
    std::vector<std::complex<double> >fft;
    Eigen::FFT<double> fft_algo;
    std::vector<double> padding(coefs.size()*10,0.0);
    for (unsigned int i=0; i<coefs.size(); i++)
    {
        padding[i]=coefs[i];
    }

    fft.resize(padding.size(),std::complex<double>(0,0));
    fft_algo.fwd(fft.data(),padding.data(),padding.size());
    return fft;
}

void FIR::save_fft(const char* filename,double fe,double fmax)
{
    std::ofstream os(filename);
    std::vector< std::complex<double> > fft=getFFT();
    for (unsigned int k=0; k<fft.size()/2; k++)
    {
        double f=(k*fe)/fft.size();
        if (f>fmax)
        {
            return;
        }
        os<<f<<";"<<20*log10(std::abs(fft[k]))<<std::endl;
    }
    os.close();
}

/////////////////////////////////////////////////////COEF
void FIR::getKaiserBesselCoef(int N,double fc_fe)
{
    coefs.clear();
    coefs.resize(N,0.0);
    std::vector<double> tmp_coef(N,0.0);

    double beta=7.85726;

    int mid = (N-1)/2;
    double gain=0;

    for (int n=-mid; n<=mid; n++)
    {
        double t    = sqrt(1.-pow(((2.*n)/(N)),2));
        double num  = bessel_i(30,0,beta*t);
        double den  = bessel_i(30,0,beta);
        tmp_coef[n+mid] = 2*fc_fe*sinc(2*M_PI*fc_fe*n)*num/den;  // source fen
        gain        += tmp_coef[n+mid];
    }
    for (unsigned int i=0; i<coefs.size(); i++)
    {
        coefs[i]=tmp_coef[i]/gain;
    }
    hist.resize(coefs.size(),0.0);
    reset();
}

void FIR::getLeastSquareCoef(double fp_fe,double fs_fe,unsigned int N,double K)
{
    unsigned int M=((N-1)/2);
    unsigned int Mp=M+1;

    //Construct q(k)
    Eigen::VectorXd q(2*Mp);
    for (unsigned int k=0; k<2*Mp; k++)
    {
        if (k==0)
        {
            q[k]=fp_fe+K*(1-fs_fe);
        }
        else
        {
            q[k]=fp_fe*sinc(M_PI*fp_fe*k)-K*fs_fe*sinc(M_PI*fs_fe*k);
        }
    }

    //Construct Q1 Q2 Q
    Eigen::MatrixXd Q1(Mp,Mp),Q2(Mp,Mp),Q(Mp,Mp);
    for (unsigned int k=0; k<Mp; k++)
    {
        for (unsigned int n=0; n<Mp; n++)
        {
            Q1(k,n)=q(std::max<unsigned int>(k,n)-std::min<unsigned int>(k,n));
            Q2(k,n)=q(k+n);
        }
    }
    Q=0.5*(Q1+Q2);

    //Construct B
    Eigen::VectorXd B(Mp);
    for (unsigned int k=0; k<Mp; k++)
    {
        B[k]=fp_fe*sinc(M_PI*fp_fe*k);
    }

    //Solve linear syst
    Eigen::VectorXd A(Mp);
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(Q, Eigen::ComputeThinU | Eigen::ComputeThinV);
    A=svd.solve(B);

    //Build coef
    coefs.resize(2*M+1,0.0);
    for (unsigned int k=0; k<M; k++)
    {
        coefs[k]=A[Mp-k-1]*0.5;   //0 --> M-1     //Mp-1 --> 1
    }
    coefs[M]=A[0];                               //M             //0
    for (unsigned int k=1; k<M+1; k++)
    {
        coefs[k+M]=A[k]*0.5;   //M+1 --> 2M+1  //1 --> Mp-1
    }

    hist.resize(coefs.size(),0.0);
    reset();
}
///////////////////:
void FIR::getBlackmanNuttallCoef(unsigned int N)
{
    coefs.resize(N,0.0);

    for (unsigned int i=0; i<N; i++)
    {
        coefs[i]=0.3635819-0.4891775*cos( (2.0*M_PI*i)/N)+0.1365995*cos((4.0*M_PI*i)/N)-0.0106411*cos((6.0*M_PI*i)/N);
    }

    reset();
}

void FIR::getBlackmanCoef(unsigned int N)
{
    coefs.resize(N,0.0);

    for (unsigned int i=0; i<N; i++)
    {
        coefs[i]=(0.42-0.5*cos( (2.0*M_PI*i)/N)+0.08*cos((4.0*M_PI*i)/N));
    }

    reset();
}

void FIR::getFlatTopCoef(unsigned int N)
{
    coefs.resize(N,0.0);

    for (unsigned int i=0; i<N; i++)
    {
        coefs[i]=0.21557895-0.41663158*cos( (2.0*M_PI*i)/N)+0.277263158*cos((4.0*M_PI*i)/N)-0.083578947*cos((6.0*M_PI*i)/N)+0.006947368*cos((8.0*M_PI*i)/N);
    }

    reset();
}

void FIR::getHannCoef(unsigned int N)
{
    coefs.resize(N,0.0);

    for (unsigned int i=0; i<N; i++)
    {
        coefs[i]=0.5*(1.0-cos((2.0*M_PI*i)/N));
    }

    reset();
}

void FIR::getRectCoef(unsigned int N)
{
    coefs.resize(N,1.0);

    reset();
}

void FIR::mul(double gain)
{
    for (unsigned int i=0; i<coefs.size(); i++)
    {
        coefs[i]*=gain;
    }
}

////PRIVATE
double FIR::fact(double n)
{
    return (n==1 || n==0)?1:(n*fact(n-1));
}

double FIR::bessel_i(int N,int n,double y)
{
    double sum=0.0;
    for (int m=0; m<N; m++)
    {
        sum+= pow((0.25*y*y),m)/(fact(m)*fact(m+n));
    }
    return sum*pow(0.5*y,n);
}

double FIR::sinc(double x)
{
    if (x==0.0)
    {
        return 1.0;
    }
    return sin(x)/x;
}

void FIR::normalizeCoefs()
{
    double sum=0.0;
    for (unsigned int i=0; i<coefs.size(); i++)
    {
        sum+=coefs[i];
    }

    if (sum!=0)
    {
        for (unsigned int i=0; i<coefs.size(); i++)
        {
            coefs[i]*=static_cast<double>(coefs.size())/sum;
        }
    }
}

double FIR::sumCoef()
{
    double sum=0.0;
    for (unsigned int i=0; i<coefs.size(); i++)
    {
        sum+=coefs[i];
    }
    return sum;
}
