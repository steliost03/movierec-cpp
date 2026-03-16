#include "dataCalculations.h"

#include "../logging/recommenderLogger.h"
#include "../utils/comparators.h"
#include "../utils/utils.h"
#include "../math/mathUtils.h"

#include <set>
#include <algorithm>

using namespace std;
typedef unsigned long ulong;

//log levels: 1->ERROR,2->WARN,3->INFO,4->DEBUG,5->TRACE
static int LOGLEVEL=4; //TRACE level may impact performance. recommended=DEBUG or higher for non-testing use.
static bool USE_HARDCODED_LOGLEVEL = false;

void filterNeighborsOfUser(Configuration* config,User& user){

    //sort neighbors by weight.
    vector<NeighborUser>& neighborUsers = user.neighbors;
    sort(neighborUsers.begin(),neighborUsers.end(),neighborUserWeightComparator);
    vector<NeighborUser>::iterator first = neighborUsers.begin();
    vector<NeighborUser>::iterator last;
    if(config->maxNeighbors <= neighborUsers.size()){
        //last = neighborUsers.begin() + config->maxNeighbors + 1;
        last = neighborUsers.begin() + config->maxNeighbors;
    } else{
        last = neighborUsers.end();
    }
    neighborUsers = vector<NeighborUser>(first,last);
}

void filterNeighborsOfMovie(Configuration* config,Movie& movie){

    //sort neighbors by weight.
    vector<NeighborMovie>& neighborMovies = movie.neighbors;
    sort(neighborMovies.begin(),neighborMovies.end(),neighborMovieWeightComparator);
    vector<NeighborMovie>::iterator first = neighborMovies.begin();
    vector<NeighborMovie>::iterator last;
    if(config->maxNeighbors <= neighborMovies.size()){
        //last = neighborMovies.begin() + config->maxNeighbors + 1;
        last = neighborMovies.begin() + config->maxNeighbors;
    } else{
        last = neighborMovies.end();
    }
    neighborMovies = vector<NeighborMovie>(first,last);
}


vector<ulong> findCommonMovieIdsBetweenUsers(User& usr1 , User& usr2){

    vector<MovieEntry>& movieEntries1 = usr1.movieEntries;
    vector<MovieEntry>& movieEntries2 = usr2.movieEntries;

    set<ulong> movieEntryIds1;
    set<ulong> movieEntryIds2;

    for(auto it=movieEntries1.begin();it!=movieEntries1.end();it++){
        if(it->rated){
             movieEntryIds1.insert(it->movieId);
        }
    }

    for(auto it=movieEntries2.begin();it!=movieEntries2.end();it++){
        if(it->rated){
             movieEntryIds2.insert(it->movieId);
        }
    }

    vector<ulong> result;
    set_intersection(movieEntryIds1.begin(),movieEntryIds1.end(),movieEntryIds2.begin(),movieEntryIds2.end(),back_inserter(result));
    return result;
}

vector<ulong> findCommonUserIdsBetweenMovies(Movie& movie1,Movie& movie2){

    vector<UserEntry>& userEntries1 = movie1.userEntries;
    vector<UserEntry>& userEntries2 = movie2.userEntries;

    set<ulong> userEntryIds1;
    set<ulong> userEntryIds2;

    for(auto it=userEntries1.begin();it!=userEntries1.end();it++){
        if(it->rated){
            userEntryIds1.insert(it->userId);
        }
    }

    for(auto it=userEntries2.begin();it!=userEntries2.end();it++){
        if(it->rated){
         userEntryIds2.insert(it->userId);
        }
    }

    vector<ulong> result;
    set_intersection(userEntryIds1.begin(),userEntryIds1.end(),userEntryIds2.begin(),userEntryIds2.end(),back_inserter(result));
    return result;
}

vector<double> getSubsetOfUserRatingsByMovieIds(User& user,vector<ulong> movieIds){

    vector<double> ratingsSubset;
    vector<MovieEntry>& movieEntries = user.movieEntries;
    for(auto it=movieIds.begin();it!=movieIds.end();it++){
        ulong movieId = *it;
        auto searchResult = find_if(movieEntries.begin(),movieEntries.end(),[&movieId](MovieEntry& mvEntr){return mvEntr.movieId == movieId;});
        if(searchResult == movieEntries.end()){
            continue;
        }
        ratingsSubset.push_back(searchResult->rating);
    }
    return ratingsSubset;
}

