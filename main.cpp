#include "DataLinkLayer.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[])
{
    timeval tv;
    gettimeofday(&tv,NULL);
    srand(tv.tv_usec);
    vector<bool> boolvec;
    DataLinkLayer DaLLObj;
    int lenght=100;
    while(lenght--) boolvec.push_back(rand()%2);
    for(auto bit : boolvec) cout << bit; cout << endl;
    DaLLObj.CRCencoder(boolvec);
    for(auto bit : boolvec) cout << bit; cout << endl;
//    boolvec[7]=!boolvec[7];
    cout << DaLLObj.CRCdecoder(boolvec) << endl;
    for(auto bit : boolvec) cout << bit; cout << endl;
}
