#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QTabWidget>
#include <QMdiArea>

#include "1d_viewer.h"
#include "2d_viewer.h"
#include "3d_viewer.h"
#include "bode_viewer.h"
#include "MyGradientComboBox.h"
#include "mytablemodel.h"
#include "MyTableView.h"
#include "FFT.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    //IO & edition
    void receivedMessage(int instanceId, QByteArray message);

    void slot_filter();
    void slot_new();
    void slot_open();
    void slot_save();
    void slot_save_as();
    void slot_export();
    void direct_open(QStringList filenames);
    void direct_save(QString filename);

    //Graphs
    void slot_plot_y();
    void slot_plot_graph_xy();
    void slot_plot_curve_xy();
    void slot_plot_fft();
    void slot_plot_histogram();
    void slot_plot_cloud_2D();
    void slot_plot_field_2D();
    void slot_plot_map_2D();
    void slot_plot_cloud_3D();
    void slot_plot_bode();

    //data
    void slot_select();
    void slot_parameters();
    void slot_results(QString results);
    void slot_colourize();
    void slot_currentTableModified();

    //tab
    void closeCurrentTable();
    void closeTable(int index);
    void slot_tab_moved(int from,int to);

    //term
    void slot_showHideTerminal();

private:
    //Close
    void closeEvent (QCloseEvent *event);

    //Error
    void error(QString title,QString msg);

    //View & Gui
    QTextEdit* te_results;
    QMdiSubWindow * subWindowsResults;
    QTabWidget* te_widget;
    Ui::MainWindow* ui;

    Viewer1D* createViewer1D(int sx=600,int sy=400);
    ViewerBode* createViewerBode();

    QMdiArea* mdiArea;

    //Shortcuts
    QMap<QString,QKeySequence> shortcuts;
    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);
    bool loadShortcuts();
    void saveShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);

    //Tables
    void addNewTable(MyTableView * newTable);
    QList<MyTableView*> tables;
    MyTableView * getCurrentTable();
    QString getCurrentFilename();

    //Constants
    const float graphyVersion=7.0f;
    //1.0//Basic graphs
    //2.0//Add lots of graph facilities
    //3.0//Add Copy/Paste management between graphs and table
    //4.0//Bigs files support
    //5.0//complex support !
    //6.0//multithread and multifiles tab + console
    //7.0//

    //Helpers
    void plot_xy(Curve2D::CurveType type);
};

#endif // MAINWINDOW_H
