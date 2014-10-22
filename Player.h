#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include<queue>
#include<cmath>
#include <iostream>
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

const int AMPLITUDE = 28000;
const int FREQUENCY = 4000;   //Highest freq = 1633*2 = 3266 Hz min. sampling rate

struct BeepObject{
    float freq1;
    float freq2;
    int samplesLeft;
};


class DualTonePlayer{
    private:
        double v1;
        double v2;
        std::queue<BeepObject> beeps;
        SDL_AudioSpec obtainedSpec;
    public:
        DualTonePlayer();
        ~DualTonePlayer();
        void beep(float freq1, float freq2, int duration);
        void generateSamples(Sint16 *stream, int lenght);
        void WaitForFinish();
        void PlayDTMF(char tone, int duration);

};


void DualTonePlayerCallBack(void *DualToneP, Uint8 *_stream, int _length);
