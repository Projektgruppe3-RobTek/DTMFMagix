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
        
        while (!physLayer.isFrameAvaliable()) usleep(1000);; //is there a new frame?
        vector<bool> recievedFrame = physLayer.getFrame(); //Get the frame
        
        removePadding(recievedFrame); 
        revBitStuff(recievedFrame);
        if(!CRCdecoder(recievedFrame)) continue;
        int frameID=getID(recievedFrame);
        int frameType=getType(recievedFrame);
        
        switch(MasterSlaveState)
        {
            case masterSlaveEnum::NotConnected: //Things to do if not connected
                //If the node is neither slave or master, it will recieve requests to establish a connection,
                //and accepts (to establih)
                //We however also need to respond to terminate request, as the accept of a terminate can have been lost.
                
                if (frameType == 2) //request
                {
                    lastinID = frameID;
                    sendAccept(!lastoutID);
                    lastoutID =! lastoutID;
                    MasterSlaveState = masterSlaveEnum::slave;
                }
                else if(frameType == 3) //accept
                {
                    if(curWaitingRequest == 1)
                    {
                        lastinID = frameID;
                        MasterSlaveState = masterSlaveEnum::master;
                        curWaitingRequest = 0;
                    }
                }
                else if(frameType == 5) //terminate
                {
                    lastinID = frameID;
                    sendAccept(!lastoutID);
                    lastoutID =! lastoutID;
                }
                else 
                {
                    cout << "ERROR in NotConnected recieve (got a frame with wrong ID)" << endl;
                    cout << "frameID=" << frameID << ", frametype=" << frameType << endl;
                    cout << "This is the frame: " << endl;
                    for(auto bit : recievedFrame) cout << bit; cout << endl; 
                }
            break;
                
            case masterSlaveEnum::slave: //Things to do if slave
            //when slave, we can only respond to dataframes (with an ACK) and terminate frames (with an accept)
            //However, we also need to respond to request frames, as the accept can have been lost
                if (frameType == 0) //data
                {
                    if (frameID == lastinID)
                    {
                        sendACK(frameID);
                        continue;
                    }
                    lastinID = frameID;
                    inBuffer.push_back(recievedFrame);
                    sendACK(frameID);
                }
                else if (frameType == 2) //request
                {
                    lastinID = frameID;
                    sendAccept(!lastoutID);
                    lastoutID =! lastoutID;
                }
                else if (frameType == 5) //Terminate
                {
                    MasterSlaveState = masterSlaveEnum::NotConnected;
                    lastinID = frameID;
                    sendAccept(!lastoutID);
                    lastoutID =! lastoutID;
                }
                else 
                {
                    cout << "ERROR in slave recieve (got a frame with wrong ID)" << endl;
                    cout << "frameID=" << frameID << ", frametype=" << frameType << endl;
                    cout << "This is the frame: " << endl;
                    for(auto bit : recievedFrame) cout << bit; cout << endl; 
                }
            break;
            case masterSlaveEnum::master: //Things to do if master
            //when master, we can only recieve ACK's and accepts.
                if (frameType == 1) //ACK
                {
                    if (curWaitingACK.waiting and curWaitingACK.ID == frameID)
                    {                                //If we are waiting for this ACK, mark it as recieved.
                        curWaitingACK.waiting = false;
                    }
                }
                else if (frameType == 3) //accept
                {
                    lastinID = frameID;
                    if(curWaitingRequest == 1) //If we are waiting for request accept (we should not be doing that at this point), mark it as recieved
                    {
                        curWaitingRequest = 0;
                    }
                    else if (curWaitingRequest == 2) //If we are waiting for terminate accept,
                    {                              //mark as recieved and change mode to not connected.
                        MasterSlaveState = masterSlaveEnum::NotConnected;
                        curWaitingRequest = 0;
                    }
                    else cout << "ERROR in accept recieve master" << endl;
                }
                else 
                {
                    cout << "ERROR in master recieve (got a frame with wrong ID)" << endl;
                    cout << "frameID=" << frameID << ", frametype=" << frameType << endl;
                    cout << "This is the frame: " << endl;
                    for(auto bit : recievedFrame) cout << bit; cout << endl; 
                }
            break;
            default:
                cout << "ERROR" << endl;
        
        }
    }
}

