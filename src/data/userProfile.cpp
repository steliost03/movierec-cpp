#include "recommenderDataHandler.h"
#include "../data/import/importJson.h"
#include "../export/exportToJson.h"
#include "../data/dtos.h"
#include "../logging/recommenderLogger.h"
#include "dataCalculations.h"
#include <algorithm>

using namespace std;
typedef unsigned long  ulong;

bool determineDataAvailable(User& userData){

    bool dataAvailable = true;

    dataAvailable = dataAvailable && (userData.meanRating>0);
    dataAvailable = dataAvailable && (userData.ratingsStandardDeviation>0);
    dataAvailable = dataAvailable && (!userData.neighbors.empty());

    return dataAvailable;
}

void showNonexistentIds(vector<ulong> nonexistentIds,string profileName){
    cout<<"Warning: the below movie ids of the profile:"<<profileName<<" were not found in data!"<<endl;
    for(size_t i=0;i<nonexistentIds.size();i++){
        cout<<nonexistentIds[i];
        if(i+1 < nonexistentIds.size()){
            cout<<",";
        }
    }
    cout<<"\n";
    cout<<"The profile will not have its parameters calculated. They will remain pending."<<endl;
    cout<<"Press Enter to continue"<<endl;
    cin.get();
}

void RecommenderDataHandler::initUserProfiles(bool dataIsAvailable){
    if(!hasConfig){
        throw RecommenderDataHandlerException("Cannot initialize user profiles without a configuration.");
    }

    try{
        RecommenderLogger LOG;
        LOG.init("recommenderDataHandler:initUserProfiles");

        LOG.info("START: initUserProfiles. dataIsAvailable="+to_string(dataIsAvailable));

        if(config->profileStorageType == "FILE"){
            LOG.debug("Will attempt to read file: "+config->profileFilepath+" in order to load user profiles");
            JsonParseResultWrapper userProfileParseResult = parseUserProfilesFromJson(config->profileFilepath.c_str());
            vector<UserProfileStored> parsedUserProfiles = userProfileParseResult.parsedUserProfiles;
            LOG.debug("Parsed "+to_string(parsedUserProfiles.size())+" user profiles.");
            for(auto it = parsedUserProfiles.begin();it!=parsedUserProfiles.end();it++){
                UserProfileFull newFullProfile;
                if(it->isDataBound){
                    LOG.trace("Loading a databound profile.");
                    UserProfileDataBound dataBound;
                    dataBound.profileName = it->profileName;
                    try{
                        if(dataIsAvailable){
                            dataBound.userData = extractUserInfoPointerFromId(it->userId);
                            newFullProfile.setDataBoundProfile(dataBound);
                        } else{
                            newFullProfile.setDataBoundProfileWithDataUnavailable(it->profileName,it->userId);
                        }
                    }catch(UserNotFoundException& e){
                        cout<<"User id referenced in databound profile was not found in data! Will not populate parameters for this profile. profileName="<<it->profileName<<", userId="<<it->userId<<endl;
                        LOG.warn("User id referenced in databound profile was not found in data! Will not populate parameters for this profile. profileName="+it->profileName+", userId="+to_string(it->userId));
                        newFullProfile.setDataBoundProfileWithDataUnavailable(it->profileName,it->userId);
                    }
                } else{
                    LOG.trace("Loading a standalone profile.");
                    UserProfileStandalone standalone;
                    standalone.profileName = it->profileName;
                    standalone.userData = it->userData;
                    bool dataWasAvailable = determineDataAvailable(standalone.userData);
                    bool dataAvailableAfterThisMethod = dataWasAvailable;
                    //data was not available at the time of usrprf creation, but is available now. Will calculate parameters.
                    if(!dataWasAvailable && dataIsAvailable){
                        LOG.debug("Data was not available at the time of user profile creation, but is available now. "
                                  "Will calculate parameters for it.");
                        vector<ulong> nonexistentIds = this->checkAllMovieIdsOfStandaloneProfileIfExistingInData(standalone);
                        if(nonexistentIds.size() > 0){
                            showNonexistentIds(nonexistentIds,standalone.profileName);
                            dataAvailableAfterThisMethod = false;
                        }
                        else{
                            calculateParametersForSingleUser(standalone.userData,standalone.userData.movieEntries,allUsers,config);
                            dataAvailableAfterThisMethod = true;
                        }
                    }
                    newFullProfile.setStandaloneProfile(standalone,dataAvailableAfterThisMethod);
                }
                userProfiles.push_back(newFullProfile);
            }
        }
        LOG.close();
    } catch(RecommenderException& e){
        throw RecommenderDataHandlerException("Error while initializing user profiles.",&e);
    }
}

