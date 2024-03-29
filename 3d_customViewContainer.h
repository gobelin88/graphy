#ifndef CUSTOMVIEWCONTAINER_H
#define CUSTOMVIEWCONTAINER_H

#include <QWidget>
#include <MyCustomPlot.h>

class CustomViewContainer: public QWidget
{
    Q_OBJECT
public:
    CustomViewContainer(QWidget* container,QWidget*parent,Qt::WindowFlags flags);
    ~CustomViewContainer();

    QWidget* getContainer();
    QCPColorScale* getColorScale();
    QCustomPlot* getColorScalePlot();

    QCPAxis* getXAxis();
    QCPAxis* getYAxis();
    QCPAxis* getZAxis();
    QCPAxis* getAxis(int axisIndex);

    QVector3D getTranslation();
    QVector3D getScale();
    QVector3D getScaleInv();

    QListWidget * getSelectionView();

    void replot();

public slots:
    void slot_fullscreen(bool checked);
    void slot_onItemDoubleClicked(QListWidgetItem *item);
    void slot_onAxisDoubleClicked(QCPAxis* axis);

signals:
    void sig_itemDoubleClicked(int itemIndex);
    void sig_axisDoubleClicked(int axisIndex);

private:
    //----------------------------

    QWidget* container;

    void createColorAxisPlot();
    QCustomPlot* color_plot;
    QCPColorScale* scale;

    void createXAxisPlot();
    QCustomPlot * axisX_plot;
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

    QListWidget * selectionView;

    int axisSize;
    QGridLayout* glayout;

    QVector<QCPAxis *> axisList;//X Y Z S;
};

#endif // CUSTOMVIEWCONTAINER_H
