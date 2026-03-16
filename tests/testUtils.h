#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "../src/exceptions/recommenderExceptions.h"
#include "../src/application/configuration/configuration.h"
#include "../src/data/dtos.h"
#include "../src/main/movieRecommenderResult.h"
#include <string>
#include <vector>

class TestResultWrapper {

private:

	bool testSuccess = false;
	std::string message;
	std::string testName;

public:

	TestResultWrapper(){}

	TestResultWrapper(bool testSuccess, std::string testName){
		this->testSuccess = testSuccess;
		this->testName = testName;
		this->message = "";
	}

	TestResultWrapper(bool testSuccess, std::string testName, std::string message){
		this->testSuccess = testSuccess;
		this->testName = testName;
		this->message = message;
	}

	TestResultWrapper(RecommenderException e, std::string testName){
		testSuccess = false;
		this->testName = testName;
		message = e.getExceptionMessage();
	}

	bool success(){
		return testSuccess;
	}

	std::string getTestName(){
		return testName;
	}

	std::string getMessage(){
		return message;
	}
};

bool userIdComparator(User a,User b);
bool userIdEntryComparator(UserEntry a, UserEntry b);
bool userIdNeighborComparator(NeighborUser a, NeighborUser b);
bool movieIdComparator(Movie a, Movie b);
bool movieIdEntryComparator(MovieEntry a,MovieEntry b);
bool movieIdNeighborComparator(NeighborMovie a, NeighborMovie b);
bool tagIdComparator(TagEntry a,TagEntry b);
bool recommendationMovieIdComparator(MovieRecommendation a, MovieRecommendation b);
bool userProfileComparator(UserProfileFull a,UserProfileFull b);

void printCommonIds(vector<unsigned long> common);
void printNeighborUser(NeighborUser neighbor);
void printNeighborUsers(vector<NeighborUser> neighbors);
void printNeighborMovie(NeighborMovie neighbor);
void printNeighborMovies(vector<NeighborMovie> neighbors);
void printTagEntry(TagEntry tagEntry);
void printTagEntries(vector<TagEntry> tagEntries);
void printUserEntry(UserEntry userEntry);
void printUserEntries(vector<UserEntry> userEntries);
void printMovieEntry(MovieEntry movieEntry);
void printMovieEntries(vector<MovieEntry> movieEntries);
void printUser(User user);
void printUsers(vector<User> users);
void printMovie(Movie movie);
void printMovies(vector<Movie> movies);
void printAllUsers(vector<User>& allUsers);
void printAllMovies(vector<Movie>& allMovies);
void printStrVector(vector<string>,string separator=",");

void printRecommendations(vector<MovieRecommendation>,bool);
void printRecommendation(MovieRecommendation,bool);

void writeCommonIdsToFile(vector<unsigned long> common,ofstream& f);
void writeNeighborUserToFile(NeighborUser neighbor,ofstream& f);
void writeNeighborUsersToFile(vector<NeighborUser> neighbors,ofstream& f);
void writeNeighborMovieToFile(NeighborMovie neighbor,ofstream& f);
void writeNeighborMoviesToFile(vector<NeighborMovie> neighbors,ofstream& f);
void writeTagEntryToFile(TagEntry tagEntry,ofstream& f);
void writeTagEntriesToFile(vector<TagEntry> tagEntries,ofstream& f);
void writeUserEntryToFile(UserEntry userEntry,ofstream& f);
void writeUserEntriesToFile(vector<UserEntry> userEntries,ofstream& f);
void writeMovieEntryToFile(MovieEntry movieEntry,ofstream& f);
void writeMovieEntriesToFile(vector<MovieEntry> movieEntries,ofstream& f);
void writeUserToFile(User user,ofstream& f);
void writeUsersToFile(vector<User> users,ofstream& f);
void writeMovieToFile(Movie movie,ofstream& f);
void writeMoviesToFile(vector<Movie> movies,ofstream& f);
void writeAllUsersToFile(vector<User>& allUsers,ofstream& f);
void writeAllMoviesToFile(vector<Movie>& allMovies,ofstream& f);

