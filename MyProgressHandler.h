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

    void reset(int max);
    void update();
    void full();

    bool isBusy();

    void setWhat(QString what);
    void setErrorMsg(QString msg);

signals:
    void sig_progress(int percent);
    void sig_what(QString what);
    void sig_error(QString what,QString msg);

public:
    int max;
    QAtomicInt counter;
    QAtomicInt index;
    int indexes[101];
    QString what;
    QMutex mutex_what;
};

#endif // MYPROGRESSHANDLER_H
