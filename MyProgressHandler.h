#ifndef MYPROGRESSHANDLER_H
#define MYPROGRESSHANDLER_H

#include <QMutex>
#include <QObject>
#include <omp.h>
#include <QAtomicInt>
#include <iostream>
#include <QString>

class MyProgressHandler:public QObject
{
    Q_OBJECT
public:
    MyProgressHandler();
    MyProgressHandler(int max);

    void full();
    void reset(int max);
    void update();
    void setWhat(QString what);
    bool isBusy();

    void errorMsg(QString msg);

signals:
    void sig_progress(int percent);
    void sig_what(QString what);
    void sig_error(QString what,QString msg);

public:
    int max;
    QAtomicInt counter;
    QAtomicInt index;
    QAtomicInt value;
    int indexes[100];
    QString what;
    QMutex mutex_what;
};

#endif // MYPROGRESSHANDLER_H
