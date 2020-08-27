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
#include "bode_viewer.h"
#include "2d_viewer.h"
#include "3d_viewer.h"
#include "view3d.h"
#include "qgradientcombobox.h"

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

    QString getColName(int id);
    int getColId(QString colName);

public slots:
    //underlying table
    QStandardItem* itemAt(int i,int j);
    QString at(int i,int j);
    void updateTable();
    void updateTable(const QModelIndex& indexA, const QModelIndex& indexB);
    void updateTableViewRows();

    void slot_editColumn();
    void slot_sectionDoubleClicked(int value);

    void slot_delete();
    void slot_newRow();
    void slot_newRows();
    void slot_updateColumns();
    void slot_newColumn(QString name,Eigen::VectorXd data);

    void slot_copy();
    void slot_paste();

    //IO
    void clear();
    void slot_new();
    void slot_open();
    void slot_save();
    void slot_save_as();
    void slot_export();
    void direct_open(QString filename);
    void direct_save(QString filename);
    void direct_export(QString filename);
    void direct_new(int sx,int sy);

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
    void slot_filter();
    void slot_select();

    //Misc
    void slot_parameters();
    void slot_results(QString results);

    //
    void slot_vSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex);
    void slot_hSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex);

    //
    void slot_colourize();

private:
    void closeEvent (QCloseEvent *event);

    QStringList extractToken(QString fileLine);
    bool isValidExpression(QString variableExpression);
    bool isValidVariable(QString variableName, int currentIndex);
    bool editVariableAndExpression(int currentIndex);

    void setCurrentFilename(QString filename);
    void fileModified();

    void resizeEvent(QResizeEvent* event);

    QTabWidget* te_widget;
    Ui::MainWindow* ui;
    QTableView* table;
    QString current_filename;
    QMdiArea* mdiArea;
    QStandardItemModel* model;
    bool hasheader;
    Eigen::MatrixXd datatable;
    Curve2D shared;

    QAction* a_newColumn;
    QAction* a_newRow;
    QAction* a_newRows;
    QAction* a_delete;
    QAction* a_updateColumns;
    QAction* a_copy;
    QAction* a_paste;

    //Col/row
    QVector<QString> getColumn(int idCol);
    void setColumn(int idCol,const QVector<QString>& vec_col);
    void addModelRow(const QStringList& str_row);
    void addModelRow(const Eigen::VectorXd & value_row);

    //expr
    QVector<QString> evalColumn(int colId);

    void dispVariables();
    void registerClear();
    void registerNewVariable(QString varname,QString varexpr);
    void registerDelVariable(QString varname);
    void registerRenameVariable(QString old_varname, QString new_varname, QString oldExpression, QString newExpression);
    void swapVariables(int ida,int idb);
    void moveVariable(int ida,int idb);
    exprtk::symbol_table<double> symbolsTable;
    QVector<double*> variables;
    QStringList variables_names;
    QStringList variables_expressions;

    double activeRow;
    double activeCol;

    bool custom_exp_parse(QString expression, int currentRow, QString& result);

    QMap<QString,QKeySequence> shortcuts;
    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);
    bool loadShortcuts();
    void saveShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);

    //Results
    QTextEdit* te_results;

    //
    const float graphyVersion=3.3f;

    //selections
    QString getSelectionPattern();
    void setSelectionPattern(QString pattern);

    //conversions double/string
    Eigen::VectorXd toSafeDouble(QVector<QString> vec_col_str);
    double toSafeDouble(const QString& str) const;
    QString fromNumber(double value);
    QString fromNumber(double value,int precision);
    const int internal_precision=12;
    bool asColumnStrings(int idCol);

    QString separator;


    bool isModified;
    void getRowColSelectedRanges(QCPRange &range_row,QCPRange &range_col);
};

#endif // MAINWINDOW_H
