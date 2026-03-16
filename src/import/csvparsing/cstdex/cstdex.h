#ifndef CSTDEX_H
#define CSTDEX_H

#include <cstdlib>
#include <cstring>
#include <cstdio>

#define TOK_MAX_LEN 250
#define LINE_MAX_LEN 1000

typedef unsigned int uint;
typedef unsigned long ulong;

/*
 * * * C STANDARD LIBRARY EXTENSION * * *
*/


/*
Provides similar functionality with fgets,but for a string input not belonging in a file stream.
input_str is read char-by-char until a user-supplied character, a newline or a terminating character is encountered.
input_str is assumed to be non-null and to have a prior appropriate memory allocation.
output_str does not contain newlines after the operation, but it will contain the user-supplied character, if it is encountered.
returns the value of input_idx corresponding to the first character after newline/terminating.
*/
int sgets_general(const char* input_str,char* output_str,ulong input_idx,char stoppingch);

/*
Provides similar functionality with fgets,but for a string input not belonging in a file stream.
input_str is read char-by-char until a newline or terminating character is encountered.
input_str is assumed to be non-null and to have a prior appropriate memory allocation.
input_idx is the index of the first character of input_str to be scanned.
output_str does not contain newlines after the operation.
returns the value of input_idx corresponding to the first character after newline/terminating.
*/
int sgets(const char* input_str,char* output_str,ulong input_idx);


/*
Detects whether or not a character 'ch' is contained in the string 'str'. 
*/
bool chinstr(char ch,const char* str);

/*
Extension of c standard library strtok, which also accepts the 'spch' parameter.
The 'spch' parameter represents the special characters that signal the beggining and/or the end
of the parts of the input str that will not be scanned for delimiters and will be taken
as is.

For example, for str == "A,list,"of,comma,separated",values" , delimiters == "," and spch == "\""

the resulting tokens will be :
A
list
of,comma,separated
values
*/

char* strtok_ignr(char* str,const char* delimiters,const char* spch);

/*
Thread safe version of strtok_ignr
*/
char* strtok_ignr_mthread(char* str,const char* delimiters,const char* spch,uint* stored_idx,
						  bool* term_found,char* stored_str,char* token);


int strchr_idx(char* str,const char* characters);
char* strchr_n(char* str,const char* unwantedchars);
int strchr_nidx(char* str,const char* unwantedchars);

/*
Expands upon the functionality of strcpy, enabling the copying of only a part of the src string.
assumes dest is properly allocated and not null.
end_idx is inclusive and should not point to a terminating character.
The terminating character is added manually to the end of dest.
*/
void partialstrcpy(char* dest,const char* src,ulong start_idx,ulong end_idx,bool ignore_newlines = true);

#endif
