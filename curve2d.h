#ifndef CURVE2D_H
#define CURVE2D_H

#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
#include <QVector>
#include "ShapeFit.h"
#include "qcustomplot.h"
#include "FIR.h"
#include "tabledata.h"

class Curve2D
{
public:
    struct Style
    {
        Style()
        {
            brush.setStyle(Qt::BrushStyle::NoBrush);
            pen.setColor(QColor(0,0,0));
            pen.setStyle(Qt::SolidLine);
            mLineStyle=QCPGraph::lsLine;
            mScatterStyle=QCPScatterStyle::ssNone;
            mScatterSize=10;
        }

        QPen pen;
        QBrush brush;
        int mLineStyle;
        int mScatterStyle;
        int mScatterSize;
    };

    enum CurveType
    {
        GRAPH=0,
        CURVE=1
    };

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

    Eigen::VectorXd getX()const;
    Eigen::VectorXd getY()const;
    QVector<double> getQX()const;
    QVector<double> getQY()const;

    //Optional scalar field
    void setScalarField(const Eigen::VectorXd& s);
    Eigen::VectorXd getScalarField()const;
    QVector<double> getQScalarField()const;

    //Optional scalar field
    void setAlphaField(const Eigen::VectorXd& a);
    Eigen::VectorXd getAlphaField()const;
    QVector<double> getQAlphaField()const;

    //Optional labelField
    void setLabelsField(const QVector<QString>& l);
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
    Curve2D getFFT();
    double getRms();
    double guessMainFrequency();
    uint getMaxIndex();
    Eigen::Vector2d getBarycenter();
    Eigen::VectorXd getLinX(int n);
    void getLinXY(int n, Eigen::VectorXd& valuesX, Eigen::VectorXd& valuesY);

    static Eigen::VectorXd buildX(int sz);

    void operator=(const Curve2D& other);
    void fromQCP(const QCPCurve* other);
    void fromQCP(const QCPGraph* other);

    QCPGraph* toQCPGraph(QCustomPlot* plot) const;
    QCPCurve* toQCPCurve(QCustomPlot* plot) const;

    Style& getStyle()
    {
        return style;
    }

private:
    Eigen::VectorXd x;
    Eigen::VectorXd y;
    Eigen::VectorXd s;
    Eigen::VectorXd a;
    QVector<QString> l;
    QString legendname;
    CurveType type;



    Style style;
};

#endif // CURVE2D_H
