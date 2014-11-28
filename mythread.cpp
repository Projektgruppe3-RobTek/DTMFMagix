#include "mythread.h"
#include <QtCore>

MyThread::MyThread(QObject *parent,AppLayer *appL_,bool *send) :
    QThread(parent),appL(appL_)
{
	sending = send;
}

void MyThread::run()
{
	
    while(true)
    {
    	if(sending)
	{
	emit NumberChanged(appL->getFramesSend(),appL->getTotalFramesToSend());
	}
	else
	{
        emit NumberChanged(appL->getEstimatedSize(),appL->getNumberOfFrames());
	}        
	usleep(100000);
    }
}
