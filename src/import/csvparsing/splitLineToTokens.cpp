#include "csvparsing.h"
#include "cstdex/cstdex.h"
#include "stringutils/stringUtils.h"

/*
splits a given string into tokens,with user-passed delimiters.
output : tokens , assumes 'tokens' is not null and is allocated properly, prior to function call.

Columns that are enclosed in ' or " will not be scanned for delimiters.
For example, if delimiter = "," and a column has the value: 'a,string,with,commas' ,then this column will
be considered as a single token (instead of 4).

*/ 

void splitLineToTokens(char* str,char** tokens,const char* delimiters){
	char* currToken;
    int t_idx = 0;
	currToken = strtok_ignr(str,delimiters,"\"");
	while(currToken != NULL){
		strip(&currToken,'~'); //TODO: less strict version of strip needed here.
		strcpy(tokens[t_idx],currToken);
		t_idx++;
		currToken = strtok_ignr(NULL,delimiters,"\"");
	}
}
