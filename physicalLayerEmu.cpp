#include "physicalLayerEmu.h"
using namespace std;

physicalLayer::physicalLayer()
{
    startTimer();
    timer.tv_usec -= 50000;
    ofstream f1;            //Ensure
    ofstream f2;            //The
    f1.open(mediafile);     //Files
    f2.open(newmediafile);  //is
    f1.close();             //created
    f2.close();             //correctly
    
    sendert=thread(frameGrabWrapper, this);      //Create input and output threads
    recievert=thread(frameSendWrapper, this);    
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
        if (getTimer() < 100) continue;
        if(!getNewState()) continue;
        
        vector<bool> frame = getData();
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
        
        startTimer();
        setData(outBuffer.pop_front());
        setNewState(1);
    }   
}
void physicalLayer::startTimer()
{
    gettimeofday(&timer, NULL);
}

int physicalLayer::getTimer()
{
    timeval tv;
    gettimeofday(&tv, NULL);
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
    ifstream newStateFile;
    newStateFile.open(newmediafile);
    string newStateString;
    getline(newStateFile, newStateString);
    newStateFile.close();
    if (newStateString[0] == '1') return true;
    else return false;
}
void physicalLayer::setNewState(bool state)
{
    ofstream newStateFile;
    newStateFile.open(newmediafile, ios::trunc);
    if (state) newStateFile << '1';
    else newStateFile << '0';
}
vector<bool> physicalLayer::getData()
{
    vector<bool> data;
    
    ifstream dataFile;
    dataFile.open(mediafile);
    string dataString;
    getline(dataFile, dataString);
    dataFile.close();
    for(char chr : dataString)
    {
        if (chr=='0') data.push_back(0);
        else data.push_back(1);
    }
    return data;
}
void physicalLayer::setData(vector<bool> data)
{
    string dataString;
    for(auto bit : data) 
    {
        if (bit) dataString += '1';
        else dataString += '0';
    }
    ofstream dataFile;
    dataFile.open(mediafile, ios::trunc);
    dataFile << dataString << endl;
    dataFile.close();
    usleep(10000);
}
