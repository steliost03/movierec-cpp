#include "dataTransformations.h"

#include "../math/mathUtils.h"
#include "../utils/utils.h"
#include "../logging/recommenderLogger.h"
#include "../utils/comparators.h"

#include <algorithm>
#include <string>
#include <set>
#include <unordered_set>
#include <cstdlib>

using namespace std;

typedef unsigned int uint;
typedef unsigned long ulong;

//log levels: 1->ERROR,2->WARN,3->INFO,4->DEBUG,5->TRACE
static int LOGLEVEL=4; //TRACE level may impact performance. recommended=DEBUG or higher for non-testing use.
static bool USE_HARDCODED_LOGLEVEL = false;

//NAMESPACE_START
namespace dataTransform{

vector<TagEntry> createTagEntriesOfMovieEntry(float* userEntriesRow,char** tags,size_t tags_rowsize){
    vector<TagEntry> result;
    size_t j=3;
    while(userEntriesRow[j] > 0){
        TagEntry newTagEntry;
        //tagId must not already exist in tagEntries.
        ulong tagId = static_cast<ulong>(userEntriesRow[j]);
        if(find_if(result.begin(),result.end(),[&tagId](TagEntry& tagEntry){ return tagEntry.tagId == tagId;}) != result.end()){
            continue;
        }
        newTagEntry.tagId = tagId;
        if(userEntriesRow[j] > tags_rowsize){
            string exMsg = "Tag ID found in userEntriesRow does not exist in tags matrix. TagIDFound="+to_string(userEntriesRow[j]);
            throw DataTransformationException(exMsg);
        }
        //newTagEntry.tagName = string(tags[static_cast<size_t>(userEntriesRow[j])]);
        newTagEntry.tagName = string(tags[tagId-1]);
        result.push_back(newTagEntry);
        j++;
    }
    return result;
}

/*
 * Generates the 'userEntries' vector for a given Movie. Also populates a given vector with the movie's ratings ('currentMovieRatings').
 * If the rating of a userEntry is zero, it is not included in the movie ratings vector.
*/
vector<UserEntry> createUserEntriesOfMovie(ulong targetMovieId,float** userEntries,size_t rowsize,char** tags,size_t tags_rowsize,vector<double>& currentMovieRatings){
    vector<UserEntry> result;
    for(size_t i=0;i<rowsize;i++){
        if(targetMovieId == static_cast<ulong>(userEntries[i][1])){
            UserEntry newUserEntry;
            ulong userId = static_cast<ulong>(userEntries[i][0]);
            //userId must not already exist in userEntries.
            if(find_if(result.begin(),result.end(),[&userId](UserEntry& usrEntry){ return usrEntry.userId == userId;}) != result.end()){
                continue;
            }
            newUserEntry.userId = userId;
            newUserEntry.rating = userEntries[i][2];
            if(newUserEntry.rating > 0){
                newUserEntry.rated = true;
                currentMovieRatings.push_back(userEntries[i][2]);
            }
            newUserEntry.tags = createTagEntriesOfMovieEntry(userEntries[i],tags,tags_rowsize);
            if(newUserEntry.tags.size() > 0){
                newUserEntry.tagged = true;
            }
            result.push_back(newUserEntry);
        }
    }
    return result;
}

/*
 * Generates the 'movieEntries' vector for a given User. Also populates a given vector with the user's ratings ('currentUserRatings').
 * If the rating of a movieEntry is zero, it is not included in the user ratings vector.
*/
vector<MovieEntry> createMovieEntriesOfUser(uint targetUserId,float** userEntries,size_t rowsize,char** tags,size_t tags_rowsize,vector<double>& currentUserRatings){

    vector<MovieEntry> result;
    for(size_t i=0;i<rowsize;i++){
        if(targetUserId == static_cast<uint>(userEntries[i][0])){
            MovieEntry newMovieEntry;
            ulong movieId = static_cast<ulong>(userEntries[i][1]);
            //movieId must not already exist in movieEntries.
            if(find_if(result.begin(),result.end(),[&movieId](MovieEntry& mvEntry){ return mvEntry.movieId == movieId;}) != result.end()){
                continue;
            }
            newMovieEntry.movieId = movieId;
            newMovieEntry.rating = userEntries[i][2];
            if(newMovieEntry.rating > 0){
                newMovieEntry.rated = true;
               currentUserRatings.push_back(userEntries[i][2]);
            }
            newMovieEntry.tags = createTagEntriesOfMovieEntry(userEntries[i],tags,tags_rowsize);
            if(newMovieEntry.tags.size() > 0){
                newMovieEntry.tagged = true;
            }
            result.push_back(newMovieEntry);
         }
     }
    return result;
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

vector<string> createGenresOfMovie(Configuration* config,size_t movieIndex,char*** genreEntries,size_t genreEntries_rowsize){

    if(movieIndex >= genreEntries_rowsize){
        string exMsg = "MovieID does not have genre information stored in matrices.";
        throw DataTransformationException(exMsg);
    }
    set<string> resultSet;
    for(size_t j=0;j<config->maxGenreCount;j++){
        if(genreEntries[movieIndex][j][0] != 0){
            resultSet.insert(genreEntries[movieIndex][j]);
        }
    }
    vector<string> result(resultSet.begin(),resultSet.end());
    return result;
}

vector<TagEntry> constructTotalTagsOfMovie(Movie& mv){

    vector<UserEntry>& userEntries = mv.userEntries;
    vector<TagEntry>& totalTags = mv.totalTags;
    for(auto it=userEntries.begin();it!=userEntries.end();it++){
        vector<TagEntry> userTags = it->tags;
        for(auto it_inner=userTags.begin();it_inner!=userTags.end();it_inner++){
            ulong tagId = it_inner->tagId;
            //tag already exists in movie total tags.
            vector<TagEntry>::iterator tagIter = find_if(totalTags.begin(),totalTags.end(),[&tagId](TagEntry& tagEntry){return tagEntry.tagId == tagId;});
            if(tagIter != totalTags.end()){
                tagIter->tagCount++;
            } else{
                TagEntry newTagEntry;
                newTagEntry.tagId = it_inner->tagId;
                newTagEntry.tagName = it_inner->tagName;
                newTagEntry.tagCount = 1;
                totalTags.push_back(newTagEntry);
            }
        }
    }
    return totalTags;
}

//version where 'allUsers' information is supplied.
vector<double> getMeanRatingsOfCommonUsers(vector<ulong>& commonUsers,vector<User>& allUsers){

    vector<double> result;
    for(auto it = commonUsers.begin();it!=commonUsers.end();it++){
        ulong userId = *it;
        auto userIter = find_if(allUsers.begin(),allUsers.end(),[&userId](User& usr){return usr.userId == userId;});
        if(userIter == allUsers.end()){
            string exMsg = "Common User ID was unexpectedly not found in memory.";
            throw DataTransformationException(exMsg);
        }
        result.push_back(userIter->meanRating);
    }
    return result;
}

} //NAMESPACE_END


//

// MAIN FUNCTIONS

//

double createUsersFromCsv(Configuration* config,float** userEntries,size_t usr_rowsize,char** tags,size_t tags_rowsize,vector<User>& users){

    //TODO: is sorting userEntries beneficial for performance?

    CustomSteadyTimer createUsersTimer;
    createUsersTimer.markStartPoint();

    cout<<"--Creating user data..."<<flush;

    size_t beforeSize = users.size();

    int loglevel = USE_HARDCODED_LOGLEVEL? LOGLEVEL : config->dataProceduresLogLevel;

    RecommenderLogger LOG;
    LOG.init("DataTransformations:createUsersFromCsv",loglevel);
    LOG.info("START: createUsersFromCsv");

    //STAGE A: for every user: calculate all parameters except of neighbors.
    LOG.debug("STAGE A: calculating all parameters (except neighbors) for each user.");
    unordered_set<ulong>  processedUsers;
    for(size_t i=0;i<usr_rowsize;i++){

        ulong currentUserId = static_cast<ulong>(userEntries[i][0]);

        //0: check if not already processed :if already processed,skip to next user.
        /*
        if(find_if(users.begin(),users.end(),[&currentUserId](User& usr){return usr.userId == currentUserId;}) != users.end()){
            continue;
        }
        */
        if(processedUsers.find(currentUserId) != processedUsers.end()){
            continue;
        }
        LOG.trace("Processing userID="+to_string(currentUserId));

        User newUser;
        newUser.userId = currentUserId;

        //1:collect all the user's movie entries (ratings+tags).
        vector<double> currentUserRatingsVector;
        newUser.movieEntries = dataTransform::createMovieEntriesOfUser(currentUserId,userEntries,usr_rowsize,tags,tags_rowsize,currentUserRatingsVector);

        //2:calculate their mean rating and rating standard deviation.
        double* currentUserRatingsPointer = &currentUserRatingsVector[0];
        newUser.meanRating = calculateMean(currentUserRatingsPointer,currentUserRatingsVector.size());
        newUser.ratingsStandardDeviation = calculateStandardDeviation(currentUserRatingsPointer,currentUserRatingsVector.size());
        users.push_back(newUser);
        processedUsers.insert(currentUserId);
     }

    //STAGE B: for every user: calculate neighbors.
    LOG.debug("STAGE B: calculating neighbors for each user.");
    for(size_t i=0;i<users.size();i++){
        User& currentUser = users[i];
        //for every OTHER user:
        for (size_t j = i+1; j < users.size(); j++){

            User& otherUser = users[j];
            NeighborUser currentUserNeighbor; // === the other user.
            NeighborUser otherUserNeighbor; // === the current user.

            //1: find their common items,the ratings and the mean rating of the users for these common items.
            vector<ulong> commonItemsIds = dataTransform::findCommonMovieIdsBetweenUsers(currentUser,otherUser);
            if(commonItemsIds.size() < config->minimumCommonElementsForUserNeighbors){
                continue;
            }
            sort(commonItemsIds.begin(),commonItemsIds.end(),ulongComparator);
            vector<double> currentUserRatingSubset = dataTransform::getSubsetOfUserRatingsByMovieIds(currentUser,commonItemsIds);
            vector<double> otherUserRatingSubset = dataTransform::getSubsetOfUserRatingsByMovieIds(otherUser,commonItemsIds);
            double* currentUserRatingSubsetPointer = &currentUserRatingSubset[0];
            double* otherUserRatingSubsetPointer = &otherUserRatingSubset[0];
            double currentUserRatingSubsetMean = calculateMean(currentUserRatingSubsetPointer,currentUserRatingSubset.size());
            double otherUserRatingSubsetMean = calculateMean(otherUserRatingSubsetPointer,otherUserRatingSubset.size());

            //2: calculate idf for every common item
            size_t userCount = users.size();
            vector<double> inverseFrequenciesCommon;
            size_t itemRatingCount;
            for (auto it=commonItemsIds.begin();it!=commonItemsIds.end();it++){
                itemRatingCount = dataTransform::calculateItemRatingCount(users,*it);
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
            otherUserNeighbor.id = currentUser.userId;
            otherUserNeighbor.weight = weight;
            otherUserNeighbor.commonMoviesIds = commonItemsIds;

            currentUser.neighbors.push_back(currentUserNeighbor);
            otherUser.neighbors.push_back(otherUserNeighbor);

        }
        //final step:
        //keep only the closest neighbors for the current user.
        dataTransform::filterNeighborsOfUser(config,currentUser);
    }
    LOG.info("createUsersFromCsv finished.");
    LOG.close();
    size_t afterSize = users.size();
    size_t newUserCount = afterSize - beforeSize;
    double timeElapsed = createUsersTimer.markEndPointAndGetTimeElapsed();
    cout<<"DONE. Created "<<newUserCount<<" users. Time elapsed="<<timeElapsed<<endl;
    return timeElapsed;
}

//version where all User information is supplied.
double createMoviesFromCsv(Configuration* config,float** userEntries,size_t usr_rowsize,char*** movies,size_t movies_rowsize,char** tags,size_t tags_rowsize,char*** genreEntries,size_t genreEntries_rowsize,vector<User>& allUsers,vector<Movie>& moviesVec){

    CustomSteadyTimer  createMoviesTimer;
    createMoviesTimer.markStartPoint();

    cout<<"--Creating movie data.."<<flush;

    size_t beforeSize = moviesVec.size();

    int loglevel = USE_HARDCODED_LOGLEVEL? LOGLEVEL : config->dataProceduresLogLevel;

    RecommenderLogger LOG;
    LOG.init("DataTransformations:createMoviesFromCsv",loglevel);
    LOG.info("START: createMoviesFromCsv");

    //STAGE A: for every movie: calculate all parameters except of neighbors.
    LOG.debug("STAGE A: calculating all parameters (except neighbors) for each movie.");
    set<ulong> processedMovies;
    for(size_t i=0;i<movies_rowsize;i++){
        ulong currentMovieId = strtoul(movies[i][0],nullptr,0);
        if(currentMovieId == 0){
            string exMsg = "Could not convert data for movieID ="+string(movies[i][0]);
            throw DataTransformationException(exMsg);
        }
        /*
        if(find_if(moviesVec.begin(),moviesVec.end(),[&currentMovieId](Movie& mv){return mv.movieId == currentMovieId;}) != moviesVec.end()){
            continue;
        }
        */
        if(processedMovies.find(currentMovieId) != processedMovies.end()){
            continue;
        }

        LOG.trace("Processing movieID="+to_string(currentMovieId));

        Movie newMovie;
        newMovie.movieId = currentMovieId;
        newMovie.movieName = movies[i][1];
        //1:collect all the movie genres
        newMovie.genres = dataTransform::createGenresOfMovie(config,i,genreEntries,genreEntries_rowsize);

        //2:collect all user entries of movie
        vector<double> currentMovieRatingsVector;
        newMovie.userEntries = dataTransform::createUserEntriesOfMovie(currentMovieId,userEntries,usr_rowsize,tags,tags_rowsize,currentMovieRatingsVector);

        //3:calculate their mean rating and rating standard deviation.
        double* currentMovieRatingsPointer = &currentMovieRatingsVector[0];
        newMovie.meanRating = calculateMean(currentMovieRatingsPointer,currentMovieRatingsVector.size());
        newMovie.ratingsStandardDeviation = calculateStandardDeviation(currentMovieRatingsPointer,currentMovieRatingsVector.size());

        //4:calculate total tags
        newMovie.totalTags = dataTransform::constructTotalTagsOfMovie(newMovie);

        moviesVec.push_back(newMovie);
    }
    //STAGE B: for every movie: calculate neighbors.
    LOG.debug("STAGE B: calculating neighbors for each movie.");
    for(size_t i=0;i<moviesVec.size();i++){
        Movie& currentMovie = moviesVec[i];
        for(size_t j=i+1;j<moviesVec.size();j++){

            Movie& otherMovie = moviesVec[j];
            NeighborMovie currentMovieNeighbor; // === the other movie.
            NeighborMovie otherMovieNeighbor; // === the current movie.

            //1: find their common users,the ratings and the mean rating of these common users.
           vector<ulong> commonUsersIds = dataTransform::findCommonUserIdsBetweenMovies(currentMovie,otherMovie);
           if(commonUsersIds.size() < config->minimumCommonElementsForMovieNeighbors){
               continue;
           }
           sort(commonUsersIds.begin(),commonUsersIds.end(),ulongComparator);
           vector<double> commonUsersMeanRatings = dataTransform::getMeanRatingsOfCommonUsers(commonUsersIds,allUsers);
           vector<double> currentMovieRatingSubset = dataTransform::getSubsetOfMovieRatingsByUserIds(currentMovie,commonUsersIds);
           vector<double> otherMovieRatingSubset = dataTransform::getSubsetOfMovieRatingsByUserIds(otherMovie,commonUsersIds);
           double* currentMovieRatingSubsetPointer = &currentMovieRatingSubset[0];
           double* otherMovieRatingSubsetPointer = &otherMovieRatingSubset[0];
           double* commonUsersMeanRatingsPointer = &commonUsersMeanRatings[0];

            //2: calculate idf for every common user
           vector<double> inverseFrequenciesCommon;
           size_t itemCount = moviesVec.size();
           for (auto it=commonUsersIds.begin();it!=commonUsersIds.end();it++){
               size_t userRatingCount = dataTransform::calculateUserRatingCount(moviesVec,*it);
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
           otherMovieNeighbor.id = currentMovie.movieId;
           otherMovieNeighbor.weight = weight;
           otherMovieNeighbor.commonUsersIds = commonUsersIds;

           currentMovie.neighbors.push_back(currentMovieNeighbor);
           otherMovie.neighbors.push_back(otherMovieNeighbor);

        }
        //final step:
        //keep only the closest neighbors for the current movie.
        dataTransform::filterNeighborsOfMovie(config,currentMovie);
    }
    LOG.info("createMoviesFromCsv finished.");
    LOG.close();
    size_t afterSize = moviesVec.size();
    size_t newMovieCount = afterSize - beforeSize;
    double timeElapsed = createMoviesTimer.markEndPointAndGetTimeElapsed();
    cout<<"DONE. Created "<<newMovieCount<<" movies. Time elapsed="<<timeElapsed<<endl;
    return timeElapsed;
}

void appendUsersFromCsv(){}
void appendMoviesFromCsv(){}
void createUsersFromJson(){}
void appendUsersFromJson(){}
