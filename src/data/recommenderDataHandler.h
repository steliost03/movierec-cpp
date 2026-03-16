#ifndef RCMD_DAO_H
#define RCMD_DAO_H

#include "dtos.h"
#include "../main/movieRecommenderUtils.h"
#include "../application/configuration/configuration.h"
#include "../main/movieGenres.h"
#include "../utils/functionalityNames.h"

#include <vector>
#include <cmath>
#include <string>

class RecommenderDataHandler;

//AGGREGATIONS
struct MostUsedTagResult{
    //TagEntry* tag;
    string tagName;
    size_t timesUsed;
};

struct MostTaggedMovieResult{
    string movieName;
    size_t tagCount;
    Movie* movie;
};

struct MostCommonGenreResult{
    std::string genre;
    size_t timesUsed;
};

struct HighestRatedGenreResult{
    std::string genre;
    double meanRating;
};

class Aggregations {
    private:
    RecommenderDataHandler* rdh;
    bool useDatabase = false;
    public:
    Aggregations(){}
    Aggregations(bool useDatabase,RecommenderDataHandler* rdh){
        this->useDatabase = useDatabase;
        this->rdh = rdh;
    }
    //the below use movie data for result.
    Movie& highestRatedMovie(unsigned long minimumRatingCount=15);
    Movie& mostWatchedMovie();
    Movie& highestDeviationMovie();
    MostTaggedMovieResult mostTaggedMovie();
    MostUsedTagResult mostUsedTag();
    MostCommonGenreResult mostCommonGenre();
    HighestRatedGenreResult highestRatedGenre();
};


//for cases where recommenderDataHandler is not used correctly or is an in invalid state.
class RecommenderDataHandlerException : public RecommenderException {

    protected:

        std::string exceptionName = "RecommenderDataHandlerException";

    public:

        //create with default message.
        RecommenderDataHandlerException(){
            exceptionMessage = "Invalid operation with recommender data handler.";
        }

        //create with user-defined message.
        RecommenderDataHandlerException(std::string message){
            exceptionMessage = "Invalid operation with recommender data handler: "+message;
        }

        //create with user-defined message and cause.
        RecommenderDataHandlerException(std::string message,RecommenderException* cause){
            exceptionMessage = message;
            exceptionCause = cause;
            causeIsRecommenderException = true;
        }

        //overrides
        std::string getExceptionName(){
            return exceptionName;
        }
};


struct NormalizationParameters{

    double rating;
    double meanRating;
    double standardDeviation;

};

class RecommenderDataHandler {
	
    private:
	
    bool hasConfig = false;
    Configuration* config;

    //aggregations object
    Aggregations aggr;
	
    //in-memory data (if enabled)
    vector<User> allUsers;
    vector<Movie> allMovies;
    vector<UserProfileFull> userProfiles;

    //elapsed times for various operations (corresponding to the last instance each one was initiated)
    ElapsedTimes elapsedTimes;
	
