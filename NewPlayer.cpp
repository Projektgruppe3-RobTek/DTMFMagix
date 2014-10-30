
#include "NewPlayer.h"
using namespace std;


DTMFPlayer::DTMFPlayer()
{
    SDL_AudioSpec desiredSpec;

    desiredSpec.freq =FREQUENCYaudio;//Samples per second
    desiredSpec.format = AUDIO_S16SYS; //Audio format Signed 16bit native
    desiredSpec.channels = 1; //Seperate audio channels
    desiredSpec.samples = 512; //Buffer size
    desiredSpec.callback = DTMFCallBack; //Callback function
    desiredSpec.userdata = this;
    if(SDL_OpenAudio(&desiredSpec, &obtainedSpec)==0)
    {
        cout << "Desired specs obtained" << endl;
    }
    else
    {
        cout << "Could not obtain desired specs" << endl;
    }

    //SDL_PauseAudio(0);
}


DTMFPlayer::~DTMFPlayer()
{
    SDL_CloseAudio();
}



//This syntax is lend from the internet, and makes it possible to use SDL audio inside a class.
//http://stackoverflow.com/questions/10110905/simple-wave-generator-with-sdl-in-c
 void DTMFCallBack(void *DualToneP, Uint8 *_stream, int _length)
{
   // cout << "CallBack" << endl;
    Sint16 *stream =(Sint16*) _stream;
    int length=_length/2;
    DTMFPlayer *Player =(DTMFPlayer *) DualToneP;
    Player->generateSamples(stream,length);
}





void DTMFPlayer::generateSamples(Sint16 *stream, int length)
{
    cout << "Genelate samples" << endl;
    streamPlace=0;
    sampleTurn = 0;
    while(streamPlace < length)
    {
        if (toneNumber >= (totalTones))
        {
            cout << "fill with 0" << endl;
            while(streamPlace<length)
            {
                stream[streamPlace] = 0;
                streamPlace++;
            }
            return;
        }
        else if(samplesLeft ==0)
        {

            if (delayFlag == false)
            {


                cout << "new tone" << endl;
                freq1 = Tones.getFreqL(toneIndput[toneNumber]);
                freq2 = Tones.getFreqH(toneIndput[toneNumber]);
                samplesLeft = toneLength;
                sinStep1 = 0;
                sinStep2 = 0;
            }
            else
            {
                cout << "delay" << endl;
                toneNumber++;
                freq1 = 0;
                freq2 = 0;
                samplesLeft = delayLength;
                sinStep1 = 0;
                sinStep2 = 0;
            }
        }
        else
        {
            cout << "Fill buffer" << endl;
            if (samplesLeft< length-sampleTurn)
            {
                sampleRun = samplesLeft;
            }
            else
            {
                sampleRun = length-sampleTurn;
            }

            samplesLeft -= sampleRun;
            sampleTurn += sampleRun;

            while (sampleRun > 0)
            {
                stream[streamPlace] = AMPLITUDEaudio*(sin(sinStep1*2*M_PI/FREQUENCYaudio)+sin(sinStep2*2*M_PI/FREQUENCYaudio))/2;
                streamPlace++;
                sampleRun--;
                sinStep1 += freq1;
                sinStep2 += freq2;
            }
            if (samplesLeft == 0 && delayFlag == 0)
            {
                delayFlag = true;
            }
            else if(samplesLeft == 0 && delayFlag == true)
            {
                delayFlag = false;
            }
        }
        cout << "sampleTurn" << sampleTurn << endl;
        cout << "samplesLeft" << samplesLeft << endl;


    }
}


void DTMFPlayer::beep(float freq1,float freq2,int duration)
{


}


void DTMFPlayer::WaitForFinish() //Wait until current beep queue is empty
{
    cout << "WaitForFinish" << endl;
    int size;
    do
    {
        SDL_Delay(1);
        SDL_LockAudioDevice(1);
        size=totalTones-(toneNumber);
        SDL_UnlockAudioDevice(1);
    } while (size>0);
    cout << "Finished" << endl;
}


void DTMFPlayer::PlayDTMF(vector<char> tones,int duration,int delay)
{
    cout << "PlayDTMF" << endl;

    toneIndput = tones;
    toneNumber = 0;
    totalTones = tones.size();
    toneLength = duration*FREQUENCYaudio/1000;
    delayLength = delay*FREQUENCYaudio/1000;
    samplesLeft = 0;
    sinStep1 = 0;
    sinStep2 = 0;
    delayFlag = false;

//Start audio
    SDL_PauseAudio(0);
    DTMFPlayer::WaitForFinish();
    SDL_PauseAudio(1);
    cout <<"End PlayDTMF" <<endl;

}

