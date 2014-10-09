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
    vector<bool> boolvec={0,1,1,1,0,0,0,1,0,0,1,1,1,0,0,0,1,0,1,1,1,1,0,1,1,1,0,1,0,1,1,0,1,1,1,1,1,0,0,1,0,0,0,1,0,0,1,1,1,0,1,0,0,1,1,1,0,1,0,0,1,0,1,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,1,1,0,0,1,0,1,1,1,1,0,0,1,1,0,0,1,1,1,0,1,1};
    DataLinkLayer DaLLObj;
    for(auto bit : boolvec) cout << bit; cout << endl;
    DaLLObj.CRCencoder(boolvec);
    for(auto bit : boolvec) cout << bit; cout << endl;
    //boolvec[7]=!boolvec[7];
    cout << DaLLObj.CRCdecoder(boolvec) << endl;
    for(auto bit : boolvec) cout << bit; cout << endl;
}
