#include "DataLinkLayer.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <sys/time.h>
using namespace std;

int main(int argc, char *argv[])
{
    timeval tv;
    gettimeofday(&tv,NULL);
    srand(tv.tv_usec);
    DataLinkLayer DaLLObj;
    DaLLObj.startTimer();
    while(true)
    {
        cout << DaLLObj.getTimer() << endl;
        sleep(1);
    }
}
