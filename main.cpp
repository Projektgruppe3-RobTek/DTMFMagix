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

AppLayer AppL;
string commands[]= {"send\0","sendcompressed\0","request\0","requestcompressed\0","delete\0","sendmessage\0","filetree\0","makedir\0"};
#define COMMANDS 8
static char** my_completion( const char * text , int start,  int end);
char* my_generator(const char* text, int state);
char * dupstr (char* s);
bool stringcmp(char *str1, char *str2, int len)
{
    for(int i=0;i<len;i++)
    {
        if (str1[i]!=str2[i]) return false;
    }
    return true;    

}
static char** my_completion( const char * text , int start,  int end)
{
    char **matches;
    matches = (char **)NULL;
 
    if (start == 0)
        matches = rl_completion_matches ((char*)text, &my_generator);
 
    return (matches);
 
}
 
char* my_generator(const char* text, int state)
{
    static int list_index, len;
    char *name;
    if (!state) {
        list_index = 0;
        len = strlen (text);
    }
    while ((name = (char *)commands[list_index].c_str()) and list_index<COMMANDS) {
        list_index++;
        if (stringcmp (name, (char*)text, len))
        {
            return(dupstr(name));
        }
    }
    
    /* If no names matched, then return NULL. */
    return ((char *)NULL);
 
}

int rl_possible_completions_sub(int count,int invoking_key)
{
    return rl_complete_internal('!');
} 
char * dupstr (char* s) {
  char *r;
 
  r = new char[((strlen (s) + 1))];
  strcpy (r, s);
  return (r);
}

int main(){
	char *input, shell_prompt[1000];
	
	// Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_possible_completions_sub);
    rl_attempted_completion_function = my_completion;
	while (true)
	{
		// Create prompt string from user name and current working directory.
        snprintf(shell_prompt, sizeof(shell_prompt), "Yo! What do you wanna do now? Enter a fucking command!$ ");
        
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
	return 0;
}
