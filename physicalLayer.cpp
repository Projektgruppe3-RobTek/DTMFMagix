#include "physicalLayer.h"

using namespace std;

physicalLayer::physicalLayer()
{
    frameGrabberThread=thread(frameGrabWrapper,this);
    frameSenderThread=thread(frameSendWrapper,this);
}


void physicalLayer::applyHamming(vector<float> &data)
{
    for(unsigned int i=0;i<data.size();i++)
    {
        data[i]*=0.54-0.46*cos((2*M_PI*i)/(data.size()-1));
    }
}

void physicalLayer::GetSync()
{
    SyncData SData;
    char RecordedSequence[4];
    int SequenceCounter=0;
    char LastNote='!';
    int nonRaiseCounter=0;
    while(!ArrayComp(RecordedSequence,SyncSequence,4,SequenceCounter)) //Loop until we get syncsequence in buffer
    {
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
        //cout << Note << endl;
        if (Note==LastNote) 
        {
            if (freq1max+freq2max>SData.MaxMagnitude) 
            {
                gettimeofday(&SData.tv,NULL);
                nonRaiseCounter=0;
                
            }
            else
            {
                nonRaiseCounter++;
                //if (nonRaiseCounter>=5) usleep(SILENTLENGHT*1000);
                
            }
            usleep(1000);
        }
        else
        {
            //cout << (SData.tv.tv_sec*1000+SData.tv.tv_usec/1000) << endl;
            RecordedSequence[SequenceCounter]=Note;
            SequenceCounter=(SequenceCounter+1)%4;
            
            //for(int j=0;j<4;j++) cout << RecordedSequence[j];
            //cout << endl;
            LastNote=Note;
        }
    }
    //This is the synctime for the last tone in the samplesequence
    synctime=SData.tv.tv_sec*1000+SData.tv.tv_usec/1000+TONELENGHT;
    cout << synctime << endl;
    //Wait for syncend signal
    samplesSinceSync=0;
    while(!ArrayComp(RecordedSequence,SyncEnd,4,SequenceCounter))
    {
        timeval tv;
        gettimeofday(&tv,NULL);
        while(synctime+(TONELENGHT+SILENTLENGHT)*(samplesSinceSync+1)>=tv.tv_sec*1000+tv.tv_usec/1000)
        {
            usleep(1000);
            gettimeofday(&tv,NULL);
            }
        //cout << (tv.tv_sec*1000+tv.tv_usec/1000-synctime) << endl;
        samplesSinceSync++;
        auto in=Rec.GetAudioData(TONELENGHT,0);
        applyHamming(in);
        float max=0;
        int freq1Index;
        int freq2Index;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray1[k],SAMPLE_RATE,in);
            if (gMag>max) {max=gMag; freq1Index=k;}
        }
        if (max<SILENTLIMIT) continue;
        max=0;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray2[k],SAMPLE_RATE,in);
            if (gMag>max) {max=gMag; freq2Index=k;}
        }
        if (max<SILENTLIMIT) continue;
        char Note=DTMFTones[freq1Index*4+freq2Index];
        RecordedSequence[SequenceCounter]=Note;
        SequenceCounter=(SequenceCounter+1)%4;
           
        //for(int j=0;j<4;j++) cout << RecordedSequence[j];
        //cout << endl;
                
    }
    synctime=synctime+(TONELENGHT+SILENTLENGHT)*(samplesSinceSync+1);
    //This is the expected time for next tone
    
}


