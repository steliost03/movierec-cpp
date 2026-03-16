#ifndef CSVHEADER_PARALLEL_H
#define CSVHEADER_PARALLEL_H

#include "../csvparsing.h"

#include "../../mem_management/checkedAllocationFunctions.h"
#include "../../mem_management/cleanupFunctions.h"

#include "../../multithreadcommons/multithreadcommons.h"

#include <vector>
#include <thread>

class ParseTextBlockParameterWrapper{
	
	public:
	char* textBlock;
	float** floatmat;
	char** singlestringmat;
	char*** stringsmat;
	char*** tokenlist;
	const char* delimiters;
	uint colcount;
	uint linecount;
	int idx_col;
	bool fileidxcol_startswithzero;
	bool tolowercase;
	uint startingMtrxLineIdx;
	char*** strlistmat_regular;
	char*** strlistmat;
	uint listPosition;
	bool mthread;
	
	void populateForFloatVersion(char* textBlock,float** matrix,char*** tokenlist,const char* delimiters,uint colcount,uint linecount,
								 int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx);
								 
	void populateForStringsVersion(char* textBlock,char*** matrix,char*** tokenlist,const char* delimiters,
								   uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx);
								   
	void populateForSingleStringVersion(char* textBlock,char** matrix,char*** tokenlist,const char* delimiters,
										uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx);
										
	void populateForStringListsVersion(char* textBlock,char*** matrix,char*** strlistMatrix,uint listPosition,char*** tokenlist,
									  const char* delimiters,uint maxcolcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,
									 uint startingMtrxLineIdx);
	
	
};

void splitTotalTextToBlocks(char** total_text,ulong* newlinePositions,uint blockcount,uint total_linecount,char*** textBlocks,ulong** textBlock_startingLineIdxs,uint** textBlock_linecounts,bool cleanBlock=false);

void splitAndParallelParseTextBlock_float(unsigned int chosenThreadcount,char* textBlock,ulong* newlinePositions,float** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,int idx_col,bool fileidxcol_startswithzero);

void splitAndParallelParseTextBlock_strings(unsigned int chosenThreadcount,char* textBlock,ulong* newlinePositions,char*** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero);

void splitAndParallelParseTextBlock_singleString(unsigned int chosenThreadcount,char* textBlock,ulong* newlinePositions,char** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero);

void splitAndParallelParseTextBlock_stringLists(unsigned int chosenThreadcount,char* textBlock,ulong* newlinePositions,char*** matrix,char*** strlistMatrix,uint listPosition,char*** tokenlist,const char* delimiters,
                          uint maxcolcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero);
                          
                          
void parseTextBlock_float_Wrapper(ParseTextBlockParameterWrapper params);

void parseTextBlock_strings_Wrapper(ParseTextBlockParameterWrapper params);

void parseTextBlock_singleString_Wrapper(ParseTextBlockParameterWrapper params);

void parseTextBlock_stringLists_Wrapper(ParseTextBlockParameterWrapper params);

#endif
