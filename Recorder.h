#include <iostream>
#include <portaudio.h>
#include "RingBuffer.h"
#include <vector>
#define INPUTBUFFERSIZE 882000   // 2* sample rate
#define INPUTCHANNELS 1
#define OUTPUTCHANNELS 0
#define REC_SAMPLE_RATE 8000     // delay from played to obtainable in RingBuffer ~128/8000 ~= 16 ms
                                                               // (RECBUFFERSIZE/REC_SAMPLE_RATE)
#define RECBUFFERSIZE 128
#pragma once
using namespace std;


class DTMFRecorder{

public:
    DTMFRecorder();

    // Method called by Physical layer
    vector<float> GetAudioData(int lenght ,int OffSet);

    ~DTMFRecorder();


private:

    PaError err;                                // Error handling

    RingBuffer<float,INPUTBUFFERSIZE> RecData;  // Create ringbuffer for recorded data

    // PortAudio callback function
    static int RecCallback( const void *inputBuffer,void *outputBuffer,unsigned long framePerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,
                    void *userData ) ;


};
