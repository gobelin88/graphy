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
//#include "3d_viewer.h"
#include "view3d.h"
#include "qgradientcombobox.h"
#include "mytablemodel.h"
#include "mytableview.h"

#include <random>

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

    void slot_editColumn();//ok
    void slot_sectionDoubleClicked();//ok
    void slot_remove_columns_and_rows();
    void slot_delete_selectedRows();
    void slot_delete_selectedColumns();
    void slot_delete_selected();
    void slot_newRow();//ok
    void slot_newRows();//ok
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
    void directNew(int sx,int sy);

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
    void slot_parameters();
    void slot_results(QString results);
    void slot_colourize();

    //
    void slot_vSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex);
    void slot_hSectionMoved(int logicalIndex,int oldVisualIndex,int newVisualIndex);

    void fileModified();

private:
    //Close
    void closeEvent (QCloseEvent *event);

    //Error
    void error(QString title,QString msg);

    //Register
    Register reg;

    //View & Gui
    QTextEdit* te_results;
    QTabWidget* te_widget;
    Ui::MainWindow* ui;
    QTableView* table;
    void resizeEvent(QResizeEvent* event);
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

    //Model
    void createModel();
    void affectModel();
    void modelMute();
    void modelUnMute();
    QStandardItemModel * model;
    QAbstractItemModel * old_model;

    //Io
    bool hasheader;
    bool isModified;
    QString separator;
    QString current_filename;
    QStringList extractToken(QString fileLine);
    void setCurrentFilename(QString filename);


    //Data
    Eigen::MatrixXd datatable;
    Curve2D shared;

    //Col/row
    QVector<QString> getColumn(int idCol);
    QVector<QString> getRow(int idRow);
    void setColumn(int idCol,const QVector<QString>& vec_col);
    void addModelRow(const QStringList& str_row);
    void addModelRow(const Eigen::VectorXd & value_row);

    //expr
    QVector<QString> evalColumn(int colId);

    //Shortcuts
    QMap<QString,QKeySequence> shortcuts;
    void applyShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);
    bool loadShortcuts();
    void saveShortcuts(const QMap<QString,QKeySequence>& shortcuts_map);

    //Constants
    const float graphyVersion=3.5f;

    //selections
    QString getSelectionPattern();
    void setSelectionPattern(QString pattern);

    //conversions double/string
    Eigen::VectorXd toSafeDouble(QVector<QString> vec_col_str);
    double toSafeDouble(const QString& str) const;
    QString fromNumber(double value);
    QString fromNumber(double value,int precision);
    bool asColumnStrings(int idCol);
    void getRowColSelectedRanges(QCPRange &range_row,QCPRange &range_col);

    //Experimental
    void createExperimental();
    MyTableView * experimental_table;

};

#endif // MAINWINDOW_H
