#include "exportToJson.h"

#include "../../include/rapidjson/stringbuffer.h"
#include "../../include/rapidjson/prettywriter.h"
#include "../logging/recommenderLogger.h"

#include <string>
#include <fstream>

using namespace rapidjson;

//TODO: error handling.

void writeStringWithCondition(bool condition,std::string key,std::string value,Writer<StringBuffer>& writer){
	if(condition){
		writer.Key(key.c_str());
		writer.String(value.c_str());
	}
}

void writeStringWithConditionPretty(bool condition,std::string key,std::string value,PrettyWriter<StringBuffer>& writer){
    if(condition){
        writer.Key(key.c_str());
        writer.String(value.c_str());
    }
}

// //

void writeDoubleWithCondition(bool condition,std::string key,double value,Writer<StringBuffer>& writer){
	if(condition){
		writer.Key(key.c_str());
		writer.Double(value);
	}
}

void writeDoubleWithConditionPretty(bool condition,std::string key,double value,PrettyWriter<StringBuffer>& writer){
    if(condition){
        writer.Key(key.c_str());
        writer.Double(value);
    }
}

// //

void writeUintWithCondition(bool condition,std::string key,unsigned int value,Writer<StringBuffer>& writer){
	if(condition){
		writer.Key(key.c_str());
		writer.Uint(value);
	}
}

void writeUintWithConditionPretty(bool condition,std::string key,unsigned int value,PrettyWriter<StringBuffer>& writer){
    if(condition){
        writer.Key(key.c_str());
        writer.Uint(value);
    }
}

// //

void writeBoolWithCondition(std::string key,bool value,Writer<StringBuffer>& writer){
    writer.Key(key.c_str());
    writer.Bool(value);
}

void writeBoolWithConditionPretty(std::string key,bool value,PrettyWriter<StringBuffer>& writer){
    writer.Key(key.c_str());
    writer.Bool(value);
}

// //

//clears StringBuffer before using it.
void writeUsersToJsonFile(vector<User>& users,Writer<StringBuffer>& writer,StringBuffer& s,std::string& outputFileName){

    RecommenderLogger LOG;
    LOG.init("ExportToJson: writeUsersToJsonFilePretty");
    LOG.info("START: writeUsersToJsonFilePretty.");

    s.Clear();
    writer.Reset(s);

    writer.StartArray();
	for (auto it = users.begin(); it != users.end(); it++){
		User currentUser = *it;
		writer.StartObject();
        writeUintWithCondition(currentUser.userId > 0,"userId",currentUser.userId,writer);
        //accepting zero in meanRating because it may happen in some edge cases.
        writeDoubleWithCondition(currentUser.meanRating >= 0, "meanRating", currentUser.meanRating, writer);
        //accepting zero in ratingsStandardDeviation because it may happen in some edge cases.
        writeDoubleWithCondition(currentUser.ratingsStandardDeviation >= 0,"ratingsStandardDeviation",currentUser.ratingsStandardDeviation,writer);
        //later addition (1nov2019)
        if(currentUser.movieEntries.empty()){
            writer.Key("movieEntries");
            writer.StartArray();
            writer.EndArray();
        }
        if (!currentUser.movieEntries.empty()){
			writer.Key("movieEntries");
			writer.StartArray();
			for (auto it2 = currentUser.movieEntries.begin(); it2 != currentUser.movieEntries.end(); it2++){
				MovieEntry currentMovieEntry = *it2;
				writer.StartObject();
                writeUintWithCondition(currentMovieEntry.movieId > 0,"movieId",currentMovieEntry.movieId,writer);
				writeDoubleWithCondition(currentMovieEntry.rating > 0 , "rating",currentMovieEntry.rating,writer);
				writer.Key("tags");
				writer.StartArray();
				for (auto it3 = currentMovieEntry.tags.begin(); it3 != currentMovieEntry.tags.end(); it3++){
					TagEntry currentTagEntry = *it3;
                    writer.StartObject();
					writeStringWithCondition(!currentTagEntry.tagName.empty(),"tagName",currentTagEntry.tagName,writer);
                    writeUintWithCondition(currentTagEntry.tagId > 0,"tagId",currentTagEntry.tagId,writer);
					writeUintWithCondition(currentTagEntry.tagCount > 0,"tagCount",currentTagEntry.tagCount,writer);
                    writer.EndObject();
				}
				writer.EndArray();
				writer.EndObject();
			}
			writer.EndArray();
		}
        //later addition (1nov2019)
        if(currentUser.neighbors.empty()){
            writer.Key("neighbors");
            writer.StartArray();
            writer.EndArray();
        }
        //
		if(!currentUser.neighbors.empty()){
			writer.Key("neighbors");
			writer.StartArray();
			for(auto it2 = currentUser.neighbors.begin(); it2 != currentUser.neighbors.end(); it2++){
                NeighborUser currentNeighborUser = *it2;
				writer.StartObject();
                writeUintWithCondition(currentNeighborUser.id > 0,"id",currentNeighborUser.id,writer);
                writeDoubleWithCondition(currentNeighborUser.weight > 0,"weight",currentNeighborUser.weight,writer);
                if(!currentNeighborUser.commonMoviesIds.empty()){
                    writer.Key("commonMoviesIds");
                    writer.StartArray();
                    for(auto it3 = currentNeighborUser.commonMoviesIds.begin(); it3 != currentNeighborUser.commonMoviesIds.end(); it3++){
                        writer.Uint(*it3);
                    }
                    writer.EndArray();
                }
				writer.EndObject();
			}
			writer.EndArray();
		}
		writer.EndObject();
	}
	writer.EndArray();

	ofstream outfile;
	outfile.open(outputFileName.c_str());
	outfile << s.GetString();
	outfile.close();

    LOG.info("writeUsersToJsonFilePretty finished.");
    LOG.close();
}

