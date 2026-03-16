#ifndef MTRXCPY_PAR_H
#define MTRXCPY_PAR_H

//#include "../arrayutils/searchArrayFunctions.h"
#include "../multithreadcommons/multithreadcommons.h"
//#include "../boostutils/boostUtils.h"

long getIndexGlobalSearchArrayByMultipleColKeysRealNum_parallel(uint givenThreadcount,float** inputarray,float* keys,uint* keyPositions,
																uint keyCount,uint rowcount);

#endif



