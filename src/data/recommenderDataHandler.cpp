#include "recommenderDataHandler.h"

#include "../exceptions/recommenderExceptions.h"
#include "../math/mathUtils.h"
#include "../utils/utils.h"
#include "../logging/recommenderLogger.h"
#include "../utils/similarityUtils.h"
#include "../data/dataCalculations.h"
#include "../main/movieGenres.h"
#include "../utils/similarityUtils.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <map>
#include <set>

using namespace std;

typedef unsigned long ulong;

//log levels: 1->ERROR,2->WARN,3->INFO,4->DEBUG,5->TRACE
static int LOGLEVEL=4; //TRACE level may impact performance. recommended=DEBUG or higher.

//static bool USE_HARDCODED_LOGLEVEL = false; //TODO:add functionality for this parameter.

struct NormalizedRatingPredictionWrapper {

    double predictedRatingNormalized;
    double currentWeightSum; //sum of absolute values.

};

//AGGREGATIONS:
//

bool mostUsedTagResultComparator(pair<string,size_t> p1,pair<string,size_t> p2){
    return p1.second > p2.second; //descending order.
}

Movie& Aggregations::highestRatedMovie(unsigned long minimumRatingCount){
    if(useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE);
    } else{
        Movie* highestRatedMovie = &rdh->getAllMovies()[0];
        double largestMeanRating = 0;
        double currentMeanRating = 0;
        for(auto it=rdh->getAllMovies().begin();it!=rdh->getAllMovies().end();it++){
            //only take into account movies with at least 'minimumRatingCount' ratings.
            if(it->userEntries.size() < minimumRatingCount){
                continue;
            } else{
                ulong actuallyRated = 0;
                for(auto it_inner = it->userEntries.begin();it_inner!=it->userEntries.end();it_inner++){
                    if(it_inner->rated){
                        actuallyRated++;
                    }
                }
                if(actuallyRated < minimumRatingCount){
                    continue;
                }
            }
            currentMeanRating = it->meanRating;
            if(currentMeanRating > largestMeanRating){
                largestMeanRating = currentMeanRating;
                highestRatedMovie = &(*it);
            }
        }
        return *highestRatedMovie;
    }
}

Movie& Aggregations::mostWatchedMovie(){
    if(useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement.
    } else{
        Movie* mostWatchedMovie = &rdh->getAllMovies()[0];
        size_t largestWatchedMovieCount = 0;
        size_t currentWatchedMovieCount = 0;
        for(auto it=rdh->getAllMovies().begin();it!=rdh->getAllMovies().end();it++){
//            for(auto it2=it->userEntries.begin();it2!=it->userEntries.end();it2++){
//                if(it2->rated){
//                    currentWatchedMovieCount++;
//                }
//            } //commented-out:counting only the rated userEntries. below: counting all userEntries.
            currentWatchedMovieCount = it->userEntries.size();
            if(currentWatchedMovieCount > largestWatchedMovieCount){
                largestWatchedMovieCount = currentWatchedMovieCount;
                mostWatchedMovie = &(*it);
            }
        }
        return *mostWatchedMovie;
    }
}

Movie& Aggregations::highestDeviationMovie(){
    if(useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
    } else{
        Movie* highestDeviationMovie = &rdh->getAllMovies()[0];
        double highestDeviation = 0;
        double currentDeviation = 0;
        for(auto it=rdh->getAllMovies().begin();it!=rdh->getAllMovies().end();it++){
            currentDeviation = it->ratingsStandardDeviation;
            if(currentDeviation > highestDeviation){
                highestDeviation = currentDeviation;
                highestDeviationMovie = &(*it);
            }
        }
        return *highestDeviationMovie;
    }
}

MostTaggedMovieResult Aggregations::mostTaggedMovie(){
    MostTaggedMovieResult result;
    if(useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement.
    } else{
        auto& allMovies = rdh->getAllMovies();
        if(allMovies.empty()){
            result.movie = nullptr;
            return result;
        }
        Movie* mostTaggedMovie = &rdh->getAllMovies()[0];
        size_t largestTagCount = 0;
        size_t currentTagCount = 0;
        for(auto it=rdh->getAllMovies().begin();it!=rdh->getAllMovies().end();it++){
            currentTagCount = 0;
            for(auto it2=it->totalTags.begin();it2!=it->totalTags.end();it2++){
                currentTagCount += it2->tagCount;
            }
            if(currentTagCount > largestTagCount){
                largestTagCount = currentTagCount;
                mostTaggedMovie = &(*it);
            }
        }
        result.movieName = mostTaggedMovie->movieName;
        result.tagCount = largestTagCount;
        result.movie = mostTaggedMovie;
        return result;
    }
}

MostUsedTagResult Aggregations::mostUsedTag(){
    if(useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
    } else{
        MostUsedTagResult result;
        //key:tag name, value:times used
        unordered_map<string,size_t> tagTimesUsedMap;
        for(auto it=rdh->getAllMovies().begin();it!=rdh->getAllMovies().end();it++){
            vector<TagEntry> tagEntries = it->totalTags; //totalTags contains tagCounts.
            for(auto inner_it = tagEntries.begin();inner_it!=tagEntries.end();inner_it++){
                string tagname = strToLower(inner_it->tagName);
                tagname = removeWhitespaces(tagname);
                //if doesnt already exist as key,create new pair.
                //if already exists, append the times used value.
                if(tagTimesUsedMap.count(tagname) == 0){
                    pair<string,size_t> newPair = make_pair(tagname,inner_it->tagCount);
                    tagTimesUsedMap.insert(newPair);
                } else{
                    auto found = tagTimesUsedMap.find(tagname);
                    found->second = found->second + inner_it->tagCount;
                }
            }
        }
        //convert map to vector of pairs.
        vector<pair<string,size_t>> tagTimesUsedVector(tagTimesUsedMap.size());
        copy(tagTimesUsedMap.begin(),tagTimesUsedMap.end(),tagTimesUsedVector.begin());
        sort(tagTimesUsedVector.begin(),tagTimesUsedVector.end(),mostUsedTagResultComparator);
        string mostUsedTagName = tagTimesUsedVector[0].first;
        size_t mostUsedTagCount = tagTimesUsedVector[0].second;
        result.tagName = mostUsedTagName;
        result.timesUsed = mostUsedTagCount;
        return result;
    }
}

MostCommonGenreResult Aggregations::mostCommonGenre(){
    if(useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement.
    } else{
        MostCommonGenreResult result;
        //key: genre name , value: times encountered.
        map<string,size_t> genresMap;
        pair<string,size_t> pAction = make_pair(MovieGenre::ACTION.str,0);
        genresMap.insert(pAction);
        pair<string,size_t> pAdventure = make_pair(MovieGenre::ADVENTURE.str,0);
        genresMap.insert(pAdventure);
        pair<string,size_t> pAnimation = make_pair(MovieGenre::ANIMATION.str,0);
        genresMap.insert(pAnimation);
        pair<string,size_t> pChildren = make_pair(MovieGenre::CHILDREN.str,0);
        genresMap.insert(pChildren);
        pair<string,size_t> pComedy = make_pair(MovieGenre::COMEDY.str,0);
        genresMap.insert(pComedy);
        pair<string,size_t> pCrime = make_pair(MovieGenre::CRIME.str,0);
        genresMap.insert(pCrime);
        pair<string,size_t> pDocumentary = make_pair(MovieGenre::DOCUMENTARY.str,0);
        genresMap.insert(pDocumentary);
        pair<string,size_t> pDrama = make_pair(MovieGenre::DRAMA.str,0);
        genresMap.insert(pDrama);
        pair<string,size_t> pFantasy = make_pair(MovieGenre::FANTASY.str,0);
        genresMap.insert(pFantasy);
        pair<string,size_t> pNoir = make_pair(MovieGenre::NOIR.str,0);
        genresMap.insert(pNoir);
        pair<string,size_t> pHorror = make_pair(MovieGenre::HORROR.str,0);
        genresMap.insert(pHorror);
        pair<string,size_t> pMusical = make_pair(MovieGenre::MUSICAL.str,0);
        genresMap.insert(pMusical);
        pair<string,size_t> pMystery = make_pair(MovieGenre::MYSTERY.str,0);
        genresMap.insert(pMystery);
        pair<string,size_t> pRomance = make_pair(MovieGenre::ROMANCE.str,0);
        genresMap.insert(pRomance);
        pair<string,size_t> pSciFi = make_pair(MovieGenre::SCI_FI.str,0);
        genresMap.insert(pSciFi);
        pair<string,size_t> pThriller = make_pair(MovieGenre::THRILLER.str,0);
        genresMap.insert(pThriller);
        pair<string,size_t> pWar = make_pair(MovieGenre::WAR.str,0);
        genresMap.insert(pWar);
        pair<string,size_t> pWestern = make_pair(MovieGenre::WESTERN.str,0);
        genresMap.insert(pWestern);
        for(auto it=rdh->getAllMovies().begin();it!=rdh->getAllMovies().end();it++){
            vector<string> currentGenres = it->genres;
            for(auto it2 = currentGenres.begin();it2!=currentGenres.end();it2++){
                string currentGenre = *it2;
                map<string,size_t>::iterator found = genresMap.find(currentGenre);
                found->second++;
            }
        }
        std::string mostCommonGenre = MovieGenre::ACTION.str;
        size_t largestTimesUsed = 0;
        size_t currentTimesUsed = 0;
        for(auto it=genresMap.begin();it!=genresMap.end();it++){
            currentTimesUsed = it->second;
            if(currentTimesUsed > largestTimesUsed){
                largestTimesUsed = currentTimesUsed;
                mostCommonGenre = it->first;
            }
        }
        result.genre = mostCommonGenre;
        result.timesUsed = largestTimesUsed;
        return result;
    }
}

