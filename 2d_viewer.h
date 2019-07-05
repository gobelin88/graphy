#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H

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
    void slot_set_data(const TableData & data, const BoxPlot &box, int knn);
    void slot_save_image();
    void setGradient(int preset);
    void slot_rescale();

protected:
    QVector<double> extract(std::vector<QVector<double>> v,int id);

    //void mousePressEvent(QMouseEvent * event);

    QCPMarginGroup * marginGroup;
    QCPColorScale * colorScale;
    QCPColorMap * colorMap;

    QString current_filename;
    QMenu * popup_menu;
    QAction * actSave;
    QAction * actRescale;
};

#endif // IMAGE_VIEWER_H
