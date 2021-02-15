#ifndef CURVE2D_H
#define CURVE2D_H

#include <Eigen/Dense>

#include <QVector>
#include "ShapeFit.h"
#include "MyCustomPlot.h"
#include "FIR.h"
#include "DataConvert.h"
#include "FFT.h"

class Curve2D
{
public:
    //-------------------------------------------------------
    struct MapParams
    {
        enum InterpolationMode
        {
            MODE_NEAREST,
            MODE_WEIGHTED
        };

        uint resolutionX;
        uint resolutionY;
        uint knn;
        InterpolationMode mode;

        MapParams()
        {
            this->resolutionX=512;
            this->resolutionY=512;
            this->knn=5;
            this->mode=MODE_NEAREST;
        }

        MapParams(uint resolutionX,
                  uint resolutionY,
                  uint knn,
                  InterpolationMode mode)
        {
            this->resolutionX=resolutionX;
            this->resolutionY=resolutionY;
            this->knn=knn;
            this->mode=mode;
        }
    };

    //-------------------------------------------------------
    struct Style
    {
        Style()
        {
            brush.setStyle(Qt::BrushStyle::NoBrush);
            pen.setColor(QColor(0,0,0));
            pen.setStyle(Qt::SolidLine);
            mLineStyle=QCPGraph::lsLine;
            mScatterShape=QCPScatterStyle::ssNone;
            mScatterSize=2;
            gradientType=QCPColorGradient::gpPolar;
        }

        QPen pen;
        QBrush brush;
        int mLineStyle;
        int mScatterShape;
        int mScatterSize;
        int gradientType;

        void write(QDataStream & ds)
        {
            ds<<mLineStyle;
            ds<<mScatterShape;
            ds<<mScatterSize;
            ds<<mScatterSize;
            ds<<pen;
            ds<<brush;
        }

        void read(QDataStream & ds)
        {
            ds>>mLineStyle;
            ds>>mScatterShape;
            ds>>mScatterSize;
            ds>>mScatterSize;
            ds>>pen;
            ds>>brush;
        }
    };

    enum CurveType
    {
        GRAPH=0,
        CURVE=1,
        MAP=2
    };

    //-------------------------------------------------------
    Curve2D();

    Curve2D(const Eigen::VectorXd& y,
            QString legendname,
            CurveType type);

    Curve2D(const Eigen::VectorXd& x,
            const Eigen::VectorXd& y,
            QString legendname,
            CurveType type);

    Curve2D(const Eigen::VectorXd& x,
            const Eigen::VectorXd& y,
            const Eigen::VectorXd& s,
            QString legendname,
            CurveType type);

    const Eigen::VectorXd & getX()const;
    const Eigen::VectorXd & getY()const;
    QVector<double> getQX()const;
    QVector<double> getQY()const;

    //Optional scalar field
    void setScalarField(const Eigen::VectorXd& s);
    const Eigen::VectorXd &getScalarField()const;
    QVector<double> getQScalarField()const;

    //Optional scalar field
    void setAlphaField(const Eigen::VectorXd& a);
    Eigen::VectorXd getAlphaField()const;
    QVector<double> getQAlphaField()const;

    //Optional labelField
    void setLabelsField(const QVector<QString> &l);
    QVector<QString> getLabelsField()const;

    //Fit a model
    void fit(Shape<Eigen::Vector2d>* model);

    //Fit a Polynome
    Eigen::VectorXd fit(unsigned int order);
    double at(const Eigen::VectorXd& A,double valuex);
    Eigen::VectorXd at(const Eigen::VectorXd& A,Eigen::VectorXd values);
    static QString getPolynomeString(const Eigen::VectorXd& C,unsigned int order);

    //Fit a 2d Polynome
    Eigen::VectorXd fit2d(unsigned int order);
    double at(const Eigen::VectorXd& C,double valuex,double valuey,unsigned int order);
    Eigen::VectorXd at(const Eigen::VectorXd& C, Eigen::VectorXd valuex, Eigen::VectorXd valuey, unsigned int order);
    static QString getPolynome2VString(const Eigen::VectorXd& C,unsigned int order);

    //Legend name
    QString getLegend()const;
    void setLegend(QString legendname);

    //Type
    CurveType getType() const;

    //misc
    void knnMeanDistance(int knn);
    double getRms();
    std::complex<double> guessMainFrequencyPhaseModule(double & mainFrequency);
    uint getMaxIndex();
    Eigen::Vector2d getBarycenter();
    Eigen::VectorXd getLinX(int n)const;
    void getLinXY(int n, Eigen::VectorXd& valuesX, Eigen::VectorXd& valuesY);

    static Eigen::VectorXd buildX(int sz);

    void fromByteArray(QByteArray data);
    QByteArray toByteArray();

    void operator=(const Curve2D& other);


    QCPCurve * getCurvePtr()
    {
        return ptrCurve;
    }

    QCPGraph * getGraphPtr()
    {
        return ptrGraph;
    }


