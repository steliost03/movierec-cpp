#include "dtos.h"
#include "../exceptions/recommenderExceptions.h"
#include <algorithm>

string UserProfileFull::getProfileName(){
    return profileName;
}

unsigned long UserProfileFull::getUserId(){
    return userId;
}

bool UserProfileFull::isDataBound(){
    return dataBound;
}

UserProfileDataBound UserProfileFull::getDataBoundProfile(){
    if(!dataBound){
        throw InvalidStateException("User profile is not databound, but databound details were requested.");
    }
    UserProfileDataBound profile;
    profile.profileName = this->profileName;
    profile.userData = this->userDataPtr;
    return profile;
}

UserProfileStandalone UserProfileFull::getStandaloneProfile(){
    if(dataBound){
        throw InvalidStateException("User profile is not standalone, but standalone details were requested.");
    }
    UserProfileStandalone profile;
    profile.profileName = this->profileName;
    profile.userData = this->userData;
    return profile;
}

UserProfileStored UserProfileFull::getStoredProfile(){
    UserProfileStored stored;
    stored.profileName = this->profileName;
    stored.isDataBound = this->dataBound;
    if(!stored.isDataBound){
        stored.userData = this->userData;
    }
    stored.userId = this->userId;
    return stored;
}

bool UserProfileFull::isWaitingForData(){
    return waitingForData;
}

void UserProfileFull::setUserId(unsigned long userId){
    this->userId = userId;
}

void UserProfileFull::setProfileName(string profileName){
    this->profileName = profileName;
}

void UserProfileFull::setStandaloneProfile(UserProfileStandalone profile,bool allDataAvailable){
    this->profileName = profile.profileName;
    this->userData = profile.userData;
    this->userId = profile.userData.userId;
    this->dataBound = false;
    this->waitingForData = !allDataAvailable;
}

void UserProfileFull::setDataBoundProfile(UserProfileDataBound profile){
    this->profileName = profile.profileName;
    this->userDataPtr = profile.userData;
    this->userId = profile.userData->userId;
    this->dataBound = true;
    this->waitingForData = false;
}

void UserProfileFull::setDataBoundProfileWithDataUnavailable(string profileName,unsigned long userId){
    this->profileName = profileName;
    this->userId = userId;
    this->dataBound = true;
    this->waitingForData = true;
}

void UserProfileFull::setWaitingForData(bool waitingForData){
    this->waitingForData = waitingForData;
}

bool UserProfileFull::mvIdExistsInMvEntries(unsigned long movieId){
    if(this->isDataBound()){
        if(this->isWaitingForData()){
            return false;
        } else{
            vector<MovieEntry> movieEntries = this->getDataBoundProfile().userData->movieEntries;
            auto iter = find_if(movieEntries.begin(),movieEntries.end(),[&movieId](MovieEntry& mvEntr){return mvEntr.movieId == movieId;});
            return !(iter == movieEntries.end());
        }
    } else{
        vector<MovieEntry> movieEntries = this->getStandaloneProfile().userData.movieEntries;
        auto iter = find_if(movieEntries.begin(),movieEntries.end(),[&movieId](MovieEntry& mvEntr){return mvEntr.movieId == movieId;});
        return !(iter == movieEntries.end());
    }
}

bool UserProfileFull::removeMovieEntryFromProfile(unsigned long movieId){
    size_t initialSize,afterSize;
    if(this->isDataBound()){

        vector<MovieEntry>& movieEntries = this->getDataBoundProfile().userData->movieEntries;

        initialSize = movieEntries.size();

        movieEntries.erase(remove_if(movieEntries.begin(),movieEntries.end(),
                                     [&movieId](MovieEntry& mvEntr){return mvEntr.movieId == movieId;}));

        afterSize = movieEntries.size();
    } else{

        vector<MovieEntry>& movieEntries = this->userData.movieEntries;

        initialSize = movieEntries.size();

        movieEntries.erase(remove_if(movieEntries.begin(),movieEntries.end(),
                                     [&movieId](MovieEntry& mvEntr){return mvEntr.movieId == movieId;}));

        afterSize = movieEntries.size();
    }
    if(!(afterSize < initialSize)){
        return false;
    }
    return true;
}
