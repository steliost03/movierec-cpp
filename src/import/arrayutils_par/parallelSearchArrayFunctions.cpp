#include "parallelSearchArrayFunctions.h"
using namespace std;

bool searchKeyComboRealNum2(float** array,uint row,float* keys,uint* keyPositions,uint keyCount) {
	
	
	bool key_combo_found = true;
	for(uint k=0;k<keyCount;k++){
		
		if(array[row][keyPositions[k]-1] != keys[k]){
			key_combo_found = false;
		}
	}
	return key_combo_found;
}

long getIndexGlobalSearchArrayByMultipleColKeysRealNum2(float** inputarray,float* keys,uint* keyPositions,uint keyCount,uint rowcount) {
	
    //uint rowcounter;
	
	bool key_combo_found;
	for(long r=0;r<rowcount;r++){
			
		key_combo_found = searchKeyComboRealNum2(inputarray,r,keys,keyPositions,keyCount);
		
		if(key_combo_found)
			return r;
	}
	
	return -1;
	
}

void getIndexGlobalSearchArrayByMultipleColKeysRealNum_parWrapper(float** inputarray,float* keys,uint* keyPositions,uint keyCount,
                                                                 uint rowBegin,uint rowEnd,vector<int>& idx_found,vector<long>& par_results,size_t threadIdx){
																	 		
	float** inputarrayCopy = inputarray + rowBegin;
	
	uint rowcount = rowEnd - rowBegin;
	
    //TODO: implement custom interruption mechanism.
	par_results[threadIdx] = getIndexGlobalSearchArrayByMultipleColKeysRealNum2(inputarrayCopy,keys,keyPositions,keyCount,rowcount);
	
    //(nov2019) changed >0 to >=0 because it would otherwise not trigger in cases where the keys were found in the first row (index zero)
    if(par_results[threadIdx] >= 0){
		par_results[threadIdx] = par_results[threadIdx] + rowBegin;
		idx_found[threadIdx] = 1;	
	}
	else{
		idx_found[threadIdx] = 0;
	}
}

long getIndexGlobalSearchArrayByMultipleColKeysRealNum_parallel(uint givenThreadcount,float** inputarray,float* keys,uint* keyPositions,
																uint keyCount,uint rowcount){
	
	long par_final_result = -1;
	
    uint chosenThreadcount = initThreadCount(givenThreadcount,thread::hardware_concurrency());

    vector<thread> threads; //the threads that will be used.
    vector<int> idx_found; //for every row block, contains m
    vector<long> par_results; //contains the results for each row block.
    vector<uint> intervLimits; //contains the index limits for each row block.
    splitInterval(chosenThreadcount,0,rowcount,intervLimits); //populates 'intervLimits' vector.
	idx_found.assign(chosenThreadcount,0);
	par_results.resize(chosenThreadcount);
	
    for(size_t i=0;i<chosenThreadcount;i++){
      threads.push_back(thread(getIndexGlobalSearchArrayByMultipleColKeysRealNum_parWrapper,inputarray,keys,keyPositions,keyCount,intervLimits[i],intervLimits[i+1],ref(idx_found),ref(par_results),i));
	}
	
    for(size_t i=0;i<chosenThreadcount;i++){
        threads[0].join();
        threads.erase(threads.begin());
        if(idx_found[i]){
            par_final_result = par_results[i];
            //interruptAllThreads(threads); //TODO: implement custom mechanism for thread interruption.
            break;
        }
	}
	waitForAllThreads(threads);
	threads.clear();
	
	return par_final_result;
}
