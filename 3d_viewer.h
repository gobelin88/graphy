#include <Q3DScatter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QFontComboBox>
#include <QDoubleSpinBox>
#include <QSplitter>

#include "ScatterModifier.h"
#include "SurfaceModifier.h"
#include "Cloud.h"

class ViewerScatter3D:public QWidget
{
public:
    ViewerScatter3D();

    void set_data(const Cloud & cloud);
    void set_data(const CloudTransform & cloud);

private:
    ScatterDataModifier *modifier;

    QDoubleSpinBox * sb_xmin;
    QDoubleSpinBox * sb_xmax;
    QDoubleSpinBox * sb_ymin;
    QDoubleSpinBox * sb_ymax;
    QDoubleSpinBox * sb_zmin;
    QDoubleSpinBox * sb_zmax;
    QDoubleSpinBox * scatterSize;
};

class ViewerSurface3D:public QWidget
{
public:
    ViewerSurface3D();

    void set_data(const Cloud & cloud);

private:
    SurfaceDataModifier *modifier;

    QDoubleSpinBox * sb_xmin;
    QDoubleSpinBox * sb_xmax;
    QDoubleSpinBox * sb_ymin;
    QDoubleSpinBox * sb_ymax;
    QDoubleSpinBox * sb_zmin;
    QDoubleSpinBox * sb_zmax;    
};
