#include "DataLinkLayer.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <sys/time.h>

#include <thread>

using namespace std;

bool connectionEstablished =  false;

void transmit_Thread(){
    cout << "Entered transmit_Thread" << endl;

    //If there is something in the buffer continue else keep looking

        //divide the data into smaller segments for the data linklayer to send

        while (!DaLLObj.connectionEstablished){
            master = true;
            for (int i = 0; i < 5; i++){
                if (!DaLLObj.connectionEstablished){
                    DaLLObj.sendRequest();
                    DaLLObj.startTimer();
                    while(DaLLObj.getTimer() < 500/*The time we decide*/){
                        sleep(10);
                    }
                } else break;
            }
            //If after 5 tires the connection was not established step back for a while
            master = false;
            sleep(1000 + rand() % 100);
        }


        myMutex.lock();

        myMutex.unlock();
}

void recive_Thread(){
    cout << endl << "Entered recive_Thread" << endl;

    //Physlayer.getFrame();

    DaLLObj.removePadding(); //Removes the padding and the Length of Padding header

    DaLLObj.revBitStuff(); //Remove bit stuffing

    if (!DaLLObj.CRCdecoder()) break; //Syndrom = 0 An error and the message is discarded, Syndrom = 1 the message is kept

    if (!DaLLObj.getID()) break; //If the ID is the same as the preivous ID the message is discarded

    switch (DaLLObj.getType()){
        case 0b001: //ACK
            //Tell transmitter that he can send next frame

        case 0b010: //connection request (CR)
            //If not busy
            DaLLObj.sendAccept();
            //else
            DaLLObj.sendDecline();

        case 011: //confirm
            if (!connectionEstablished) connectionEstablished = true;
            else connectionEstablished = false;

        case 100: //connection refuse (CREF)
            //if the connection request was declined we wait a bit and try again
            sleep(100);
            DaLLObj.sendRequest();

        case 101: //release connecion (RC)
            connectionEstablished = false;

        case 111: //interactivity test (IT)
            DaLLObj.sendACK();

        default: // data
            DaLLObj.sendACK();
            //push to app buffer
    }

    myMutex.lock();

    myMutex.unlock();
}

int main(int argc, char *argv[])
{
    DataLinkLayer DaLLObj;

    //Test threads
    thread firstThread(transmit_Thread);
    thread secondThread(recive_Thread);
}
