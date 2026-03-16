#include "testUtils.h"
#include "../src/data/dtos.h"
#include "../src/application/configuration/configuration.h"
#include "../src/math/mathUtils.h"
#include "../src/logging/recommenderLogger.h"
#include "../src/main/movieRecommenderResult.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <algorithm>
typedef unsigned int uint;
typedef unsigned long ulong;
using namespace std;

void reportIfError(bool result,string message,RecommenderLogger& LOG){
    if(!result){
        LOG.error(message);
    }
}

bool userIdComparator(User a,User b){
    return (a.userId < b.userId);
}

bool userIdEntryComparator(UserEntry a, UserEntry b){
    return (a.userId < b.userId);
}

bool userIdNeighborComparator(NeighborUser a, NeighborUser b){
    return (a.id < b.id);
}

bool movieIdComparator(Movie a, Movie b){
    return (a.movieId < b.movieId);
}

bool movieIdEntryComparator(MovieEntry a,MovieEntry b){
    return (a.movieId < b.movieId);
}

bool movieIdNeighborComparator(NeighborMovie a, NeighborMovie b){
    return (a.id < b.id);
}

bool tagIdComparator(TagEntry a,TagEntry b){
    return (a.tagId < b.tagId);
}

bool recommendationMovieIdComparator(MovieRecommendation a, MovieRecommendation b){
    return (a.movieId < b.movieId);
}

bool userProfileComparator(UserProfileFull a,UserProfileFull b){
    return (a.getUserId() < b.getUserId());
}

void printCommonIds(vector<ulong> common){
    cout<<"commonIds:"<<endl;
    for(auto it=common.begin(); it !=common.end(); it++){
        cout<<(*it)<<endl;
    }
}

void printNeighborUser(NeighborUser neighborUser){
    cout<<"id: "<<neighborUser.id<<"  -  "<<"weight: "<<neighborUser.weight<<"  -  ";
    printCommonIds(neighborUser.commonMoviesIds);
}

void printNeighborUsers(vector<NeighborUser> neighbors){
    for(auto it = neighbors.begin(); it!= neighbors.end(); it++){
        printNeighborUser(*it);
    }
}

void printNeighborMovie(NeighborMovie neighborMovie){
    cout<<"id: "<<neighborMovie.id<<"  -  "<<"weight: "<<neighborMovie.weight<<"  -  ";
    printCommonIds(neighborMovie.commonUsersIds);
}

void printNeighborMovies(vector<NeighborMovie> neighborMovies){

    for(auto it=neighborMovies.begin();it!=neighborMovies.end();it++){
        printNeighborMovie(*it);
    }
}

void printTagEntry(TagEntry tagEntry){
    cout << "tagId:" << tagEntry.tagId << "  -  " << "tagName:" << tagEntry.tagName << "  -  " << "tagCount:" << tagEntry.tagCount << endl;
}

void printTagEntries(vector<TagEntry> tagEntries){
    for (auto it = tagEntries.begin(); it != tagEntries.end(); it++){
        printTagEntry(*it);
    }
}

void printUserEntry(UserEntry userEntry){
    cout << "userId: " << userEntry.userId << "  -  " << "rating: " << userEntry.rating << "  -  " << "tags: \n";
    printTagEntries(userEntry.tags);
}

void printUserEntries(vector<UserEntry> userEntries){
    for (auto it = userEntries.begin(); it != userEntries.end(); it++){
        printUserEntry(*it);
    }
}

void printMovieEntry(MovieEntry movieEntry){

    cout << "movieId: " << movieEntry.movieId << "  -  " << "rating: " << movieEntry.rating << "  -  " << "tags: \n";
    printTagEntries(movieEntry.tags);
}

void printMovieEntries(vector<MovieEntry> movieEntries){
    for(auto it = movieEntries.begin(); it!= movieEntries.end(); it++){
        printMovieEntry(*it);
    }
}

void printUser(User user){

    cout<<"userId: "<<user.userId<<"  -  "<<"meanRating: "<<user.meanRating<<"  -  "<<"ratingStdDev "<<user.ratingsStandardDeviation;
    cout<<"  -  "<<"movieEntries: \n";
    printMovieEntries(user.movieEntries);
    cout<<"  -  "<<"neighbors: \n";
    printNeighborUsers(user.neighbors);
}

void printUsers(vector<User> users){

    for(auto it = users.begin(); it != users.end(); it++){
        printUser(*it);
    }
}

void printMovie(Movie movie){

    cout<<"movieId: "<<movie.movieId<<"  -  "<<"movieName:"<<movie.movieName<<"  -  "<<"meanRating: "<<movie.meanRating<<"  -  "<<"ratingStdDev "<<movie.ratingsStandardDeviation;
    cout<<"  -  "<<"userEntries: \n";
    printUserEntries(movie.userEntries);
    cout<<"  -  "<<"neighbors: \n";
    printNeighborMovies(movie.neighbors);
    cout<<"  -  "<<"genres: \n";
    printStrVector(movie.genres,",");
    cout<<" - "<<"totalTags: \n";
    printTagEntries(movie.totalTags);
}

void printMovies(vector<Movie> movies){

    for(auto it=movies.begin();it!=movies.end();it++){
        printMovie(*it);
    }
}

void printAllUsers(vector<User>& allUsers){

    cout<<"ALL USERS:"<<endl;
    printUsers(allUsers);
}

void printAllMovies(vector<Movie>& allMovies){

    cout<<"ALL MOVIES:"<<endl;
    printMovies(allMovies);
}

void printStrVector(vector<string> vec,string separator){

    for(unsigned int i=0;i<vec.size();i++){
        cout<<vec[i]<<separator;
    }
    cout<<endl;
}

void printRecommendations(vector<MovieRecommendation> recommendations,bool minimal){

    if(recommendations.size() < 1){
        cout<<"none."<<endl;
        return;
    }

    if(minimal){
        for(unsigned int i=0;i<recommendations.size();i++){
            cout<<"--"<<endl;
            printRecommendation(recommendations[i],minimal);
            cout<<"--"<<endl;
        }
        return;
    }

    //TODO

}

void printRecommendation(MovieRecommendation rec,bool minimal){

    if(minimal){
        cout<<"movieID: "<<rec.movieId<<endl;
        cout<<"movieName: "<<rec.movieName<<endl;
        cout<<"genres: ";
        printStrVector(rec.genres," * ");
        cout<<"mostCommonTags: ";
        printTagEntries(rec.mostCommonTags);
        cout<<endl;
        cout<<"meanRating: "<<rec.meanRating<<endl;
        return;
    }

   //TODO
}

void writeStrVectorsToFile(vector<string> vec,ofstream& f){
    f<<"\n";
    for(auto it = vec.begin();it!=vec.end();it++){
        f<<"-\n";
        f<<*it<<"\n";
    }
}

void writeCommonIdsToFile(vector<ulong> common,ofstream& f){

    f<<"commonIds:\n";
    for(auto it=common.begin(); it !=common.end(); it++){
        f<<(*it);
    }
    f<<"\n";
    f.flush();
}
void writeNeighborUserToFile(NeighborUser neighbor,ofstream& f){

    f<<"id: "<<neighbor.id<<"  -  "<<"weight: "<<neighbor.weight<<"  -  \n";
    writeCommonIdsToFile(neighbor.commonMoviesIds,f);
}
void writeNeighborUsersToFile(vector<NeighborUser> neighbors,ofstream& f){
    for(auto it = neighbors.begin(); it!= neighbors.end(); it++){
        writeNeighborUserToFile(*it,f);
    }
}
void writeNeighborMovieToFile(NeighborMovie neighbor,ofstream& f){

    f<<"id: "<<neighbor.id<<"  -  "<<"weight: "<<neighbor.weight<<"  -  \n";
    writeCommonIdsToFile(neighbor.commonUsersIds,f);
}
void writeNeighborMoviesToFile(vector<NeighborMovie> neighbors,ofstream& f){
    for(auto it = neighbors.begin(); it!= neighbors.end(); it++){
        writeNeighborMovieToFile(*it,f);
    }
}
void writeTagEntryToFile(TagEntry tagEntry,ofstream& f){
    f << "tagId:" << tagEntry.tagId << "  -  " << "tagName:" << tagEntry.tagName << "  -  " << "tagCount:" << tagEntry.tagCount << "\n";
    f.flush();
}
void writeTagEntriesToFile(vector<TagEntry> tagEntries,ofstream& f){
    for (auto it = tagEntries.begin(); it != tagEntries.end(); it++){
        writeTagEntryToFile(*it,f);
    }
}
void writeUserEntryToFile(UserEntry userEntry,ofstream& f){
    f << "userId: " << userEntry.userId << "  -  " << "rating: " << userEntry.rating << "  -  " << "tags: \n";
    writeTagEntriesToFile(userEntry.tags,f);
}
void writeUserEntriesToFile(vector<UserEntry> userEntries,ofstream& f){
    for (auto it = userEntries.begin(); it != userEntries.end(); it++){
        writeUserEntryToFile(*it,f);
    }
}
void writeMovieEntryToFile(MovieEntry movieEntry,ofstream& f){
    f << "movieId: " << movieEntry.movieId << "  -  " << "rating: " << movieEntry.rating << "  -  " << "tags: \n";
    writeTagEntriesToFile(movieEntry.tags,f);
}
void writeMovieEntriesToFile(vector<MovieEntry> movieEntries,ofstream& f){
    for(auto it = movieEntries.begin(); it!= movieEntries.end(); it++){
        writeMovieEntryToFile(*it,f);
    }
}
void writeUserToFile(User user,ofstream& f){
    f<<"\nuserId: "<<user.userId<<"  -  "<<"meanRating: "<<user.meanRating<<"  -  "<<"ratingStdDev "<<user.ratingsStandardDeviation;
    f<<"  -  "<<"movieEntries: \n";
    writeMovieEntriesToFile(user.movieEntries,f);
    f<<"  -  "<<"neighbors: \n";
    writeNeighborUsersToFile(user.neighbors,f);
    f.flush();
}
void writeUsersToFile(vector<User> users,ofstream& f){
    for(auto it = users.begin(); it != users.end(); it++){
        f<<"\n";
        writeUserToFile(*it,f);
    }
}
void writeMovieToFile(Movie movie,ofstream& f){
    f<<"\nmovieId: "<<movie.movieId<<"  -  "<<"meanRating: "<<movie.meanRating<<"  -  "<<"ratingStdDev "<<movie.ratingsStandardDeviation;
    f<<"  -  "<<"userEntries: \n";
    writeUserEntriesToFile(movie.userEntries,f);
    f<<"  -  "<<"neighbors: \n";
    writeNeighborMoviesToFile(movie.neighbors,f);
    f<<" - "<<" totalTags: \n";
    writeTagEntriesToFile(movie.totalTags,f);
    f<<" - "<<" genres: \n";
    writeStrVectorsToFile(movie.genres,f);
}
void writeMoviesToFile(vector<Movie> movies,ofstream& f){
    for(auto it=movies.begin();it!=movies.end();it++){
        f<<"\n";
        writeMovieToFile(*it,f);
    }
}

