#include "Recorder.h"
#include "Player.h"
#include <SDL2/SDL_version.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include "Goertzel.h"
#include <vector>
#define SIZEX 1200
#define SIZEY 600
#define TONELENGHT 50
#define SILENTLENGHT 0
#define SILENTLIMIT 0
#define M_PI 3.14159265359
int Freqarray1[]={697,770,852,941};
int Freqarray2[]={1209,1336,1477,1633};
char SyncSequence[]={'1','2','3','4'}; //Should not contain the same note two times in a row.
char DTMFTones[]={'1','2','3','a','4','5','6','b','7','8','9','c','*','0','#','d',' '};

using namespace std;
void print_version()
{
    SDL_version compiled;
    SDL_VERSION(&compiled);
    printf("We used SDL version %d.%d.%d ...\n",
         compiled.major, compiled.minor, compiled.patch);
    cout << "We used PortAudio version " << Pa_GetVersionText() << endl;
}

template <typename Type> // this is the template parameter declaration
bool ArrayComp(Type *Array1, Type *Array2,int size,int index=0)
{
    for(int i=0;i<size;i++)
    {
    if( Array1[(i+index)%size]!=Array2[i]) return false;
    }
    return true;
}

void PlaySync(DualTonePlayer &Player)
{
    for(int k=0;k<1000;k++)
    for (int i=0;i<4;i++) {Player.PlayDTMF(SyncSequence[i],TONELENGHT); }
}
struct SyncData
{
float MaxMagnitude=0;
timeval tv;
};
long long GetSync(Recorder &Rec) //Sync on the SyncSequence and output the starttime of the last tone in the sequence.
{
    SyncData SData;
    char RecordedSequence[4];
    int SequenceCounter=0;
    char LastNote='!';
    while(!ArrayComp(RecordedSequence,SyncSequence,4,SequenceCounter))
    {
        auto in=Rec.GetAudioData(TONELENGHT,0);
        float freq1max=0;
        int freq1Index=0;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray1[k],SAMPLE_RATE,in);
            if (gMag>freq1max) {freq1max=gMag; freq1Index=k;}
        }
        if (freq1max<SILENTLIMIT) continue;
        float freq2max=0;
        int freq2Index=0;

        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray2[k],SAMPLE_RATE,in);
            if (gMag>freq2max) {freq2max=gMag; freq2Index=k;}
        }
        if (freq2max<SILENTLIMIT) continue;
        //cout << freq1Index << " " << freq2Index << endl;
        char Note=DTMFTones[freq1Index*4+freq2Index];
        if (Note==LastNote) 
        {
            if (freq1max+freq2max>SData.MaxMagnitude) gettimeofday(&SData.tv,NULL);
            //cout << Note << endl;
        }
        else
        {
            RecordedSequence[SequenceCounter]=Note;
            SequenceCounter=(SequenceCounter+1)%4;
            
            for(int j=0;j<4;j++) cout << RecordedSequence[j];
            cout << endl;
        }
        LastNote=Note;
        
    }
    return SData.tv.tv_sec*1000000+SData.tv.tv_usec;
}

int main(int argc, char **argv)
{
    srand(time(0));
    print_version();
    
    DualTonePlayer Player;
    Recorder Rec;
    PlaySync(Player);
    //for(int i=0;i<100;i++) for(int j=0;j<16;j++) {Player.PlayDTMF(DTMFTones[1],TONELENGHT); Player.PlayDTMF(' ',SILENTLENGHT); }
    
    long long synctime=GetSync(Rec);
    //We are now synced!
    cout << synctime << endl;
    return 0;
    char LastTone=' ';
    while(true)
    {   
        auto in=Rec.GetAudioData(TONELENGHT,0);
        float max=0;
        int freq1Index;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray1[k],SAMPLE_RATE,in);
            if (gMag>max) {max=gMag; freq1Index=k;}
        }
        max=0;
        int freq2Index;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray2[k],SAMPLE_RATE,in);
            if (gMag>max) {max=gMag; freq2Index=k;}
        }
        Pa_Sleep(10);
        auto Tone=DTMFTones[freq1Index*4+freq2Index];
        /*if (Tone!=LastTone)*/ cout << Tone << endl;
        LastTone=Tone;
        
    }
    
}
