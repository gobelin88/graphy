#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QTabWidget>
#include <QMdiArea>

#include "1d_viewer.h"
#include "3d_viewer.h"
#include "bode_viewer.h"
#include "MyGradientComboBox.h"
#include "MyTableModel.h"
#include "MyTableView.h"
#include "FFT.h"
#include "WinCPURAM.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setArgs(QStringList args);

public slots:


    //IO & edition
    void receivedMessage(int instanceId, QByteArray message);

    void slot_filter();
    void slot_new();
    void slot_open();
    void slot_save();
    void slot_save_as();
    void slot_export();
    void direct_open_args();
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

    //Progress
    void slot_progress(int t);
    void slot_what(QString what);
    void slot_error(QString what,QString msg);

    //
    void slot_open_end(MyTableView * newtable);
    void slot_save_end(MyTableView * table);
    void connectTable(MyTableView * newtable);

    void slot_updateRamCpu();

private:
    //Close
    void closeEvent (QCloseEvent *event);

    //Error
    void error(QString title,QString msg);

    //View & Gui
    QProgressBar * pb_bar;
    QLabel * l_what;
    QLabel * l_ramcpu;
    QTimer * t_ramcpu;
    WinCPURAM winRamCpu;

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

    //1.0 //Basic graphs
    //2.0 //Add lots of graph facilities
    //3.0 //Add Copy/Paste management between graphs and table
    //4.0 //Bigs files support
    //5.0 //complex support !
    //6.0 //multithread and multifiles tab + console
    //7.0 //remove 2d viewer map fully integrated + customisation facilities + error handling
    //8.0 //specials functions support + Direct data acces + string functions + improve save time
    //9.0 //newton solver + items configurables
    //10.0//Bars de progression (ProgressHandler) + correction de bugs + fit ellisoid + threading.
    //11.0//Cmake + grosses correction de bugs

    //Helpers
    void plot_xy(Curve2D::CurveType type);

    //misc
    int fileAlreadyOpened(QString filename);
    QStringList args;
};

#endif // MAINWINDOW_H
