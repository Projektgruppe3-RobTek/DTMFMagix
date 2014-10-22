#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>

const float PI = 3.14159265358979323846;
const int AMPLITUDEaudio = 28000;
const int FREQUENCYaudio = 4000;   //Highest freq = 1633*2 = 3266 Hz min. sampling rate

class DTMFPlayer{
    private:
        double v1;
        double v2;
        SDL_AudioSpec obtainedSpec;
    public:
        DTMFPlayer();
        ~DTMFPlayer();
        void beep(float freq1, float freq2, int duration);
        void generateSamples(Sint16 *stream, int lenght);
        void WaitForFinish();
        void PlayDTMF(char tone, int duration);

};
void DTMFPlayerCallBack(void *DualToneP, Uint8 *_stream, int _length);

