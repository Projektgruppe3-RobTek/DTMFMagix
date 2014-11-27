#include "DataLinkLayer.h"
#include <iostream>
DataLinkLayer::DataLinkLayer()
{
	pthread_create(getFramesThread,(pthread_attr_t *)NULL,getFramesWrapper, static_cast<void*>(this));
	pthread_create(getDatagramsThread,(pthread_attr_t *)NULL,getDatagramsWraper, static_cast<void*>(this));
}
DataLinkLayer::~DataLinkLayer()
{
    stop=true;
	pthread_join(getFramesThread,NULL);
	pthread_join(getDatagramsThread,NULL);
}

void DataLinkLayer::getFrames()
{
    while(true)
    {
        if(stop) return;   
        while (!physLayer.dataAvailable()) //is there a new frame?
        {
            if(stop) return;
            usleep(1000);
            if(mode==Mode::client and getTimer() > 5 * ((MAX_FRAMESIZE / 4) + 25) * SENDTIME){
                mode = Mode::idle; //release connection if server is dead
                cout << "timeout " << endl;
            }
        }
        vector<bool> recievedFrame = physLayer.popData(); //Get the frame
        //for (int a = 0; a < recievedFrame.size(); a++) cout << recievedFrame[a];
        //cout << endl;
        removePadding(recievedFrame); 
        revBitStuff(recievedFrame);
        if(!CRCdecoder(recievedFrame))
        {
            cout << "Fejl i CRC!!!" << endl;
            continue;
        }
        int frameID=getID(recievedFrame);
        int frameType=getType(recievedFrame);
        //cout << "ID: " << frameID << "---" << "Type: " << frameType << endl;
        switch(mode)
        {
            case Mode::idle: //Things to do if we are idle
                //If the node is neither client or server, it will recieve requests to establish a connection,
                //and accepts (to establih)
                //We however also need to respond to terminate request, as the accept of a terminate can have been lost.
                
                if (frameType == 2) //request
                {
                    lastinID = frameID;
                    sendAccept(!lastoutID);
                    lastoutID =! lastoutID;
                    mode = Mode::client;
                    startTimer();
                }
                else if(frameType == 3) //accept
                {
                    if(conWait.waiting and conWait.type == 1)
                    {
                        lastinID = frameID;
                        mode = Mode::server;
                        conWait.waiting = 0;
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
                    cout << "ERROR in idle recieve (got a frame with wrong ID)" << endl;
                    cout << "frameID=" << frameID << ", frametype=" << frameType << endl;
                    cout << "This is the frame: " << endl;
                    for(auto bit : recievedFrame) cout << bit; cout << endl; 
                }
            break;
                
            case Mode::client: //Things to do if client
            //when client, we can only respond to dataframes (with an ACK) and terminate frames (with an accept)
            //However, we also need to respond to request frames, as the accept can have been lost
                startTimer();
                if (frameType == 0) //data
                {
                    if (frameID == lastinID)
                    {
                        sendACK(frameID);
                        //cout << "Discarded frame" << endl;
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
                    mode = Mode::idle;
                    lastinID = frameID;
                    sendAccept(!lastoutID);
                    lastoutID =! lastoutID;
                }
                else 
                {
                    cout << "ERROR in client recieve (got a frame with wrong ID)" << endl;
                    cout << "frameID=" << frameID << ", frametype=" << frameType << endl;
                    cout << "This is the frame: " << endl;
                    for(auto bit : recievedFrame) cout << bit; cout << endl; 
                }
            break;
            
            case Mode::server: //Things to do if server
            //when server, we can only recieve ACK's and accepts.
                if (frameType == 1) //ACK
                {
                    if (ackWait.waiting and ackWait.ID == frameID)
                    {                                //If we are waiting for this ACK, mark it as recieved.
                        ackWait.waiting = false;
                    }
                }
                else if (frameType == 3) //accept
                {
                    lastinID = frameID;
                    if(conWait.waiting and conWait.type == 1) //If we are waiting for request accept (we should not be doing that at this point), mark it as recieved
                    {
                        cout << "ERROR in server request wait" << endl;
                    }
                    else if (conWait.waiting and conWait.type == 0) //If we are waiting for terminate accept,
                    {                                                //mark as recieved and change mode to not connected.
                        mode = Mode::idle;
                        conWait.waiting = false;
                    }
                    else cout << "ERROR in accept recieve master" << endl;
                }
                else 
                {
                    cout << "ERROR in server recieve (got a frame with wrong ID)" << endl;
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


void DataLinkLayer::getDatagrams(){
    vector<bool> data_to_send;
    bool connectionLost = false;

    while(!stop){
        while((mode == Mode::client or outBuffer.empty()) and !stop){ 
            usleep(10000);
        }
        
        while(outBuffer.size() and !stop){
            if(mode != Mode::server and !connectionRequest()) break;
            if(!connectionLost)
            {
                data_to_send = outBuffer.pop_front();

                setType(data_to_send, 0);
                setID(data_to_send);
                CRCencoder(data_to_send);
                bitStuff(data_to_send);
                setPadding(data_to_send);
            }
            
            else connectionLost = false;
            if(!sendPacket(data_to_send)) {
                connectionLost = true;
                break;
            }
        }

        if(outBuffer.empty() && mode==Mode::server){
            releaseConnection();
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

void DataLinkLayer::bitStuff(vector<bool> &frame) //Bitstuff.
{                                                 //We bitstuff with the inverse of the last bit in our flag.
    vector<int> elementsToStuff;
    for(unsigned int i = flag.size() - 1; i < frame.size(); i++)
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
    for(unsigned int i = flag.size() - 1; i < frame.size(); i++)
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
    while(physLayer.layerBusy() and !stop) usleep(1000);
    physLayer.pushData(frame);
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
    int DividendOffset=0;
    for (unsigned int i=0; i < dataWord.size(); i++)
    {
        if(Dividend[0+DividendOffset])                                         // If the MSB is 1, XOR with Divisor
        {
            for(unsigned int j=0; j<Divisor.size(); j++)
            {
                Dividend[j+DividendOffset] = Dividend[j+DividendOffset] ^ Divisor[j];
            }
        }
        //Dividend.erase(Dividend.begin()) ;                      // When XOR is done, the vector is moved one
        DividendOffset++;                                                        // place to the left.
    }

    for(unsigned int i=0; i<Divisor.size()-1; i++)              // When the division is done, the remainder
    {                                                           // is put behind the Dataword to make the Codeword
        dataWord.push_back(Dividend[i+DividendOffset]);
    }
}

bool DataLinkLayer::dataAvailable()
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

int DataLinkLayer::dataBufferSize()
{
    return outBuffer.size();
}
void DataLinkLayer::pushData(vector<bool> data)
{
    while(mode == Mode::client and !stop) usleep(1000); //Block if we are client
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

bool DataLinkLayer::connectionRequest(){
    int requestsSend = 0;
    conWait.waiting=true;
    conWait.type=1;
 
    while(conWait.waiting and !stop){
        if (requestsSend % 3 == 2){
            usleep((((25 + MAX_FRAMESIZE / 4) * SENDTIME) + MAX_FRAMESIZE / 4 + rand() % (2000 + requestsSend * MAX_FRAMESIZE * 2)) * 1000); //ack 25 tones, data max length MAX_FRAMESIZE/4 tones, MAX_FRAMESIZE/4 is added as a guard and a random time
            cout << "Stepping down, failed to send request (DLL)" << endl;
            if(mode == Mode::client) return false;
        }

        startTimer();
        sendRequest(!lastoutID);
        lastoutID = !lastoutID;
        while(!stop and conWait.waiting and getTimer() < ((25 + MAX_FRAMESIZE / 4) * SENDTIME) + MAX_FRAMESIZE / 4){ //ack 25 tones, data max length MAX_FRAMESIZE/4 tones, MAX_FRAMESIZE/4 is added as a guard 
            usleep(2000);
        }

        requestsSend++;
    }
    return true;
}

bool DataLinkLayer::releaseConnection(){
    int terminateSend = 0;
    conWait.waiting=true;
    conWait.type=0;
    while(!stop and conWait.waiting){
        if (terminateSend > 5){
            mode = Mode::idle;
            return false;
        }

        startTimer();
        sendTerminate(!lastoutID);
        lastoutID = !lastoutID;
        while(!stop and conWait.waiting and getTimer() < ((25 + MAX_FRAMESIZE / 4) * SENDTIME) + MAX_FRAMESIZE / 4){ //ack 25 tones, data max length MAX_FRAMESIZE/4 tones, MAX_FRAMESIZE/4 is added as a guard 
            usleep(2000);
        }

        terminateSend++;
    }
    return (mode==Mode::idle);
}

bool DataLinkLayer::sendPacket(vector<bool> &packet){
    int packetsSend = 0;
    ackWait.ID=lastoutID;
    ackWait.waiting=true;
    
    while(!stop and ackWait.waiting){
        if (packetsSend > 5){
            sendTerminate(!lastoutID);
            lastoutID = !lastoutID;
            mode = Mode::idle;
            return false;
        }


        startTimer();
        physLayer.pushData(packet);

        while(!stop and ackWait.waiting and getTimer() < ((25 + MAX_FRAMESIZE / 4) * SENDTIME) + MAX_FRAMESIZE/4){ //ack 25 tones, data max length MAX_FRAMESIZE/4 tones, MAX_FRAMESIZE/4 is added as a guard 
            usleep(2000);
        }

    packetsSend++;
    }
    return (mode == Mode::server);
}

int DataLinkLayer::getMode()
{
    switch(mode)
    {
        case Mode::idle:
            return 0;
        case Mode::server:
            return 1;
        case Mode::client:
            return 2;
        default:
            return -1;
    }
}