void writeAllUsersToFile(vector<User>& allUsers,ofstream& f){

    f<<"ALL USERS\n";
    writeUsersToFile(allUsers,f);
}

void writeAllMoviesToFile(vector<Movie>& allMovies,ofstream& f){

    f<<"ALL MOVIES\n";
    writeMoviesToFile(allMovies,f);
}

void writeMovieRecommendationToFile(MovieRecommendation rec,ofstream& f,bool minimal){

    if(minimal){
        f<<"\n---";
        f<<"\nrecommendationType(0->user,1->item): "<<rec.recommendationType;
        f<<"\nmovieId: "<<rec.movieId;
        f<<"\nmovieName:"<<rec.movieName;
        f<<"\nmostCommonTags: \n";
        writeTagEntriesToFile(rec.mostCommonTags,f);
        f<<"\nmeanRating: "<<rec.meanRating;
        return;
    }

    f<<"\n---";
    f<<"\nrecommendationType(0->user,1->item): "<<rec.recommendationType;
    f<<"\npredictedRating: "<<rec.predictedRating;
    f<<"\npredictedRatingNormalized: "<<rec.predictedRatingNormalized;
    f<<"\n-";
    f<<"\nmovieId: "<<rec.movieId;
    f<<"\nmostCommonTags: \n";
    writeTagEntriesToFile(rec.mostCommonTags,f);
    f<<"\nmeanRating: "<<rec.meanRating;
    if(rec.recommendationType == '0'){
        f<<"\nmeanRatingFromNeighbors: "<<rec.meanRatingFromNeighbors;
    } else{
        f<<"\nsimilarWatchedMovies: ";
        writeStrVectorsToFile(rec.similarWatchedMovies,f);
    }

    f<<"\nneighborsUsedCount: "<<rec.neighborsUsedCount;
    f.flush();
}

void writeMovieRecommendationsToFile(vector<MovieRecommendation> recommendations,ofstream& f,bool minimal){
    f<<"--Movie Recommendations--\n";
    if(recommendations.size() == 0){
        f<<"NONE\n";
    }
    for(auto it=recommendations.begin();it!=recommendations.end();it++){
        writeMovieRecommendationToFile(*it,f,minimal);
        f<<"\n";
    }
}

void writeMovieRecommendationWrapperToFile(MovieRecommendationsWrapper w,ofstream& f){
    f<<"--MovieRecommendationResult--\n";
    f<<"--Recommendations from user-based method--\n";
    writeMovieRecommendationsToFile(w.recommendedFromUserMethod,f);
    f<<"--Recommendations from item-based method--\n";
    writeMovieRecommendationsToFile(w.recommendedFromItemMethod,f);
    f<<"\n";
}
void writeMovieRecommendationWrappersToFile(vector<MovieRecommendationsWrapper> vw , ofstream& f){
    f<<"**MOVIE RECOMMENDATION RESULTS**\n";
    for(auto it=vw.begin();it!=vw.end();it++){
        writeMovieRecommendationWrapperToFile(*it,f);
    }
}

bool commonIdVectorsEqual(vector<ulong> commonIds1,vector<ulong> commonIds2){

    RecommenderLogger LOG;
    LOG.init("testUtils:commonIdVectorsEqual");

    bool result;
    result = commonIds1.size() == commonIds2.size();
    if(!result){
        LOG.error("commonIds sizes are not equal! size1="+to_string(commonIds1.size())+", size2="+to_string(commonIds2.size()));
        return result;
    }
    sort(commonIds1.begin(),commonIds1.end());
    sort(commonIds2.begin(),commonIds2.end());
    for(size_t i=0; i<commonIds1.size();i++){
        result = result && (commonIds1[i] == commonIds2[i]);
    }
    if(!result){
        LOG.error("commonIds values are not equal!");
    }

    LOG.close();
    return result;
}

bool tagEntriesEqual(TagEntry tagEntry1 ,TagEntry tagEntry2){

    RecommenderLogger LOG;
    LOG.init("testUtils:tagEntriesEqual");

    bool result;
    bool resultTagId = tagEntry1.tagId == tagEntry2.tagId;
    reportIfError(resultTagId,"tagIds of tags are not equal!",LOG);
    bool resultTagName = (tagEntry1.tagName == tagEntry2.tagName);
    reportIfError(resultTagName,"tagNames of tags are not equal!",LOG);
    bool resultTagCount = (tagEntry1.tagCount == tagEntry2.tagCount);
    reportIfError(resultTagCount,"tagCounts of tags are not equal!",LOG);

    result = resultTagId && resultTagName && resultTagCount;

    if(!result){
        LOG.error("tagEntries are not equal!");
    }

    LOG.close();
    return result;
}

bool tagEntryVectorsEqual(vector<TagEntry> tagEntries1, vector<TagEntry> tagEntries2){

    RecommenderLogger LOG;
    LOG.init("testUtils:tagEntryVectorsEqual");

    sort(tagEntries1.begin(),tagEntries1.end(),tagIdComparator);
    sort(tagEntries2.begin(),tagEntries2.end(),tagIdComparator);

    bool result = tagEntries1.size() == tagEntries2.size();

    if(!result){
        LOG.error("tagEntryVectors are not equal in size! size1="+to_string(tagEntries1.size())+",size2="+to_string(tagEntries2.size()));
        return result;
    }

    for (size_t i = 0; i < tagEntries1.size();i++){
        result = result && tagEntriesEqual(tagEntries1[i],tagEntries2[i]);
    }

    if(!result){
        LOG.error("tagEntryVectors are not equal!");
    }

    LOG.close();
    return result;
}

bool userEntriesEqual(UserEntry userEntry1, UserEntry userEntry2){

    RecommenderLogger LOG;
    LOG.init("testUtils:userEntriesEqual");

    bool result;
    bool resultUserId = userEntry1.userId == userEntry2.userId;
    reportIfError(resultUserId,"userIds of userEntries are not equal! userId1=+"+to_string(userEntry1.userId)+", userId2="+to_string(userEntry2.userId),LOG);
    if(resultUserId){
        LOG.info("Checking userEntries of ID="+to_string(userEntry1.userId));
    }
    bool resultRatings = (realNumEqualWithinError(userEntry1.rating,userEntry2.rating,0.00001));
    reportIfError(resultRatings,"ratings of userEntries are not equal! rating1=+"+to_string(userEntry1.rating)+", rating2="+to_string(userEntry2.rating),LOG);
    bool resultTagSize = (userEntry1.tags.size() == userEntry2.tags.size());
    reportIfError(resultTagSize,"tag sizes of tagEntries of userEntries are not equal! size1="+to_string(userEntry1.tags.size())+" , size2="+to_string(userEntry2.tags.size()),LOG);
    bool resultTagEntries = tagEntryVectorsEqual(userEntry1.tags,userEntry2.tags);
    reportIfError(resultTagEntries,"tagEntries of userEntries are not equal!",LOG);

    result = resultUserId && resultRatings && resultTagSize && resultTagEntries;

    if(!result){
        LOG.error("userEntries are not equal!");
    }

    LOG.close();
    return result;
}

bool userEntryVectorsEqual(vector<UserEntry> userEntries1,vector<UserEntry> userEntries2){

    RecommenderLogger LOG;
    LOG.init("testUtils:userEntryVectorsEqual");

    bool result = userEntries1.size() == userEntries2.size();

    if(!result){
        LOG.error("userEntryVector sizes are not equal! size1="+to_string(userEntries1.size())+" ,size2="+to_string(userEntries2.size()));
        return result;
    }

    for(size_t i=0; i<userEntries1.size();i++){
        result = result && userEntriesEqual(userEntries1[i],userEntries2[i]);
    }

    if(!result){
        LOG.error("userEntry vectors are not equal!");
    }

    LOG.close();
    return result;
}

bool movieEntriesEqual(MovieEntry movieEntry1, MovieEntry movieEntry2){

    RecommenderLogger LOG;
    LOG.init("testUtils:movieEntriesEqual");

    bool result;
    bool resultMovieId = movieEntry1.movieId == movieEntry2.movieId;
    reportIfError(resultMovieId,"movieIds of movies are not equal!",LOG);
    bool resultRating = (realNumEqualWithinError(movieEntry1.rating,movieEntry2.rating,0.00001));
    reportIfError(resultRating,"ratings of movies are not equal!",LOG);

    result = (movieEntry1.tags.size() == movieEntry2.tags.size());
    if(!result){
        LOG.error("tag vector sizes of movieEntries are not equal! size1="+to_string(movieEntry1.tags.size())+",size2="+to_string(movieEntry2.tags.size()));
    }
    result = result && tagEntryVectorsEqual(movieEntry1.tags,movieEntry2.tags);

    result = result && resultMovieId && resultRating;

    if(!result){
        LOG.error("movieEntries are not equal!");
    }

    LOG.close();
    return result;
}

