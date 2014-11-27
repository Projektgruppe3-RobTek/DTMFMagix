#pragma once
#include <vector>
#include <sys/time.h>
#include <boost/thread.hpp>
#include <array>
#include <iostream>
#include "PhysicalLayer.h" // Lyd
//#include "PhysicalLayerEmu.h" // Fil
#include "RingBuffer.h"
#define BUFFERSIZE 100
#define SENDTIME (TONELENGTH+SILENTLENGTH)
#define MAX_FRAMESIZE 512

using namespace std;
/*
Layout of frame:
|-----------------|----|----|----|---|
|Length of padding| ID |Type|Data|CRC|
|       2         | 1  | 3  |  ? | 4 |
|-----------------|----|----|----|---|
Frametypes:
000 = data
001 = ACK
010 = request
011 = accept
100 = decline
101 = terminate

*/
/* Befor sending frame, we do this in this order:
1. Prepend Type.
2. Prepend ID.
3. Do CRC
4. Do Bitstuffing
5. Prepend paddinglenght (and pad)

When recieving a frame, we do this:
1. Remove padding lenght and padding.
2. Remove bitstuffing.
3. Verify CRC and remove CRC field.
4. Read ID and remove ID field.
5. Read Type and remove Type field.

*/

struct ack {
    bool ID;
    bool waiting=false;
};

struct Connection {
    bool waiting=false;
    bool type=0; //0=terminate, 1=connect request
};
enum class Mode {idle, server, client};

class DataLinkLayer
{
    private:
        void bitStuff(vector<bool> &frame); //Stuff frame to avoid flags in data.
        void revBitStuff(vector<bool> &frame); //Remove stuffing.

        void setPadding(vector<bool> &frame); //Pad to multiple of 4, and set length of padding field.
        void removePadding(vector<bool> &frame); //Remove padding.. and padding field.

        void CRCencoder(vector<bool> &dataWord); //Make dataword into codeword. (append CRC)
        bool CRCdecoder(vector<bool> &codeWord); //Make codeword into dataword, discard frame if corrupt. return false on fail, else true.

        bool getID(vector<bool> &frame); //Get id of frame
        void setID(vector<bool> &frame); //set id of frame.
        void setID(vector<bool> &frame, int ID); //set id of frame.

        int getType(vector<bool> &frame); //Get type of frame.
        void setType(vector<bool> &frame, int type); //Set type of frame.

        void startTimer(); //Start the timer
        int getTimer(); //Return milliseconds since setTimer was called.
        void sendACK(bool ID); //send ACK.
        void sendRequest(bool ID); //send Request
        void sendAccept(bool ID); //send Accept
        void sendDecline(bool ID); //send Decline
        void sendTerminate(bool ID); //send Terminate
        void sendControl(int Type,bool ID);
        void sendFrame(vector<bool> &frame);
        bool connect(); //Try to set the node as master. This is blocking.
        bool terminate(); //Flush the data queue and terminate master status. This is blocking.

    public:
        DataLinkLayer();
        bool dataAvailable(); //Is there new data for AppLayer?
        vector<bool> popData(); //return data to AppLayer.
        bool dataBufferFull(); //is outBuffer full?
        int dataBufferSize();
        void pushData(vector<bool>); //push data from AppLayer to outBuffer
        void getFrames(); //Grab frames from physical layer, parse to AppLayer if reqiured.
        void getDatagrams(); //Grab frames from inBuffer and parse to physical layer.
        int getMode(); //Return mode
        ~DataLinkLayer();
    private:
        PhysicalLayer physLayer;// Lyd
        bool lastinID=0;
        bool lastoutID=0;
        bool connectionRequest();
        bool releaseConnection();
        bool sendPacket(vector<bool> &packet);
        array<bool, 8> flag={{1, 0, 1, 0, 1, 1, 1, 0}};
        timeval timer;
        RingBuffer<vector<bool>,BUFFERSIZE> inBuffer;
        RingBuffer<vector<bool>,BUFFERSIZE> outBuffer;
        boost::thread getFramesThread;
        boost::thread getDatagramsThread;
        ack ackWait;
        Connection conWait;
        Mode mode=Mode::idle;
        bool stop=false;
};
void getFramesWrapper(DataLinkLayer *DaLLObj);
void getDatagramsWraper(DataLinkLayer *DaLLObj);
void getDatagramsAndFramesThread(DataLinkLayer *DaLLObj);
bool flagcheck(vector<bool> &vec1, int start1,array<bool, 8> &flag, int lenght);

