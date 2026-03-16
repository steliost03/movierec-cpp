#include "importJson.h"

#include "../../exceptions/recommenderExceptions.h"
#include "../../logging/recommenderLogger.h"
#include "../../../include/rapidjson/document.h"
#include "../../../include/rapidjson/filereadstream.h"

using namespace rapidjson;

//TODO: use SAX for large json files?

static RecommenderLogger LOG;

//util functions:

Document parseJsonFromFile(const char* filepath){

    LOG.debug("START: parseJsonFromFile");
    LOG.trace("Opening json file: "+string(filepath));

	FILE* fp = fopen(filepath, "rb"); 

    if (fp == nullptr){
		string filepath_str = filepath;
        LOG.error("Json file was not found: "+filepath_str);
		string exMsg = "Json file was not found: " + filepath_str;
		throw RecJsonParsingException(exMsg);
	}

	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	Document d;
	d.ParseStream(is);

	fclose(fp);
	return d;
}

TagEntry parseTagEntryFromJson(const Value& jsonValue){

	try{

		TagEntry parsedTagEntry;

		if (!(jsonValue.HasMember("tagName"))){
			string exMsg = "Required field of TagEntry: 'tagName' is missing.";
			throw RecJsonParsingException(exMsg);
		}

		if (!(jsonValue.HasMember("tagId"))){
			string exMsg = "Required field of TagEntry: 'tagId' is missing.";
			throw RecJsonParsingException(exMsg);
		}

		parsedTagEntry.tagName = jsonValue["tagName"].GetString();
        parsedTagEntry.tagId = jsonValue["tagId"].GetUint();
        if(jsonValue.HasMember("tagCount")){
            parsedTagEntry.tagCount = jsonValue["tagCount"].GetUint();
        }

		return parsedTagEntry;
	}
	catch (RecJsonParsingException& e){
		throw e;
	}
	catch (exception& e){
		throw RecJsonParsingException(&e);
	}
}

UserEntry parseUserEntryFromJson(const Value& jsonValue){

	try{

		UserEntry parsedUserEntry;

		if (!jsonValue.HasMember("userId")){
			string exMsg = "Required field of UserEntry: 'userId' is missing.";
			throw RecJsonParsingException(exMsg);
		}

        //29apr/2019 update: a user entry can exist without a rating.
//		if (!jsonValue.HasMember("rating")){
//			string exMsg = "Required field of UserEntry: 'rating' is missing.";
//			throw RecJsonParsingException(exMsg);
//		}

        parsedUserEntry.userId = jsonValue["userId"].GetUint();

        if(jsonValue.HasMember("rating")){
            parsedUserEntry.rating = jsonValue["rating"].GetDouble();
            parsedUserEntry.rated = true;
        }

        if(jsonValue.HasMember("tags")){
            parsedUserEntry.tagged = true;
            if (!(jsonValue["tags"].IsArray())){ 
                parsedUserEntry.tags.push_back(parseTagEntryFromJson(jsonValue["tags"]));
            }
            else{
                vector<TagEntry> tags;
                for (auto it_inner = jsonValue["tags"].Begin(); it_inner != jsonValue["tags"].End(); it_inner++){
                    parsedUserEntry.tags.push_back(parseTagEntryFromJson(*it_inner));
                }
            }
        }
		return parsedUserEntry;
	}
	catch (RecJsonParsingException& e){
		throw e;
	}
	catch (exception& e){
		throw RecJsonParsingException(&e);
	}
}

