#ifndef CLI_DECLARATIONS_H
#define CLI_DECLARATIONS_H

#include "CLI_DEPENDENCIES.h"

using namespace std;

struct RecommendationParameters {
    unsigned long userID;
    vector<MovieGenre> genres;
    vector<string> tagNames;
    bool standaloneProfileIsUsed;
    User userProfileData; //for standalone profiles only.
    string profileName; //for standalone profiles only.
    bool profilesExist = true;

    //key->what will be filtered. possible values: 'genres' , 'tags'
    //value->filtering type
    unordered_map<string,char> filteringTypes; //for generalized only
                                               //'1'->strict, '2'->lenient , '3'->very lenient
};

struct UserProfileSelection {
    UserProfileFull* profile;
    uint selection_id;
};

struct ShowAllProfilesWrapper {
    uint max_selection_id;
    vector<UserProfileFull>* userProfiles;
    bool profilesExist = true;
};

struct ConfigValueDataTypeWrapper{
    //1->boolean,2->long,3->double,4->string,5->unsigned long
    char dataTypeId;
    bool boolvalue;
    long longvalue;
    double doublevalue;
    unsigned long ulongvalue;
    string strvalue;
};

struct ConfigValue{
    bool boolvalue;
    long longvalue;
    unsigned long ulongvalue;
    double doublevalue;
    string strvalue;
};

//function declarations
void multipleNewlines();
void enterToContinue();
void enterToContinue(string);
bool emptyInput(string);
[[noreturn]] void terminateApplication();
void aboutScreen();
void addMovieMenu();
void addUserMenu();
void addMovieFromFileMenu();
void addMovieManualMenu();
void addUserFromFileMenu();
void addUserManualMenu();
void applicationInit();
void exportDataMenu();
bool exportDataMenuSelectionResolve(char);
void exportToJsonMenu();
void helpScreen();
void importDataMenu();
bool importDataMenuSelectionResolve(char);
void importFromJsonMenu();
void importFromCsvMenu();
[[noreturn]] void mainMenu();
void mainMenuInitial();
void dataInformationScreen();
void parseInit();
void parsePathsInit();
void resolveSelection(char);
void userProfileMenu();
void userProfileSelectionResolve(char);
void createNewProfileMenu();
void resolveCreateNewProfileMenuSelection(char);
void createNewStandaloneProfileMenu();
void createNewDataBoundProfileMenu();
void createNewProfileFromFileMenu();
short profileCreationMethodMenu();

void manageDataMenu();

void deleteSpecificMenu();
void deleteSpecificUserMenu();
void deleteSpecificMovieMenu();

void viewDataMenu();
void viewSpecificUserMenu();
void viewSpecificMovieMenu();

void deleteAllDataMenu();

void settingsMenu();
void generalSettingsMenu();
void parsingSettingsMenu();
void profileSettingsMenu();
void loggingSettingsMenu();

string checkIfHasSignAndRemove(string value_str,bool* hasSign);

void showAndModifyConfigParameterGeneral(int numericSelection);
void showAndModifyConfigParameterParsing(int numericSelection);
void showAndModifyConfigParameterProfile(int numericSelection);
void showAndModifyConfigParameterLogging(int numericSelection);

ConfigValue determinePrintAndModifyConfigValue(string configValueStr);
ConfigValue determinePrintAndModifyConfigValue(string configValueStr,bool forceLong);
//1->boolean,2->long,3->double,4->string
ConfigValueDataTypeWrapper determineConfigValueStrDataType(string configValueStr);
ConfigValueDataTypeWrapper determineConfigValueStrDataType(string configValueStr,bool forceLong);

vector<ulong> parseMovieIds(string);
vector<double> parseRatings(string);
vector<TagEntry> parseTagEntries(string);
UserProfileFull* loadProfileMenu(bool);
bool resolveProfileSelection(vector<UserProfileSelection>&,uint,UserProfileFull**);
void editProfileMenu(UserProfileFull*);
void resolveEditProfileSelection(char,UserProfileFull*);
void editProfileAddMovieEntry(UserProfileFull*);
void editProfileRemoveMovieEntry(UserProfileFull*);
void editProfileShowAllMovieEntries(UserProfileFull*);
void editProfileChangeProfileName(UserProfileFull*);
ShowAllProfilesWrapper showAllProfiles(bool onlyShow=true);
void saveProfiles();
void showProfileDetails(UserProfileFull*);
vector<UserProfileSelection> bindProfilesToSelectionIds(vector<UserProfileFull>*);
void deleteProfileMenu();
void profileHelp();
char recommendationTypeMenu();
void inputGenresMenu(RecommendationParameters&);
void inputTagsMenu(RecommendationParameters&);
void inputFilteringMethodMenu(RecommendationParameters&,string);
void filteringHelpScreen();
vector<string> inputGenresMenu();
vector<TagEntry> inputTagsMenu();
RecommendationParameters recommendationParametersMenu(char);

void recalculateScreen();

void recommendMoviesMenu();
void showRecommendationResults(MovieRecommendationsWrapper&,RecommendationParameters,char,double,bool standaloneProfile=false);
uint showUserBasedRecommendations(vector<MovieRecommendation>&,uint);
uint showItemBasedRecommendations(vector<MovieRecommendation>&,uint);
void showGeneralRecommendations(vector<MovieRecommendation>&);
string showGenres(MovieRecommendation&);
bool resolveMovieRecommendationSelectionPersonalized(uint,vector<MovieRecommendation>&,vector<MovieRecommendation>&);
bool resolveMovieRecommendationSelectionGeneral(uint,vector<MovieRecommendation>&);
void showRecommendationDetails(MovieRecommendation&);
void showRecommendationDetailsUserBased(MovieRecommendation&);
void showRecommendationDetailsItemBased(MovieRecommendation&);
void showRecommendationDetailsGeneral(MovieRecommendation&);
string showMostCommonTags(vector<TagEntry>&);
string showSimilarWatchedMovies(vector<string>&);

void showUserDetails(User&);
void showMovieDetails(Movie&);

void storeParsedElements(JsonParseResultWrapper& parseResult);

string movieEntriesAsStr(vector<MovieEntry>);
string userEntriesAsStr(vector<UserEntry> userEntries);
string neighborsAsStrIdsOnly(vector<NeighborUser>);
string neighborsAsStrIdsOnly(vector<NeighborMovie>);
string neighborsAsStrWeightsOnly(vector<NeighborUser>);
string neighborsAsStrWeightsOnly(vector<NeighborMovie>);
string tagsAsStr(vector<TagEntry> tags);
string booleanAsPrettyStr(bool);

void applyCmdLineOptions();
#endif
