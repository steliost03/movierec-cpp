#include "csvparsing.h"
#include "stringutils/stringUtils.h"

//For a given tokenlist, transforms each token to the appropriate type and stores it in output matrix.
//idx_col -> the column which should be considered as the index number for the first dimension of the resulting matrix.
//           If no column is used as index, then a negative value should be passed for idx_col.


//assumptions : tokenlist is correctly allocated prior to these calls. For ..Lists functions, tokenlist must also have all elements
//				initialized to zero.

//float matrix type
void parseTokenList_float(float** matrix,char*** tokenlist,uint colcount,uint linecount,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx){

  if(tokenlist == NULL)
	throwErrorAndTerminate("Encountered NULL input while parsing");
	
  bool valid_idx_col = (idx_col > 0 && idx_col <= colcount);
  idx_col--; //reducing idx_col by 1, so that it represents an array index.
  float tokenvalue;
  uint mtrx_line_idx;
  uint mtrx_col_idx;

  for(uint l=0;l<linecount;l++){
	 
    if(valid_idx_col){
      int sscanf_success = sscanf(tokenlist[l+startingMtrxLineIdx][idx_col],"%u",&mtrx_line_idx);
      if(sscanf_success == EOF)
        throwErrorAndTerminate("Could not parse index number in file.");
      if(!fileidxcol_startswithzero)
        mtrx_line_idx--;
    }
    else{
      	mtrx_line_idx = l + startingMtrxLineIdx;
	}

    for(uint c=0;c<colcount;c++){

      if(c == idx_col)
        continue;


      if(valid_idx_col && c > idx_col)
        mtrx_col_idx = c - 1;
      else
        mtrx_col_idx = c;

	  strip(&(tokenlist[l+startingMtrxLineIdx][c]),'~'); //TODO: this is a temporary solution.
      sscanf(tokenlist[l+startingMtrxLineIdx][c],"%f",&tokenvalue);
      matrix[mtrx_line_idx][mtrx_col_idx] = tokenvalue;
    }
  }


}

//string matrix type (multiple strings in each row)
void parseTokenList_strings(char*** matrix,char*** tokenlist,uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx){

  if(tokenlist == NULL)
	throwErrorAndTerminate("Encountered NULL input while parsing");
	
  bool valid_idx_col = (idx_col > 0 && idx_col <= colcount);
  idx_col--; //reducing idx_col by 1, so that it represents an array index.
  //char* tokenvalue;
  uint mtrx_line_idx;
  uint mtrx_col_idx;

  for(uint l=0;l<linecount;l++){

    if(valid_idx_col){
      int sscanf_success = sscanf(tokenlist[l+startingMtrxLineIdx][idx_col],"%u",&mtrx_line_idx);
      if(sscanf_success == EOF)
        throwErrorAndTerminate("Could not parse index number in file.");
      if(!fileidxcol_startswithzero)
        mtrx_line_idx--;
    }
    else{
      	mtrx_line_idx = l + startingMtrxLineIdx;
	}

    for(uint c=0;c<colcount;c++){

      if(c == idx_col)
        continue;

      if(valid_idx_col && c > idx_col)
        mtrx_col_idx = c - 1;
      else
        mtrx_col_idx = c;

      strcpy(matrix[mtrx_line_idx][mtrx_col_idx],tokenlist[l+startingMtrxLineIdx][c]);
      strip(&(matrix[mtrx_line_idx][mtrx_col_idx]),'~');
      remove_extra_whitespaces(matrix[mtrx_line_idx][mtrx_col_idx]);
      if(tolowercase)
		alltolower(matrix[mtrx_line_idx][mtrx_col_idx]);
    }
  }
}


