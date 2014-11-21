#include <iostream>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include "PhysicalLayer.h"
using namespace std;

int main()
{
    srand(time(NULL));
    vector<bool> vecin;
    vector<bool> vecout;
    PhysicalLayer physLayer;
    for(int i=0; i<200; i++)
    {
        vecin.push_back((rand() % 100)%2);
    }
    for(int i=0; i<vecin.size(); i++)
        cout << vecin[i];
    cout << endl;
    physLayer.pushFrame(vecin);
    while(!physLayer.returnReceiveFlag())
    {
        usleep(100);
    }
    vecout=physLayer.popFrame();
    for(int i=0; i<vecout.size(); i++)
        cout << vecout[i];
    cout << endl;
    if(vecin==vecout)
        cout << "tillykke";
    return 0;
}
