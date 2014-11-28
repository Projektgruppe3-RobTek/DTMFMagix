#include "mythread.h"
#include <QtCore>

MyThread::MyThread(QObject *parent,AppLayer *appL_) :
    QThread(parent),appL(appL_)
{

}

void MyThread::run()
{
	int counter=0;
    while(true)
    {
    	counter++;
        //emit NumberChanged(20,100);
        emit NumberChanged(appL->getEstimatedSize(),appL->getNumberOfFrames());
        usleep(100000);
    }
}
