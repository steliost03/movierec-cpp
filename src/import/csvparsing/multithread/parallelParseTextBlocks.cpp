#include "csvparsing_parallel.h"
using namespace std;
typedef unsigned int uint;

void splitAndParallelParseTextBlock_float(uint chosenThreadcount,char* textBlock,ulong* newlinePositions,float** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,int idx_col,bool fileidxcol_startswithzero){
	
    uint threadcount = initThreadCount(chosenThreadcount,thread::hardware_concurrency());
	//threadcount === blockcount
	char** textBlocks;
	ulong* textBlock_startingLineIdxs;
	uint* textBlock_linecounts; 
	splitTotalTextToBlocks(&textBlock,newlinePositions,threadcount,linecount,&textBlocks,&textBlock_startingLineIdxs,&textBlock_linecounts);
    ParseTextBlockParameterWrapper params;
    vector<thread> threads;
    for(size_t i=0;i<threadcount;i++){
        params.populateForFloatVersion(textBlocks[i],matrix,tokenlist,delimiters,colcount,textBlock_linecounts[i],idx_col,fileidxcol_startswithzero,textBlock_startingLineIdxs[i]);
        threads.push_back(thread(parseTextBlock_float_Wrapper,params));
	}
	
	waitForAllThreads(threads);
	threads.clear();
	
	//cleanup
	arrcleanup(&textBlock_startingLineIdxs);
	arrcleanup(&textBlock_linecounts);
	arrcleanup_2D(&textBlocks,threadcount);
}

void splitAndParallelParseTextBlock_strings(uint chosenThreadcount,char* textBlock,ulong* newlinePositions,char*** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero){
							  
	
    uint threadcount = initThreadCount(chosenThreadcount,thread::hardware_concurrency());
	//threadcount === blockcount
	char** textBlocks;
	ulong* textBlock_startingLineIdxs;
	uint* textBlock_linecounts; 	
	splitTotalTextToBlocks(&textBlock,newlinePositions,threadcount,linecount,&textBlocks,&textBlock_startingLineIdxs,&textBlock_linecounts);
	ParseTextBlockParameterWrapper params;
    vector<thread> threads;
    for(size_t i=0;i<threadcount;i++){
		params.populateForStringsVersion(textBlocks[i],matrix,tokenlist,delimiters,colcount,textBlock_linecounts[i],tolowercase,idx_col,fileidxcol_startswithzero,textBlock_startingLineIdxs[i]);
        threads.push_back(thread(parseTextBlock_strings_Wrapper,params));
	}
	waitForAllThreads(threads);
	threads.clear();
	
	//cleanup
	arrcleanup(&textBlock_startingLineIdxs);
	arrcleanup(&textBlock_linecounts);
	arrcleanup_2D(&textBlocks,threadcount);
	
}

void splitAndParallelParseTextBlock_singleString(uint chosenThreadcount,char* textBlock,ulong* newlinePositions,char** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero){
							  
    uint threadcount = initThreadCount(chosenThreadcount,thread::hardware_concurrency());
	//threadcount === blockcount
	char** textBlocks;
	ulong* textBlock_startingLineIdxs;
	uint* textBlock_linecounts; 	
	splitTotalTextToBlocks(&textBlock,newlinePositions,threadcount,linecount,&textBlocks,&textBlock_startingLineIdxs,&textBlock_linecounts);
	ParseTextBlockParameterWrapper params;
    vector<thread> threads;
    for(size_t i=0;i<threadcount;i++){
		params.populateForSingleStringVersion(textBlocks[i],matrix,tokenlist,delimiters,colcount,textBlock_linecounts[i],tolowercase,idx_col,fileidxcol_startswithzero,textBlock_startingLineIdxs[i]);
        threads.push_back(thread(parseTextBlock_singleString_Wrapper,params));
	}
	
	waitForAllThreads(threads);
	threads.clear();
	
	//cleanup
	arrcleanup(&textBlock_startingLineIdxs);
	arrcleanup(&textBlock_linecounts);
	arrcleanup_2D(&textBlocks,threadcount);
	
}

void splitAndParallelParseTextBlock_stringLists(uint chosenThreadcount,char* textBlock,ulong* newlinePositions,char*** matrix,char*** strlistMatrix,uint listPosition,char*** tokenlist,const char* delimiters,
                          uint maxcolcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero){
							  
    uint threadcount = initThreadCount(chosenThreadcount,thread::hardware_concurrency());
	//threadcount === blockcount
	char** textBlocks;
	ulong* textBlock_startingLineIdxs;
	uint* textBlock_linecounts; 	
	splitTotalTextToBlocks(&textBlock,newlinePositions,threadcount,linecount,&textBlocks,&textBlock_startingLineIdxs,&textBlock_linecounts);
	ParseTextBlockParameterWrapper params;
    vector<thread> threads;
    for(size_t i=0;i<threadcount;i++){
		params.populateForStringListsVersion(textBlocks[i],matrix,strlistMatrix,listPosition,tokenlist,delimiters,maxcolcount,textBlock_linecounts[i],tolowercase,idx_col,fileidxcol_startswithzero,textBlock_startingLineIdxs[i]);
        threads.push_back(thread(parseTextBlock_stringLists_Wrapper,params));
	}
	
	waitForAllThreads(threads);
	threads.clear();
	
	//cleanup
	arrcleanup(&textBlock_startingLineIdxs);
	arrcleanup(&textBlock_linecounts);
	arrcleanup_2D(&textBlocks,threadcount);
	
}