void writeUsersToJsonFilePretty(vector<User>& users,PrettyWriter<StringBuffer>& prettyWriter,StringBuffer& s,std::string& outputFileName){

    RecommenderLogger LOG;
    LOG.init("ExportToJson: writeUsersToJsonFilePretty");
    LOG.info("START: writeUsersToJsonFilePretty.");

    s.Clear();
    prettyWriter.Reset(s);

    prettyWriter.StartArray();
    for (auto it = users.begin(); it != users.end(); it++){
        User currentUser = *it;
        prettyWriter.StartObject();
        writeUintWithCondition(currentUser.userId > 0,"userId",currentUser.userId,prettyWriter);
        //accepting zero in meanRating because it may happen in some edge cases.
        writeDoubleWithCondition(currentUser.meanRating >= 0, "meanRating", currentUser.meanRating, prettyWriter);
        //accepting zero in ratingsStandardDeviation because it may happen in some edge cases.
        writeDoubleWithCondition(currentUser.ratingsStandardDeviation >= 0,"ratingsStandardDeviation",currentUser.ratingsStandardDeviation,prettyWriter);
        //later addition (1nov2019)
        if(currentUser.movieEntries.empty()){
            prettyWriter.Key("movieEntries");
            prettyWriter.StartArray();
            prettyWriter.EndArray();
        }
        if (!currentUser.movieEntries.empty()){
            prettyWriter.Key("movieEntries");
            prettyWriter.StartArray();
            for (auto it2 = currentUser.movieEntries.begin(); it2 != currentUser.movieEntries.end(); it2++){
                MovieEntry currentMovieEntry = *it2;
                prettyWriter.StartObject();
                writeUintWithCondition(currentMovieEntry.movieId > 0,"movieId",currentMovieEntry.movieId,prettyWriter);
                writeDoubleWithCondition(currentMovieEntry.rating > 0 , "rating",currentMovieEntry.rating,prettyWriter);
                prettyWriter.Key("tags");
                prettyWriter.StartArray();
                for (auto it3 = currentMovieEntry.tags.begin(); it3 != currentMovieEntry.tags.end(); it3++){
                    TagEntry currentTagEntry = *it3;
                    prettyWriter.StartObject();
                    writeStringWithCondition(!currentTagEntry.tagName.empty(),"tagName",currentTagEntry.tagName,prettyWriter);
                    writeUintWithCondition(currentTagEntry.tagId > 0,"tagId",currentTagEntry.tagId,prettyWriter);
                    writeUintWithCondition(currentTagEntry.tagCount > 0,"tagCount",currentTagEntry.tagCount,prettyWriter);
                    prettyWriter.EndObject();
                }
                prettyWriter.EndArray();
                prettyWriter.EndObject();
            }
            prettyWriter.EndArray();
        }
        //later addition (1nov2019)
        if(currentUser.neighbors.empty()){
            prettyWriter.Key("neighbors");
            prettyWriter.StartArray();
            prettyWriter.EndArray();
        }
        if(!currentUser.neighbors.empty()){
            prettyWriter.Key("neighbors");
            prettyWriter.StartArray();
            for(auto it2 = currentUser.neighbors.begin(); it2 != currentUser.neighbors.end(); it2++){
                NeighborUser currentNeighborUser = *it2;
                prettyWriter.StartObject();
                writeUintWithCondition(currentNeighborUser.id > 0,"id",currentNeighborUser.id,prettyWriter);
                writeDoubleWithCondition(currentNeighborUser.weight > 0,"weight",currentNeighborUser.weight,prettyWriter);
                if(!currentNeighborUser.commonMoviesIds.empty()){
                    prettyWriter.Key("commonMoviesIds");
                    prettyWriter.StartArray();
                    for(auto it3 = currentNeighborUser.commonMoviesIds.begin(); it3 != currentNeighborUser.commonMoviesIds.end(); it3++){
                        prettyWriter.Uint(*it3);
                    }
                    prettyWriter.EndArray();
                }
                prettyWriter.EndObject();
            }
            prettyWriter.EndArray();
        }
        prettyWriter.EndObject();
    }
    prettyWriter.EndArray();

    ofstream outfile;
    outfile.open(outputFileName.c_str());
    outfile << s.GetString();
    outfile.close();

    LOG.info("writeUsersToJsonFilePretty finished.");
    LOG.close();
}

