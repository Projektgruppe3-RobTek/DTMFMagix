#include "NewRecorder.h"


DTMFRecorder::DTMFRecorder()
{

    PaStream *stream;
    err = Pa_Initialize();
    if (err != paNoError)
    {
        cout << Pa_GetErrorText(err) << endl;
        err = Pa_Terminate();
    }

     err = Pa_OpenDefaultStream(  &stream,
                                        1, /* input channels */
                                        0, /* output channels */
                                paFloat32, /* 32 bit floating point output */
                          REC_SAMPLE_RATE, /* Sample Rate */
                                      256, /* frames per buffer */
                           RecCallback, /* this is your callback function */
                                  &RecData ); /* Point passed to callback */

    if (err != paNoError)
    {
        cout << Pa_GetErrorText(err) << endl;
        err = Pa_Terminate();
    }

    err = Pa_StartStream(stream);
    if (err != paNoError)
    {

        cout << Pa_GetErrorText(err) << endl;
        err = Pa_Terminate();
    }


}

int DTMFRecorder::RecCallback( const void *inputBuffer,void *outputBuffer,unsigned long framePerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,
                    void *userData )
{
    RingBuffer<float,INPUTBUFFERSIZE> *data = (RingBuffer<float,INPUTBUFFERSIZE>*)userData;
    float *in = (float*)inputBuffer;
    (void) outputBuffer;        /* Prevent unused variable warning. */

    for(unsigned int i = 0; i< framePerBuffer; i++)
    {
    data->push_back(in[i]);
    }
    return paContinue;


}

vector<float> DTMFRecorder::GetAudioData(int lenght ,int OffSet)
{
    vector<float> Samples;   // Output vector
    RingBuffer<float,INPUTBUFFERSIZE> copyBuf = RecData; //make a copy of Input buffer
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
    err = Pa_Terminate();
    if (err !=  paNoError)
    {
        cout << Pa_GetErrorText(err) << endl;
    }
}
