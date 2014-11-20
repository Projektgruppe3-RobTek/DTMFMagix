#pragma once
#include "Recorder.h"
#include "Player.h"
#include "Goertzel.h"
#include <vector>
#include <thread>
#include <sys/time.h>
#include <unistd.h>
#include <thread>
#define TONELENGTH 75
#define SILENTLENGTH 25
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#pragma once
using namespace std;

class   PhysicalLayer
{
private:
	void			applyHamming(vector<float> &in);
	vector<char>	applyPreambleTrailer(vector<char>);
	vector<char>	convertToDTMF(vector<bool>);
	vector<bool>	convertToBinary(vector<char>);


public:
    PhysicalLayer();
    bool	    returnReceiveFlag();
    bool            returnSendFlag();
    vector<bool>    popFrame();
    void            pushFrame(vector<bool>);
    void            getFrame();
    void            playFrame();
    ~PhysicalLayer();

private:

    int             Freqarray1[4]={697,770,852,941};
    int             Freqarray2[4]={1209,1336,1477,1633};
    char            SyncSequence[4]={'A','6','8','*'}; //Should not contain the same note two times in a row.
    char            startFlag[4]={'1','2','3','4'};
    char            endFlag[4]={'*','8','6','A'};
    char            DTMFTones[16]={'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
    DTMFPlayer      Player;
    Recorder        Rec;
    long long       synctime;
    bool            receiveFlag;
    bool            sendFlag;
    thread          incommingThread;
    thread          outgoingThread;
    vector<char>    incommingFrame;
    vector<char>    outgoingFrame;
    timeval         tv;
    bool            bugfix=true;
};
void getFrameWrapper(PhysicalLayer * DaLLObj);
void playFrameWrapper(PhysicalLayer * DaLLObj);
template <typename Type>
bool ArrayComp(Type *Array1, Type *Array2,int size,int index=0);
