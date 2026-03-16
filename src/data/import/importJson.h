#ifndef IMPORT_JSON_H
#define IMPORT_JSON_H

#include "../../data/dtos.h"
#include <string>

struct JsonParseResultWrapper {
	vector<User> parsedUsers;
	vector<Movie> parsedMovies;
    vector<UserProfileStored> parsedUserProfiles;
};

char* getFiletext(string filepath);

JsonParseResultWrapper parseUsersFromJson(const char* filepath,bool allFieldsRequired);
JsonParseResultWrapper parseMoviesFromJson(const char* filepath,bool allFieldsRequired);
JsonParseResultWrapper parseUserProfilesFromJson(const char* filepath);

#endif
