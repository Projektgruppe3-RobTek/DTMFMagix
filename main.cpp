#include "AppLayer.h"
#include <iostream>
#include <sstream>
#include <string.h>

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
		
		if (args[0] == "Send"){
			if (arg == 2){
				AppL.sendFile(args[1]);
			}
			else if (arg == 3){
				AppL.sendFile(args[1], args[2]);
			}
		}
		else if (args[0] == "Request"){
			if (arg == 2){
				AppL.requestFile(args[1]);
			}
			else if (arg == 3){
				AppL.requestFile(args[1], args[2]);
			}
		}
		else if (args[0] == "Delete"){
			if (arg == 2){
				AppL.requestDeleteFile(args[1]);
			}
		}
		else if (args[0] == "SendMessage"){
			if (arg == 2){
				AppL.sendMessage(args[1]);
			}
		}
		else if (args[0] == "FileTree"){
			if (arg == 2){
				AppL.requestFileTree(args[1]);
			}
			else AppL.requestFileTree(".");
		}
		else if (args[0] == "MakeDir"){
			if (arg == 2){
				AppL.requestMakeDir(args[1]);
			}
		}	
	}
	return 0;
}
