#include    "PhysicalLayer.h"

PhysicalLayer::PhysicalLayer() // Default constructer
{
	Rec=DTMFRecorder::getInstance();
	Player=DTMFPlayer::getInstance();
    incommingThread=thread(getFrameWrapper,this);
    outgoingThread=thread(playFrameWrapper,this);
}

void PhysicalLayer::applyHamming(vector<float> &data) // Applies Hamming to given vector
{

    for(unsigned int i=0;i<data.size();i++)
    {
        data[i]*=0.54-0.46*cos((2*M_PI*i)/(data.size()-1)); //Hamming
        //data[i]*=0.5*(1-cos((2*M_PI*i)/(data.size()-1))); //hanning
        /*
        double a=0.16;
        double a_0=(1-a)/2.;
        double a_1=1./2.;
        double a_2=a/2.;
        data[i]*=a_0-a_1*cos(2*M_PI*i/(data.size()-1)+a_2*cos(4*M_PI*i/(data.size()-1)*/
    }
}

vector<char> PhysicalLayer::applyPreambleTrailer(vector<char> data)
{
    vector<char> syncSequenceVec(SyncSequence, SyncSequence + 4);   //  
    vector<char> startFlagVec(startFlag, startFlag + 4);            //
    vector<char> endFlagVec(endFlag, endFlag + 4);                  // Save sync and flag arrays, as vectors
    vector<char> frame;                                             // Create empty vector
    for(int i = 0; i<SYNCREPEAT; i++)
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
    while(!stop)
    {
        usleep(2000);
        if(sendFlag)
        {
            Player->PlayDTMF(outgoingFrame,TONELENGTH,SILENTLENGTH);
            sendFlag=false;
        }

    }
            
}

void PhysicalLayer::getFrame()
{
    while(!stop)
    {
        char RecordedSequence[SYNCLENGHT];                                           // Initiate RecordedSequence array
        int SequenceCounter=0;
        char previousNote='!';
        if(bugfix) cout << "entering sync" << endl;
        while(!ArrayComp(RecordedSequence,SyncSequence,SYNCLENGHT,SequenceCounter))  // Compare last 4 recorded notes with sync sequence
        {
            usleep(250);
            if(stop) return;
            while(sendFlag) usleep(2000);                                   // If sendFlag = true sleep until sendFlag = false
            while(receiveFlag) usleep(2000);                                // If receiveFlag = true sleep until receiveFlag = false
            vector<float> in1=Rec->GetAudioData(TONELENGTH,0);               // Save samples made last TONELENGTH ms in vector in1
            applyHamming(in1);                                              // Apply Hamming to in1
            float freq1max=0;
            int freq1Index=0;
            float freq2max=0;
            int freq2Index=0;
            for(int k=0;k<4;k++)                                            // Calculate Goertzel on all vertical frequencies
            {                                                               // Save index for frequency with largest Goertzel value
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray1[k],in1);
                if (gMag>freq1max) 
                {
                    freq1max=gMag; 
                    freq1Index=k;
                }
            }

            for(int k=0;k<4;k++)                                            // Calculate Goertzel on all horizontal frequencies
            {                                                               // Save index for frequency with largest Goertzel value
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray2[k],in1);
                if (gMag>freq2max) 
                {
                    freq2max=gMag;
                    freq2Index=k;
                }
            }

            char Note=DTMFTones[freq1Index*4+freq2Index];                   // Determine DTMF by frequency index
            
            RecordedSequence[SequenceCounter]=Note;                         // Save recorded DTMF in RecordedSequence array
            if(Note!=previousNote)
            {
                SequenceCounter=(SequenceCounter+1)%SYNCLENGHT;
                gettimeofday(&tv,NULL);                                             
                synctime=tv.tv_sec*1000+tv.tv_usec/1000-(TONELENGTH+SILENTLENGTH)/2;
                if(bugfix) cout << Note << endl;
            }
                
            previousNote=Note;
        }                                                               
        SequenceCounter=0;
        if(bugfix) cout << "entering start" << endl;
        while(!ArrayComp(RecordedSequence,startFlag,4,SequenceCounter))                     // Compare last 4 recorded notes with start flag sequence
        {
            if(stop) return;
            do
            {                                                                               
                usleep(1000);
                gettimeofday(&tv,NULL);
            } while((tv.tv_sec*1000+tv.tv_usec/1000-synctime)%(TONELENGTH+SILENTLENGTH)>4); // Sleep until SILENTLENGTH + TONELENGTH ms since start of last DTMF
            usleep(TONELENGTH*1000);
            vector<float> in2=Rec->GetAudioData(TONELENGTH,0);                               // Save samples made last TONELENGTH ms in vector in2
            applyHamming(in2);
            float freq1max=0;
            int freq1Index=0;
            float freq2max=0;
            int freq2Index=0;
            for(int k=0;k<4;k++)
            {
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray1[k],in2);                   // Calculate Goertzel on all vertical frequencies
                if (gMag>freq1max)
                {
                    freq1max=gMag;
                    freq1Index=k;                                                           // Save index for frequency with largest Goertzel value
                }                           
            }

            for(int k=0;k<4;k++)
            {
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray2[k],in2);                   // Calculate Goertzel on all horizontal frequencies
                if (gMag>freq2max)
                {
                    freq2max=gMag;
                    freq2Index=k;                                                           // Save index for frequency with largest Goertzel value
                }                           
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
            if(stop) return;
            do
            {
                usleep(1000);
                gettimeofday(&tv,NULL);
            } while((tv.tv_sec*1000+tv.tv_usec/1000-synctime)%(TONELENGTH+SILENTLENGTH)>4); // Sleep until SILENTLENGTH + TONELENGTH ms since start of last DTMF
            usleep(TONELENGTH*1000);
            vector<float> in=Rec->GetAudioData(TONELENGTH,0);                                // Save samples made last TONELENGTH ms in vector in3
            applyHamming(in);
            float freq1max=0;
            int freq1Index=0;
            float freq2max=0;
            int freq2Index=0;
            for(int k=0;k<4;k++)
            {
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray1[k],in);                    // Calculate Goertzel on all vertical frequencies
                if (gMag>freq1max)
                {
                    freq1max=gMag;
                    freq1Index=k;                                                           // Save index for frequency with largest Goertzel value
                }                           
            }

            for(int k=0;k<4;k++)
            {
                float gMag=doGoertzel(REC_SAMPLE_RATE,Freqarray2[k],in);                    // Calculate Goertzel on all horizontal frequencies
                if (gMag>freq2max)
                {
                    freq2max=gMag;
                    freq2Index=k;                                                           // Save index for frequency with largest Goertzel value
                }                           
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

bool PhysicalLayer::layerBusy()
{
    return sendFlag;
}

bool PhysicalLayer::dataAvailable()
{
    return receiveFlag;
}

void PhysicalLayer::pushData(vector<bool> frame)                
{
    //for(auto bit : frame) std::cout << bit; std::cout << endl;
    outgoingFrame=applyPreambleTrailer(convertToDTMF(frame));       // Convert binary frame to DTMF, and save as outgoingFrame
    sendFlag=true;
}

vector<bool> PhysicalLayer::popData()
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
PhysicalLayer* PhysicalLayer::instance = nullptr;
PhysicalLayer *PhysicalLayer::getInstance()
{
	if(instance==nullptr) instance = new PhysicalLayer;
	return instance;
}

PhysicalLayer::~PhysicalLayer()
{
    stop=true;
    incommingThread.join();
    outgoingThread.join();
    delete instance;
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

