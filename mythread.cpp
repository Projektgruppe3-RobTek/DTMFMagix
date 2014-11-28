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
    	if(*sending)
		{
			cout << "SENDING!!" << endl;
			emit NumberChanged(appL->getTotalFramesToSend(),appL->getFramesSend());
		}
		else
		{
       		emit NumberChanged(appL->getEstimatedSize(),appL->getNumberOfFrames());
		}        
		usleep(100000);
    }
}
