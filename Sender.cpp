#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>

#include "DataLinkLayer.h"

using namespace boost::filesystem;
using namespace std;

vector<bool> stringToVectorBool(string dataStr){
	vector<bool> dataBin;
	for (int a = 0; a < dataStr.size(); a++){
		for (int b = 7; b >= 0; b--){
			dataBin.push_back(dataStr[a] & (1 << b));
		}
	}
	return dataBin;
}

string vectorBoolToString(vector<bool> dataBin){
	string dataStr;
	for (int a = 0; a < (dataBin.size() / 8); a++){
		char dataChar = 0;
		for (int b = 0; b < 8 ; b++){
			dataChar += (dataBin[a * 8 + b] << (7 - b));
		}
		dataStr += dataChar;
	}
	return dataStr;
}

vector<bool> loadFile(vector<bool> filename){
	ifstream file(vectorBoolToString(filename), ios::in | ios::binary);
	if (file){
		string dataStr;
		file.seekg(0, ios::end);
		dataStr.resize(file.tellg());
		file.seekg(0, ios::beg);
		file.read(&dataStr[0], dataStr.size());
		file.close();
		return stringToVectorBool(dataStr);
	}
}

DataLinkLayer dll;

int main(){
	dll.pushData(loadFile(stringToVectorBool("data.test")));
	while (true);
}
