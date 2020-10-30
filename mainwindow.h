#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QTabWidget>
#include <QMdiArea>

#include "1d_viewer.h"
#include "2d_viewer.h"
#include "bode_viewer.h"
#include "view3d.h"
#include "qgradientcombobox.h"
#include "mytablemodel.h"
#include "mytableview.h"


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
    void slot_new();
    void slot_open();
    void slot_save();
    void slot_save_as();
    void slot_export();
    void direct_open(QString filename);
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
    void slot_plot_gain_phase();

    //data
    void slot_select();
    void slot_parameters();
    void slot_results(QString results);
    void slot_colourize();
    void fileModified();

private:
    //Close
    void closeEvent (QCloseEvent *event);

    //Error
    void error(QString title,QString msg);

    //View & Gui
    QTextEdit* te_results;
    QTabWidget* te_widget;
    Ui::MainWindow* ui;
    MyTableView * table;
    Viewer1D* createViewerId();
    QAction* a_newColumn;
    QAction* a_newRow;
    QAction* a_newRows;
    QAction* a_delete;
    QAction* a_removeColumnsRows;
    QAction* a_updateColumns;
    QAction* a_copy;
    QAction* a_paste;
    QMdiArea* mdiArea;

    //Io
    bool isModified;
    QString current_filename;
    void setCurrentFilename(QString filename);

    //Plots
    Curve2D shared;

    //Shortcuts
    QMap<QString,QKeySequence> shortcuts;
    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);
    bool loadShortcuts();
    void saveShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);

    //Constants
    const float graphyVersion=4.5f;
};

#endif // MAINWINDOW_H