    template<typename T>
    void fromQCP(T* other)
    {
        x.resize(other->data()->size());
        y.resize(other->data()->size());
        auto it =other->data()->begin();
        int i=0;
        while (it!=other->data()->end())
        {
            x[i]=it->key;
            y[i]=it->value;
            it++;
            i++;
        }

        this->a=fromQVector(other->getAlphaField());
        this->s=fromQVector(other->getScalarField());
        this->l=other->getLabelField();
        this->legendname=other->name();
        this->type= (std::is_same<QCPCurve, T>::value == true)? Curve2D::CURVE: Curve2D::GRAPH;
        this->style.mLineStyle=other->lineStyle();
        this->style.mScatterShape=other->scatterStyle().shape();
        this->style.mScatterSize=other->scatterStyle().size();
        this->style.pen=other->pen();
        this->style.brush=other->brush();
        this->style.gradientType=other->getScalarFieldGradientType();

        if(this->type==Curve2D::CURVE)
        {
            ptrCurve=reinterpret_cast<QCPCurve*>(other);
            ptrGraph=nullptr;
        }
        else
        {
            ptrCurve=nullptr;
            ptrGraph=reinterpret_cast<QCPGraph*>(other);
        }
    }

    //T = QCPColorMap
    template<typename T>
    T* toQCPMap(QCustomPlot* plot)const
    {
        T* pqcp = nullptr;

        if(std::is_same<QCPColorMap, T>::value == true)
        {
            if((getX().rows()==getY().rows()) &&
               (getX().rows()==getScalarField().rows()))
            {
                pqcp = new T(plot);

                pqcp->setColorScale(new QCPColorScale(plot));
                pqcp->colorScale()->setType(QCPAxis::atRight);
                plot->plotLayout()->addElement(0, 1, pqcp->colorScale());
                pqcp->setGradient(static_cast<QCPColorGradient::GradientPreset>(style.gradientType));

                pqcp->data()->clear();
                pqcp->data()->setSize(mapParams.resolutionX,mapParams.resolutionY);

                QCPRange rx(getX().minCoeff(),getX().maxCoeff());
                QCPRange ry(getY().minCoeff(),getY().maxCoeff());
                pqcp->data()->setRange(rx,ry);

                interpolate(getX(),getY(),getScalarField(),pqcp,mapParams.knn,mapParams.mode);

                plot->xAxis->setRange(pqcp->data()->keyRange());
                plot->yAxis->setRange(pqcp->data()->valueRange());
                pqcp->colorScale()->rescaleDataRange(true);

                pqcp->setName(getLegend());
            }
            else
            {
                std::cout<<"Error :"<<getX().rows()<<" "<<getY().rows()<<" "<<getScalarField().rows()<<std::endl;
            }
        }

        return pqcp;
    }

    //T = QCPCurve or QCPGraph
    template<typename T>
    T* toQCP(QCustomPlot* plot)const
    {
        T* pqcp = nullptr;

        if(std::is_same<QCPCurve, T>::value || std::is_same<QCPGraph, T>::value)
        {
            pqcp = new T(plot);
            pqcp->setAlphaField(getQAlphaField());
            pqcp->setScalarField(getQScalarField());
            pqcp->setLabelField(getLabelsField());
            pqcp->setScalarFieldGradientType(style.gradientType);

            pqcp->setLineStyle(static_cast<T::LineStyle>(style.mLineStyle));

            pqcp->setScatterStyle(QCPScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(style.mScatterShape), style.mScatterSize));
            pqcp->setSelectable(QCP::stWhole);
            pqcp->setSelectionDecorator(nullptr);
            pqcp->setName(getLegend());
            pqcp->setData(getQX(),getQY());
            pqcp->setPen(style.pen);
            pqcp->setBrush(style.brush);

            if (getLabelsField().size()>0)
            {
                buildX(getY().size());
                QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
                textTicker->addTicks(getQX(), getLabelsField() );
                plot->xAxis->setTicker(textTicker);
                plot->xAxis->setTickLabelColor(Qt::black);
                plot->xAxis->setLabelColor(Qt::black);
            }

            if (getScalarField().size()>0)
            {
                plot->plotLayout()->addElement(0, plot->plotLayout()->columnCount(), pqcp->getColorScale());
            }
        }

        return pqcp;
    }

    Style& getStyle()
    {
        return style;
    }

    double getMeanXWeightedByY()
    {
        double sumx=0.0,sumy=0.0;
        for (int i=0; i<x.size(); i++)
        {
            sumx+=x[i]*y[i];
            sumy+=y[i];
        }

        return sumx/sumy;
    }

    void clear();


    void updateLinkedScalarfield(QCustomPlot * plot)
    {
        if(getCurvePtr())
        {
            getCurvePtr()->setScalarField(getQScalarField());

            if ( !plot->plotLayout()->elements(false).contains(getCurvePtr()->getColorScale()) )
            {
                plot->plotLayout()->addElement(0, plot->plotLayout()->columnCount(), getCurvePtr()->getColorScale());
            }
        }
        else if(getGraphPtr())
        {
            getGraphPtr()->setScalarField(getQScalarField());

            if ( !plot->plotLayout()->elements(false).contains(getGraphPtr()->getColorScale()) )
            {
                plot->plotLayout()->addElement(0, plot->plotLayout()->columnCount(), getGraphPtr()->getColorScale());
            }
        }
    }

private:
    Eigen::VectorXd x;
    Eigen::VectorXd y;
    Eigen::VectorXd s;
    Eigen::VectorXd a;
    QVector<QString> l;
    QString legendname;
    CurveType type;

    QCPGraph * ptrGraph;
    QCPCurve * ptrCurve;

    Style style;
    MapParams mapParams;

    //Map
    static void interpolate(const Eigen::VectorXd& dataX,
                     const Eigen::VectorXd& dataY,
                     const Eigen::VectorXd& dataZ,
                     QCPColorMap* map,
                     size_t knn,
                     MapParams::InterpolationMode mode);


};

#endif // CURVE2D_H
