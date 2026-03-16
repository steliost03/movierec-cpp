#ifndef MOV_GEN_UTIL
#define MOV_GEN_UTIL

#include "movieGenres.h"
#include <vector>
#include <string>

struct GenreResultWrapper {
    bool isValid;
    MovieGenre genre;
};

GenreResultWrapper determineGenre(std::string str,std::vector<std::string> validGenres);

#endif
