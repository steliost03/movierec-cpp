#include "movieRecommenderUtils.h"
#include "../data/recommenderDataHandler.h"
#include "../main/movieRecommenderResult.h"
#include "../exceptions/recommenderExceptions.h"
#include "../logging/recommenderLogger.h"
#include "../utils/comparators.h"
#include "../utils/utils.h"
#include <algorithm>

using namespace std;

bool isDataAvailableForStandaloneProfile(User& userData){

    bool dataAvailable = true;

    dataAvailable = dataAvailable && (userData.meanRating>0);
    dataAvailable = dataAvailable && (userData.ratingsStandardDeviation>0);
    dataAvailable = dataAvailable && (!userData.neighbors.empty());

    return dataAvailable;
}


vector<MovieRecommendation> recommendMoviesUserBasedMethod(unsigned long targetUserId, RecommenderDataHandler& recommenderDataHandler, Configuration& config);
vector<MovieRecommendation> recommendMoviesItemBasedMethod(unsigned long targetUserId, RecommenderDataHandler& recommenderDataHandler, Configuration& config);
vector<MovieRecommendation> recommendMoviesForStandaloneUserProfileUserBasedMethod(User& userData,RecommenderDataHandler& recommenderDataHandler, Configuration& config);
vector<MovieRecommendation> recommendMoviesForStandaloneUserProfileItemBasedMethod(User& userData,RecommenderDataHandler& recommenderDataHandler, Configuration& config);

MovieRecommendationsWrapper recommendMoviesPersonalized(unsigned long targetUserId,RecommenderDataHandler& recommenderDataHandler,Configuration& config) {
	
    RecommenderLogger LOG;
    LOG.init("movieRecommender:recommendMovies");

    LOG.info("START: recommendMovies");

    MovieRecommendationsWrapper result;

    result.recommendedFromUserMethod = recommendMoviesUserBasedMethod(targetUserId, recommenderDataHandler,config);
    result.recommendedFromItemMethod = recommendMoviesItemBasedMethod(targetUserId, recommenderDataHandler,config);
	
    LOG.info("Personalized recommendations obtained successfully.");
    LOG.close();
	return result;
}

MovieRecommendationsWrapper recommendMoviesPersonalizedStandaloneProfile(User& userData,string profileName,RecommenderDataHandler& recommenderDataHandler,Configuration& config){

    RecommenderLogger LOG;
    LOG.init("MovieRecommender: recommendMoviesPersonalizedStandaloneProfile");

    LOG.info("START: recommendMoviesPersonalizedStandaloneProfile");

    LOG.debug("First: checking if standalone profile has pending undetermined parameters.");
    bool isDataAvailable = isDataAvailableForStandaloneProfile(userData);
    if(!isDataAvailable){
        throw InvalidStateException("The given standalone profile has undetermined parameters. Cannot create a personalized recommendation.");
    }
    UserProfileStandalone tempStandalone;
    tempStandalone.userData = userData;
    tempStandalone.profileName = profileName;
    vector<unsigned long> nonExistentMovieIds = recommenderDataHandler.checkAllMovieIdsOfStandaloneProfileIfExistingInData(tempStandalone);
    if(nonExistentMovieIds.size() > 0){
        throw InvalidStateException("The movie ids of the given standalone profile were not found in data! Cannot create a personalized recommendation. "
                                    "The movieIds not found in data were: "+vecUlongAsCommaSeparatedStr(nonExistentMovieIds));
    }
    MovieRecommendationsWrapper result;

    result.recommendedFromUserMethod = recommendMoviesForStandaloneUserProfileUserBasedMethod(userData,recommenderDataHandler,config);
    result.recommendedFromItemMethod = recommendMoviesForStandaloneUserProfileItemBasedMethod(userData,recommenderDataHandler,config);

    LOG.info("Personalized recommendations for standalone user profile obtained successfully.");
    LOG.close();
    return result;
}

MovieRecommendationsWrapper recommendMoviesGeneralBestRated(RecommenderDataHandler& recommenderDataHandler,
                                                            vector<MovieGenre> genres,vector<string> tagNames,
                                                            unordered_map<string,char>& filteringTypes){

    RecommenderLogger LOG;
    LOG.init("MovieRecommender:recommendMoviesGeneralBestRated");

    LOG.info("START: recommendMoviesGeneralBestRated");

    MovieRecommendationsWrapper result;

    if(genres.empty() && tagNames.empty()){
        LOG.info("No genres or tags provided! Will NOT filter the results.");
        cout<<"No genres or tags provided. Will not filter the results."<<endl;
        result.recommendedFromGeneralMethod = recommenderDataHandler.extractBestRatedMovies();
        return result;
    } else{
        LOG.info("Genres and/or tags provided! Will filter the results.");
        cout<<"Genres and/or tags provided. Will filter the results"<<endl;
        result.recommendedFromGeneralMethod = recommenderDataHandler.extractBestRatedMoviesFiltered(genres,tagNames,filteringTypes);
        return result;
    }
}

