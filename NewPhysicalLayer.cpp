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


vector<char> NewPhysialLayer::convertToDTMF(vector<bool> bFrame)
{
    vector<char> dFrame((bFrame.size())/4);
    for (int i = 0; i < dFrame.size(); i++)
    {
        bool a = bFrame[i*4];
        bool b = bFrame[i*4+1];
        bool c = bFrame[i*4+2];
        bool d = bFrame[i*4+3];

        dFrame[i] = DTMFTones[a*8+b*4+c*2+d];
    }
    return dFrame
}

vector<bool> NewPhysialLayer::convertToBinary(vector<char> dFrame)
{
    vector<bool> bFrame((dFrame.size())*4);
    bool ref[64] = {0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,1,0,1,0,0,0,1,0,1,0,1,1,0,0,1,1,1,1,0,0,0,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,1,1,1};

    for (int j = 0; j < dFrame.size();j++)
    {
        int i = 0;
        while (DTMFTones[i] != dFrame[j])
        {
            i++;
        }
        for (int k = 0; int k < 4; k++)
        {
            bFrame[j+k] = ref[i*4+k];
        }
    }
    return bFrame;
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
