#ifndef MTRXCPY_H
#define MTRXCPY_H

typedef unsigned int uint;
typedef unsigned long ulong;

#include <cstdio>
#include <cstdlib>
#include <cstring>

void partialArrayCopyRealNum(float** destinationArray,float** sourceArray,long src_rowrange[2],long dest_rowrange[2],
									long src_colrange[2],long dest_colrange[2]);
									
void partialArrayCopyRealNum_flatOutput(float* destinationArray,float** sourceArray,long src_rowrange[2],long src_colrange[2],long dest_startcol);

void partialArrayCopyText_flatOutput(char** flatoutput,char*** sourceArray,long src_rowrange[2],long target_srccol);

void partialArrayCopyRealNum_flatInput(float** destinationArray,float* sourceArray,long dest_row,long dest_startcol,uint src_size);


#endif