//clears StringBuffer before using it.
void writeMoviesToJsonFile(vector<Movie>& movies, Writer<StringBuffer>& writer,StringBuffer& s,std::string& outputFileName){

    RecommenderLogger LOG;
    LOG.init("ExportToJson: writeMoviesToJsonFilePretty");
    LOG.info("START: writeMoviesToJsonFilePretty");

    s.Clear();
    writer.Reset(s);

	writer.StartArray();
	for (auto it = movies.begin(); it != movies.end(); it++){
		Movie currentMovie = *it;
		writer.StartObject();
        writeUintWithCondition(currentMovie.movieId > 0,"movieId",currentMovie.movieId,writer);
		writeStringWithCondition(!currentMovie.movieName.empty(),"movieName",currentMovie.movieName,writer);
        //accepting zero in meanRating because it may happen in some edge cases.
        writeDoubleWithCondition(currentMovie.meanRating >= 0,"meanRating",currentMovie.meanRating,writer);
        //accepting zero in ratingsStandardDeviation because it may happen in some edge cases.
        writeDoubleWithCondition(currentMovie.ratingsStandardDeviation >= 0, "ratingsStandardDeviation",currentMovie.ratingsStandardDeviation,writer);
        //writeDoubleWithCondition(currentMovie.inverseUserFrequency > 0,"inverseUserFrequency",currentMovie.inverseUserFrequency,writer);
		if(!currentMovie.genres.empty()){
			writer.Key("genres");
			writer.StartArray();
			for (auto it2 = currentMovie.genres.begin(); it2 != currentMovie.genres.end(); it2++){
				writer.String(it2->c_str());
			}
			writer.EndArray();
		}
        //later addition (1nov2019)
        if(currentMovie.totalTags.empty()){
            writer.Key("totalTags");
            writer.StartArray();
            writer.EndArray();
        }
		if(!currentMovie.totalTags.empty()){
			writer.Key("totalTags");
			writer.StartArray();
			for (auto it2 = currentMovie.totalTags.begin(); it2 != currentMovie.totalTags.end(); it2++){
				TagEntry currentTagEntry = *it2;
				writer.StartObject();
				writeStringWithCondition(!currentTagEntry.tagName.empty(),"tagName",currentTagEntry.tagName,writer);
                writeUintWithCondition(currentTagEntry.tagId > 0,"tagId",currentTagEntry.tagId,writer);
				writeUintWithCondition(currentTagEntry.tagCount > 0,"tagCount",currentTagEntry.tagCount,writer);
				writer.EndObject();
			}
			writer.EndArray();
		}
        if(currentMovie.userEntries.empty()){
            writer.Key("userEntries");
            writer.StartArray();
            writer.EndArray();
        }
		if(!currentMovie.userEntries.empty()){
			writer.Key("userEntries");
			writer.StartArray();
			for(auto it2 = currentMovie.userEntries.begin(); it2 != currentMovie.userEntries.end(); it2++){
				UserEntry currentUserEntry = *it2;
				writer.StartObject();
                writeUintWithCondition(currentUserEntry.userId > 0,"userId",currentUserEntry.userId,writer);
				writeDoubleWithCondition(currentUserEntry.rating > 0,"rating",currentUserEntry.rating,writer);
				if(!currentUserEntry.tags.empty()){
					writer.Key("tags");
					writer.StartArray();
					for(auto it3 = currentUserEntry.tags.begin(); it3 != currentUserEntry.tags.end(); it3++){
						TagEntry currentTagEntry = *it3;
						writer.StartObject();
						writeStringWithCondition(!currentTagEntry.tagName.empty(),"tagName",currentTagEntry.tagName,writer);
                        writeUintWithCondition(currentTagEntry.tagId > 0,"tagId",currentTagEntry.tagId,writer);
						writeUintWithCondition(currentTagEntry.tagCount > 0,"tagCount",currentTagEntry.tagCount,writer);
						writer.EndObject();
					}
					writer.EndArray();
				}
				writer.EndObject();
			}
			writer.EndArray();
		}
        //later addition (1nov2019)
        if(currentMovie.neighbors.empty()){
            writer.Key("neighbors");
            writer.StartArray();
            writer.EndArray();
        }
		if(!currentMovie.neighbors.empty()){
			writer.Key("neighbors");
			writer.StartArray();
			for(auto it2 = currentMovie.neighbors.begin(); it2 != currentMovie.neighbors.end(); it2++){
				NeighborMovie currentNeighborMovie = *it2;
				writer.StartObject();
                writeUintWithCondition(currentNeighborMovie.id > 0,"id",currentNeighborMovie.id,writer);
				writeDoubleWithCondition(currentNeighborMovie.weight > 0,"weight",currentNeighborMovie.weight,writer);
				if(!currentNeighborMovie.commonUsersIds.empty()){
                    writer.Key("commonUsersIds");
					writer.StartArray();
					for(auto it3 = currentNeighborMovie.commonUsersIds.begin(); it3 != currentNeighborMovie.commonUsersIds.end(); it3++){
                        writer.Uint(*it3);
					}
					writer.EndArray();
				}
				writer.EndObject();
			}
			writer.EndArray();
		}
		writer.EndObject();
	}
	writer.EndArray();

	ofstream outfile;
    outfile.open(outputFileName.c_str());
	outfile << s.GetString();
	outfile.close();

    LOG.info("writeMoviesToJsonFilePretty finished.");
    LOG.close();
}

