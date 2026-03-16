#ifndef DATA_TRANSFORM_H
#define DATA_TRANSFORM_H

#include "dtos.h"
#include "../application/configuration/configuration.h"
#include "../exceptions/recommenderExceptions.h"
#include <string>
#include <vector>

class DataTransformationException: public RecommenderException {
	
	protected:

	std::string exceptionName = "DataTransformationException";
	
	public:
	
	//create with user-defined message.
	DataTransformationException(std::string message){
		exceptionMessage = message;
	}

	//create with cause
	DataTransformationException(std::exception* cause){
		this->exceptionCause = cause;
	}

	//create with message and cause
	DataTransformationException(std::string message,std::exception* cause){
		this->exceptionMessage = message;
		this->exceptionCause = cause;
	}

    std::string getExceptionName(){
        return exceptionName;
    }
	
	std::string getExceptionMessage(){
		return exceptionMessage;
	}
};

//returns time elapsed.
double createUsersFromCsv(Configuration* config,float** userEntries,size_t usr_rowsize,
                        char** tags,size_t tags_rowsize,vector<User>& users);

//returns time elapsed.
double createMoviesFromCsv(Configuration* config,float** userEntries,size_t usr_rowsize,char*** movies,size_t movies_rowsize,
                         char** tags,size_t tags_rowsize,char*** genreEntries,size_t genreEntries_rowsize,vector<User>& allUsers,
                         vector<Movie>& moviesVec);

//TODO:
void appendUsersFromCsv();
void appendMoviesFromCsv();
void createUsersFromJson();
void appendUsersFromJson();
void createMoviesFromJson();
void appendMoviesFromJson();

#endif
