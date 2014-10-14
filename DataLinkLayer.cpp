#include "DataLinkLayer.h"
#include <iostream>
DataLinkLayer::DataLinkLayer()
{
    getFramesThread=thread(getFramesWrapper,this);
    getDatagramsThread=thread(getDatagramsWraper, this);
}


void DataLinkLayer::getFrames()
{
    while(true)
    {
        usleep(1000);
        
        if (!physLayer.isFrameAvaliable()) continue; //is there a new frame?
        vector<bool> recievedFrame=physLayer.getFrame(); //Get the frame
        
        removePadding(recievedFrame); 
        revBitStuff(recievedFrame);
        if(!CRCdecoder(recievedFrame)) continue;
        int frameID=getID(recievedFrame);
        int frameType=getType(recievedFrame);
        
        switch(MasterSlaveState)
        {
            case masterSlaveEnum::undecided: //Things to do if neither slave or master
                //If the node is neither slave or master, it will recieve requests to establish a connection,
                //and accepts (to establih)
                //We however also need to respond to terminate request, as the accept of a terminate can have been lost.
                if (frameType==2) //request
                {
                    sendAccept(!lastoutID);
                    lastoutID=!lastoutID;
                }
                
            case masterSlaveEnum::slave: //Things to do if slave
            //when slave, we can only respond to dataframes (with an ACK) and terminate frames (with an accept)
            //However, we also need to respond to request frames, as the accept can have been lost
            
            case masterSlaveEnum::master: //Things to do if master
            //when master, we can only recieve ACK's and accepts.
            
            
            default:
                cout <<"ERROR" << endl;
        
        }
        if (frameType!=1)
        { 
            if (frameID==lastinID) //We have already recieved this frame.
            {                      //Just send an ACK
                sendACK(frameID);
                continue;
            }
            else
            {
                switch(frameType)
                {
                    case 0:  //data
                        sendACK(frameID);
                        inBuffer.push_back(recievedFrame);
                        lastinID=frameID;
                        break;
                    case 2: //request
                        break;
                    case 3: //accept
                        break;
                    case 4: //decline
                        break;
                    case 5: //terminate
                        break;
                    default:
                         cout << "ERROR in getFrames. This is probably a logic bug!" << endl;
                }
            }
        }
        else
        {
            //cout << "ack" << endl;
            //cout << curWaitingACK.waiting << curWaitingACK.ID << frameID << endl;
            if (curWaitingACK.waiting and curWaitingACK.ID==frameID)
            {
                //cout << "acpack" << endl;
                curWaitingACK.waiting=false;
            }
        }
    } 
}

void DataLinkLayer::getDatagrams() 
{
    while(true)
    {
        usleep(1000);
        if(!outBuffer.empty())
        {
            vector<bool> frameToSend=outBuffer.pop_front();
            setType(frameToSend,0);
            bool frameID= !lastoutID;
            lastoutID=!lastoutID;
            setID(frameToSend,frameID);
            auto newframe=frameToSend;
            //cout << getID(newframe) << "!!" <<getType(newframe) << endl;
            CRCencoder(frameToSend);
            bitStuff(frameToSend);
            setPadding(frameToSend);
            curWaitingACK.ID=frameID;
            //cout << frameID << endl;
            curWaitingACK.waiting=true;
            while(curWaitingACK.waiting)
            {
                startTimer();
                sendFrame(frameToSend);
                while(getTimer()<3000 and curWaitingACK.waiting)
                {
                    usleep(1000);
                }
            }
        
        }
    }
}

void getFramesWrapper(DataLinkLayer *DaLLObj)
{
    DaLLObj->getFrames();
}

void getDatagramsWraper(DataLinkLayer *DaLLObj)
{
    DaLLObj->getDatagrams();
}

void DataLinkLayer::bitStuff(vector<bool> &frame)
{
    vector<int> elementsToStuff;
    for(int i = flag.size() - 1; i < frame.size(); i++)
    {
        if(flagcheck(frame, i - ((int)flag.size() - 1), flag,flag.size() - 1)) elementsToStuff.push_back(i);
    }
    int offset = 0;
    for(int index : elementsToStuff)
    {
        frame.insert(frame.begin() + offset + index, !flag.back());
        offset++;
    }
}


void DataLinkLayer::revBitStuff(vector<bool> &frame)
{
    vector<int> elementsToRemove;
    for(int i = flag.size() - 1; i < frame.size(); i++)
    {
        if (flagcheck(frame, i - (int)flag.size() + 1, flag, flag.size() - 1)) elementsToRemove.push_back(i);
    }
    int offset = 0;
    for(int index : elementsToRemove)
    {
        frame.erase(frame.begin() + offset + index);
        offset--;
    }
}


void DataLinkLayer::setPadding(vector<bool> &frame)
{
    int lengthOfPadding = 0;

    while((frame.size()+2) % 4)
    {
        lengthOfPadding++;
        frame.push_back(!flag.back());
    }
    frame.insert(frame.begin(), lengthOfPadding % 2);
    frame.insert(frame.begin(), lengthOfPadding % 4 - lengthOfPadding % 2);
}

void DataLinkLayer::removePadding(vector<bool> &frame)
{
    int paddinglength = (frame[0]<<1) + frame[1];
    frame.erase(frame.begin(), frame.begin() + 2);
    for(int i = 0; i < paddinglength; i++) frame.pop_back();
}

bool DataLinkLayer::getID(vector<bool> &frame)
{
    int ID = frame[0];
    frame.erase(frame.begin());
    return ID;
}

