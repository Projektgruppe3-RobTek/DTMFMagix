#include "AppLayer.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <boost/algorithm/string.hpp>    

AppLayer AppL;

int main(){
	
	while (true){
		
		string input;
		getline(cin, input);
		istringstream text(input);
		vector<string> args;
		
		while (true){
			string temp;
			if (!(text >> temp)) break;
			args.push_back(temp);
		}
		
		int arg = args.size();
		boost::algorithm::to_lower(args[0]);
		if (args[0] == "send"){
			if (arg == 2){
				AppL.sendFile(args[1]);
			}
			else if (arg == 3){
				AppL.sendFile(args[1], args[2]);
			}
		}
		else if (args[0] == "sendcompressed"){
			if (arg == 2){
				AppL.sendFile(args[1],true);
			}
			else if (arg == 3){
				AppL.sendFile(args[1], args[2],true);
			}
		} 
		else if (args[0] == "request"){
			if (arg == 2){
				AppL.requestFile(args[1]);
			}
			else if (arg == 3){
				AppL.requestFile(args[1], args[2]);
			}
		}
		else if (args[0] == "requestcompressed"){
			if (arg == 2){
				AppL.requestFile(args[1],true);
			}
			else if (arg == 3){
				AppL.requestFile(args[1], args[2],true);
			}
		}
		else if (args[0] == "delete"){
			if (arg == 2){
				AppL.requestDeleteFile(args[1]);
			}
		}
		else if (args[0] == "sendmessage"){
			if (arg == 2){
				AppL.sendMessage(args[1]);
			}
		}
		else if (args[0] == "filetree"){
			if (arg == 2){
				AppL.requestFileTree(args[1]);
			}
			else AppL.requestFileTree(".");
		}
		else if (args[0] == "makedir"){
			if (arg == 2){
				AppL.requestMakeDir(args[1]);
			}
		}	
	}
	return 0;
}
