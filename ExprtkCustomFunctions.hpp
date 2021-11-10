//#include <cstdio>
// #include <cstdlib>
// #include <ctime>
// #include <string>

#include "exprtk/exprtk.hpp"
#include <qmath.h>
#include <iomanip>
#include <string>
#include <QStringList>
#include "omp.h"
#include "solidHarmonics/solidHarmonics.h"

#ifndef EXPRTKCUSTOMFUNCTIONS_HPP
#define EXPRTKCUSTOMFUNCTIONS_HPP

template <typename T>
struct isPrimeFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    isPrimeFunction()
        : exprtk::ifunction<T>(1)
    {
        exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& n)
    {
        int N=n.real();
        int sqrtN=sqrt(N)+1;
        std::cout<<sqrtN<<std::endl;
        for(int i=2;i<sqrtN;i++)
        {
            if((N%i)==0)
            {
                return 0.0;
            }
        }

        return 1.0;
    }

};


template <typename T>
struct uniformFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    uniformFunction()
        : exprtk::ifunction<T>(2)
    {
        noise_uniform=std::uniform_real<double>(0,1.0);
        //exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& a, const T& b)
    {
        return (b-a)*noise_uniform(generator)+a;
    }

    std::default_random_engine generator;
    std::uniform_real<double> noise_uniform;
};

template <typename T>
struct normalFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    normalFunction()
        : exprtk::ifunction<T>(2)
    {
        noise_normal=std::normal_distribution<double>(0,1.0);
        //exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& mu, const T& sigma)
    {
        return sigma*noise_normal(generator)+mu;
    }

    std::default_random_engine generator;
    std::normal_distribution<double> noise_normal;
};

template <typename T>
struct linFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    linFunction()
        : exprtk::ifunction<T>(2)
    {
        //exprtk::disable_has_side_effects(*this);
        rows=nullptr;
        current_rows=nullptr;
    }

    void setNumberOfRowsPtr(T * rows)
    {
        this->rows=rows;
    }
    void setCurrentRowPtr(T * current_rows)
    {
        this->current_rows=current_rows;
    }

    inline T operator()(const T& a, const T& b)
    {
        if(current_rows && rows)
        {
            return (b-a)*(*current_rows)/(*rows-1.0)+a;
        }
        else
        {
            return 0.0;
        }
    }

    T * rows;
    T * current_rows;
};

template <typename T>
struct clinFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    clinFunction()
        : exprtk::ifunction<T>(2)
    {
        //exprtk::disable_has_side_effects(*this);
        rows=nullptr;
        current_rows=nullptr;
    }

    void setNumberOfRowsPtr(T * rows)
    {
        this->rows=rows;
    }
    void setCurrentRowPtr(T * current_rows)
    {
        this->current_rows=current_rows;
    }

    inline T operator()(const T& a, const T& b)
    {
        if(current_rows && rows)
        {
            int N=sqrt( (*rows).real() );
            int N_current=(*current_rows).real();

            double Xa=a.real();
            double Ya=a.imag();
            double Xb=b.real();
            double Yb=b.imag();

            return std::complex<double>( ((N_current)%N)*(Xb-Xa)/(N-1)+Xa,( (N_current/N)*(Yb-Ya)/(N-1)+Ya) );
        }
        else
        {
            return 0.0;
        }
    }

    T * rows;
    T * current_rows;
};

template <typename T,typename Tdata>
struct dataFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    dataFunction()
        : exprtk::ifunction<T>(2)
    {
        //exprtk::disable_has_side_effects(*this);
        p_data=nullptr;
    }

    void setDataPtr(const Tdata * data)
    {
        this->p_data=data;
    }

    inline T operator()(const T& i, const T& j)
    {
        if(p_data)
        {
            if( i.real()<p_data->rows() && j.real()<p_data->cols() && i.real()>=0 && j.real()>=0)
            {
                return (*p_data)(static_cast<unsigned int> (i.real()),
                                 static_cast<unsigned int> (j.real())).toComplex();
            }
            else
            {
                return 0.0;
            }
        }
        else
        {
            return 0.0;
        }
    }

    const Tdata * p_data;
};

