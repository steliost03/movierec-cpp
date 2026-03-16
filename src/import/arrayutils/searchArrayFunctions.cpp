#include "searchArrayFunctions.h"

/*
 *Difference between local and global search:
 *local search: assumes that the key values in the selected column exist continuously in a range of local rows.
 *global search: does not make that assumption and searches the entirety of the array.
*/

bool searchKeyComboRealNum(float** array,uint row,float* keys,uint* keyPositions,uint keyCount) {
	
	
	bool key_combo_found = true;
    for(size_t k=0;k<keyCount;k++){
		
		if(array[row][keyPositions[k]-1] != keys[k]){
			key_combo_found = false;
		}
	}
	return key_combo_found;
}


bool searchKeyComboText(char*** array,uint row,char** keys,uint* keyPositions,uint keyCount){
	
	bool key_combo_found = true;
    for(size_t k=0;k<keyCount;k++){
		
		if(strcmp(array[row][keyPositions[k]-1],keys[k])){
			key_combo_found = false;
		}
	}
	return key_combo_found;
	
}

void indexRangeLocalSearchArrayByColKeyRealNum(float** inputarray,float key,uint key_pos,uint rowcount,long result[2]){
	
	result[0] = -1;
	result[1] = -1;
	
	key_pos--; //so that it represents an array index.
	
	uint rowcounter;
	
    for(size_t r=0;r<rowcount;r++){
		
		if(inputarray[r][key_pos] == key){
			result[0] = r;
			rowcounter = r + 1;
			while(rowcounter < rowcount && inputarray[rowcounter][key_pos] == key){
				rowcounter++;
			}
			result[1] = rowcounter - 1;
			return;
		}
	}
}

void indexRangeLocalSearchArrayByMultipleColKeysRealNum(float** inputarray,float* keys,uint* keyPositions,uint keyCount,uint rowcount,long result[2]){
	
	
	result[0] = -1;
	result[1] = -1;
	
	uint rowcounter;
	
	bool key_combo_found;
    for(size_t r=0;r<rowcount;r++){
		
		key_combo_found = searchKeyComboRealNum(inputarray,r,keys,keyPositions,keyCount);
		
		if(key_combo_found){
			
			result[0] = r;
			rowcounter = r+1;
			while(rowcounter < rowcount && searchKeyComboRealNum(inputarray,rowcounter,keys,keyPositions,keyCount)){
				rowcounter++;
			}
			result[1] = rowcounter - 1;
			return;
		}
		
	}
}


long getIndexGlobalSearchArrayByMultipleColKeysRealNum(float** inputarray,float* keys,uint* keyPositions,uint keyCount,uint rowcount) {
	
	bool key_combo_found;
	for(long r=0;r<rowcount;r++){
		
		key_combo_found = searchKeyComboRealNum(inputarray,r,keys,keyPositions,keyCount);
		
		if(key_combo_found)
			return r;
		
		
	}
	return -1;
}

//TODO:implement
void localSearchArrayByColKeyRealNum(){}

void localSearchArrayByMultipleColKeysRealNum(){}


void globalSearchArrayByColKeyRealNum(){}

void globalSearchArrayByMultipleColKeysRealNum(){}
//

void indexRangeLocalSearchArrayByMultipleColKeysText(char*** inputarray,char** keys,uint* keyPositions,uint keyCount,uint rowcount,long result[2]){
	
	result[0] = -1;
	result[1] = -1;
	
	uint rowcounter;
	
	bool key_combo_found;
    for(size_t r=0;r<rowcount;r++){
		
		key_combo_found = searchKeyComboText(inputarray,r,keys,keyPositions,keyCount);
		
		if(key_combo_found){
			
			result[0] = r;
			rowcounter = r+1;
			while(rowcounter < rowcount && searchKeyComboText(inputarray,rowcounter,keys,keyPositions,keyCount)){
				rowcounter++;
			}
			result[1] = rowcounter - 1;
			return;
		}
		
	}
		
}


long getIndexGlobalSearchArrayByColKeyText(char*** inputarray,char** input1D,char* key,uint key_pos,uint rowcount) {
	
	//1D input overrides regular input.
	bool onedimensional = !(input1D == NULL);
	
	key_pos--; //so that it represents an array index.
	
	if(onedimensional){
        for(size_t i=0;i<rowcount;i++){
			
			if(!strcmp(input1D[i],key))
				return i;
			
		}
	}
	else{
    for(size_t r=0;r<rowcount;r++){
		
		if(!strcmp(inputarray[r][key_pos],key))
			return r;
			
		}
	}
	
	return -1;
}


long getIndexGlobalSearchArrayByMultipleColKeysText(char*** inputarray,char** keys,uint* keyPositions,uint keyCount,uint rowcount){
	
	bool key_combo_found;
    for(size_t r=0;r<rowcount;r++){
		
		key_combo_found = searchKeyComboText(inputarray,r,keys,keyPositions,keyCount);
		
		if(key_combo_found)
			return r;
		
	}
	
	return -1;
}