vector<double> getSubsetOfMovieRatingsByUserIds(Movie& movie,vector<ulong> userIds){

    vector<double> ratingsSubset;
    vector<UserEntry>& userEntries = movie.userEntries;
    for(auto it=userIds.begin();it!=userIds.end();it++){
        ulong userId = *it;
        auto searchResult = find_if(userEntries.begin(),userEntries.end(),[&userId](UserEntry& usrEntr){return usrEntr.userId == userId;});
        if(searchResult == userEntries.end()){
            continue;
        }
        ratingsSubset.push_back(searchResult->rating);
    }
    return ratingsSubset;
}

//version where 'allUsers' information is supplied.
vector<double> getMeanRatingsOfCommonUsers(vector<ulong>& commonUsers,vector<User>& allUsers){

    vector<double> result;
    for(auto it = commonUsers.begin();it!=commonUsers.end();it++){
        ulong userId = *it;
        auto userIter = find_if(allUsers.begin(),allUsers.end(),[&userId](User& usr){return usr.userId == userId;});
        if(userIter == allUsers.end()){
            string exMsg = "Common User ID was unexpectedly not found in memory.";
            throw DataCalculationException(exMsg);
        }
        result.push_back(userIter->meanRating);
    }
    return result;
}

//note: only movieEntries with non-zero ratings are considered.
size_t calculateItemRatingCount(vector<User>& allUsers,ulong targetMovieId){
    size_t result = 0;
    for(auto it=allUsers.begin();it!=allUsers.end();it++){
        vector<MovieEntry>& movieEntries = it->movieEntries;
        if(find_if(movieEntries.begin(),movieEntries.end(),[&targetMovieId](MovieEntry& mvEntry){return (mvEntry.movieId == targetMovieId)&&(mvEntry.rated);}) != movieEntries.end()){
            result++;
        }
    }
    return result;
}

//note: only userEntries with non-zero ratings are considered.
size_t calculateUserRatingCount(vector<Movie>& allMovies,ulong targetUserId){

    size_t result = 0;
    for(auto it=allMovies.begin();it!=allMovies.end();it++){
        vector<UserEntry>& userEntries = it->userEntries;
        if(find_if(userEntries.begin(),userEntries.end(),[&targetUserId](UserEntry& usrEntry){return (usrEntry.userId == targetUserId)&&(usrEntry.rated);}) != userEntries.end()){
            result++;
        }
    }
    return result;
}

//version where movieEntries are NOT initially stored in the User object
vector<double> collectRatingsForUser(User& currentUser,vector<MovieEntry>& movieEntries,int loglevel){
    RecommenderLogger LOG;
    LOG.init("dataCalculations:collectRatingsForUser",loglevel);
    LOG.debug("Will collect ratings for user="+to_string(currentUser.userId));
    vector<double> collectedRatings;
    for(size_t i=0;i<movieEntries.size();i++){
        if(movieEntries[i].rated){
            collectedRatings.push_back(movieEntries[i].rating);
        }
    }
    LOG.debug("Collected rating count: "+to_string(collectedRatings.size()));
    currentUser.movieEntries = movieEntries;
    LOG.close();
    return collectedRatings;
}

//version where movieEntries are ALREADY stored in the User object
vector<double> collectRatingsForUser(User& currentUser){
    vector<double> collectedRatings;
    for(size_t i=0;i<currentUser.movieEntries.size();i++){
        if(currentUser.movieEntries[i].rated){
            collectedRatings.push_back(currentUser.movieEntries[i].rating);
        }
    }
    return collectedRatings;
}

