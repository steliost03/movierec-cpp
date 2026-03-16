#include "csvparsing.h"
#include "cstdex/cstdex.h"

//Splits each line of a given piece of text block, to tokens.
//output : tokenlist -> an array of all the tokens by line. Assumes that it is allocated properly and not null.
void splitTextBlockToTokens(char* textBlock,const char* delimiters,char*** tokenlist,uint startingLineIdx,bool mthread){

  if(tokenlist == NULL){
    throwErrorAndTerminate("Encountered NULL input while parsing.");
  }

  //+1 because strlen doesnt count the terminating character.
  const ulong end_idx = strlen(textBlock)+1;
  uint tokenlist_idx = startingLineIdx;
  ulong idx = 0;
  char line[MAXLINELEN];

  idx = sgets(textBlock,line,idx);
  while(idx <= end_idx){
	if(mthread){
		splitLineToTokens_t(line,tokenlist[tokenlist_idx],delimiters);
	}
	else{
		splitLineToTokens(line,tokenlist[tokenlist_idx],delimiters);
	}
    tokenlist_idx++;
    idx = sgets(textBlock,line,idx);
  }
}