HighestRatedGenreResult Aggregations::highestRatedGenre(){
    if(useDatabase){
        throw new NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
    } else{
        HighestRatedGenreResult result;
        //key:genre name , value:pair with p1=rating and p2=count
        map<string,pair<double,size_t>> genresMap;
        pair<string,pair<double,size_t>> pAction = make_pair(MovieGenre::ACTION.str,make_pair(0,0));
        genresMap.insert(pAction);
        pair<string,pair<double,size_t>> pAdventure = make_pair(MovieGenre::ADVENTURE.str,make_pair(0,0));
        genresMap.insert(pAdventure);
        pair<string,pair<double,size_t>> pAnimation = make_pair(MovieGenre::ANIMATION.str,make_pair(0,0));
        genresMap.insert(pAnimation);
        pair<string,pair<double,size_t>> pChildren = make_pair(MovieGenre::CHILDREN.str,make_pair(0,0));
        genresMap.insert(pChildren);
        pair<string,pair<double,size_t>> pComedy = make_pair(MovieGenre::COMEDY.str,make_pair(0,0));
        genresMap.insert(pComedy);
        pair<string,pair<double,size_t>> pCrime = make_pair(MovieGenre::CRIME.str,make_pair(0,0));
        genresMap.insert(pCrime);
        pair<string,pair<double,size_t>> pDocumentary = make_pair(MovieGenre::DOCUMENTARY.str,make_pair(0,0));
        genresMap.insert(pDocumentary);
        pair<string,pair<double,size_t>> pDrama = make_pair(MovieGenre::DRAMA.str,make_pair(0,0));
        genresMap.insert(pDrama);
        pair<string,pair<double,size_t>> pFantasy = make_pair(MovieGenre::FANTASY.str,make_pair(0,0));
        genresMap.insert(pFantasy);
        pair<string,pair<double,size_t>> pNoir = make_pair(MovieGenre::NOIR.str,make_pair(0,0));
        genresMap.insert(pNoir);
        pair<string,pair<double,size_t>> pHorror = make_pair(MovieGenre::HORROR.str,make_pair(0,0));
        genresMap.insert(pHorror);
        pair<string,pair<double,size_t>> pMusical = make_pair(MovieGenre::MUSICAL.str,make_pair(0,0));
        genresMap.insert(pMusical);
        pair<string,pair<double,size_t>> pMystery = make_pair(MovieGenre::MYSTERY.str,make_pair(0,0));
        genresMap.insert(pMystery);
        pair<string,pair<double,size_t>> pRomance = make_pair(MovieGenre::ROMANCE.str,make_pair(0,0));
        genresMap.insert(pRomance);
        pair<string,pair<double,size_t>> pSciFi = make_pair(MovieGenre::SCI_FI.str,make_pair(0,0));
        genresMap.insert(pSciFi);
        pair<string,pair<double,size_t>> pThriller = make_pair(MovieGenre::THRILLER.str,make_pair(0,0));
        genresMap.insert(pThriller);
        pair<string,pair<double,size_t>> pWar = make_pair(MovieGenre::WAR.str,make_pair(0,0));
        genresMap.insert(pWar);
        pair<string,pair<double,size_t>> pWestern = make_pair(MovieGenre::WESTERN.str,make_pair(0,0));
        genresMap.insert(pWestern);

        for(auto it=rdh->getAllMovies().begin();it!=rdh->getAllMovies().end();it++){
            vector<string> currentGenres = it->genres;
            for(auto it2=it->userEntries.begin();it2!=it->userEntries.end();it2++){
                for(auto it3 = currentGenres.begin();it3!=currentGenres.end();it3++){
                    string currentGenre = *it3;
                    map<string,pair<double,size_t>>::iterator found = genresMap.find(currentGenre);
                    found->second.first = found->second.first + it2->rating;
                    found->second.second = found->second.second + 1;
                }
            }
        }
        std::string highestRatedGenre;
        double highestRatedMeanRating = 0;
        for(auto it = genresMap.begin();it!=genresMap.end();it++){
            double ratingSum = it->second.first;
            size_t ratingCount = it->second.second;
            double currentMeanRating = 0;
            if(ratingCount > 0){
                currentMeanRating = ratingSum / double(ratingCount);
            }
            if(currentMeanRating > highestRatedMeanRating){
                highestRatedMeanRating = currentMeanRating;
                highestRatedGenre = it->first;
            }
        }
        result.genre = highestRatedGenre;
        result.meanRating = highestRatedMeanRating;
        return result;
    }
}

//utility functions:

bool movieMeanRatingComparator(Movie& a,Movie& b){
    return a.meanRating > b.meanRating; //descending order.
}

bool tagEntryTagCountComparator(TagEntry& a,TagEntry& b){
    return (a.tagCount > b.tagCount); //descending order
}

User extractUserElementWithUserId(vector<User>& users,ulong inputUserId){

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:extractUserElementWithUserId",LOGLEVEL);

	auto userIter = find_if(users.begin(),users.end(),[&inputUserId](User& usr) {return usr.userId == inputUserId;});

	if (userIter == users.end()){
        LOG.error("User with given ID was not found in memory");
        string exMsg = string("User with ID=")+to_string(inputUserId)+string(" was not found in memory.");
		throw UserNotFoundException(exMsg);
	}
	
	User user = *userIter;

    LOG.trace("Found user with ID="+to_string(inputUserId)+". Extracting User information.");
	return user;
}

User* extractUserElementPointerWithUserId(vector<User>& users,ulong inputUserId){

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:extractUserElementPointerWithUserId",LOGLEVEL);

    auto userIter = find_if(users.begin(),users.end(),[&inputUserId](User& usr) {return usr.userId == inputUserId;});

    if (userIter == users.end()){
        LOG.error("User with given ID was not found in memory");
        string exMsg = string("User with ID=")+to_string(inputUserId)+string(" was not found in memory.");
        throw UserNotFoundException(exMsg,inputUserId);
    }

    User* userPointer = &(*userIter);

    LOG.trace("Found user with ID="+to_string(inputUserId)+". Extracting User information.");
    return userPointer;
}

Movie extractMovieElementWithMovieId(vector<Movie>& movies,ulong inputMovieId){
	
	auto movieIter = find_if(movies.begin(), movies.end(), [&inputMovieId](Movie& movie) {return movie.movieId == inputMovieId; });

	if (movieIter == movies.end()){
        string exMsg = string("Movie with ID=") + to_string(inputMovieId) + string(" was not found in memory.");
		throw MovieNotFoundException(exMsg);
	}

	Movie movie = *movieIter;
	
	return movie;
}

MovieEntry extractMovieEntryElementWithMovieId(vector<MovieEntry>& movieEntries, ulong inputMovieId){

	auto movieEntryIter = find_if(movieEntries.begin(), movieEntries.end(), [&inputMovieId](MovieEntry& movieEntry){return movieEntry.movieId == inputMovieId; });

	if (movieEntryIter == movieEntries.end()){
        string exMsg = string("MovieEntry with ID=") + to_string(inputMovieId) + string(" was not found in memory.");
		throw MovieNotFoundException(exMsg);
	}

	MovieEntry movieEntry = *movieEntryIter;

	return movieEntry;
}

bool movieIdExistsInMovieEntryVector(ulong inputMovieId,vector<MovieEntry> movieEntries){
    auto movieEntryIter = find_if(movieEntries.begin(),movieEntries.end(),[&inputMovieId](MovieEntry& movieEntry){return movieEntry.movieId == inputMovieId;});
    if(movieEntryIter == movieEntries.end()){
        return false;
    } else{
        return true;
    }
}

double extractRatingFromMovieEntryVector(ulong inputMovieId,vector<MovieEntry>& movieEntries){
   MovieEntry foundMovieEntry = extractMovieEntryElementWithMovieId(movieEntries,inputMovieId);
   return foundMovieEntry.rating;
}


MovieRecommendation& extractMovieRecommendationReferenceFromId(vector<MovieRecommendation>& movieRecommendations, ulong inputMovieId){
	
	auto movieRecommendationIter = find_if(movieRecommendations.begin(), movieRecommendations.end(), [&inputMovieId](MovieRecommendation& movieRecommendation){return movieRecommendation.movieId == inputMovieId; });

	if (movieRecommendationIter == movieRecommendations.end()){
		static MovieRecommendation emptyRecommendation; //TODO: probably unsafe?
		emptyRecommendation.exists = false;
		return emptyRecommendation;
	}

    //static MovieRecommendation movieRecommendation = *movieRecommendationIter;
    return *movieRecommendationIter;
}