bool movieRecommendationPredictedRatingComparator(MovieRecommendation& a, MovieRecommendation& b){
	
	return (a.predictedRating > b.predictedRating); //descending order.
	
}

vector<MovieRecommendation> recommendMoviesUserBasedMethod(unsigned long targetUserId, RecommenderDataHandler& recommenderDataHandler, Configuration& config){

    RecommenderLogger LOG;
    LOG.init("MovieRecommender:recommendMoviesUserBasedMethod. targetUserId="+to_string(targetUserId));

    LOG.info("START: recommendMoviesUserBasedMethod. targetUserId="+to_string(targetUserId));

    LOG.info("Step 1: Find the user information from userId.");
	User targetUser;
	try{
        targetUser = recommenderDataHandler.extractUserInfoFromId(targetUserId);
	}
	catch (UserNotFoundException& e){
        LOG.error("Could not find user with given userId");
        string exMsg = "Could not find user with userId=" + to_string(targetUserId);
        throw UserNotFoundException(exMsg,&e);
	}

    LOG.info("Step 2: Extract all the movies the target user has watched.");
	vector<MovieEntry> watchedMovies;
	try{
		watchedMovies = recommenderDataHandler.extractWatchedMovies(targetUser);
        LOG.debug("Extracted "+to_string(watchedMovies.size())+" movies.");
	}
	catch (MovieNotFoundException& e){
        LOG.error("User has no watched movies. Cannot make a personalized recommendation.");
		string exMsg = "User has no watched movies. Cannot make a personalized recommendation.";
        throw MovieNotFoundException(exMsg,&e);
	}
	if(watchedMovies.size() < 1){
        LOG.error("User has no watched movies. Cannot make a personalized recommendation");
		throw MovieNotFoundException("User has no watched movies. Cannot make a personalized recommendation.");
	}
	
    LOG.info("Step 3: Extract the nearest neighbors of the user.");
	vector<NeighborUser> nearestNeighbors = recommenderDataHandler.extractNearestNeighborUsers(targetUser);
    LOG.debug("Extracted "+to_string(nearestNeighbors.size())+" neighbors.");
	
    LOG.info("Step 4: For each nearest neighbor, find the predicted rating for each movie they have seen that the target user has not. Gather all predicted ratings.");
    vector<MovieRecommendation> predictedRatings = recommenderDataHandler.extractPredictedRatingsUserBased(nearestNeighbors,watchedMovies,targetUser);
	
    LOG.info("Step 5: Sort by predicted rating (from highest to lowest).");
	sort(predictedRatings.begin(),predictedRatings.end(),movieRecommendationPredictedRatingComparator);
	
    LOG.info("Step 6: show a subset of the best-rated movies (based on predicted rating).");
	vector<MovieRecommendation>::const_iterator first = predictedRatings.begin();
	vector<MovieRecommendation>::const_iterator last;
	if(config.recommendationCount <= predictedRatings.size()){ 
		last = predictedRatings.begin() + config.recommendationCount;
	} else{
		last = predictedRatings.end();
	}
	vector<MovieRecommendation> movieRecommendations(first,last);
	
    LOG.info("Recommendations for user-based method extracted successfully.");
    if(movieRecommendations.size() == 0){
        LOG.warn("No user-based personalized recommendations could be made with the existing data for userID:"+to_string(targetUserId));
    } else{
        LOG.info("Obtained "+to_string(movieRecommendations.size())+" user-based peronalized recommendations for userID"+to_string(targetUserId));
    }
    LOG.close();
	return movieRecommendations;	
}

