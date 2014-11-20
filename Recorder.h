#include<portaudio.h>
#include<iostream>
#include<cmath>
#include<array>
#include <ctime>
#include <vector>
#define SAMPLE_RATE 44100
#define RECBUFLENGHT 10
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
typedef struct
{
    std::array<float,SAMPLE_RATE*RECBUFLENGHT> recBuff; //don't do this! may cause lag! don't use dynamic memory. Maybe use a ringbuffer instead. Size?    
    int NextRec=0;
} PortAudioData;
class Recorder
{
private:
    static int patestCallback(  const void *inputBuffer,void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo *timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData );
    PaStream *stream;
    PortAudioData RecData;
public:
    Recorder();
    ~Recorder();
    std::vector<float> GetAudioData(int lenght ,int OffSet);

};
