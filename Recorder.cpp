#include "Recorder.h"


DTMFRecorder::DTMFRecorder()
{

    PaStream *stream;               // Stream buffer
    err = Pa_Initialize();          // Initialize Port audio

    // Check for failed initaialization
    if (err != paNoError)
    {
        cout << Pa_GetErrorText(err) << endl;
        err = Pa_Terminate();
    }

    // Open PortAudio with desired specs.
     err = Pa_OpenDefaultStream(  &stream, /* stream buffer  */
                            INPUTCHANNELS, /* input channels */
                           OUTPUTCHANNELS, /* output channels */
                                paFloat32, /* 32 bit floating point output */
                          REC_SAMPLE_RATE, /* Sample Rate */
                            RECBUFFERSIZE, /* frames per buffer */
                              RecCallback, /* this is your callback function */
                                  &RecData ); /* Point passed to callback */
    // Check if desired specs is obtained.
    if (err != paNoError)
    {
        cout << Pa_GetErrorText(err) << endl;
        err = Pa_Terminate();
    }

    // Start reording
    err = Pa_StartStream(stream);

    // Check for errors
    if (err != paNoError)
    {

        cout << Pa_GetErrorText(err) << endl;
        err = Pa_Terminate();
    }


}

// Rec callback function, running in loops.
int DTMFRecorder::RecCallback( const void *inputBuffer,void *outputBuffer,unsigned long framePerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,
                    void *userData )
{

    // Casting from PortAudio to RingBuffer
    RingBuffer<float,INPUTBUFFERSIZE> *data = (RingBuffer<float,INPUTBUFFERSIZE>*)userData;
    float *in = (float*)inputBuffer;


    (void) outputBuffer;        /* Prevent unused variable warning. */

    // Keep recording and add input to RingBuffer, from inputbuffer
    for(unsigned int i = 0; i< framePerBuffer; i++)
    {
    data->push_back(in[i]);
    }
    return paContinue;


}

vector<float> DTMFRecorder::GetAudioData(int lenght ,int OffSet)
{
    vector<float> Samples;   // Output vector

    //Make a copy of Input buffer
    RingBuffer<float,INPUTBUFFERSIZE> copyBuf = RecData;

    // Check if requested samples is presented in RingBuffer
    if ((REC_SAMPLE_RATE*(OffSet+lenght)/1000) < INPUTBUFFERSIZE)
        {
            copyBuf.turn(-REC_SAMPLE_RATE*(OffSet+lenght)/1000);             //Turn ringbuffer for offset
        }
        else
        {
            cout << "OFFSet to big" << endl;
        }
    for(int i = 0; i< (REC_SAMPLE_RATE*lenght/1000);i++)
    {
        Samples.push_back(copyBuf.pop_front());                     // Copy desired samples to Samples
    }

    return Samples;


}

DTMFRecorder::~DTMFRecorder()
{
    // Close recording and check for errors
    err = Pa_Terminate();
    if (err !=  paNoError)
    {
        cout << Pa_GetErrorText(err) << endl;
    }
}