void RecommenderDataHandler::loadUserProfilesFromFile(string filepath,bool dataIsAvailable){
    try {
        RecommenderLogger LOG;
        LOG.init("recommenderDataHandler:loadUserProfilesFromFile");
        LOG.info("START: loadUserProfilesFromFile. filepath="+filepath+" , dataIsAvailable="+to_string(dataIsAvailable));

        JsonParseResultWrapper userProfileParseResult = parseUserProfilesFromJson(filepath.c_str());
        vector<UserProfileStored> parsedUserProfiles = userProfileParseResult.parsedUserProfiles;
        LOG.debug("Parsed "+to_string(parsedUserProfiles.size())+" user profiles.");

        for(auto it = parsedUserProfiles.begin();it!=parsedUserProfiles.end();it++){
            UserProfileFull newFullProfile;
            if(it->isDataBound){
                UserProfileDataBound dataBound;
                dataBound.profileName = it->profileName;
                try{
                    if(dataIsAvailable){
                        dataBound.userData = extractUserInfoPointerFromId(it->userId);
                        newFullProfile.setDataBoundProfile(dataBound);
                    } else{
                        newFullProfile.setDataBoundProfileWithDataUnavailable(it->profileName,it->userId);
                    }
                }catch(UserNotFoundException& e){
                    cout<<"User id referenced in databound profile was not found in data! Will not populate parameters for this profile. profileName="<<it->profileName<<", userId="<<it->userId<<endl;
                    LOG.warn("User id referenced in databound profile was not found in data! Will not populate parameters for this profile. profileName="+it->profileName+", userId="+to_string(it->userId));
                    newFullProfile.setDataBoundProfileWithDataUnavailable(it->profileName,it->userId);
                }
            } else{
                UserProfileStandalone standalone;
                standalone.profileName = it->profileName;
                standalone.userData = it->userData;
                bool dataWasAvailable = determineDataAvailable(standalone.userData);
                bool dataAvailableAfterThisMethod = dataWasAvailable;
                //data was not available at the time of usrprf creation, but is available now. Will calculate parameters.
                if(!dataWasAvailable && dataIsAvailable){
                    LOG.debug("Data was not available at the time of user profile creation, but is available now. "
                              "Will calculate parameters for it.");
                    vector<ulong> nonexistentIds = this->checkAllMovieIdsOfStandaloneProfileIfExistingInData(standalone);
                    if(nonexistentIds.size() > 0){
                        showNonexistentIds(nonexistentIds,standalone.profileName);
                        dataAvailableAfterThisMethod = false;
                    }
                    else{
                        calculateParametersForSingleUser(standalone.userData,standalone.userData.movieEntries,allUsers,config);
                        dataAvailableAfterThisMethod = true;
                    }
                }
                newFullProfile.setStandaloneProfile(standalone,dataAvailableAfterThisMethod);
            }
            userProfiles.push_back(newFullProfile);
        }
        LOG.close();
    } catch(RecommenderException& e){
        throw RecommenderDataHandlerException("Error while loading user profiles from file.",&e);
    }
}

void RecommenderDataHandler::saveUserProfiles(){
    if(!hasConfig){
        throw RecommenderDataHandlerException("Cannot save user profiles without a configuration.");
    } try{

        RecommenderLogger LOG;
        LOG.init("recommenderDataHandler:saveUserProfiles");

        if(config->profileStorageType == "FILE"){
            LOG.trace("profileStorageType=FILE. Will write user profiles to file.");
            saveUserProfiles(config->profileFilepath,false);
        }

        LOG.close();
    } catch(RecommenderException& e){
        throw RecommenderDataHandlerException("Error while storing user profiles.",&e);
    }
}

