#include "physicalLayerEmu.h"
#include <iostream>
//#define randomflip
//#define flippercent 0.1f
#define debug
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
    srand(timer.tv_usec); 
    framenumber=rand()%1000000000+1;
}

void physicalLayer::QueueFrame(vector<bool> frame) 
{
    #ifdef debug
    cout << "Framesize=" << frame.size() << endl;
    #endif
    outBuffer.push_back(frame);
}

bool physicalLayer::isQueueFull()
{
    return outBuffer.full();
}

bool physicalLayer::isFrameAvaliable()
{
    return inBuffer.size();
}
vector<bool> physicalLayer::getFrame()
{
    return inBuffer.pop_front();
}

void physicalLayer::FrameGrabber()
{
    while(true)
    {
        while(!getNewState()) usleep(2000);
        
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
        while(outBuffer.empty()) usleep(1000);
        if (getNewState()) continue;
        
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
    if(!newStateString.size()) return false;
    if (stoi(newStateString.c_str())!=0 and stoi(newStateString.c_str())!=framenumber)
    {
        //cout << "NewData " << stoi(newStateString.c_str()) << endl;
        return true;
    }
    else return false;
}
void physicalLayer::setNewState(bool state)
{
    ofstream newStateFile;
    newStateFile.open(newmediafile, ios::trunc);
    if (state) newStateFile << to_string(++framenumber);
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
    #ifdef randomflip
    for(int i = 0; i<data.size(); i++)
    {
        if (rand()%100000000>int(100000000.*(1.-(flippercent)/100.))) data[i]=!data[i];
    }
    #endif
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
}