void writeMoviesToJsonFilePretty(vector<Movie>& movies, PrettyWriter<StringBuffer>& prettyWriter,StringBuffer& s,std::string& outputFileName){

    RecommenderLogger LOG;
    LOG.init("ExportToJson: writeMoviesToJsonFilePretty");
    LOG.info("START: writeMoviesToJsonFilePretty");

    s.Clear();
    prettyWriter.Reset(s);

    prettyWriter.StartArray();
    for (auto it = movies.begin(); it != movies.end(); it++){
        Movie currentMovie = *it;
        prettyWriter.StartObject();
        writeUintWithCondition(currentMovie.movieId > 0,"movieId",currentMovie.movieId,prettyWriter);
        writeStringWithCondition(!currentMovie.movieName.empty(),"movieName",currentMovie.movieName,prettyWriter);
        //accepting zero in meanRating because it may happen in some edge cases.
        writeDoubleWithCondition(currentMovie.meanRating >= 0,"meanRating",currentMovie.meanRating,prettyWriter);
        //accepting zero in ratingsStandardDeviation because it may happen in some edge cases.
        writeDoubleWithCondition(currentMovie.ratingsStandardDeviation >= 0, "ratingsStandardDeviation",currentMovie.ratingsStandardDeviation,prettyWriter);
        //writeDoubleWithCondition(currentMovie.inverseUserFrequency > 0,"inverseUserFrequency",currentMovie.inverseUserFrequency,writer);
        if(!currentMovie.genres.empty()){
            prettyWriter.Key("genres");
            prettyWriter.StartArray();
            for (auto it2 = currentMovie.genres.begin(); it2 != currentMovie.genres.end(); it2++){
                prettyWriter.String(it2->c_str());
            }
            prettyWriter.EndArray();
        }
        //later addition (1nov2019)
        if(currentMovie.totalTags.empty()){
            prettyWriter.Key("totalTags");
            prettyWriter.StartArray();
            prettyWriter.EndArray();
        }
        if(!currentMovie.totalTags.empty()){
            prettyWriter.Key("totalTags");
            prettyWriter.StartArray();
            for (auto it2 = currentMovie.totalTags.begin(); it2 != currentMovie.totalTags.end(); it2++){
                TagEntry currentTagEntry = *it2;
                prettyWriter.StartObject();
                writeStringWithCondition(!currentTagEntry.tagName.empty(),"tagName",currentTagEntry.tagName,prettyWriter);
                writeUintWithCondition(currentTagEntry.tagId > 0,"tagId",currentTagEntry.tagId,prettyWriter);
                writeUintWithCondition(currentTagEntry.tagCount > 0,"tagCount",currentTagEntry.tagCount,prettyWriter);
                prettyWriter.EndObject();
            }
            prettyWriter.EndArray();
        }
        if(currentMovie.userEntries.empty()){
            prettyWriter.Key("userEntries");
            prettyWriter.StartArray();
            prettyWriter.EndArray();
        }
        if(!currentMovie.userEntries.empty()){
            prettyWriter.Key("userEntries");
            prettyWriter.StartArray();
            for(auto it2 = currentMovie.userEntries.begin(); it2 != currentMovie.userEntries.end(); it2++){
                UserEntry currentUserEntry = *it2;
                prettyWriter.StartObject();
                writeUintWithCondition(currentUserEntry.userId > 0,"userId",currentUserEntry.userId,prettyWriter);
                writeDoubleWithCondition(currentUserEntry.rating > 0,"rating",currentUserEntry.rating,prettyWriter);
                if(!currentUserEntry.tags.empty()){
                    prettyWriter.Key("tags");
                    prettyWriter.StartArray();
                    for(auto it3 = currentUserEntry.tags.begin(); it3 != currentUserEntry.tags.end(); it3++){
                        TagEntry currentTagEntry = *it3;
                        prettyWriter.StartObject();
                        writeStringWithCondition(!currentTagEntry.tagName.empty(),"tagName",currentTagEntry.tagName,prettyWriter);
                        writeUintWithCondition(currentTagEntry.tagId > 0,"tagId",currentTagEntry.tagId,prettyWriter);
                        writeUintWithCondition(currentTagEntry.tagCount > 0,"tagCount",currentTagEntry.tagCount,prettyWriter);
                        prettyWriter.EndObject();
                    }
                    prettyWriter.EndArray();
                }
                prettyWriter.EndObject();
            }
            prettyWriter.EndArray();
        }
        if(currentMovie.neighbors.empty()){
            prettyWriter.Key("neighbors");
            prettyWriter.StartArray();
            prettyWriter.EndArray();
        }
        if(!currentMovie.neighbors.empty()){
            prettyWriter.Key("neighbors");
            prettyWriter.StartArray();
            for(auto it2 = currentMovie.neighbors.begin(); it2 != currentMovie.neighbors.end(); it2++){
                NeighborMovie currentNeighborMovie = *it2;
                prettyWriter.StartObject();
                writeUintWithCondition(currentNeighborMovie.id > 0,"id",currentNeighborMovie.id,prettyWriter);
                writeDoubleWithCondition(currentNeighborMovie.weight > 0,"weight",currentNeighborMovie.weight,prettyWriter);
                if(!currentNeighborMovie.commonUsersIds.empty()){
                    prettyWriter.Key("commonUsersIds");
                    prettyWriter.StartArray();
                    for(auto it3 = currentNeighborMovie.commonUsersIds.begin(); it3 != currentNeighborMovie.commonUsersIds.end(); it3++){
                        prettyWriter.Uint(*it3);
                    }
                    prettyWriter.EndArray();
                }
                prettyWriter.EndObject();
            }
            prettyWriter.EndArray();
        }
        prettyWriter.EndObject();
    }
    prettyWriter.EndArray();

    ofstream outfile;
    outfile.open(outputFileName.c_str());
    outfile << s.GetString();
    outfile.close();

    LOG.info("writeMoviesToJsonFilePretty finished.");
    LOG.close();
}


