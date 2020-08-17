#include "1d_viewer.h"

#include "curve2d_gainphase.h"

#ifndef BODE_VIEWER_H
#define BODE_VIEWER_H


class ViewerBode:public QWidget
{
    Q_OBJECT
public:
    ViewerBode(Curve2D* sharedBuf, const QMap<QString, QKeySequence>& shortcuts_map, QWidget* parent);

public slots:
    void slot_add_data(const Curve2DModulePhase& curve);
    void slot_fit_rl();
    void slot_fit_rlc();
    void slot_fit_rlpc();
    void slot_fit_ralpcprb();
    void slot_fit_rlcapcb();

protected:
    QVector<Curve2DModulePhase> getCurves();

    QTabWidget * tab;
    Viewer1D * modules_viewer;
    Viewer1D * phases_viewer;
    QVector<Curve2DModulePhase> curves;

    //Menu
    void createFitImpedanceMenu();
    QMenu * menuFitImpedance;
    QAction * actFitRL;
    QAction * actFitRLC;
    QAction * actFitRLpC;
    QAction * actFitRaLpCpRb;
    QAction * actFitRLCapCb;
};

#endif // BODE_VIEWER_H