bool movieEntryVectorsEqual(vector<MovieEntry> movieEntries1, vector<MovieEntry> movieEntries2){

    RecommenderLogger LOG;
    LOG.init("testUtils:movieEntryVectorsEqual");

    bool result;
    result = movieEntries1.size() == movieEntries2.size();

    if(!result){
        LOG.error("movieEntryVectors are not equal!");
        return result;
    }

    sort(movieEntries1.begin(),movieEntries1.end(),movieIdEntryComparator);
    sort(movieEntries2.begin(),movieEntries2.end(),movieIdEntryComparator);

    for (size_t i = 0; i < movieEntries1.size(); i++){
        result = result && movieEntriesEqual(movieEntries1[i], movieEntries2[i]);
    }

    if(!result){
        LOG.error("movieEntryVectors are not equal!");
    }

    LOG.close();
    return result;
}

bool neighborUsersEqual(NeighborUser neighborUser1,NeighborUser neighborUser2){

    RecommenderLogger LOG;
    LOG.init("testUtils:neighborUsersEqual");

    bool result;
    bool resultUserId = neighborUser1.id == neighborUser2.id;
    reportIfError(resultUserId,"ids of neighborUsers are not equal! id1="+to_string(neighborUser1.id)+", id2="+to_string(neighborUser2.id),LOG);
    bool resultWeight = (realNumEqualWithinError(neighborUser1.weight,neighborUser2.weight,0.00001));
    reportIfError(resultWeight,"weights of neighborUsers are not equal! weight1="+to_string(neighborUser1.weight)+",weight2="+to_string(neighborUser2.weight),LOG);

    result = resultUserId && resultWeight;

    result = result && (commonIdVectorsEqual(neighborUser1.commonMoviesIds,neighborUser2.commonMoviesIds));

    if(!result){
        LOG.error("neighborUsers are not equal!");
    }

    LOG.close();
    return result;
}

bool neighborMoviesEqual(NeighborMovie neighborMovie1,NeighborMovie neighborMovie2){

    RecommenderLogger LOG;
    LOG.init("testUtils:neighborMoviesEqual");

    bool result;
    bool resultMovieId = neighborMovie1.id == neighborMovie2.id;
    reportIfError(resultMovieId,"ids of neighborMovies are not equal!",LOG);
    bool resultWeight = (realNumEqualWithinError(neighborMovie1.weight,neighborMovie2.weight,0.00001));
    reportIfError(resultWeight,"weights of neighborMovies are not equal!",LOG);

    result = resultMovieId && resultWeight;

    result = result && (commonIdVectorsEqual(neighborMovie1.commonUsersIds,neighborMovie2.commonUsersIds));

    if(!result){
        LOG.error("neighbor movies are not equal!");
    }

    LOG.close();
    return result;
}

bool neighborUserVectorsEqual(vector<NeighborUser> neighborUsers1,vector<NeighborUser> neighborUsers2){

    RecommenderLogger LOG;
    LOG.init("testUtils:neighborUserVectorsEqual");

    bool result;
    result = neighborUsers1.size() == neighborUsers2.size();
    if(!result){
        LOG.error("neighborUserVectors are not equal in size! size1="+to_string(neighborUsers1.size())+",size2="+to_string(neighborUsers2.size()));
        return result;
    }

    sort(neighborUsers1.begin(),neighborUsers1.end(),userIdNeighborComparator);
    sort(neighborUsers2.begin(),neighborUsers2.end(),userIdNeighborComparator);

    for(size_t i=0; i<neighborUsers1.size(); i++){
        result = result && neighborUsersEqual(neighborUsers1[i],neighborUsers2[i]);
    }

    if(!result){
        LOG.error("neighborUserVectors are not equal!");
    }

    LOG.close();
    return result;
}

bool neighborMovieVectorsEqual(vector<NeighborMovie> neighborMovies1,vector<NeighborMovie> neighborMovies2){

    RecommenderLogger LOG;
    LOG.init("testUtils:neighborMovieVectorsEqual");

    bool result;
    result = neighborMovies1.size() == neighborMovies2.size();

    sort(neighborMovies1.begin(),neighborMovies1.end(),movieIdNeighborComparator);
    sort(neighborMovies2.begin(),neighborMovies2.end(),movieIdNeighborComparator);

    for(size_t i=0;i<neighborMovies1.size();i++){
        result = result && neighborMoviesEqual(neighborMovies1[i],neighborMovies2[i]);
    }

    if(!result){
        LOG.error("neighborMovieVectors are not equal!");
    }

    LOG.close();
    return result;
}

bool userVectorsEqual(vector<User> users1,vector<User> users2){

    RecommenderLogger LOG;
    LOG.init("testUtils:userVectorsEqual");

    bool result;
    result = users1.size() == users2.size();

    if(!result){
        LOG.error("userVectors are not equal!");
        return result;
    }

    //sort vectors before comparing.
    sort(users1.begin(),users1.end(),userIdComparator);
    sort(users2.begin(),users2.end(),userIdComparator);

    for (size_t i = 0; i < users1.size(); i++){
        result = result && usersEqual(users1[i],users2[i]);
    }

    if(!result){
        LOG.error("userVectors are not equal!");
    }

    LOG.close();
    return result;
}

bool usersEqual(User user1,User user2){
    RecommenderLogger LOG;
    LOG.init("testUtils:usersEqual");
    LOG.debug("Starting: usersEqual");
    bool result;
    bool resultUserIds = user1.userId == user2.userId;
    reportIfError(resultUserIds,"userIds of users are not equal! id1="+to_string(user1.userId)+",id2="+to_string(user2.userId),LOG);
    if(resultUserIds){
        LOG.info("Checking users of ID="+to_string(user1.userId));
    }
    bool resultMeanRatings = (realNumEqualWithinError(user1.meanRating,user2.meanRating,0.00001));
    reportIfError(resultMeanRatings,"meanRatings of users are not equal! meanRating1="+to_string(user1.meanRating)+", meanRating2="+to_string(user2.meanRating),LOG);
    bool resultStdDev = (realNumEqualWithinError(user1.ratingsStandardDeviation,user2.ratingsStandardDeviation,0.00001));
    reportIfError(resultStdDev,"ratingStandardDeviation of users are not equal! rStdDev1="+to_string(user1.ratingsStandardDeviation)+", rStdDev2="+to_string(user2.ratingsStandardDeviation),LOG);
    bool resultMovEntrVec = movieEntryVectorsEqual(user1.movieEntries, user2.movieEntries);
    reportIfError(resultMovEntrVec,"movieEntries of users are not equal!",LOG);
    bool resultNeighborUserVec = neighborUserVectorsEqual(user1.neighbors, user2.neighbors);
    reportIfError(resultNeighborUserVec,"neighborUsers of users are not equal!",LOG);

    result = resultUserIds && resultMeanRatings && resultStdDev &&
            resultMovEntrVec &&  resultNeighborUserVec;

    if(!result){
        LOG.error("users are not equal!");
    }

    LOG.close();
    return result;
}

bool genresEqual(vector<string> genres1,vector<string> genres2){

    RecommenderLogger LOG;
    LOG.init("testUtils:genresEqual");

    bool result = genres1.size() == genres2.size();
    bool sizesEqual = result;

    sort(genres1.begin(),genres1.end());
    sort(genres2.begin(),genres2.end());
    if(sizesEqual){
        for(size_t i=0;i<genres1.size();i++){
            LOG.trace("Comparing "+genres1[i]+" with "+genres2[i]);
            result = result && (genres1[i] == genres2[i]);
        }
    }

    if(!result){
        LOG.error("genres are not equal!");
    }

    LOG.close();
    return result;
}

bool similarWatchedMoviesEqual(vector<string> similarW1, vector<string> similarW2){

    RecommenderLogger LOG;
    LOG.init("testUtils:similarWatchedMoviesEqual");
    bool result = similarW1.size() == similarW2.size();
    if(!result){
        LOG.error("sizes of similarWatchedMovies are not equal. \
        size1="+to_string(similarW1.size())+", size2="+to_string(similarW2.size()));
    }

    sort(similarW1.begin(),similarW1.end());
    sort(similarW2.begin(),similarW2.end());

    for(size_t i=0;i<similarW1.size();i++){
        LOG.trace("Comparing "+similarW1[i]+" with "+similarW2[i]);
        result = result && (similarW1[i] == similarW2[i]);
    }

    if(!result){
        LOG.error("similarWatchedMovies are not equal!");
    }

    LOG.close();
    return result;
}

bool moviesEqual(Movie movie1,Movie movie2){
    RecommenderLogger LOG;
    LOG.init("testUtils:moviesEqual");

    bool result;
    bool resultMovieId = movie1.movieId == movie2.movieId;
    reportIfError(resultMovieId,"movieIds of movies are not equal!",LOG);
    if(resultMovieId){
        LOG.info("Checking movies of ID="+to_string(movie1.movieId));
    }
    LOG.trace("comparing mean ratings.");
    bool resultMeanRating = (realNumEqualWithinError(movie1.meanRating,movie2.meanRating,0.00001));
    reportIfError(resultMeanRating,"meanRating of movies are not equal!",LOG);
    LOG.trace("comparing stdDev");
    bool resultStdDev = (realNumEqualWithinError(movie1.ratingsStandardDeviation,movie2.ratingsStandardDeviation,0.00001));
    reportIfError(resultStdDev,"ratingStdDev of movies are not equal!",LOG);
    LOG.trace("comparing user entries.");
    bool resultUserEntries = userEntryVectorsEqual(movie1.userEntries,movie2.userEntries);
    reportIfError(resultUserEntries,"userEntries of movies are not equal!",LOG);
    LOG.trace("comparing genres.");
    bool resultGenres = genresEqual(movie1.genres,movie2.genres);
    reportIfError(resultGenres,"genres of movies are not equal!",LOG);
    LOG.trace("comparing totalTags");
    bool resultTagEntries = tagEntryVectorsEqual(movie1.totalTags,movie2.totalTags);
    reportIfError(resultTagEntries,"tagEntries of movies are not equal!",LOG);
    LOG.trace("comparing neighbors.");
    bool resultNeighborMovies = neighborMovieVectorsEqual(movie1.neighbors,movie2.neighbors);
    reportIfError(resultNeighborMovies,"neighbors of movies are not equal!",LOG);

    result = resultMovieId && resultMeanRating && resultStdDev && resultUserEntries && resultGenres && resultTagEntries&& resultNeighborMovies;

    if(!result){
        LOG.error("movies are not equal!");
    }

    LOG.close();
    return result;
}

