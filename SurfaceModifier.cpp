#include "SurfaceModifier.h"
#include <QtDataVisualization/qscatterdataproxy.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qscatter3dseries.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtCore/qmath.h>
#include <QtCore/qrandom.h>
#include <QtWidgets/QComboBox>

using namespace QtDataVisualization;

//#define RANDOM_SCATTER // Uncomment this to switch to random scatter


SurfaceDataModifier::SurfaceDataModifier(Q3DSurface *scatter)
    : m_graph(scatter),
      m_fontSize(40.0f),
      m_style(QAbstract3DSeries::MeshSphere),
      m_smooth(true)
{
    m_graph->activeTheme()->setType(Q3DTheme::ThemeArmyBlue);

    QFont font = m_graph->activeTheme()->font();
    font.setPointSize(m_fontSize);
    m_graph->activeTheme()->setFont(font);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);

    m_graph->scene()->activeCamera()->setZoomLevel(180);

    QSurfaceDataProxy *proxy = new QSurfaceDataProxy;
    QSurface3DSeries *series = new QSurface3DSeries(proxy);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    series->setMeshSmooth(m_smooth);
    m_graph->addSeries(series);

    scatter->setAspectRatio(1.0);
    scatter->setHorizontalAspectRatio(1.0);
}

SurfaceDataModifier::~SurfaceDataModifier()
{
    delete m_graph;
}

void SurfaceDataModifier::setData(const Cloud & cloud)
{
    Surface surface=cloud.getExtrapolated();

    // Configure the axes according to the data
    m_graph->axisX()->setTitle("X");
    m_graph->axisY()->setTitle("Y");
    m_graph->axisZ()->setTitle("Z");

    QSurfaceDataArray * dataArray = new QSurfaceDataArray;
    QSurfaceDataRow * dataRow=new QSurfaceDataRow;

    for (int j=0;j<surface.cols();j++)
    {
        dataRow->resize(surface.rows());
        QSurfaceDataItem * ptrToDataRow = &dataRow->first();
        for (int k=0;k<surface.rows();k++)
        {
            ptrToDataRow->setPosition( QVector3D(k,j,surface(k,j)) );
            ptrToDataRow++;
        }
    }


    m_graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);
}

void SurfaceDataModifier::setXMin(double xmin){if(xmin<getXMax())m_graph->axisX()->setMin(xmin);}
void SurfaceDataModifier::setXMax(double xmax){if(xmax>getXMin())m_graph->axisX()->setMax(xmax);}
void SurfaceDataModifier::setYMin(double ymin){if(ymin<getYMax())m_graph->axisY()->setMin(ymin);}
void SurfaceDataModifier::setYMax(double ymax){if(ymax>getYMin())m_graph->axisY()->setMax(ymax);}
void SurfaceDataModifier::setZMin(double zmin){if(zmin<getZMax())m_graph->axisZ()->setMin(zmin);}
void SurfaceDataModifier::setZMax(double zmax){if(zmax>getZMin())m_graph->axisZ()->setMax(zmax);}

void SurfaceDataModifier::changeStyle(int style)
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (comboBox) {
        m_style = QAbstract3DSeries::Mesh(comboBox->itemData(style).toInt());
        if (m_graph->seriesList().size())
            m_graph->seriesList().at(0)->setMesh(m_style);
    }
}

void SurfaceDataModifier::setSmoothDots(int smooth)
{
    m_smooth = bool(smooth);
    QSurface3DSeries *series = m_graph->seriesList().at(0);
    series->setMeshSmooth(m_smooth);
}

void SurfaceDataModifier::changeTheme(int theme)
{
    Q3DTheme *currentTheme = m_graph->activeTheme();
    currentTheme->setType(Q3DTheme::Theme(theme));
    emit backgroundEnabledChanged(currentTheme->isBackgroundEnabled());
    emit gridEnabledChanged(currentTheme->isGridEnabled());
    emit fontChanged(currentTheme->font());
}

void SurfaceDataModifier::changePresetCamera()
{
    static int preset = Q3DCamera::CameraPresetFrontLow;

    m_graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);

    if (++preset > Q3DCamera::CameraPresetDirectlyBelow)
        preset = Q3DCamera::CameraPresetFrontLow;
}

void SurfaceDataModifier::changeLabelStyle()
{
    m_graph->activeTheme()->setLabelBackgroundEnabled(!m_graph->activeTheme()->isLabelBackgroundEnabled());
}

void SurfaceDataModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSizeF(m_fontSize);
    m_graph->activeTheme()->setFont(newFont);
}

void SurfaceDataModifier::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality sq)
{
    int quality = int(sq);
    emit shadowQualityChanged(quality); // connected to a checkbox in main.cpp
}

void SurfaceDataModifier::changeShadowQuality(int quality)
{
    QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
}

void SurfaceDataModifier::setBackgroundEnabled(int enabled)
{
    m_graph->activeTheme()->setBackgroundEnabled((bool)enabled);
}

void SurfaceDataModifier::setGridEnabled(int enabled)
{
    m_graph->activeTheme()->setGridEnabled((bool)enabled);
}
