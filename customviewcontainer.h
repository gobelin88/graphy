#ifndef CUSTOMVIEWCONTAINER_H
#define CUSTOMVIEWCONTAINER_H

#include <QWidget>
#include <qcustomplot.h>

class CustomViewContainer: public QWidget
{
    Q_OBJECT
public:
    CustomViewContainer(QWidget* container);

    QWidget* getContainer();
    QCPColorScale* getColorScale();
    QCustomPlot* getColorScalePlot();

    QCPAxis* getXAxis();
    QCPAxis* getYAxis();
    QCPAxis* getZAxis();

    QVector3D getTranslation();
    QVector3D getScale();

    void replot();


private:
    //----------------------------

    QWidget* container;

    void createColorAxisPlot();
    QCustomPlot* color_plot;
    QCPColorScale* scale;

    void createXAxisPlot();
    QCustomPlot* axisX_plot;
    QCPAxisRect* axisX_rect;
    QCPAxis* axisX;

    void createYAxisPlot();
    QCustomPlot* axisY_plot;
    QCPAxisRect* axisY_rect;
    QCPAxis* axisY;

    void createZAxisPlot();
    QCustomPlot* axisZ_plot;
    QCPAxisRect* axisZ_rect;
    QCPAxis* axisZ;

    int axisSize;
};

#endif // CUSTOMVIEWCONTAINER_H
