#include "NewPlayer.h"
using namespace std;


DTMFPlayer::DTMFPlayer()
{
    SDL_AudioSpec desiredSpec;

    desiredSpec.freq =FREQUENCYaudio;//Samples per second
    desiredSpec.format = AUDIO_S16SYS; //Audio format Signed 16bit native
    desiredSpec.channels = 1; //Seperate audio channels
    desiredSpec.samples = 1024; //Buffer size
    desiredSpec.callback = DTMFPlayerCallBack; //Callback function
    desiredSpec.userdata = this;
    SDL_OpenAudio(&desiredSpec, &obtainedSpec);
    //Start audio
    SDL_PauseAudio(0);

}


DTMFPlayer::~DTMFPlayer()
{
    SDL_CloseAudio();
}


void DTMFPlayerCallBack(void *DualToneP, Uint8 *_stream, int _length)
{
    /*
    Sint16 *stream =(Sint16*) _stream;
    int length=_length/2;
    DualTonePlayer *Player =(DualTonePlayer *) DualToneP;
    Player->generateSamples(stream,length);
    */
}


void DTMFPlayer::generateSamples(Sint16 *stream, int length)
{
    /*
    int i=0;
    while(i<length)
    {
        if (beeps.empty())
        {
            while(i<length)
            {
                stream[i]=0; //Fill with zero if no beeps are waiting
                i++;
            }

            return;
        }
        BeepObject& bo=beeps.front(); //Grab next beep
        int SamplesToDo=min(i+bo.samplesLeft,length);
        bo.samplesLeft-=SamplesToDo-i;

        while(i<SamplesToDo)
        {
            stream[i]=AMPLITUDE*(sin(v1*2*M_PI/FREQUENCY)+sin(v2*2*M_PI/FREQUENCY))/2;
            i++;
            v1+=bo.freq1;
            v2+=bo.freq2;
        }
        if (bo.samplesLeft==0) beeps.pop();
    } */
}


void DTMFPlayer::beep(float freq1,float freq2,int duration)
{
    /*
    if (duration==0) return;
    BeepObject bo;
    bo.freq1=freq1;
    bo.freq2=freq2;
    bo.samplesLeft=duration*FREQUENCY/1000;
    SDL_LockAudioDevice(1);
    beeps.push(bo);
    SDL_UnlockAudioDevice(1);
    */
}


void DTMFPlayer::WaitForFinish() //Wait until current beep queue is empty
{
    /*
    int size;
    do
    {
        SDL_Delay(1);
        SDL_LockAudioDevice(1);
        size=beeps.size();
        SDL_UnlockAudioDevice(1);
    } while (size>0);
*/
}


void DTMFPlayer::PlayDTMF(char tone,int duration)
{
    /*
    if (tone=='1') beep(697,1209,duration);
    else if (tone=='2') beep(697,1336,duration);
    else if (tone=='3') beep(697,1477,duration);
    else if (tone=='a' or tone=='A') beep(697,1633,duration);

    else if (tone=='4') beep(770,1209,duration);
    else if (tone=='5') beep(770,1336,duration);
    else if (tone=='6') beep(770,1477,duration);
    else if (tone=='b' or tone=='B') beep(770,1633,duration);

    else if (tone=='7') beep(852,1209,duration);
    else if (tone=='8') beep(852,1336,duration);
    else if (tone=='9') beep(852,1477,duration);
    else if (tone=='c' or tone=='C') beep(852,1633,duration);

    else if (tone=='*') beep(941,1209,duration);
    else if (tone=='0') beep(941,1336,duration);
    else if (tone=='#') beep(941,1477,duration);
    else if (tone=='d' or tone=='D') beep(941,1633,duration);
    else if (tone==' ') beep(0,0,duration);
    else cout << "Not a vaild DTMF tone!" << endl;
    */
}

