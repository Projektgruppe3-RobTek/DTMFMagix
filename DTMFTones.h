
class DTMFTones
{

    public:
    DTMFTones();
    ~DTMFTones();

    int getFreqL(char tone);
    int getFreqH(char tone);

    private:
    int L1 = 697;
    int L2 = 770;
    int L3 = 852;
    int L4 = 941;

    int H1 = 1209;
    int H2 = 1336;
    int H3 = 1477;
    int H4 = 1633;
};