vector<MovieRecommendation> recommendMoviesForStandaloneUserProfileUserBasedMethod(User& userData,RecommenderDataHandler& recommenderDataHandler,Configuration& config){

    RecommenderLogger LOG;
    LOG.init("MovieRecommender: recommendMoviesForStandaloneUserProfileUserBasedMethod");
    LOG.info("START: recommendMoviesForStandaloneUserProfileUserBasedMethod");

    LOG.info("Step 1: Extract all the movies the target user profile has watched.");
    vector<MovieEntry> watchedMovies;
    try{
        watchedMovies = recommenderDataHandler.extractWatchedMovies(userData);
        LOG.debug("Extracted "+to_string(watchedMovies.size())+" movies.");
    }	catch (MovieNotFoundException& e){
        LOG.error("User profile has no watched movies. Cannot make a personalized recommendation.");
        string exMsg = "User profile has no watched movies. Cannot make a personalized recommendation.";
        throw MovieNotFoundException(exMsg,&e);
    }
    if(watchedMovies.size() < 1){
        LOG.error("User profile has no watched movies. Cannot make a personalized recommendation");
        throw MovieNotFoundException("User profile has no watched movies. Cannot make a personalized recommendation.");
    }

    LOG.info("Step 2: Extract the nearest neighbors of the user.");
    vector<NeighborUser> nearestNeighbors = recommenderDataHandler.extractNearestNeighborUsers(userData);
    LOG.debug("Extracted "+to_string(nearestNeighbors.size())+" neighbors.");

    LOG.info("Step 3: For each nearest neighbors, find the predicted rating for each movie they have seen that the target user profile has not. Gather all predicted ratings.");
    vector<MovieRecommendation> predictedRatings = recommenderDataHandler.extractPredictedRatingsUserBased(nearestNeighbors,watchedMovies,userData,true);
    LOG.debug("Extracted "+to_string(predictedRatings.size())+" predicted ratings.");

    LOG.info("Step 4: Sort by predicted rating (from highest to lowest).");
    sort(predictedRatings.begin(),predictedRatings.end(),movieRecommendationPredictedRatingComparator);

    LOG.info("Step 5: show a subset of the best-rated movies (based on predicted rating).");
    LOG.trace("Current configured maximum recommendation count is "+to_string(config.recommendationCount));
    unsigned long targetUserId = userData.userId;
    vector<MovieRecommendation>::const_iterator first = predictedRatings.begin();
    vector<MovieRecommendation>::const_iterator last;
    if(config.recommendationCount <= predictedRatings.size()){
        last = predictedRatings.begin() + config.recommendationCount;
    } else{
        last = predictedRatings.end();
    }
    vector<MovieRecommendation> movieRecommendations(first,last);

    LOG.info("Recommendations for user-based method extracted successfully.");
    if(movieRecommendations.size() == 0){
        LOG.warn("No user-based personalized recommendations could be made with the existing data for user profile with userID:"+to_string(targetUserId));
    } else{
        LOG.info("Obtained "+to_string(movieRecommendations.size())+" user-based peronalized recommendations for user profile with userID"+to_string(targetUserId));
    }

    LOG.info("recommendMoviesForStandaloneUserProfileUserBasedMethod finished.");
    LOG.close();
    return movieRecommendations;
}

vector<MovieRecommendation> recommendMoviesItemBasedMethod(unsigned long targetUserId,RecommenderDataHandler& recommenderDataHandler,Configuration& config) {
	
    RecommenderLogger LOG;
    LOG.init("MovieRecommender:recommendMoviesItemBasedMethod");

    LOG.info("START: recommendMoviesItemBasedMethod. targetUserId="+to_string(targetUserId));

    //1: find the user information from the userId.
	User targetUser = recommenderDataHandler.extractUserInfoFromId(targetUserId);
	
	//2: extract all the movies the target user has watched.
	vector<MovieEntry> watchedMovies = recommenderDataHandler.extractWatchedMovies(targetUser);
	if(watchedMovies.size() < 1){
		throw MovieNotFoundException("User has no watched movies. Cannot make a personalized recommendation.");
	}
	
	//3: sort movies by rating (from highest to lowest). //TODO: add a recency criterion in the sorting?
	sort(watchedMovies.begin(),watchedMovies.end(),movieEntryRatingComparator);
	
    //4: keep only a subset of the user's favorite movies (by rating). If subset disabled, keep all of the watched movies.
	vector<MovieEntry> moviesToCheck;
	if(config.useLimitInWatchedMoviesItemBasedMethod){
		vector<MovieEntry>::const_iterator first = watchedMovies.begin();
        vector<MovieEntry>::const_iterator last;
        if(config.maxWatchedMovieCountUsedInItemBasedMethod <= watchedMovies.size()){
            //last = watchedMovies.begin() + config.maxWatchedMovieCountUsedInItemBasedMethod+1;
			last = watchedMovies.begin() + config.maxWatchedMovieCountUsedInItemBasedMethod;
        } else{
            last = watchedMovies.end();
        }
		moviesToCheck = vector<MovieEntry>(first,last);
	}
	else{
		moviesToCheck = watchedMovies; //maybe use reference instead of copy-constructor here?
	}
	
	//5: for each (favorite,if subset is enabled) movie, find the predicted rating for each of its neighbor-movies. Gather all predicted ratings.
	vector<MovieRecommendation> predictedRatings = recommenderDataHandler.extractPredictedRatingsItemBased(moviesToCheck,watchedMovies);
	
	//6: sort by predicted rating (from highest to lowest)
	sort(predictedRatings.begin(),predictedRatings.end(),movieRecommendationPredictedRatingComparator);
	
	//7: show a subset of the best-rated movies (based on predicted rating).
	vector<MovieRecommendation>::const_iterator first = predictedRatings.begin();
	vector<MovieRecommendation>::const_iterator last;
	if(config.recommendationCount <= predictedRatings.size()){ 
		//last = predictedRatings.begin() + config.recommendationCount+1;
		last = predictedRatings.begin() + config.recommendationCount;
	} else{
		last = predictedRatings.end();
	}
	vector<MovieRecommendation> movieRecommendations(first,last);
	
    LOG.info("Recommendations for item-based method extracted successfully.");
    if(movieRecommendations.size() == 0){
        LOG.warn("No item-based personalized recommendations could be made with the existing data for userID:"+to_string(targetUserId));
    } else{
        LOG.info("Obtained "+to_string(movieRecommendations.size())+" item-based peronalized recommendations for userID"+to_string(targetUserId));
    }
    LOG.close();
	return movieRecommendations;
}

