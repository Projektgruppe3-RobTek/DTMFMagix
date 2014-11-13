#include "DTMFTones.h"

DTMFTones::DTMFTones()
{

}

DTMFTones::~DTMFTones()
{

}

int DTMFTones::getFreqL(char tone)
{
    if (tone == '1' ||tone == '2' || tone == '3' ||tone == 'A')
    {
        return L1;
    }
    else if(tone == '4' ||tone == '5' || tone == '6' || tone == 'B' )
    {
        return L2;
    }
    else if(tone == '7' || tone == '8' || tone == '9' || tone == 'C')
    {
        return L3;
    }
    else if(tone == '*' || tone == '0' || tone == '#' || tone == 'D')
    {
        return L4;
    }
    else
        return 0;
    }

int DTMFTones::getFreqH(char tone)
{
    if (tone == '1' ||tone == '4' ||tone == '7' ||tone == '*')
    {
        return H1;
    }
    else if(tone == '2' || tone == '5' || tone == '8' || tone == '0')
    {
        return H2;
    }
    else if (tone == '3' || tone == '6' || tone == '9' || tone =='#')
    {
        return H3;
    }
    else if (tone == 'A' || tone == 'B' || tone == 'C' || tone == 'D')
    {
        return H4;
    }
    else return 0;
}
