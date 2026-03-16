/*
Copyright (C) 2018 Stylianos Tsiakalos
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef DEMOCONFIG
#define DEMOCONFIG

#include "DEMO_DEPENDENCIES.h"

#define MOVTITLE_LEN 175   //default = 175
#define TAGNAME_LEN 175    //default = 175
#define GENRENAME_LEN 20   //default = 20
#define MAXGENRECOUNT 15   //default = 15
#define MAXTAGS_PERUSER 20 //default = 20

#define USRENTR_REALLOC_FACTOR 1.1 //default = 1.1
#define TAGS_REALLOC_FACTOR 1.5    //default = 1.5

#define AUTOMATIC_MULTITHREAD 1 //if enabled,the size of the files determines if and which multithreading will be used.
								//default: enabled

//In the case that automatic multithread is disabled:
#define MULTITHREAD_PARSE_RATINGS_ENABLED 1
#define MULTITHREAD_PARSE_OTHER_ENABLED 0
#define MULTITHREAD_SEARCH_ENABLED 1

#define DEFAULT_THREADCOUNT 4 //only applicable if multithread is enabled.
							  //default: 4

string OUTPUT_FILE_PATH_BEGIN = "demo_output_files/";

string MOCK_MOVIESFILE_PATH = "files/mockfiles/movies.csv";
string MOCK_RATINGSFILE_PATH = "files/mockfiles/ratings.csv";
string MOCK_TAGSFILE_PATH = "files/mockfiles/tags.csv";
string MOCK_GENOMETAGSFILE_PATH = "files/mockfiles/genome-tags.csv";

string SMALL_MOVIESFILE_PATH = "files/full_files/100k/movies.csv";
string SMALL_RATINGSFILE_PATH = "files/full_files/100k/ratings.csv";
string SMALL_TAGSFILE_PATH = "files/full_files/100k/tags.csv";
string SMALL_GENOMETAGSFILE_PATH = "files/full_files/100k/genome-tags.csv";

string LARGE_MOVIESFILE_PATH = "files/full_files/10m/movies.csv";
string LARGE_RATINGSFILE_PATH = "files/full_files/10m/ratings.csv";
string LARGE_TAGSFILE_PATH = "files/full_files/10m/tags.csv";
string LARGE_GENOMETAGSFILE_PATH = "files/full_files/10m/genome-tags.csv";

string VERY_LARGE_MOVIESFILE_PATH = "files/full_files/20m/movies.csv";
string VERY_LARGE_RATINGSFILE_PATH = "files/full_files/20m/ratings.csv";
string VERY_LARGE_TAGSFILE_PATH = "files/full_files/20m/tags.csv";
string VERY_LARGE_GENOMETAGSFILE_PATH = "files/full_files/20m/genome-tags.csv";

#endif
