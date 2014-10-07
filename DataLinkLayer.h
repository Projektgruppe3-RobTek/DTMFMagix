#include <vector>
#include "physicalLayer.h"
#include <sys/time.h>
#define BUFFERSIZE 100
struct Buffer 
{
    array<vector<bool>,BUFFERSIZE> Buffer;
    int head=0;
    int tail=0; 
};

class DataLinkLayer
{
    private:
        void bitStuff(vector<bool> &frame); //Stuff frame to avoid flags in data.
        void revBitStuff(vector<bool> &frame); //Remove stuffing.
        
        void setPadding(vector<bool> &frame); //Pad to multiple of 4, and set length of padding field.
        void removePadding(vector<bool> &frame); //Remove padding. and padding field.
        
        void CRCencoder(vector<bool> &dataWord); //Make dataword into codeword. (append CRC)
        bool CRCdecoder(vector<bool> &codeWord); //Make codeword into dataword, discard frame if corrupt. return false on fail, else true.
        
        bool getID(vector<bool> &frame); //Get id of frame, discard if same as lastID
        void setID(vector<bool> &frame); //set id of frame.
        
        
        int getType(vector<bool> &frame); //Get type of frame.
        void setType(vector<bool> &frame,int type); //Set type of frame.
        
        void setTimer(); //Set the timer
        void sendACK(bool ID); //send ACK.
        void getFrame(); //Grab frames from physical layer.
        
    
    public:
        bool dataAvaliable(); //Is there new data for AppLayer?
        vector<bool> popData(); //return data to AppLayer.
        bool bufferFull(); //is outBuffer full?
        void pushData(vector<bool>); //push data from AppLayer to outBuffer
    
    private:
        physicalLayer physLayer;
        bool lastinID;
        bool lastoutID;
        array<bool,8> flag;
        timeval timer;
        Buffer inBuffer;
        Buffer outBuffer;

};
void getFrameWrapper(DataLinkLayer *DaLLObj);


