#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QDialog>


class FilterDialog : public QDialog
{
    Q_OBJECT
public:
    FilterDialog(QString defaultValue, QString varName);

    QString getValue();
    int getMode();

public slots:
    void slot_modeChanged(int mode);

private:
    QComboBox* cb_mode;
    QLineEdit* sb_value;
    QGridLayout * gbox;

};

#endif // FILTERDIALOG_H