bool movieExistsInVector(MovieEntry& inputMovie, vector<MovieEntry>& moviesVec){
	
	auto movieEntryIter = find_if(moviesVec.begin(), moviesVec.end(), [&inputMovie](MovieEntry& mv){return mv.movieId == inputMovie.movieId; });
	
	if(movieEntryIter == moviesVec.end()){
		return false;
	}
	return true;
}

bool neighborMovieExistsInVector(NeighborMovie& inputNeighborMovie, vector<MovieEntry>& moviesVec){
	
	auto neighborMovieEntryIter = find_if(moviesVec.begin(), moviesVec.end(), [&inputNeighborMovie](MovieEntry& mv){return mv.movieId == inputNeighborMovie.id; });
	
	if(neighborMovieEntryIter == moviesVec.end()){
		return false;
	}
	return true;
}

void removeWatchedMoviesFromNeighborMoviesVector(vector<NeighborMovie>& moviesVec,vector<MovieEntry>& watchedMovies){

    if(moviesVec.size() == 0 || watchedMovies.size() == 0){
        return;
    }
	
    moviesVec.erase(remove_if(moviesVec.begin(), moviesVec.end(), [&watchedMovies](NeighborMovie& mv){return neighborMovieExistsInVector(mv, watchedMovies); }),moviesVec.end());
	
}

double calculateZScore(double rating,double meanRating,double standardDeviation){

    RecommenderLogger LOG;
    LOG.init("recommenderDataHandler:calculateZScore",LOGLEVEL);

    LOG.debug("Calculating Z-score with inputRating="+to_string(rating)+" meanRating="+to_string(meanRating)+" and standardDeviation="+to_string(standardDeviation));

    if(realNumEqualWithinError(standardDeviation,0,0.0001)){
        LOG.trace("Deviation is zero,this rating will not be taken into account. (returning zero value)");
        LOG.close();
        return 0; //if no deviation exists, this rating should NOT be taken into account.
    }

    LOG.close();
	return (static_cast<double>(rating)-meanRating)/standardDeviation;
}

//if penaltyEnabled=true, items with zero deviation will have their result reduced.
double calculateInverseZScore(double normalizedRating,double meanRating,double standardDeviation,bool penaltyEnabled=false){

    RecommenderLogger LOG;
    LOG.init("recommenderDataHandler:calculateInverseZScore",LOGLEVEL);

    LOG.debug("Calculating inverse Z-Score with normalizedRating="+to_string(normalizedRating)+", meanRating="+to_string(meanRating)+", stdDev="+to_string(standardDeviation));

    //if no deviation exists, the predicted rating will be identical to the current mean rating.
    //TODO: should this be done in the case of item-based recommendation? It may skew results in favor of low/zero deviation items.
    double result = standardDeviation*normalizedRating + meanRating;
    if(penaltyEnabled && realNumEqualWithinError(standardDeviation,0,0.0001)){
        LOG.debug("Penalizing item because of zero deviation.");
        result = result / 1.5; //TODO: penalty factor should be in configuration.
    }
    LOG.debug("Z-Score calc result="+to_string(result));
    LOG.close();
    return result;
}


NormalizedRatingPredictionWrapper calculatePredictedRatingUserBasedNormalized(double weight,double neighborNormalizedRating){
	
	double numerator = weight*neighborNormalizedRating;
	double denominator = abs(weight);
	
	NormalizedRatingPredictionWrapper result;
	result.predictedRatingNormalized = numerator / denominator;
	result.currentWeightSum = denominator;
	
	return result;
}

NormalizedRatingPredictionWrapper updatePredictedRatingUserBasedNormalized(double weight,double neighborNormalizedRating,double oldPredictedNormalizedRating,double oldWeightSum){

	double newNumerator = (oldWeightSum*oldPredictedNormalizedRating + weight*neighborNormalizedRating);
	double newDenominator = oldWeightSum + abs(weight);
	
	NormalizedRatingPredictionWrapper result;
	result.predictedRatingNormalized = newNumerator / newDenominator;
	result.currentWeightSum = newDenominator;
	
	return result;
}

double calculatePredictedRatingItemBasedNormalized(vector<double>& weights,vector<double> normalizedRatings){

    double numerator = calculateSumMultiplicative(&weights[0],&normalizedRatings[0],weights.size());
    double denominator = calculateSumOfAbsolutes(&weights[0],weights.size());

    double result = numerator / denominator;
    return result;
}

vector<TagEntry> getMostCommonTagsFromMovie(Movie& movie,unsigned int numberToKeep){
    vector<TagEntry> allTags = movie.totalTags;
    //sort by tagCount
    sort(allTags.begin(),allTags.end(),tagEntryTagCountComparator);
    //keep a subset of the tagEntries (based on 'numberToKeep')
    auto first = allTags.begin();
    vector<TagEntry>::iterator last;
    if(numberToKeep < allTags.size()){
        //last = allTags.begin() + numberToKeep+1;
        last = allTags.begin() + numberToKeep;
    } else{
        last = allTags.end();
    }
    vector<TagEntry> mostCommonTags = vector<TagEntry>(first,last);
    return mostCommonTags;
}

void keepOnlyUserWatchedFromNeighborMovies(vector<NeighborMovie>& neighborMovies,vector<MovieEntry> watchedMovies){
    //remove from neighborMovies only if movie is NOT in watchedMovies.
    neighborMovies.erase(remove_if(neighborMovies.begin(),neighborMovies.end(),[&watchedMovies](NeighborMovie& mv){return !movieIdExistsInMovieEntryVector(mv.id,watchedMovies);}),neighborMovies.end());
}

bool strIsContainedInGenres(string& str,vector<MovieGenre>& genres){
    for(auto it=genres.begin();it!=genres.end();it++){
        if(it->equals(str)){
            return true;
        }
    }
    return false;
}

bool strIsContainedInTags(Configuration* config,TagEntry& tg,vector<string>& tagNames){
    for(auto it=tagNames.begin();it!=tagNames.end();it++){
       if(strsSimilarWithinLimit(static_cast<unsigned int>(config->stringSimilarityMethod),tg.tagName,*it,config->similarityThreshold)){
           return true;
       }
    }
    return false;
}

// // // // //

//very lenient: at least one genre must be contained in vec.
bool genresAreContainedInVecVeryLenient(vector<string>& vec,vector<MovieGenre>& genres){
    for(auto it=vec.begin();it!=vec.end();it++){
        if(strIsContainedInGenres(*it,genres)){
            return true;
        }
    }
    return false;
}

//lenient: all given genres must be contained in vec,but vec can have more.
bool genresAreContainedInVecLenient(vector<string>& vec,vector<MovieGenre>& genres){
    for(auto it=genres.begin();it!=genres.end();it++){
        string this_genre_str = it->str;
        auto isContained = find_if(vec.begin(),vec.end(),[&this_genre_str](string str){return str ==this_genre_str;});
        if(isContained == vec.end()){
            return false;
        }
    }
    return true;
}

bool genresAreContainedInVecStrict(vector<string>& vec,vector<MovieGenre>& genres){
    if(vec.size() != genres.size()){
        return false;
    }
    for(auto it=vec.begin();it!=vec.end();it++){
        if(!strIsContainedInGenres(*it,genres)){
            return false;
        }
    }
    return true;
}

// // // // //

bool tagNamesAreContainedInVecVeryLenient(Configuration* config,vector<TagEntry>& vec,vector<string>& tagNames){
    for(auto it=vec.begin();it!=vec.end();it++){
        if(strIsContainedInTags(config,*it,tagNames)){
            return true;
        }
    }
    return false;
}

bool tagNamesAreContainedInVecLenient(Configuration* config,vector<TagEntry>& vec,vector<string>& tagNames){
    for(auto it=tagNames.begin();it!=tagNames.end();it++){
        string this_tagname_str = *it;
        auto isContained = find_if(vec.begin(),vec.end(),[&this_tagname_str,&config](TagEntry tg)
        {return strsSimilarWithinLimit(static_cast<unsigned int>(config->stringSimilarityMethod),tg.tagName,this_tagname_str,config->similarityThreshold);});
        if(isContained == vec.end()){
            return false;
        }
    }
    return true;
}

bool tagNamesAreContainedInVecStrict(Configuration* config,vector<TagEntry>& vec,vector<string>& tagNames){
    if(vec.size() != tagNames.size()){
        return false;
    }
    for(auto it=vec.begin();it!=vec.end();it++){
        if(!strIsContainedInTags(config,*it,tagNames)){
            return false;
        }
    }
    return true;
}

// // // // //

void filterVectorWithGenreCriteriaStrict(vector<Movie>& movies,vector<MovieGenre>& genres){
    vector<Movie> moviesSubset;
    for(auto it=movies.begin();it!=movies.end();it++){
        if(genresAreContainedInVecStrict(it->genres,genres)){
            moviesSubset.push_back(*it);
        }
    }
    movies.clear();
    movies = moviesSubset;
}

