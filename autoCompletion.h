#pragma once
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>    
#include <string>
using namespace std;

static char** customCompletion( const char * text , int start,  int end);
char* commandGenerator(const char* text, int state);
char * dupstr (char* s);
bool stringcmp(char *str1, char *str2, int len);
int rl_possible_completions_sub(int count,int invoking_key);
