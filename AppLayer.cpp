#include "AppLayer.h"

AppLayer::AppLayer(){
	cout << "Applayer created" << endl;
	receiveThread = thread(receiverWrapper, this);
}

AppLayer::~AppLayer() {
    stop=true;
    receiveThread.join();
}
void AppLayer::receiver(){
	vector<bool> size;
	vector<bool> hash;
	vector<bool> name;
	vector<bool> data;
	
	vector<bool> requestedFile;
	vector<bool> requestedFileName;
	
	numberOfFrames = 0;
	estimatedSize = 0;
	
	while (!stop){
		
		while (!stop and dll.dataAvailable()){
			
			vector<bool> frame = dll.popData();
			cout << "Frame size: " << frame.size() <<  endl;
			numberOfFrames++;
			
			if (estimatedSize != 0) if (debug) cout << "frames received: " << numberOfFrames << " of " << estimatedSize << endl;
			
			if (cmpFlag(frame, startFlag[APP_SIZE_FLAG])){ // start størrelse fra framen
				size.insert(size.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_SIZE_FLAG])){ // stop størrelse fra framen
				size.insert(size.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
				estimatedSize = stoi(vectorBoolToString(size));
				data.reserve((stoi(vectorBoolToString(size)))*APP_DATA_FRAME_SIZE);
			}
			else if (cmpFlag(frame, startFlag[APP_NAME_FLAG])){ // start navn fra framen
				name.insert(name.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_NAME_FLAG])){ // stop navn fra framen
				name.insert(name.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, startFlag[APP_DATA_FLAG]) or cmpFlag(frame, startFlag[APP_COMPRESSED_DATA_FLAG])){ // start data fra framen
				data.insert(data.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_DATA_FLAG]) || cmpFlag(frame, endFlag[APP_COMPRESSED_DATA_FLAG])){ // stop data fra framen
				data.insert(data.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
				if (hash == MD5(data)){
					cout << "Datasize: " << data.size() << endl;
					if (cmpFlag(frame, endFlag[APP_COMPRESSED_DATA_FLAG])){
						saveFile(decompress(data), name, 1);
					}
					else saveFile(data, name, 1);
					if (debug){
						sendMessage(stringToVectorBool("File received and saved!\n"));
						cout << "File reseived and saved as " << vectorBoolToString(name) << endl;
						cout << "Frames received: " << numberOfFrames << endl;
						cout << "Size frames: " << (size.size() - 1) / APP_DATA_FRAME_SIZE + 1 << endl;
						cout << "Name frames: " << (name.size() - 1) / APP_DATA_FRAME_SIZE + 1 << endl;
						cout << "Data frames: " << (data.size() - 1) / APP_DATA_FRAME_SIZE + 1+1 << endl;
						cout << "File size: " << vectorBoolToString(size) << " " << data.size() << " " << data.size() / 8 << endl;
						cout << "Name: " << vectorBoolToString(name) << endl;
				
						cout << endl;
					}
					
				}
				else{
					if (debug) sendMessage(stringToVectorBool("File corrupted!"));
					if (debug) cout << "File corrupted!" << endl;
				}
				hash.clear();
				size.clear();
				name.clear();
				data.clear();
				numberOfFrames = 0;
				estimatedSize = 0;
			}
			else if (cmpFlag(frame, startFlag[APP_REQUEST_FILE_FLAG]) or cmpFlag(frame, startFlag[APP_REQUEST_COMPRESSED_FILE_FLAG])){
				requestedFile.insert(requestedFile.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_REQUEST_FILE_FLAG]) || cmpFlag(frame, endFlag[APP_REQUEST_COMPRESSED_FILE_FLAG])){
				requestedFile.insert(requestedFile.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
				if (exists(vectorBoolToString(requestedFile)) && is_regular_file(vectorBoolToString(requestedFile))){
					if (name.size()){
						if (debug) cout << "File requested: " << vectorBoolToString(requestedFile) << " as " << vectorBoolToString(name) << endl;
						sendFile(requestedFile, name, cmpFlag(frame, endFlag[APP_REQUEST_COMPRESSED_FILE_FLAG]));
						if (debug) cout << "Sending file " << vectorBoolToString(requestedFile) << " as " << vectorBoolToString(name) << endl;
					}
					else{
						if (debug) cout << "File requested: " << vectorBoolToString(requestedFile) << endl;
						sendFile(requestedFile, cmpFlag(frame, endFlag[APP_REQUEST_COMPRESSED_FILE_FLAG]));
						if (debug) cout << "Sending file " << vectorBoolToString(requestedFile) << endl;
					}
				}
				else{
					if (debug) sendMessage(stringToVectorBool("File doesn't exists!"));
				}
				requestedFile.clear();
				requestedFileName.clear();
				name.clear();
				numberOfFrames = 0;
			}
			else if (cmpFlag(frame, startFlag[APP_DELTE_FILE_FLAG])){
				name.insert(name.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_DELTE_FILE_FLAG])){
				name.insert(name.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
				#ifdef ENABLE_DELETE
				if (exists(vectorBoolToString(name))){
					
					////////////////////////
					// Don't delete files //
					////////////////////////
					
					if (is_regular_file(vectorBoolToString(name))){
						if (debug) cout << "Deleting file " << vectorBoolToString(requestedFile) << " (udkommenteret for ikke at slette vigtige filer, men er testet)\n" << endl;
						//deleteFile(name);
						if (debug) sendMessage(stringToVectorBool("File deleted! (udkommenteret for ikke at slette vigtige filer, men er testet)\n"));
					}
					else if (is_directory(vectorBoolToString(name))){
						if (debug) cout << "Deleting folder " << vectorBoolToString(requestedFile) << " (udkommenteret for ikke at slette vigtige filer, men er IKKE testet)\n" << endl;
						//remove_all(vectorBoolToString(name));
						if (debug) sendMessage(stringToVectorBool("Folder deleted!  (udkommenteret for ikke at slette vigtige filer, men er IKKE testet)\n"));
					}
				}
				else{
					if (debug) sendMessage(stringToVectorBool("Path doesn't exists!"));
				}
				#else
				cout << "Can delete files (Define ENABLE_DELETE)" << endl;
				sendMessage("Can delete files (Define ENABLE_DELETE)");
				#endif
				name.clear();
				numberOfFrames = 0;
			}
			else if (cmpFlag(frame, startFlag[APP_MESSAGE_FLAG])){
				data.insert(data.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_MESSAGE_FLAG])){
				data.insert(data.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
				cout << vectorBoolToString(data) << endl;
				data.clear();
				numberOfFrames = 0;
			}
			else if (cmpFlag(frame, startFlag[APP_FILE_TREE_REQUEST_FLAG])){
				name.insert(name.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_FILE_TREE_REQUEST_FLAG])){
				name.insert(name.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
				
				if (debug) cout << "File tree requested!" << endl;
				if (debug) cout << "Sending filetree!\n" << endl;
				
				sendFileTree(name, APP_SHOW_SUBDIRECTORIES);
				
				name.clear();
				numberOfFrames = 0;
			}
			else if (cmpFlag(frame, startFlag[APP_MAKE_FOLDER_FLAG])){
				name.insert(name.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_MAKE_FOLDER_FLAG])){
				name.insert(name.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
				cout << "1" << endl;
				if (!create_directory(vectorBoolToString(name))){
					if (debug) sendMessage(stringToVectorBool("Can't create folder!\n"));
				}
				else{
					if (debug) cout << "Create folder requested!\n" << endl;
					if (debug) sendMessage(stringToVectorBool("Folder created!\n"));
				}
				name.clear();
				numberOfFrames = 0;
			}
			else if (cmpFlag(frame, startFlag[APP_HASH_FLAG])){
				hash.insert(hash.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_HASH_FLAG])){
				hash.insert(hash.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, startFlag[APP_FILE_TREE_FLAG])){
				data.insert(hash.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
			}
			else if (cmpFlag(frame, endFlag[APP_FILE_TREE_FLAG])){
				data.insert(data.end(), frame.begin() + APP_FLAG_SIZE, frame.end());
				fileTree.push_back(vectorBoolToString(data));
				for (int a = 0; a < fileTree.size(); a++){
					cout << fileTree[a] << endl;
				}
				data.clear();
			}
		}
		usleep(1000);
	}
}