MovieEntry parseMovieEntryFromJson(const Value& jsonValue){

	try{

		MovieEntry parsedMovieEntry;

		if (!(jsonValue.HasMember("movieId"))){
			string exMsg = "Required field of MovieEntry: 'movieId' is missing.";
			throw RecJsonParsingException(exMsg);
		} 

        //29apr/2019 update: a movie entry can exist without a rating.
//		if (!(jsonValue.HasMember("rating"))){
//			string exMsg = "Required field of MovieEntry: 'rating' is missing.";
//			throw RecJsonParsingException(exMsg);
//		}

        parsedMovieEntry.movieId = jsonValue["movieId"].GetUint();

        if(jsonValue.HasMember("rating")){
            parsedMovieEntry.rating = jsonValue["rating"].GetDouble();
            parsedMovieEntry.rated = true;
        }
        
        if(jsonValue.HasMember("tags")){
            parsedMovieEntry.tagged = true;
            if (!(jsonValue["tags"].IsArray())){
                parsedMovieEntry.tags.push_back(parseTagEntryFromJson(jsonValue["tags"]));
            }
            else{
                vector<TagEntry> tags;
                for (auto it_inner = jsonValue["tags"].Begin(); it_inner != jsonValue["tags"].End(); it_inner++){
                    parsedMovieEntry.tags.push_back(parseTagEntryFromJson(*it_inner));
                }
            }
        }

		return parsedMovieEntry;
	}
	catch (RecJsonParsingException& e){
		throw e;
	}
	catch (exception& e){
		throw RecJsonParsingException(&e);
	}
}

NeighborUser parseNeighborUserFromJson(const Value& jsonValue){
    
    NeighborUser parsedNeighborUser;
    try{

        if(!(jsonValue.HasMember("id"))){
            string exMsg = "Required field of NeighborUser: 'id' is missing.";
			throw RecJsonParsingException(exMsg);
        }
        
        parsedNeighborUser.id = jsonValue["id"].GetUint();
        
        if(jsonValue.HasMember("weight")){
            parsedNeighborUser.weight = jsonValue["weight"].GetDouble();
        }
        
        if(jsonValue.HasMember("commonMoviesIds")){
            if (!(jsonValue["commonMoviesIds"].IsArray())){
                parsedNeighborUser.commonMoviesIds.push_back(jsonValue["commonMoviesIds"].GetUint());
            }
            else{
                for(auto it = jsonValue["commonMoviesIds"].Begin();it != jsonValue["commonMoviesIds"].End(); it++){
                    parsedNeighborUser.commonMoviesIds.push_back(it->GetUint());
                }
            }
        }
    }
	catch (RecJsonParsingException& e){
		throw e;
	}
	catch (exception& e){
		throw RecJsonParsingException(&e);
	}
    return parsedNeighborUser;
}

NeighborMovie parseNeighborMovieFromJson(const Value& jsonValue){
    
    NeighborMovie parsedNeighborMovie;
    try{
        
        if(!(jsonValue.HasMember("id"))){
            string exMsg = "Required field of NeighborMovie: 'id' is missing.";
			throw RecJsonParsingException(exMsg);
        }
        
        parsedNeighborMovie.id = jsonValue["id"].GetUint();
        
        if(jsonValue.HasMember("weight")){
            parsedNeighborMovie.weight = jsonValue["weight"].GetDouble();
        }
        
        if(jsonValue.HasMember("commonUsersIds")){
            if (!(jsonValue["commonUsersIds"].IsArray())){
                parsedNeighborMovie.commonUsersIds.push_back(jsonValue["commonUsersIds"].GetUint());
            }
            else{
                for(auto it = jsonValue["commonUsersIds"].Begin();it != jsonValue["commonUsersIds"].End(); it++){
                    parsedNeighborMovie.commonUsersIds.push_back(it->GetUint());
                }
            }
        }
    }
	catch (RecJsonParsingException& e){
		throw e;
	}
	catch (exception& e){
		throw RecJsonParsingException(&e);
	}
    return parsedNeighborMovie;
}

string parseGenreFromJson(const Value& jsonValue){

    string genre;
    try {
        genre = jsonValue.GetString();
    }
    catch (RecJsonParsingException& e) {
        throw e;
    }
    catch(exception& e){
        throw RecJsonParsingException(&e);
    }
    return genre;
}

