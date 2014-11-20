#include <iostream>
#include <vector>
#include <unistd.h>
#include "PhysicalLayer.h"
using namespace std;

int main()
{
    vector<bool> vec;
    PhysicalLayer physLayer;
    for(int i=0; i<20; i++)
    {
        vec.push_back((i*i)%2);
    }
    physLayer.pushFrame(vec);
    while(!physLayer.returnReceiveFlag())
    {
        sleep(1);
    }
    vec=physLayer.popFrame();
    for(int i=0; i<vec.size(); i++)
        cout << vec[i];
    return 0;
}
