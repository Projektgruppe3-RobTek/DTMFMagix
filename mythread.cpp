#include "mythread.h"
#include <QtCore>

MyThread::MyThread(QObject *parent,AppLayer *appL_,bool *send) :
    QThread(parent),appL(appL_)
{
	sending = send;
}
void MyThread::Stop()
{
	stop=true;
}
void MyThread::run()
{
	
    while(!stop)
    {
    	
		if(*sending && appL->getTotalFramesToSend() == appL->getFramesSend())
		{
		*sending = false;
		} 		
		else if(*sending)
		{
		emit NumberChanged(appL->getTotalFramesToSend(),appL->getFramesSend());
		}
		
				
		else
		{
       		emit NumberChanged(appL->getEstimatedSize(),appL->getNumberOfFrames());
		}        
		usleep(100000);
    }
}