void filterVectorWithGenreCriteriaLenient(vector<Movie>& movies,vector<MovieGenre>& genres){
    vector<Movie> moviesSubset;
    for(auto it=movies.begin();it!=movies.end();it++){
        if(genresAreContainedInVecLenient(it->genres,genres)){
            moviesSubset.push_back(*it);
        }
    }
    movies.clear();
    movies = moviesSubset;

}
void filterVectorWithGenreCriteriaVeryLenient(vector<Movie>& movies,vector<MovieGenre>& genres){
    vector<Movie> moviesSubset;
    for(auto it=movies.begin();it!=movies.end();it++){
        if(genresAreContainedInVecVeryLenient(it->genres,genres)){
            moviesSubset.push_back(*it);
        }
    }
    movies.clear();
    movies = moviesSubset;
}

// // // // //

void filterVectorWithGenreCriteria(vector<Movie>& movies,vector<MovieGenre>& genres,char filteringType){
    switch(filteringType){
        case '1': //strict
        filterVectorWithGenreCriteriaStrict(movies,genres);
        break;
        case '2': //lenient
        filterVectorWithGenreCriteriaLenient(movies,genres);
        break;
        case '3': //very lenient
        filterVectorWithGenreCriteriaVeryLenient(movies,genres);
        break;
        default:
        throw InvalidStateException("Unrecognized filtering type for genres: "+to_string(filteringType));
    }
}

// // // ////

void filterVectorWithTagCriteriaStrict(Configuration* config,vector<Movie>& movies,vector<string>& tagNames){
    vector<Movie> moviesSubset;
    for(auto it=movies.begin();it!=movies.end();it++){
        if(tagNamesAreContainedInVecStrict(config,it->totalTags,tagNames)){
            moviesSubset.push_back(*it);
        }
    }
    movies.clear();
    movies = moviesSubset;
}

void filterVectorWithTagCriteriaLenient(Configuration* config,vector<Movie>& movies,vector<string>& tagNames){
    vector<Movie> moviesSubset;
    for(auto it=movies.begin();it!=movies.end();it++){
        if(tagNamesAreContainedInVecLenient(config,it->totalTags,tagNames)){
            moviesSubset.push_back(*it);
        }
    }
    movies.clear();
    movies = moviesSubset;
}

void filterVectorWithTagCriteriaVeryLenient(Configuration* config,vector<Movie>& movies,vector<string>& tagNames){
    vector<Movie> moviesSubset;
    for(auto it=movies.begin();it!=movies.end();it++){
        if(tagNamesAreContainedInVecVeryLenient(config,it->totalTags,tagNames)){
            moviesSubset.push_back(*it);
        }
    }
    movies.clear();
    movies = moviesSubset;
}

// // // // //

void filterVectorWithTagCriteria(Configuration* config,vector<Movie>& movies,vector<string>& tagNames,char filteringType){
    switch(filteringType){
        case '1':
        filterVectorWithTagCriteriaStrict(config,movies,tagNames);
        break;
        case '2':
        filterVectorWithTagCriteriaLenient(config,movies,tagNames);
        break;
        case '3':
        filterVectorWithTagCriteriaVeryLenient(config,movies,tagNames);
        break;
        default:
        throw InvalidStateException("Unrecognized filtering type for tags: "+to_string(filteringType));
    }
}

// // // // //

//

//RecommenderDataHandler METHOD DEFINITIONS:

//

Movie RecommenderDataHandler::extractMovieFromMovieId(ulong movieId){

    if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
    }
    else{
        Movie movie = extractMovieElementWithMovieId(allMovies,movieId);
        return movie;
    }
}

NormalizationParameters RecommenderDataHandler::extractNormalizationParametersFromMovieId(ulong movieId,vector<MovieEntry>& watchedMovies){

    NormalizationParameters normParams;

    //rating
    normParams.rating = extractRatingFromMovieEntryVector(movieId,watchedMovies);

    //meanRating,standardDeviation
    Movie mv = extractMovieFromMovieId(movieId);
    normParams.meanRating = mv.meanRating;
    normParams.standardDeviation = mv.ratingsStandardDeviation;

    return normParams;
}


/*
Calculates the following for a movie recommendation:
-denormalized predicted rating.
 -mostCommonTags
 -meanRating
 -meanRatingFromNeighbors
*/
void RecommenderDataHandler::finalizeMovieRecommendationUserBased(MovieRecommendation& movieRecommendation,User& currentUser){

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:finalizeMovieRecommendationUserBased",LOGLEVEL);
    LOG.debug("Finalizing user-based recommendation for userID="+to_string(currentUser.userId)+", movieID="+to_string(movieRecommendation.movieId));
    Movie movie = extractMovieElementWithMovieId(allMovies,movieRecommendation.movieId);
    //movieRecommendation.predictedRating = calculateInverseZScore(movieRecommendation.predictedRatingNormalized,movie.meanRating,movie.ratingsStandardDeviation);
    movieRecommendation.predictedRating = calculateInverseZScore(movieRecommendation.predictedRatingNormalized,currentUser.meanRating,currentUser.ratingsStandardDeviation);
    LOG.trace("final predicted rating (denormalized)="+to_string(movieRecommendation.predictedRating));
    movieRecommendation.mostCommonTags = getMostCommonTagsFromMovie(movie,5); //TODO: numberToKeep to be obtained from configuration?
    movieRecommendation.meanRating = movie.meanRating;
    movieRecommendation.genres = movie.genres;
    LOG.debug("Finalized movie recommendation for movieId="+to_string(movieRecommendation.movieId)+": predictedRating="+to_string(movieRecommendation.predictedRating));
    LOG.close();
}


Movie RecommenderDataHandler::convertMovieEntryToMovieObject(MovieEntry& movieEntry) {

	return extractMovieElementWithMovieId(allMovies,movieEntry.movieId);
}

Movie RecommenderDataHandler::convertNeighborMovieToMovieObject(NeighborMovie& neighborMovie){
    return extractMovieElementWithMovieId(allMovies,neighborMovie.id);
}

MovieRecommendation RecommenderDataHandler::predictRatingUserBasedMethodWithDB(){
	return MovieRecommendation();//TODO
}

MovieRecommendation RecommenderDataHandler::predictRatingUserBasedMethodWithInMemoryDataRealTimeCalculations(){
	return MovieRecommendation();//TODO
}

MovieRecommendation RecommenderDataHandler::predictRatingUserBasedMethodWithInMemoryData(NeighborUser& neighborUser,MovieEntry& movieEntry){

    MovieRecommendation movieRecommendation;

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:predictRatingUserBasedMethodWithInMemoryData",LOGLEVEL);

    LOG.debug("START predictRatingUserBasedMethodWithInMemoryData. neighborUserId="+to_string(neighborUser.id)+" movieEntryId="+to_string(movieEntry.movieId));

	User neighborInfo = extractUserInfoFromId(neighborUser.id);
    double neighborRating = extractMovieEntryElementWithMovieId(neighborInfo.movieEntries,movieEntry.movieId).rating;
	
	//1: get the inverse user frequency for the movie.
    //double iuf = getInverseUserFrequency(movieEntry.movieId); //TODO:DEPRECATED: this is calculated at data import.
	
	//2: get weight.
	double w = neighborUser.weight;
    LOG.trace("neighbor weight="+to_string(w));

    //TODO: is this the right approach for zero weight?
    if(realNumEqualWithinError(w,0,0.0001)){
        LOG.trace("Weight is zero, will ignore this neighbor for this movie.");
        NormalizedRatingPredictionWrapper wrapper;
        wrapper.currentWeightSum = 0;
        wrapper.predictedRatingNormalized = 0;
        Movie movie = extractMovieElementWithMovieId(allMovies,movieEntry.movieId);
        movieRecommendation.setMovieRecommendationFromUserBasedMethod(wrapper.predictedRatingNormalized,movie.movieId,movie.movieName,movie.genres,movie.meanRating,neighborRating,wrapper.currentWeightSum);
        LOG.close();
        return movieRecommendation;
    }
		
	//3: get normalized rating of neighbor.
	double neighborNormalizedRating = calculateZScore(neighborRating,neighborInfo.meanRating,neighborInfo.ratingsStandardDeviation);
    LOG.trace("z-score normalized rating="+to_string(neighborNormalizedRating));

    //4:get predicted rating based on the above.
	NormalizedRatingPredictionWrapper wrapper = calculatePredictedRatingUserBasedNormalized(w,neighborNormalizedRating);
    LOG.trace("current normalized predicted rating="+to_string(wrapper.predictedRatingNormalized));
    LOG.trace("current weight sum="+to_string(wrapper.currentWeightSum));
	
	Movie movie = extractMovieElementWithMovieId(allMovies,movieEntry.movieId);
	
    movieRecommendation.setMovieRecommendationFromUserBasedMethod(wrapper.predictedRatingNormalized,movie.movieId,movie.movieName,movie.genres,movie.meanRating,neighborRating,wrapper.currentWeightSum);
	
    LOG.close();
	return movieRecommendation; 
}