//version where userEntries are NOT initally stored in the Movie object
vector<double> collectRatingsForMovie(Movie& currentMovie,vector<UserEntry>& userEntries,int loglevel){
    RecommenderLogger LOG;
    LOG.init("dataCalculations:collectRatingsForMovie",loglevel);
    LOG.debug("Will collect ratings for movie="+to_string(currentMovie.movieId));
    vector<double> collectedRatings;
    for(size_t i=0;i<userEntries.size();i++){
        if(userEntries[i].rated){
            collectedRatings.push_back(userEntries[i].rating);
        }
    }
    LOG.debug("Collected rating count: "+to_string(collectedRatings.size()));
    currentMovie.userEntries = userEntries;
    LOG.close();
    return collectedRatings;
}

//version where userEntries are ALREADY stored in the Movie object
vector<double> collectRatingsForMovie(Movie& currentMovie){
    vector<double> collectedRatings;
    for(size_t i=0;i<currentMovie.userEntries.size();i++){
        if(currentMovie.userEntries[i].rated){
            collectedRatings.push_back(currentMovie.userEntries[i].rating);
        }
    }
    return collectedRatings;
}

//in this version of 'calculateNeighbors' not only the 'neighbors' vector of the given User are updated,
//but also any other user in the data that has the input User as neighbor.
void calculateNeighborsForUserInternal(User& targetUser,vector<User>& allUsers,size_t startIndex,Configuration* config){
    for(size_t i=startIndex;i<allUsers.size();i++){
        User& otherUser = allUsers[i];
        NeighborUser currentUserNeighbor; // == the other user.
        NeighborUser otherUserNeighbor; // == the current user.
        //1: find their common items,the ratings and the mean rating of the users for these common items.
        vector<ulong> commonItemsIds = findCommonMovieIdsBetweenUsers(targetUser,otherUser);
        if(commonItemsIds.size() < config->minimumCommonElementsForUserNeighbors){
            continue;
        }
        sort(commonItemsIds.begin(),commonItemsIds.end(),ulongComparator);
        vector<double> currentUserRatingSubset = getSubsetOfUserRatingsByMovieIds(targetUser,commonItemsIds);
        vector<double> otherUserRatingSubset = getSubsetOfUserRatingsByMovieIds(otherUser,commonItemsIds);
        double* currentUserRatingSubsetPointer = &currentUserRatingSubset[0];
        double* otherUserRatingSubsetPointer = &otherUserRatingSubset[0];
        double currentUserRatingSubsetMean = calculateMean(currentUserRatingSubsetPointer,currentUserRatingSubset.size());
        double otherUserRatingSubsetMean = calculateMean(otherUserRatingSubsetPointer,otherUserRatingSubset.size());

        //2: calculate idf for every common item
        size_t userCount = allUsers.size();
        vector<double> inverseFrequenciesCommon;
        size_t itemRatingCount;
        for (auto it=commonItemsIds.begin();it!=commonItemsIds.end();it++){
            itemRatingCount = calculateItemRatingCount(allUsers,*it);
            inverseFrequenciesCommon.push_back(calculateIDF(userCount, itemRatingCount));
        }

        //3: calculate corellation.
        double* inverseFrequenciesCommonPointer = &inverseFrequenciesCommon[0];
        double weight = calculatePearsonCorrelation(currentUserRatingSubsetPointer, otherUserRatingSubsetPointer, commonItemsIds.size(), currentUserRatingSubsetMean, otherUserRatingSubsetMean, inverseFrequenciesCommonPointer);

        //4: calculate weight significance and modify corellation.
        weight = modifyCorrelationBySignificance(weight, config->significanceWeightingFactor, commonItemsIds.size());
        if(config->discardNegativeWeightNeighbors && weight<0){
            continue;
        }
        //5: add the neighbor entries to both users.
        currentUserNeighbor.id = otherUser.userId;
        currentUserNeighbor.weight = weight;
        currentUserNeighbor.commonMoviesIds = commonItemsIds;
        otherUserNeighbor.id = targetUser.userId;
        otherUserNeighbor.weight = weight;
        otherUserNeighbor.commonMoviesIds = commonItemsIds;

        targetUser.neighbors.push_back(currentUserNeighbor);
        otherUser.neighbors.push_back(otherUserNeighbor);
    }
    //final step:
    //keep only the closest neighbors for the current user.
    filterNeighborsOfUser(config,targetUser);
}