template <typename T,typename Tdata>
struct cwiseFunction : public exprtk::igeneric_function<T>
{
    typedef typename exprtk::igeneric_function<T>::parameter_list_t
                                                   parameter_list_t;

    typedef typename generic_type::string_view string_t;

    using exprtk::igeneric_function<T>::operator();

    cwiseFunction()
        : exprtk::igeneric_function<T>("SS")
    {
        //exprtk::disable_has_side_effects(*this);
        p_data=nullptr;
        p_variablesNames=nullptr;
        currentCol=false;
    }

    void setDataPtr(const Tdata * data)
    {
        this->p_data=data;
    }

    void reset(unsigned int currentCol)
    {
        this->currentCol=currentCol;
        if(p_data)
        {
            if(firstEvaluation.size()!=p_data->cols())
            {
                firstEvaluation.resize(p_data->cols(),true);
                previousResult.resize(p_data->cols(),T(0,0));
            }
            else
            {
                firstEvaluation[currentCol]=true;
                previousResult[currentCol]=T(0,0);
            }
        }
        else
        {
            std::cout<<"reset error no data ptr"<<std::endl;
        }
    }

    inline T operator()(parameter_list_t parameters)
    {
        if(currentCol<firstEvaluation.size())
        {
            if(firstEvaluation[currentCol])
            {
                firstEvaluation[currentCol]=false;

                if(p_data && p_variablesNames && parameters.size()==2)
                {
                    generic_type& gt_var = parameters[0];
                    generic_type& gt_op = parameters[1];
                    T result;

                    if (generic_type::e_string == gt_op.type &&
                        generic_type::e_string == gt_var.type)
                    {
                        string_t optName(gt_op);
                        string_t variableName(gt_var);

                        QString qOptName=QString::fromStdString(to_str(optName));
                        unsigned int cindex=p_variablesNames->indexOf(QString::fromStdString(to_str(variableName)));

                        if( cindex<p_data->cols() && cindex>=0)
                        {
                            if(qOptName==QString("sum"))
                            {
                                T sum(0.0,0.0);
                                for(int i=0;i<p_data->rows();i++)
                                {
                                    sum+=(*p_data)(i,cindex).toComplex();
                                }
                                result=sum;
                            }
                            else if(qOptName==QString("mean"))
                            {
                                T sum(0.0,0.0);
                                for(int i=0;i<p_data->rows();i++)
                                {
                                    sum+=(*p_data)(i,cindex).toComplex();
                                }
                                result=sum/double(p_data->rows());
                            }
                            else if(qOptName==QString("max"))
                            {
                                T max(-DBL_MAX,-DBL_MAX);
                                for(int i=0;i<p_data->rows();i++)
                                {
                                    auto v=(*p_data)(i,cindex).toComplex();
                                    if (v.real()>max.real())
                                    {
                                        max.real(v.real());
                                    }
                                    else if (v.imag()>max.imag())
                                    {
                                        max.imag(v.imag());
                                    }
                                }
                                result=max;
                            }
                            else if(qOptName==QString("min"))
                            {
                                T min(DBL_MAX,DBL_MAX);
                                for(int i=0;i<p_data->rows();i++)
                                {
                                    auto v=(*p_data)(i,cindex).toComplex();
                                    if (v.real()<min.real())
                                    {
                                        min.real(v.real());
                                    }
                                    else if (v.imag()<min.imag())
                                    {
                                        min.imag(v.imag());
                                    }
                                }
                                result=min;
                            }
                            else if(qOptName==QString("std"))
                            {
                                double N=double(p_data->rows());
                                T sumA(0.0,0.0);
                                T sumB(0.0,0.0);
                                for(int i=0;i<p_data->rows();i++)
                                {
                                    std::complex<double> x=(*p_data)(i,cindex).toComplex();

                                    sumA+=x;
                                    sumB+=std::complex<double>( x.real()*x.real(),
                                                                x.imag()*x.imag());
                                }

                                result=sqrt( sumB/N - std::complex<double>(sumA.real()*sumA.real()/(N*N),sumA.imag()*sumA.imag()/(N*N)) );
                            }


                        }
                        else
                        {
                            result=T(0.0);
                        }
                        previousResult[currentCol]=result;
                        return result;
                    }
                    else
                    {
                        return 0.0;
                    }
                }
                else
                {
                    return 0.0;
                }
            }
            else
            {
                return previousResult[currentCol];
            }            
        }
        else
        {
            return 0.0;
        }
    }

