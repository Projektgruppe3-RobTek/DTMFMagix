#include    <vector>
using namespace std;

class   NewPhysicalLayer
{
public:

    NewPhysicalLayer();
    bool            IsNewFrame();
    vector<bool>    PopFrame();

    bool            IsBufferFull();
    void            PushFrame(vector<bool>);



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