//in this version of 'calculateNeighbors' not only the 'neighbors' of given Movie are updated,
//but also any other movie in the data that has the input Movie as neighbor.
void calculateNeighborsForMovieInternal(Movie& targetMovie,vector<Movie>& allMovies,size_t startIndex,vector<User>& allUsers,Configuration* config){
    for(size_t i=startIndex;i<allMovies.size();i++){
        Movie& otherMovie = allMovies[i];
        NeighborMovie currentMovieNeighbor; // === the other movie.
        NeighborMovie otherMovieNeighbor; // === the current movie.

        //1: find their common users,the ratings and the mean rating of these common users.
       vector<ulong> commonUsersIds = findCommonUserIdsBetweenMovies(targetMovie,otherMovie);
       if(commonUsersIds.size() < config->minimumCommonElementsForMovieNeighbors){
           continue;
       }
       sort(commonUsersIds.begin(),commonUsersIds.end(),ulongComparator);
       vector<double> commonUsersMeanRatings = getMeanRatingsOfCommonUsers(commonUsersIds,allUsers);
       vector<double> currentMovieRatingSubset = getSubsetOfMovieRatingsByUserIds(targetMovie,commonUsersIds);
       vector<double> otherMovieRatingSubset = getSubsetOfMovieRatingsByUserIds(otherMovie,commonUsersIds);
       double* currentMovieRatingSubsetPointer = &currentMovieRatingSubset[0];
       double* otherMovieRatingSubsetPointer = &otherMovieRatingSubset[0];
       double* commonUsersMeanRatingsPointer = &commonUsersMeanRatings[0];

        //2: calculate idf for every common user
       vector<double> inverseFrequenciesCommon;
       size_t itemCount = allMovies.size();
       for (auto it=commonUsersIds.begin();it!=commonUsersIds.end();it++){
           size_t userRatingCount = calculateUserRatingCount(allMovies,*it);
           inverseFrequenciesCommon.push_back(calculateIDF(itemCount, userRatingCount));
       }
       double* inverseFrequenciesCommonPointer = &inverseFrequenciesCommon[0];

       //3: calculate corellation
       double weight = calculateAdjustedCosineSimilarity(currentMovieRatingSubsetPointer,otherMovieRatingSubsetPointer,commonUsersIds.size(),commonUsersMeanRatingsPointer,inverseFrequenciesCommonPointer);
       if(config->discardNegativeWeightNeighbors && weight<0){
           continue;
       }
       //4: calculate weight significance and modify corellation.
       weight = modifyCorrelationBySignificance(weight,config->significanceWeightingFactor,commonUsersIds.size());

       //5: add the neighbor entries to both movies.
       currentMovieNeighbor.id = otherMovie.movieId;
       currentMovieNeighbor.weight = weight;
       currentMovieNeighbor.commonUsersIds = commonUsersIds;
       otherMovieNeighbor.id = targetMovie.movieId;
       otherMovieNeighbor.weight = weight;
       otherMovieNeighbor.commonUsersIds = commonUsersIds;

       targetMovie.neighbors.push_back(currentMovieNeighbor);
       otherMovie.neighbors.push_back(otherMovieNeighbor);
    }
    filterNeighborsOfMovie(config,targetMovie);
}