MovieRecommendation RecommenderDataHandler::updateRatingPredictionUserBasedMethodWithInMemoryData(NeighborUser& neighborUser,MovieEntry& movieEntry,MovieRecommendation& movieRecommendation){
	
    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:updateRatingPredictionUserBasedMethodWithInMemoryData",LOGLEVEL);

    LOG.debug("START: updateRatingPredictionUserBasedMethodWithInMemoryData. neighborUserId="+to_string(neighborUser.id));

    User neighborInfo = extractUserInfoFromId(neighborUser.id);
    double neighborRating = extractMovieEntryElementWithMovieId(neighborInfo.movieEntries,movieEntry.movieId).rating;
	
	//1: get the inverse user frequency for the movie.
    //double iuf = getInverseUserFrequency(movieEntry.movieId); //TODO:DEPRECATED: this is calculated at data import.
	
	//2: get weight.
	double w = neighborUser.weight;
    LOG.trace("neighbor weight="+to_string(w));

    //if zero weight or zero stdDeviation:
    if(realNumEqualWithinError(w,0,0.0001) || realNumEqualWithinError(neighborInfo.ratingsStandardDeviation,0,0.0001)){
        LOG.trace("Weight or standardDeviation is zero, will ignore this neighbor for this movie.");
        return movieRecommendation; //return movieRecommendation unchanged.
    }
		
	//3: get normalized rating of neighbor.
	double neighborNormalizedRating = calculateZScore(neighborRating,neighborInfo.meanRating,neighborInfo.ratingsStandardDeviation);
    LOG.trace("neighbor normalized rating="+to_string(neighborNormalizedRating));

    //4:get predicted rating based on the above.
	NormalizedRatingPredictionWrapper wrapper = updatePredictedRatingUserBasedNormalized(w,neighborNormalizedRating,movieRecommendation.predictedRatingNormalized,movieRecommendation.currentWeightSum);
    LOG.trace("current normalized predicted rating="+to_string(wrapper.predictedRatingNormalized));
    LOG.trace("current weight sum="+to_string(wrapper.currentWeightSum));

	Movie movie = extractMovieElementWithMovieId(allMovies,movieEntry.movieId);
	
    movieRecommendation.calculateNewMeanRatingFromNeighbors(neighborRating);
    LOG.trace("current meanRatingFromNeighbors="+to_string(movieRecommendation.meanRatingFromNeighbors));
	movieRecommendation.updateMovieRecommendationFromUserBasedMethod(wrapper.predictedRatingNormalized,movieRecommendation.meanRatingFromNeighbors,wrapper.currentWeightSum);
	
    LOG.close();
	return movieRecommendation; 
}

/*
 * neighborMovies = contains the neighbor movies of a target item, that the target user has watched (they should be filtered before this function call).
*/
MovieRecommendation RecommenderDataHandler::predictRatingItemBasedMethodWithInMemoryData(vector<NeighborMovie>& neighborMovies,vector<MovieEntry>& watchedMovies,Movie& targetMovie){

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:predictRatingItemBasedMethodWithInMemoryData",LOGLEVEL);

    LOG.debug("START: predictRatingItemBasedMethodWithInMemoryData. targetMovieID="+to_string(targetMovie.movieId));

    vector<double> weights;
    vector<double> normalizedRatings;
    //for each neighbor:
    LOG.trace("For each neighbor of "+to_string(targetMovie.movieId)+", getting normalization parameters.");
    for(size_t i=0;i<neighborMovies.size();i++){
        LOG.trace("Calculating normalization parameters for "+to_string(neighborMovies[i].id));
        //1:get weights.
        weights.push_back(neighborMovies[i].weight);
        //2:get normalized ratings of neighbors (ratings are from target user).
        NormalizationParameters normParams = extractNormalizationParametersFromMovieId(neighborMovies[i].id,watchedMovies);
        normalizedRatings.push_back(calculateZScore(normParams.rating,normParams.meanRating,normParams.standardDeviation));
    }

    //3:get normalized predicted rating.
    double predictedRatingNormalized = calculatePredictedRatingItemBasedNormalized(weights,normalizedRatings);
    LOG.trace("predictedRatingNormalized="+to_string(predictedRatingNormalized));
    //4:get denormalized predicted rating.
    double predictedRating = calculateInverseZScore(predictedRatingNormalized,targetMovie.meanRating,targetMovie.ratingsStandardDeviation,true);
    LOG.trace("predictedRating (denormalized)="+to_string(predictedRating));
    //5:get most common tags.
    vector<TagEntry> mostCommonTags = getMostCommonTagsFromMovie(targetMovie,5);
    //6:get neighborMovie names
    vector<string> neighborMovieNames;
    for(auto it=neighborMovies.begin();it!=neighborMovies.end();it++){
        neighborMovieNames.push_back(extractMovieFromMovieId(it->id).movieName);
    }
    MovieRecommendation movieRecommendation;
    movieRecommendation.setMovieRecommendationFromItemBasedMethod(predictedRatingNormalized,predictedRating,targetMovie.movieId, targetMovie.movieName, targetMovie.genres,
                                                                  mostCommonTags, targetMovie.meanRating, neighborMovieNames,static_cast<unsigned long>(neighborMovies.size()));
    return movieRecommendation;
}


MovieRecommendation RecommenderDataHandler::predictRatingItemBasedMethodWithDB(){
    return MovieRecommendation(); //TODO
}

MovieRecommendation RecommenderDataHandler::predictRatingItemBasedMethodWithInMemoryDataRealTimeCalculations(){
    return MovieRecommendation(); //TODO
}

MovieRecommendation RecommenderDataHandler::updateRatingPredictionItemBasedMethod(){
    return MovieRecommendation(); //TODO: PROBABLY OBSOLETE.
}

size_t RecommenderDataHandler::getMovieUserRatingsCount(ulong movieId){
	
	if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
	}
	
	else{
			Movie movie = extractMovieElementWithMovieId(allMovies,movieId);
			return movie.userEntries.size();
	}
}

double RecommenderDataHandler::getInverseUserFrequency(ulong movieId){
	
    size_t movieUserRatingsCount = getMovieUserRatingsCount(movieId);
	
	if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
	}
	
	else{
		if(config->realTimeCalculations){
            throw NotYetImplementedException(FunctionalityNames::ON_DEMAND_CALC); //TODO:implement
		}
		else{
			double iuf;
            iuf = double(getUserCount()) / double(movieUserRatingsCount);
			iuf = log10(iuf);
			return iuf;
		}
	}
}

bool RecommenderDataHandler::addSingleUser(User& newUser){

    //TODO: check for id?
    if(config->useDatabase){

    }

    else{
        size_t beforeSize = allUsers.size();
        allUsers.push_back(newUser);
        size_t afterSize = allUsers.size();
        if(afterSize == beforeSize + 1){
            return true;
        } else{
            return false;
        }
    }
    return false; //if execution reaches this point, something went wrong.
}

bool RecommenderDataHandler::addSingleMovie(Movie& newMovie){

    if(config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
    }

    else{
        size_t beforeSize = allMovies.size();
        allMovies.push_back(newMovie);
        size_t afterSize = allMovies.size();
        if(afterSize == beforeSize+1){
            return true;
        } else{
            return false;
        }
    }
    return false; //if execution reaches this point, something went wrong.
}

size_t RecommenderDataHandler::getUserCount(){
	
	if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
	}
	
	else{
		return allUsers.size();
	}
}

size_t RecommenderDataHandler::getMovieCount(){
	
	if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
	}
	
	else{
		return allMovies.size();
	}
}

User RecommenderDataHandler::extractUserInfoFromId(ulong targetUserId){
	
	if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
	}
	
	else {
		User user = extractUserElementWithUserId(allUsers, targetUserId);
		return user;
	}
}

User* RecommenderDataHandler::extractUserInfoPointerFromId(ulong targetUserId){

    if(config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE); //TODO:implement
    }

    else {
        User* userpointer = extractUserElementPointerWithUserId(allUsers,targetUserId);
        return userpointer;
    }
}

vector<MovieEntry> RecommenderDataHandler::extractWatchedMovies(User& targetUser){
	
	return targetUser.movieEntries;
}

vector<NeighborUser> RecommenderDataHandler::extractNearestNeighborUsers(User& targetUser){
	
	if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE);
	}
	
	else {
		if(config->realTimeCalculations){
            throw NotYetImplementedException(FunctionalityNames::ON_DEMAND_CALC);
		}
		else{
			return targetUser.neighbors;
		}
	}
}

vector<NeighborMovie> RecommenderDataHandler::extractNearestNeighborMovies(Movie& targetMovie){
	
	if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE);
	}
	
	else{
		if(config->realTimeCalculations){
            throw NotYetImplementedException(FunctionalityNames::ON_DEMAND_CALC);
		}
		else{
			return targetMovie.neighbors;
		}
	}
}

vector<MovieEntry> RecommenderDataHandler::extractWatchedMoviesFromUserId(ulong userId){

    RecommenderLogger LOG;
    LOG.init("recommenderDataHandler:extractWatchedMoviesFromUserId",LOGLEVEL);

    LOG.trace("Extracting watched movies from userId="+to_string(userId));
	
	User targetUser = extractUserInfoFromId(userId);
	
	vector<MovieEntry> watchedMovies = extractWatchedMovies(targetUser);

    LOG.close();
	return watchedMovies;
}

Movie RecommenderDataHandler::getMovieFromMovieEntry(MovieEntry& movieEntry){
	
	return extractMovieFromMovieId(movieEntry.movieId);
}


