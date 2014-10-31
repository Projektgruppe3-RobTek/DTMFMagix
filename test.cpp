#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int> tal;
    for(int i=0;i<8;i++)
    {
        tal.push_back(i);
    }
    tal.erase(tal.end()-2,tal.end());
    for(int i=0;i<tal.size();i++)
    {
        cout << tal[i] << endl;
    }
    return 0;
}