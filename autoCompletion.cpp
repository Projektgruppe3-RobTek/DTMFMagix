#include "autoCompletion.h"
string commands[]= {"send\0","sendcompressed\0","request\0","requestcompressed\0","delete\0","sendmessage\0","filetree\0","makedir\0"};
bool stringcmp(char *str1, char *str2, int len)
{
    for(int i=0;i<len;i++)
    {
        if (str1[i]!=str2[i]) return false;
    }
    return true;    
}


char* commandGenerator(const char* text, int state)
{
    static int list_index, len;
    char *name;
    if (!state) {
        list_index = 0;
        len = strlen (text);
    }
    while ((name = (char *)commands[list_index].c_str()) and list_index<sizeof(commands)/sizeof(string)) {
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
