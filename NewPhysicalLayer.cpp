#include    "NewPhysicalLayer.h"

NewPhysicalLayer::NewPhysicalLayer()
{

}

void NewPhysicalLayer::applyHamming(vector<float> &data)
{
    for(unsigned int i=0;i<data.size();i++)
    {
        data[i]*=0.54-0.46*cos((2*M_PI*i)/(data.size()-1));
    }
}

void NewPhysicalLayer::applyPreambleTrailer()
{

}


void NewPhysicalLayer::removePreambleTrailer()
{

}

vector<char> NewPhysialLayer::convertToDTMF(vector<bool> frame)
{

}

vector<bool> NewPhysialLayer::convertToBinary(vector<char> frame)
{
    
}

void NewPhysicalLayer::playFrame()
{
    for(int j=0;j<outgoingFrame.size();j++)
    {
        Player.PlayDTMF(outgoingFrame[j],TONELENGHT);
        Player.PlayDTMF(' ',SILENTLENGHT);
    }
}

void NewPhysicalLayer::getFrame()
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
 }       

bool NewPhysicalLayer::returnSendFlag()
{

}

bool NewPhysicalLayer::returnReceiveFlag()
{

}

bool NewPhysicalLayer::isFrameAvailable()
{

}

void NewPhysicalLayer::pushFrame(vector<bool> frame)
{
    outgoingFrame=convertToDTMF(frame);
    sendFlag=true;
}

vector<bool> NewPhysicalLayer::popFrame()
{

}