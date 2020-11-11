#ifndef VIEWER2D_H
#define VIEWER2D_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <iostream>
#include <QAction>
#include <QMenu>
#include <QFileInfo>
#include <QFileDialog>

#include "boxplot.h"
#include "qcustomplot.h"
#include "tabledata.h"
#include "qgradientcombobox.h"

class Viewer2D:public QCustomPlot
{
    Q_OBJECT
public:
    Viewer2D();
    ~Viewer2D();

    void createPopup();

    void setRange(QCPRange range_x, QCPRange range_y);
    void resetRange();

public slots:
    void slot_setData(const Eigen::VectorXd &dataX, const Eigen::VectorXd &dataY, const Eigen::VectorXd &dataZ, const Resolution& box);
    void slot_saveImage();
    void slot_setGradient(int preset);
    void slot_rescale();
    void slot_setParameters();
    void slot_updateData();

    void slot_setKnn(int value);
    void slot_setMode(int mode);

protected:
    QVector<double> extract(std::vector<QVector<double>> v,int id);

    void mousePressEvent(QMouseEvent* event);

    QCPMarginGroup* marginGroup;
    QCPColorScale* colorScale;
    QCPColorMap* colorMap;

    QString current_filename;
    QMenu* popup_menu;
    QAction* actSave;
    QAction* actRescale;
    QAction* actParameters;


    InterpolationMode mode;
    int knn;

    Eigen::VectorXd dataX;
    Eigen::VectorXd dataY;
    Eigen::VectorXd dataZ;

    Resolution box;
    QCPColorGradient::GradientPreset currentgradient;
};

#endif // IMAGE_VIEWER_H