//string matrix type (single string in each row)
void parseTokenList_singleString(char** matrix,char*** tokenlist,uint colcount,uint linecount,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx){

  if(tokenlist == NULL)
	throwErrorAndTerminate("Encountered NULL input while parsing");
	
  bool valid_idx_col = (idx_col > 0 && idx_col <= colcount);
  idx_col--; //reducing idx_col by 1, so that it represents an array index.
  //char* tokenvalue;
  uint mtrx_line_idx;

  for(uint l=0;l<linecount;l++){
    if(valid_idx_col){
      int sscanf_success = sscanf(tokenlist[l+startingMtrxLineIdx][idx_col],"%u",&mtrx_line_idx);
      if(sscanf_success == EOF)
        throwErrorAndTerminate("Could not parse index number in file.");
      if(!fileidxcol_startswithzero)
        mtrx_line_idx--;
    }
    else{
      	mtrx_line_idx = l + startingMtrxLineIdx;
	}

    for(uint c=0;c<colcount;c++){

      if(c == idx_col)
        continue;

      strcpy(matrix[mtrx_line_idx],tokenlist[l+startingMtrxLineIdx][c]);
      strip(&(matrix[mtrx_line_idx]),'~');
      remove_extra_whitespaces(matrix[mtrx_line_idx]);
      if(tolowercase)
		alltolower(matrix[mtrx_line_idx]);
    }
  }
}

//string matrix type , with a subset of the columns being lists of strings that will be stored in a different matrix (strlistMatrix).
//the subset of the columns must be in the last (rightmost) positions.
void parseTokenList_stringLists(char*** matrix,char*** strlistMatrix,char*** tokenlist,uint maxcolcount,uint linecount,
								uint listPosition,bool tolowercase,int idx_col,bool fileidxcol_startswithzero,uint startingMtrxLineIdx){
    
    if(tokenlist == NULL)
        throwErrorAndTerminate("Encountered NULL input while parsing");
    
    bool valid_idx_col = (idx_col > 0 && idx_col <= maxcolcount);
    
    if(idx_col >= int(listPosition)){
        throwErrorAndTerminate("Column element containing index number cannot be a part of a nested list.");
    }
        
    idx_col--; //reducing idx_col by 1, so that it represents an array index.
    listPosition--; //reducing listPosition by 1, so that it represents an array index.
    
    //char* tokenvalue;
    uint mtrx_line_idx;
    uint mtrx_col_idx;
    uint strlstmtrx_line_idx;
    uint strlstmtrx_col_idx;

    for(uint l=0;l<linecount;l++){

        if(valid_idx_col){
            int sscanf_success = sscanf(tokenlist[l+startingMtrxLineIdx][idx_col],"%u",&mtrx_line_idx);
            if(sscanf_success == EOF)
                throwErrorAndTerminate("Could not parse index number in file.");
            if(!fileidxcol_startswithzero)
                mtrx_line_idx--;
		}
		else{
		   mtrx_line_idx = l + startingMtrxLineIdx;
		}
    
    strlstmtrx_col_idx = 0;
    
    strlstmtrx_line_idx = l + startingMtrxLineIdx;
    for(uint c=0;c<maxcolcount;c++){
        
        
        if(c == idx_col)
            continue;

        if(valid_idx_col && c > idx_col)
          mtrx_col_idx = c - 1;
        else
          mtrx_col_idx = c;
          
        if(c >= listPosition) {
			if(!strcmp(tokenlist[l+startingMtrxLineIdx][c],"")){
				break;
			}
            strcpy(strlistMatrix[strlstmtrx_line_idx][strlstmtrx_col_idx],tokenlist[l+startingMtrxLineIdx][c]);
            strip(&(strlistMatrix[strlstmtrx_line_idx][strlstmtrx_col_idx]),'~');
            remove_extra_whitespaces(strlistMatrix[strlstmtrx_line_idx][strlstmtrx_col_idx]);
            if(tolowercase)
				alltolower(strlistMatrix[strlstmtrx_line_idx][strlstmtrx_col_idx]);
            strlstmtrx_col_idx++;
        }
        else{
            strcpy(matrix[mtrx_line_idx][mtrx_col_idx],tokenlist[l+startingMtrxLineIdx][c]);
            strip(&(matrix[mtrx_line_idx][mtrx_col_idx]),'~');
            remove_extra_whitespaces(matrix[mtrx_line_idx][mtrx_col_idx]);
            if(tolowercase)
				alltolower(matrix[mtrx_line_idx][mtrx_col_idx]);
        }
    }
  }
}
