#ifndef APP_LAYER_H
#define APP_LAYER_H

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <fstream>
#include <bzlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <cryptopp/md5.h>
#include <cryptopp/hex.h>
#include <thread>
#include "DataLinkLayer.h"
#include <boost/algorithm/string.hpp>
using namespace boost::filesystem;
using namespace std;


#define APP_MAX_FRAME_SIZE	MAX_FRAMESIZE // bit
#define APP_FLAG_SIZE		5   // bit
#define APP_DATA_FRAME_SIZE 	(APP_MAX_FRAME_SIZE - APP_FLAG_SIZE) // bit

#define APP_SIZE_FLAG                       0
#define APP_NAME_FLAG	                    1
#define APP_DATA_FLAG	                    2
#define APP_REQUEST_FILE_FLAG               3
#define APP_DELTE_FILE_FLAG	                4
#define APP_MESSAGE_FLAG	                5
#define APP_FILE_TREE_REQUEST_FLAG	        6
#define APP_MAKE_FOLDER_FLAG	            7
#define APP_HASH_FLAG		                8
#define APP_COMPRESSED_DATA_FLAG            9
#define APP_REQUEST_COMPRESSED_FILE_FLAG    10
#define APP_FILE_TREE_FLAG    				11

class AppLayer{
	
	public:
		
		~AppLayer();
		void sendMessage(string message);
		void sendFile(string fileName,bool compressed=0);
		void sendFile(string fileName, string targetName,bool compressed=0);
		void requestFile(string fileName,bool compressed=0);
		void requestFile(string fileName, string targetName,bool compressed=0);
		void requestDeleteFile(string fileName);
		void requestFileTree(string path);
		void requestMakeDir(string path);
		string stripPath(string filename);
		void receiver();
		int getEstimatedSize();
		int getNumberOfFrames();
		int getFramesSend();
		int getTotalFramesToSend();
		vector<string> &getFileTree();
		static AppLayer* getInstance(bool _cli=0);
		
	private:
		static AppLayer* instance;
		AppLayer(bool _cli=0);
		#ifdef DEBUG
			const bool debug = DEBUG;
		#else
			const bool debug = true;
		#endif
		
		DataLinkLayer *dll;
		
		
		bool cli;
		void sendMessage(vector<bool> message);
		void sendFile(vector<bool> fileName,bool compressed=0);
		void sendFile(vector<bool> fileName, vector<bool> targetName,bool compressed=0);
		void requestFile(vector<bool> fileName,bool compressed=0);
		void requestFile(vector<bool> fileName, vector<bool> targetName,bool compressed=0);
		void requestDeleteFile(vector<bool> fileName);
		void requestFileTree(vector<bool> path);
		void requestMakeDir(vector<bool> path);
		vector<bool> compress(vector<bool> uncompressed);
		vector<bool> decompress(vector<bool> compressed);
		vector<bool> loadFile(vector<bool> filename);
		vector<bool> stringToVectorBool(string dataStr);
		string vectorBoolToString(vector<bool> dataBin);
		int vectorBoolToInt(vector<bool> dataBin);
		int totalFramesToSend=0;
		int FramesSend=0;
		//bool copyFile(vector<bool> source, vector<bool> destination, bool force);
		//bool moveFile(vector<bool> source, vector<bool> destination, bool force);
		
		bool saveFile(vector<bool> dataBin, vector<bool> fileName, bool force);
		vector<bool> loadFileSize(vector<bool> filename);
		bool deleteFile(vector<bool> fileName);
		
		void sendFrames(vector<bool> data, int type);
		bool cmpFlag(vector<bool> dataBin, bool *flag);
		void sendFileDetail(vector<bool> path);
		void sendFileTree(vector<bool> pathTarget);
		vector<bool> MD5(vector<bool> dataBin);
		
		void appendByte(vector<bool> &boolVec, unsigned char byte);
		int estimatedSize=0;
		int numberOfFrames=0;
		vector<string> fileTree;
		thread receiveThread;
		bool stop=false;
		bool startFlag[12][APP_FLAG_SIZE] = {
			{0,0,0,0,0},// size
			{0,0,0,1,0},// name
			{0,0,1,0,0},// data
			{0,0,1,1,0},// requestedFile
			{0,1,0,0,0},// deleteFile
			{0,1,0,1,0},// message
			{0,1,1,0,0},// fileTreeRequest
			{0,1,1,1,0},// make folder
			{1,0,0,0,0},// hash (MD5)
			{1,0,0,1,0},// Compresed data
			{1,0,1,0,0}, // RequestCompressedfile 
			{1,0,1,1,0} // fileTree
		};
		bool endFlag[12][APP_FLAG_SIZE] = {
			{0,0,0,0,1},// size
			{0,0,0,1,1},// name
			{0,0,1,0,1},// data
			{0,0,1,1,1},// requestedFile
			{0,1,0,0,1},// deleteFile
			{0,1,0,1,1},// message
			{0,1,1,0,1},// fileTreeRequest
			{0,1,1,1,1},// make folder
			{1,0,0,0,1},// hash (MD5)
			{1,0,0,1,1},// Compresed data
			{1,0,1,0,1}, // RequestCompressedfile
			{1,0,1,1,1} // fileTree
		};		
};

void receiverWrapper(AppLayer *APObj);

#endif

