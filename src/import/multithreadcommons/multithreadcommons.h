#ifndef MULTITHREADCOMMONS_H
#define MULTITHREADCOMMONS_H

#include <thread>
#include <vector>
typedef unsigned int uint;

inline uint initThreadCount(uint givenThreadcount, uint maxThreadcount){
	
	uint chosenThreadcount;
	
	if(givenThreadcount > maxThreadcount){
		chosenThreadcount = maxThreadcount;
	}
	else{
		chosenThreadcount = givenThreadcount;
	}
	
	return chosenThreadcount;
}

inline void splitInterval(uint subIntervalCount,uint begin,uint end,std::vector<uint>& subIntervLimits){
	
	if(subIntervalCount > (end-begin) ){
		subIntervalCount = end-begin;
	}
	
	subIntervLimits.clear();
	subIntervLimits.resize(subIntervalCount+1);
	subIntervLimits[0] = begin;
	subIntervLimits[subIntervalCount] = end;
	
	uint subIntervLen = (end-begin) / subIntervalCount;
	
	for(uint i=1;i<subIntervalCount;i++){
		
		subIntervLimits[i] = subIntervLimits[i-1] + subIntervLen;
	}
}

inline void waitForAllThreads(std::vector<std::thread>& threads){

    for(unsigned int i=0;i<threads.size();i++){

        threads[i].join();
    }
}

inline unsigned int getMaximumThreads(){
    return std::thread::hardware_concurrency();
}

#endif
