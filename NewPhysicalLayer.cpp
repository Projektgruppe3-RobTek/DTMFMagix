#include    "NewPhysicalLayer.h"

void NewPhysicalLayer::applyHamming(vector<float> &data)
{
    for(unsigned int i=0;i<data.size();i++)
    {
        data[i]*=0.54-0.46*cos((2*M_PI*i)/(data.size()-1));
    }
}

void NewPhysicalLayer::playSync() //Play the sync sequence
{
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<4;j++)
        {
            Player.PlayDTMF(SyncSequence[j],TONELENGHT);
            Player.PlayDTMF(' ',SILENTLENGHT);
        }
    }
}

void NewPhysicalLayer::playFrame(vector Tones)
{
    PlaySync();
    for(char Tone : Tones)
    {
        Player.PlayDTMF(Tone,TONELENGHT);
        Player.PlayDTMF(' ',SILENTLENGHT);
    }
    PlayEndSequence();
}

void NewPhysicalLayer::getSync()
{
    char RecordedSequence[4];
    int SequenceCounter=0;
    char previousNote='!'
    while(!ArrayComp(RecordedSequence,SyncSequence,4,SequenceCounter))
        vector<float> in=Rec.GetAudioData(TONELENGHT,0);
        applyHamming(in);
        float freq1max=0;
        int freq1Index=0;
        float freq2max=0;
        int freq2Index=0;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray1[k],SAMPLE_RATE,in);
            if (gMag>freq1max) {freq1max=gMag; freq1Index=k;}
        }
        
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray2[k],SAMPLE_RATE,in);
            if (gMag>freq2max) {freq2max=gMag; freq2Index=k;}
        }
        char Note=DTMFTones[freq1Index*4+freq2Index];
        if(!(Note==previousNote))
        {
            RecordedSequence[SequenceCounter]=Note;
            SequenceCounter=(SequenceCounter+1)%4;
            previousNote=Note
        }
        

void NewPhysicalLayer::getFrame()
{

}