void writeMovieRecommendationToFile(MovieRecommendation rec,ofstream& f,bool minimal=false);
void writeMovieRecommendationsToFile(vector<MovieRecommendation> recommendations,ofstream& f,bool minimal=false);
void writeMovieRecommendationWrapperToFile(MovieRecommendationsWrapper w,ofstream& f);
void writeMovieRecommendationWrappersToFile(vector<MovieRecommendationsWrapper> vw , ofstream& f);

void writeUserProfileToFile(UserProfileFull userProfile,ofstream& f);
void writeUserProfilesVectorToFile(vector<UserProfileFull> userProfiles,ofstream& f);

bool tagEntriesEqual(TagEntry tagEntry1 ,TagEntry tagEntry2);
bool tagEntryVectorsEqual(vector<TagEntry> tagEntries1, vector<TagEntry> tagEntries2);
bool userEntriesEqual(UserEntry userEntry1, UserEntry userEntry2);
bool movieEntriesEqual(MovieEntry movieEntry1, MovieEntry movieEntry2);
bool movieEntryVectorsEqual(vector<MovieEntry> movieEntries1, vector<MovieEntry> movieEntries2);
bool neighborUsersEqual(NeighborUser neighborUser1,NeighborUser neighborUser2);
bool neighborUserVectorsEqual(vector<NeighborUser> neighborUsers1,vector<NeighborUser> neighborUsers2);
bool neighborMoviesEqual(NeighborMovie neighborMovie1,NeighborMovie neighborMovie2);
bool neighborUserVectorsEqual(vector<NeighborUser> neighborUsers1,vector<NeighborUser> neighborUsers2);
bool usersEqual(User user1,User user2);
bool userVectorsEqual(vector<User> users1,vector<User> users2);
bool moviesEqual(Movie movie1,Movie movie2);
bool movieVectorsEqual(vector<Movie> movies1,vector<Movie> movies2);
bool recommendationsEqual(MovieRecommendation rec1,MovieRecommendation rec2,bool minimal=false);
bool recommendationVectorsEqual(vector<MovieRecommendation> recVec1, vector<MovieRecommendation> recVec2,bool minimal=false);

bool userProfilesStandaloneEqual(UserProfileFull profile1,UserProfileFull profile2);
bool userProfileStandaloneVectorsEqual(vector<UserProfileFull> profileVec1,vector<UserProfileFull> profileVec2);
bool userProfilesDataBoundEqual(UserProfileFull profile1,UserProfileFull profile2);
bool userProfilesDataBoundVectorsEqual(vector<UserProfileFull> profileVec1,vector<UserProfileFull> profileVec2);
bool userProfilesFullEqual(UserProfileFull profile1,UserProfileFull profile2);
bool userProfilesFullVectorsEqual(vector<UserProfileFull> profileVec1,vector<UserProfileFull> profileVec2);

void reportTestResults(TestResultWrapper result,bool enterToContinue=true);
void reportTestResults(std::vector<TestResultWrapper> results,bool enterToContinue=false);

UserProfileFull createDummyUserProfileStandalone(string profileName,User userData,bool dataIsAvailable);
UserProfileFull createDummyUserProfileDataBound(string profileName,User* userDataPtr,bool dataIsAvailable);

Configuration createDummyConfig();
int randomInt(int minInt, int maxInt);
double randomdouble(double mindouble, double maxdouble);
void extractRatingsFromMovieEntries(std::vector<MovieEntry> movieEntries, std::vector<double>& ratings);
void extractRatingsFromUserEntries(std::vector<UserEntry> userEntries, std::vector<double>& ratings);
TagEntry createDummyTagEntry(unsigned long tagId, std::string tagName, unsigned int tagCount);
TagEntry createDummyTagEntryPreset(short id);
TagEntry createDummyTagEntryRandom();

