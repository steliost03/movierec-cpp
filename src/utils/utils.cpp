#include "utils.h"
#include <sstream>
#include <iostream>
#include <exception>
#include <algorithm>
#include <cctype>
#include <cmath>
using namespace std;

typedef unsigned int uint;
typedef unsigned long ulong;

//returns 'false' for negative numbers.
bool strIsNumeric(string str){
    for(size_t i=0;i<str.size();i++){
        if(!isdigit(str[i])){
            return false;
        }
    }
    return true;
}

//first character must be the minus sign.
bool strIsNegativeNumber(string str){
    if(str.at(0) == '-'){
        str = str.substr(1,string::npos);
        return strIsNumeric(str);
    } else{
        return false;
    }
}

//assumes that input 'str' already represents a number.
unsigned int strToUint(string str){
    unsigned int digitCount = static_cast<unsigned int>(str.size());
    unsigned int result = 0;
    for(size_t i=0;i<str.size();i++){
        result = result + charToUint(str[i])*static_cast<unsigned int>(pow(10,digitCount-1));
        digitCount--;
    }
    return result;
}

//assumes that input 'str' already represents a number.
unsigned long strToUlong(string str){
    unsigned int digitCount = static_cast<unsigned int>(str.size());
    unsigned long result = 0;
    for(size_t i=0;i<str.size();i++){
        result = result + charToUlong(str[i])*static_cast<unsigned long>(pow(10,digitCount-1));
        digitCount--;
    }
    return result;
}

long strToPositiveLong(std::string str){
    unsigned int digitCount = static_cast<unsigned int>(str.size());
    long result = 0;
    for(size_t i=0;i<str.size();i++){
        result = result + charToPositiveLong(str[i])*static_cast<long>(pow(10,digitCount-1));
        digitCount--;
    }
    return result;
}

double strToDouble(std::string str){
    return strtod(str.c_str(),nullptr);
}

//converts a boolean str to either 'true' or 'false.
std::string boolToAlphaStr(bool b){
    return b ? "true" : "false";
}

//assumes that the input 'char' already represents a digit.
unsigned int charToUint(char c){
    return static_cast<unsigned int>(c - '0');
}

//assumes that the input 'char' already represents a digit.
unsigned long charToUlong(char c){
    return static_cast<unsigned long>(c - '0');
}

//assumes that the input 'char' already represents a digit.
long charToPositiveLong(char c){
    return static_cast<long>(c - '0');
}

string vecUlongAsCommaSeparatedStr(vector<unsigned long> vec){
    string producedStr = "";
    for(size_t i=0;i<vec.size();i++){
        producedStr += vec[i];
        if(i+1 < vec.size()){
            producedStr += ",";
        }
    }
    return producedStr;
}

string vecStrAsCommaSeparatedStr(vector<string> vec){
    string producedStr = "";
    for(size_t i=0;i<vec.size();i++){
        producedStr += vec[i];
        if(i+1 < vec.size()){
            producedStr += ",";
        }
    }
    return producedStr;
}

vector<string> splitAroundCharacter(string str, char c){

	stringstream orig(str);
	string segment;
	vector<string> seglist;

	while (getline(orig, segment, c)) {
		seglist.push_back(segment);
	}

	return seglist;
}

void reportError(RecommenderException& e){
	cout << "- - -" << endl;
	cout << "An error has occured." << endl;
	cout << "Error: " << e.getExceptionName() << endl;
	if (!e.getExceptionMessage().empty()){
		cout << "Message: " << e.getExceptionMessage() << endl;
	}
    if (e.getExceptionCause() != nullptr){
        if(e.isCauseRecommenderException()){
            RecommenderException* cause = dynamic_cast<RecommenderException*>(e.getExceptionCause()); 
            cout<<"Cause: "<<cause->getExceptionName()<<endl;
            cout<<"Cause Message: "<<cause->getExceptionMessage()<<endl;
        } else{
            cout << "Cause: " << e.getExceptionCause()->what() << endl;
        }
	}
	cout << "- - -" << endl;
	cout << "Press Enter to exit." << endl;
	cin.get();
}

//lighter version for non-critical errors.
bool reportErrorLight(RecommenderException& e){
    return reportErrorLight(e,"");
}

