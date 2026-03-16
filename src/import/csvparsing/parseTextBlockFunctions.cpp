#include "csvparsing.h"

//wrapper functions for parsing a text block and storing the data into a matrix.

//idx_col -> the column which should be considered as the index number for the first dimension of the resulting matrix.
//           If no column is used as index, then a negative value should be passed for idx_col


//tokenlist is expected to be not null, and correctly allocated.
//it is also expected that it will be deallocated outside these functions.

//float matrix type
void parseTextBlock_float(char* textBlock,float** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx,bool mthread){


    splitTextBlockToTokens(textBlock,delimiters,tokenlist,startingMtrxLineIdx,mthread);
    parseTokenList_float(matrix,tokenlist,colcount,linecount,idx_col,fileidxcol_startswithzero,startingMtrxLineIdx);

}

//string matrix type (multiple strings in each row)
void parseTextBlock_strings(char* textBlock,char*** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx,bool mthread){

    splitTextBlockToTokens(textBlock,delimiters,tokenlist,startingMtrxLineIdx,mthread);
    parseTokenList_strings(matrix,tokenlist,colcount,linecount,tolowercase,idx_col,fileidxcol_startswithzero,startingMtrxLineIdx);

  }


//string matrix type (single string in each row)
void parseTextBlock_singleString(char* textBlock,char** matrix,char*** tokenlist,const char* delimiters,
                          uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx,bool mthread){

    splitTextBlockToTokens(textBlock,delimiters,tokenlist,startingMtrxLineIdx,mthread);
    parseTokenList_singleString(matrix,tokenlist,colcount,linecount,tolowercase,idx_col,fileidxcol_startswithzero,startingMtrxLineIdx);


  }
  
//string matrix type , with a subset of the columns being lists of strings that will be stored in a different matrix (strlistMatrix).
//the lists of strings must be in the last (rightmost) positions.
void parseTextBlock_stringLists(char* textBlock,char*** matrix,char*** strlistMatrix,uint listPosition,char*** tokenlist,const char* delimiters,
                          uint maxcolcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx,bool mthread){

	
    splitTextBlockToTokens(textBlock,delimiters,tokenlist,startingMtrxLineIdx,mthread);  
	parseTokenList_stringLists(matrix,strlistMatrix,tokenlist,maxcolcount,linecount,listPosition,tolowercase,idx_col,fileidxcol_startswithzero,startingMtrxLineIdx);
							  							  
}