bool movieVectorsEqual(vector<Movie> movies1,vector<Movie> movies2){

    RecommenderLogger LOG;
    LOG.init("testUtils:movieVectorsEqual");

    bool result;
    result = movies1.size() == movies2.size();

    if(!result){
        LOG.error("movieVectors are not equal in size! size1="+to_string(movies1.size())+", size2="+to_string(movies2.size()));
        return result;
    }

    //sort vectors before comparing.
    sort(movies1.begin(),movies1.end(),movieIdComparator);
    sort(movies2.begin(),movies2.end(),movieIdComparator);

    for (size_t i = 0; i < movies1.size(); i++){
        result = result && moviesEqual(movies1[i],movies2[i]);
    }

    if(!result){
        LOG.error("movieVectors are not equal!");
    }

    LOG.close();
    return result;

}

bool recommendationsEqual(MovieRecommendation rec1,MovieRecommendation rec2,bool minimal){

    RecommenderLogger LOG;
    LOG.init("testUtils:recommendationsEqual");

    if(minimal){
        bool result;
        LOG.trace("Comparing movieIDs");
        bool resultMovieId = (rec1.movieId == rec2.movieId);
        reportIfError(resultMovieId,"movieIds of recommendations are not equal! movieId1="+to_string(rec1.movieId)+" ,movieId2="+to_string(rec2.movieId),LOG);
        LOG.trace("Comparing movie names.");
        bool resultMovieName = (rec1.movieName == rec2.movieName);
        reportIfError(resultMovieName,"movieNames of recommendations are not equal! movieName1="+rec1.movieName+" ,movieName2="+rec2.movieName,LOG);
        LOG.trace("Comparing genres.");
        bool resultGenres = genresEqual(rec1.genres,rec2.genres);
        reportIfError(resultGenres,"genres of recommendations are not equal!",LOG);
        LOG.trace("Comparing common tags.");
        bool resultMostCommonTags = tagEntryVectorsEqual(rec1.mostCommonTags,rec2.mostCommonTags);
        reportIfError(resultMostCommonTags,"mostCommonTags of recommendations are not equal!",LOG);
        LOG.trace("Comparing mean ratings.");
        bool resultMeanRating = realNumEqualWithinError(rec1.meanRating,rec2.meanRating,0.00001);
        reportIfError(resultMeanRating,"meanRating of recommendations are not equal! meanRating1="+to_string(rec1.meanRating)+" ,meanRating2="+to_string(rec2.meanRating),LOG);
        result = resultMovieId && resultMovieName && resultGenres && resultMostCommonTags && resultMeanRating;
        LOG.close();
        return result;
    }

    LOG.trace("Comparing predictedRatings");
    bool resultPredicted = realNumEqualWithinError(rec1.predictedRating,rec2.predictedRating,0.00001);
    reportIfError(resultPredicted,"predictedRating of recommendations are not equal! predictedRating1="+to_string(rec1.predictedRating)+", predictedRating2="+to_string(rec2.predictedRating),LOG);
    LOG.trace("Comparing predictedRatingsNormalized");
    bool resultPredictedNormalized = (realNumEqualWithinError(rec1.predictedRatingNormalized,rec2.predictedRatingNormalized,0.00001));
    reportIfError(resultPredictedNormalized,"predictedRatingNormalized of recommendations are not equal! predictedRatingNormalized1="+to_string(rec1.predictedRatingNormalized)+" , predictedRatingNormalized2="+to_string(rec2.predictedRatingNormalized),LOG);
    LOG.trace("Comparing currentWeightSums");
    bool resultCurrentWeightSum = (realNumEqualWithinError(rec1.currentWeightSum,rec2.currentWeightSum,0.00001));
    reportIfError(resultCurrentWeightSum,"currentWeightSum of recommendations are not equal! currentWeightSum1="+to_string(rec1.currentWeightSum)+" ,currentWeightSum2="+to_string(rec2.currentWeightSum),LOG);
    LOG.trace("Comparing movieIDs");
    bool resultMovieId = (rec1.movieId == rec2.movieId);
    reportIfError(resultMovieId,"movieIds of recommendations are not equal! movieId1="+to_string(rec1.movieId)+" ,movieId2="+to_string(rec2.movieId),LOG);
    LOG.trace("Comparing movie names.");
    bool resultMovieName =  (rec1.movieName == rec2.movieName);
    reportIfError(resultMovieName,"movieNames of recommendations are not equal! movieName1="+rec1.movieName+" ,movieName2="+rec2.movieName,LOG);
    LOG.trace("Comparing genres.");
    bool resultGenres =  genresEqual(rec1.genres,rec2.genres);
    reportIfError(resultGenres,"genres of recommendations are not equal!",LOG);
    LOG.trace("Comparing common tags.");
    bool resultMostCommonTags = tagEntryVectorsEqual(rec1.mostCommonTags,rec2.mostCommonTags);
    reportIfError(resultMostCommonTags,"mostCommonTags of recommendations are not equal!",LOG);
    LOG.trace("Comparing mean ratings.");
    bool resultMeanRating = (realNumEqualWithinError(rec1.meanRating,rec2.meanRating,0.00001));
    reportIfError(resultMeanRating,"meanRating of recommendations are not equal! meanRating1="+to_string(rec1.meanRating)+" ,meanRating2="+to_string(rec2.meanRating),LOG);

    //user-based only.
    LOG.trace("Comparing meanRatingsFromNeighbors.");
    bool resultMeanRatingFromNeighbors = (realNumEqualWithinError(rec1.meanRatingFromNeighbors,rec2.meanRatingFromNeighbors,0.00001));
    reportIfError(resultMeanRatingFromNeighbors,"meanRatingFromNeighbors of recommendations are not equal! meanRatingFromNeighbors1="+to_string(rec1.meanRatingFromNeighbors)+" ,meanRatingFromNeighbors2="+to_string(rec2.meanRatingFromNeighbors),LOG);

    //item-based only.
    LOG.trace("Comparing similarWatchedMovies.");
    bool resultSimilarWatchedMovies = similarWatchedMoviesEqual(rec1.similarWatchedMovies,rec2.similarWatchedMovies);
    reportIfError(resultSimilarWatchedMovies,"similarWatchedMovies of recommendations are not equal!",LOG);


    bool result = resultPredicted && resultPredictedNormalized && resultCurrentWeightSum
                  && resultMovieId && resultMovieName && resultGenres && resultMostCommonTags
                  && resultMeanRating && resultMeanRatingFromNeighbors && resultSimilarWatchedMovies;

    if(!result){
        LOG.error("recommendations are not equal!");
    }

    LOG.close();
    return result;
}

bool recommendationVectorsEqual(vector<MovieRecommendation> recVec1, vector<MovieRecommendation> recVec2,bool minimal){

    RecommenderLogger LOG;
    LOG.init("testUtils:recommendationVectorsEqual");

    sort(recVec1.begin(),recVec1.end(),recommendationMovieIdComparator);
    sort(recVec2.begin(),recVec2.end(),recommendationMovieIdComparator);

    bool result = recVec1.size() == recVec2.size();
    for(size_t i=0;i<recVec1.size();i++){
        result = result && recommendationsEqual(recVec1[i],recVec2[i],minimal);
    }

    if(!result){
        LOG.error("recommendationVectors are not equal!");
    }

    LOG.close();
    return result;
}

bool userProfilesStandaloneEqual(UserProfileFull profile1,UserProfileFull profile2){
    RecommenderLogger LOG;
    LOG.init("testUtils:userProfilesStandaloneEqual");
    LOG.debug("Starting: userProfilesStandaloneEqual");
    bool result = true;
    result = result && (profile1.getProfileName() == profile2.getProfileName());
    if(!result){
        LOG.error("Profile names are not equal! profileName1="+profile1.getProfileName()+" , profileName2="+profile2.getProfileName());
    }
    result = result && (profile1.getUserId() == profile2.getUserId());
    if(!result){
        LOG.error("Profile userIds are not equal! userId1="+to_string(profile1.getUserId())+" , userId2="+to_string(profile2.getUserId()));
    }
    result = result && (profile1.isWaitingForData() == profile2.isWaitingForData());
    if(!result){
        LOG.error("Profile waitingForData booleans are not equal! waitingForData1="+to_string(profile1.isWaitingForData())+
                  " , waitingForData2="+to_string(profile2.isWaitingForData()));
    }
    result = result && usersEqual(profile1.getStandaloneProfile().userData,profile2.getStandaloneProfile().userData);
    if(!result){
        LOG.error("Profile userData are not equal!");
    }
    LOG.close();
    return result;
}

bool userProfileStandaloneVectorsEqual(vector<UserProfileFull> profileVec1,vector<UserProfileFull> profileVec2){
    RecommenderLogger LOG;
    LOG.init("testUtils:userProfileStandaloneVectorsEqual");
    bool equalSizes = profileVec1.size() == profileVec2.size();
    if(!equalSizes){
        LOG.error("standalone profile vectors are not equal in size! size1="+to_string(profileVec1.size())+" , size2="+to_string(profileVec2.size()));
        return false;
    }
    bool result = true;
    for(size_t i=0;i<profileVec1.size();i++){
        result = result && userProfilesStandaloneEqual(profileVec1[i],profileVec2[i]);
    }
    LOG.close();
    return result;
}

