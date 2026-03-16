#include "csvparsing.h"
#include "cstdex/cstdex.h"
#include "stringutils/stringUtils.h"

/*
Thread-safe version of splitLineToTokens
*/

void splitLineToTokens_t(char* str,char** tokens,const char* delimiters){
	
	uint stored_idx = 0;
	bool term_found = false;
	char stored_str[LINE_MAX_LEN] = "";
	char token[TOK_MAX_LEN] = "";
	
	char* currToken;
    int t_idx = 0;
	currToken = strtok_ignr_mthread(str,delimiters,"\"",&stored_idx,&term_found,stored_str,token);
	while(currToken != NULL){
		strip(&currToken,'~'); //TODO: less strict version of strip needed here.
		strcpy(tokens[t_idx],currToken);
		t_idx++;
		currToken = strtok_ignr_mthread(NULL,delimiters,"\"",&stored_idx,&term_found,stored_str,token);
	}
}
