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

#include <QDirIterator>
#include <QDir>

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
    //underlying table
    void addRowTable(QVector<double> dataRow);
    void addColTable(QVector<double> dataCol);
    void delColTable(int id);
    void delRowTable(int id);
    void updateTable();
    void updateTable(const QModelIndex& indexA, const QModelIndex& indexB);

    void slot_editColumn();
    void slot_delete();
    void slot_newRow();
    void slot_updateColumns();

    //IO
    void slot_new();
    void slot_open();
    void slot_save();
    void slot_export();
    void direct_open(QString filename);
    void direct_save(QString filename);
    void direct_export(QString filename);
    void direct_new(int sx,int sy);

    //Graphs
    void slot_plot_y();
    void slot_plot_graph_xy();
    void slot_plot_curve_xy();
    void slot_plot_cloud_xys();
    void slot_plot_xyz();
    void slot_plot_histogram();
    void slot_plot_cloud_3D();
    void slot_plot_gain_phase();
    void slot_plot_fft();

private:

    bool isValidExpression(QString variableExpression);
    bool isValidVariable(QString variableName, int currentIndex);
    bool editVariableAndExpression(int currentIndex);

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

    QAction* a_newColumn;
    QAction* a_newRow;
    QAction* a_delete;
    QAction* a_updateColumns;

    //Col/row
    void setColumn(int idCol,const QVector<QString>& vec_col);
    void addRow(const QStringList& str_row);
    void addRow(const QVector<double>& vec_row);

    //expr
    QVector<QString> evalColumn(int colId);

    void dispVariables();
    void registerClear();
    void registerNewVariable(QString varname,QString varexpr);
    void registerDelVariable(QString varname);
    void registerRenameVariable(QString old_varname, QString new_varname, QString oldExpression, QString newExpression);

    exprtk::symbol_table<double> symbolsTable;

    QLinkedList<double> variables;
    QStringList variables_names;
    QStringList variables_expressions;

    double activeRow;

    bool custom_exp_parse(QString expression, int currentRow, QString& result);
};

#endif // MAINWINDOW_H
