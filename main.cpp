#include "DataLinkLayer.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    DataLinkLayer DataLink;
    DataLink.PlayFrame("25264abc863");
    
    while(1)
    {
        if(DataLink.DataAvaliable())
        {
            cout << DataLink.GetNextFrame() << endl;
        }
        usleep(100000);
    }
}