MovieRecommendation RecommenderDataHandler::predictRatingUserBasedMethod(NeighborUser& neighborUser,MovieEntry& movieEntry){
	
	if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE);
		//return predictRatingUserBasedMethodWithDB();
	}
	else if(config->realTimeCalculations){
        throw NotYetImplementedException(FunctionalityNames::ON_DEMAND_CALC);
		//return predictRatingUserBasedMethodWithInMemoryDataRealTimeCalculations();
	}
	else{
		return predictRatingUserBasedMethodWithInMemoryData(neighborUser,movieEntry);
	}	
}

MovieRecommendation RecommenderDataHandler::updateRatingPredictionUserBasedMethod(NeighborUser& neighborUser,MovieEntry& movieEntry,MovieRecommendation& movieRecommendation){
	
    if (config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE);
	}
	else if(config->realTimeCalculations){
        throw NotYetImplementedException(FunctionalityNames::ON_DEMAND_CALC);
	}
	else{
		return updateRatingPredictionUserBasedMethodWithInMemoryData(neighborUser,movieEntry,movieRecommendation);
	}
}

MovieRecommendation RecommenderDataHandler::predictRatingItemBasedMethod(vector<NeighborMovie>& neighborMovies,vector<MovieEntry>& watchedMovies,Movie& targetMovie){
    if (config->useDatabase){
        //return predictRatingItemBasedMethodWithDB();
        return MovieRecommendation();
    }
    else if(config->realTimeCalculations){
        //return predictRatingItemBasedMethodWithInMemoryDataRealTimeCalculations();
        return MovieRecommendation();
    }
    else{
        return predictRatingItemBasedMethodWithInMemoryData(neighborMovies,watchedMovies,targetMovie);
    }
}


vector<MovieRecommendation> RecommenderDataHandler::extractPredictedRatingsUserBased(vector<NeighborUser>& nearestNeighbors,vector<MovieEntry>& watchedMovies,User& currentUser,bool forStandaloneProfile){
	
    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:extractPredictedRatingsUserBased",LOGLEVEL);
    LOG.info("START: extractPredictedRatingsUserBased");

	vector<MovieRecommendation> movieRecommendations;
	vector<MovieEntry> moviesToCheck;

    if(nearestNeighbors.size() == 0){
        LOG.warn("userID="+to_string(currentUser.userId)+" has no neighbors! Cannot make a user-based personalized recommendation.");
        LOG.info("extractPredictedRatingsUserBased finished.");
        LOG.close();
        return movieRecommendations;
    }
	
	//for each nearest neighbor: 
    for(size_t i=0;i<nearestNeighbors.size();i++){
        LOG.debug("Processing nearest neighbor="+to_string(nearestNeighbors[i].id));
		if(config->useLimitInWatchedMoviesUserBasedMethod){
			//extract their favorite movies and predict target user rating: filter out those that the target user has already watched.
            vector<MovieEntry> watchedMoviesByNeighbor = extractWatchedMoviesFromUserId(nearestNeighbors[i].id);
            vector<MovieEntry>::const_iterator first = watchedMoviesByNeighbor.begin();
            vector<MovieEntry>::const_iterator last;
            if(config->maxWatchedMovieCountUsedInUserBasedMethod <= watchedMoviesByNeighbor.size()){
                //last = predictedRatings.begin() + config.recommendationCount+1;
                last = watchedMoviesByNeighbor.begin() + config->maxWatchedMovieCountUsedInUserBasedMethod;
            } else{
                last = watchedMoviesByNeighbor.end();
            }
            vector<MovieEntry> watchedMoviesByNeighborSubset = vector<MovieEntry>(first,last);
            LOG.debug("Extracted subset of user favorite movies, watchedMoviesByNeighborToCheck. size="+to_string(watchedMoviesByNeighborSubset.size()));
            //TODO:is it faster to remove all watched movies from user here (instead of checking one-by-one)
            LOG.debug("Start processing each movie to check by neighbor="+to_string(nearestNeighbors[i].id));
            for(size_t j=0;j<watchedMoviesByNeighborSubset.size();j++){
                if(movieExistsInVector(watchedMoviesByNeighborSubset[j],watchedMovies)){
                    LOG.trace("watched movie by neighbor="+to_string(watchedMoviesByNeighborSubset[j].movieId)+" (movieID) is already watched by user, will not process it.");
                    continue;
                }

                if(realNumEqualWithinError(watchedMoviesByNeighborSubset[j].rating,0,0.00001)){
                    LOG.trace("neighbor has not rated this movie, moving to the next movie. neighbor= "+to_string(nearestNeighbors[i].id)+", movie="+to_string(watchedMoviesByNeighborSubset[j].movieId));
                    continue;
                }

                MovieRecommendation& movieRecommendation = extractMovieRecommendationReferenceFromId(movieRecommendations, watchedMoviesByNeighborSubset[j].movieId);
                if (movieRecommendation.exists){
                    LOG.trace("Updating prediction for movieId="+to_string(watchedMoviesByNeighborSubset[j].movieId));
                    updateRatingPredictionUserBasedMethod(nearestNeighbors[i], watchedMoviesByNeighborSubset[j], movieRecommendation);
                }
                else{
                    LOG.trace("Initializing prediction for movieId="+to_string(watchedMoviesByNeighborSubset[j].movieId));
                    movieRecommendations.push_back(predictRatingUserBasedMethod(nearestNeighbors[i], watchedMoviesByNeighborSubset[j]));
                }
            }
		}
		else{
			//extract all of their movies and predict target user rating: filter out those that the target user has already watched.
			vector<MovieEntry> watchedMoviesByNeighbor = extractWatchedMoviesFromUserId(nearestNeighbors[i].id);
            LOG.debug("Extracted watchedMoviesByNeighbor, size="+to_string(watchedMoviesByNeighbor.size()));
            //TODO: is it faster to remove all watched movies from user here (instead of checking one-by-one).
            //calculate/update predicted rating and other recommendation data for each watched movie of the neighbor:
            LOG.debug("Start processing each watched movie by neighbor="+to_string(nearestNeighbors[i].id));
            for(size_t j=0;j<watchedMoviesByNeighbor.size();j++){
                LOG.trace("Processing watched movie by neighbor with id="+to_string(watchedMoviesByNeighbor[j].movieId));
				if(movieExistsInVector(watchedMoviesByNeighbor[j],watchedMovies)){
                    LOG.trace("watched movie by neighbor="+to_string(watchedMoviesByNeighbor[j].movieId)+" (movieID) is already watched by user, will not process it.");
					continue;
				}
		
                if(realNumEqualWithinError(watchedMoviesByNeighbor[j].rating,0,0.00001)){
                    LOG.trace("neighbor has not rated this movie, moving to the next movie. neighbor= "+to_string(nearestNeighbors[i].id)+", movie="+to_string(watchedMoviesByNeighbor[j].movieId));
                    continue;
                }
				MovieRecommendation& movieRecommendation = extractMovieRecommendationReferenceFromId(movieRecommendations, watchedMoviesByNeighbor[j].movieId);
				if (movieRecommendation.exists){
                    LOG.trace("Updating prediction for movieId="+to_string(watchedMoviesByNeighbor[j].movieId));
					updateRatingPredictionUserBasedMethod(nearestNeighbors[i], watchedMoviesByNeighbor[j], movieRecommendation);
				}
				else{
                    LOG.trace("Initializing prediction for movieId="+to_string(watchedMoviesByNeighbor[j].movieId));
					movieRecommendations.push_back(predictRatingUserBasedMethod(nearestNeighbors[i], watchedMoviesByNeighbor[j]));
				}
			}
		}
	}
    for(unsigned long i=0;i<movieRecommendations.size();i++){
        finalizeMovieRecommendationUserBased(movieRecommendations[i],currentUser);
    }

    LOG.info("extractPredictedRatingsUserBased finished.");
    LOG.close();

	return movieRecommendations;
}

