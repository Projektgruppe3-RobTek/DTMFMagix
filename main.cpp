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
    int counter=0;
    int errorcount=0;
            vector<bool> boolvec;//={0,1,1,1,0,0,0,1,0,0,1,1,1,0,0,0,1,0,1,1,1,1,0,1,1,1,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,0,0,1,0,0,1,1,1,0,1,0,0,1,1,1,0,1,0,0,1,0,1,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,1,1,0,0,1,0,1,1,1,1,0,0,1,1,0,0,1,1,1,0,1,1};
    while(true)
    {
        counter++;
        int lenght=rand()%1000+1;
        int lenghtcp=lenght;
        boolvec.clear();
        while(lenght--) boolvec.push_back(rand()%2);

        DaLLObj.CRCencoder(boolvec);
        //boolvec[4]=!boolvec[4];
        if(!DaLLObj.CRCdecoder(boolvec))
        {
            errorcount++;
            cout << "ERROR!" << errorcount/float(counter) << " " << counter << " "<< lenghtcp << " " <<boolvec.size() <<  endl;
        }
    }
}
