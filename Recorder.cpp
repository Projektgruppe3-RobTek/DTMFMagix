#include "Recorder.h"


DTMFRecorder::DTMFRecorder()
{
    err = Pa_Initialize();			// Initialize portaudio
    if (err != paNoError)
    {
        cout << Pa_GetErrorText(err) << endl;
        err = Pa_Terminate();
    }

     err = Pa_OpenDefaultStream(  &stream,
                            INPUTCHANNELS, /* input channels */
                           OUTPUTCHANNELS, /* output channels */
                                paFloat32, /* 32 bit floating point output */
                          REC_SAMPLE_RATE, /* Sample Rate */
                          REC_BUFFER_SIZE, /* frames per buffer */
                              RecCallback, /* this is your callback function */
                                  &RecData ); /* Point passed to callback */

    if (err != paNoError)			// Check for errors
    {
        cout << Pa_GetErrorText(err) << endl;
        err = Pa_Terminate();
    }

    for (int i = 0;i<INPUTBUFFERSIZE;i++)	// Fill input buffer with 0
   {
	RecData.recBuff[i] = 0;
   }

    err = Pa_StartStream(stream);		// Start recording and check for error
    if (err != paNoError)
    {

        cout << Pa_GetErrorText(err) << endl;
        err = Pa_Terminate();
    }
    


}
DTMFRecorder* DTMFRecorder::instance = nullptr;
DTMFRecorder *DTMFRecorder::getInstance()
{
	if(instance==nullptr) instance = new DTMFRecorder;
	return instance;
}
// Callback function
int DTMFRecorder::RecCallback( const void *inputBuffer,void *outputBuffer,unsigned long framePerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,
                    void *userData )
{
    
    // Cast container to callback function
    RecordingContainer *input = (RecordingContainer*)userData;
    float *in = (float*)inputBuffer;

    (void) outputBuffer;        /* Prevent unused variable warning. */

    //Fill container with recording (ringbuffer)
    for(unsigned int i = 0; i< framePerBuffer; i++)
    {
        input->recBuff[input->streamPlace] = in[i];
        input->streamPlace++;
        if(input->streamPlace == INPUTBUFFERSIZE)
        {
            input->streamPlace =0;
        }
    }
    return paContinue;


}
// Method called by Physical layer
vector<float> DTMFRecorder::GetAudioData(int lenght ,int OffSet)
{
    Samples.clear();   // Clear vector for previous samples
    Samples.reserve(REC_SAMPLE_RATE*lenght/1000); //Set correct vector lenght.
   
    // set pointer to correct place
    collect = (RecData.streamPlace - (REC_SAMPLE_RATE*(lenght+OffSet)/1000));
	if(collect <0)
    {
    collect += INPUTBUFFERSIZE;
    }
   
    // copy samples from container to vector. 
for (int i =0; i< REC_SAMPLE_RATE*lenght/1000;i++)
    {
	      	
	Samples.push_back(RecData.recBuff[collect]);
        collect++;
	if (collect == INPUTBUFFERSIZE)
        {
            collect = 0;
        }
    }
	
    

    return Samples;




}

DTMFRecorder::~DTMFRecorder()
{
    // Free space on head and terminate recorder.
    err = Pa_CloseStream( stream );
    if( err != paNoError )
    {
        cout << Pa_GetErrorText(err) << endl;
    }
    err = Pa_Terminate();
    if (err !=  paNoError)
    {
        cout << Pa_GetErrorText(err) << endl;
    }
    delete [] RecData.recBuff;
    delete instance;
}


