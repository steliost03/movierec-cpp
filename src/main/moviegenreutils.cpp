#include "movieGenresUtils.h"
#include "../utils/utils.h"

using namespace std;

string formatInput(string str){
    str = removeWhitespaces(str);
    str = strToLower(str);
    return str;
}

vector<string> formatValidGenres(vector<string> unformattedVec){
    vector<string> formattedVec;
    for(auto it = unformattedVec.begin();it!=unformattedVec.end();it++){
        string formattedGenre = *it;
        formattedGenre = formatInput(formattedGenre);
        formattedVec.push_back(formattedGenre);
    }
    return formattedVec;
}

GenreResultWrapper determineGenre(string str,vector<string> validGenres){
    GenreResultWrapper result;
    //formattedValidGenres vector's elements will match 1-1 with 'the validGenresStrVec' vector's elements.
    vector<string> formattedValidGenres = formatValidGenres(validGenres);
    str = formatInput(str);
    for(unsigned int i=0;i<formattedValidGenres.size();i++){
        if(str == formattedValidGenres[i]){
            result.isValid = true;
            result.genre = MovieGenre(validGenres[i]);
            return result;
        }
    }
    result.isValid = false;
    return result;
}
