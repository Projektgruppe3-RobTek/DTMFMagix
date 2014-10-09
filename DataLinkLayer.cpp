#include "DataLinkLayer.h"

DataLinkLayer::DataLinkLayer()
{
    getFramesThread=thread(getFramesWrapper,this);
    getDatagramsThread=thread(getDatagramsWraper, this);
}


void DataLinkLayer::getFrames()
{
    while(true) usleep(4000000000); //not done
}

void DataLinkLayer::getDatagrams()
{
    while(true) usleep(4000000000); //not done.
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

    while(frame.size()+2 % 4)
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
    if (lastinID == ID) return false;
    lastinID = ID;
    return true;
}

void DataLinkLayer::setID(vector<bool> &frame)
{
    lastoutID = !lastoutID;
    frame.insert(frame.begin(), lastoutID);
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
    frame.insert(frame.begin(), Type % 2);
    frame.insert(frame.begin(), Type % 4 - Type % 2);
    frame.insert(frame.begin(), Type % 8 - Type % 4 - Type % 2);
}

void DataLinkLayer::sendACK(bool ID)
{
    vector<bool> ACK;
    setType(ACK, 1);
    setID(ACK);
    CRCencoder(ACK);
    //bitstuffing
    //padding
    //Push to physicalLayer.buffer
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


bool flagcheck(vector<bool> &vec1, int start1, array<bool, 8> &flag, int lenght) //check if the flag matches given vec
{
    for(int i = 0 ; i < lenght; i++)
    {
        if (vec1[start1 + i] != flag[i]) return false;
    }
    return true;
}
