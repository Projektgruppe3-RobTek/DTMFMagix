
#include "Player.h"
using namespace std;


DTMFPlayer::DTMFPlayer()
{
    // Setup audio device with desired specs.
    SDL_AudioSpec desiredSpec;

    desiredSpec.freq =FREQUENCYaudio;           //Samples per second
    desiredSpec.format = AUDIO_S16SYS;          //Audio format Signed 16bit native
    desiredSpec.channels = AUDIOCHANNELS;       //Mono audio
    desiredSpec.samples = BUFFERaudio;          //Buffer size
    desiredSpec.callback = DTMFCallBack;        //Callback function
    desiredSpec.userdata = this;                //Data space

    // Check if desired specs is obtained for audio device
    if(SDL_OpenAudio(&desiredSpec, &obtainedSpec)==0)
    {
        //cout << "Desired specs obtained" << endl;
    }
    else
    {
        //cout << "Could not obtain desired specs" << endl;

    }

}


DTMFPlayer::~DTMFPlayer()
{
    SDL_CloseAudio();               // Close audio device
    delete instance;
}



//This syntax is lend from the internet, and makes it possible to use SDL audio inside a class.
//http://stackoverflow.com/questions/10110905/simple-wave-generator-with-sdl-in-c
// CHECK UP ON THIS, some sort of cast?
 void DTMFCallBack(void *DualToneP, Uint8 *_stream, int _length)
{

    Sint16 *stream =(Sint16*) _stream;
    int length=_length/2;
    DTMFPlayer *Player =(DTMFPlayer *) DualToneP;
    Player->generateSamples(stream,length);
}
DTMFPlayer* DTMFPlayer::instance = nullptr;
DTMFPlayer *DTMFPlayer::getInstance()
{
	if(instance==nullptr) instance = new DTMFPlayer;
	return instance;
}


void DTMFPlayer::generateSamples(Sint16 *stream, int length)
{
    // Generating samples to output buffer
    streamPlace=0;
    sampleTurn = 0;
    while(streamPlace < length)                     // Run until buffer is full.
    {
        if (toneNumber >= (totalTones))             //Fill rest of buffer, if no tones left.
        {
            zeroFill(stream,length);
            return;
        }
        else if(samplesLeft ==0)
        {

            if (delayFlag == false)
            {
                toneSetup();
            }
            else
            {
                delaySetup();
            }
        }
        else
        {


            // Adjust number of samples for current buffer-run
            sampleAdjust(length);

            // Run for current fillup.
            while (sampleRun > 0)           // Run for current fillup (buffer full, end of tone/delay)
            {
                if(endOfTone < 3)          // Fade out 1/10 for each last 10 samples.
                {
                    fadeOut -= (1./3.);
                }
                if(startOfTone < 3)        // Fade in 1/10 for each first 10 samples.
                {
                    fadeIn +=(1./3.);
                }
                // Generate samples for buffer (sinus generator)
                stream[streamPlace] = fadeIn*fadeOut*AMPLITUDEaudio*(sin(sinStep1*2*M_PI/FREQUENCYaudio)+sin(sinStep2*2*M_PI/FREQUENCYaudio))/2;
                streamPlace++;
                sampleRun--;
                sinStep1 += freq1;
                sinStep2 += freq2;
                startOfTone++;
                endOfTone--;
            }

            if (samplesLeft == 0 && delayFlag == 0)         // End of tone, set for delay.
            {
                delayFlag = true;
            }
            else if(samplesLeft == 0 && delayFlag == true)  // End of delay, set for tone.
            {
                delayFlag = false;
            }
        }
    }
}



void DTMFPlayer::WaitForFinish() //Wait until all tones have been played.
{
    int size;
    do
    {
        SDL_Delay(1);
        SDL_LockAudioDevice(1);
        size=totalTones-(toneNumber);
        SDL_UnlockAudioDevice(1);
    } while (size>0);
}

void DTMFPlayer::toneSetup()
{
    // Setup for next tone in input vector.
                freq1 = Tones.getFreqL(toneIndput[toneNumber]);
                freq2 = Tones.getFreqH(toneIndput[toneNumber]);
                samplesLeft = toneLength;

                sinStep1 = 0;
                sinStep2 = 0;

                endOfTone = toneLength;
                fadeOut = 1.0;
                startOfTone = 0;
                fadeIn = 0.0;
}

void DTMFPlayer::delaySetup()
{
    // Setup for delay between tones.
                toneNumber++;
                freq1 = 0;
                freq2 = 0;
                samplesLeft = delayLength;
                sinStep1 = 0;
                sinStep2 = 0;
}

void DTMFPlayer::sampleAdjust(int length)
{
    // Adjust number of samples for current buffer fillup.
            if ((int)samplesLeft< length-sampleTurn)
            {
                sampleRun = samplesLeft;
            }
            else
            {
                sampleRun = length-sampleTurn;
            }

            samplesLeft -= sampleRun;
            sampleTurn += sampleRun;
}

void DTMFPlayer::zeroFill(Sint16 *stream, int length)
{
    while(streamPlace<length)
            {
                stream[streamPlace] = 0;
                streamPlace++;
            }
}


void DTMFPlayer::PlayDTMF(vector<char> tones,int duration,int delay)
{
    // Method called by Physical layer.
    toneIndput = tones;
    toneNumber = 0;
    totalTones = tones.size();
    toneLength = duration*FREQUENCYaudio/1000;
    delayLength = delay*FREQUENCYaudio/1000;
    samplesLeft = 0;
    delayFlag = false;


    SDL_PauseAudio(0);                  //Start audio
    DTMFPlayer::WaitForFinish();        // Wait until finished.
    SDL_PauseAudio(1);                  // Pause audio device

}