void receiverWrapper(AppLayer *ALObj){
	ALObj->receiver();
}

int AppLayer::vectorBoolToInt(vector<bool> dataBin){
	int dataInt = 0;
	for (unsigned int a = 0; a < dataBin.size() / 8; a++){
		char dataChar = 0;
		for (int b = 0; b < 8; b++){
			dataChar |= dataBin[dataBin.size() - 1 - b - 8 * a] << b;
		}
		dataInt += (dataChar - '0') * pow(10, a);
	}

	return dataInt;
}

vector<bool> AppLayer::stringToVectorBool(string dataStr){
	vector<bool> dataBin;
	for (unsigned int a = 0; a < dataStr.size(); a++){
		for (int b = 7; b >= 0; b--){
			dataBin.push_back(dataStr[a] & (1 << b));
		}
	}
	return dataBin;
}

string AppLayer::vectorBoolToString(vector<bool> dataBin){
	string dataStr;
	for (unsigned int a = 0; a < (dataBin.size() / 8); a++){
		char dataChar = 0;
		for (int b = 0; b < 8 ; b++){
			dataChar += (dataBin[a * 8 + b] << (7 - b));
		}
		dataStr += dataChar;
	}
	return dataStr;
}

bool AppLayer::saveFile(vector<bool> dataBin, vector<bool> fileName, bool force){
	if (!force && exists(vectorBoolToString(fileName))){
		return 1;
	}
	ofstream file(vectorBoolToString(fileName));
	file << vectorBoolToString(dataBin);
	file.close();
	return 0;
}

