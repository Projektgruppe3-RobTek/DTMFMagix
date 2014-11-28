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

// Container for input	
typedef struct
{
    float *recBuff = new float[INPUTBUFFERSIZE];    // point to place en heap.
    int streamPlace=0;
} RecordingContainer;


class DTMFRecorder{

public:

    // Method called by Physical layer
    vector<float> GetAudioData(int lenght ,int OffSet);
    static DTMFRecorder* getInstance();
    ~DTMFRecorder();


private:
	static DTMFRecorder* instance;
	DTMFRecorder();
    PaStream* stream;
    PaError err;				// Error control
    RecordingContainer RecData;			// Create container
    int collect;
    vector<float> Samples;
    
    // Callback function for recorder
    static int RecCallback( const void *inputBuffer,void *outputBuffer,unsigned long framePerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,
                    void *userData ) ;

};
