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
    vector<int> elementsToStuff;
    for(int i=flag.size()-1;i<frame.size();i++)
    {
        if(flagcheck(frame,i-((int)flag.size()-1),flag,flag.size()-1)) elementsToStuff.push_back(i);
    }
    int offset=0;
    for(int index : elementsToStuff) 
    {
        frame.insert(frame.begin()+offset+index,!flag.back());
        offset++;
    }
}


void DataLinkLayer::revBitStuff(vector<bool> &frame)
{
    vector<int> elementsToRemove;
    for(int i=flag.size()-1;i<frame.size();i++)
    {
        if (flagcheck(frame,i-(int)flag.size()+1,flag,flag.size()-1)) elementsToRemove.push_back(i);
    }
    int offset=0;
    for(int index : elementsToRemove) 
    {
        frame.erase(frame.begin()+offset+index);
        offset--;
    }
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

bool DataLinkLayer::getID(vector<bool> &frame)
{
    int ID=frame[0];
    frame.erase(frame.begin());
    if (lastinID==ID) return false;
    lastinID=ID;
    return true;
}

void DataLinkLayer::setID(vector<bool> &frame,bool ID)
{
    frame.insert(frame.begin(),ID);
    lastoutID=ID; 
}


bool flagcheck(vector<bool> &vec1, int start1,array<bool,8> &flag,int lenght) //check if the flag matches given vec
{
    for(int i=0;i<lenght;i++)
    {
        if (vec1[start1+i]!=flag[i]) return false;
        
    }
    return true;
}
