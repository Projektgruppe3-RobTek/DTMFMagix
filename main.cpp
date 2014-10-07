#include "DataLinkLayer.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[])
{
    vector<bool> boolvec;
    DataLinkLayer DaLLObj;
    while(true)
    {
        vector<bool> boolvec;
        int lenght=rand()%101;
        while(lenght--) boolvec.push_back(rand()%2);
        auto cpboolvec=boolvec;
        DaLLObj.setPadding(boolvec);
        DaLLObj.removePadding(boolvec);
        for(int i=0;i<boolvec.size();i++) if (boolvec[i]!=cpboolvec[i]) {
            for(auto bit : cpboolvec) cout << bit ; cout << endl;
            DaLLObj.setPadding(cpboolvec);
            for(auto bit : cpboolvec) cout << bit ; cout << endl;
            for(auto bit : boolvec) cout << bit ; cout << endl;
            return 0;
        }
    }
}
