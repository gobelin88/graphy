#include "1d_viewer.h"

#include "2d_curveComplex.h"

#ifndef BODE_VIEWER_H
#define BODE_VIEWER_H


class ViewerBode:public QWidget
{
    Q_OBJECT
public:
    ViewerBode(const QMap<QString, QKeySequence>& shortcuts_map, QWidget* parent);

public slots:
    void slot_add_data(const Curve2DComplex& curve);
    void slot_fit_rl();
    void slot_fit_rlc();
    void slot_fit_rlpc();
    void slot_fit_ralpcprb();
    void slot_fit_rlcapcb();

signals:
    void sig_newColumn(QString,Eigen::VectorXd);
    void sig_displayResults(QString);

protected:
    QVector<Curve2DComplex> getCurves();

    QTabWidget * tab;
    Viewer1D * modules_viewer;
    Viewer1D * phases_viewer;
    Viewer1D * real_viewer;
    Viewer1D * imag_viewer;
    QVector<Curve2DComplex> curves;

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