User parseSingleUserFromJson(const Value& jsonValue,bool allFieldsRequired){

    User parsedUser;
    try{
        if (!(jsonValue.HasMember("userId"))){
            string exMsg = "Required field of User: 'userId' is missing.";
            throw RecJsonParsingException(exMsg);
        }
        parsedUser.userId = jsonValue["userId"].GetUint();

        if (jsonValue.HasMember("meanRating")){
            parsedUser.meanRating = jsonValue["meanRating"].GetDouble();
        } else{
            if(allFieldsRequired){
                string exMsg = "Field of User: 'meanRating' is missing";
                throw RecJsonParsingException(exMsg);
            }
        }

        if (jsonValue.HasMember("ratingsStandardDeviation")){
            parsedUser.ratingsStandardDeviation = jsonValue["ratingsStandardDeviation"].GetDouble();
        } else{
            if(allFieldsRequired){
                string exMsg = "Field of User: 'ratingsStandardDeviation' is missing";
                throw RecJsonParsingException(exMsg);
            }
        }

        if (jsonValue.HasMember("movieEntries")){
            if (!(jsonValue["movieEntries"].IsArray())){
                parsedUser.movieEntries.push_back(parseMovieEntryFromJson(jsonValue["movieEntries"]));
            }
            else{
                for (auto it_inner = jsonValue["movieEntries"].Begin(); it_inner != jsonValue["movieEntries"].End(); it_inner++){
                    parsedUser.movieEntries.push_back(parseMovieEntryFromJson(*it_inner));
                }
            }
        } else{
            if(allFieldsRequired){
                string exMsg = "Field of User: 'movieEntries' is missing.";
                throw RecJsonParsingException(exMsg);
            }
        }
        if(jsonValue.HasMember("neighbors")){
            if (!(jsonValue["neighbors"].IsArray())){
                parsedUser.neighbors.push_back(parseNeighborUserFromJson(jsonValue["neighbors"]));
            }
            else{
                for(auto it_inner = jsonValue["neighbors"].Begin();it_inner != jsonValue["neighbors"].End(); it_inner++){
                    parsedUser.neighbors.push_back(parseNeighborUserFromJson(*it_inner));
                }
            }
        } else{
            if(allFieldsRequired){
                string exMsg = "Field of User: 'neighbors' is missing.";
                throw RecJsonParsingException(exMsg);
            }
        }
    } catch(RecJsonParsingException& e){
        throw e;
    } catch(exception& e){
        throw RecJsonParsingException(&e);
    }
    return parsedUser;
}

//* * * * * * * *
//MAIN FUNCTIONS:
//* * * * * * * *

JsonParseResultWrapper parseUsersFromJson(const char* filepath,bool allFieldsRequired){
    
    LOG.init("importJson:parseUsersFromJson");
    LOG.info("START: parseUsersFromJson");
    
	try{

		JsonParseResultWrapper result;
		Document d = parseJsonFromFile(filepath);

		if (d.IsArray()){
			for (Value::ConstValueIterator it = d.Begin(); it != d.End(); it++){
				User parsedUser;
				if (!(it->HasMember("userId"))){
					string exMsg = "Required field of User: 'userId' is missing.";
					throw RecJsonParsingException(exMsg);
				}
                parsedUser.userId = (*it)["userId"].GetUint();

				if (it->HasMember("meanRating")){
					parsedUser.meanRating = (*it)["meanRating"].GetDouble();
				} else{
                    if(allFieldsRequired){
                        string exMsg = "Field of User: 'meanRating' is missing";
                        throw RecJsonParsingException(exMsg);
                    }
                }

				if (it->HasMember("ratingsStandardDeviation")){
					parsedUser.ratingsStandardDeviation = (*it)["ratingsStandardDeviation"].GetDouble();
				} else{
                    if(allFieldsRequired){
                        string exMsg = "Field of User: 'ratingsStandardDeviation' is missing";
                        throw RecJsonParsingException(exMsg);
                    }
                }

				if (it->HasMember("movieEntries")){
					if (!((*it)["movieEntries"].IsArray())){
						parsedUser.movieEntries.push_back(parseMovieEntryFromJson((*it)["movieEntries"]));
					}
					else{
						for (auto it_inner = (*it)["movieEntries"].Begin(); it_inner != (*it)["movieEntries"].End(); it_inner++){
							parsedUser.movieEntries.push_back(parseMovieEntryFromJson(*it_inner));
						}
					}
				} else{
                    if(allFieldsRequired){
                        string exMsg = "Field of User: 'movieEntries' is missing.";
                        throw RecJsonParsingException(exMsg);
                    }
                }
                if(it->HasMember("neighbors")){
                	if (!((*it)["neighbors"].IsArray())){
                        parsedUser.neighbors.push_back(parseNeighborUserFromJson((*it)["neighbors"]));
                    }
                    else{
                        for(auto it_inner = (*it)["neighbors"].Begin();it_inner != (*it)["neighbors"].End(); it_inner++){
                            parsedUser.neighbors.push_back(parseNeighborUserFromJson(*it_inner));
                        }
                    }
                } else{
                    if(allFieldsRequired){
                        string exMsg = "Field of User: 'neighbors' is missing.";
                        throw RecJsonParsingException(exMsg);
                    }
                }
				result.parsedUsers.push_back(parsedUser);
			}
		}
		else{
            LOG.error("Invalid format for users json file. Users json file should be in json array format");
            string exMsg = "Invalid format for users json file. Multiple users from json file should be in valid json array format.";
			throw RecJsonParsingException(exMsg); 
		}

        LOG.info("parseUsersFromJson finished.");
        LOG.close();
		return result;
	}

	catch (RecJsonParsingException& e){
		throw e;
	}
	catch (exception& e){
		throw RecJsonParsingException(&e);
	}
}

