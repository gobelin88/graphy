#include "ScatterModifier.h"
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


ScatterDataModifier::ScatterDataModifier(Q3DScatter* scatter)
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

    QScatterDataProxy* proxy = new QScatterDataProxy;
    QScatter3DSeries* series = new QScatter3DSeries(proxy);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    series->setMeshSmooth(m_smooth);
    m_graph->addSeries(series);

    scatter->setAspectRatio(1.0);
    scatter->setHorizontalAspectRatio(1.0);
}

ScatterDataModifier::~ScatterDataModifier()
{
    delete m_graph;
}

void ScatterDataModifier::setGradient(int preset)
{
    gradient.loadPreset((QCPColorGradient::GradientPreset)preset);
}

void ScatterDataModifier::setData(const CloudScalar& cloud)
{
    // Configure the axes according to the data
    m_graph->axisX()->setTitle("X");
    m_graph->axisY()->setTitle("Y");
    m_graph->axisZ()->setTitle("Z");

    QScatterDataArray* dataArray = new QScatterDataArray;
    dataArray->resize(cloud.data().size());
    QScatterDataItem* ptrToDataArray = &dataArray->first();

    for (int k=0; k<cloud.data().size(); k++)
    {
        ptrToDataArray->setPosition(cloud.data()[k]);
        ptrToDataArray++;
    }

    m_graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);
    m_graph->seriesList().at(0)->setBaseColor(QColor(0,255,0));
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleUniform);
}

void ScatterDataModifier::setXMin(double xmin)
{
    if (xmin<getXMax())
    {
        m_graph->axisX()->setMin(xmin);
    }
}
void ScatterDataModifier::setXMax(double xmax)
{
    if (xmax>getXMin())
    {
        m_graph->axisX()->setMax(xmax);
    }
}
void ScatterDataModifier::setYMin(double ymin)
{
    if (ymin<getYMax())
    {
        m_graph->axisY()->setMin(ymin);
    }
}
void ScatterDataModifier::setYMax(double ymax)
{
    if (ymax>getYMin())
    {
        m_graph->axisY()->setMax(ymax);
    }
}
void ScatterDataModifier::setZMin(double zmin)
{
    if (zmin<getZMax())
    {
        m_graph->axisZ()->setMin(zmin);
    }
}
void ScatterDataModifier::setZMax(double zmax)
{
    if (zmax>getZMin())
    {
        m_graph->axisZ()->setMax(zmax);
    }
}

void ScatterDataModifier::changeStyle(int style)
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox)
    {
        m_style = QAbstract3DSeries::Mesh(comboBox->itemData(style).toInt());
        if (m_graph->seriesList().size())
        {
            if (m_style==QAbstract3DSeries::MeshUserDefined)
            {
                if (m_graph->seriesList().at(0)->userDefinedMesh().isEmpty())
                {
                    setUserDefinedMesh();
                }
                else
                {
                    m_graph->seriesList().at(0)->setMesh(m_style);
                }
            }
            else
            {
                m_graph->seriesList().at(0)->setMesh(m_style);
            }
        }
    }
}

void ScatterDataModifier::setSize(double size)
{
    if (m_graph->seriesList().size())
    {
        m_graph->seriesList().at(0)->setItemSize((float)size);
    }
}

void ScatterDataModifier::setSmoothDots(int smooth)
{
    m_smooth = bool(smooth);
    QScatter3DSeries* series = m_graph->seriesList().at(0);
    series->setMeshSmooth(m_smooth);
}

void ScatterDataModifier::setUserDefinedMesh()
{
    QString filename=QFileDialog::getOpenFileName(nullptr,"Custom Mesh","","*.obj");

    if (!filename.isEmpty())
    {
        QScatter3DSeries* series = m_graph->seriesList().at(0);
        series->setUserDefinedMesh(filename);

        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshUserDefined);
    }
}

void ScatterDataModifier::changeTheme(int theme)
{
    Q3DTheme* currentTheme = m_graph->activeTheme();
    currentTheme->setType(Q3DTheme::Theme(theme));
    emit backgroundEnabledChanged(currentTheme->isBackgroundEnabled());
    emit gridEnabledChanged(currentTheme->isGridEnabled());
    emit fontChanged(currentTheme->font());
}

void ScatterDataModifier::changePresetCamera()
{
    static int preset = Q3DCamera::CameraPresetFrontLow;

    m_graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);

    if (++preset > Q3DCamera::CameraPresetDirectlyBelow)
    {
        preset = Q3DCamera::CameraPresetFrontLow;
    }
}

void ScatterDataModifier::changeLabelStyle()
{
    m_graph->activeTheme()->setLabelBackgroundEnabled(!m_graph->activeTheme()->isLabelBackgroundEnabled());
}

void ScatterDataModifier::changeFont(const QFont& font)
{
    QFont newFont = font;
    newFont.setPointSizeF(m_fontSize);
    m_graph->activeTheme()->setFont(newFont);
}

void ScatterDataModifier::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality sq)
{
    int quality = int(sq);
    emit shadowQualityChanged(quality); // connected to a checkbox in main.cpp
}

void ScatterDataModifier::changeShadowQuality(int quality)
{
    QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
}

void ScatterDataModifier::setBackgroundEnabled(int enabled)
{
    m_graph->activeTheme()->setBackgroundEnabled((bool)enabled);
}

void ScatterDataModifier::setGridEnabled(int enabled)
{
    m_graph->activeTheme()->setGridEnabled((bool)enabled);
}
