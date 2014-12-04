#include "DataLinkLayer.h"
#include <iostream>
DataLinkLayer::DataLinkLayer()
{
	physLayer=PhysicalLayer::getInstance();
    getFramesThread = thread(getFramesWrapper, this);
    getDatagramsThread = thread(getDatagramsWraper, this);
}
DataLinkLayer::~DataLinkLayer()
{
    stop=true;
    getFramesThread.join();
    getDatagramsThread.join();
    delete instance;
}
DataLinkLayer *DataLinkLayer::getInstance()
{
	if(instance==nullptr) instance = new DataLinkLayer;
	return instance;
}
DataLinkLayer* DataLinkLayer::instance = nullptr;
void DataLinkLayer::getFrames()
{
    while(true)
    {
        if(stop) return;
        while (!physLayer->dataAvailable()) //is there a new frame?
        {
            if(stop) return;
            usleep(2000);
            //If the client have not heard from the server in the time it takes to send 5 packet and send 5 connection release
            //The mode is set to idle
            if(mode == Mode::client and getTimer() > 5 * (((PACKET_SIZE + ACKLENGHT) / 4) * SENDTIME)){
                mode = Mode::idle; //release connection if server is dead
                #ifdef DLLDEBUG
                cout << "timeout " << endl;
                #endif
            }
        }
        vector<bool> receivedFrame = physLayer->popData(); //Get the frame
        //for (int a = 0; a < receivedFrame.size(); a++) cout << receivedFrame[a];
        //cout << endl;
        removePadding(receivedFrame);
        revBitStuff(receivedFrame);
        if(!CRCdecoder(receivedFrame))
        {
            //#ifdef DLLDEBUG
            cout << "Fejl i CRC!!!" << endl;
            //#endif
            continue;
        }
        int frameID=getID(receivedFrame);
        int frameType=getType(receivedFrame);
        //cout << "ID: " << frameID << "---" << "Type: " << frameType << endl;
        switch(mode)
        {
            case Mode::idle: //Things to do if we are idle
                //If the node is neither client or server, it will receive requests to establish a connection,
                //and accepts (to establih)
                //We however also need to respond to terminate request, as the accept of a terminate can have been lost.

                if (frameType == 2) //request
                {
                    sendAccept();
                    mode = Mode::client;
                    startTimer();
                }
                else if(frameType == 3) //accept
                {
                    if(conWait.waiting and conWait.type == 1)
                    {
                        conWait.waiting = 0;
                        mode = Mode::server;
                    }
                }
                else if(frameType == 4) //terminate
                {
                    sendAccept();
                }
                else
                {
                    #ifdef DLLDEBUG
                    cout << "ERROR in idle receive (got a frame with wrong ID)" << endl;
                    cout << "frameID=" << frameID << ", frametype=" << frameType << endl;
                    cout << "This is the frame: " << endl;
                    for(auto bit : receivedFrame) cout << bit; cout << endl;
                    #endif
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
                        cout << "Discarded frame" << endl;
                        continue;
                    }
                    lastinID = frameID;
                    inBuffer.push_back(receivedFrame);
                    sendACK(frameID);
                }
                else if (frameType == 2) //request
                {
                    sendAccept();
                }
                else if (frameType == 4) //Terminate
                {
                    sendAccept();
                    mode = Mode::idle;
                }
                else
                {
                    #ifdef DLLDEBUG
                    cout << "ERROR in client receive (got a frame with wrong ID)" << endl;
                    cout << "frameID=" << frameID << ", frametype=" << frameType << endl;
                    cout << "This is the frame: " << endl;
                    for(auto bit : receivedFrame) cout << bit; cout << endl;
                    #endif
                }
            break;

            case Mode::server: //Things to do if server
            //when server, we can only receive ACK's and accepts.
                if (frameType == 1) //ACK
                {
                    if (ackWait.waiting and ackWait.ID == frameID) //If we are waiting for this ACK, mark it as received.
                    {
                        ackWait.waiting = false;
                    }
                }
                else if (frameType == 3) //accept
                {
                    if(conWait.waiting and conWait.type == 1) //If we are waiting for request accept (we should not be doing that at this point), mark it as received
                    {
                        #ifdef DLLDEBUG
                        cout << "ERROR in server request wait" << endl;
                        #endif
                    }
                    else if (conWait.waiting and conWait.type == 0) //If we are waiting for terminate accept,
                    {                                                //mark as received and change mode to not connected.
                        conWait.waiting = false;
                        mode = Mode::idle;
                    }
                    #ifdef DLLDEBUG
                    else cout << "ERROR in accept receive master" << endl;
                    #endif
                }
                else
                {
                	cout << "Du ligner en fisk" << endl;
                    releaseConnection(); //If any other message is received that must mean the other computer is not in client mode
                    connectionLost=true;
                    #ifdef DLLDEBUG
                    cout << "ERROR in server receive (got a frame with wrong ID)" << endl;
                    cout << "frameID=" << frameID << ", frametype=" << frameType << endl;
                    cout << "This is the frame: " << endl;
                    for(auto bit : receivedFrame) cout << bit; cout << endl;
                    #endif
                }
            break;
            #ifdef DLLDEBUG
            default:
                cout << "ERROR" << endl;
            #endif

        }
    }
}


