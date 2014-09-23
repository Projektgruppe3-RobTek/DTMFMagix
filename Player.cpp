#include "Player.h"
using namespace std;
DualTonePlayer::DualTonePlayer()
{
    SDL_AudioSpec aSpec;
    
    aSpec.freq =FREQUENCY;//Samples per second
    aSpec.format = AUDIO_S16SYS; //Audio format Signed 16bit native
    aSpec.channels=1; //Seperate audio channels
    aSpec.samples=1024; //Buffer size
    aSpec.callback=DualTonePlayerCallBack;
    aSpec.userdata=this;
    SDL_OpenAudio(&aSpec, &obtainedSpec);
    //Start audio
    SDL_PauseAudio(0);
}
DualTonePlayer::~DualTonePlayer()
{
    SDL_CloseAudio();
}

void DualTonePlayerCallBack(void *DualToneP, Uint8 *_stream, int _length)
{
    Sint16 *stream =(Sint16*) _stream;
    int length=_length/2;
    DualTonePlayer *Player =(DualTonePlayer *) DualToneP;
    Player->generateSamples(stream,length);
}
void DualTonePlayer::generateSamples(Sint16 *stream, int length)
{
    int i=0;
    while(i<length)
    {
        if (beeps.empty()) 
        {
            while(i<length)
            {
                stream[i]=0;
                i++;
            }
        
            return;
        }
        BeepObject& bo=beeps.front();
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
    }
}
void DualTonePlayer::beep(float freq1,float freq2,int duration)
{
    BeepObject bo;
    bo.freq1=freq1;
    bo.freq2=freq2;
    bo.samplesLeft=duration*FREQUENCY/1000;
    SDL_LockAudioDevice(1);
    beeps.push(bo);
    SDL_UnlockAudioDevice(1);
}
void DualTonePlayer::WaitForFinish() //Wait until current beep queue is empty
{
    int size;
    do
    {
        SDL_Delay(1);
        SDL_LockAudioDevice(1);
        size=beeps.size();
        SDL_UnlockAudioDevice(1);
    } while (size>0);
    
}
void DualTonePlayer::PlayDTMF(char tone,int duration)
{
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
    else cout << "Not a vaild DTMF tone!" << endl;
    
}
