#include 	"Recorder.h"
#include	"Player.h"
#include 	"Goertzel.h"
#include    <vector>
#include 	<thread>
using namespace std;




class   NewPhysicalLayer
{
private:
	void			applyHamming(vector<float>);
	void			applyPreambleTrailer();
	void			removePreambleTrailer();
	vector<char>	convertToDTMF(vector<bool>);
	vector<bool>	convertToBinary(vector<char>);
	void			playFrame(vector<bool>);
	void			getFrame();

public:

    NewPhysicalLayer();
    bool			returnReceiveFlag();
    bool            returnSendFlag();
    bool            isFrameAvailable();
    vector<bool>    popFrame();
    void            pushFrame(vector<bool>);

private:

    int             Freqarray1[4]={697,770,852,941};
    int             Freqarray2[4]={1209,1336,1477,1633};
    char            SyncSequence[4]={'a','6','8','*'}; //Should not contain the same note two times in a row.
    char            startFlag[4]={'1','2','3','4'};
    char            endFlag[4]={'*','8','6','a'};
    char            DTMFTones[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','*','#'};
    DualTonePlayer  Player;
    Recorder        Rec;
    int             samplesSinceSync=0;
    long long       synctime;
    bool            receiveFlag;
    bool            sendFlag;
    thread          incommingThread;
    thread          outgoingThread;
    vector<char>    incommingFrame;
    vector<char>    outgoingFrame;
};