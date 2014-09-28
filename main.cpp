#include "Recorder.h"
#include "Player.h"
#include <SDL2/SDL_version.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include "Goertzel.h"
#define SIZEX 1200
#define SIZEY 600
int Freqarray1[]={697,770,852,941};
int Freqarray2[]={1209,1336,1477,1633};

using namespace std;
void print_version()
{
    SDL_version compiled;
    SDL_VERSION(&compiled);
    printf("We used SDL version %d.%d.%d ...\n",
         compiled.major, compiled.minor, compiled.patch);
    cout << "We used PortAudio version " << Pa_GetVersionText() << endl;
}


void makesin(int size,fftw_complex * array)
{
double c1=0;
double c2=0;
double c3=0;
for(int i=0; i<size;i++)
    {
    c1+=100;
    c2+=200;
    array[i][0]=sin(c1)+sin(c2);
    }
}
int main(int argc, char **argv)
{
    //ALLEGRO_DISPLAY *display = NULL;
    //if(!al_init() or !al_init_primitives_addon()) cout << "error" << endl;
    //display = al_create_display(SIZEX, SIZEY);
    srand(time(0));
    print_version();
    char DTMFTones[]={'1','2','3','a','4','5','6','b','7','8','9','c','*','0','#','d',' '};
    
    DualTonePlayer b;
    Recorder P;
    for(int i=0;i<100;i++) for(int j=0;j<16;j++) b.PlayDTMF(DTMFTones[j],60);
    int i=0;
    /*fftw_complex *in, *out;
    fftw_plan p;
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * SAMPLE_RATE*50/1000);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * SAMPLE_RATE*50/1000);
    p = fftw_plan_dft_1d(SAMPLE_RATE*50/1000, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    */int j=0;
    float *in;
    in = new float[SAMPLE_RATE*50/1000];
    while(true)
    {
        j++;
        //al_clear_to_color(al_map_rgb(0,0,0));
        P.GetAudioData(50,0,in);
        //fftw_execute(p);
        /*if(j%60==0) cout << endl;
        float prevmag=0;
        for(int i=0;i<SAMPLE_RATE*50/1000/2;i++)
        {
            float real=in[i][0];
            float img=in[i][1];
            float magnitude=in[i][0];
            al_draw_line(float(i-1)*float(SIZEX)/float(SAMPLE_RATE*50./1000./2.),(prevmag+1.0)*SIZEY/2,float(i)*float(SIZEX)/float(SAMPLE_RATE*50./1000./2.),(magnitude+1.0)*SIZEY/2,al_map_rgb(255,255,255),1);
            prevmag=magnitude;
            
            if (j%60==0) if (magnitude>50) cout << i*float(SAMPLE_RATE)/float(SAMPLE_RATE*50./1000.) << endl;
        }
        al_flip_display();*/
        float max=0;
        int max1num;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(SAMPLE_RATE*50/1000,Freqarray1[k],SAMPLE_RATE,in);
            if (gMag>max) {max=gMag; max1num=k;}
        }
        int max2num;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(SAMPLE_RATE*50/1000,Freqarray2[k],SAMPLE_RATE,in);
            if (gMag>max) {max=gMag; max2num=k;}
        }
        Pa_Sleep(10);
        cout << DTMFTones[max1num*4+max2num] << endl;
        
    }
    
}