//version where tags are NOT supplied.
MovieEntry createDummyMovieEntry(unsigned long movieId, double rating);

//version where tags are supplied.
MovieEntry createDummyMovieEntry(unsigned long movieId, double rating, std::vector<TagEntry> tags);

MovieEntry createDummyMovieEntryPreset(short id);
MovieEntry createDummyMovieEntryRandom();
NeighborUser createDummyNeighborUser(unsigned long id, double weight, std::vector<unsigned long> commonMoviesIds);
NeighborUser createDummyNeighborUserPreset(short id);
NeighborUser createDummyNeighborUserRandom();

//version with all parameters given.
User createDummyUser(unsigned long userId, std::vector<MovieEntry> movieEntries, std::vector<NeighborUser> neighbors, double meanRating, double ratingStdDev);

//version with some empty parameters (fake scenarios).
User createDummyUser(unsigned long userId,double meanRating,double ratingStdDev);
User createDummyUser(unsigned long userId,std::vector<MovieEntry> movieEntries,double meanRating,double ratingStdDev);

//version with some parameters calculated.
User createDummyUser(unsigned long userId, std::vector<MovieEntry> movieEntries, std::vector<NeighborUser> neighbors);

User createDummyUserPreset(short id);
User createDummyUserRandom();

//version where tags are not supplied.
UserEntry createDummyUserEntry(unsigned long userId, double rating);

//version where tags are supplied.
UserEntry createDummyUserEntry(unsigned long userId, double rating, std::vector<TagEntry> tags);

UserEntry createDummyUserEntryPreset(short id);
UserEntry createDummyUserEntryRandom();

//version with all parameters given.
Movie createDummyMovie(unsigned long movieId, std::string movieName, double meanRating, double ratingsStandardDeviation, double inverseUserFrequency, std::vector<std::string> genres
	, std::vector<TagEntry> totalTags, std::vector<UserEntry> userEntries, std::vector<NeighborMovie> neighbors);

//version with some empty parameters (fake scenarios)
Movie createDummyMovie(unsigned long movieId,std::string movieName,double meanRating
                       ,double ratingsStandardDeviation,double inverseUserFrequency);

//version with some parameters calculated.
Movie createDummyMovie(unsigned long movieId, std::string movieName, std::vector<std::string> genres, std::vector<TagEntry> totalTags, std::vector<UserEntry> userEntries, std::vector<NeighborMovie> neighbors);

Movie createDummyMoviePreset(short id);
Movie createDummyMovieRandom();

//version where weight is random.
NeighborMovie createDummyNeighborMovie(unsigned long movieId);

//version where weight is non-random and given by user.
NeighborMovie createDummyNeighborMovie(unsigned long movieId, double weight,std::vector<unsigned long> commonUsersIds);

NeighborMovie createDummyNeighborMovieRandom();
NeighborMovie createNeighborMoviePreset(short id);

void populateDummyConfigurationPresetBoolean(Configuration&);
void populateDummyConfigurationPresetNumeric(Configuration&);
void populateDummyConfigurationPresetParsing(Configuration&);
void populateDummyConfigurationPresetParsingPaths(Configuration&);
void populateDummyConfigurationPresetProfiles(Configuration& config);

void populateDummyConfigurationParsingPaths(Configuration& config,string moviesPath,string ratingsPath,string tagsPath,string genometagsPath);

void writeToFile_usrentr(Configuration* config,float** userEntries, size_t rowcount,FILE* file);
void writeToFile_movies(char*** movies, size_t rowcount,FILE* file);
void writeToFile_gnrentr(Configuration* config,char*** genreEntries, size_t rowcount,FILE* file);
void writeToFile_tags(char** tags, size_t rowcount,FILE* file);

#endif

