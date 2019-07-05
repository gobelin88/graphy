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
#include "2d_viewer.h"
#include "3d_viewer.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    QVector<double> getCol(int id,const TableData & table);
    QString getColName(int id);

public slots:
    void updateTable();
    void updateTable(int i,int j,double value);

    void slot_open();
    void slot_save();
    void direct_open(QString filename);
    void direct_save(QString filename);
    void direct_new(int sx,int sy);

    void slot_plot_y();
    void slot_plot_graph_xy();
    void slot_plot_curve_xy();
    void slot_plot_xyz();
    void slot_plot_histogram();
    void slot_plot_cloud_3D();

private:
    void setCurrentFilename(QString filename);

    void resizeEvent(QResizeEvent * event);

    Ui::MainWindow *ui;

    QTableView * table;

    QString current_filename;

    QMdiArea * mdiArea;

    QStandardItemModel *model;

    bool hasheader;

    TableData datatable;

    Curve2D shared;
};

#endif // MAINWINDOW_H
