#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <unistd.h>
#include "AppLayer.h"

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0,AppLayer *appL_ = 0,bool *send=0);
    void run();
    bool Stop();

    AppLayer *appL;
    bool *sending;
signals:
    void NumberChanged(int, int);


public slots:

};

#endif // MYTHREAD_H