    //definitions for the below in: initInMemoryData.cpp
    ElapsedTimes initInMemoryData(Configuration* config);
    ElapsedTimes initDB(){throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE);}//TODO:implement
    //definitions for the above in: initInMemoryData.cpp

    size_t getMovieUserRatingsCount(unsigned long movieId);
    double getInverseUserFrequency(unsigned long movieId);

    vector<MovieEntry> extractWatchedMoviesFromUserId(unsigned long userId);
	
    MovieRecommendation predictRatingUserBasedMethodWithDB(); //TODO
    MovieRecommendation predictRatingUserBasedMethodWithInMemoryDataRealTimeCalculations(); //TODO
    MovieRecommendation predictRatingUserBasedMethodWithInMemoryData(NeighborUser& neighborUser,MovieEntry& movieEntry);
	
    void updateRatingPredictionUserBasedMethodWithDB();	 //TODO
    MovieRecommendation updateRatingPredictionUserBasedMethodWithInMemoryDataRealTimeCalculations();  //TODO
    MovieRecommendation updateRatingPredictionUserBasedMethodWithInMemoryData(NeighborUser& neighborUser,MovieEntry& movieEntry,MovieRecommendation& movieRecommendation);
	
	
    MovieRecommendation predictRatingItemBasedMethodWithDB(); //TODO
    MovieRecommendation predictRatingItemBasedMethodWithInMemoryDataRealTimeCalculations(); //TODO
    MovieRecommendation predictRatingItemBasedMethodWithInMemoryData(vector<NeighborMovie>& neighborMovies,vector<MovieEntry>& watchedMovies,Movie& targetMovie);

    MovieRecommendation predictRatingUserBasedMethod(NeighborUser& neighborUser,MovieEntry& movieEntry);
    MovieRecommendation predictRatingItemBasedMethod(vector<NeighborMovie>& neighborMovie,vector<MovieEntry>& watchedMovies,Movie& targetMovie);

    MovieRecommendation updateRatingPredictionUserBasedMethod(NeighborUser& neighborUser, MovieEntry& movieEntry, MovieRecommendation& movieRecommendation);
    MovieRecommendation updateRatingPredictionItemBasedMethod();
	 
    double getInverseUserFrequency();
		
    Movie convertMovieEntryToMovieObject(MovieEntry& movieEntry);
    Movie convertNeighborMovieToMovieObject(NeighborMovie& neighborMovie);

    void finalizeMovieRecommendationUserBased(MovieRecommendation& movieRecommendation,User& currentUser);
    void finalizeMovieRecommendationItemBased(MovieRecommendation& movieRecommendation);

    NormalizationParameters extractNormalizationParametersFromMovieId(unsigned long movieId,vector<MovieEntry>& watchedMovies);

    public:
	
    RecommenderDataHandler(){}
	
    //sets configs, initializes data , initializes aggregations object.
    RecommenderDataHandler(Configuration* config){
        setConfig(config);
        initData();
        initOrUpdateAggr();
    }
	//

    void setConfig(Configuration* config){
        this->config = config;
        hasConfig = true;
    }

    void initData(){
        if(!hasConfig){
            throw RecommenderDataHandlerException("Cannot initialize data without a configuration.");
        }
        if(config->useDatabase){
            initDB();
        } else{
            initInMemoryData(config);
        }
    }

    void initOrUpdateAggr(){
        if(!hasConfig){
            throw RecommenderDataHandlerException("Cannot initialize aggregations object without a configuration.");
        }
        aggr = Aggregations(config->useDatabase,this);
    }

    //definitions for the below in: initInMemoryData.cpp
    double initParametersForSingleUser(User& targetUser,Configuration* config,bool userHasMovieEntriesPopulated=true,vector<MovieEntry>* givenMovieEntries=nullptr);
    double initParametersForSingleMovie(Movie& targetMovie,Configuration* config,bool movieHasUserEntriesPopulated=true,vector<UserEntry>* givenUserEntries=nullptr);
    //definitions for the above in: initInMemoryData.cpp

    //loads profiles from default filepath (defined in configuration class).
    void initUserProfiles(bool dataIsAvailable = true);
    //loads profile from a custom filepath.
    void loadUserProfilesFromFile(string filepath,bool dataIsAvailable);

    void saveUserProfiles();
    void saveUserProfiles(std::string filepath,bool pretty);

    //bool return value-> operation success.
    bool createNewUserProfileStandalone(std::string profileName,unsigned long userId,vector<MovieEntry> movieEntries,bool dataIsAvailable=true);
    bool createNewUserProfileStandalone(UserProfileFull); //for testing purposes.
    bool createNewUserProfileDataBound(std::string profileName,unsigned long userId,bool dataIsAvailable=true);
    bool createNewuserProfileDataBound(UserProfileFull); //for testing purposes.

    //returns the number of profiles affected.
    unsigned long populateWaitingUserProfiles();
    //returns the number of profiles affected.
    unsigned long recalculateProfileParameters();

    bool deleteUserProfile(unsigned long userId);
    bool deleteALLUserProfiles();
    UserProfileFull& loadUserProfile(unsigned long userId);
    void editUserProfile(unsigned long userId,short opId);

    //standalone profile only
    //returns the movie ids that were NOT found in loaded data.
    vector<unsigned long> checkAllMovieIdsOfStandaloneProfileIfExistingInData(UserProfileStandalone&);

    vector<UserProfileFull>& getAllUserProfiles(){
        return userProfiles;
    }

    vector<User>& getAllUsers(){
		return allUsers;
	}

    vector<Movie>& getAllMovies(){
		return allMovies;
	}

    //append data //

    //the below return true if addition was successful.
    bool addSingleUser(User&);
    bool addSingleMovie(Movie&);
    //the below return true if deletion was successful.
    bool deleteSingleUser(User&);
    bool deleteSingleUser(unsigned long);
    bool deleteSingleMovie(Movie&);
    bool deleteSingleMovie(unsigned long);

    unsigned long addUsers(vector<User>& usersToAppend){
        unsigned long addedCount = 0;
        for(auto it=usersToAppend.begin();it!=usersToAppend.end();it++){
            if(addSingleUser(*it)){
                addedCount++;
            }
        }
        return addedCount;
    }

    unsigned long addMovies(vector<Movie>& moviesToAppend){
        unsigned long addedCount = 0;
        for(auto it=moviesToAppend.begin();it!=moviesToAppend.end();it++){
            if(addSingleMovie(*it)){
                addedCount++;
            }
        }
        return addedCount;
    }

    //

    //rollback:will delete after index(position) exclusive, until the end.
    bool rollbackUsers(size_t position);
    bool rollbackMovies(size_t position);

    //fully delete data
    bool deleteALLUsers();
    bool deleteALLMovies();

    //

    //to be mostly used for testing purposes
    void setAllUsers(vector<User> users){
        if(config->useDatabase){
            //TODO
        } else{
            allUsers = users;
        }
    }

    void setAllMovies(vector<Movie> movies){
        if(config->useDatabase){
            //TODO
        } else{
            allMovies = movies;
        }
    }

    void setUserProfiles(vector<UserProfileFull> usrPrf){
        if(config->useDatabase){
            //TODO
        } else{
            userProfiles = usrPrf;
        }
    }
    // //
	
    size_t getUserCount();
    size_t getMovieCount();

    size_t getRatingCount(); //uses user data to extract result.
    size_t getTotalTagCount();    //uses movie data to extract result.
    size_t getUniqueTagCount(); //uses movie data to extract result.

    ElapsedTimes& getElapsedTimes(){
        return elapsedTimes;
    }

    User extractUserInfoFromId(unsigned long targetUserId);
    User* extractUserInfoPointerFromId(unsigned long targetUserId);
    vector<User> extractNearestNeighbors(User& targetUser);
    vector<NeighborUser> extractNearestNeighborUsers(User& targetUser);
    vector<MovieEntry> extractWatchedMovies(User& targetUser);
    vector<NeighborMovie> extractNearestNeighborMovies(Movie& targetMovie);
    vector<MovieRecommendation> extractPredictedRatingsUserBased(vector<NeighborUser>& nearestNeighbors,vector<MovieEntry>& watchedMovies,User& currentUser,bool forStandaloneProfile=false);
    vector<MovieRecommendation> extractPredictedRatingsItemBased(vector<MovieEntry>& moviesToCheck, vector<MovieEntry>& watchedMovies,bool forStandaloneProfile=false);
    Movie extractMovieFromMovieId(unsigned long movieId);
    Movie getMovieFromMovieEntry(MovieEntry& movieEntry);

    vector<MovieRecommendation> extractBestRatedMovies();

    //filteringType values: '1'->strict, '2'->lenient , '3'->very lenient
    vector<MovieRecommendation> extractBestRatedMoviesFiltered(vector<MovieGenre> genres,vector<string> tagNames,unordered_map<string,char>& filteringTypes);

    vector<MovieRecommendation> extractPredictedRatingsUserBasedFiltered(); //TODO
    vector<MovieRecommendation> extractPredictedRatingsItemBasedFiltered(); //TODO

    //positionFound is only populated if not given as nullptr.
    //it will be returned as nullptr if target is not found.
    bool userIdExists(unsigned long,size_t* positionFound = nullptr);
    bool movieIdExists(unsigned long,size_t* positionFound = nullptr);
    bool tagIdExists(unsigned long,size_t* positionFound = nullptr);

    //if tagId is not found, returns 0.
    unsigned long getTagIdForTagName(string);
    //generates a user id that does not already exist.
    unsigned long autogenerateUserId();
    //autogenerate a random tag id.
    unsigned long autogenerateRandomTagId(int seed=0);
    //tries to find if the tagname already exists and returns its tagId, otherwise generates a tagId that does not already exist.
    unsigned long findOrGenerateTagId(string tagName);

    ElapsedTimes calculateAllParameters();

    //AGGREGATIONS
    Aggregations aggregation(){
        return aggr;
    }
};
#endif