void RecommenderDataHandler::saveUserProfiles(string filepath,bool pretty){
    if(!hasConfig){
        throw RecommenderDataHandlerException("Cannot save user profiles without a configuration.");
    } try{

        RecommenderLogger LOG;
        LOG.init("recommenderDataHandler:saveUserProfiles");

        if(config->profileStorageType == "FILE"){
            exportUserProfilesToJson(*this,filepath,pretty);
        }

        LOG.close();
    } catch(RecommenderException& e){
        throw RecommenderDataHandlerException("Error while storing user profiles.",&e);
    }
}

bool RecommenderDataHandler::createNewUserProfileStandalone(string profileName,ulong userId,vector<MovieEntry> movieEntries,bool dataIsAvailable){
    try{
    RecommenderLogger LOG;
    LOG.init("recommenderDataHandler:createNewUserProfileStandalone");
    LOG.info("START: createNewUserProfileStandalone. profileName="+
             profileName+",userId="+to_string(userId)
             +",movieEntryCount="+to_string(movieEntries.size())+",dataIsAvailable="+to_string(dataIsAvailable));
    UserProfileFull newFullProfile;
    UserProfileStandalone newUserProfileStandalone;
    newUserProfileStandalone.profileName = profileName;
    User newUser;
    newUser.userId = userId;
    newUser.movieEntries = movieEntries;
    if(dataIsAvailable){
        LOG.debug("Data is available, will calculate user parameters.");
        vector<ulong> nonexistentIds;
        for(auto it=movieEntries.begin();it!=movieEntries.end();it++){
            if(!this->movieIdExists(it->movieId)){
                nonexistentIds.push_back(it->movieId);
            }
        }
        if(nonexistentIds.size() > 0){
            showNonexistentIds(nonexistentIds,profileName);
            LOG.debug("Movie ids of standalone profile were not found in data! Will only populate userId and movieEntries for the standalone profile");
            newUserProfileStandalone.userData = newUser;
            dataIsAvailable = false;
        }
        else{
            calculateParametersForSingleUser(newUser,movieEntries,allUsers,config);
            newUserProfileStandalone.userData = newUser;
            dataIsAvailable = true;
        }
    }
    //if data is NOT available, only userId and movieEntries will be populated for standalone profile.
    else{
        LOG.debug("Data is NOT available, will only populate userId and movieEntries for the standalone profile.");
        newUserProfileStandalone.userData = newUser;
        dataIsAvailable = false;
    }
    newFullProfile.setStandaloneProfile(newUserProfileStandalone,dataIsAvailable);
    userProfiles.push_back(newFullProfile);
    LOG.info("createNewUserProfileStandalone finished. "
             "standaloneProfileName="+newFullProfile.getProfileName()+" standaloneMovieEntryCount="+to_string(newFullProfile.getStandaloneProfile().userData.movieEntries.size()));
    LOG.close();
    }catch(RecommenderException& e){
        return false;
    }
    return true;
}

bool RecommenderDataHandler::createNewUserProfileDataBound(string profileName,ulong userId,bool dataIsAvailable){
    try{
    RecommenderLogger LOG;
    LOG.init("recommenderDataHandler:createNewUserProfileDataBound");
    LOG.info("START: createNewUserProfileDataBound. profileName="+profileName+", userId="+to_string(userId)
             +" dataIsAvailable="+to_string(dataIsAvailable));
    UserProfileDataBound newUserProfileDataBound;
    newUserProfileDataBound.profileName = profileName;
    UserProfileFull newFullProfile;
    if(dataIsAvailable){
        LOG.debug("Data is available, will bind user profile to existing user data.");
        newUserProfileDataBound.userData = extractUserInfoPointerFromId(userId);
        newFullProfile.setDataBoundProfile(newUserProfileDataBound);
    } else {
        LOG.debug("Data is not available, will wait for data to be loaded in order to bind user profile to a user.");
        newFullProfile.setDataBoundProfileWithDataUnavailable(profileName,userId);
    }
    userProfiles.push_back(newFullProfile);
    LOG.info("createNewUserProfileDataBound finished. databoundProfileName="+newFullProfile.getProfileName()+" databoundUserID="+to_string(newFullProfile.getUserId()));
    LOG.close();
    }catch(RecommenderException& e){
        return false;
    }
    return true;
}

