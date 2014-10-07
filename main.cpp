#include "physicalLayer.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    physicalLayer PhysLayer;
    PhysLayer.QueueFrame("25264abc863");
    
    while(1)
    {
        if(PhysLayer.isFrameAvaliable())
        {
            cout << PhysLayer.GetNextFrame() << endl;
        }
        usleep(100000);
    }
}