JsonParseResultWrapper parseMoviesFromJson(const char* filepath,bool allFieldsRequired){
    
    LOG.init("importJson:parseMoviesFromJson");
    LOG.info("START: parseMoviesFromJson");

	try{

		JsonParseResultWrapper result;
		Document d = parseJsonFromFile(filepath);

		if (d.IsArray()){
			for (Value::ConstValueIterator it = d.Begin(); it != d.End(); it++){
				Movie parsedMovie;

				if (!(it->HasMember("movieId"))){
					string exMsg = "Required field of Movie: 'movieId' is missing.";
					throw RecJsonParsingException(exMsg);
				}

				if (!(it->HasMember("movieName"))){
					string exMsg = "Required field of Movie: 'movieName' is missing.";
					throw RecJsonParsingException(exMsg);
				}

				if (!(it->HasMember("genres"))){
					string exMsg = "Required field of Movie: 'genres' is missing.";
					throw RecJsonParsingException(exMsg);
				}

                parsedMovie.movieId = (*it)["movieId"].GetUint();
				parsedMovie.movieName = (*it)["movieName"].GetString();

                if(!((*it)["genres"].IsArray())){
                    parsedMovie.genres.push_back(parseGenreFromJson((*it)["genres"]));
                }else{
                    for(auto it_inner = (*it)["genres"].Begin(); it_inner != (*it)["genres"].End(); it_inner++)
                    parsedMovie.genres.push_back(parseGenreFromJson(*it_inner));
                }
                
                if(it->HasMember("meanRating")){
                    parsedMovie.meanRating = (*it)["meanRating"].GetDouble();
                } else{
                    if(allFieldsRequired){
                        string exMsg = "Field of Movie: 'meanRating' is missing.";
                        throw RecJsonParsingException(exMsg);
                    }
                }
                
                if(it->HasMember("ratingsStandardDeviation")){
                    parsedMovie.ratingsStandardDeviation = (*it)["ratingsStandardDeviation"].GetDouble();
                } else{
                    if(allFieldsRequired){
                        string exMsg = "Field of Movie: 'ratingsStandardDeviation' is missing.";
                        throw RecJsonParsingException(exMsg);
                    }
                }
                
                /* MAYBE ADD IN FUTURE.
                if(it->HasMember("inverseUserFrequency")){
                    parsedMovie.inverseUserFrequency = (*it)["inverseUserFrequency"].GetDouble();
                } else{
                    if(allFieldsRequired){
                        string exMsg = "Field of Movie: 'inverseUserFrequency' is missing.";
                        throw RecJsonParsingException(exMsg);
                    }
                }
                */
                
                if(it->HasMember("totalTags")){
                    if (!((*it)["totalTags"].IsArray())){
						parsedMovie.totalTags.push_back(parseTagEntryFromJson((*it)["totalTags"]));
					}
					else{
						for (auto it_inner = (*it)["totalTags"].Begin(); it_inner != (*it)["totalTags"].End(); it_inner++){
							parsedMovie.totalTags.push_back(parseTagEntryFromJson(*it_inner));
						}
					}
                } else{
                    if(allFieldsRequired){
                        string exMsg = "Field of Movie: 'totalTags' is missing";
                        throw RecJsonParsingException(exMsg);
                    }
                }
                
				if (it->HasMember("userEntries")){
					if (!((*it)["userEntries"].IsArray())){
						parsedMovie.userEntries.push_back(parseUserEntryFromJson((*it)["userEntries"]));
					}
					else{
						for (auto it_inner = (*it)["userEntries"].Begin(); it_inner != (*it)["userEntries"].End(); it_inner++){
							parsedMovie.userEntries.push_back(parseUserEntryFromJson(*it_inner));
						}
					}
				} else{
                    if(allFieldsRequired){
                        string exMsg = "Field of Movie: 'userEntries' is missing.";
                        throw RecJsonParsingException(exMsg);
                    }
                }

                if(it->HasMember("neighbors")){
                    if (!((*it)["neighbors"].IsArray())){
                        parsedMovie.neighbors.push_back(parseNeighborMovieFromJson(((*it)["neighbors"])));
					}
					else{
						for (auto it_inner = (*it)["neighbors"].Begin(); it_inner != (*it)["neighbors"].End(); it_inner++){
                            parsedMovie.neighbors.push_back(parseNeighborMovieFromJson(*it_inner));
						}
					}
                } else{
                    if(allFieldsRequired){
                        string exMsg = "Field of Movie: 'neighbors' is missing.";
                        throw RecJsonParsingException(exMsg);
                    }
                }
				result.parsedMovies.push_back(parsedMovie);
			}
		}
		else{
            LOG.error("Invalid format for movies json file. Movies json file should be in json array format");
            string exMsg = "Invalid format for movies json file. Multiple movies from json file should be in valid json array format.";
			throw RecJsonParsingException(exMsg); 
		}
        LOG.info("parseMoviesFromJson finished.");
		return result;
	}
    catch(RecJsonParsingException& e){
        throw e;
    }
	catch (exception& e){
		throw RecJsonParsingException(&e);
	}
}

