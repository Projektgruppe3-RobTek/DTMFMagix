#include "DataLinkLayer.h"

DataLinkLayer::DataLinkLayer()
{
    getFramesThread = thread(getFramesWrapper, this);
    getDatagramsThread = thread(getDatagramsWraper, this);
}


void DataLinkLayer::getFrames()
{
    while(true) usleep(4000000000); //not done
}

void DataLinkLayer::getDatagrams(){
    vector<bool> data_to_send;
    bool connectionLost = false;

    while(true){
        while(mode == 2){
            sleep(10);
        }

        while(!outBuffer.empty()){
            if(!connectionRequest()) break;

            if(!connectionLost) data_to_send = outBuffer.pop_front();
            else connectionLost = false;

            setType(data_to_send, 0);
            setID(data_to_send);
            CRCencoder(data_to_send);
            bitStuff(data_to_send);
            setPadding(data_to_send);

            if(!sendPacket(data_to_send)) {
                connectionLost = true;
                break;
            }
        }

        if(outBuffer.empty() && mode){
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
    //Push to datalinklayer
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

bool DataLinkLayer::bufferFull()
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

bool DataLinkLayer::connectionRequest(){
    int requestsSend = 0;

    while(mode != 1){
        if (requestsSend > 5){
            sleep(((53 + 32) * sendTime) + 100 + rand() % 1000); //ack 53 tone, data max length 32 tone, 100 is added as a guard and a random time
            if(mode == 2) return false;
        }

        startTimer();
        sendRequest(!lastoutID);

        while(getTimer() < ((53 + 32) * sendTime) + 100){ //ack 53 tone, data max length 32 tone, 100 is added as a guard
            sleep(2);
            lastoutID = !lastoutID;
        }

        requestsSend++;
    }
    return true;
}

bool DataLinkLayer::releaseConnection(){
    int terminateSend = 0;

        while(!mode){
            if (terminateSend > 5){
                mode = 0;
                return false;
            }

            startTimer();
            sendTerminate(!lastoutID);

            while(getTimer() < getTimer() < ((53 + 32) * sendTime) + 100){ //ack 53 tone, data max length 32 tone, 100 is added as a guard
                sleep(2);
                lastoutID = !lastoutID;
            }

        terminateSend++;
    }
    mode = 0;
    return true;
}

bool DataLinkLayer::sendPacket(vector<bool> &packet){
    int packetsSend = 0;

        while(!ack){
            if (packetsSend > 5){
                sendTerminate(!lastoutID);
                lastoutID = !lastoutID;
                mode = 0;
                return false;
            }

            while(physLayer.isQueueFull()){
                sleep(10);
            }

            startTimer();
            physLayer.QueueFrame(packet);

            while(getTimer() < ((53 + 32) * sendTime) + 100){ //ack 53 tone, data max length 32 tone, 100 is added as a guard
                sleep(2);
            }

        packetsSend++;
    }
    ack = !ack;
    return true;
}