bool userProfilesDataBoundEqual(UserProfileFull profile1,UserProfileFull profile2){
    RecommenderLogger LOG;
    LOG.init("testUtils:userProfilesDataBoundEqual");
    LOG.debug("Starting: userProfilesDataBoundEqual");
    bool result = true;
    LOG.trace("Comparing profile names.");
    result = result && (profile1.getProfileName() == profile2.getProfileName());
    if(!result){
        LOG.error("Profile names are not equal! profileName1="+profile1.getProfileName()+" , profileName2="+profile2.getProfileName());
    }
    LOG.trace("Comparing user ids.");
    result = result && (profile1.getUserId() == profile2.getUserId());
    if(!result){
        LOG.error("Profile userIds are not equal! userId1="+to_string(profile1.getUserId())+" , userId2="+to_string(profile2.getUserId()));
    }
    LOG.trace("Comparing 'waitingForData' status.");
    result = result && (profile1.isWaitingForData() == profile2.isWaitingForData());
    if(!result){
        LOG.error("Profile waitingForData booleans are not equal! waitingForData1="+to_string(profile1.isWaitingForData())+
                  " , waitingForData2="+to_string(profile2.isWaitingForData()));
    }

    if(!profile1.isWaitingForData()){
    LOG.trace("Comparing userData.");
    UserProfileDataBound boundProfile1 = profile1.getDataBoundProfile(); 
    UserProfileDataBound boundProfile2 = profile2.getDataBoundProfile();
    User* userData1 = boundProfile1.userData;
    User* userData2 = boundProfile2.userData;
    if (userData1 == nullptr || userData2 == nullptr) {
        LOG.error("userData is null even though 'waitingForData' is false!");
        return false; 
    }
    result = result && usersEqual(*userData1,*userData2);
    if(!result){
        LOG.error("Profile userData are not equal!");
    }
    }
    return result;
}

bool userProfilesDataBoundVectorsEqual(vector<UserProfileFull> profileVec1,vector<UserProfileFull> profileVec2){
    RecommenderLogger LOG;
    LOG.init("testUtils:userProfilesDataBoundVectorsEqual");
    LOG.trace("comparing user profile vectors.");
    bool equalSizes = profileVec1.size() == profileVec2.size();
    if(!equalSizes){
        LOG.error("databound profile sizes are not equal! size1="+to_string(profileVec1.size())+" , size2="+to_string(profileVec2.size()));
        return false;
    }
    bool result = true;
    for(size_t i=0;i<profileVec1.size();i++){
        result = result && userProfilesDataBoundEqual(profileVec1[i],profileVec2[i]);
    }
    LOG.close();
    return result;
}

bool userProfilesFullEqual(UserProfileFull profile1,UserProfileFull profile2){
    RecommenderLogger LOG;
    LOG.init("testUtils:userProfilesFullEqual");
    LOG.debug("Starting: userProfilesFullEqual");
    bool result = true;
    LOG.trace("Comparing profile types (standalone/databound)");
    result = result && profile1.isDataBound() == profile2.isDataBound();
    if(!result){
        LOG.error("Profile mismatch: they are not both standalone or databound.");
        return false;
    }
    //databound comparison
    if(profile1.isDataBound()){
        LOG.trace("User profiles are both databound.");
        LOG.close();
        return userProfilesDataBoundEqual(profile1,profile2);
    }
    //standalone comparison
    else{
        LOG.trace("User profiles are both standalone.");
        LOG.close();
        return userProfilesStandaloneEqual(profile1,profile2);
    }
}

bool userProfilesFullVectorsEqual(vector<UserProfileFull> profileVec1,vector<UserProfileFull> profileVec2){
    RecommenderLogger LOG;
    LOG.init("testUtils:userProfilesFullVectorsEqual");
    LOG.debug("Starting: userProfilesFullVectorsEqual");
    LOG.trace("comparing user profile vectors.");
    LOG.trace("comparing user profile vector sizes.");
    bool equalSizes = profileVec1.size() == profileVec2.size();
    if(!equalSizes){
        LOG.error("profile sizes are not equal! size1="+to_string(profileVec1.size())+" , size2="+to_string(profileVec2.size()));
        return false;
    }
    LOG.trace("user profile vector sizes are equal.");
    bool result = true;
    for(size_t i=0;i<profileVec1.size();i++){
        result = result && userProfilesFullEqual(profileVec1[i],profileVec2[i]);
    }
    LOG.close();
    return result;
}

// // // // // // // // // //

void reportTestResult(TestResultWrapper result,bool enterToContinue){
    cout << endl;
    if (result.success()) {
        cout << "* " << result.getTestName() << ": PASSED. *" << endl;
    }
    else{
        cout << "*** " << result.getTestName() << ": FAILED. ***" << endl;
        if (!result.getMessage().empty())
            cout << "Error message : " << result.getMessage() << endl;
    }
    if(enterToContinue){
        cout<<"Press Enter to continue."<<endl;
        cin.get();
    }
}

void reportTestResults(vector<TestResultWrapper> results,bool enterToContinue){
	for (auto it = results.begin(); it != results.end(); it++){
		reportTestResult(*it,enterToContinue);
	}
    if(!enterToContinue){
        cout<<"Press Enter to continue."<<endl;
        cin.get();
    }
}

void reportTestResults(TestResultWrapper result,bool enterToContinue){
    reportTestResult(result,enterToContinue);
}

int randomInt(int minInt, int maxInt){

	int result = rand() % maxInt + minInt;
	return result;
}

double randomdouble(double mindouble, double maxdouble){

    double result = mindouble + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (maxdouble - mindouble)));
	return result;
}

double randomDouble(double minDouble, double maxDouble){

	double result = minDouble + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (maxDouble - minDouble)));
	return result;
}

void extractRatingsFromMovieEntries(vector<MovieEntry> movieEntries, vector<double>& ratings){

	for (uint i = 0; i < movieEntries.size(); i++){
		ratings.push_back(movieEntries[i].rating);
	}
}

void extractRatingsFromUserEntries(vector<UserEntry> userEntries, vector<double>& ratings){

	for (uint i = 0; i < userEntries.size(); i++){
		ratings.push_back(userEntries[i].rating);
	}
}

UserProfileFull createDummyUserProfileStandalone(string profileName,User userData,bool dataIsAvailable){
    RecommenderLogger LOG;
    LOG.init("testUtils:createDummyUserProfileStandalone");
    LOG.trace("creating dummy standalone user profile.");
    UserProfileStandalone standalone;
    standalone.profileName = profileName;
    standalone.userData = userData;
    UserProfileFull dummyProfile;
    dummyProfile.setWaitingForData(!dataIsAvailable);
    dummyProfile.setStandaloneProfile(standalone,dataIsAvailable);
    LOG.trace("dummy standalone user profile created successfully.");
    LOG.close();
    return dummyProfile;
}

UserProfileFull createDummyUserProfileDataBound(string profileName,User* userDataPtr,bool dataIsAvailable){
    RecommenderLogger LOG;
    LOG.init("testUtils:createDummyUserProfileDataBound");
    LOG.trace("creating dummy databound user profile.");
    UserProfileDataBound databound;
    databound.profileName = profileName;
    databound.userData = userDataPtr;
    UserProfileFull dummyProfile;
    dummyProfile.setWaitingForData(!dataIsAvailable);
    if(dataIsAvailable){
        LOG.trace("setting databound profile with available data.");
        dummyProfile.setDataBoundProfile(databound);
    } else{
        LOG.trace("setting databound profile without available data.");
        dummyProfile.setDataBoundProfileWithDataUnavailable(profileName,userDataPtr->userId);
    }
    LOG.trace("dummy databound user profile created successfully.");
    LOG.close();
    return dummyProfile;
}

TagEntry createDummyTagEntry(ulong tagId, string tagName, unsigned int tagCount){

	TagEntry dummyTagEntry;

	dummyTagEntry.tagId = tagId;
	dummyTagEntry.tagName = tagName;
	dummyTagEntry.tagCount = tagCount;

	return dummyTagEntry;
}

TagEntry createDummyTagEntryPreset(short id){

	switch (id){
	case 1:
        return createDummyTagEntry(111, "presetTag1", 1);
	case 2:
        return createDummyTagEntry(222, "presetTag2", 2);
	case 3:
        return createDummyTagEntry(333, "presetTag3", 3);
	case 4:
        return createDummyTagEntry(444, "presetTag4", 4);
	case 5:
        return createDummyTagEntry(555, "presetTag5", 5);
	}
}

TagEntry createDummyTagEntryRandom(){

	srand(time(nullptr));

	TagEntry randomTagEntry;

	randomTagEntry.tagCount = 5;
    randomTagEntry.tagId = static_cast<ulong>(randomInt(1, 100000));
	randomTagEntry.tagName = "randomTagName"; //TODO: maybe use a list of tag names to randomly select from.

	return randomTagEntry;
}


//version where tags are NOT supplied.
MovieEntry createDummyMovieEntry(ulong movieId, double rating){

	MovieEntry dummyMovieEntry;

	dummyMovieEntry.movieId = movieId;
	dummyMovieEntry.rating = rating;

	vector<TagEntry> dummyTags;
    dummyTags.push_back(createDummyTagEntry(11, "first", 0));
    dummyTags.push_back(createDummyTagEntry(22, "second", 0));
    dummyTags.push_back(createDummyTagEntry(33, "third", 0));
    dummyTags.push_back(createDummyTagEntry(44, "fourth", 0));
    dummyTags.push_back(createDummyTagEntry(55, "fifth", 0));
	dummyMovieEntry.tags = dummyTags;

    dummyMovieEntry.rated = true;
    dummyMovieEntry.tagged = true;

	return dummyMovieEntry;
}

//version where tags are supplied.
MovieEntry createDummyMovieEntry(ulong movieId, double rating, vector<TagEntry> tags){

	MovieEntry dummyMovieEntry;

	dummyMovieEntry.movieId = movieId;
	dummyMovieEntry.rating = rating;
	dummyMovieEntry.tags = tags;

    dummyMovieEntry.rated = true;
    dummyMovieEntry.tagged = true;

	return dummyMovieEntry;
}

MovieEntry createDummyMovieEntryPreset(short id){

	vector<TagEntry> dummyTagEntries;

	switch (id){
	case 1:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyMovieEntry(111, 1.0);
	case 2:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyMovieEntry(222, 2.0);
	case 3:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyMovieEntry(333, 3.0);
	case 4:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyMovieEntry(444, 4.0);
	case 5:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyMovieEntry(555, 5.0);
	}
}

MovieEntry createDummyMovieEntryRandom(){

	srand(time(nullptr));

	MovieEntry dummyMovieEntryRandom;

    dummyMovieEntryRandom.movieId = static_cast<ulong>(randomInt(1, 100000));
    dummyMovieEntryRandom.rating = randomdouble(1.0, 5.0);

	for (int i = 0; i < 5; i++){
		dummyMovieEntryRandom.tags.push_back(createDummyTagEntryRandom());
	}

	return dummyMovieEntryRandom;
}

