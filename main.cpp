#include "DataLinkLayer.h"
#include<iostream> 
#include <string>
#include <vector>
#include <thread>
#include <sys/stat.h>
#include <fstream>
#define MAXSIZE 256 //max size of frame in bits
using namespace std;
/*
We Prepend headers to the data when sending.
Start header for start of data, con header for continueing and end for end of data
Start=00
con=01
end=10
*/
RingBuffer<string,100> InputQueue;

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
            frames[i].insert(frames[i].begin(),1);
            frames[i].insert(frames[i].begin(),0);
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
        //for( auto bit : frame) cout << bit; cout << endl; 
        frameheader+=frame[0]<<1;
        frame.erase(frame.begin());
        frameheader+=frame[0];
        frame.erase(frame.begin());
     //   if (!frames.size() and frameheader) return false;
     //   else if (frames.size() and !frameheader) return false;
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

void getUserInput()
{
    while(true)
    {
        string input;
        getline(cin,input);
        InputQueue.push_back(input);
    }

}
void appendByte(vector<bool> &boolVec, unsigned char byte)
{
    for(int i=7;i>=0;i--)
    {
        if (byte-(1<<i) >= 0)
        {
            byte-=(1 << i);
            boolVec.push_back(1);
            
        }
        else boolVec.push_back(0);
    }
}
unsigned char extractByte(vector<bool> &boolVec,long long pos)
{
    char byte=0;
    for(int i=7;i>=0;i--)
    {
        byte+=boolVec[pos+7-i]<<i;
    }
    return byte;
}
bool boolvecToFile(string filepath,vector<bool> &boolVec)
{
    if(boolVec.size()%8) return false;
    char *memblockToFile;
    long long size=boolVec.size()/8;
    memblockToFile=new char [size];
    for(long long i=0;i<size;i++)
    {
        memblockToFile[i]=extractByte(boolVec,i*8);
    }
    ofstream file(filepath,ios::out | ios::binary | ios::trunc);
    if (!file.is_open()) return false;
    file.write(memblockToFile,size);
    return file.good();
    
    
}
bool fileToBoolVector(string filepath,vector<bool> &boolVec)
{
    ifstream file(filepath, ios::in | ios::binary |ios::ate);
    if(!file.is_open()) return false;
    char * memblockToFile;
    streampos size=file.tellg();
    memblockToFile=new char [size];
    file.seekg(0,ios::beg);
    file.read(memblockToFile,size);
    file.close();
    for(int i=0;i<size;i++)
    {
        appendByte(boolVec, memblockToFile[i]);
    }
    return file.good();
}
int main()
{
    DataLinkLayer DaLLObj;
    thread inputthread(getUserInput);
    while(true)
    {
        while (!InputQueue.empty())
        {
            string inputstring=InputQueue.pop_front();
            if (inputstring.substr(0,4)=="send")
            {
                string path=inputstring.substr(4,inputstring.size()-4);
                while(path.size()>1 and path.front()==' ') path.erase(path.begin());
                vector<bool> filedata;
                if(!fileToBoolVector(path,filedata))
                {
                    cout << "path not valid" << endl;
                }
                else
                {
                     cout << "sending file" << endl;
                     if(sendData(filedata,&DaLLObj) )
                     {
                        cout << "file send succesfully." << endl;
                     }
                     else
                     {
                        cout << "error while sending file" << endl;
                     }
                }
            }
            else if(inputstring.substr(0,4)=="quit") exit(0);
        }
        vector<bool> indata;
        if (getData(indata,&DaLLObj))
        {
            cout << "Recieved file of size " << indata.size()/8 <<" bytes" << endl;
            boolvecToFile("data",indata);
            cout << "File saved " << endl;
        }
    usleep(1000);
    }   
}
