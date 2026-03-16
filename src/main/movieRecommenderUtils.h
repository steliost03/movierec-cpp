#ifndef RECOMMEND_MOVIE_UTILS_H
#define RECOMMEND_MOVIE_UTILS_H

#include "../data/dtos.h"

#include <vector>

class ElapsedTimes {

    private:

    double parseElapsed;
    double parsePostProcessElapsed;
    double dataCalcElapsed;
    double recommendationRetrievalElapsed;
    double userMovieParamsCalcElapsed;

    public:

    double getUserMovieParamsCalcElapsed(){
        return userMovieParamsCalcElapsed;
    }

    void setUserMovieParamsCalcElapsed(double userMovieParamsCalcElapsed){
        this->userMovieParamsCalcElapsed = userMovieParamsCalcElapsed;
    }

    double getParseElapsed(){
        return parseElapsed;
    }

    double getParsePostProcessElapsed(){
        return parsePostProcessElapsed;
    }

    double getDataCalcElapsed(){
        return dataCalcElapsed;
    }

    double getRecommendationRetrievalElapsed(){
        return recommendationRetrievalElapsed;
    }

    void setParseElapsed(double parseElapsed){
        this->parseElapsed = parseElapsed;
    }

    void setParsePostProcessElapsed(double parsePostProcessElapsed){
        this->parsePostProcessElapsed = parsePostProcessElapsed;
    }

    void setDataCalcElapsed(double dataCalcElapsed){
        this->dataCalcElapsed = dataCalcElapsed;
    }

    void setRecommendationRetrievalElapsed(double recommendationRetrievalElapsed){
        this->recommendationRetrievalElapsed = recommendationRetrievalElapsed;
    }
};

class MovieRecommendation {

    public:

    unsigned int recommendationID;
    char recommendationType; //'0':user-based , '1':item-based , '2':general(best-rated)

    //will not be shown to user:
    bool exists;
	
    double predictedRating;
    double predictedRatingNormalized;
    double currentWeightSum;

    bool accessedOnce = false;
	
    //will be shown to user (depending on method used):
    unsigned long movieId;
    std::string movieName;
    std::vector<std::string> genres;
    std::vector<TagEntry> mostCommonTags;
    double meanRating;
    double meanRatingFromNeighbors = -1; //user-based only
    std::vector<std::string> similarWatchedMovies; //item-based only
	
    unsigned long neighborsUsedCount;
	
    MovieRecommendation(){
        this->accessedOnce = false;
        this->exists = true;
    }

    //constructor from Movie object.
    MovieRecommendation(Movie& mv,vector<TagEntry> mostCommonTags){
         this->exists = true;
         this->recommendationType = '2';
         this->movieId = mv.movieId;
         this->movieName = mv.movieName;
         this->genres = mv.genres;
         this->mostCommonTags = mostCommonTags;
         this->meanRating = mv.meanRating;
    }
	//
    void setMovieRecommendationFromItemBasedMethod(double predictedRatingNormalized,double predictedRating, unsigned long movieId, std::string movieName, std::vector<std::string> genres,
                                                   std::vector<TagEntry> mostCommonTags, double meanRating, std::vector<std::string> similarWatchedMovies
                                                   ,unsigned long neighborsUsedCount){

        this->predictedRatingNormalized = predictedRatingNormalized;
        this->predictedRating = predictedRating;
        this->movieId = movieId;
        this->movieName = movieName;
        this->genres = genres;
        this->mostCommonTags = mostCommonTags;
        this->meanRating = meanRating;
        this->similarWatchedMovies = similarWatchedMovies;
        this->recommendationType = '1';
        this->neighborsUsedCount = neighborsUsedCount;
		
	}
	
	void updateMovieRecommendationFromItemBasedMethod(){
        //TODO:implement.
	}
	
	void finalSetMovieRecommendationFromItemBasedMethod(){
        //TODO:implement.
	}
	
    void setMovieRecommendationFromUserBasedMethod(double predictedRatingNormalized, unsigned long movieId, std::string movieName, std::vector<std::string> genres, double meanRating,double meanRatingFromNeighbors,double currentWeightSum){

        this->predictedRatingNormalized = predictedRatingNormalized;
        this->movieId = movieId;
        this->movieName = movieName;
        this->genres = genres;
        this->meanRating = meanRating;
        this->neighborsUsedCount = 1;
        this->meanRatingFromNeighbors = meanRatingFromNeighbors;
        this->recommendationType = '0';
        this->currentWeightSum = currentWeightSum;
    }
	
    void updateMovieRecommendationFromUserBasedMethod(double newPredictedRatingNormalized,double newMeanRatingFromNeighbors,double newWeightSum){
		
        this->predictedRatingNormalized = newPredictedRatingNormalized;
        this->meanRatingFromNeighbors = newMeanRatingFromNeighbors;
        this->currentWeightSum = newWeightSum;
        this->neighborsUsedCount++;
	}
	
	//
	
    void calculateNewMeanRatingFromNeighbors(double newRating){
		
        double numerator = neighborsUsedCount*meanRatingFromNeighbors + static_cast<double>(newRating);
        double denominator = neighborsUsedCount+1;
        double result = numerator/denominator;
        meanRatingFromNeighbors = result;
	}
};

#endif