NeighborUser createDummyNeighborUser(ulong id, double weight, vector<ulong> commonMoviesIds){

	NeighborUser dummyNeighborUser;

	dummyNeighborUser.id = id;
	dummyNeighborUser.weight = weight;
    dummyNeighborUser.commonMoviesIds = commonMoviesIds;

	return dummyNeighborUser;
}

NeighborUser createDummyNeighborUserPreset(short id){

	/*switch (id){
	case 1:
	return;
	case 2:
	return;
	case 3:
	return;
	case 4:
	return;
	case 5:
	return;
	}*/ //TODO

	NeighborUser emptyNeighborUser;
	return emptyNeighborUser;

}

NeighborUser createDummyNeighborUserRandom(){

	srand(time(nullptr));

	NeighborUser dummyNeighborUser;

    dummyNeighborUser.id = static_cast<ulong>(randomInt(1, 100000));
	dummyNeighborUser.weight;  //TODO
	dummyNeighborUser.commonMoviesIds; //TODO

	return dummyNeighborUser;
}

//version with all parameters given.
User createDummyUser(ulong userId, vector<MovieEntry> movieEntries, vector<NeighborUser> neighbors, double meanRating, double ratingStdDev){

	User dummyUser;

	dummyUser.userId = userId;
	dummyUser.meanRating = meanRating;
	dummyUser.ratingsStandardDeviation = ratingStdDev;
	dummyUser.movieEntries = movieEntries;
	dummyUser.neighbors = neighbors;

	return dummyUser;
}

//version with some empty parameters (fake scenarios).
User createDummyUser(ulong userId){
    User dummyUser;
    dummyUser.userId = userId;
    return dummyUser;
}

User createDummyUser(ulong userId,double meanRating,double ratingStdDev){

    User dummyUser;

    dummyUser.userId = userId;
    dummyUser.meanRating = meanRating;
    dummyUser.ratingsStandardDeviation = ratingStdDev;

    return dummyUser;
}

User createDummyUser(ulong userId, vector<MovieEntry> movieEntries, double meanRating, double ratingStdDev){
        User dummyUser;

    dummyUser.userId = userId;
    dummyUser.meanRating = meanRating;
    dummyUser.ratingsStandardDeviation = ratingStdDev;
    dummyUser.movieEntries = movieEntries;

    return dummyUser;
}

//version with some parameters calculated.
User createDummyUser(ulong userId, vector<MovieEntry> movieEntries, vector<NeighborUser> neighbors) {

	User dummyUser;

	dummyUser.userId = userId;
	dummyUser.movieEntries = movieEntries;
	dummyUser.neighbors = neighbors;

    vector<double> ratings;
	extractRatingsFromMovieEntries(movieEntries, ratings);
    double* ratingsPointer = &ratings[0];
	dummyUser.meanRating = calculateMean(ratingsPointer, ratings.size());
	dummyUser.ratingsStandardDeviation = calculateStandardDeviation(ratingsPointer, ratings.size());

	return dummyUser;
}


User createDummyUserPreset(short id){

	vector<MovieEntry> dummyMovieEntries;
    vector<NeighborUser> dummyNeighbors;
    vector<TagEntry> dummyTagEntries;

    vector<unsigned long> commonMoviesIds1={1,2,3};
    vector<unsigned long> commonMoviesIds2={4,5,6};
    vector<unsigned long> commonMoviesIds3={7,8,9};

	switch (id){
	case 1:
        dummyMovieEntries.push_back(createDummyMovieEntry(1, 5));
        dummyMovieEntries.push_back(createDummyMovieEntry(2, 1));
        dummyMovieEntries.push_back(createDummyMovieEntry(4, 2));
        dummyMovieEntries.push_back(createDummyMovieEntry(5, 2));
        dummyNeighbors.push_back(createDummyNeighborUser(111,0.1,commonMoviesIds1));
        dummyNeighbors.push_back(createDummyNeighborUser(222,0.2,commonMoviesIds2));
        dummyNeighbors.push_back(createDummyNeighborUser(333,0.3,commonMoviesIds3));
        return createDummyUser(111, dummyMovieEntries, dummyNeighbors, 2.5, 1.73);
	case 2:
        dummyMovieEntries.push_back(createDummyMovieEntry(1, 1));
        dummyMovieEntries.push_back(createDummyMovieEntry(2, 5));
        dummyMovieEntries.push_back(createDummyMovieEntry(3, 2));
        dummyMovieEntries.push_back(createDummyMovieEntry(4, 5));
        dummyMovieEntries.push_back(createDummyMovieEntry(5, 5));
        dummyNeighbors.push_back(createDummyNeighborUser(444,0.4,commonMoviesIds1));
        dummyNeighbors.push_back(createDummyNeighborUser(555,0.5,commonMoviesIds2));
        dummyNeighbors.push_back(createDummyNeighborUser(666,0.6,commonMoviesIds3));
        return createDummyUser(222, dummyMovieEntries, dummyNeighbors, 3.6, 1.95);
	case 3:
        dummyMovieEntries.push_back(createDummyMovieEntry(1, 2));
        dummyMovieEntries.push_back(createDummyMovieEntry(3, 3));
        dummyMovieEntries.push_back(createDummyMovieEntry(4, 5));
        dummyMovieEntries.push_back(createDummyMovieEntry(5, 4));
        dummyNeighbors.push_back(createDummyNeighborUser(777,0.7,commonMoviesIds1));
        dummyNeighbors.push_back(createDummyNeighborUser(888,0.8,commonMoviesIds2));
        dummyNeighbors.push_back(createDummyNeighborUser(999,0.9,commonMoviesIds3));
        return createDummyUser(333, dummyMovieEntries, dummyNeighbors, 3.5, 1.29);
	case 4:
        dummyMovieEntries.push_back(createDummyMovieEntry(1, 4));
        dummyMovieEntries.push_back(createDummyMovieEntry(2, 3));
        dummyMovieEntries.push_back(createDummyMovieEntry(3, 5));
        dummyMovieEntries.push_back(createDummyMovieEntry(4, 3));
        return createDummyUser(444, dummyMovieEntries, dummyNeighbors, 3.75, 0.96);
	case 5:
        dummyMovieEntries.push_back(createDummyMovieEntry(1, 1));
        dummyMovieEntries.push_back(createDummyMovieEntry(2, 1));
        dummyMovieEntries.push_back(createDummyMovieEntry(3, 1));
        dummyMovieEntries.push_back(createDummyMovieEntry(4, 1));
        dummyMovieEntries.push_back(createDummyMovieEntry(5, 5));
        return createDummyUser(555, dummyMovieEntries, dummyNeighbors, 1.8, 1.79);
    case 6:
    {
        dummyTagEntries.push_back(createDummyTagEntry(11,"first",0));
        dummyTagEntries.push_back(createDummyTagEntry(22,"second",0));
        dummyTagEntries.push_back(createDummyTagEntry(33,"third",0));
        dummyTagEntries.push_back(createDummyTagEntry(44,"fourth",0));
        dummyTagEntries.push_back(createDummyTagEntry(55,"fifth",0));
        vector<ulong> commonMoviesIds1;
        commonMoviesIds1.push_back(99);
        commonMoviesIds1.push_back(100);
        vector<ulong> commonMoviesIds2;
        commonMoviesIds2.push_back(123);
        commonMoviesIds2.push_back(456);
        commonMoviesIds2.push_back(789);
        vector<ulong> commonMoviesIds3;
        commonMoviesIds3.push_back(1);
        commonMoviesIds3.push_back(2);
        commonMoviesIds3.push_back(3);
        dummyNeighbors.push_back(createDummyNeighborUser(200,0.9,commonMoviesIds1));
        dummyNeighbors.push_back(createDummyNeighborUser(300,1.0,commonMoviesIds2));
        dummyNeighbors.push_back(createDummyNeighborUser(400,1.1,commonMoviesIds3));
        dummyMovieEntries.push_back(createDummyMovieEntry(1,5,dummyTagEntries));
        dummyMovieEntries.push_back(createDummyMovieEntry(2,1,dummyTagEntries));
        dummyMovieEntries.push_back(createDummyMovieEntry(4,2,dummyTagEntries));
        dummyMovieEntries.push_back(createDummyMovieEntry(5,2,dummyTagEntries));
        return createDummyUser(100,dummyMovieEntries,dummyNeighbors,3.5,2.18);
    }
    case 7:
        dummyTagEntries.push_back(createDummyTagEntry(123,"sametag1",0));
        dummyTagEntries.push_back(createDummyTagEntry(456,"sametag2",0));
        dummyMovieEntries.push_back(createDummyMovieEntry(111, 1.0,dummyTagEntries));
        dummyMovieEntries.push_back(createDummyMovieEntry(222, 2.0,dummyTagEntries));
        dummyNeighbors.push_back(createDummyNeighborUser(111,0.1,commonMoviesIds1));
        dummyNeighbors.push_back(createDummyNeighborUser(222,0.2,commonMoviesIds2));
        dummyNeighbors.push_back(createDummyNeighborUser(333,0.3,commonMoviesIds3));
        return createDummyUser(991, dummyMovieEntries, dummyNeighbors, 1.5, 0.5);
    case 8:
        dummyTagEntries.push_back(createDummyTagEntry(123,"sametag1",0));
        dummyTagEntries.push_back(createDummyTagEntry(456,"sametag2",0));
        dummyMovieEntries.push_back(createDummyMovieEntry(111, 1.0,dummyTagEntries));
        dummyMovieEntries.push_back(createDummyMovieEntry(222, 2.0,dummyTagEntries));
        dummyNeighbors.push_back(createDummyNeighborUser(111,0.1,commonMoviesIds1));
        dummyNeighbors.push_back(createDummyNeighborUser(222,0.2,commonMoviesIds2));
        dummyNeighbors.push_back(createDummyNeighborUser(333,0.3,commonMoviesIds3));
        return createDummyUser(992, dummyMovieEntries, dummyNeighbors, 1.5, 0.5);
    case 9:
        return createDummyUser(992);
	}
}

