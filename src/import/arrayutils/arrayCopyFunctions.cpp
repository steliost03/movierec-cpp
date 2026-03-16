#include "arrayCopyFunctions.h"

void arrcpy_error(const char* message){

   	fprintf(stderr,"ERROR : ");
	fprintf(stderr,"%s", message);
	fprintf(stderr,"\nThe program will now terminate.");
	fprintf(stderr,"\n");
	fprintf(stderr,"Press Enter to continue.");
	getchar();
	exit(0);

}

//TODO : some explanation

void partialArrayCopyRealNum(float** destinationArray,float** sourceArray,long src_rowrange[2],long dest_rowrange[2],long src_colrange[2],long dest_colrange[2]){
	
	if( (src_colrange[1] - src_colrange[0]) != (dest_colrange[1] - dest_colrange[0]))
		arrcpy_error("Encountered unequal column ranges while performing array column copy.");
		
	if(src_colrange[0] < 0 || src_colrange[1] < 0 || dest_colrange[0] < 0 || dest_colrange[1] < 0)
		arrcpy_error("Encountered invalid value in column range, while performing array column copy.");
		
	if( (src_rowrange[1] - src_rowrange[0]) != (dest_rowrange[1] - dest_rowrange[0]))
		arrcpy_error("Encountered unequal row ranges while performing array column copy.");
		
	if(src_rowrange[0] < 0 || src_rowrange[1] < 0 || dest_rowrange[0] < 0 || dest_rowrange[1] < 0)
		arrcpy_error("Encountered invalid value in row range, while performing array column copy.");
	
	uint rowrange = src_rowrange[1] - src_rowrange[0];
	uint colrange = src_colrange[1] - src_colrange[0];
	
	size_t sizenum = colrange*sizeof(float);
	
	for(uint r=0;r<rowrange;r++){
		
		memcpy(destinationArray[dest_rowrange[0]+r]+dest_colrange[0],sourceArray[src_rowrange[0]+r]+src_colrange[0],sizenum);
		
	}
		
	
}

//TODO:unimplemented.
void partialArrayCopyRealNum_flatOutput(float* destinationArray,float** sourceArray,long src_rowrange[2],long src_colrange[2],long dest_startcol){

    //silence warnings for unused parameters.
    (void)destinationArray;
    (void)sourceArray;
    (void)src_rowrange;
    (void)src_colrange;
    (void)dest_startcol;
    //
	
//	if(src_colrange[0] < 0 || src_colrange[1] < 0 || dest_startcol < 0)
//		arrcpy_error("Encountered invalid value in column range, while performing array column copy.");
		
	
//	uint rowrange = src_rowrange[1] - src_rowrange[0];
//	uint colrange = src_colrange[1] - src_colrange[0];
	
//	size_t sizenum = colrange*sizeof(float);
	
//	for(uint r=0;r<rowrange;r++){
		
		
		
//	}
	
}


void partialArrayCopyRealNum_flatInput(float** destinationArray,float* sourceArray,long dest_row,long dest_startcol,uint src_size){
	
	dest_row--; 
	dest_startcol--; //so that they represent array indexes.
	
	for(uint i=0;i<src_size;i++){
		
		destinationArray[dest_row][dest_startcol+i] = sourceArray[i];
		
	}
	
	
}

void partialArrayCopyText_flatOutput(char** destinationArray,char*** sourceArray,long src_rowrange[2],long target_srccol){
	
	target_srccol--; //so that it represents an array index.
	
	if(src_rowrange[0] < 0 || src_rowrange[1] < 0)
		arrcpy_error("Encountered invalid value in row range, while performing array column copy.");
		
	
	uint rowrange = src_rowrange[1] - src_rowrange[0];
	
	for(uint i=0;i<=rowrange;i++) {
		
		strcpy(destinationArray[i],sourceArray[src_rowrange[0]+i][target_srccol]);
		
	}	
}




