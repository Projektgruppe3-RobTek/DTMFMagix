#include<portaudio.h>
#include<iostream>
#include<cmath>
#include<array>
#include <ctime>
#include <vector>
#include <fftw3.h>
#define SAMPLE_RATE 44100
#define RECBUFLENGHT 10
#define M_PI 3.14159265359
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
    void GetAudioData(int lenght ,int OffSet,float * outarray);

};
