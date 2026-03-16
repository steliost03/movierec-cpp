#include "recommenderDataHandler.h"
#include "import/importCsv.h"
#include "dataTransformations.h"
#include "dataCalculations.h"
#include "../logging/recommenderLogger.h"

#include <set>

using namespace std;

//utility functions

size_t calculateUserCountFromUserEntries(float** userEntries,size_t userEntriesMatrixRowsize){
	
    set<float> uniqueUserIds;

    for(size_t i=0;i<userEntriesMatrixRowsize;i++){
		uniqueUserIds.insert(userEntries[i][0]);
	}
	
	return uniqueUserIds.size();
}


// method definitions

ElapsedTimes RecommenderDataHandler::initInMemoryData(Configuration* config){

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:initInMemoryData");
    LOG.info("START: initInMemoryData");
	
	//matrices
	float** userEntries;
	char*** movies;
	char** tags;
	char*** genreEntries;
		
    //(allocates memory for c arrays internally).
    LOG.debug("STEP 1: Loading data from files and storing it to matrices.");
    MatrixSizesWrapper matrixRowSizes = loadFromFiles(config,config->moviesFileActivePath,config->ratingsFileActivePath,config->tagEntriesFileActivePath,config->tagIdsFileActivePath,
    &userEntries,&movies,&tags,&genreEntries,elapsedTimes,config->terminateApplicationOnCsvImportError);
	
    //calculate user count: CURRENTLY UNUSED.
    //size_t userCount = calculateUserCountFromUserEntries(userEntries,matrixRowSizes.userEntriesMatrixRowsize);

    //calculate item count: CURRENTLY UNUSED.
    //size_t itemCount = matrixRowSizes.moviesMatrixRowsize;

    LOG.debug("STEP 2: Converting data to suitable form, calculating neighbors and other parameters.");
    cout<<"-Converting data to suitable form, calculating neighbors and other parameters..."<<endl;
    double createUsersElapsed = createUsersFromCsv(config,userEntries,matrixRowSizes.userEntriesMatrixRowsize,tags,matrixRowSizes.tagsMatrixRowsize,allUsers);
    double createMoviesElapsed = createMoviesFromCsv(config,userEntries,matrixRowSizes.userEntriesMatrixRowsize,movies,matrixRowSizes.moviesMatrixRowsize,tags,matrixRowSizes.tagsMatrixRowsize,genreEntries,matrixRowSizes.genreEntriesMatrixRowsize,allUsers,allMovies);

    this->elapsedTimes.setDataCalcElapsed(createUsersElapsed+createMoviesElapsed);

    LOG.debug("STEP 3: Cleaning up manually allocated memory.");
	cleanup(config,matrixRowSizes,&userEntries,&movies,&tags,&genreEntries);

    LOG.info("initInMemoryData finished.");
    LOG.close();

    return this->elapsedTimes;
}


double RecommenderDataHandler::initParametersForSingleUser(User& user,Configuration* config,bool userHasMovieEntriesPopulated,vector<MovieEntry>* givenMovieEntries){

    vector<MovieEntry> movieEntries;
    if(userHasMovieEntriesPopulated){
        movieEntries = user.movieEntries;
    } else{
        if(givenMovieEntries != nullptr){ //if it is nullptr, movieEntries will remain an empty vector.
            movieEntries = *givenMovieEntries;
        }
    }
    vector<User>& currentAllUsers = this->getAllUsers();
    double elapsed = calculateParametersForSingleUser(user,movieEntries,currentAllUsers,config,userHasMovieEntriesPopulated);
    return elapsed;
}

double RecommenderDataHandler::initParametersForSingleMovie(Movie& movie,Configuration* config,bool movieHasUserEntriesPopulated,vector<UserEntry>* givenUserEntries){
    //TODO:return ElapsedTimes
    vector<UserEntry> userEntries;
    if(movieHasUserEntriesPopulated){
        userEntries = movie.userEntries;
    } else{
        if(givenUserEntries != nullptr){ //if it is nullptr, userEntries will remain an empty vector.
            userEntries = *givenUserEntries;
        }
    }
    vector<Movie>& currentAllMovies = this->getAllMovies();
    vector<User>& currentAllUsers = this->getAllUsers();
    double elapsed = calculateParametersForSingleMovie(movie,userEntries,currentAllMovies,currentAllUsers,config,movieHasUserEntriesPopulated);
    return elapsed;
}


