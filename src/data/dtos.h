#ifndef USERINFO_H
#define USERINFO_H
#include <string>
#include <vector>
using namespace std;

//UNIQUE_KEY: element that uniquely identifies the struct/object.

struct TagEntry {
	
    string tagName = ""; //required
    unsigned long tagId = 0; //required
    unsigned int tagCount = 0; //only non-zero in case of Movie.
};

struct MovieEntry {

    unsigned long movieId = 0; //required
    double rating = 0; //required
    vector<TagEntry> tags;
    bool rated = false; //rating is non-zero
    bool tagged = false; //atleast one tag exists.
};

struct UserEntry {

    unsigned long userId = 0;
    double rating;
    vector<TagEntry> tags;
    bool rated = false; //rating is non-zero
    bool tagged = false; //atleast one tag exists.
};

//TODO: join NeighborUser and NeighborMovie into one Neighbor struct?
struct NeighborUser {
	
    unsigned long id = 0; //required
    double weight = 0;
    vector<unsigned long> commonMoviesIds;
};

struct NeighborMovie {
	
    unsigned long id = 0; //required
    double weight = 0;
    vector<unsigned long> commonUsersIds;
};

//user-based method.
struct User {
	
    unsigned long userId = 0; //required , UNIQUE_KEY

    double meanRating = 0;

    double ratingsStandardDeviation = 0;

    //double inverseItemFrequency = 0; :MAYBE USE IN FUTURE.

    vector<MovieEntry> movieEntries;
		
    vector<NeighborUser> neighbors;
	
};

//item-based method.
struct Movie {

    unsigned long movieId = 0; //required , UNIQUE_KEY

    string movieName = ""; //required

    double meanRating = 0;

    double ratingsStandardDeviation = 0;

    //double inverseUserFrequency = 0; :MAYBE USE IN FUTURE.

    vector<string> genres; //required

    vector<TagEntry> totalTags;

    vector<UserEntry> userEntries;

    vector<NeighborMovie> neighbors;

};


struct UserProfileDataBound {
    string profileName;
    User* userData;
};

struct UserProfileStandalone {
    string profileName;
    User userData;
};

//database/file version.
struct UserProfileStored {
    string profileName; //always required.
    bool isDataBound; //always required.
    unsigned long userId; //always required.
    User userData; //required for standalone.

    //databound: contains profileName,isDataBound,userId
    //standalone: contains profileName,isDataBound,userId,userData
};

class UserProfileFull {

    private:
    string profileName; //always
    unsigned long userId; //always
    bool dataBound; //always
    User* userDataPtr; //databound,only with data available.
    bool waitingForData; //always
    User userData; //standalone

    public:
    //getters:
    bool isDataBound();
    //warning:the below return copies!
    UserProfileDataBound getDataBoundProfile();
    UserProfileStandalone getStandaloneProfile();
    UserProfileStored getStoredProfile();
    //the above return copies.
    string getProfileName();
    unsigned long getUserId();
    bool isWaitingForData();
    //setters:
    void setUserId(unsigned long userId);
    void setProfileName(string);
    void setStandaloneProfile(UserProfileStandalone,bool allDataAvailable=true);
    void setDataBoundProfile(UserProfileDataBound);
    void setDataBoundProfileWithDataUnavailable(string profileName,unsigned long userId);
    void setWaitingForData(bool);
    //utils:
    bool mvIdExistsInMvEntries(unsigned long movieId);
    bool removeMovieEntryFromProfile(unsigned long movieId);
};

#endif
