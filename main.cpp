#include "DataLinkLayer.h"

using namespace std;

int main()
{
    DataLinkLayer DataLink;
    DataLink.PlaySync();
    DataLink.PlayTones("23456abcd627328452427224243547653374893");
    DataLink.PlayEndSequence();
    usleep(100000000);
    

}