User createDummyUserRandom() {

	srand(time(nullptr));

    vector<double> randomRatings;
	for (int i = 0; i < 5; i++){
        randomRatings.push_back(randomdouble(1.0, 5.0));
	}
    double* randomRatingsPointer = &randomRatings[0];

	User dummyUserRandom;

    dummyUserRandom.userId = static_cast<ulong>(randomInt(1, 100000));
	dummyUserRandom.meanRating = calculateMean(randomRatingsPointer, randomRatings.size());
	dummyUserRandom.ratingsStandardDeviation = calculateStandardDeviation(randomRatingsPointer, randomRatings.size());

	for (int i = 0; i < 5; i++){
		dummyUserRandom.movieEntries.push_back(createDummyMovieEntryRandom());
	}

	for (int i = 0; i < 5; i++){
		dummyUserRandom.neighbors.push_back(createDummyNeighborUserRandom());
	}

	return dummyUserRandom;
}

//////////////////////////////////////////////


//version where tags are not supplied.
UserEntry createDummyUserEntry(ulong userId, double rating){
    //TODO:implement.
}

//version where tags are supplied.
UserEntry createDummyUserEntry(ulong userId, double rating, vector<TagEntry> tags){

	UserEntry dummyUserEntry;

	dummyUserEntry.userId = userId;
	dummyUserEntry.rating = rating;;
	dummyUserEntry.tags = tags;

	return dummyUserEntry;
}

UserEntry createDummyUserEntryPreset(short id){

	vector<TagEntry> dummyTagEntries;

	switch (id){
	case 1:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyUserEntry(111, 1.0, dummyTagEntries);
	case 2:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyUserEntry(222, 2.0, dummyTagEntries);
	case 3:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyUserEntry(333, 3.0, dummyTagEntries);
	case 4:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyUserEntry(444, 4.0, dummyTagEntries);
	case 5:
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 1));
        dummyTagEntries.push_back(createDummyTagEntry(5, "dummy5", 1));
        return createDummyUserEntry(555, 5.0, dummyTagEntries);
	}

	UserEntry emptyUserEntry;
	return emptyUserEntry;
}

UserEntry createDummyUserEntryRandom(){

	srand(time(nullptr));

	UserEntry randomUserEntry;

    randomUserEntry.userId = static_cast<ulong>(randomInt(1, 100000));
    randomUserEntry.rating = randomdouble(1.0, 5.0);

	for (int i = 0; i < 5; i++){
		randomUserEntry.tags.push_back(createDummyTagEntryRandom());
	}
}

//version with all parameters given.
Movie createDummyMovie(ulong movieId, string movieName, double meanRating, double ratingsStandardDeviation, double inverseUserFrequency, vector<string> genres
	, vector<TagEntry> totalTags, vector<UserEntry> userEntries, vector<NeighborMovie> neighbors) {

	Movie dummyMovie;

	dummyMovie.movieId = movieId;
	dummyMovie.movieName = movieName;
	dummyMovie.meanRating = meanRating;
	dummyMovie.ratingsStandardDeviation = ratingsStandardDeviation;
    //dummyMovie.inverseUserFrequency = inverseUserFrequency;
	dummyMovie.genres = genres;
	dummyMovie.totalTags = totalTags;
	dummyMovie.userEntries = userEntries;
	dummyMovie.neighbors = neighbors;

	return dummyMovie;
}

//version with some empty parameters (fake scenarios)
Movie createDummyMovie(ulong movieId,string movieName,double meanRating
                       ,double ratingsStandardDeviation,double inverseUserFrequency){

    Movie dummyMovie;

    dummyMovie.movieId = movieId;
    dummyMovie.movieName = movieName;
    dummyMovie.meanRating = meanRating;
    dummyMovie.ratingsStandardDeviation = ratingsStandardDeviation;
    //dummyMovie.inverseUserFrequency = inverseUserFrequency;

    return dummyMovie;
}

//version with some parameters calculated.
Movie createDummyMovie(ulong movieId, string movieName, vector<string> genres, vector<TagEntry> totalTags, vector<UserEntry> userEntries, vector<NeighborMovie> neighbors){

	Movie dummyMovie;

	dummyMovie.movieId = movieId;
	dummyMovie.movieName = movieName;
	dummyMovie.genres = genres;
	dummyMovie.totalTags = totalTags;
	dummyMovie.userEntries = userEntries;
	dummyMovie.neighbors = neighbors;

    vector<double> ratings;
	extractRatingsFromUserEntries(userEntries, ratings);
    double* ratingsPointer = &ratings[0];

	dummyMovie.meanRating = calculateMean(ratingsPointer, ratings.size());
	dummyMovie.ratingsStandardDeviation = calculateStandardDeviation(ratingsPointer, ratings.size());
    //dummyMovie.inverseUserFrequency;

	return dummyMovie;
}


