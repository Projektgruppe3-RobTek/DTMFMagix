#include "md5Generator.h"
void md5Generator::appendByte(std::vector<bool> &boolVec, unsigned char byte)
{
    for(int i=7;i>=0;i--)
    {
        if (byte-(1<<i) >= 0)
        {
            byte-=(1 << i);
            boolVec.push_back(1);
        }
        else boolVec.push_back(0);
    }
}

std::string md5Generator::vectorBoolToString(std::vector<bool> &dataBin){
	std::string dataStr;
	for (int a = 0; a < (dataBin.size() / 8); a++){
		char dataChar = 0;
		for (int b = 0; b < 8 ; b++){
			dataChar += (dataBin[a * 8 + b] << (7 - b));
		}
		dataStr += dataChar;
	}
	return dataStr;
}


std::vector<bool> md5Generator::doMd5(std::vector<bool> &file)
{
    CryptoPP::Weak1::MD5 hash;
    byte digest[ CryptoPP::Weak1::MD5::DIGESTSIZE ];
    std::string filestr=vectorBoolToString(file);
    char *memblock=new char[filestr.size()];
    memblock=const_cast<char*>(filestr.c_str());
    hash.CalculateDigest( digest, (byte*) memblock, filestr.size() );
    std::vector<bool> md5;
    for(int i=0;i<CryptoPP::Weak1::MD5::DIGESTSIZE;i++)
    {
        appendByte(md5,digest[i]);
    }
    return md5;
}