vector<MovieRecommendation> recommendMoviesForStandaloneUserProfileItemBasedMethod(User& userData,RecommenderDataHandler& recommenderDataHandler,Configuration& config){

    RecommenderLogger LOG;
    LOG.init("MovieRecommender: recommendMoviesForStandaloneUserProfileItemBasedMethod");

    LOG.info("START: recommendMoviesForStandaloneUserProfileItemBasedMethod");

    LOG.info("Step 1: extract all the movies the target user profile has watched.");
    vector<MovieEntry> watchedMovies;
    try {
        watchedMovies = recommenderDataHandler.extractWatchedMovies(userData);
        LOG.debug("Extracted "+to_string(watchedMovies.size())+" movies.");
    } catch (MovieNotFoundException& e) {
        LOG.error("User profile has no watched movies. Cannot make a personalized recommendation.");
        string exMsg = "User profile has no watched movies. Cannot make a personalized recommendation.";
        throw MovieNotFoundException(exMsg,&e);
    }
    if(watchedMovies.size() < 1){
        LOG.error("User profile has no watched movies. Cannot make a personalized recommendation");
        throw MovieNotFoundException("User profile has no watched movies. Cannot make a personalized recommendation.");
    }

    //TODO: add a recency criterion in the sorting?
    LOG.info("Step 2: sort watched movies by rating (from highest to lowest)");
    sort(watchedMovies.begin(),watchedMovies.end(),movieEntryRatingComparator);

    //if subset disabled, keep all of the watched movies.
    vector<MovieEntry> moviesToCheck;
    if(config.useLimitInWatchedMoviesItemBasedMethod){
        LOG.info("Step 3: keep only a subset of the user's favorite movies (by rating)");
        vector<MovieEntry>::const_iterator first = watchedMovies.begin();
        vector<MovieEntry>::const_iterator last;
        if(config.maxWatchedMovieCountUsedInItemBasedMethod <= watchedMovies.size()){
            last = watchedMovies.begin() + config.maxWatchedMovieCountUsedInItemBasedMethod;
        } else{
            last = watchedMovies.end();
        }
        moviesToCheck = vector<MovieEntry>(first,last);
    }
    else{
        LOG.info("Step 3: Subset disabled: will keep and check all watched movies.");
        moviesToCheck = watchedMovies; //maybe use reference instead of copy-constructor here?
    }

    LOG.info("Step 4: for each movie, find the predicted rating for each of its neighbor-movies (that the user profile has not watched), and gather all predicted ratings.");
    vector<MovieRecommendation> predictedRatings = recommenderDataHandler.extractPredictedRatingsItemBased(moviesToCheck,watchedMovies,true);

    LOG.info("Step 5: Sort by predicted rating (from highest to lowest).");
    sort(predictedRatings.begin(),predictedRatings.end(),movieRecommendationPredictedRatingComparator);

    LOG.info("Step 6: Showing a subset of the best-rated movies (based on predicted rating)");
    vector<MovieRecommendation>::const_iterator first = predictedRatings.begin();
    vector<MovieRecommendation>::const_iterator last;
    if(config.recommendationCount <= predictedRatings.size()){
        last = predictedRatings.begin() + config.recommendationCount;
    } else{
        last = predictedRatings.end();
    }
    vector<MovieRecommendation> movieRecommendations(first,last);

    LOG.info("Recommendations for item-based method for standalone user profile extracted successfully.");
    if(movieRecommendations.size() == 0){
        LOG.warn("No item-based personalized recommendations could be made with the existing data for userID:"+to_string(userData.userId));
    } else{
        LOG.info("Obtained "+to_string(movieRecommendations.size())+" item-based peronalized recommendations for standalone user profile with userID"+to_string(userData.userId));
    }
    LOG.close();
    return movieRecommendations;
}