void calculateNeighborsForUser(User& targetUser,vector<User>& allUsers,Configuration* config){

    int loglevel = USE_HARDCODED_LOGLEVEL? LOGLEVEL : config->dataProceduresLogLevel;

    RecommenderLogger LOG;
    LOG.init("dataCalculations:calculateNeighborsForUser",loglevel);
    LOG.debug("Calculating neighbors for user="+to_string(targetUser.userId));

    //for every OTHER user:
    for(size_t i=0;i<allUsers.size();i++){

        User& otherUser = allUsers[i];

        if(targetUser.userId == otherUser.userId){
            continue;
        }
        LOG.trace("Processing 'other' user="+to_string(otherUser.userId));

        NeighborUser currentUserNeighbor; // === the other user.

        //1: find their common items,the ratings and the mean rating of the users for these common items.
        vector<ulong > commonItemsIds = findCommonMovieIdsBetweenUsers(targetUser,otherUser);
        if(commonItemsIds.size() < config->minimumCommonElementsForUserNeighbors){
             continue;
        }
        sort(commonItemsIds.begin(),commonItemsIds.end(),ulongComparator);
        vector<double> currentUserRatingSubset = getSubsetOfUserRatingsByMovieIds(targetUser,commonItemsIds);
        vector<double> otherUserRatingSubset = getSubsetOfUserRatingsByMovieIds(otherUser,commonItemsIds);
        double* currentUserRatingSubsetPointer = &currentUserRatingSubset[0];
        double* otherUserRatingSubsetPointer = &otherUserRatingSubset[0];
        double currentUserRatingSubsetMean = calculateMean(currentUserRatingSubsetPointer,currentUserRatingSubset.size());
        double otherUserRatingSubsetMean = calculateMean(otherUserRatingSubsetPointer,otherUserRatingSubset.size());

        //2: calculate idf for every common item
        size_t userCount = allUsers.size();
        vector<double> inverseFrequenciesCommon;
        size_t itemRatingCount;
        for (auto it=commonItemsIds.begin();it!=commonItemsIds.end();it++){
            itemRatingCount = calculateItemRatingCount(allUsers,*it);
            inverseFrequenciesCommon.push_back(calculateIDF(userCount, itemRatingCount));
        }

        //3: calculate corellation.
        double* inverseFrequenciesCommonPointer = &inverseFrequenciesCommon[0];
        double weight = calculatePearsonCorrelation(currentUserRatingSubsetPointer, otherUserRatingSubsetPointer, commonItemsIds.size(), currentUserRatingSubsetMean, otherUserRatingSubsetMean, inverseFrequenciesCommonPointer);

        //4: calculate weight significance and modify corellation.
        weight = modifyCorrelationBySignificance(weight, config->significanceWeightingFactor, commonItemsIds.size());
        if(config->discardNegativeWeightNeighbors && weight<0){
            continue;
        }
        //5: add the neighbor entry to the target user.
        currentUserNeighbor.id = otherUser.userId;
        currentUserNeighbor.weight = weight;
        currentUserNeighbor.commonMoviesIds = commonItemsIds;

        targetUser.neighbors.push_back(currentUserNeighbor);
    }
    //final step:
    //keep only the closest neighbors for the current user.
    filterNeighborsOfUser(config,targetUser);
}

