#include "NewPhysicalLayer.h"
#include <iostream>
#include <unistd.h>
#include <vector>
using namespace std;

int main()
{
    vector<bool> abe;
    for (int i = 0; i < 4; ++i)
    {
        abe.push_back(i%2);
    }
    for (int i = 0; i < 4; ++i)
    {
        abe.push_back((i+1)%2);
    }
    for (int i = 0; i < 4; ++i)
    {
        abe.push_back(i*i%2);
    }
    for (int i = 0; i < 4; ++i)
    {
        abe.push_back(i*3%2);
    }
    for (int i = 0; i < 4; ++i)
    {
        abe.push_back((i^i)%2);
    }
    
    NewPhysicalLayer physLayer;
    physLayer.pushFrame(abe);
    while(true)
    {
        bool abelars=physLayer.returnReceiveFlag();
        if(abelars)
        {
            abe=physLayer.popFrame();
            for (int i = 0; i < abe.size(); ++i)
            {
                cout << abe[i];
            }
        }
        sleep(1);
    }
}
