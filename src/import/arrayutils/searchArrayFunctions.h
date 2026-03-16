#ifndef SEARCHARRAY_H
#define SEARCHARRAY_H

#include <cstring>
typedef unsigned int uint;
typedef unsigned long ulong;

/*
 *Difference between local and global search:
 *local search: assumes that the key values in the selected column exist continuously in a range of local rows.
 *global search: does not make that assumption and searches the entirety of the array.
*/

//real numbers

void indexRangeLocalSearchArrayByColKeyRealNum(float** inputarray,float key,uint key_pos,uint rowcount,long result[2]);

void indexRangeLocalSearchArrayByMultipleColKeysRealNum(float** inputarray,float* keys,uint* keyPositions,uint keyCount,uint rowcount,long result[2]);

long getIndexGlobalSearchArrayByMultipleColKeysRealNum(float** inputarray,float* keys,uint* keyPositions,uint keyCount,uint rowcount);


//text

void indexRangeLocalSearchArrayByMultipleColKeysText(char*** inputarray,char** keys,uint* keyPositions,uint keyCount,uint rowcount,long result[2]);

long getIndexGlobalSearchArrayByColKeyText(char*** inputarray,char** input1D,char* key,uint key_pos,uint rowcount);

long getIndexGlobalSearchArrayByMultipleColKeysText(char*** inputarray,char** keys,uint* keyPositions,uint keyCount,uint rowcount);

#endif