void calculateNeighborsForMovie(Movie& targetMovie,vector<Movie>& allMovies,vector<User>& allUsers,Configuration* config){

    int loglevel = USE_HARDCODED_LOGLEVEL? LOGLEVEL: config->dataProceduresLogLevel;

    RecommenderLogger LOG;
    LOG.init("dataCalculations:calculateNeighborsForMovie",loglevel);
    LOG.debug("Calculating neighbors for movie="+to_string(targetMovie.movieId));

    //for every OTHER movie:
    for(size_t i=0;i<allMovies.size();i++){

        Movie& otherMovie = allMovies[i];
        NeighborMovie currentMovieNeighbor; // === the other movie.

        //1: find their common users,the ratings and the mean rating of these common users.
       vector<ulong> commonUsersIds = findCommonUserIdsBetweenMovies(targetMovie,otherMovie);
       if(commonUsersIds.size() < config->minimumCommonElementsForMovieNeighbors){
           continue;
       }
       sort(commonUsersIds.begin(),commonUsersIds.end(),ulongComparator);
       vector<double> commonUsersMeanRatings = getMeanRatingsOfCommonUsers(commonUsersIds,allUsers);
       vector<double> currentMovieRatingSubset = getSubsetOfMovieRatingsByUserIds(targetMovie,commonUsersIds);
       vector<double> otherMovieRatingSubset = getSubsetOfMovieRatingsByUserIds(otherMovie,commonUsersIds);
       double* currentMovieRatingSubsetPointer = &currentMovieRatingSubset[0];
       double* otherMovieRatingSubsetPointer = &otherMovieRatingSubset[0];
       double* commonUsersMeanRatingsPointer = &commonUsersMeanRatings[0];

        //2: calculate idf for every common user
       vector<double> inverseFrequenciesCommon;
       size_t itemCount = allMovies.size();
       for (auto it=commonUsersIds.begin();it!=commonUsersIds.end();it++){
           size_t userRatingCount = calculateUserRatingCount(allMovies,*it);
           inverseFrequenciesCommon.push_back(calculateIDF(itemCount, userRatingCount));
       }
       double* inverseFrequenciesCommonPointer = &inverseFrequenciesCommon[0];

       //3: calculate corellation
       double weight = calculateAdjustedCosineSimilarity(currentMovieRatingSubsetPointer,otherMovieRatingSubsetPointer,commonUsersIds.size(),commonUsersMeanRatingsPointer,inverseFrequenciesCommonPointer);
       if(config->discardNegativeWeightNeighbors && weight<0){
           continue;
       }
       //4: calculate weight significance and modify corellation.
       weight = modifyCorrelationBySignificance(weight,config->significanceWeightingFactor,commonUsersIds.size());

       //5: add the neighbor entries to both movies.
       currentMovieNeighbor.id = otherMovie.movieId;
       currentMovieNeighbor.weight = weight;
       currentMovieNeighbor.commonUsersIds = commonUsersIds;

       targetMovie.neighbors.push_back(currentMovieNeighbor);
    }
    //final step:
    //keep only the closest neighbors for the current movie.
    filterNeighborsOfMovie(config,targetMovie);
}

//assuming that the User has userId populated.
double calculateParametersForSingleUser(User& user,vector<MovieEntry> movieEntries,vector<User>& allUsers,Configuration* config,bool movieEntriesStoredInUser){

    int loglevel = USE_HARDCODED_LOGLEVEL? LOGLEVEL : config->dataProceduresLogLevel;

    RecommenderLogger LOG;
    LOG.init("dataCalculations:calculateParametersForSingleUser",loglevel);
    LOG.debug("Calculating user parameters for user="+to_string(user.userId));

    CustomSteadyTimer singleUserParamCalcTimer;
    singleUserParamCalcTimer.markStartPoint();

    //first,collect all ratings.
    vector<double> allRatings;
    if(movieEntriesStoredInUser){
        LOG.trace("movieEntries are already initially stored in user.");
        allRatings = collectRatingsForUser(user);
    } else{
        LOG.trace("movieEntries are not stored in user.");
        allRatings = collectRatingsForUser(user,movieEntries,loglevel);
    }
    //then,calculate parameters
    double* allRatingsPtr = &allRatings[0];
    user.meanRating = calculateMean(allRatingsPtr,allRatings.size());
    LOG.trace("meanRating="+to_string(user.meanRating));
    user.ratingsStandardDeviation = calculateStandardDeviation(allRatingsPtr,allRatings.size());
    LOG.trace("ratingsStandardDeviation="+to_string(user.ratingsStandardDeviation));
    //finally, calculate neighbors
    LOG.trace("Starting neighbor calculation for user.");
    calculateNeighborsForUser(user,allUsers,config);
    double userParamCalcElapsed = singleUserParamCalcTimer.markEndPointAndGetTimeElapsed();
    LOG.trace("Neighbor calculation finished. neighborCount="+to_string(user.neighbors.size()));
    LOG.close();
    return userParamCalcElapsed;
}

