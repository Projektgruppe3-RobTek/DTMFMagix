#include <iostream>
#include <portaudio.h>
#include "RingBuffer.h"
#include <vector>
#define INPUTBUFFERSIZE 882000   // 2* sample rate
#define REC_SAMPLE_RATE 8000
#define INPUTCHANNELS 1
#define OUTPUTCHANNELS 0
#define REC_BUFFER_SIZE 8

#pragma once
using namespace std;


class DTMFRecorder{

public:
    DTMFRecorder();
    vector<float> GetAudioData(int lenght ,int OffSet);

    ~DTMFRecorder();


private:
    PaError err;
    RingBuffer<float,INPUTBUFFERSIZE> *RecData;
    static int RecCallback( const void *inputBuffer,void *outputBuffer,unsigned long framePerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,
                    void *userData ) ;










};