JsonParseResultWrapper parseUserProfilesFromJson(const char* filepath/*,RecommenderDataHandler& rdh*/){

    LOG.init("importJson:parseUserProfilesFromJson");
    LOG.info("START: parseUserProfilesFromJson");

    try {

        JsonParseResultWrapper result;
        Document d = parseJsonFromFile(filepath);

        LOG.trace("Document parsed.");

        if(d.IsArray()){
            for (Value::ConstValueIterator it = d.Begin(); it != d.End(); it++){
                UserProfileStored parsedUserProfileStored;
                if(!(it->HasMember("profileName"))){
                    string exMsg = "Required field of UserProfile json: 'profileName' is missing.";
                    throw RecJsonParsingException(exMsg);
                }

                if(!(it->HasMember("isDataBound"))){
                    string exMsg = "Required field of UserProfile json: 'isDataBound' is missing.";
                    throw RecJsonParsingException(exMsg);
                }

                parsedUserProfileStored.profileName = (*it)["profileName"].GetString();
                parsedUserProfileStored.isDataBound = (*it)["isDataBound"].GetBool();

                if(!parsedUserProfileStored.isDataBound){
                    if(!(it->HasMember("userData"))){
                        string exMsg = "Required field of 'standalone' UserProfile json: 'userData' is missing.";
                        throw RecJsonParsingException(exMsg);
                    }
                    User parsedUser = parseSingleUserFromJson((*it)["userData"],false);
                    parsedUserProfileStored.userData = parsedUser;
                } else{
                    if(!(it->HasMember("userId"))){
                        string exMsg = "Required field of 'databound' UserProfile json: 'userId' is missing.";
                        throw RecJsonParsingException(exMsg);
                    }
                    parsedUserProfileStored.userId = (*it)["userId"].GetUint();
                }
                result.parsedUserProfiles.push_back(parsedUserProfileStored);
            }
        } else{
            LOG.error("Invalid format for user profiles json file. User profile json file should be in json array format.");
            string exMsg = "Invalid format for user profiles json file. User profiles should be in valid json array format.";
            throw RecJsonParsingException(exMsg);
        }
        LOG.info("parseUserProfilesFromJson finished. "
                 "parsedUserProfilesCount="+to_string(result.parsedUserProfiles.size()));
        return result;

    } catch(RecJsonParsingException& e) {
        throw e;
    }
    catch(exception& e){
        throw RecJsonParsingException(&e);
    }
}