void exportToJsonFromMemory(vector<User>& allUsers, vector<Movie>& allMovies,
                            std::string& outputFileNameUsers,std::string& outputFileNameMovies,bool pretty){

    RecommenderLogger LOG;
    LOG.init("ExportToJson:exportToJsonFromMemory");
    LOG.info("START: exportToJsonFromMemory");

    if(pretty){
        LOG.trace("'pretty' output is enabled.");
    } else{
        LOG.trace("'pretty' output is disabled.");
    }

	StringBuffer s;
	PrettyWriter<StringBuffer> prettyWriter(s);
    Writer<StringBuffer> regularWriter(s);

    if(pretty){
        writeUsersToJsonFilePretty(allUsers, prettyWriter,s,outputFileNameUsers);
        writeMoviesToJsonFilePretty(allMovies, prettyWriter,s,outputFileNameMovies);
    } else{
        writeUsersToJsonFile(allUsers,regularWriter,s,outputFileNameUsers);
        writeMoviesToJsonFile(allMovies,regularWriter,s,outputFileNameMovies);
    }

    LOG.info("exportToJsonFromMemory finished.");
    LOG.close();
}

//generic method. The type of export is dependent on the RecommenderDataHandler configuration
void exportToJson(RecommenderDataHandler& rdh,std::string& outputFileNameUsers,std::string& outputFileNameMovies,bool pretty){

    vector<User>& allUsers = rdh.getAllUsers();
    vector<Movie>& allMovies = rdh.getAllMovies();

    exportToJsonFromMemory(allUsers,allMovies,outputFileNameUsers,outputFileNameMovies,pretty);
}

