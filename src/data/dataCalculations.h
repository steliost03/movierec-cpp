#ifndef DATA_CALCULATIONS_H
#define DATA_CALCULATIONS_H

#include "../application/configuration/configuration.h"
#include "dtos.h"
#include <vector>

class DataCalculationException: public RecommenderException {

    protected:

    std::string exceptionName = "DataCalculationException";

    public:

    //create with user-defined message.
    DataCalculationException(std::string message){
        exceptionMessage = message;
    }

    //create with cause
    DataCalculationException(std::exception* cause){
        this->exceptionCause = cause;
    }

    //create with message and cause
    DataCalculationException(std::string message,std::exception* cause){
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

//assuming that the User has userId populated.
double calculateParametersForSingleUser(User& user,vector<MovieEntry> movieEntries,vector<User>& allUsers,Configuration* config,bool movieEntriesStoredInUser=false);

//assuming that the Movie has movieId populated.
double calculateParametersForSingleMovie(Movie& movie,vector<UserEntry> userEntries,vector<Movie>& allMovies,vector<User>& allUsers,Configuration* config,bool userEntriesStoredInMovie=false);

//assuming that the Users have: userIds and movieEntries populated.
double calculateParametersForAllUsers(vector<User>& allUsers,Configuration* config);

//assuming that the Movies have: movieIds,movieNames,genres,totalTags and userEntries populated.
double calculateParametersForAllMovies(vector<Movie>& allMovies,vector<User>& allUsers,Configuration* config);

#endif