//assuming that the Movie has movieId populated.
double calculateParametersForSingleMovie(Movie& movie,vector<UserEntry> userEntries,vector<Movie>& allMovies,vector<User>& allUsers,Configuration* config,bool userEntriesStoredInMovie){

    int loglevel = USE_HARDCODED_LOGLEVEL? LOGLEVEL : config->dataProceduresLogLevel;

    RecommenderLogger LOG;
    LOG.init("dataCalculations:calculateParametersForSingleMovie",loglevel);
    LOG.debug("Calculating movie parameters for movie="+to_string(movie.movieId));

    CustomSteadyTimer singleMovieParamCalcTimer;
    singleMovieParamCalcTimer.markStartPoint();

    //first,collect all ratings.
    vector<double> allRatings;
    if(userEntriesStoredInMovie){
        LOG.trace("userEntries are already initially stored in movie.");
        allRatings = collectRatingsForMovie(movie);
    } else{
        LOG.trace("userEntries are not stored in movie.");
        allRatings = collectRatingsForMovie(movie,userEntries,loglevel);
    }

    //calculate parameters other than neighbors
    double* allRatingsPtr = &allRatings[0];
    movie.meanRating = calculateMean(allRatingsPtr,allRatings.size());
    LOG.trace("meanRating="+to_string(movie.meanRating));
    movie.ratingsStandardDeviation = calculateStandardDeviation(allRatingsPtr,allRatings.size());
    LOG.trace("ratingsStandardDeviation="+to_string(movie.ratingsStandardDeviation));

    //calculate neighbors
    LOG.trace("Starting neighbor calculation for movie.");
    calculateNeighborsForMovie(movie,allMovies,allUsers,config);
    double movieParamCalcElapsed = singleMovieParamCalcTimer.markEndPointAndGetTimeElapsed();
    LOG.trace("Neighbor calculation finished. neighborCount="+to_string(movie.neighbors.size()));
    LOG.close();
    return movieParamCalcElapsed;
}

//assuming that the Users have: userIds and movieEntries populated.
double calculateParametersForAllUsers(vector<User>& allUsers,Configuration* config){

    int loglevel = USE_HARDCODED_LOGLEVEL? LOGLEVEL: config->dataProceduresLogLevel;

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:calculateParametersForAllUsers",loglevel);
    LOG.info("START: calculateParametersForAllUsers");

    CustomSteadyTimer allUserParamCalcTimer;
    allUserParamCalcTimer.markStartPoint();
    for(size_t i=0;i<allUsers.size();i++){
        User& currentUser = allUsers[i];
        vector<double> collectedRatings = collectRatingsForUser(currentUser);
        double* collectedRatingsPtr = &collectedRatings[0];
        currentUser.meanRating = calculateMean(collectedRatingsPtr,collectedRatings.size());
        currentUser.ratingsStandardDeviation = calculateStandardDeviation(collectedRatingsPtr,collectedRatings.size());
        //calculate neighbors for so far unprocessed users.
        calculateNeighborsForUserInternal(currentUser,allUsers,i+1,config);
    }
    double userParamCalcElapsed = allUserParamCalcTimer.markEndPointAndGetTimeElapsed();
    LOG.info("calculateParametersForAllUsers finished.");
    LOG.close();
    return userParamCalcElapsed;
}

//assuming that the Movies have: movieIds,movieNames,genres,totalTags and userEntries populated.
double calculateParametersForAllMovies(vector<Movie>& allMovies,vector<User>& allUsers,Configuration* config){

    int loglevel = USE_HARDCODED_LOGLEVEL? LOGLEVEL : config->dataProceduresLogLevel;

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:calculateParametersForAllMovies",loglevel);
    LOG.info("START: calculateParametersForAllMovies");

    CustomSteadyTimer allMovieParamCalcTimer;
    allMovieParamCalcTimer.markStartPoint();
    for(size_t i=0;i<allMovies.size();i++){
        Movie& currentMovie = allMovies[i];
        vector<double> collectedRatings = collectRatingsForMovie(currentMovie);
        double* collectedRatingsPtr = &collectedRatings[0];
        currentMovie.meanRating = calculateMean(collectedRatingsPtr,collectedRatings.size());
        currentMovie.ratingsStandardDeviation = calculateStandardDeviation(collectedRatingsPtr,collectedRatings.size());
        //calculate neighbors for so far unprocessed movies.
        calculateNeighborsForMovieInternal(currentMovie,allMovies,i+1,allUsers,config);
    }
    double movieParamCalcElapsed = allMovieParamCalcTimer.markEndPointAndGetTimeElapsed();
    LOG.info("calculateParametersForAllMovies finished.");
    LOG.close();
    return movieParamCalcElapsed;
}
