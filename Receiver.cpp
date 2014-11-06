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

bool saveFile(vector<bool> dataBin, vector<bool> dir, bool force = false){
	if (!force && exists(vectorBoolToString(dir))) return 1;
	ofstream file(vectorBoolToString(dir));
	file << vectorBoolToString(dataBin);
	file.close();
	return 0;
}

DataLinkLayer dll;

int main(){
	while (true){
		if (dll.dataAvailable()){
			if (saveFile(dll.popData(), stringToVectorBool("data.test1"))) cout << "File is received!";
			else cout << "File already exists!" << endl;
		}
	}
}
