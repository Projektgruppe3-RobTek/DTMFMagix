#pragma once 
#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>
#include "DTMFTones.h"
#define  AMPLITUDEaudio 28000
#define  FREQUENCYaudio 4000   //Highest freq = 1633*2 = 3266 Hz min. sampling rate
#define  AUDIOCHANNELS 1       // 1 for Mono, 2 for Sterio
#define  BUFFERaudio 1024       // Buffersize for audio output.

using namespace std;
class DTMFPlayer{
    private:
        // All Uint32 could be regular int, but in case of extreme tone/delay lengths they are set to Uint32
        Uint32  sinStep1;
        Uint32  sinStep2;
        Uint32  samplesLeft;
        Uint32  toneLength;
        Uint32  delayLength;
        Uint32  endOfTone;
        Uint32  startOfTone;

        // DTMF frequencies, <2000 Hz
        int     freq1;
        int     freq2;

        // In case of extreme big buffer or number of tones, change to larger data type.
        int     streamPlace;
        int     sampleRun;
        int     sampleTurn;
        int     toneNumber;
        int     totalTones;

        bool    delayFlag;
        float   fadeOut;
        float  fadeIn;

        // Vector for inputTones.
        vector<char> toneIndput;

        SDL_AudioSpec obtainedSpec;     // Communication to hardware.
        DTMFTones Tones;                // DTMF class
		static DTMFPlayer* instance;
		DTMFPlayer();
    public:
        static DTMFPlayer* getInstance();
        ~DTMFPlayer();
        void generateSamples(Sint16 *stream, int lenght);
        void WaitForFinish();
        void PlayDTMF(vector<char> tones, int duration,int delay);

    private:
        void toneSetup();
        void delaySetup();
        void sampleAdjust(int length);
        void zeroFill(Sint16 *stream, int length);


};

// Method called by SDL audio, cannot be inside class.
void DTMFCallBack(void *DualToneP, Uint8 *_stream, int _length);