UserProfileFull& RecommenderDataHandler::loadUserProfile(ulong userId){
    auto iter = find_if(userProfiles.begin(),userProfiles.end(),[&userId](UserProfileFull& usrProf){return usrProf.getUserId() == userId;});
    if(iter == userProfiles.end()){
        throw UserProfileNotFoundException("Could not load user profile with userId="+to_string(userId));
    }
    return *iter;
}

bool RecommenderDataHandler::deleteUserProfile(ulong userId){
    auto initialSize = userProfiles.size();
    userProfiles.erase(remove_if(userProfiles.begin(),userProfiles.end(),
                                 [&userId](UserProfileFull& usrProf){return usrProf.getUserId() == userId;}),
                                 userProfiles.end());
    auto afterSize = userProfiles.size();
    if(!(afterSize < initialSize)){
        return false;
    }
    return true;
}

unsigned long RecommenderDataHandler::populateWaitingUserProfiles(){
    unsigned long affectedProfileCount = 0;
    RecommenderLogger LOG;
    LOG.init("recommenderDataHandler:populateWaitingUserProfiles");
    vector<UserProfileFull>& allUserProfiles = getAllUserProfiles();
    for(auto it=allUserProfiles.begin();it!=allUserProfiles.end();it++){
        LOG.trace("Processing userProfile="+it->getProfileName()+" with userId="+to_string(it->getUserId()));
        if(it->isWaitingForData()){
            if(it->isDataBound()){
                UserProfileDataBound dataBound;
                dataBound.profileName = it->getProfileName();
                try{
                    dataBound.userData = extractUserInfoPointerFromId(it->getUserId());
                }catch(UserNotFoundException& e){
                    LOG.warn("userId="+to_string(e.getExceptionUserIdIfExists())+" was not found in data, the parameters of the databound profile:"+it->getProfileName()+" will remain pending.");
                    cout<<"userId="<<e.getExceptionUserIdIfExists()<<" was not found in data, the parameters of the databound profile:"<<it->getProfileName()<<" will remain pending."<<endl;
                    continue;
                }catch(exception& e){
                    LOG.warn("An error occured while updating the parameters of databound profile:"+it->getProfileName()+" . Error:"+e.what());
                    LOG.warn("The parameters of the databound profile will remain pending.");
                    cout<<"An error occured while updating the parameters of databound profile:"<<it->getProfileName()<<" . Error:"<<e.what()<<endl;
                    cout<<"The parameters of the databound profile will remain pending."<<endl;
                    continue;
                }
                it->setDataBoundProfile(dataBound);
                affectedProfileCount++;
            } else{
                //standalone profile waiting for data
                UserProfileStandalone standalone;
                standalone.profileName = it->getProfileName();
                standalone.userData = it->getStandaloneProfile().userData;
                try{
                    vector<ulong> nonexistentMovieIds = this->checkAllMovieIdsOfStandaloneProfileIfExistingInData(standalone);
                    if(nonexistentMovieIds.size() > 0){
                        showNonexistentIds(nonexistentMovieIds,standalone.profileName);
                        it->setStandaloneProfile(standalone,false);
                        continue;
                    }
                    calculateParametersForSingleUser(standalone.userData,standalone.userData.movieEntries,allUsers,config,true);
                }catch(exception& e){
                    LOG.warn("An error occured while updating the parameters of standalone profile:"+it->getProfileName()+" . Error:"+e.what());
                    cout<<"An error occured while updating the parameters of standalone profile:"<<it->getProfileName()<<" . Error:"<<e.what()<<endl;
                    continue;
                }
                it->setStandaloneProfile(standalone,true);
                affectedProfileCount++;
            }
        }
    }
    LOG.close();
    return affectedProfileCount;
}

bool RecommenderDataHandler::deleteALLUserProfiles(){
    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:deleteALLUserProfiles");
    LOG.info("START: deleteALLUserProfiles");
    //using swap 'trick' so that the, so-far, reserved memory is freed.
    vector<UserProfileFull>& allUserProfiles = getAllUserProfiles();
    vector<UserProfileFull>().swap(allUserProfiles);
    LOG.close();
    if(allUserProfiles.size() == 0){
        LOG.info("deleteALLUserProfiles finished. Deletions were successful.");
        return true;
    } else{
        LOG.warn("deleteALLUserProfiles finished. Deletions were NOT successful!");
        return false;
    }
}

