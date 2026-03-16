#ifndef RECUTILS_H
#define RECUTILS_H

#include "../exceptions/recommenderExceptions.h"
#include <vector>
#include <string>
#include <chrono>

//returns 'false' for negative numbers.
bool strIsNumeric(std::string);

//returns 'false' for positive numbers.
bool strIsNegativeNumber(std::string);

//assumes that input 'str' already represents a number.
unsigned int strToUint(std::string str);
unsigned long strToUlong(std::string str);
long strToPositiveLong(std::string str);
double strToDouble(std::string str);

//converts a boolean str to either 'true' or 'false.
std::string boolToAlphaStr(bool b);

//assumes that the input 'char' already represents a digit.
unsigned int charToUint(char);
unsigned long charToUlong(char);
long charToPositiveLong(char);

std::string vecUlongAsCommaSeparatedStr(std::vector<unsigned long> vec);
std::string vecStrAsCommaSeparatedStr(std::vector<std::string> vec);

std::vector<std::string> splitAroundCharacter(std::string str,char c);

void reportError(RecommenderException& e);

//'lighter' version for non-critical errors.
//return a boolean which determines whether the application should be terminated.
bool reportErrorLight(RecommenderException& e);
bool reportErrorLight(RecommenderException& e,std::string extraMessage);

void stripLeadingAndTrailingWhitespaces(std::string& str);

std::string removeWhitespaces(std::string str);

//converts all instances of 1+ spaces to 1 space.
std::string removeExtraWhitespaces(std::string str);

std::string strToLower(std::string str);

//matrix utils.
void writeToFile_UserEntries(unsigned long maxTagsPerUser,float** userEntries, size_t rowcount,FILE* file);
void writeToFile_Movies(char*** movies, size_t rowcount,FILE* file);
void writeToFile_GenreEntries(unsigned long maxGenreCount,char*** genreEntries, size_t rowcount,FILE* file);
void writeToFile_Tags(char** tags, size_t rowcount,FILE* file);
//

class CustomSteadyTimer{
	
	private:
	
    std::chrono::steady_clock::time_point startPoint;
    std::chrono::steady_clock::time_point endPoint;

    inline std::chrono::steady_clock::time_point timeMark(){

        return std::chrono::steady_clock::now();
    }

    inline double getTimeElapsed(std::chrono::steady_clock::time_point t1 ,
                                 std::chrono::steady_clock::time_point t2){

        std::chrono::duration<double> durat =
                std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

        return durat.count();
    }
	
	public:
	
	void markStartPoint(){
		
		startPoint = timeMark();
	}
	
	void markEndPoint(){
		
		endPoint = timeMark();
		
	}
	
	double getTimeElapsed(){
		
        return getTimeElapsed(startPoint,endPoint);
		
	}
	
	double markEndPointAndGetTimeElapsed(){
		
		markEndPoint();
		return getTimeElapsed();
		
	}
};

#endif