/*
moviesToCheck: the subset of the user's favorite movies
watchedMovies: all of the user's watched movies.

If no limit was used, moviesToCheck == watchedMovies.
*/
vector<MovieRecommendation> RecommenderDataHandler::extractPredictedRatingsItemBased(vector<MovieEntry>& moviesToCheck,vector<MovieEntry>& watchedMovies,bool forStandaloneProfile){

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:extractPredictedRatingsItemBased",LOGLEVEL);
    LOG.info("START: extractPredictedRatingsItemBased");

    LOG.trace("MoviesToCheckSize="+to_string(moviesToCheck.size()) +". WatchedMoviesSize="+to_string(watchedMovies.size()));

    vector<MovieRecommendation> movieRecommendations;

    //for each movie to check: extract neighbor movies, and remove all those that the user has already watched.
    LOG.debug("For each movie to check, extracting neighbor movies, and removing all those that the user has already watched.");
    vector<NeighborMovie> totalNeighborMoviesRecCandidates; //the movies that are candidates for recommendation.
    for(size_t i=0;i<moviesToCheck.size();i++){

        /* //TODO: Discard movies not rated by the user (and only tagged)?
        if(realNumEqualWithinError(moviesToCheck[i].rating,0,0.1)){
            LOG.trace("movieToCheck="+to_string(moviesToCheck[i].movieId)+" has not been rated by the user. Ignoring it.");
            continue;
        }
        */

        LOG.trace("Processing movieToCheck="+to_string(moviesToCheck[i].movieId));
        Movie movieToCheckConverted = convertMovieEntryToMovieObject(moviesToCheck[i]);
        vector<NeighborMovie> neighborMovies = extractNearestNeighborMovies(movieToCheckConverted); //TODO: check if correct.
        LOG.trace("movieToCheck="+to_string(moviesToCheck[i].movieId)+" has "+to_string(neighborMovies.size())+" neighbors.");
        //remove watched movies from neighborMovies.
        removeWatchedMoviesFromNeighborMoviesVector(neighborMovies,watchedMovies);
        LOG.trace("Removed watched movies from neighbors, remaining="+to_string(neighborMovies.size()));
        //add neighborMovies to totalNeighborMovies.
        totalNeighborMoviesRecCandidates.insert(totalNeighborMoviesRecCandidates.end(),neighborMovies.begin(),neighborMovies.end());
    }

    LOG.debug("Total candidate movies for recommendation="+to_string(totalNeighborMoviesRecCandidates.size()));

    //for each candidate movie: predict ratings
    for(size_t i=0;i<totalNeighborMoviesRecCandidates.size();i++){
        LOG.trace("Processing candidate movie="+to_string(totalNeighborMoviesRecCandidates[i].id));
        //extract the movie's neighborhood: keep only the movies that the user has watched.
        Movie movieToProcess = convertNeighborMovieToMovieObject(totalNeighborMoviesRecCandidates[i]);
        vector<NeighborMovie> neighborMovies = extractNearestNeighborMovies(movieToProcess);
        LOG.trace("Extracted nearest neighbor movies of candidate movie. size="+to_string(neighborMovies.size()));
        keepOnlyUserWatchedFromNeighborMovies(neighborMovies,watchedMovies);
        LOG.trace("Kept only user watched from neighbor movies. newSize="+to_string(neighborMovies.size()));
        //movieToProcess == candidate movie. neighborMovies == neighbors of candidate movie that user has watched.
        movieRecommendations.push_back(predictRatingItemBasedMethod(neighborMovies,watchedMovies,movieToProcess));
    }

    LOG.info("extractPredictedRatingsItemBased finished. totalRecommendations="+to_string(movieRecommendations.size()));
    LOG.close();

    return movieRecommendations;
}

vector<MovieRecommendation> RecommenderDataHandler::extractBestRatedMovies(){

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:extractBestRatedMovies",LOGLEVEL);

    LOG.info("START: extractBestRatedMovies");

    vector<Movie>& allMovies = getAllMovies();
    //make a copy of all movies.
    vector<Movie> allMoviesSubsetCopy(allMovies);
    //filter out the movies with less than "minimumRatingCountForAggregations" ratings.
    for(auto it=allMoviesSubsetCopy.begin();it != allMoviesSubsetCopy.end();it++){
        unsigned long currentRatingCount = 0;
        for(auto it_inner = it->userEntries.begin();it_inner != it->userEntries.end(); it_inner++){
            if(it_inner->rated){
                currentRatingCount++;
            }
        }
        if(currentRatingCount < config->minimumRatingCountForAggregations){
            allMoviesSubsetCopy.erase(it);
            it--;
        }
    }
    sort(allMoviesSubsetCopy.begin(),allMoviesSubsetCopy.end(),movieMeanRatingComparator);

    vector<Movie>::iterator first = allMoviesSubsetCopy.begin();
    vector<Movie>::iterator last;
    if(config->recommendationCount <= allMoviesSubsetCopy.size()){
        //last = neighborMovies.begin() + config->maxNeighbors + 1;
        last = allMoviesSubsetCopy.begin() + config->recommendationCount;
    } else{
        last = allMoviesSubsetCopy.end();
    }
    allMoviesSubsetCopy = vector<Movie>(first,last);

    vector<MovieRecommendation> movieRecommendations;
    for(auto it=allMoviesSubsetCopy.begin();it!=allMoviesSubsetCopy.end();it++){
        movieRecommendations.push_back(MovieRecommendation(*it,getMostCommonTagsFromMovie(*it,5)));
    }

    LOG.info("extractBestRatedMovies finished. totalRecommendations="+to_string(movieRecommendations.size()));
    return movieRecommendations;
}

vector<MovieRecommendation> RecommenderDataHandler::extractBestRatedMoviesFiltered(vector<MovieGenre> genres,vector<string> tagNames
                                                                                   ,unordered_map<string,char>& filteringTypes){

    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:extractBestRatedMoviesFiltered",LOGLEVEL);

    LOG.info("START: extractBestRatedMoviesFiltered");

    vector<Movie> allMovies = getAllMovies();
    //TODO: make a copy or sort the actual vector?
    vector<Movie> allMoviesSubsetCopy(allMovies);
    //filter the movies based on the given criteria.
    //filter out the movies with less than "minimumRatingCountForAggregations" ratings.
    for(auto it=allMoviesSubsetCopy.begin();it != allMoviesSubsetCopy.end();it++){
        unsigned long currentRatingCount = 0;
        for(auto it_inner = it->userEntries.begin();it_inner != it->userEntries.end(); it_inner++){
            if(it_inner->rated){
                currentRatingCount++;
            }
        }
        if(currentRatingCount < config->minimumRatingCountForAggregations){
            allMoviesSubsetCopy.erase(it);
            it--;
        }
    }
    //genre criteria (only if the vector is non-empty)
    if(!genres.empty()){
        LOG.debug("'genres' parameter is not empty. Will filter based on given genres. genreCount="+to_string(genres.size()));
        auto filteringTypeIterGenres = filteringTypes.find("genres");
        if(filteringTypeIterGenres == filteringTypes.end()){
            throw InvalidStateException("Could not find filtering type for genres.");
        }
        LOG.debug("candidate movie count before genre filtering="+to_string(allMoviesSubsetCopy.size()));
        filterVectorWithGenreCriteria(allMoviesSubsetCopy,genres,filteringTypeIterGenres->second);
        LOG.debug("candidate movie count after genre filtering="+to_string(allMoviesSubsetCopy.size()));
    }
    //tag criteria (only if the vector is non-empty)
    if(!tagNames.empty()){
        LOG.debug("'tagNames' parameter is not empty. Will filter based on given tag names. tagNameCount="+to_string(tagNames.size()));
        auto filteringTypeIterTags = filteringTypes.find("tags");
        if(filteringTypeIterTags == filteringTypes.end()){
            throw InvalidStateException("Could not find filtering type for tags.");
        }
        LOG.debug("candidate movie count before tag filtering="+to_string(allMoviesSubsetCopy.size()));
        filterVectorWithTagCriteria(config,allMoviesSubsetCopy,tagNames,filteringTypeIterTags->second);
        LOG.debug("candidate movie count after tag filtering="+to_string(allMoviesSubsetCopy.size()));
    }

    sort(allMoviesSubsetCopy.begin(),allMoviesSubsetCopy.end(),movieMeanRatingComparator);

    vector<Movie>::iterator first = allMoviesSubsetCopy.begin();
    vector<Movie>::iterator last;
    if(config->recommendationCount <= allMoviesSubsetCopy.size()){
        last = allMoviesSubsetCopy.begin() + config->recommendationCount;
    } else{
        last = allMoviesSubsetCopy.end();
    }
    allMoviesSubsetCopy = vector<Movie>(first,last);

    vector<MovieRecommendation> movieRecommendations;
    for(auto it=allMoviesSubsetCopy.begin();it!=allMoviesSubsetCopy.end();it++){
        movieRecommendations.push_back(MovieRecommendation(*it,getMostCommonTagsFromMovie(*it,5)));
    }

    LOG.info("extractBestRatedMoviesFiltered finished. totalRecommendations="+to_string(movieRecommendations.size()));
    return movieRecommendations;
}

// // // // // // // // // // // // // //

//positionFound is only populated if not given as nullptr.
//it will be returned as nullptr if target is not found.
bool RecommenderDataHandler::userIdExists(unsigned long targetUserId,size_t* positionFound){

    if(config->useDatabase){
        //TODO
        return false;
    } else{

        auto exist_iter = find_if(allUsers.begin(),allUsers.end(),
                                  [&targetUserId](User& usr){return usr.userId == targetUserId;});

        if(positionFound != nullptr){
            if(exist_iter != allUsers.end()){
                *positionFound = exist_iter - allUsers.begin();
            } else{
                positionFound = nullptr;
            }
        }
        return !(exist_iter == allUsers.end());
    }
}

//positionFound is only populated if not given as nullptr.
//it will be returned as nullptr if target is not found.
bool RecommenderDataHandler::movieIdExists(unsigned long targetMovieId,size_t* positionFound){

    if(config->useDatabase){
        //TODO
        return false;
    } else{

        auto exist_iter = find_if(allMovies.begin(),allMovies.end(),
                                  [&targetMovieId](Movie& mv){return mv.movieId == targetMovieId;});

        if(positionFound!=nullptr){
            if(exist_iter != allMovies.end()){
                *positionFound = exist_iter - allMovies.begin();
            } else{
                positionFound = nullptr;
            }
        }
        return !(exist_iter == allMovies.end());
    }
}

