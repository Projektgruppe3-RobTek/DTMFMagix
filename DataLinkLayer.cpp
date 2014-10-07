#include "DataLinkLayer.h"

DataLinkLayer::DataLinkLayer()
{
    getFramesThread=thread(getFramesWrapper,this);
    getDatagramsThread=thread(getDatagramsWraper,this);
}


void DataLinkLayer::getFrames()
{
    while(true) usleep(10000000); //not done
}

void DataLinkLayer::getDatagrams()
{
    while(true) usleep(10000000); //not done.
}
void getFramesWrapper(DataLinkLayer *DaLLObj)
{
    DaLLObj->getFrames();
}

void getDatagramsWraper(DataLinkLayer *DaLLObj)
{
    DaLLObj->getFrames();
}

void DataLinkLayer::bitStuff(vector<bool> &frame)
{
    for(int i=flag.size(); i<frame.size(); i++)
    {
        if(flagcheck(frame,i-(int)flag.size(),flag))
        {
            frame.insert(frame.begin()+i-1,0);
        }
    }
}
void DataLinkLayer::revBitStuff(vector<bool> &frame)
{
    if (frame.size()<flag.size()) return;
    
    vector<bool> newframe;
    newframe.reserve(frame.size());
    
    for(int i=0;i<flag.size();i++) newframe.push_back(frame[i]);
    
    for(int i=flag.size(); i<frame.size(); i++) 
    {
        newframe.push_back(frame[i]);
        if(flagcheck(frame,i-(int)flag.size(),flag))
        {
            newframe.pop_back();
        }
    }
    frame=newframe;
}


void DataLinkLayer::setPadding(vector<bool> &frame)
{
    int lengthOfPadding=0;
    
    while(frame.size()%4)
    {
        lengthOfPadding++;
        frame.push_back(0);
    }
    frame.insert(frame.begin(),lengthOfPadding%2);
    frame.insert(frame.begin(),lengthOfPadding%4-lengthOfPadding%2);
}

void DataLinkLayer::removePadding(vector<bool> &frame)
{
    int paddinglength=(frame[0]<<1)+(frame[1]);
    frame.erase(frame.begin(),frame.begin()+2);
    for(int i=0;i<paddinglength;i++) frame.pop_back();
}



bool flagcheck(vector<bool> &vec1, int start1,array<bool,8> &flag) //check if the flag matches given vec
{
    for(int i=0;i<flag.size();i++)
    {
        if (vec1[start1+i]!=flag[i]) return false;
        
    }
    return true;
}
