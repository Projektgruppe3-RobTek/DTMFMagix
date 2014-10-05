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
#define TONELENGHT 100
#define SILENTLENGHT 20
#define SILENTLIMIT 0.0000f
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
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

};
struct FrameBuffer
{
    array<string,100> Buffer;
    int NextFrameToRecord=0;
    int LastFrameElement=0;
};
class physicalLayer
{
    private:
        void applyHamming(vector<float> &in);
        void GetSync();
        void GetFrame();
        void PlaySync();
        void PlayFrame(string);
        void PlayEndSequence();
    public:
        physicalLayer();
        void QueueFrame(string frame);
        bool isFrameAvaliable();
        void frameGrabber();
        void frameSender();
        string GetNextFrame();
    private:
        int Freqarray1[4]={697,770,852,941};
        int Freqarray2[4]={1209,1336,1477,1633};
        char SyncSequence[4]={'a','6','8','*'}; //Should not contain the same note two times in a row.
        char SyncEnd[4]={'1','2','3','4'};
        char EndSequence[4]={'*','8','6','a'};
        char DTMFTones[16]={'1','2','3','a','4','5','6','b','7','8','9','c','*','0','#','d'};
        DualTonePlayer Player;
        Recorder Rec;
        int samplesSinceSync=0;
        long long synctime;
        thread frameGrabberThread;
        thread frameSenderThread;
        FrameBuffer outBuffer;
        FrameBuffer inBuffer;
        
};
void frameGrabWrapper(physicalLayer * DaLLObj);
void frameSendWrapper(physicalLayer * DaLLObj);
template <typename Type>
bool ArrayComp(Type *Array1, Type *Array2,int size,int index=0);