vector<bool> AppLayer::loadFile(vector<bool> fileName){
	ifstream file(vectorBoolToString(fileName), ios::in | ios::binary);
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

vector<bool> AppLayer::loadFileSize(vector<bool> fileName){
	return stringToVectorBool(to_string(file_size(vectorBoolToString(fileName))));
}

bool AppLayer::deleteFile(vector<bool> fileName){
	return remove(vectorBoolToString(fileName));
}

/*
bool AppLayer::copyFile(vector<bool> source, vector<bool> destination, bool force){
	return saveFile(loadFile(source), destination, force);
}

bool AppLayer::moveFile(vector<bool> source, vector<bool> destination, bool force){
	if (!copyFile(source, destination, force)){
		deleteFile(source);
	}
	else{
		return true;
	}
	return false;
}
*/

void AppLayer::sendFrames(vector<bool> dataBin, int type){

    long dataBinOffset=0;
	while ( ((int)dataBin.size())-dataBinOffset >= APP_DATA_FRAME_SIZE){ 
		vector<bool> frame;
		frame.reserve(APP_DATA_FRAME_SIZE);
		frame.insert(frame.begin(), dataBin.begin()+dataBinOffset, dataBin.begin()+dataBinOffset + APP_DATA_FRAME_SIZE);
		dataBinOffset+=APP_DATA_FRAME_SIZE;
		frame.insert(frame.begin(), startFlag[type], startFlag[type] + APP_FLAG_SIZE);
		while (dll.dataBufferFull()){
			usleep(1000);
			if(stop) return;
		}
		dll.pushData(frame);
		FramesSend++;
	}
	
	vector<bool> frame;
	frame.insert(frame.begin(), dataBin.begin()+dataBinOffset, dataBin.begin() + dataBin.size());
	frame.insert(frame.begin(), endFlag[type], endFlag[type] + APP_FLAG_SIZE);
	while (dll.dataBufferFull()){
			usleep(1000);
			if(stop) return;
		}
	dll.pushData(frame);
	FramesSend++;
}

void AppLayer::sendMessage(vector<bool> message){
	sendFrames(message, APP_MESSAGE_FLAG);
}

void AppLayer::sendMessage(string message){
	sendMessage(stringToVectorBool(message));
}

void AppLayer::sendFile(vector<bool> fileName,bool compressed){
	if (exists(vectorBoolToString(fileName)) && is_regular_file(vectorBoolToString(fileName))){
	
	    vector<bool> file=loadFile(fileName);
	    if (compressed) file=compress(file);
	    totalFramesToSend=(file.size() + stringToVectorBool(to_string(file.size()/8)).size() + fileName.size() + CryptoPP::Weak1::MD5::DIGESTSIZE*8) / APP_DATA_FRAME_SIZE + 3;
	    FramesSend=0;
		sendFrames(stringToVectorBool(to_string(totalFramesToSend)), APP_SIZE_FLAG);
		sendFrames(fileName, APP_NAME_FLAG);
		sendFrames(MD5(file), APP_HASH_FLAG);
		if (compressed)	sendFrames(file, APP_COMPRESSED_DATA_FLAG);
		else sendFrames(file, APP_DATA_FLAG);
	}
	else{
		if (debug) cout << "File doesn't exists!" << endl;
	}
}

void AppLayer::sendFile(string fileName,bool compressed){
	sendFile(stringToVectorBool(fileName),compressed);
}

void AppLayer::sendFile(vector<bool> fileName, vector<bool> targetName,bool compressed){
	
	if (exists(vectorBoolToString(fileName)) && is_regular_file(vectorBoolToString(fileName))){
	    vector<bool> file=loadFile(fileName);
	    if (compressed) file=compress(file);
	    totalFramesToSend=(file.size() + stringToVectorBool(to_string(file.size()/8)).size() + targetName.size() + CryptoPP::Weak1::MD5::DIGESTSIZE*8) / APP_DATA_FRAME_SIZE + 3;
	    FramesSend=0;
		sendFrames(stringToVectorBool(to_string(totalFramesToSend)), APP_SIZE_FLAG);
		sendFrames(targetName, APP_NAME_FLAG);
		sendFrames(MD5(file), APP_HASH_FLAG);
		if (compressed)	sendFrames(file, APP_COMPRESSED_DATA_FLAG);
		else sendFrames(file, APP_DATA_FLAG);
	}
	else{
		if (debug) cout << "File doesn't exists!" << endl;
	}
}

void AppLayer::sendFile(string fileName, string targetName,bool compressed){
	sendFile(stringToVectorBool(fileName), stringToVectorBool(targetName),compressed);
}

void AppLayer::requestFile(vector<bool> fileName,bool compressed){
	if (compressed) sendFrames(fileName, APP_REQUEST_COMPRESSED_FILE_FLAG);
	else sendFrames(fileName, APP_REQUEST_FILE_FLAG);
}

void AppLayer::requestFile(string fileName, bool compressed){
	requestFile(stringToVectorBool(fileName),compressed);
}

void AppLayer::requestFile(vector<bool> fileName, vector<bool> targetName, bool compressed){
	sendFrames(targetName, APP_NAME_FLAG);
	if (compressed) sendFrames(fileName, APP_REQUEST_COMPRESSED_FILE_FLAG);
	else sendFrames(fileName, APP_REQUEST_FILE_FLAG);
}

void AppLayer::requestFile(string fileName, string targetName,bool compressed){
	requestFile(stringToVectorBool(fileName), stringToVectorBool(targetName),compressed);
}

void AppLayer::requestDeleteFile(vector<bool> fileName){
	sendFrames(fileName, APP_DELTE_FILE_FLAG);
}

void AppLayer::requestDeleteFile(string fileName){
	requestDeleteFile(stringToVectorBool(fileName));
}

void AppLayer::requestFileTree(vector<bool> path){
	sendFrames(path, APP_FILE_TREE_REQUEST_FLAG);
}

void AppLayer::requestFileTree(string path){
	requestFileTree(stringToVectorBool(path));
}

void AppLayer::requestMakeDir(vector<bool> path){
	sendFrames(path, APP_MAKE_FOLDER_FLAG);
}

void AppLayer::requestMakeDir(string path){
	requestMakeDir(stringToVectorBool(path));
}

bool AppLayer::cmpFlag(vector<bool> dataBin, bool *flag){
	for (int a = 0; a < APP_FLAG_SIZE; a++){
		if (dataBin[a] != flag[a]){
			return false;
		}
	}
	return true;
}

void AppLayer::sendFileDetail(vector<bool> fileName){
	sendFrames(fileName, APP_FILE_TREE_FLAG);
}

void AppLayer::sendFileTree(vector<bool> pathTarget, bool subdirectories){
	vector<path> filePath;
	copy(directory_iterator(vectorBoolToString(pathTarget)), directory_iterator(), back_inserter(filePath));
	sort(filePath.begin(), filePath.end());
	sendFileDetail(pathTarget);
	for (unsigned int a = 0; a < filePath.size(); a++){
	    if(stop) return;
	    
		if (is_regular_file(filePath[a])){
			sendFileDetail(stringToVectorBool(filePath[a].string()));
		}
		else{
			if (subdirectories){
				sendFileTree(stringToVectorBool(filePath[a].string()), 0);
			}
			else{
				sendFileDetail(stringToVectorBool(filePath[a].string()));
			}
		}
	}
}

void AppLayer::appendByte(vector<bool> &dataBin, unsigned char byte){
	for (int i = 7; i >= 0; i--){
		if (byte-(1<<i) >= 0){
			byte-=(1 << i);
			dataBin.push_back(1);
		}
		else{
			dataBin.push_back(0);
		}
	}
}

vector<bool> AppLayer::MD5(vector<bool> dataBin){
	string filestr = vectorBoolToString(dataBin);
	char *memblock;
	memblock = const_cast<char*>(filestr.c_str());
	byte digest[CryptoPP::Weak1::MD5::DIGESTSIZE];
	
	CryptoPP::Weak1::MD5 hash;
	hash.CalculateDigest(digest, (byte*) memblock, filestr.size());
	
	vector<bool> md5;
	for (int a = 0; a < CryptoPP::Weak1::MD5::DIGESTSIZE; a++){
		appendByte(md5, digest[a]);
	}
	return md5;
}

vector<bool> AppLayer::compress (vector<bool> uncompressed)
{
    cout << uncompressed.size() << endl;
    vector<bool> compressed;
    string filestr = vectorBoolToString(uncompressed);
	char *memblockin;
	unsigned int destlenght=(int)(1.01 * filestr.size() + 600);
	char *memblockout = new char[destlenght];
	memblockin = const_cast<char*>(filestr.c_str());
	unsigned int filelenght=filestr.size();
	if(BZ2_bzBuffToBuffCompress(memblockout, &destlenght, memblockin, filelenght, 9, 0, 30)!=BZ_OK)
	{
	    cout << "Error while compressing" << endl;
	    delete memblockin;
	    delete memblockout;
	    return compressed;
	}
	for(unsigned int i=0; i<destlenght; i++)
	{
	    appendByte(compressed,memblockout[i]);
	}
	cout << compressed.size() << endl;
	delete memblockout;
	return compressed;
}
string AppLayer::stripPath(string filename)
{
    int lastslashpos=filename.find_last_of("/");
    return filename.substr(lastslashpos+1);
    
}
vector<bool> AppLayer::decompress(vector<bool> compressed)
{
    vector<bool> decompressed;
    string compressedstr = vectorBoolToString(compressed);
    unsigned int compressedsize=compressedstr.size();
    char *compressedmem=const_cast<char*>(compressedstr.c_str());
    unsigned int decompressedsize=compressedsize;
    char *decompressedmem = new char[decompressedsize];
    while( (BZ2_bzBuffToBuffDecompress(decompressedmem,&decompressedsize,compressedmem,compressedsize,0,0)==BZ_OUTBUFF_FULL))
    {
        delete decompressedmem;
        decompressedsize*=2;
        decompressedmem = new char[decompressedsize];
    }
    for(unsigned int i=0; i<decompressedsize; i++)
	{
	    appendByte(decompressed,decompressedmem[i]);
	}
    delete decompressedmem;
    return decompressed;
}
int AppLayer::getEstimatedSize()
{
    return estimatedSize;
}
int AppLayer::getNumberOfFrames()
{
    return numberOfFrames;
}
int AppLayer::getFramesSend()
{
    return FramesSend-dll.dataBufferSize();
}
int AppLayer::getTotalFramesToSend()
{
    return totalFramesToSend;
}
vector<string> &AppLayer::getFileTree()
{
    return fileTree;
}
