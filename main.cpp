#include "DataLinkLayer.h"
#include<iostream> 
#include <string>
#include <vector>
#define MAXSIZE 192 //max size of frame in bits
using namespace std;
/*
We Prepend headers to the data when sending.
Start header for start of data, con header for continueing and end for end of data
Start=00
con=01
end=10
*/
bool sendData(vector<bool> Data,DataLinkLayer *DaLLObj)
{
    while(DaLLObj->getMode()==2) usleep(1000); //wait if slave
    if(!DaLLObj->connect()) return false; //open connection
    vector<vector<bool>> frames;
    vector<bool> tmpframe;
    for(int i=0;i<Data.size();i++)
    {
        if(tmpframe.size()==MAXSIZE-2)
        {
            frames.push_back(tmpframe);
            tmpframe.clear();
        }
        tmpframe.push_back(Data[i]);
    }
    frames.push_back(tmpframe);
    for(int i=0;i<frames.size();i++)
    {
        if(i==0) //first frame
        {
            frames[i].insert(frames[i].begin(),0);
            frames[i].insert(frames[i].begin(),0);
        }
        else if(i==frames.size()-1) //last frame
        {
            frames[i].insert(frames[i].begin(),0);
            frames[i].insert(frames[i].begin(),1);
        }
        else //rest of frames
        {
            frames[i].insert(frames[i].begin(),0);
            frames[i].insert(frames[i].begin(),1);
        }
    }
    for (auto frame : frames)
    {
        while(DaLLObj->dataBufferFull())
        {
            usleep(1000);
        }
        if(DaLLObj->getMode()!=1) return false; //We lost connection
        DaLLObj->pushData(frame);
    }
    return (DaLLObj->terminate());
}

bool getData(vector<bool> &data,DataLinkLayer *DaLLObj)
{
    if (!DaLLObj->dataAvaliable()) return false;
    vector<vector<bool>> frames;
    int frameheader=-1;
    while(frameheader!=2)
    {
        while(!DaLLObj->dataAvaliable()) usleep(1000);
        frameheader=0;
        auto frame=DaLLObj->popData();
        frameheader+=frame[0]<<1;
        frame.erase(frame.begin());
        frameheader+=frame[0];
        frame.erase(frame.begin());
        if (!frames.size() and !frameheader) return false;
        else if (!frameheader) return false;
        frames.push_back(frame);
    }
    for(auto frame : frames)
    {
        for(auto bit : frame)
        {
            data.push_back(bit);
        }
    }
    return true;
}

int main()
{


}
