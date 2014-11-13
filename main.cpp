#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <unistd.h>
#include "DataLinkLayer.h"

DataLinkLayer dll;

bool flag[] = {0,1,1,1,1,1,1,0};
bool invFlag[] = {0,1,1,1,1,1,1,1,0};

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
	vector<bool> temp;
	return temp;
}

vector<bool> loadFileSize(vector<bool> filename){
	return stringToVectorBool(to_string(file_size(vectorBoolToString(filename))));
}

bool deleteFile(vector<bool> fileName){
	return remove(vectorBoolToString(fileName));
}

bool copyFile(vector<bool> fromFileName, vector<bool> toFileName, bool force = false){
	return saveFile(loadFile(fromFileName), toFileName, force);
}

bool moveFile(vector<bool> fromFileName, vector<bool> toFileName, bool force = false){
	if (!copyFile(fromFileName, toFileName, force = false)) deleteFile(fromFileName);
	else return true;
	return false;
}

vector<bool> bitStuff(vector<bool> dataBin){
	for (int a = 0; a < dataBin.size() - 8; a++){
		bool match = true;
		for (int b = 0; b < 8; b++){
			if (dataBin[a + b] != flag[b]) match = false;
		}
		if (match){
			dataBin.insert(dataBin.begin() + a + 4, 1);
		}
	}
	// Hvis der kommmer problemmer med bitstuffing gentag løkken
	
	return dataBin;
}

vector<bool> invBitStuff(vector<bool> dataBin){
	for (int a = 0; a < dataBin.size() - 9; a++){
		bool match = true;
		for (int b = 0; b < 9; b++){
			if (dataBin[a + b] != invFlag[b]) match = false;
		}
		if (match){
			dataBin.erase(dataBin.begin() + a + 4);
		}
	}
	// Hvis der kommmer problemmer med bitstuffing gentag løkken
	// test for bistuffing md flag
	return dataBin;
}

vector<bool> insertFlag(){
	vector<bool> tempFlag;
	for (int a = 0; a < 8; a++) tempFlag.push_back(flag[a]);
	return tempFlag;
}

vector<bool> constructor(string fileName){
	vector<bool> dataBin;
	vector<bool> tempFlag = insertFlag();
	vector<bool> size = bitStuff(loadFileSize(stringToVectorBool(fileName)));
	vector<bool> name = bitStuff(stringToVectorBool(fileName));
	vector<bool> data = bitStuff(loadFile(stringToVectorBool(fileName)));
	
	dataBin.insert(dataBin.end(), tempFlag.begin(), tempFlag.end());
	dataBin.insert(dataBin.end(), size.begin(), size.end());
	dataBin.insert(dataBin.end(), tempFlag.begin(), tempFlag.end());
	dataBin.insert(dataBin.end(), name.begin(), name.end());
	dataBin.insert(dataBin.end(), tempFlag.begin(), tempFlag.end());
	dataBin.insert(dataBin.end(), data.begin(), data.end());
	dataBin.insert(dataBin.end(), tempFlag.begin(), tempFlag.end());
	
	return dataBin;
}

void coutVectorBool(vector<bool> dataBin){
	for (int a = 0; a < dataBin.size(); a++){
		cout << dataBin[a];
	}
	cout << endl;
}

void sender(vector<bool> dataBin){
	for (int a = 0; a < dataBin.size(); a += 128){
		vector<bool> frame;
		
		if (dataBin.size() > a + 128){
			frame.insert(frame.begin(), dataBin.begin() + a, dataBin.begin() + (a + 128));
		}
		else{
			frame.insert(frame.begin(), dataBin.begin() + a, dataBin.end());
		}
		while (dll.dataBufferFull()) usleep(1000);
		dll.pushData(frame);
		//coutVectorBool(frame);
	}
}

void process(int size){

}
/*
void receiver(vector<bool> dataBin){
	
	vector<bool> frame;
	
	int numberOfFlags = 0;
	
	while (numberOfFlags < 4){
		vector<bool> tempFrame;// = dl.pop();
		frame.insert(frame.end(), tempFrame.begin(), tempFrame.end());
		
		for (int a = 0; a < frame.size() - 8; a++){
			bool match = true;
			for (int b = 0; b < 8; b++){
				if (frame[a + b] != flag[b]) match = false;
			}
			if (match){
				
			}
		}
		
	}
	
}
*/

void receiver(){
	while (true){
		while (dll.dataAvailable()){
			vector<bool> frame = dll.popData();
			saveFile(frame, stringToVectorBool("receivedFile.txt"));
		}
		usleep(1000);
	}
}

thread receiverThread;

int main(){
	
	receiverThread = thread(receiver);
	
	string fileName = "text.txt";
	
	sender(constructor(fileName));
	while (true){
		usleep(1000);
	}
	return 0;
}
