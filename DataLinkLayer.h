#include "Recorder.h"
#include "Player.h"
#include "Goertzel.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include <array>
#include <thread>
#include <string>
#define TONELENGHT 50
#define SILENTLENGHT 10
#define SILENTLIMIT 0.0
#define M_PI 3.14159265359
#define RECIEVEDBUFFSIZE 10000
using namespace std;
struct SyncData
{
    float MaxMagnitude=0;
    timeval tv;

};
struct RecievedData
{
    //We can't implement this as FIFO if we want thread safety    
    array<char,RECIEVEDBUFFSIZE> RecArray; 
    int NextElementToRecord=0;
    int LastDataElement=0;

};

class DataLinkLayer
{
    private:
        int Freqarray1[4]={697,770,852,941};
        int Freqarray2[4]={1209,1336,1477,1633};
        char SyncSequence[4]={'a','6','8','*'}; //Should not contain the same note two times in a row.
        char SyncEnd[4]={'1','2','3','4'};
        char EndSequence[4]={'*','8','6','a'};
        char DTMFTones[16]={'1','2','3','a','4','5','6','b','7','8','9','c','*','0','#','d'};
        DualTonePlayer Player;
        Recorder Rec;
        long long synctime;
        thread grabberThread;
        RecievedData RecData;
        
    public:
        bool GetPackage();
        int samplesSinceSync;
        void PlaySync();
        void GetSync();
        DataLinkLayer();
        void PlayTones(string Tones);
        bool DataAvaliable();
        char GetNextTone();


};

void toneGrabber(DataLinkLayer * DaLLObj);
template <typename Type>
bool ArrayComp(Type *Array1, Type *Array2,int size,int index=0);