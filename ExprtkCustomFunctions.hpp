//#include <cstdio>
// #include <cstdlib>
// #include <ctime>
// #include <string>

#include "exprtk/exprtk.hpp"
#include <qmath.h>
#include <iomanip>
#include <string>
#include <QStringList>

#ifndef EXPRTKCUSTOMFUNCTIONS_HPP
#define EXPRTKCUSTOMFUNCTIONS_HPP

//Noise management


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
            if(p_data->rows()>i && p_data->cols()>j)
            {
                return (*p_data)(static_cast<unsigned int> (i.real()),
                                 static_cast<unsigned int> (j.real())).toComplex();
            }
            else
            {
                //std::cout<<"data out index"<<std::endl;
                return 0.0;
            }
        }
        else
        {
            //std::cout<<"nullptr"<<std::endl;
            return 0.0;
        }
    }

    const Tdata * p_data;
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
struct zetaFunction : public exprtk::ifunction<T>
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

            int n=1.0;
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

 solveNewtonFunction():exprtk::igeneric_function<T>("SST|SSTT")
 {
     symbolsTable.add_variable("z",z);
     f_exp.register_symbol_table(symbolsTable);
     fp_exp.register_symbol_table(symbolsTable);

     ok_f =false;
     ok_fp=false;
     a=1.0;
 }

 inline T operator()(const std::size_t& ps_index,parameter_list_t parameters)
 {
    //if (ps_index > 1) return T(ps_index);
    Q_UNUSED(ps_index)

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
            }

         }
         else if (generic_type::e_scalar == gt.type)
         {
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
         }
    }

    ////////////////////////////////////
    //Perform newton iterations
    ////////////////////////////////////
    if(ok_f && ok_fp)
    {
        z=z0;
//        std::cout<<"-----------"<<std::endl;
//        std::cout<<"z0="<<z0.real()<<" "<<z0.imag()<<std::endl;
//        std::cout<<"f.value()="<<f_exp.value()<<" fp="<<fp_exp.value()<<std::endl;

        T dz=0.0;
        double p=1e-7;

        do
        {
            dz=-a*f_exp.value()/fp_exp.value();
            z+=dz;
        }
        while(std::abs(dz)>p);

//        std::cout<<"z="<<z.real()<<" "<<z.imag()<<std::endl;

        return z;
    }
    else
    {
        return -1;
    }

    return T(0);
 }

 void setVariablesNamesPtr(const QStringList * p_variablesNames)
 {
     this->p_variablesNames=p_variablesNames;
 }

 bool ok_f,ok_fp;
 std::string f_Str;
 std::string fp_Str;
 T z0,z,a;
 //
 exprtk::parser<T> parser;
 exprtk::symbol_table<T> symbolsTable;
 exprtk::expression<T> f_exp;
 exprtk::expression<T> fp_exp;

};

#endif // EXPRTKCUSTOMFUNCTIONS_HPP
