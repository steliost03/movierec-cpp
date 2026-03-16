#ifndef EXPORT_JSON
#define EXPORT_JSON

#include "../data/dtos.h"
#include "../data/recommenderDataHandler.h"

//generic method. The type of export is dependent on the RecommenderDataHandler configuration.
void exportToJson(RecommenderDataHandler& rdh,std::string& outputFileNameUsers,std::string& outputFileNameMovies,bool pretty=false);

void exportUserProfilesToJson(RecommenderDataHandler& rdh,std::string& outputFileNameProfiles,bool pretty=true);

void exportToJsonFromMemory(vector<User>& allUsers, vector<Movie>& allMovies,
                            std::string& outputFileNameUsers,std::string& outputFileNameMovies,bool pretty=false);

//does not load all of the data in memory at once, but only in small batches.
void exportToJsonFromDB();

#endif
