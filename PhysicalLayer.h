#pragma once
#include "Recorder.h"
#include "Player.h"
#include "Goertzel.h"
#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include <thread>
#define TONELENGTH 35
#define SILENTLENGTH 2
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#define SYNCLENGHT 3
#define SYNCREPEAT 5
using namespace std;

class   PhysicalLayer
{
private:
	void			applyHamming(vector<float> &in);
	vector<char>	applyPreambleTrailer(vector<char>);
	vector<char>	convertToDTMF(vector<bool>);
	vector<bool>	convertToBinary(vector<char>);
    bool            stop=false;

public:
    PhysicalLayer();
    bool            dataAvailable();
    bool            layerBusy();
    vector<bool>    popData();
    void            pushData(vector<bool>);
    void            getFrame();
    void            playFrame();
    ~PhysicalLayer();

private:

    int             Freqarray1[4]={697,770,852,941};
    int             Freqarray2[4]={1209,1336,1477,1633};
    char            SyncSequence[SYNCLENGHT]={'A','6','8'}; //Should not contain the same note two times in a row.
    char            startFlag[4]={'1','2','3','4'};
    char            endFlag[4]={'*','8','6','A'};
    char            DTMFTones[16]={'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
    DTMFPlayer      Player;
    DTMFRecorder    Rec;
    long long       synctime;
    bool            receiveFlag;
    bool            sendFlag;
    thread          incommingThread;
    thread          outgoingThread;
    vector<char>    incommingFrame;
    vector<char>    outgoingFrame;
    timeval         tv;
    bool            bugfix=false;
};
void getFrameWrapper(PhysicalLayer * DaLLObj);
void playFrameWrapper(PhysicalLayer * DaLLObj);
template <typename Type>
bool ArrayComp(Type *Array1, Type *Array2,int size,int index=0);
