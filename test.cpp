#include <iostream>
#include <vector>
using namespace std;

int main()
{
    char            DTMFTones[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','*','#'};
	vector<bool> bFrame(8);
	vector<char> dFrame((bFrame.size())/4);
    for(int i = 0; i < (bFrame.size())/4; i++)
    {
        bool a = bFrame[i*4];
        bool b = bFrame[i*4+1];
        bool c = bFrame[i*4+2];
        bool d = bFrame[i*4+3];

        dFrame[i] = DTMFTones[a*8+b*4+c*2+d];
    }
    cout << dFrame[0] << dFrame[1] << endl;
    return 0;
}