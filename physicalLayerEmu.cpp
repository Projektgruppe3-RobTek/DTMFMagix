#include "physicalLayerEmu.h"
#include <iostream>
using namespace std;
physicalLayer::physicalLayer()
{
    startTimer();
    timer.tv_usec-=50000;
    ofstream f1;            //Ensure
    ofstream f2;            //The
    f1.open(mediafile);     //Files
    f2.open(newmediafile);  //is
    f1.close();             //created
    f2.close();             //correctly
    sendert=thread(frameGrabWrapper,this);
    recievert=thread(frameSendWrapper,this);
}

void physicalLayer::QueueFrame(vector<bool> frame) 
{
    outBuffer.push_back(frame);
}

bool physicalLayer::isQueueFull()
{
    return outBuffer.full();
}

bool physicalLayer::isFrameAvaliable()
{
    return !inBuffer.empty();
}
vector<bool> physicalLayer::getFrame()
{
    return inBuffer.pop_front();
}

void physicalLayer::FrameGrabber()
{
    while(true)
    {
        usleep(2000);
        if (getTimer()<50) continue;
        if(!getNewState()) continue;
        vector<bool> frame=getData();
        while(inBuffer.full()) usleep(500);
        inBuffer.push_back(frame);
        setNewState(0);
    }
}

void physicalLayer::FrameSender()
{
    while(true)
    {
        usleep(500);
        if(outBuffer.empty()) continue;
        if (getNewState()) continue;
        setData(outBuffer.pop_front());
        startTimer();
        setNewState(1);
    }   
}
void physicalLayer::startTimer()
{
    gettimeofday(&timer,NULL);
}

int physicalLayer::getTimer()
{
    timeval tv;
    gettimeofday(&tv,NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000 )  - (timer.tv_sec * 1000 + timer.tv_usec / 1000);
}

void frameGrabWrapper(physicalLayer * physLayerObj)
{
    physLayerObj->FrameGrabber();
}

void frameSendWrapper(physicalLayer * physLayerObj)
{
    physLayerObj->FrameSender();
}

bool physicalLayer::getNewState()
{
    ifstream newstuff;
    newstuff.open(newmediafile);
    string nw;
    getline(newstuff,nw);
    newstuff.close();
    if (nw[0]=='0') return false;
    else return true;
}
void physicalLayer::setNewState(bool state)
{
    ofstream newstuff;
    newstuff.open(newmediafile,ios::trunc);
    if (state) newstuff << '1';
    else newstuff << '0';
}
vector<bool> physicalLayer::getData()
{
    vector<bool> data;
    
    ifstream stuff;
    stuff.open(mediafile);
    string nw;
    getline(stuff,nw);
    stuff.close();
    for(char chr : nw)
    {
        if (chr=='0') data.push_back(0);
        else data.push_back(1);
    }
    return data;
}
void physicalLayer::setData(vector<bool> data)
{
    string datastring;
    for(auto bit : data) 
    {
        if (bit) datastring+='1';
        else datastring+='0';
    }
    ofstream stuff;
    stuff.open(mediafile,ios::trunc);
    stuff << datastring << endl;
}
