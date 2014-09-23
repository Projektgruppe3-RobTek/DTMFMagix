#include "Recorder.h"
#include "Player.h"
#include <SDL2/SDL_version.h>
#include <stdio.h>
#include <cstdlib>
using namespace std;
void print_version()
{
    SDL_version compiled;
    SDL_VERSION(&compiled);
    printf("We used SDL version %d.%d.%d ...\n",
         compiled.major, compiled.minor, compiled.patch);
    cout << "We used PortAudio version " << Pa_GetVersionText() << endl;
}
int main()
{
    print_version();
    char DTMFTones[]={'1','2','3','a','4','5','6','b','7','8','9','c','*','0','#','d'};
    DualTonePlayer b;
    Recorder P;
    for(int i=0;i<200;i++) b.PlayDTMF(DTMFTones[rand()%16],30);
    b.WaitForFinish();
    auto audio=P.GetAudioData(1000);
     cout << audio.size() << endl;
    
}