    void setVariablesNamesPtr(const QStringList * p_variablesNames)
    {
        this->p_variablesNames=p_variablesNames;
    }

    const QStringList * p_variablesNames;

    const Tdata * p_data;
    std::vector<bool> firstEvaluation;
    std::vector<T> previousResult;

    unsigned int currentCol;
};

template <typename T>
struct gammaFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    gammaFunction()
        : exprtk::ifunction<T>(1)
    {
        exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& s)
    {
        //Hadamar weierstrass
        //      const double euler_mascheroni=0.5772156649015328606;
        //      T value= exp(-euler_mascheroni*s)/s;
        //      for(int k=1;k<100000;k++)
        //      {
        //          T s_k=s/double(k);
        //          value*=exp(s_k)/(1.0+s_k);
        //      }
        //      return value;

        //Lancoz approx
        const double p [] = {676.5203681218851
                             ,-1259.1392167224028
                             ,771.32342877765313
                             ,-176.61502916214059
                             ,12.507343278686905
                             ,-0.13857109526572012
                             ,9.9843695780195716e-6
                             ,1.5056327351493116e-7};

        if (s.real() < 0.5)
        {
            return M_PI / (sin(M_PI * s) * this->operator()(1.0 - s));// Reflection formula
        }
        else
        {
            T x = 0.99999999999980993;
            for(int i=0;i<8;i++)
            {
                x =x + p[i] / ( (s-1.0) + double(i) + 1.0);
            }
            T t = (s-1.0) + 8.0 - 0.5;
            return sqrt(2.0 * M_PI) * std::pow(t,(s-1.0) + 0.5) * exp(-t) * x;
        }
    }


};


template <typename T>
struct  zetaFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    zetaFunction()
        : exprtk::ifunction<T>(1)
    {
        exprtk::disable_has_side_effects(*this);

        dtab=new double[N+1];
        for(int n=0;n<=N;n++)
        {
            dtab[n]=d(n,N);
            //std::cout<<std::setprecision(10)<<" dtab["<<n<<"]"<<dtab[n]<<std::endl;
        }

        //std::cout<<std::setprecision(10)<<"zeta( 2)="<<operator()(2 )<<" "<<M_PI*M_PI/6.0<<std::endl;
        //std::cout<<std::setprecision(10)<<"zeta(-1)="<<operator()(-1)<<" "<<-1.0/12<<std::endl;
    }

    inline T operator()(const T& s)
    {
        if(s.real()>0.5)
        {
            ///by eta function
//            T eta=1.0;
//            double precision=1e-3;
//            T deta;
//            unsigned int n=2.0;
//            do
//            {
//               deta=std::pow(n,-s);
//               eta+=(n%2==0)?-deta:deta;
//               n++;
//            }
//            while(std::norm(deta)>(precision*precision));
//            return eta/(1.0-std::pow(2.0,1.0-s));

            ///by eta function + acceleration
            T eta=0.0;
            T deta;

            int n=1;
            do
            {
               deta=std::pow(n,-s)*dtab[n];//d(n,N);
               eta+=(n%2==0)?-deta:deta;
               n++;
            }
            while(n<=N);

            return eta/((1.0-std::pow(2.0,1.0-s))*dtab[0]);

        }
        else
        {
            return 2.0*std::pow(2.0*M_PI,s-1.0)*sin(s*M_PI*0.5)*m_gamma(1.0-s)*this->operator()(1.0-s);
        }
    }

    double d(double k,int n)
    {
        double sum=0.0;

        for(int j=k;j<=n;j++)
        {
            sum=sum+std::pow(4.0,j)*m_gamma(j+n).real()/(m_gamma(2*j+1).real()*m_gamma(-j+n+1).real());
        }

        return n*sum;
    }

    gammaFunction<T> m_gamma;

    const int N=20;
    double * dtab;
};

