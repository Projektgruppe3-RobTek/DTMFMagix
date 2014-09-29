#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include<queue>
#include<cmath>
#include <iostream>
#define M_PI 3.14159265359

const int AMPLITUDE =28000;
const int FREQUENCY =44100;

struct BeepObject
{
    float freq1;
    float freq2;
    int samplesLeft;
};


class DualTonePlayer
{
    private:
        double v1;
        double v2;
        std::queue<BeepObject> beeps;
        SDL_AudioSpec obtainedSpec;
    public:
        DualTonePlayer();
        ~DualTonePlayer();
        void beep(float freq1, float freq2,int duration);
        void generateSamples(Sint16 *stream,int lenght);
        void WaitForFinish();
        void PlayDTMF(char tone,int duration);
        
};


void DualTonePlayerCallBack(void *DualToneP, Uint8 *_stream, int _length);