void DataLinkLayer::getDatagrams() 
{
    while(true)
    {
        while(outBuffer.empty()) usleep(1000);
        
        while(outBuffer.size())
        {
            if (!connect()) continue;   //if connect fails, try again
            vector<bool> frameToSend = outBuffer.pop_front();
            setType(frameToSend,0);     //Set frametype for data frame
            
            bool frameID = !lastoutID;   //Set ID
            lastoutID = !lastoutID;       //and change
            setID(frameToSend, frameID); //lastoutID
            
            CRCencoder(frameToSend);
            bitStuff(frameToSend);
            setPadding(frameToSend); 
            
            curWaitingACK.ID=frameID;   //Set that we are waiting for
            curWaitingACK.waiting=true; //an ACK to this frame.
            
            while(curWaitingACK.waiting)    //Wait for ACK
            {                               //Resend if timer times out.
                startTimer();
                sendFrame(frameToSend);
                while(getTimer() < 3000 and curWaitingACK.waiting)
                {
                    usleep(1000);
                }
            }
        }
        if(!terminate()) cout << "couldn't terminate connection" << endl;
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

void DataLinkLayer::bitStuff(vector<bool> &frame) //Bitstuff.
{                                                 //We bitstuff with the inverse of the last bit in our flag.
    vector<int> elementsToStuff;
    for(int i = flag.size() - 1; i < frame.size(); i++)
    {
        if(flagcheck(frame, i - ((int)flag.size() - 1), flag, flag.size() - 1)) elementsToStuff.push_back(i);
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

    while( ( frame.size() + 2 ) % 4 )
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
    frame.erase(frame.begin(),frame.begin() + 3);
    return Type;
}

void DataLinkLayer::setType(vector<bool> &frame, int Type)
{
    Type%=8;
    bool booltype[3] = {0,0,0};
    for(int i = 2; i >= 0; i--)
    {
        if( Type - (1 << i) >=0)
        {
            Type-=(1 << i);
            booltype[2 - i]=true;
        }
    }
    
    frame.insert(frame.begin(),booltype[2]);
    frame.insert(frame.begin(),booltype[1]);
    frame.insert(frame.begin(),booltype[0]);
}

void DataLinkLayer::sendControl(int Type, bool ID)
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
    vector<bool> Divisor = {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1};                 //CRC 16
    //vector<bool> Divisor = {1,1,1,0,1,0,1,0,1};                                     //CRC 8    
    //vector<bool> Divisor    = {1,0,0,1,1};                      // CRC 4 generator
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
    vector<bool> Divisor = {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1};                 //CRC 16
    //vector<bool> Divisor = {1,1,1,0,1,0,1,0,1};                                     //CRC 8
    //vector<bool> Divisor    = {1,0,0,1,1};                                        //CRC 4 generator
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
    return inBuffer.size();
}

vector<bool> DataLinkLayer::popData()
{
    return inBuffer.pop_front();
}

bool DataLinkLayer::dataBufferFull()
{
    return (outBuffer.full());
}
bool DataLinkLayer::dataBufferEmpty()
{
    return (outBuffer.empty());
}

void DataLinkLayer::pushData(vector<bool> data)
{
    while(MasterSlaveState == masterSlaveEnum::slave) usleep(1000); //Block if we are slave
    outBuffer.push_back(data);
}
bool DataLinkLayer::connect()
{
    while(MasterSlaveState == masterSlaveEnum::slave) usleep(1000); //Block if we are slave
    bool requestID = !lastoutID;
    lastoutID = !lastoutID;
    curWaitingRequest = 1;
    while(curWaitingRequest)
    {
        startTimer();
        sendRequest(requestID);
        while(getTimer() < 3000 and curWaitingRequest)
        {
            usleep(1000);
        }
    }
    if(MasterSlaveState == masterSlaveEnum::master) return true;
    else 
    {
        cout << "ERROR in connect" << endl;
        return false;
    }
}
bool DataLinkLayer::terminate()
{   
    bool terminateID = !lastoutID;
    lastoutID = !lastoutID;
    curWaitingRequest = 2;
    while(curWaitingRequest)
    {
        startTimer();
        sendTerminate(terminateID);
        while(getTimer() < 1000 and curWaitingRequest)
        {
            usleep(1000);
        }
    }
    if(MasterSlaveState == masterSlaveEnum::NotConnected) return true;
    else 
    {
        cout << "ERROR in terminate" << endl;
        return false;
    }
}
int DataLinkLayer::getMode()
{
    switch (MasterSlaveState)
    {
    case masterSlaveEnum::NotConnected:
    return 0;
    case masterSlaveEnum::master:
    return 1;
    case masterSlaveEnum::slave:
    return 2;
    }
}

bool flagcheck(vector<bool> &vec1, int start1, array<bool, 8> &flag, int lenght) //check if the flag matches given vec
{
    for(int i = 0 ; i < lenght; i++)
    {
        if (vec1[start1 + i] != flag[i]) return false;
    }
    return true;
}