template <typename T>
struct xsiFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    xsiFunction()
        : exprtk::ifunction<T>(1)
    {
        exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& s)
    {
        return (s-1.0)*std::pow(M_PI,-s/2.0)*m_gamma(s/2.0+1.0)*m_zeta(s);
    }

    zetaFunction<T> m_zeta;
    gammaFunction<T> m_gamma;
};

template <typename T>
struct solidHarmonicsRegularFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    solidHarmonicsRegularFunction()
        : exprtk::ifunction<T>(5)
    {
        exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& x,const T& y,const T& z,const T& l,const T& m)
    {
        return T(solidHarmonicsR(x.real(),y.real(),z.real(),
                               std::round(l.real()),std::round(m.real())));
    }
};

template <typename T>
struct solidHarmonicsIrregularFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    solidHarmonicsIrregularFunction()
        : exprtk::ifunction<T>(5)
    {
        exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& x,const T& y,const T& z,const T& l,const T& m)
    {
        return T(solidHarmonicsI(x.real(),y.real(),z.real(),
                               std::round(l.real()),std::round(m.real())));
    }
};

template <typename T>
struct sphericalHarmonicsFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    sphericalHarmonicsFunction()
        : exprtk::ifunction<T>(4)
    {
        exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& theta,const T& phi,const T& l,const T& m)
    {
        return T(sphericalHarmonicsY(theta.real(),phi.real(),std::round(l.real()),std::round(m.real())));
    }
};

template <typename T>
struct legendrePolynomeFunction : public exprtk::ifunction<T>
{
    using exprtk::ifunction<T>::operator();

    legendrePolynomeFunction()
        : exprtk::ifunction<T>(3)
    {
        exprtk::disable_has_side_effects(*this);
    }

    inline T operator()(const T& z,const T& l,const T& m)
    {
        return T(legendrePolynomeP(z.real(),std::round(l.real()),std::round(m.real())));
    }
};



template <typename T>
struct indexFunction : public exprtk::igeneric_function<T>
{
 typedef typename exprtk::igeneric_function<T>::parameter_list_t
                                                parameter_list_t;

 typedef typename generic_type::string_view string_t;

 indexFunction():exprtk::igeneric_function<T>("S")
 {
     p_variablesNames=nullptr;
 }

 inline T operator()(parameter_list_t parameters)
 {
    for (std::size_t i = 0; i < parameters.size(); ++i)
    {
        if(p_variablesNames!=nullptr)
        {
            generic_type& gt = parameters[i];

             if (generic_type::e_string == gt.type)
             {
                string_t variableName(gt);

                //std::cout<<to_str(variableName)<<std::endl;

                return T(p_variablesNames->indexOf(QString::fromStdString(to_str(variableName))));
             }
        }
    }

    return T(0);
 }

 void setVariablesNamesPtr(const QStringList * p_variablesNames)
 {
     this->p_variablesNames=p_variablesNames;
 }

 const QStringList * p_variablesNames;
};

template <typename T>
struct solveNewtonFunction : public exprtk::igeneric_function<T>
{
 typedef typename exprtk::igeneric_function<T>::parameter_list_t
                                                parameter_list_t;

 typedef typename generic_type::string_view string_t;
 typedef typename generic_type::scalar_view scalar_t;

