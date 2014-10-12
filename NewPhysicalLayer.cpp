#include    "NewPhysicalLayer.h"

void physicalLayer::applyHamming(vector<float> &data)
{
    for(unsigned int i=0;i<data.size();i++)
    {
        data[i]*=0.54-0.46*cos((2*M_PI*i)/(data.size()-1));
    }
}

void physicalLayer::playSync() //Play the sync sequence
{
    for(int i=0;i<3;i++) //Times the sequenc is played. May need to be adjusted later.
    {
        for(int j=0;j<4;j++)
        {
            Player.PlayDTMF(SyncSequence[j],TONELENGHT);
            Player.PlayDTMF(' ',SILENTLENGHT);
        }
    }
}

void physicalLayer::playFrame(vector Tones)
{
    PlaySync();
    for(char Tone : Tones)
    {
        Player.PlayDTMF(Tone,TONELENGHT);
        Player.PlayDTMF(' ',SILENTLENGHT);
    }
    PlayEndSequence();
}

void get