#define SDL_MAIN_HANDLED
#include "AppLayer.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <boost/algorithm/string.hpp>
#include <readline/readline.h>
#include <readline/history.h>  
#include "autoCompletion.h"  

static char** customCompletion( const char * text , int start,  int end)
{
    char **matches;
    matches = (char **)NULL;
 
    if (start == 0)
        matches = rl_completion_matches ((char*)text, &commandGenerator);
 
    return (matches);
}
int main(){
	cout << "test" << endl;
	char *input, shell_prompt[1000];
	
	// Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_possible_completions_sub);
    rl_attempted_completion_function = customCompletion;
	AppLayer AppL; 
	while (true)
	{
		// Create (empty) prompt string
        snprintf(shell_prompt, sizeof(shell_prompt), "");
        
        // Display prompt and read input (n.b. input must be freed after use)...
        input = readline(shell_prompt);
           
        // Check for EOF.
        if (!input) break;
        
        string inputstring=input;
		istringstream text(inputstring);
		vector<string> args;
		
		while (true){
			string temp;
			if (!(text >> temp)) break;
			args.push_back(temp);
		}
		
		
		int arg = args.size();
		if (arg==0) continue;
		add_history(input);
		
		if (args[0] == "send"){
			if (arg == 2){
				AppL.sendFile(args[1], AppL.stripPath(args[1]));
			}
			else if (arg == 3){
				AppL.sendFile(args[1], args[2]);
			}
		}
		else if (args[0] == "sendcompressed"){
			if (arg == 2){
				AppL.sendFile(args[1], AppL.stripPath(args[1]),true);
			}
			else if (arg == 3){
				AppL.sendFile(args[1], args[2],true);
			}
		} 
		else if (args[0] == "request"){
			if (arg == 2){
				AppL.requestFile(args[1], AppL.stripPath(args[1]));
			}
			else if (arg == 3){
				AppL.requestFile(args[1], args[2]);
			}
		}
		else if (args[0] == "requestcompressed"){
			if (arg == 2){
				AppL.requestFile(args[1], AppL.stripPath(args[1]),true);
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
		else
		{
		    cout << "Command \"" << inputstring << "\" not recognized." << endl;
		}
        delete input;
	}
	exit(0);
}
