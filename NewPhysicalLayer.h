#include    <vector>
using namespace std;

class   NewPhysicalLayer
{
private:
	void			applyHamming();
	void			applyEndFlags();
	void			removeEndFlags();
	vector<string>	convertToDTMF(vector<bool>);
	vector<bool>	convertToBinary(vector<string>);
	void			playSync();
	void			getSync();
	void			playFrame(vector);
	void			getFrame();

public:

    NewPhysicalLayer();
    bool			returnReceiveFlag();
    bool            isFrameAvailable();
    vector<bool>    popFrame();

    bool			returnSendFlag();
    void            pushFrame(vector<bool>);



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