void exportUserProfilesToJson(RecommenderDataHandler&rdh,std::string& outputFileNameProfiles,bool pretty){

    RecommenderLogger LOG;
    LOG.init("ExportToJson:exportUserProfilesToJson");
    LOG.info("START: exportUserProfilesToJson");

    vector<UserProfileFull> allUserProfiles = rdh.getAllUserProfiles();

    //if no user profiles exist, just write an empty array and return.
    if(allUserProfiles.size() == 0){
        LOG.debug("No user profiles found. Will just write an empty array.");
        ofstream outfile;
        outfile.open(outputFileNameProfiles.c_str());
        string emptyArray = "[]";
        outfile << emptyArray;
        outfile.flush();
        outfile.close();
        LOG.info("exportUserProfilesToJson finished. filename="+outputFileNameProfiles);
        LOG.close();
        return;
    }

    StringBuffer s;
    PrettyWriter<StringBuffer> prettyWriter(s);
    Writer<StringBuffer> regularWriter(s);

    if(pretty){
        prettyWriter.StartArray();
        for(auto it=allUserProfiles.begin();it!=allUserProfiles.end();it++){
            UserProfileStored profileToStore = it->getStoredProfile();
            prettyWriter.StartObject();
            writeStringWithConditionPretty(!profileToStore.profileName.empty(),"profileName",profileToStore.profileName,prettyWriter);
            writeBoolWithConditionPretty("isDataBound",profileToStore.isDataBound,prettyWriter);
            writeUintWithConditionPretty(profileToStore.userId > 0,"userId",profileToStore.userId,prettyWriter);
            if(!profileToStore.isDataBound){
                prettyWriter.Key("userData");
                prettyWriter.StartObject();
                writeUintWithConditionPretty(profileToStore.userId > 0,"userId",profileToStore.userData.userId,prettyWriter);
                writeDoubleWithConditionPretty(profileToStore.userData.meanRating >0,"meanRating",profileToStore.userData.meanRating,prettyWriter);
                writeDoubleWithConditionPretty(profileToStore.userData.ratingsStandardDeviation>0,"ratingsStandardDeviation",profileToStore.userData.ratingsStandardDeviation,prettyWriter);
                //movieEntries
                if(!profileToStore.userData.movieEntries.empty()){
                    prettyWriter.Key("movieEntries");
                    prettyWriter.StartArray();
                }
                for(auto it=profileToStore.userData.movieEntries.begin();it!=profileToStore.userData.movieEntries.end();it++){
                    prettyWriter.StartObject();
                    writeUintWithConditionPretty(it->movieId>0,"movieId",it->movieId,prettyWriter);
                    writeDoubleWithConditionPretty(it->rating>0,"rating",it->rating,prettyWriter);
                    //tagEntries
                    if(!it->tags.empty()){
                        prettyWriter.Key("tags");
                        prettyWriter.StartArray();
                    }
                    for(auto it_inner = it->tags.begin();it_inner != it->tags.end();it_inner++){
                        prettyWriter.StartObject();
                        writeStringWithConditionPretty(!it_inner->tagName.empty(),"tagName",it_inner->tagName,prettyWriter);
                        writeUintWithConditionPretty(it_inner->tagId>0,"tagId",it_inner->tagId,prettyWriter);
                        writeUintWithConditionPretty(it_inner->tagCount>0,"tagCount",it_inner->tagCount,prettyWriter);
                        prettyWriter.EndObject();
                    }
                    if(!it->tags.empty()){
                        prettyWriter.EndArray();
                    }
                    writeBoolWithConditionPretty("rated",it->rated,prettyWriter);
                    writeBoolWithConditionPretty("tagged",it->tagged,prettyWriter);
                    prettyWriter.EndObject();
                }
                if(!profileToStore.userData.movieEntries.empty()){
                    prettyWriter.EndArray();
                }
                //neighbors
                if(!profileToStore.userData.neighbors.empty()){
                    prettyWriter.Key("neighbors");
                    prettyWriter.StartArray();
                }
                for(unsigned int i=0;i<profileToStore.userData.neighbors.size();i++){
                    prettyWriter.StartObject();
                    NeighborUser currentNeighborUser = profileToStore.userData.neighbors[i];
                    writeUintWithConditionPretty(currentNeighborUser.id > 0,"id",currentNeighborUser.id,prettyWriter);
                    writeDoubleWithConditionPretty(currentNeighborUser.weight > 0,"weight",currentNeighborUser.weight,prettyWriter);
                    if(!currentNeighborUser.commonMoviesIds.empty()){
                        prettyWriter.Key("commonMoviesIds");
                        prettyWriter.StartArray();
                        for(auto it3 = currentNeighborUser.commonMoviesIds.begin(); it3 != currentNeighborUser.commonMoviesIds.end(); it3++){
                            prettyWriter.Uint(*it3);
                        }
                        prettyWriter.EndArray();
                    }
                    prettyWriter.EndObject();
                }
                if(!profileToStore.userData.neighbors.empty()){
                    prettyWriter.EndArray();
                }

                prettyWriter.EndObject();
            }
            prettyWriter.EndObject();
        }
        prettyWriter.EndArray();
    }

    else{
        regularWriter.StartArray();
        for(auto it=allUserProfiles.begin();it!=allUserProfiles.end();it++){
            UserProfileStored profileToStore = it->getStoredProfile();
            regularWriter.StartObject();
            writeStringWithCondition(!profileToStore.profileName.empty(),"profileName",profileToStore.profileName,regularWriter);
            writeBoolWithCondition("isDataBound",profileToStore.isDataBound,regularWriter);
            writeUintWithCondition(profileToStore.userId > 0,"userId",profileToStore.userId,regularWriter);
            if(!profileToStore.isDataBound){
                regularWriter.Key("userData");
                regularWriter.StartObject();
                writeUintWithCondition(profileToStore.userId > 0,"userId",profileToStore.userData.userId,regularWriter);
                writeDoubleWithCondition(profileToStore.userData.meanRating >0,"meanRating",profileToStore.userData.meanRating,regularWriter);
                writeDoubleWithCondition(profileToStore.userData.ratingsStandardDeviation>0,"ratingsStandardDeviation",profileToStore.userData.ratingsStandardDeviation,regularWriter);
                //movieEntries
                if(!profileToStore.userData.movieEntries.empty()){
                    regularWriter.Key("movieEntries");
                    regularWriter.StartArray();
                }
                for(auto it=profileToStore.userData.movieEntries.begin();it!=profileToStore.userData.movieEntries.end();it++){
                    regularWriter.StartObject();
                    writeUintWithCondition(it->movieId>0,"movieId",it->movieId,regularWriter);
                    writeDoubleWithCondition(it->rating>0,"rating",it->rating,regularWriter);
                    //tagEntries
                    if(!it->tags.empty()){
                        regularWriter.Key("tags");
                        regularWriter.StartArray();
                    }
                    for(auto it_inner = it->tags.begin();it_inner != it->tags.end();it_inner++){
                        regularWriter.StartObject();
                        writeStringWithCondition(!it_inner->tagName.empty(),"tagName",it_inner->tagName,regularWriter);
                        writeUintWithCondition(it_inner->tagId>0,"tagId",it_inner->tagId,regularWriter);
                        writeUintWithCondition(it_inner->tagCount>0,"tagCount",it_inner->tagCount,regularWriter);
                        regularWriter.EndObject();
                    }
                    if(!it->tags.empty()){
                        regularWriter.EndArray();
                    }
                    writeBoolWithCondition("rated",it->rated,regularWriter);
                    writeBoolWithCondition("tagged",it->tagged,regularWriter);
                    regularWriter.EndObject();
                }
                if(!profileToStore.userData.movieEntries.empty()){
                    regularWriter.EndArray();
                }
                //neighbors
                if(!profileToStore.userData.neighbors.empty()){
                    regularWriter.Key("neighbors");
                    regularWriter.StartArray();
                }
                for(unsigned int i=0;i<profileToStore.userData.neighbors.size();i++){
                    regularWriter.StartObject();
                    NeighborUser currentNeighborUser = profileToStore.userData.neighbors[i];
                    writeUintWithCondition(currentNeighborUser.id > 0,"id",currentNeighborUser.id,regularWriter);
                    writeDoubleWithCondition(currentNeighborUser.weight > 0,"weight",currentNeighborUser.weight,regularWriter);
                    if(!currentNeighborUser.commonMoviesIds.empty()){
                        regularWriter.Key("commonMoviesIds");
                        regularWriter.StartArray();
                        for(auto it3 = currentNeighborUser.commonMoviesIds.begin(); it3 != currentNeighborUser.commonMoviesIds.end(); it3++){
                            regularWriter.Uint(*it3);
                        }
                        regularWriter.EndArray();
                    }
                    regularWriter.EndObject();
                }
                if(!profileToStore.userData.neighbors.empty()){
                    regularWriter.EndArray();
                }
                regularWriter.EndObject();
            }
            regularWriter.EndObject();
        }
        regularWriter.EndArray();
    }

    ofstream outfile;
    outfile.open(outputFileNameProfiles.c_str());
    outfile << s.GetString();
    outfile.close();

    LOG.info("exportUserProfilesToJson finished. filename="+outputFileNameProfiles);
    LOG.close();
}
