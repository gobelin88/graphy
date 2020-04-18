#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QFile>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QTextStream>
#include <QMessageBox>
#include <QGridLayout>
#include <QTabWidget>
#include <QVector>
#include <QMdiArea>
#include <QResizeEvent>
#include <QScrollArea>

#include "1d_viewer.h"
#include "1d_cplx_viewer.h"
#include "2d_viewer.h"
#include "3d_viewer.h"
#include "view3d.h"

#include "exprtk/exprtk.hpp"

#include <unsupported/Eigen/FFT>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();


    QVector<double> getCol(int id,const TableData& table);
    QString getColName(int id);
    int getColId(QString colName);

public slots:
    void updateTable();
    void updateTable(const QModelIndex& indexA, const QModelIndex& indexB);

    void slot_open();
    void slot_save();
    void slot_export();
    void direct_open(QString filename);
    void direct_save(QString filename);
    void direct_export(QString filename);
    void direct_new(int sx,int sy);

    void slot_newColumn();
    void slot_renameColumn();
    void slot_delColumn();

    void slot_plot_y();
    void slot_plot_graph_xy();
    void slot_plot_curve_xy();
    void slot_plot_cloud_xys();
    void slot_plot_xyz();
    void slot_plot_histogram();
    void slot_plot_cloud_3D();
    void slot_mode_changed();
    void slot_plot_gain_phase();

    void slot_plot_fft();

private:

    QString askForValidColumnName();

    void setCurrentFilename(QString filename);

    void resizeEvent(QResizeEvent* event);

    Ui::MainWindow* ui;
    QTableView* table;
    QString current_filename;
    QMdiArea* mdiArea;
    QStandardItemModel* model;
    bool hasheader;
    TableData datatable;
    Curve2D shared;
    View3D::PrimitiveMode graphMode;

    QAction* a_newColumn;
    QAction* a_renameColumn;
    QAction* a_delColumn;

    void addColumn(const QVector<double>& v);

    //expr
    bool eval(QString expression,QVector<double>& results);

    void dispVariables();
    void registerClear();
    void registerNewVariable(QString value);
    void registerDelVariable(QString value);
    void registerRenameVariable(QString old_varname,QString new_varname);

    exprtk::symbol_table<double> symbolsTable;
    QStringList variables_names;
    QLinkedList<double> variables;
};

#endif // MAINWINDOW_H