ulong RecommenderDataHandler::recalculateProfileParameters(){
    RecommenderLogger LOG;
    LOG.init("RecommenderDataHandler:recalculateProfileParameters");
    LOG.info("START: recalculateProfileParameters");
    ulong affectedProfileCount = 0;
    vector<UserProfileFull>& allUserProfiles = this->getAllUserProfiles();
    for(auto it=allUserProfiles.begin();it!=allUserProfiles.end();it++){
        LOG.trace("Processing userProfile="+it->getProfileName()+" with userId="+to_string(it->getUserId()));
        if(it->isDataBound() && it->isWaitingForData()){ //if it is databound, it needs to be updated only if it is waiting for data.
            UserProfileDataBound dataBound;
            dataBound.profileName = it->getProfileName();
            try{
                dataBound.userData = extractUserInfoPointerFromId(it->getUserId());
            }catch(UserNotFoundException& e){
                LOG.warn("userId="+to_string(e.getExceptionUserIdIfExists())+" was not found in data, the parameters of the databound profile:"+it->getProfileName()+" will remain pending.");
                cout<<"userId="<<e.getExceptionUserIdIfExists()<<" was not found in data, the parameters of the databound profile:"<<it->getProfileName()<<" will remain pending."<<endl;
                continue;
            }catch(exception& e){
                LOG.warn("An error occured while updating the parameters of databound profile:"+it->getProfileName()+" . Error:"+e.what());
                LOG.warn("The parameters of the databound profile will remain pending.");
                cout<<"An error occured while updating the parameters of databound profile:"<<it->getProfileName()<<" . Error:"<<e.what()<<endl;
                cout<<"The parameters of the databound profile will remain pending."<<endl;
                continue;
            }
            it->setDataBoundProfile(dataBound);
            affectedProfileCount++;
        }
        else{ //standalone
            UserProfileStandalone standalone;
            standalone.profileName = it->getProfileName();
            standalone.userData = it->getStandaloneProfile().userData;
            try{
                vector<ulong> nonexistentMovieIds = this->checkAllMovieIdsOfStandaloneProfileIfExistingInData(standalone);
                if(nonexistentMovieIds.size() > 0){
                    showNonexistentIds(nonexistentMovieIds,standalone.profileName);
                    it->setStandaloneProfile(standalone,false);
                    continue;
                }
                calculateParametersForSingleUser(standalone.userData,standalone.userData.movieEntries,allUsers,config,true);
            }catch(exception& e){
                LOG.warn("An error occured while updating the parameters of standalone profile:"+it->getProfileName()+" . Error:"+e.what());
                LOG.warn("The parameters of the standalone profile will become 'pending' (waitingForData=true)");
                cout<<"An error occured while updating the parameters of standalone profile:"<<it->getProfileName()<<" . Error:"<<e.what()<<endl;
                cout<<"The parameters of the standalone profile will become 'pending' (waitingForData=true)"<<endl;
                continue;
            }
            it->setStandaloneProfile(standalone,true);
            affectedProfileCount++;
        }
    }
    LOG.info("recalculateProfileParameters finished.");
    LOG.close();
    return affectedProfileCount;
}

//returns the movie ids that were NOT found in loaded data.
vector<ulong> RecommenderDataHandler::checkAllMovieIdsOfStandaloneProfileIfExistingInData(UserProfileStandalone& standaloneProfile){
    vector<ulong> nonexistentMovieIds;
    vector<MovieEntry> movieEntries = standaloneProfile.userData.movieEntries;
    for(auto it = movieEntries.begin();it!=movieEntries.end();it++){
        if(!this->movieIdExists(it->movieId)){
            nonexistentMovieIds.push_back(it->movieId);
        }
    }
    return nonexistentMovieIds;
}

//for testing purposes.
bool RecommenderDataHandler::createNewUserProfileStandalone(UserProfileFull newProfile){
    size_t sizeBefore = userProfiles.size();
    userProfiles.push_back(newProfile);
    size_t sizeAfter = userProfiles.size();
    return (sizeAfter == sizeBefore + 1);
}

//for testing purposes.
bool RecommenderDataHandler::createNewuserProfileDataBound(UserProfileFull newProfile){
    size_t sizeBefore = userProfiles.size();
    userProfiles.push_back(newProfile);
    size_t sizeAfter = userProfiles.size();
    return (sizeAfter == sizeBefore + 1);
}
