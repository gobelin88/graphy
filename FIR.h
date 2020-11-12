#include <vector>
#include <complex>
#include <deque>
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
#include <iostream>
#include <fstream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#ifndef FIR_H
#define FIR_H

/**
 * @class FIR
 * @brief Classe d'implémentation d'un FIR
 */
class FIR
{
public:    
    /**
     * @brief FIR
     */
    FIR();

    /**
     * @brief process
     * @param sample
     */
    void process(double sample);

    /**
     * @brief get
     * @return
     */
    double get();

    /**
     * @brief reset
     */
    void reset();

    /**
     * @brief save
     * @param filename
     */
    void save(char* filename);

    /**
     * @brief load
     * @param filename
     */
    void load(char* filename);

    /**
     * @brief getFFT
     * @return
     */
    std::vector<std::complex<double> > getFFT();

    /**
     * @brief save_fft
     * @param filename
     * @param fe
     * @param fmax
     */
    void save_fft(const char *filename, double fe, double fmax);

    /**
     * @brief getKaiserBesselCoef
     * @param N
     * @param fc_fe
     */
    void getKaiserBesselCoef(int N, double fc_fe);

    /**
     * @brief getLeastSquareCoef
     * @param fp_fe
     * @param fs_fe
     * @param N
     * @param K
     */
    void getLeastSquareCoef(double fp_fe, double fs_fe, unsigned int N, double K);

    /**
     * @brief getBlackmanNuttallCoef
     * @param N
     */
    void getBlackmanNuttallCoef(unsigned int N);
    /**
     * @brief getBlackmanCoef
     * @param N
     */
    void getBlackmanCoef(unsigned int N);

    /**
     * @brief getFlatTopCoef
     * @param N
     */
    void getFlatTopCoef(unsigned int N);

    /**
     * @brief getHannCoef
     * @param N
     */
    void getHannCoef(unsigned int N);

    /**
     * @brief getRectCoef
     * @param N
     */
    void getRectCoef(unsigned int N);

    /**
     * @brief at
     * @param i
     * @return
     */
    inline double at(unsigned int i){return coefs[i];}

    /**
     * @brief at
     * @param i
     * @return
     */
    inline double mean()
    {
        double meanv=0.0;
        for(unsigned int i=0;i<coefs.size();i++)
        {
            meanv+=coefs[i];
        }
        return meanv;
    }

    /**
     * @brief sumCoef
     * @return
     */
    double sumCoef();

    /**
     * @brief mul
     * @param gain
     */
    void mul(double gain);

    /**
     * @brief normalizeCoefs
     */
    void normalizeCoefs();
protected:
    std::vector<double> coefs;
    std::deque<double> hist;
    double fact(double n);
    double bessel_i(int N,int n,double y);
    double sinc(double x);
};

#endif // FIR_H
