#ifndef REC_DATA_RESULT
#define REC_DATA_RESULT

#include "../data/recommenderDataHandler.h"

struct MovieRecommendationsWrapper {

	std::vector<MovieRecommendation> recommendedFromUserMethod;
	std::vector<MovieRecommendation> recommendedFromItemMethod;
        std::vector<MovieRecommendation> recommendedFromGeneralMethod;

};

MovieRecommendationsWrapper recommendMoviesPersonalized(unsigned long targetUserId, RecommenderDataHandler& recommenderDataHandler, Configuration& config);
MovieRecommendationsWrapper recommendMoviesGeneralBestRated(RecommenderDataHandler& recommenderDataHandler,
                                                            vector<MovieGenre> genres,vector<string> tagNames,unordered_map<string,char>& filteringTypes);

MovieRecommendationsWrapper recommendMoviesPersonalizedStandaloneProfile(User& userData,string profileName,RecommenderDataHandler& recommenderDataHandler,Configuration& config);

#endif
