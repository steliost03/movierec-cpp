#ifndef CSVHEADER_SERIAL_H
#define CSVHEADER_SERIAL_H

#include <cstdlib>
#include <cstdio>
#include <cstring>

#define MAXLINELEN 1000
#define MAXTOKENLEN 250

typedef unsigned int uint;
typedef unsigned long ulong;

struct file {

	FILE* filepointer;
	uint linecount;
	ulong charcount;
	char* filetext;
	ulong* newlinePositions; //in terms of character count.
	int colcount;
	char** filetextBlocks; //deprecated.

};

uint linecount(FILE* file,int lines_to_ignore);
ulong charcount(FILE* file,int lines_to_ignore);
void linecharcount(file& target_file,int lines_to_ignore);
bool checkfile(FILE* file,const char* path,bool terminateOnError);
void storeFiletext(file& target_file,int lines_to_ignore);
void adjustForTrailingNewlines(file& target_file);

bool filecheckedopen(const char* path,const char* op,FILE** filepointer,bool terminateOnError);
void splitTextBlockToTokens(char* textBlock,const char* delimiters,char*** tokenlist,uint startingLineIdx = 0,bool mthread = false);
void splitLineToTokens(char* str,char** tokens,const char* delimiters);
void splitLineToTokens_t(char* str,char** tokens,const char* delimiters); //thread-safe version.
void processFile(file& target_file,int lines_to_ignore=0,bool ignore_trailing_newlines = true);
bool openAndProcessFile(const char* path,file& target_file,bool terminateOnError,int lines_to_ignore=0,bool ignore_trailing_newlines = true);


void throwErrorAndTerminate(const char* message);

void parseTextBlock_float(char* textBlock,float** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,int idx_col=-1,bool fileidxcol_startswithzero=false,uint startingMtrxLineIdx = 0,bool mthread = false);

void parseTextBlock_strings(char* textBlock,char*** matrix,char*** tokenlist,const char* delimiters,
						  uint colcount,uint linecount,bool tolowercase,int idx_col=-1,bool fileidxcol_startswithzero=false,uint startingMtrxLineIdx = 0,bool mthread = false);

void parseTextBlock_singleString(char* textBlock,char** matrix,char*** tokenlist,const char* delimiters,
						  uint colcount,uint linecount,bool tolowercase,int idx_col=-1,bool fileidxcol_startswithzero=false,uint startingMtrxLineIdx = 0,bool mthread = false);
													
void parseTextBlock_stringLists(char* textBlock,char*** matrix,char*** strlistMatrix,uint listPosition,char*** tokenlist,const char* delimiters,
                          uint maxcolcount,uint linecount,bool tolowercase,int idx_col=-1,bool fileidxcol_startswithzero=false,uint startingMtrxLineIdx = 0,bool mthread = false);

void parseTokenList_float(float** matrix,char*** tokenlist,uint colcount,uint linecount,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx);
void parseTokenList_strings(char*** matrix,char*** tokenlist,uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx);
void parseTokenList_singleString(char** matrix,char*** tokenlist,uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx);

void parseTokenList_stringLists(char*** matrix,char*** strlistMatrix,char*** tokenlist,uint colcount,uint linecount,
								uint listPosition,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx);

#endif
