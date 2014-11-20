#include    "PhysicalLayer.h"

PhysicalLayer::PhysicalLayer() // Default constructer
{
    incommingThread=thread(getFrameWrapper,this);
    outgoingThread=thread(playFrameWrapper,this);
}

void PhysicalLayer::applyHamming(vector<float> &data) // Applies Hamming to given vector
{
    for(unsigned int i=0;i<data.size();i++)
    {
        data[i]*=0.54-0.46*cos((2*M_PI*i)/(data.size()-1));
    }
}

vector<char> PhysicalLayer::applyPreambleTrailer(vector<char> data)
{
    vector<char> syncSequenceVec(SyncSequence, SyncSequence + 4);   //  
    vector<char> startFlagVec(startFlag, startFlag + 4);            //
    vector<char> endFlagVec(endFlag, endFlag + 4);                  // Save sync and flag arrays, as vectors
    vector<char> frame;                                             // Create empty vector
    for(int i = 0; i<3; i++)
    {
        frame.insert(frame.end(), syncSequenceVec.begin(), syncSequenceVec.end());  // Add sync to frame vector 3 times
    }
    frame.insert(frame.end(), startFlagVec.begin(), startFlagVec.end());            // Add start flag to frame vector
    frame.insert(frame.end(), data.begin(), data.end());                            // Add data to frame vector
    frame.insert(frame.end(), endFlagVec.begin(), endFlagVec.end());                // Add end flag to frame vector
    return frame;
}


vector<char> PhysicalLayer::convertToDTMF(vector<bool> bFrame)
{
    vector<char> dFrame((bFrame.size())/4);         // Create empty DTMF vector
    for (int i = 0; i < dFrame.size(); i++)         // Read 4 binary values and convert to DTMF
    {
        bool a = bFrame[i*4];
        bool b = bFrame[i*4+1];
        bool c = bFrame[i*4+2];
        bool d = bFrame[i*4+3];

        dFrame[i] = DTMFTones[a*8+b*4+c*2+d];       // Add DTMF to vector
    }
    return dFrame;
}

vector<bool> PhysicalLayer::convertToBinary(vector<char> dFrame)
{
    vector<bool> bFrame((dFrame.size())*4);         // Create empty binary vector
    bool ref[64] = {0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,1,0,1,0,0,0,1,0,1,0,1,1,0,0,1,1,1,1,0,0,0,1,0,0,1,1,0,1,0,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,1,1,1};

    for (int j = 0; j < dFrame.size();j++)          // Read next DTMF
    {
        int i = 0;
        while (DTMFTones[i] != dFrame[j])           // Compare to DTMF tones
        {
            i++;
        }
        for (int k = 0; k < 4; k++)                 // Convert DTMF to binary
        {
            bFrame[j*4+k] = ref[i*4+k];             // Add binary to vector
        }
    }
    return bFrame;
}

void PhysicalLayer::playFrame()                  // If sendFlag = true, play outgoing frame and set sendFlag = false
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

