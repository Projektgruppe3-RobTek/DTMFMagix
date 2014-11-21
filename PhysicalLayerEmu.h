#include "RingBuffer.h"
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#define mediafile "./media.txt"
#define newmediafile "./newmedia.txt"
class PhysicalLayer
{
    private:
        bool getNewState();
        void setNewState(bool state);
        std::vector<bool> getData();
        void setData(std::vector<bool> data);
        void startTimer();
        int getTimer();
        int framenumber;
    public:
        PhysicalLayer();
        void pushData(std::vector<bool> );
        bool layerBusy();
        bool dataAvailable();
        std::vector<bool> popData(); 
        void FrameGrabber();
        void FrameSender();
        
    private:
        RingBuffer<std::vector<bool>,1> inBuffer;
        RingBuffer<std::vector<bool>,1> outBuffer;
        std::thread sendert;
        std::thread recievert;
        timeval timer;
        
};

void frameGrabWrapper(PhysicalLayer * physLayerObj);
void frameSendWrapper(PhysicalLayer * physLayerObj);

