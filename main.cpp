#include "Recorder.h"
#include "Player.h"
#include <SDL2/SDL_version.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include "Goertzel.h"
#define SIZEX 1200
#define SIZEY 600
#define TONELENGHT 100
#define SILENTLENGHT 0
#define SILENTLIMIT 3.0f
#define M_PI 3.14159265359
int Freqarray1[]={697,770,852,941};
int Freqarray2[]={1209,1336,1477,1633};
char SyncSequence[]={'1','d','2','#'}; //Should not contain the same note two times in a row.
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
bool ArrayComp(Type *Array1, Type *Array2,int size)
{
    for(int i=0;i<size;i++)
    {
    if( Array1[i]!=Array2[i]) return false;
    }
    return true;
}

void PlaySync(DualTonePlayer &Player)
{
    for (int i=0;i<4;i++) {Player.PlayDTMF(SyncSequence[i],TONELENGHT); Player.PlayDTMF(' ',SILENTLENGHT); }
}

long long GetSync(Recorder Rec,float *in) //Sync on the SyncSequence and output the starttime of the last tone in the sequence.
{
    char RecordedSequence[4];
    int SequenceCounter=0;
    char LastNote;
    float LastMax;
    while(!ArrayComp(RecordedSequence,SyncSequence,4))
    {
        Rec.GetAudioData(TONELENGHT,0,in);
        float freq1max=0;
        int freq1Index;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(SAMPLE_RATE*TONELENGHT/1000,Freqarray1[k],SAMPLE_RATE,in);
            if (gMag>freq1max) {freq1max=gMag; freq1Index=k;}
        }
        if (freq1max<SILENTLIMIT) continue;
        float freq2max=0;
        int freq2Index;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(SAMPLE_RATE*TONELENGHT/1000,Freqarray2[k],SAMPLE_RATE,in);
            if (gMag>freq2max) {freq2max=gMag; freq2Index=k;}
        }
        if (freq2max<SILENTLIMIT) continue;
        
    }
}

int main(int argc, char **argv)
{
    srand(time(0));
    print_version();
    
    DualTonePlayer Player;
    Recorder Rec;
    PlaySync(Player);
    for(int i=0;i<100;i++) for(int j=0;j<16;j++) {Player.PlayDTMF(DTMFTones[j],TONELENGHT); Player.PlayDTMF(' ',SILENTLENGHT); }
    
    float *in;
    in = new float[SAMPLE_RATE*TONELENGHT/1000];
    //long long synctime=GetSync(Rec,in);
    while(true)
    {   
        Rec.GetAudioData(TONELENGHT,0,in);
        float max=0;
        int freq1Index;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(SAMPLE_RATE*TONELENGHT/1000,Freqarray1[k],SAMPLE_RATE,in);
            cout << gMag << endl;
            if (gMag>max) {max=gMag; freq1Index=k;}
        }
        max=0;
        int freq2Index;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(SAMPLE_RATE*TONELENGHT/1000,Freqarray2[k],SAMPLE_RATE,in);
            if (gMag>max) {max=gMag; freq2Index=k;}
        }
        Pa_Sleep(10);
        //cout << DTMFTones[freq1Index*4+freq2Index] << endl;
        
    }
    
}