void PhysicalLayer::getFrame()
{
    while(true)
    {
        char RecordedSequence[4];                                           // Initiate RecordedSequence array
        int SequenceCounter=0;
        char previousNote='!';
        float previousFreq1=0;
        float previousFreq2=0;
        bool bestSync=false;
        if(bugfix) cout << "entering sync" << endl;
        while(!bestSync)
        {
            while(!ArrayComp(RecordedSequence,SyncSequence,4,SequenceCounter) || !bestSync)  // Compare last 4 recorded notes with sync sequence
            {
                while(sendFlag) usleep(2000);                                   // If sendFlag = true sleep until sendFlag = false
                while(receiveFlag) usleep(2000);                                // If receiveFlag = true sleep until receiveFlag = false
                vector<float> in1=Rec.GetAudioData(TONELENGTH,0);               // Save samples made last TONELENGTH ms in vector in1
                applyHamming(in1);                                              // Apply Hamming to in1
                float freq1max=0;
                int freq1Index=0;
                float freq2max=0;
                int freq2Index=0;
                timeval t;
                gettimeofday(&t,NULL);
                cout << t.tv_sec*1000+t.tv_usec/1000 << endl;
                for(int k=0;k<4;k++)                                            // Calculate Goertzel on all vertical frequencies
                {                                                               // Save index for frequency with largest Goertzel value
                    float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray1[k],in1);
                    if (gMag>freq1max) {freq1max=gMag; freq1Index=k;}
                }

                for(int k=0;k<4;k++)                                            // Calculate Goertzel on all horizontal frequencies
                {                                                               // Save index for frequency with largest Goertzel value
                    float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray2[k],in1);
                    if (gMag>freq2max) {freq2max=gMag; freq2Index=k;}
                }

                char Note=DTMFTones[freq1Index*4+freq2Index];                   // Determine DTMF by frequency index

                RecordedSequence[SequenceCounter]=Note;                         // Save recorded DTMF in RecordedSequence array
                if(Note!=previousNote) {SequenceCounter=(SequenceCounter+1)%4;}
                if(previousFreq1<freq1max && previousFreq2<freq2max)
                {
                    gettimeofday(&tv,NULL);                                             
                    synctime=tv.tv_sec*1000+tv.tv_usec/1000;
                }
                previousNote=Note;
                previousFreq1=freq1max;
                previousFreq2=freq2max;
                if(previousFreq1<freq1max && previousFreq2<freq2max) {bestSync=true;}
            }
        }                                                                   
        SequenceCounter=0;
        if(bugfix) cout << "entering start" << endl;
        while(!ArrayComp(RecordedSequence,startFlag,4,SequenceCounter))                     // Compare last 4 recorded notes with start flag sequence
        {
            do
            {                                                                               
                usleep(1000);
                gettimeofday(&tv,NULL);
            } while((tv.tv_sec*1000+tv.tv_usec/1000-synctime)%(TONELENGTH+SILENTLENGTH)>4); // Sleep until SILENTLENGTH + TONELENGTH ms since start of last DTMF
            usleep(TONELENGTH*1000);
            vector<float> in2=Rec.GetAudioData(TONELENGTH,0);                               // Save samples made last TONELENGTH ms in vector in2
            applyHamming(in2);
            float freq1max=0;
            int freq1Index=0;
            float freq2max=0;
            int freq2Index=0;
            for(int k=0;k<4;k++)
            {
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray1[k],in2);                     // Calculate Goertzel on all vertical frequencies
                if (gMag>freq1max) {freq1max=gMag; freq1Index=k;}                           // Save index for frequency with largest Goertzel value
            }

            for(int k=0;k<4;k++)
            {
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray2[k],in2);                     // Calculate Goertzel on all horizontal frequencies
                if (gMag>freq2max) {freq2max=gMag; freq2Index=k;}                           // Save index for frequency with largest Goertzel value
            }
            char Note=DTMFTones[freq1Index*4+freq2Index];                                   // Determine DTMF by frequency index
            if(bugfix) cout << Note << endl;
            RecordedSequence[SequenceCounter]=Note;                                         // Save recorded DTMF in RecordedSequence array
            SequenceCounter=(SequenceCounter+1)%4;
        }
        vector<char> data;                                                                  // Initiate datagram
        SequenceCounter=0;
        if(bugfix) cout << "entering end" << endl;
        while(!ArrayComp(RecordedSequence,endFlag,4,SequenceCounter))                       // Compare last 4 recorded notes with end flag sequence
        {
            do
            {
                usleep(1000);
                gettimeofday(&tv,NULL);
            } while((tv.tv_sec*1000+tv.tv_usec/1000-synctime)%(TONELENGTH+SILENTLENGTH)>4); // Sleep until SILENTLENGTH + TONELENGTH ms since start of last DTMF
            usleep(TONELENGTH*1000);
            vector<float> in=Rec.GetAudioData(TONELENGTH,0);                                // Save samples made last TONELENGTH ms in vector in3
            applyHamming(in);
            float freq1max=0;
            int freq1Index=0;
            float freq2max=0;
            int freq2Index=0;
            for(int k=0;k<4;k++)
            {
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray1[k],in);                      // Calculate Goertzel on all vertical frequencies
                if (gMag>freq1max) {freq1max=gMag; freq1Index=k;}                           // Save index for frequency with largest Goertzel value
            }

            for(int k=0;k<4;k++)
            {
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray2[k],in);                      // Calculate Goertzel on all horizontal frequencies
                if (gMag>freq2max) {freq2max=gMag; freq2Index=k;}                           // Save index for frequency with largest Goertzel value
            }
            char Note=DTMFTones[freq1Index*4+freq2Index];                                   // Determine DTMF by frequency index
            if(bugfix) cout << Note << endl;
            RecordedSequence[SequenceCounter]=Note;                                         // Save recorded DTMF in RecordedSequence array
            SequenceCounter=(SequenceCounter+1)%4;
            data.push_back(Note);                                                           // Save DTMF to datagram
        }
        if(bugfix) cout << "Frame successfull" << endl;
        data.erase(data.end()-4,data.end());                                                // Remove end flag from datagram
        incommingFrame=data;                                                                // Save datagram to incomming frame, to allow popFrame()
        receiveFlag=true;                                                                   
    }
 }

bool PhysicalLayer::returnSendFlag()
{
    return sendFlag;
}

bool PhysicalLayer::returnReceiveFlag()
{
    return receiveFlag;
}

void PhysicalLayer::pushFrame(vector<bool> frame)                
{
    //for(auto bit : frame) std::cout << bit; std::cout << endl;
    outgoingFrame=applyPreambleTrailer(convertToDTMF(frame));       // Convert binary frame to DTMF, and save as outgoingFrame
    sendFlag=true;
}

vector<bool> PhysicalLayer::popFrame()
{
    receiveFlag=false;
    return convertToBinary(incommingFrame);                         // Convert incommingFrame to binary and return it
}

void playFrameWrapper(PhysicalLayer * PhysLayer)
{
    PhysLayer->playFrame();
}

void getFrameWrapper(PhysicalLayer * PhysLayer)
{
    PhysLayer->getFrame();
}


PhysicalLayer::~PhysicalLayer()
{

}

template <typename Type>
bool ArrayComp(Type *Array1, Type *Array2,int size,int index)       //Compare two arrays. return true on match, else false.
{                                                                   //If index is given, Array1 will be right-rotated by that amount.
    for(int i=0;i<size;i++)
    {
        if( Array1[(i+index)%size]!=Array2[i]) return false;    
    }
    return true;
}
