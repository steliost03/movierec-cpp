#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <chrono>

inline std::chrono::high_resolution_clock::time_point timeMark(){
	
	return std::chrono::high_resolution_clock::now();
	
	
}

inline double getTimeElapsed(std::chrono::high_resolution_clock::time_point t1 , std::chrono::high_resolution_clock::time_point t2){
	
	std::chrono::duration<double> durat = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	return durat.count();
	
}

class CustomTimer {
	
	private:
	
	std::chrono::high_resolution_clock::time_point startPoint;
	std::chrono::high_resolution_clock::time_point endPoint;
	
	public:
	
	void markStartPoint(){
		
		startPoint = timeMark();
	}
	
	void markEndPoint(){
		
		endPoint = timeMark();
		
	}
	
	double getTimeElapsed(){
		
		return ::getTimeElapsed(startPoint,endPoint);
		
	}
	
	double markEndPointAndGetTimeElapsed(){
		
		markEndPoint();
		return getTimeElapsed();
		
	}
};

#endif