void physicalLayer::GetFrame()
{
    string RecData;
    timeval tv;
    char RecordedSequence[4];
    int SequenceCounter=0;
    while(!ArrayComp(RecordedSequence,EndSequence,4,SequenceCounter)) //Loop until we get endsequence in buffer
    {
        gettimeofday(&tv,NULL);
        while(synctime+(TONELENGHT+SILENTLENGHT)*(samplesSinceSync)>tv.tv_sec*1000+tv.tv_usec/1000)
        {
            usleep(1000);
            gettimeofday(&tv,NULL);
        }
        samplesSinceSync++;
          
        vector<float> RecordedData=Rec.GetAudioData(TONELENGHT,0);
        applyHamming(RecordedData);
        float max=0;
        int freq1Index;
        int freq2Index;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray1[k],SAMPLE_RATE,RecordedData);
            if (gMag>max) {max=gMag; freq1Index=k;}
        }
        if (max<SILENTLIMIT) return;
        max=0;
        for(int k=0;k<4;k++)
        {
            float gMag=goertzel_mag(Freqarray2[k],SAMPLE_RATE,RecordedData);
            if (gMag>max) {max=gMag; freq2Index=k;}
        }
        if (max<SILENTLIMIT) return;
        char Tone=DTMFTones[freq1Index*4+freq2Index];
        //for(int a=0;a<=freq1Index*4+freq2Index;a++) cout << Tone;
        //cout << endl;
        
        RecData+=Tone;
        RecordedSequence[SequenceCounter]=Tone;
        SequenceCounter=(SequenceCounter+1)%4;  
    }
    

    inBuffer.Buffer[inBuffer.NextFrameToRecord++]=RecData.substr(0,RecData.size()-4);
    inBuffer.NextFrameToRecord%=inBuffer.Buffer.size();
}

void physicalLayer::PlaySync() //Play the sync sequence
{
    for(int i=0;i<3;i++) //Times the sequenc is played. May need to be adjusted later.
    {
        for(int j=0;j<4;j++)
        {
            Player.PlayDTMF(SyncSequence[j],TONELENGHT);
            Player.PlayDTMF(' ',SILENTLENGHT);
        }
    }
    for(int i=0;i<4;i++)
    {
        Player.PlayDTMF(SyncEnd[i],TONELENGHT);
        Player.PlayDTMF(' ',SILENTLENGHT);
    }
}

void physicalLayer::PlayEndSequence()
{
    for(int i=0;i<4;i++)
    {
        Player.PlayDTMF(EndSequence[i],TONELENGHT);
        Player.PlayDTMF(' ',SILENTLENGHT);
    }
}

void physicalLayer::PlayFrame(string Tones)
{
    PlaySync();
    for(char Tone : Tones)
    {
        Player.PlayDTMF(Tone,TONELENGHT);
        Player.PlayDTMF(' ',SILENTLENGHT);
    }
    PlayEndSequence();
}


void physicalLayer::frameGrabber()
{
    while(true)
    {

        GetSync();
        cout << "Sync" << endl;
        samplesSinceSync=0;
        GetFrame();
    }
    
}

void physicalLayer::frameSender()
{
    while(true)
    {
        usleep(1000);
        if(outBuffer.LastFrameElement==outBuffer.NextFrameToRecord) continue;
        PlaySync();
        PlayFrame(outBuffer.Buffer[outBuffer.LastFrameElement++]);
        outBuffer.LastFrameElement%=outBuffer.Buffer.size();
        PlayEndSequence();
    }
}

void physicalLayer::QueueFrame(string frame)
{
    outBuffer.Buffer[outBuffer.NextFrameToRecord++]=frame;
    outBuffer.NextFrameToRecord%=outBuffer.Buffer.size();
}    

string physicalLayer::GetNextFrame()
{
    string returnframe=inBuffer.Buffer[inBuffer.LastFrameElement++];
    inBuffer.LastFrameElement%=inBuffer.Buffer.size();
    return returnframe;

}

bool physicalLayer::isFrameAvaliable()
{
    if (outBuffer.LastFrameElement==outBuffer.NextFrameToRecord) return false;
    return true;
}

void frameGrabWrapper(physicalLayer * PhysLayer)
{
    PhysLayer->frameGrabber();
}

void frameSendWrapper(physicalLayer * PhysLayer)
{
    PhysLayer->frameSender();
}

template <typename Type>
bool ArrayComp(Type *Array1, Type *Array2,int size,int index)
{   //Compare two arrays. return true on match, else false.
    //If index is given, Array1 will be right-rotated by that amount.
    for(int i=0;i<size;i++)
    {
        if( Array1[(i+index)%size]!=Array2[i]) return false;
    }
    return true;
}
