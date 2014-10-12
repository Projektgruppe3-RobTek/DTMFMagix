#include "RingBuffer.h"
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <sys/time.h>
#include <unistd.h>
#define mediafile "./media.txt"
#define newmediafile "./newmedia.txt"
class physicalLayer
{
    private:
        bool getNewState();
        void setNewState(bool state);
        std::vector<bool> getData();
        void setData(std::vector<bool> data);
        void startTimer();
        int getTimer();
    public:
        physicalLayer();
        void QueueFrame(std::vector<bool> );
        bool isQueueFull();
        bool isFrameAvaliable();
        std::vector<bool> getFrame(); 
        void FrameGrabber();
        void FrameSender();
        
    private:
        RingBuffer<std::vector<bool>,100> inBuffer;
        RingBuffer<std::vector<bool>,100> outBuffer;
        std::thread sendert;
        std::thread recievert;
        timeval timer;
        
};

void frameGrabWrapper(physicalLayer * physLayerObj);
void frameSendWrapper(physicalLayer * physLayerObj);
