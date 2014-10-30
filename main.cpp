//#include "physicalLayer.h"
#include "NewPlayer.h"
#include <iostream>
#include <unistd.h>
using namespace std;

int main(int argc, char *argv[])
{
  /*   physicalLayer PhysLayer;
    PhysLayer.QueueFrame("2");

    while(1)
    {
        if(PhysLayer.isFrameAvaliable())
        {
            cout << PhysLayer.GetNextFrame() << endl;
        }
        usleep(5000);

    }

*/

    DTMFPlayer test;

//while(true)


while(true)
{


    test.PlayDTMF({'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'},500,20);
}




    return 0;





}
