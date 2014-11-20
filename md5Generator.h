#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>
#include <cryptopp/hex.h>
#include <vector>

class md5Generator
{
    private:
        void appendByte(std::vector<bool> &boolVec, unsigned char byte);
        std::string vectorBoolToString(std::vector<bool> &dataBin);
    public:
        std::vector<bool> doMd5(std::vector<bool> &file);
};