//positionFound is only populated if not given as nullptr.
//it will be returned as nullptr if target is not found.
bool RecommenderDataHandler::tagIdExists(unsigned long targetTagId,size_t* positionFound){

    if(config->useDatabase){
        //TODO
        return false;
    } else{
        vector<TagEntry> allTagEntries;
        for(auto it=allMovies.begin();it!=allMovies.end();it++){
            vector<TagEntry> tagEntries = it->totalTags;
            allTagEntries.insert(allTagEntries.end(),tagEntries.begin(),tagEntries.end());
        }
        //search in allTagEntries.
        auto exist_iter = find_if(allTagEntries.begin(),allTagEntries.end(),[&targetTagId](TagEntry& tgEntr){return tgEntr.tagId == targetTagId;});

        if(positionFound != nullptr){
            if(exist_iter != allTagEntries.end()){
                *positionFound = exist_iter - allTagEntries.begin();
            } else{
                positionFound = nullptr;
            }
        }
        return !(exist_iter == allTagEntries.end());
    }
}

//if tagId is not found, returns 0.
unsigned long RecommenderDataHandler::getTagIdForTagName(string inputTagName){
    for(auto it=allMovies.begin();it!=allMovies.end();it++){
        vector<TagEntry>& tagEntries = it->totalTags;
        auto tag_iter = find_if(tagEntries.begin(),tagEntries.end(),
                                [&inputTagName](TagEntry& tgEntry){return tgEntry.tagName == inputTagName;});

        if(tag_iter != tagEntries.end()){
            return tag_iter->tagId;
        }
    }
    return 0;
}

//

//generates a user id that does not already exist.
ulong RecommenderDataHandler::autogenerateUserId(){
    srand(time(0));
    if(allUsers.size() > 0){
        unsigned long id = allUsers[allUsers.size()-1].userId;
    while(userIdExists(id)){
        id = (rand() % 1001) + id;
        }
        return id;
    }
    return static_cast<ulong>(rand());
}

//autogenerate a random tag id.
ulong RecommenderDataHandler::autogenerateRandomTagId(int seed){
    srand(time(0)+seed);
    return static_cast<ulong>(rand());
}

//tries to find if the tagname already exists and returns its tagId, otherwise generates a tagId that does not already exist.
ulong RecommenderDataHandler::findOrGenerateTagId(string tagName){
    //first, try to find existing.
    ulong foundTargetId = this->getTagIdForTagName(tagName);
    //if not found, generate one that doesnt exist.
    if(foundTargetId == 0){
        ulong generatedId = 0;
        do{
            generatedId = autogenerateRandomTagId(12345);
        }while(tagIdExists(generatedId) && generatedId != 0);
        return generatedId;
    } else{
        return foundTargetId;
    }
}

//uses user data to extract result.
size_t RecommenderDataHandler::getRatingCount(){
    size_t count = 0;
    vector<User>& allUsers = getAllUsers();
    for(auto it=allUsers.begin();it!=allUsers.end();it++){
        vector<MovieEntry>& movieEntries = it->movieEntries;
        for(size_t i=0;i<movieEntries.size();i++){
            if(movieEntries[i].rated){
                count = count + 1;
            }
        }
    }
    return count;
}

//uses movie data to extract result.
size_t RecommenderDataHandler::getTotalTagCount(){
    size_t count = 0;
    vector<Movie>& allMovies = getAllMovies();
    for(auto it=allMovies.begin();it!=allMovies.end();it++){
        for(auto inner_it = it->totalTags.begin();inner_it!=it->totalTags.end();inner_it++){
            count = count + inner_it->tagCount;
        }
    }
    return count;
}

size_t RecommenderDataHandler::getUniqueTagCount(){
    size_t count = 0;
    vector<Movie>& allMovies = getAllMovies();
    set<ulong> allUniqueTags;
    for(auto it=allMovies.begin();it!=allMovies.end();it++){
        for(auto inner_it = it->totalTags.begin();inner_it!=it->totalTags.end();inner_it++){
            allUniqueTags.insert(inner_it->tagId);
        }
    }
    count = allUniqueTags.size();
    return count;
}

// // // // // // // // // // // // // // // // // // // // //

ElapsedTimes RecommenderDataHandler::calculateAllParameters(){
    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:calculateAllParameters");
    LOG.info("START: calculateAllParameters");
    vector<User>* currentAllUsers;
    vector<Movie>* currentAllMovies;
    if(config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE);
    } else{
        currentAllUsers = &getAllUsers();
        currentAllMovies = &getAllMovies();
    }
    CustomSteadyTimer calcAllParamsTimer;
    calcAllParamsTimer.markStartPoint();
    LOG.debug("Calculating parameters for users.");
    calculateParametersForAllUsers(*currentAllUsers,config);
    LOG.debug("Calculating parameters for movies.");
    calculateParametersForAllMovies(*currentAllMovies,*currentAllUsers,config);
    LOG.info("calculateAllParameters finished.");
    double calcAllParamsElapsed = calcAllParamsTimer.markEndPointAndGetTimeElapsed();
    LOG.close();
    elapsedTimes.setDataCalcElapsed(calcAllParamsElapsed);
    return elapsedTimes;
}

// // // // // // // // // // // // // // // // // // // //

//

//fully delete data
bool RecommenderDataHandler::deleteALLUsers(){
    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:deleteALLUsers");
    LOG.info("START: deleteALLUsers");
    //using swap 'trick' so that the, so-far, reserved memory is freed.
    vector<User>& allUsers = getAllUsers();
    vector<User>().swap(allUsers);
    LOG.close();
    if(allUsers.size() == 0){
        LOG.info("deleteALLUsers finished. Deletions were successful.");
        return true;
    } else{
        LOG.warn("deleteALLUsers finished. Deletions were NOT successful!");
        return false;
    }
}

bool RecommenderDataHandler::deleteALLMovies(){
    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:deleteALLMovies");
    LOG.info("START: deleteALLMovies");
    //using swap 'trick' so that the, so-far, reserved memory is freed.
    vector<Movie>& allMovies = getAllMovies();
    vector<Movie>().swap(allMovies);
    LOG.close();
    if(allMovies.size() == 0){
        LOG.info("deleteALLMovies finished. Deletions were successful.");
        return true;
    } else{
        LOG.warn("deleteALLMovies finished. Deletions were NOT successful!");
        return false;
    }
}

// //
//rollback:will delete after index(position) exclusive, until the end.
bool RecommenderDataHandler::rollbackUsers(size_t position){
    if(config->useDatabase){
        //TODO
        return false;
    }else{
        vector<User>& allUsers = this->getAllUsers();
        size_t expectedToDeleteCount = 0;
        for(auto target_iter = allUsers.begin() + position;target_iter!=allUsers.end();target_iter++){
            expectedToDeleteCount++;
        }
        size_t sizeBefore = allUsers.size();
        allUsers.erase(allUsers.begin()+position+1,allUsers.end());
        size_t sizeAfter = allUsers.size();
        size_t actualDeleted = sizeAfter - sizeBefore;
        if(actualDeleted == expectedToDeleteCount){
            return true;
        } else{
            return false;
        }
    }
}

//rollback:will delete after index(position) exclusive, until the end.
bool RecommenderDataHandler::rollbackMovies(size_t position){
    if(config->useDatabase){
        //TODO
        return false;
    } else{
        vector<Movie>& allMovies = this->getAllMovies();
        size_t expectedToDeleteCount = 0;
        for(auto target_iter = allMovies.begin() + position;target_iter!=allMovies.end();target_iter++){
            expectedToDeleteCount++;
        }
        size_t sizeBefore = allMovies.size();
        allMovies.erase(allMovies.begin()+position+1,allMovies.end());
        size_t sizeAfter = allMovies.size();
        size_t actualDeleted = sizeAfter - sizeBefore;
        if(actualDeleted == expectedToDeleteCount){
            return true;
        } else{
            return false;
        }
    }
}

//the below return true if deletion was successful.
bool RecommenderDataHandler::deleteSingleUser(User& userToDelete){
    return deleteSingleUser(userToDelete.userId);
}

bool RecommenderDataHandler::deleteSingleUser(unsigned long userIdToDelete){
    if(config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE);
    }else{
        size_t sizeBefore = allUsers.size();
        allUsers.erase(remove_if(allUsers.begin(),allUsers.end(),
                                 [&userIdToDelete](User& usr){return userIdToDelete == usr.userId;}),allUsers.end());
        size_t sizeAfter = allUsers.size();
        return ((sizeBefore - sizeAfter) == 1);
    }
}

bool RecommenderDataHandler::deleteSingleMovie(Movie& movieToDelete){
    return deleteSingleMovie(movieToDelete.movieId);
}

bool RecommenderDataHandler::deleteSingleMovie(unsigned long movieIdToDelete){
    if(config->useDatabase){
        throw NotYetImplementedException(FunctionalityNames::DB_PERSISTENCE);
    } else{
        size_t sizeBefore = allMovies.size();
        allMovies.erase(remove_if(allMovies.begin(),allMovies.end(),
                                  [&movieIdToDelete](Movie& mv){return movieIdToDelete == mv.movieId;}),allMovies.end());
        size_t sizeAfter = allMovies.size();
        return ((sizeBefore - sizeAfter) == 1);
    }
}
