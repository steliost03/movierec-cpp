#include "csvparsing_parallel.h"
#include "../stringutils/stringUtils.h"

/*
Splits a given piece of text into blocks. If possible, those blocks will be returned equal in size.
If not possible, all blocks will be equal with each other, except the last one.
The last block in that case will be slightly larger or smaller that the others.

For memory efficiency, text blocks are deallocated from their previous array (total_text)
at the same time as they are allocated to the new array (textBlocks). (TODO)

input -> total_text,newlinePositions,blockcount,total_linecount
output -> textBlocks, allocation prior to function call not required.

TODO: explain bool clean
*/
void splitTotalTextToBlocks(char** total_text,ulong* newlinePositions,uint blockcount,uint total_linecount,char*** textBlocks,ulong** textBlock_startingLineIdxs,uint** textBlock_linecounts,bool cleanBlocks) {

  uint active_blockcount;
  if(blockcount > total_linecount){
    active_blockcount = total_linecount;
  }
  else{
    active_blockcount = blockcount;
  }
    
    uint linecount_per_block;
    uint linecount_of_last_block;

    ulong curr_block_charcount;
    ulong curr_newlinepos_idx;
    ulong prev_newlinepos_idx;
    ulong chars_copied = 0;
    uint curr_block_idx = 0;

    //integer division : decimal value will be dropped.
    linecount_per_block = total_linecount / blockcount;

    //the last block will be assigned what's remaining. That amount could be or not be
    //equal to the amount of lines the other blocks are assigned.
    linecount_of_last_block = total_linecount - (blockcount-1)*linecount_per_block;

    checkedAllocation<char*>(textBlocks,blockcount); 
    //checkedAllocation<char*>((*textBlocks),blockcount);
    checkedAllocation<ulong>(textBlock_startingLineIdxs,blockcount);
    checkedAllocation<uint>(textBlock_linecounts,blockcount);
    
    //first block
    (*textBlock_startingLineIdxs)[0] = 0;
    (*textBlock_linecounts)[0] = linecount_per_block;
    curr_newlinepos_idx = linecount_per_block - 1; //transform amount to position.
    prev_newlinepos_idx = curr_newlinepos_idx;
    curr_block_charcount = newlinePositions[curr_newlinepos_idx] + 1; //transform position to amount.
    checkedAllocation<char>(&((*textBlocks)[curr_block_idx]),curr_block_charcount+1); //+1 char for consistency with serial method
    memcpy((*textBlocks)[curr_block_idx],(*total_text),curr_block_charcount*sizeof(char));
    chars_copied = chars_copied + curr_block_charcount;
    //add terminating character (in place of last newline).
      (*textBlocks)[curr_block_idx][curr_block_charcount-1] = '\0';
    //put terminating character in the extra char
	  (*textBlocks)[curr_block_idx][curr_block_charcount] = '\0';
    if(cleanBlocks)
		strip2(&((*textBlocks)[curr_block_idx]),'~');
    //TODO: arrcleanup while transferring.

    //remaining blocks
    for(uint i=1;i<blockcount-1;i++){
      curr_block_idx++;
      (*textBlock_startingLineIdxs)[i] = (*textBlock_startingLineIdxs)[i-1] + linecount_per_block;
      (*textBlock_linecounts)[i] = linecount_per_block;
      curr_newlinepos_idx = prev_newlinepos_idx + linecount_per_block;
      curr_block_charcount = newlinePositions[curr_newlinepos_idx] - newlinePositions[prev_newlinepos_idx];
      checkedAllocation<char>(&((*textBlocks)[curr_block_idx]),curr_block_charcount+1); //+1 char for consistency with serial method
      memcpy((*textBlocks)[curr_block_idx],(*total_text)+chars_copied,curr_block_charcount*sizeof(char));
      chars_copied = chars_copied + curr_block_charcount;
      prev_newlinepos_idx = curr_newlinepos_idx;
      //add terminating character (in place of last newline).
      (*textBlocks)[curr_block_idx][curr_block_charcount-1] = '\0';
      //put terminating character in the extra char
	  (*textBlocks)[curr_block_idx][curr_block_charcount] = '\0';
      if(cleanBlocks)
		strip2(&((*textBlocks)[curr_block_idx]),'~');
    }

    //last block
    if(blockcount - 1 > 0){
		curr_block_idx++;
		(*textBlock_startingLineIdxs)[blockcount-1] = (*textBlock_startingLineIdxs)[blockcount-2] + linecount_per_block;
		(*textBlock_linecounts)[blockcount-1] = linecount_of_last_block;
		curr_newlinepos_idx = prev_newlinepos_idx + linecount_of_last_block;
		curr_block_charcount = newlinePositions[curr_newlinepos_idx] - newlinePositions[prev_newlinepos_idx];
		checkedAllocation<char>(&((*textBlocks)[curr_block_idx]),curr_block_charcount+1); //+1 char for consistency with serial method
		memcpy((*textBlocks)[curr_block_idx],(*total_text)+chars_copied,curr_block_charcount*sizeof(char));
		//add terminating character (in place of last newline).
		  (*textBlocks)[curr_block_idx][curr_block_charcount-1] = '\0';
	    //put terminating character in the extra char
		  (*textBlocks)[curr_block_idx][curr_block_charcount] = '\0';
		if(cleanBlocks)
			strip2(&((*textBlocks)[curr_block_idx]),'~');
	}
}
