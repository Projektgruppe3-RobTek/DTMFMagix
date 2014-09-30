#include "DataLinkLayer.h"

using namespace std;

int main()
{
    DataLinkLayer DataLink;
    DataLink.PlaySync();
    DataLink.PlayTones("23456abcd62732845242722424");
    
    usleep(10000000);
    

}
