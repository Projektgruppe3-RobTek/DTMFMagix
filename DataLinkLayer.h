#include <vector>
#include <sys/time.h>
#include <thread>
#include <array>
#include "physicalLayerTest.h"
#define BUFFERSIZE 100

/*
Layout of frame:
|--|----|-----------------|----|---|
|ID|Type|Length of padding|Data|CRC|
|1 | 3  |       2         |  ? |8? |
|--|----|-----------------|----|---|
Frametypes:
000 = data
001 = ACK
010 = request
011 = accept
100 = decline
101 = terminate
*/
using namespace std;
struct Buffer
{
    array<vector<bool>, BUFFERSIZE> Buffer;
    int head = 0;
    int tail = 0;
};

class DataLinkLayer
{
    public:
        void bitStuff(vector<bool> &frame); //Stuff frame to avoid flags in data.
        void revBitStuff(vector<bool> &frame); //Remove stuffing.

        void setPadding(vector<bool> &frame); //Pad to multiple of 4, and set length of padding field.
        void removePadding(vector<bool> &frame); //Remove padding.. and padding field.

        void CRCencoder(vector<bool> &dataWord); //Make dataword into codeword. (append CRC)
        bool CRCdecoder(vector<bool> &codeWord); //Make codeword into dataword, discard frame if corrupt. return false on fail, else true.

        bool getID(vector<bool> &frame); //Get id of frame, discard if same as lastID
        void setID(vector<bool> &frame); //set id of frame.

        int getType(vector<bool> &frame); //Get type of frame.
        void setType(vector<bool> &frame, int type); //Set type of frame.

        void setTimer(); //Set the timer
        void sendACK(bool ID); //send ACK.
        void sendRequest(bool ID); //send Request
        void sendAccept(bool ID); //send Accept
        void sendDecline(bool ID); //send Decline
        void sendTerminate(bool ID); //send Terminate

    public:
        DataLinkLayer();
        bool dataAvaliable(); //Is there new data for AppLayer?
        vector<bool> popData(); //return data to AppLayer.
        bool bufferFull(); //is outBuffer full?
        void pushData(vector<bool>); //push data from AppLayer to outBuffer
        void getFrames(); //Grab frames from physical layer, parse to AppLayer if reqiured.
        void getDatagrams(); //Grab frames from inBuffer and parse to physical layer.

    private:
        //physicalLayer physLayer;
        bool lastinID;
        bool lastoutID;
        array<bool, 8> flag={{1, 0, 1, 0, 1, 1, 1, 0}};
        timeval timer;
        Buffer inBuffer;
        Buffer outBuffer;
        thread getFramesThread;
        thread getDatagramsThread;

};
void getFramesWrapper(DataLinkLayer *DaLLObj);
void getDatagramsWraper(DataLinkLayer *DaLLObj);

bool flagcheck(vector<bool> &vec1, int start1,array<bool, 8> &flag, int lenght);


