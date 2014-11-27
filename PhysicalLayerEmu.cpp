#include "PhysicalLayerEmu.h"
#include <iostream>
//#define randomflip
//#define flippercent 0.5f
//#define debug
using namespace std;

PhysicalLayer::PhysicalLayer()
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
PhysicalLayer::~PhysicalLayer()
{
    stop=true;
    sendert.join();
    recievert.join();
    
}
void PhysicalLayer::pushData(vector<bool> frame) 
{
    #ifdef debug
    cout << "Framesize=" << frame.size() << endl;
    #endif
    outBuffer.push_back(frame);
}

bool PhysicalLayer::layerBusy()
{
    return outBuffer.full();
}

bool PhysicalLayer::dataAvailable()
{
    return inBuffer.size();
}
vector<bool> PhysicalLayer::popData()
{
    return inBuffer.pop_front();
}

void PhysicalLayer::FrameGrabber()
{
    while(true)
    {
        if(stop) break;
        while(!getNewState() and !stop) usleep(500);
        
        vector<bool> frame = getData();
        while(inBuffer.full()) usleep(500);
        inBuffer.push_back(frame);
        setNewState(0);
    }
}

void PhysicalLayer::FrameSender()
{
    while(true)
    {
        if(stop) return;
        while(outBuffer.empty())
        {
            if(stop) return;
            usleep(500);
        }
        if (getNewState()) continue;
        
        setData(outBuffer.pop_front());
        setNewState(1);
    }   
}
void PhysicalLayer::startTimer()
{
    gettimeofday(&timer, NULL);
}

int PhysicalLayer::getTimer()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000 )  - (timer.tv_sec * 1000 + timer.tv_usec / 1000);
}

void frameGrabWrapper(PhysicalLayer * physLayerObj)
{
    physLayerObj->FrameGrabber();
}

void frameSendWrapper(PhysicalLayer * physLayerObj)
{
    physLayerObj->FrameSender();
}

bool PhysicalLayer::getNewState()
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
void PhysicalLayer::setNewState(bool state)
{
    ofstream newStateFile;
    newStateFile.open(newmediafile, ios::trunc);
    if (state) newStateFile << to_string(++framenumber);
    else newStateFile << '0';
}
vector<bool> PhysicalLayer::getData()
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
void PhysicalLayer::setData(vector<bool> data)
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

