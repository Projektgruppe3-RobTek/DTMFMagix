#include "DataLinkLayer.h"
#include<iostream> 
#include <string>
#include <vector>
using namespace std;
void reciever(DataLinkLayer *DaLLObj)
{
    while(true)
    {
        usleep(1000);
        if(DaLLObj->dataAvaliable()) 
        {
            for(auto bit : DaLLObj->popData())
            {
                cout << bit;
            }
            cout << endl;
        }
    
    }
}

void Sender(DataLinkLayer *DaLLObj)
{
    while(true)
    {
        usleep(1000);
        string instring;
        cin >> instring;
        vector<bool> boolvec;
        for(char chr : instring)
        {
            if(chr=='1') boolvec.push_back(1);
            else boolvec.push_back(0);
        }
        while(DaLLObj->bufferFull()) usleep(500);
        DaLLObj->pushData(boolvec);
    }
}
int main()
{
    DataLinkLayer DaLLObj;
    thread t1=thread(Sender,&DaLLObj);
    thread t2=thread(reciever,&DaLLObj);
    while(true) usleep(1000000);
}