void DataLinkLayer::setID(vector<bool> &frame)
{
    lastoutID = !lastoutID;
    frame.insert(frame.begin(), lastoutID);
}
void DataLinkLayer::setID(vector<bool> &frame, int ID) //This is mostly for ACK's
{                                       //Don't change lastoutID.
    frame.insert(frame.begin(), ID);
}
int DataLinkLayer::getType(vector<bool> &frame)
{
    int Type=(frame[0] << 2) + (frame[1] << 1) + frame[2];
    frame.erase(frame.begin(),frame.begin()+3);
    return Type;
}

void DataLinkLayer::setType(vector<bool> &frame, int Type)
{
    Type%=8;
    bool booltype[3]={0,0,0};
    for(int i=2;i>=0;i--)
    {
        if( Type - (1 << i) >=0)
        {
            Type-=(1 << i);
            booltype[2-i]=true;
        }
    }
    
    frame.insert(frame.begin(),booltype[2]);
    frame.insert(frame.begin(),booltype[1]);
    frame.insert(frame.begin(),booltype[0]);
}

void DataLinkLayer::sendControl(int Type,bool ID)
{
    vector<bool> Control;
    setType(Control, Type);
    setID(Control, ID);
    CRCencoder(Control);
    bitStuff(Control);
    setPadding(Control);
    sendFrame(Control);
}

void DataLinkLayer::sendACK(bool ID)
{
    // cout << "sendack" << endl;
    sendControl(1,ID);
}

void DataLinkLayer::sendRequest(bool ID)
{
    sendControl(2,ID);
}

void DataLinkLayer::sendAccept(bool ID)
{
    sendControl(3,ID);
}

void DataLinkLayer::sendDecline(bool ID)
{
    sendControl(4,ID);
}

void DataLinkLayer::sendTerminate(bool ID)
{
    sendControl(5,ID);
}
void DataLinkLayer::sendFrame(vector<bool> &frame)
{
    while(physLayer.isQueueFull()) usleep(1000);
    physLayer.QueueFrame(frame);
    /*auto framen=frame;
    removePadding(framen);
    revBitStuff(framen);
    CRCdecoder(framen);
    cout << getID(framen) << " " << getType(framen) << endl;*/
}

void DataLinkLayer::startTimer()
{
    gettimeofday(&timer,NULL);
}

int DataLinkLayer::getTimer()
{
    timeval tv;
    gettimeofday(&tv,NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000 )  - (timer.tv_sec * 1000 + timer.tv_usec / 1000);
}
bool DataLinkLayer::CRCdecoder(vector<bool> &codeWord)
{
    //vector<bool> Divisor    = {1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1,1};                      // CRC-32 generator

    vector<bool> Divisor    = {1,0,0,1,1};                      // CRC generator
    vector<bool> Dividend   = codeWord;

    for (unsigned int i=0; i < codeWord.size(); i++)
    {
        if(Dividend[0])                                         // If the MSB is 1, XOR with Divisor
        {
            for(unsigned int j=0; j<Divisor.size(); j++)
            {
                Dividend[j] = Dividend[j] ^ Divisor[j];
            }
        }
        Dividend.erase(Dividend.begin()) ;                      // When XOR is done, the vector is moved one
                                                                // place to the left.
    }
    for (unsigned int i = 0; i < Divisor.size(); i++)
    {
        if (Dividend[i])                                        // if the CRC check at the receiver went bad
        {                                                       // then the function will return false.
            return 0;                                           // And the frame will be discarded.
        }
    }

    for (unsigned int i = 0; i <Divisor.size()-1; i++)
    {
        codeWord.erase(codeWord.end());
    }

    return 1;                                                   // if the CRC check at the receiver went well
                                                                // then the function will return true.
}


void DataLinkLayer::CRCencoder(vector<bool> &dataWord)
{
    //vector<bool> Divisor    = {1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1,1};                      // CRC-32 generator
    vector<bool> Divisor    = {1,0,0,1,1};                      // CRC generator
    vector<bool> Dividend   = dataWord;

    for(unsigned int i=0; i<Divisor.size()-1; i++)              // Puts the appropriate amount of 0's behind the dividend.
    {
        Dividend.push_back(0);
    }

    for (unsigned int i=0; i < dataWord.size(); i++)
    {
        if(Dividend[0])                                         // If the MSB is 1, XOR with Divisor
        {
            for(unsigned int j=0; j<Divisor.size(); j++)
            {
                Dividend[j] = Dividend[j] ^ Divisor[j];
            }
        }
        Dividend.erase(Dividend.begin()) ;                      // When XOR is done, the vector is moved one
                                                                // place to the left.
    }

    for(unsigned int i=0; i<Divisor.size()-1; i++)              // When the division is done, the remainder
    {                                                           // is put behind the Dataword to make the Codeword
        dataWord.push_back(Dividend[i]);
    }
}

bool DataLinkLayer::dataAvaliable()
{
    return !inBuffer.empty();
}

vector<bool> DataLinkLayer::popData()
{
    return inBuffer.pop_front();
}

bool DataLinkLayer::dataBufferFull()
{
    return outBuffer.full();
}

void DataLinkLayer::pushData(vector<bool> data)
{
    outBuffer.push_back(data);
}

bool flagcheck(vector<bool> &vec1, int start1, array<bool, 8> &flag, int lenght) //check if the flag matches given vec
{
    for(int i = 0 ; i < lenght; i++)
    {
        if (vec1[start1 + i] != flag[i]) return false;
    }
    return true;
}