//lighter version for non-critical errors.
bool reportErrorLight(RecommenderException& e,string extraMessage){

    bool terminate;
    cout << "- - -" << endl;
    cout << "WARNING: a (non-fatal) error has occured." << endl;
    cout << "Error: " << e.getExceptionName() << endl;
    if (!e.getExceptionMessage().empty()){
        cout << "Message: " << e.getExceptionMessage() << endl;
    }
    if (e.getExceptionCause() != nullptr){
        if(e.isCauseRecommenderException()){
            RecommenderException* cause = dynamic_cast<RecommenderException*>(e.getExceptionCause());
            cout<<"Cause: "<<cause->getExceptionName()<<endl;
            cout<<"Cause Message: "<<cause->getExceptionMessage()<<endl;
        } else{
            cout << "Cause: " << e.getExceptionCause()->what() << endl;
        }
    }
    cout << "- - -" << endl;
    cout<<extraMessage<<endl;
    cout << "Input 'c' to continue the application, or anything else to terminate."<<endl;
    string selection_str;
    char selection;
    getline(cin,selection_str);
    if(selection_str.empty()){
        return true;
    }
    selection = selection_str.at(0);
    switch(selection){
        case 'c':
            terminate = false;
            break;
        default:
            terminate = true;
            break;
    }
    return terminate;
}

void stripLeadingWhitespaces(string& str) {
    // erase from the start, to the first character that is not whitespace.
    str.erase(str.begin(), find_if(str.begin(), str.end(), [](unsigned char ch) {
                  return !isspace(ch);
              }));
}

void stripTrailingWhitespaces(string& str) {
    // erase from the first non-whitespace character (found from the back), to the end.
    str.erase(find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
                  return !isspace(ch);
              }).base(), str.end());
}


void stripLeadingAndTrailingWhitespaces(string& str){
    stripLeadingWhitespaces(str);
    stripTrailingWhitespaces(str);
}

string removeWhitespaces(string str){
    str.erase(remove_if(str.begin(),str.end(),::isspace),str.end());
    return str;
}

//converts all instances of more than 1 whitespaces to 1 whitespace.
string removeExtraWhitespaces(string str){

    string output;
    output.reserve(str.size());
    const char* input = str.c_str();

    unsigned int inputIndex = 0;
    while(input[inputIndex] != '\0')
    {
        output.push_back(input[inputIndex]);

        if(input[inputIndex] == ' ')
        {
            while(input[inputIndex + 1] == ' ')
            {
                // skip over any extra spaces
                inputIndex++;
            }
        }

        inputIndex++;
    }

    return output;
}

string strToLower(string str){
    for(auto it= str.begin();it!=str.end(); it++){
        *it = static_cast<char>(tolower(*it));
    }
    return str;
}


// // matrix utils:

void writeToFile_UserEntries(unsigned long maxTagsPerUser,float** userEntries, size_t rowcount,FILE* file){

    cout << "Writing userEntries matrix to file..." << endl;

    fprintf(file,"-- USER ENTRIES MATRIX --\n");
    fprintf(file,"[userID],[movieID],[rating],[tagIDs]\n");

    for (uint r = 0; r < rowcount; r++){
        for (uint c = 0; c < (3 + maxTagsPerUser); c++){
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

void writeToFile_Movies(char*** movies, size_t rowcount,FILE* file){

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

void writeToFile_GenreEntries(unsigned long maxGenreCount,char*** genreEntries, size_t rowcount,FILE* file){

    cout << "Writing genreEntries matrix to file..." << endl;

    fprintf(file, "-- GENRE ENTRIES MATRIX --\n");
    fprintf(file, "[genres]\n");

    for (uint r = 0; r < rowcount; r++){

        for (uint c = 0; c < maxGenreCount; c++){
            if (genreEntries[r][c][0] == 0)
                break;
            fprintf(file,"%s",genreEntries[r][c]);
            fprintf(file,"|");

        }
        fprintf(file, "\n");

    }

    cout << "done." << endl;
}

void writeToFile_Tags(char** tags, size_t rowcount,FILE* file){

    cout << "Writing tags matrix to file..." << endl;

    fprintf(file, "-- TAGS MATRIX --\n");
    fprintf(file, "[tag]\n");

    for (uint r = 0; r < rowcount; r++){
        fprintf(file,"%s",tags[r]);
        fprintf(file,"\n");
    }

    cout << "done." << endl;
}