 solveNewtonFunction():exprtk::igeneric_function<T>("SST|SSTT|SSTTT|"\
                                                    "STT|STTT|STTTT")
 {
     symbolsTable.add_variable("z",z);
     symbolsTable.add_function("gamma"  ,  cf_gamma);
     symbolsTable.add_function("zeta"  ,  cf_zeta);
     symbolsTable.add_function("xsi"  ,  cf_xsi);

     f_exp.register_symbol_table(symbolsTable);
     fp_exp.register_symbol_table(symbolsTable);

     ok_f =false;
     ok_fp=false;
     finite_diff=false;
     a=1.0;
     epsilon=1.0;

     omp_init_lock(&writelock);
 }

 inline T operator()(const std::size_t& ps_index,parameter_list_t parameters)
 {
     Q_UNUSED(ps_index)
     int it=100;

     omp_set_lock(&writelock);


    for (std::size_t i = 0; i < parameters.size(); ++i)
    {
        generic_type& gt = parameters[i];

         if (generic_type::e_string == gt.type)
         {
            string_t variableName(gt);

            if(i==0)
            {
                std::string f_Str_n=to_str(variableName);
                if(f_Str_n!=f_Str)
                {
                    //compile
                    f_Str=f_Str_n;
                    ok_f=parser.compile(f_Str,f_exp);
                    if(!ok_f)
                    {
                        std::cout<<parser.error()<<std::endl;
                    }
                }
            }
            else if(i==1)
            {
                std::string fp_Str_n=to_str(variableName);
                if(fp_Str_n!=fp_Str)
                {
                    //compile
                    fp_Str=fp_Str_n;
                    ok_fp=parser.compile(fp_Str,fp_exp);
                    if(!ok_fp)
                    {
                        std::cout<<parser.error()<<std::endl;
                    }
                }
                finite_diff=false;
            }

         }
         else if (generic_type::e_scalar == gt.type)
         {
            if(i==1)
            {
                scalar_t variableName(gt);
                epsilon=variableName();
                finite_diff=true;
            }
            if(i==2)
            {
                scalar_t variableName(gt);
                z0=variableName();
                a=1;
            }
            else if(i==3)
            {
                scalar_t variableName(gt);
                a=variableName();
            }
            else if(i==4)
            {
                scalar_t variableName(gt);
                it=std::abs(variableName());
            }
         }
    }

    ////////////////////////////////////
    //Perform newton iterations
    ////////////////////////////////////
    if(ok_f && (ok_fp || finite_diff))
    {
        z=z0;
//        std::cout<<"-----------"<<std::endl;
//        std::cout<<"z0="<<z0.real()<<" "<<z0.imag()<<std::endl;
//        std::cout<<"f.value()="<<f_exp.value()<<" fp="<<fp_exp.value()<<std::endl;

        for(int i=0;i<it;i++)
        {
            if(finite_diff)
            {
                T z_prev=z;//centrale diff for quadratique convengency
                z=z_prev+epsilon;
                T fp=f_exp.value();
                z=z_prev-epsilon;
                T fm=f_exp.value();
                z=z_prev;

                z-=a*f_exp.value()/(fp-fm)*2.0*epsilon;

                if(std::isnan(z.real())||std::isnan(z.imag())||std::isinf(z.real())||std::isinf(z.imag()))
                {
                    omp_unset_lock(&writelock);
                    return z_prev;
                }
            }
            else
            {
                z-=a*f_exp.value()/fp_exp.value();
            }
        }

//        std::cout<<"z="<<z.real()<<" "<<z.imag()<<std::endl;

        omp_unset_lock(&writelock);
        return z;
    }

    omp_unset_lock(&writelock);
    return T(0);
 }

 bool ok_f,ok_fp,finite_diff;
 std::string f_Str;
 std::string fp_Str;
 T z0,z,a;
 T epsilon;

 //
 exprtk::parser<T> parser;
 exprtk::symbol_table<T> symbolsTable;
 exprtk::expression<T> f_exp;
 exprtk::expression<T> fp_exp;

 gammaFunction<T> cf_gamma;
 zetaFunction<T> cf_zeta;
 xsiFunction<T> cf_xsi;

 omp_lock_t writelock;
};

#endif // EXPRTKCUSTOMFUNCTIONS_HPP
