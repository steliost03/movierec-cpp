#ifndef HEADER_IMPORT_ALL
#define HEADER_IMPORT_ALL

#include <iostream>
#include <string>
#include <ctime>

#include "../../import/csvparsing/csvparsing.h"
#include "../../import/csvparsing/stringutils/stringUtils.h"
#include "../../import/csvparsing/multithread/csvparsing_parallel.h"

#include "../../import/mem_management/checkedAllocationFunctions.h"
#include "../../import/mem_management/checkedReallocationFunctions.h"
#include "../../import/mem_management/cleanupFunctions.h"

#include "../../import/arrayutils/arrayCopyFunctions.h"
#include "../../import/arrayutils/searchArrayFunctions.h"

#include "../../import/arrayutils_par/parallelSearchArrayFunctions.h"

#include "../../import/multithreadcommons/multithreadcommons.h"

//#include "../../import/boostutils/boostUtils.h"

#include "../../import/timeutils/timeUtils.h"

#include "../../application/configuration/configuration.h"

#include "../../main/movieRecommenderUtils.h"

struct MatrixSizesWrapper {
	
        size_t userEntriesMatrixRowsize;
        size_t moviesMatrixRowsize;
        size_t tagsMatrixRowsize;
        size_t genreEntriesMatrixRowsize;
	
};

MatrixSizesWrapper loadFromFiles(Configuration*,std::string,std::string,std::string,std::string,
                                 float***,char****,char***,char****,ElapsedTimes&,bool);

void cleanup(Configuration*,MatrixSizesWrapper,float***,char****,char***,char****);

#endif