Movie createDummyMoviePreset(short id){

    //double inverseUserFrequency; //TODO : inverse,standardDev

	vector<UserEntry> dummyUserEntries;
	vector<string> dummyGenres;
	vector<TagEntry> dummyTagEntries;
	vector<NeighborMovie> dummyNeighbors;

	vector<TagEntry> currentUserDummyTagEntries;

    vector<unsigned long> commonUsersIds1;
    vector<unsigned long> commonUsersIds2;
    vector<unsigned long> commonUsersIds3;

	switch (id){
	case 1:
        currentUserDummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 0));
        dummyUserEntries.push_back(createDummyUserEntry(1, 5, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 0));
        dummyUserEntries.push_back(createDummyUserEntry(2, 1, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 0));
        dummyUserEntries.push_back(createDummyUserEntry(3, 2, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 0));
        dummyUserEntries.push_back(createDummyUserEntry(4, 4, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

		dummyGenres.push_back("Adventure");
		dummyGenres.push_back("Animation");
		dummyGenres.push_back("Children");
		dummyGenres.push_back("Comedy");
        commonUsersIds1 = {1,2,3};
        commonUsersIds2 = {4,5,6};
        commonUsersIds3 = {7,8,9};
        dummyNeighbors.push_back(createDummyNeighborMovie(1111,0.1,commonUsersIds1));
        dummyNeighbors.push_back(createDummyNeighborMovie(2222,0.2,commonUsersIds2));
        dummyNeighbors.push_back(createDummyNeighborMovie(3333,0.3,commonUsersIds3));
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 10));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 20));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 30));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 40));
        return createDummyMovie(111, "movie1", 3.0, 1.581, 0.0, dummyGenres, dummyTagEntries, dummyUserEntries, dummyNeighbors);

	case 2:
        currentUserDummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 0));
        dummyUserEntries.push_back(createDummyUserEntry(1, 1, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 0));
        dummyUserEntries.push_back(createDummyUserEntry(2, 5, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 0));
        dummyUserEntries.push_back(createDummyUserEntry(3, 3, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

		dummyGenres.push_back("Fantasy");
		dummyGenres.push_back("Documentary");
		dummyGenres.push_back("Mystery");
        commonUsersIds1 = {1,2,3};
        commonUsersIds2 = {4,5,6};
        commonUsersIds3 = {7,8,9};
        dummyNeighbors.push_back(createDummyNeighborMovie(4444,0.4,commonUsersIds1));
        dummyNeighbors.push_back(createDummyNeighborMovie(5555,0.5,commonUsersIds2));
        dummyNeighbors.push_back(createDummyNeighborMovie(6666,0.6,commonUsersIds3));
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 10));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 20));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 30));
        dummyTagEntries.push_back(createDummyTagEntry(4, "dummy4", 40));
        return createDummyMovie(222, "movie2", 3, 1.633, 0.0, dummyGenres, dummyTagEntries, dummyUserEntries, dummyNeighbors);

	case 3:
        currentUserDummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 0));
        dummyUserEntries.push_back(createDummyUserEntry(2, 2, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 0));
        dummyUserEntries.push_back(createDummyUserEntry(3, 3, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 0));
        dummyUserEntries.push_back(createDummyUserEntry(4, 5, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

		dummyGenres.push_back("Sci-Fi");
		dummyGenres.push_back("Thriller");
		dummyGenres.push_back("Mystery");
        commonUsersIds1 = {1,2,3};
        commonUsersIds2 = {4,5,6};
        commonUsersIds3 = {7,8,9};
        dummyNeighbors.push_back(createDummyNeighborMovie(7777,0.7,commonUsersIds1));
        dummyNeighbors.push_back(createDummyNeighborMovie(8888,0.8,commonUsersIds2));
        dummyNeighbors.push_back(createDummyNeighborMovie(9999,0.9,commonUsersIds3));
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 10));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 20));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 30));
        return createDummyMovie(333, "movie3", 3.33, 1.247, 0.0, dummyGenres, dummyTagEntries, dummyUserEntries, dummyNeighbors);

	case 4:
        currentUserDummyTagEntries.push_back(createDummyTagEntry(1, "inner_dummy3", 0));
        dummyUserEntries.push_back(createDummyUserEntry(1, 2, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(2, "inner_dummy2", 0));
        dummyUserEntries.push_back(createDummyUserEntry(2, 5, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(3, "inner_dummy3", 0));
        dummyUserEntries.push_back(createDummyUserEntry(3, 5, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(4, "inner_dummy4", 0));
        dummyUserEntries.push_back(createDummyUserEntry(4, 3, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();


		dummyGenres.push_back("Action");
		dummyGenres.push_back("Crime");
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 1));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 1));
        dummyTagEntries.push_back(createDummyTagEntry(3, "dummy3", 1));
        return createDummyMovie(444, "movie4", 0.0, 0.0, 0.0, dummyGenres, dummyTagEntries, dummyUserEntries, dummyNeighbors);

	case 5:

        currentUserDummyTagEntries.push_back(createDummyTagEntry(111, "inner_dummy1", 0));
        dummyUserEntries.push_back(createDummyUserEntry(1, 2, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(222, "inner_dummy2", 0));
        dummyUserEntries.push_back(createDummyUserEntry(2, 5, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        currentUserDummyTagEntries.push_back(createDummyTagEntry(333, "inner_dummy3", 0));
        dummyUserEntries.push_back(createDummyUserEntry(3, 4, currentUserDummyTagEntries));
		currentUserDummyTagEntries.clear();

        commonUsersIds1 = {1,2,3};
        commonUsersIds2 = {4,5,6};
        commonUsersIds3 = {7,8,9};
        dummyNeighbors.push_back(createDummyNeighborMovie(1111,0.1,commonUsersIds1));
        dummyNeighbors.push_back(createDummyNeighborMovie(2222,0.2,commonUsersIds2));
        dummyNeighbors.push_back(createDummyNeighborMovie(3333,0.3,commonUsersIds3));

		dummyGenres.push_back("Drama");
		dummyGenres.push_back("Mystery");
        dummyTagEntries.push_back(createDummyTagEntry(1, "dummy1", 10));
        dummyTagEntries.push_back(createDummyTagEntry(2, "dummy2", 20));
        return createDummyMovie(555, "movie5", 3.666, 1.2472191289246, 0.0, dummyGenres, dummyTagEntries, dummyUserEntries, dummyNeighbors);
	}

	Movie emptyMovie;
	return  emptyMovie;
}

Movie createDummyMovieRandom(){

	srand(time(nullptr));

	Movie randomMovie;

	return randomMovie;
}


//version where weight is random.
NeighborMovie createDummyNeighborMovie(ulong movieId){

	srand(time(nullptr));

	NeighborMovie dummyNeighborMovie;

    dummyNeighborMovie.id = movieId;
    dummyNeighborMovie.weight = randomdouble(1.0, 50.0); //TODO: verify values.
	dummyNeighborMovie.commonUsersIds; //TODO

	return dummyNeighborMovie;
}

//version where weight is non-random and given by user.
NeighborMovie createDummyNeighborMovie(ulong movieId, double weight,vector<unsigned long> commonUsersIds){

	NeighborMovie dummyNeighborMovie;

	dummyNeighborMovie.id = movieId;
	dummyNeighborMovie.weight = weight;
    dummyNeighborMovie.commonUsersIds = commonUsersIds;

	return dummyNeighborMovie;
}

NeighborMovie createDummyNeighborMovieRandom(){

	srand(time(nullptr));

	NeighborMovie dummyNeighborMovieRandom;

    dummyNeighborMovieRandom.id = static_cast<ulong>(randomInt(1, 100000));
    dummyNeighborMovieRandom.weight = randomdouble(1.0, 50.0); //TODO: verify values.
	dummyNeighborMovieRandom.commonUsersIds; //TODO

	return dummyNeighborMovieRandom;
}

//TODO
NeighborMovie createNeighborMoviePreset(short id){

	switch (id){
	case 1:
        //return createDummyNeighborMovie("", 0.0);
        break;
	case 2:
        //return createDummyNeighborMovie("", 0.0);
        break;
	case 3:
        //return createDummyNeighborMovie("", 0.0);
        break;
	case 4:
        //return createDummyNeighborMovie("", 0.0);
        break;
	case 5:
        //return createDummyNeighborMovie("", 0.0);
        break;
    default:
        NeighborMovie emptyNeighborMovie;
        return emptyNeighborMovie;
	}
}

void populateDummyConfigurationPresetBoolean(Configuration& dummyConfig){
	
    dummyConfig.initDataAutomaticallyOnStartup = false;
	dummyConfig.useDatabase = false;
	dummyConfig.realTimeCalculations = false;
	dummyConfig.useLimitInWatchedMoviesUserBasedMethod = false;
	dummyConfig.useLimitInWatchedMoviesItemBasedMethod = false;
	dummyConfig.useAdjustedCorrellationForItemSimilarities = true;
    dummyConfig.discardNegativeWeightNeighbors = true;
    dummyConfig.useTagSimilarityInItemBasedMethod = false;
    dummyConfig.terminateApplicationOnCsvImportError=false;
    dummyConfig.terminateApplicationOnJsonImportError=false;
    dummyConfig.writeIntermediateMatricesToFile=false;
}

void populateDummyConfigurationPresetNumeric(Configuration& dummyConfig){
	
	dummyConfig.maxNeighbors = 5;
	dummyConfig.neighborWeightThreshold = 0.5;
	dummyConfig.recommendationCount = 5;
	dummyConfig.maxWatchedMovieCountUsedInItemBasedMethod = 20;
	dummyConfig.maxWatchedMovieCountUsedInUserBasedMethod = 100;
    dummyConfig.significanceWeightingFactor = 50;
    dummyConfig.minimumCommonElementsForUserNeighbors = 2;
    dummyConfig.minimumCommonElementsForMovieNeighbors = 1;
    dummyConfig.similarityThreshold = 0.7;
    dummyConfig.stringSimilarityMethod= 1;
    dummyConfig.minimumRatingCountForAggregations= 15;
}

void populateDummyConfigurationPresetParsing(Configuration& dummyConfig){
	
	dummyConfig.movieNameMaxLength = 175;
	dummyConfig.tagNameMaxLength = 175;
	dummyConfig.genreNameMaxLength = 20;
	dummyConfig.maxGenreCount = 15;
	dummyConfig.maxTagsPerUser = 20;
	dummyConfig.userEntriesReallocFactor = 1.1;
	dummyConfig.tagsReallocFactor = 1.5;
	dummyConfig.automaticMultithread = 1;
	dummyConfig.multithreadParseRatingsEnabled = 1;
	dummyConfig.multithreadParseOtherEnabled = 0;
	dummyConfig.multithreadSearchEnabled = 1;
	dummyConfig.defaultThreadcount = 4;
	
}

void populateDummyConfigurationPresetParsingPaths(Configuration& dummyConfig){
	
	dummyConfig.moviesFileDefaultPath = "path/to/movies/file";
	dummyConfig.ratingsFileDefaultPath = "path/to/ratings/file";
    dummyConfig.tagEntriesFileDefaultPath = "path/to/tags/file";
    dummyConfig.tagIdsFileDefaultPath = "path/to/genometags/file";
}

void populateDummyConfigurationParsingPaths(Configuration& dummyConfig,string moviesPath,string ratingsPath,string tagsPath,string genometagsPath){

    dummyConfig.moviesFileActivePath = moviesPath;
    dummyConfig.ratingsFileActivePath = ratingsPath;
    dummyConfig.tagEntriesFileActivePath = tagsPath;
    dummyConfig.tagIdsFileActivePath = genometagsPath;
    dummyConfig.moviesFileActivePath = moviesPath;
    dummyConfig.ratingsFileActivePath = ratingsPath;
    dummyConfig.tagEntriesFileActivePath = tagsPath;
    dummyConfig.tagIdsFileActivePath = genometagsPath;
}

void populateDummyConfigurationPresetProfiles(Configuration& dummyConfig){
    dummyConfig.profileFilepath = "path/to/profiles";
    dummyConfig.profileStorageType = "FILE";
}

void writeToFile_usrentr(Configuration* config,float** userEntries, size_t rowcount,FILE* file){

    cout << "Writing userEntries matrix to file..." << endl;

    fprintf(file,"-- USER ENTRIES MATRIX --\n");
    fprintf(file,"[userID],[movieID],[rating],[tagIDs]\n");

    for (uint r = 0; r < rowcount; r++){
        for (uint c = 0; c < (3 + config->maxTagsPerUser); c++){
            if (userEntries[r][c] == 0){
                if (c == 2){
                    fprintf(file, "-");
                    fprintf(file,",");
                    continue;
                }
                else{
                    break;
                }
            }
            if (c != 2){
                fprintf(file, "%0.f", userEntries[r][c]);
            }
            else{
                fprintf(file, "%0.1f", userEntries[r][c]);
            }
            if (c > 2){
                fprintf(file, "|");
            }
            else{
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");

    }

    cout << "done." << endl;
}

void writeToFile_movies(char*** movies, size_t rowcount,FILE* file){

    cout << "Writing movies matrix to file..." << endl;

    fprintf(file, "-- MOVIES MATRIX --\n");
    fprintf(file, "[movieID],[title]\n");

    for (uint r = 0; r < rowcount; r++){

        for (uint c = 0; c < 2; c++){
            fprintf(file,"%s",movies[r][c]);
            fprintf(file,",");
        }
        fprintf(file,"\n");

    }

    cout << "done." << endl;
}

void writeToFile_gnrentr(Configuration* config,char*** genreEntries, size_t rowcount,FILE* file){

    cout << "Writing genreEntries matrix to file..." << endl;

    fprintf(file, "-- GENRE ENTRIES MATRIX --\n");
    fprintf(file, "[genres]\n");

    for (uint r = 0; r < rowcount; r++){

        for (uint c = 0; c < config->maxGenreCount; c++){
            if (genreEntries[r][c][0] == 0)
                break;
            fprintf(file,"%s",genreEntries[r][c]);
            fprintf(file,"|");

        }
        fprintf(file, "\n");

    }

    cout << "done." << endl;
}

void writeToFile_tags(char** tags, size_t rowcount,FILE* file){

    cout << "Writing tags matrix to file..." << endl;

    fprintf(file, "-- TAGS MATRIX --\n");
    fprintf(file, "[tag]\n");

    for (uint r = 0; r < rowcount; r++){
        fprintf(file,"%s",tags[r]);
        fprintf(file,"\n");
    }

    cout << "done." << endl;
}

//

void writeUserProfileToFile(UserProfileFull profile,ofstream& f){
    f<<"UserProfile::"<<endl;
    f<<"-profileName:"<<profile.getProfileName()<<endl;
    f<<"-userId:"<<profile.getUserId()<<endl;
    f<<"-waitingForData:"<<profile.isWaitingForData()<<endl;
    f<<"-dataBound:"<<profile.isDataBound()<<endl;
    f<<"-userData:"<<endl;
    if(profile.isDataBound()){
        if(!profile.isWaitingForData()){
            writeUserToFile(*profile.getDataBoundProfile().userData,f);
        }
    } else{
        writeUserToFile(profile.getStandaloneProfile().userData,f);
    }
}

void writeUserProfilesVectorToFile(vector<UserProfileFull> profiles,ofstream& f){
    for(UserProfileFull p : profiles){
        f<<"---"<<endl;
        writeUserProfileToFile(p,f);
    }
}
