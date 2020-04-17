#ifndef SURFACEDATAMODIFIER_H
#define SURFACEDATAMODIFIER_H

#include <QtDataVisualization/q3dsurface.h>
#include <QtDataVisualization/qabstract3dseries.h>
#include <QtGui/QFont>
#include "Cloud.h"
#include <iostream>
#include <algorithm>


using namespace QtDataVisualization;

class SurfaceDataModifier : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceDataModifier(Q3DSurface* scatter);
    ~SurfaceDataModifier();

    void setData(const CloudScalar& cloud);

    void changeStyle();
    void changePresetCamera();
    void changeLabelStyle();
    void changeFont(const QFont& font);
    void changeFontSize(int fontsize);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void setSmoothDots(int smooth);
    void start();

    float getXMin()
    {
        return m_graph->axisX()->min();
    }
    float getXMax()
    {
        return m_graph->axisX()->max();
    }
    float getYMin()
    {
        return m_graph->axisY()->min();
    }
    float getYMax()
    {
        return m_graph->axisY()->max();
    }
    float getZMin()
    {
        return m_graph->axisZ()->min();
    }
    float getZMax()
    {
        return m_graph->axisZ()->max();
    }

public Q_SLOTS:
    void changeStyle(int style);
    void changeTheme(int theme);
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality shadowQuality);

    void setXMin(double xmin);
    void setXMax(double xmax);
    void setYMin(double ymin);
    void setYMax(double ymax);
    void setZMin(double zmin);
    void setZMax(double zmax);

Q_SIGNALS:
    void backgroundEnabledChanged(bool enabled);
    void gridEnabledChanged(bool enabled);
    void shadowQualityChanged(int quality);
    void fontChanged(QFont font);

private:
    Q3DSurface* m_graph;
    int m_fontSize;
    QAbstract3DSeries::Mesh m_style;
    bool m_smooth;
};

#endif
