#include <iostream>
#include <portaudio.h>
#include "RingBuffer.h"
#include <vector>
#define INPUTBUFFERSIZE 8820000   // 2* sample rate
#define REC_SAMPLE_RATE 44100
#define SAMPLE_RATE 44100
#pragma once
using namespace std;


class DTMFRecorder{

public:
    DTMFRecorder();
    vector<float> GetAudioData(int lenght ,int OffSet);

    ~DTMFRecorder();


private:
    PaError err;
    RingBuffer<float,INPUTBUFFERSIZE> RecData;
    static int RecCallback( const void *inputBuffer,void *outputBuffer,unsigned long framePerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,
                    void *userData ) ;










};
