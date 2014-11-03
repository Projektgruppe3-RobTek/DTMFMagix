#include    "NewPhysicalLayer.h"

NewPhysicalLayer::NewPhysicalLayer()
{
    incommingThread=thread(getFrameWrapper,this);
    outgoingThread=thread(playFrameWrapper,this);
}

void NewPhysicalLayer::applyHamming(vector<float> &data)
{
    for(unsigned int i=0;i<data.size();i++)
    {
        data[i]*=0.54-0.46*cos((2*M_PI*i)/(data.size()-1));
    }
}

vector<char> NewPhysicalLayer::applyPreambleTrailer(vector<char> data)
{
    vector<char> syncSequenceVec(SyncSequence, SyncSequence + 4);
    vector<char> startFlagVec(startFlag, startFlag + 4);
    vector<char> endFlagVec(endFlag, endFlag + 4);
    vector<char> frame;
    for(int i = 0; i<3; i++)
    {
        frame.insert(frame.end(), syncSequenceVec.begin(), syncSequenceVec.end());
    }
    frame.insert(frame.end(), startFlagVec.begin(), startFlagVec.end());
    frame.insert(frame.end(), data.begin(), data.end());
    frame.insert(frame.end(), endFlagVec.begin(), endFlagVec.end());
    return frame;
}


vector<char> NewPhysicalLayer::convertToDTMF(vector<bool> bFrame)
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
    return dFrame;
}

vector<bool> NewPhysicalLayer::convertToBinary(vector<char> dFrame)
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
        for (int k = 0; k < 4; k++)
        {
            bFrame[j*4+k] = ref[i*4+k];
        }
    }
    return bFrame;
}

void NewPhysicalLayer::playFrame()
{
    while(true)
    {
        usleep(2000);
        if(sendFlag)
        {
            Player.PlayDTMF(outgoingFrame,TONELENGTH,SILENTLENGTH);
            sendFlag=false;
        }

    }
            
}

void NewPhysicalLayer::getFrame()
{
    while(true)
    {
        char RecordedSequence[4];
        int SequenceCounter=0;
        char previousNote='!';
        if(bugfix) cout << "entering sync" << endl;
        while(!ArrayComp(RecordedSequence,SyncSequence,4,SequenceCounter))
        {
            usleep(SILENTLENGTH*1000+TONELENGTH*1000);
            vector<float> in1=Rec.GetAudioData(TONELENGTH,0);
            applyHamming(in1);
            float freq1max=0;
            int freq1Index=0;
            float freq2max=0;
            int freq2Index=0;
            for(int k=0;k<4;k++)
            {
                float gMag=goertzel_mag(Freqarray1[k],SAMPLE_RATE,in1);
                if (gMag>freq1max) {freq1max=gMag; freq1Index=k;}
            }

            for(int k=0;k<4;k++)
            {
                float gMag=goertzel_mag(Freqarray2[k],SAMPLE_RATE,in1);
                if (gMag>freq2max) {freq2max=gMag; freq2Index=k;}
            }
            char Note=DTMFTones[freq1Index*4+freq2Index];
            if(bugfix) cout << Note << endl;
            if(Note!=previousNote)
            {
                RecordedSequence[SequenceCounter]=Note;
                SequenceCounter=(SequenceCounter+1)%4;
                previousNote=Note;
            }
            else
            {
                usleep(2000);
            }

        }
        gettimeofday(&tv,NULL);
        synctime=tv.tv_sec*1000+tv.tv_usec/1000;
        SequenceCounter=0;
        if(bugfix) cout << "entering start" << endl;
        while(!ArrayComp(RecordedSequence,startFlag,4,SequenceCounter))
        {
            do
            {
                usleep(1000);
                gettimeofday(&tv,NULL);
            } while((tv.tv_sec*1000+tv.tv_usec/1000-synctime)%(TONELENGTH+SILENTLENGTH)>4);
            usleep(TONELENGTH*1000);
            vector<float> in2=Rec.GetAudioData(TONELENGTH,0);
            applyHamming(in2);
            float freq1max=0;
            int freq1Index=0;
            float freq2max=0;
            int freq2Index=0;
            for(int k=0;k<4;k++)
            {
                float gMag=goertzel_mag(Freqarray1[k],SAMPLE_RATE,in2);
                if (gMag>freq1max) {freq1max=gMag; freq1Index=k;}
            }

            for(int k=0;k<4;k++)
            {
                float gMag=goertzel_mag(Freqarray2[k],SAMPLE_RATE,in2);
                if (gMag>freq2max) {freq2max=gMag; freq2Index=k;}
            }
            char Note=DTMFTones[freq1Index*4+freq2Index];
            if(bugfix) cout << Note << endl;
            RecordedSequence[SequenceCounter]=Note;
            SequenceCounter=(SequenceCounter+1)%4;
        }
        vector<char> data;
        SequenceCounter=0;
        if(bugfix) cout << "entering end" << endl;
        while(!ArrayComp(RecordedSequence,endFlag,4,SequenceCounter))
        {
            do
            {
                usleep(1000);
                gettimeofday(&tv,NULL);
            } while((tv.tv_sec*1000+tv.tv_usec/1000-synctime)%(TONELENGTH+SILENTLENGTH)>4);
            usleep(TONELENGTH*1000);
            vector<float> in=Rec.GetAudioData(TONELENGTH,0);
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
            if(bugfix) cout << Note << endl;
            RecordedSequence[SequenceCounter]=Note;
            SequenceCounter=(SequenceCounter+1)%4;
            if(!ArrayComp(RecordedSequence,endFlag,4,SequenceCounter))
            {
                data.push_back(Note);
            }
        }
        if(bugfix) cout << "frame successfull" << endl;
        data.erase(data.end()-2,data.end());
        incommingFrame=data;
        receiveFlag=true;
        usleep(2000);
    }
 }

bool NewPhysicalLayer::returnSendFlag()
{
    return sendFlag;
}

bool NewPhysicalLayer::returnReceiveFlag()
{
    return receiveFlag;
}

void NewPhysicalLayer::pushFrame(vector<bool> frame)
{
    outgoingFrame=applyPreambleTrailer(convertToDTMF(frame));
    sendFlag=true;
}

vector<bool> NewPhysicalLayer::popFrame()
{
    return convertToBinary(incommingFrame);
    receiveFlag=false;
}

void playFrameWrapper(NewPhysicalLayer * PhysLayer)
{
    PhysLayer->playFrame();
}

void getFrameWrapper(NewPhysicalLayer * PhysLayer)
{
    PhysLayer->getFrame();
}


NewPhysicalLayer::~NewPhysicalLayer()
{

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