void DataLinkLayer::getDatagrams(){
    vector<bool> data_to_send;
    connectionLost = false;

    while(!stop){
        while((mode == Mode::client or outBuffer.empty()) and !connectionLost and !stop){
            usleep(2000);
        }

        while((outBuffer.size() or connectionLost) and !stop){
            if(mode != Mode::server and !connectionRequest()) break;
            if(!connectionLost)
            {
                data_to_send = outBuffer.pop_front();

                setType(data_to_send, 0);
                lastoutID=!lastoutID;
                setID(data_to_send,lastoutID);
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
	cout << "Control " << Type << endl;
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

void DataLinkLayer::sendRequest()
{
    sendControl(2,0);
}

void DataLinkLayer::sendAccept()
{
    sendControl(3,0);
}

void DataLinkLayer::sendTerminate()
{
	cout << "term" << endl;
    sendControl(4,0);
}

void DataLinkLayer::sendFrame(vector<bool> &frame)
{
    while(physLayer->layerBusy() and !stop) usleep(1000);
    physLayer->pushData(frame);
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
    //vector<bool> Divisor = {0,0,0,0,0,1,0,0,0,0,1,0,1,1,1,1,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,0,1,1,1,1,0,1,0,1,0,0,0,1,1,0,1,1,0,1,0,0,1,0,0,1,1};    //CRC-64
    vector<bool> Divisor = {1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1,1};                      // CRC-32 generator
    //vector<bool> Divisor = {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1};                 //CRC 16
    //vector<bool> Divisor = {1,1,1,0,1,0,1,0,1};                                     //CRC 8
    //vector<bool> Divisor = {1,0,0,1,1};                      // CRC 4 generator
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
    //vector<bool> Divisor = {0,0,0,0,0,1,0,0,0,0,1,0,1,1,1,1,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,0,1,1,1,1,0,1,0,1,0,0,0,1,1,0,1,1,0,1,0,0,1,0,0,1,1};    //CRC-64
    vector<bool> Divisor = {1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1,1};                      // CRC-32 generator
    //vector<bool> Divisor = {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1};                 //CRC 16
    //vector<bool> Divisor = {1,1,1,0,1,0,1,0,1};                               //CRC 8
    //vector<bool> Divisor = {1,0,0,1,1};                                       //CRC 4 generator
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
        if (requestsSend > 3){
            usleep(((PACKET_SIZE / 4) * SENDTIME) + rand() % (2 * ((PACKET_SIZE / 4) * SENDTIME))); //Minimum sleep = the time it takes to send 1 packet and get ack
            #ifdef DLLDEBUG                                                                         //Maximum sleep = the time it takes to send 3 packet and get ack
            cout << "Stepping down, failed to send request (DLL)" << endl;
            #endif
            if(mode == Mode::client) return false;
        }

        startTimer();
        cout << "conReq" << endl;
        sendRequest();
        while(!stop and conWait.waiting and getTimer() < ((PACKET_SIZE + ACKLENGHT) / 4) * SENDTIME){ //Time out happen after the time it takes to send a packet and get ack + an extra ack as guard
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
        if (terminateSend > 3){
            mode = Mode::idle;
            return false;
        }

        startTimer();
        sendTerminate();
        while(!stop and conWait.waiting and getTimer() < ((PACKET_SIZE + ACKLENGHT) / 4) * SENDTIME){ //Time out happen after the time it takes to send a packet and get ack + an extra ack as guard
            usleep(2000);
        }
        terminateSend++;
    }
    return true;
}

bool DataLinkLayer::sendPacket(vector<bool> &packet){
    int packetsSend = 0;
    ackWait.ID=lastoutID;
    ackWait.waiting=true;

    while(!stop and ackWait.waiting){
        if (packetsSend > 4){
            releaseConnection();
            return false;
        }

        startTimer();
        cout << "pack" << endl;
        physLayer->pushData(packet);
        while(!stop and ackWait.waiting and getTimer() < ((PACKET_SIZE + ACKLENGHT) / 4) * SENDTIME){ //Time out happen after the time it takes to send a packet and get ack + an extra ack as guard
            usleep(2000);
        }
        packetsSend++;
    }
    return true;
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
