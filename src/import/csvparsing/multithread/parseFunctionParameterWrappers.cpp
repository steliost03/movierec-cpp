#include "csvparsing_parallel.h"
using namespace std;

/*
Functions that use the parameter wrappers for calling the parseTextBlock functions.
Those parameters wrappers are used to overcome the arglimit limitation of boost::thread.
*/

	
void ParseTextBlockParameterWrapper::populateForFloatVersion(char* textBlock,float** matrix,char*** tokenlist,const char* delimiters,uint colcount,uint linecount,
						     int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx){
									 
	this->textBlock = textBlock;
	this->floatmat = matrix;
	this->tokenlist = tokenlist;
	this->delimiters = delimiters;
	this->colcount = colcount;
	this->linecount = linecount;
	this->idx_col = idx_col;
	this->fileidxcol_startswithzero = fileidxcol_startswithzero;
	this->startingMtrxLineIdx = startingMtrxLineIdx;
	this->mthread = true;
		
}
	
	
void ParseTextBlockParameterWrapper::populateForStringsVersion(char* textBlock,char*** matrix,char*** tokenlist,const char* delimiters,
							   uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx){
									   
	this->textBlock = textBlock;
	this->stringsmat = matrix;
	this->tokenlist = tokenlist;
	this->delimiters = delimiters;
	this->colcount = colcount;
	this->linecount = linecount;
	this->tolowercase = tolowercase;
	this->idx_col = idx_col;
	this->fileidxcol_startswithzero = fileidxcol_startswithzero;
	this->startingMtrxLineIdx = startingMtrxLineIdx;
	this->mthread = true;
		
}
	
void ParseTextBlockParameterWrapper::populateForSingleStringVersion(char* textBlock,char** matrix,char*** tokenlist,const char* delimiters,
									uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx){
											
		
	this->textBlock = textBlock;
	this->singlestringmat = matrix;
	this->tokenlist = tokenlist;
	this->delimiters = delimiters;
	this->colcount = colcount;
	this->linecount = linecount;
	this->tolowercase = tolowercase;
	this->idx_col = idx_col;
	this->fileidxcol_startswithzero = fileidxcol_startswithzero;
	this->startingMtrxLineIdx = startingMtrxLineIdx;
	this->mthread = true;
}
	
void ParseTextBlockParameterWrapper::populateForStringListsVersion(char* textBlock,char*** matrix,char*** strlistMatrix,uint listPosition,char*** tokenlist,
								   const char* delimiters,uint maxcolcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,
								   uint startingMtrxLineIdx){
										 
		
	this->textBlock = textBlock;
	this->strlistmat_regular = matrix;
	this->strlistmat = strlistMatrix;
	this->listPosition = listPosition;
	this->tokenlist = tokenlist;
	this->delimiters = delimiters;
	this->colcount = maxcolcount;
	this->linecount = linecount;
	this->tolowercase = tolowercase;
	this->idx_col = idx_col;
	this->fileidxcol_startswithzero = fileidxcol_startswithzero;
	this->startingMtrxLineIdx = startingMtrxLineIdx;
	this->mthread = true;
}
	
void parseTextBlock_float_Wrapper(ParseTextBlockParameterWrapper params){
	
	parseTextBlock_float(params.textBlock,params.floatmat,params.tokenlist,params.delimiters,params.colcount,params.linecount,params.idx_col,
						 params.fileidxcol_startswithzero,params.startingMtrxLineIdx,params.mthread);
	
	}

void parseTextBlock_strings_Wrapper(ParseTextBlockParameterWrapper params){
	
	parseTextBlock_strings(params.textBlock,params.stringsmat,params.tokenlist,params.delimiters,params.colcount,params.linecount,
						  params.tolowercase,params.idx_col,params.fileidxcol_startswithzero,params.startingMtrxLineIdx,params.mthread);
	
	}

void parseTextBlock_singleString_Wrapper(ParseTextBlockParameterWrapper params){
		
	parseTextBlock_singleString(params.textBlock,params.singlestringmat,params.tokenlist,params.delimiters,
								params.colcount,params.linecount,params.tolowercase,params.idx_col,params.fileidxcol_startswithzero,
								params.startingMtrxLineIdx,params.mthread);
	
	}

void parseTextBlock_stringLists_Wrapper(ParseTextBlockParameterWrapper params){
	
	parseTextBlock_stringLists(params.textBlock,params.strlistmat_regular,params.strlistmat,params.listPosition,params.tokenlist,
							   params.delimiters,params.colcount,params.linecount,params.tolowercase,params.idx_col,params.fileidxcol_startswithzero,
							   params.startingMtrxLineIdx,params.mthread);
	}



