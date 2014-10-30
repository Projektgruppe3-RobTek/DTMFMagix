
#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>
#include "DTMFTones.h"
const int AMPLITUDEaudio = 28000;
const int FREQUENCYaudio = 22100;   //Highest freq = 1633*2 = 3266 Hz min. sampling rate

class DTMFPlayer{
    private:
        Uint32  sinStep1;
        Uint32  sinStep2;
        int     freq1;
        int     freq2;
        int     streamPlace;
        Uint32  samplesLeft;
        int     sampleRun;
        int     sampleTurn;
        int     toneNumber;
        int     totalTones;
        Uint32  toneLength;
        Uint32  delayLength;
        bool    delayFlag;
        vector<char> toneIndput;
        SDL_AudioSpec obtainedSpec;
        DTMFTones Tones;

    public:
        DTMFPlayer();
        ~DTMFPlayer();
        void beep(float freq1, float freq2, int duration);
        void generateSamples(Sint16 *stream, int lenght);
        void WaitForFinish();
        void PlayDTMF(vector<char> tones, int duration,int delay);
        //static void (SDLCALL *callback)(void *userdata, Uint8 *stream, int len);


};
void DTMFCallBack(void *DualToneP, Uint8 *_stream, int _length);


