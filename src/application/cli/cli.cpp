#include "CLI_DEPENDENCIES.h"
#include "CLI_DECLARATIONS.h"

static RecommenderDataHandler recommenderDataHandler; //global recommenderDAO object.
static Configuration config; // global configuration object.
static bool dataLoaded = false;

static bool parsingSettingsInitialized = false;
static bool parsingPathSettingsInitialized = false;

void applicationInit(){
	
    srand(time(0));

    applyCmdLineOptions();

    cout<<"INITIALIZING APPLICATION:\n"<<endl;
	//initialize configuration object from configuration file.
    cout<<"Initializing configuration..."<<flush;
    config.initConfig(1);   //general boolean parameters
    config.initConfig(2);   //general arithmetic parameters
    config.initConfig(5);   //profile options
    config.initConfig(6);   //logging options
    recommenderDataHandler.setConfig(&config);
    cout<<"done."<<endl;
    cout<<"Initializing aggregations..."<<flush;
    recommenderDataHandler.initOrUpdateAggr();
    cout<<"done."<<endl;

    //if initOnStartup is enabled, force init.
    if(config.initDataAutomaticallyOnStartup){
        cout<<"initDataOnStartup is enabled. Will attempt to initialize data."<<endl;
        recommenderDataHandler.initData();
        dataLoaded = true;
        cout<<"Data initialized successfully."<<endl;
    }

    //load user profiles, if they exist
    try{
        cout<<"Initializing user profiles. Reading from file="<<config.profileFilepath<<endl;
        recommenderDataHandler.initUserProfiles(dataLoaded);
        cout<<"User profiles initialized successfully."<<endl;
        cout<<"Loaded "<<recommenderDataHandler.getAllUserProfiles().size()<<" user profiles."<<endl;
    }catch(RecommenderDataHandlerException& e){
        bool terminate = reportErrorLight(e,"The application can continue, but user profiles may not be available.");
        if(terminate){
            exit(0);
        }
    }
    cout<<"\nPress Enter to start the application."<<endl;
    cin.get();
}

void applyCmdLineOptions(){
    if(!PARSED_OPTIONS.configpath.empty()){
        cout<<"Overriding default configuration path from cmdline option. new path="+PARSED_OPTIONS.configpath<<endl;
        config.overrideConfigLocation(PARSED_OPTIONS.configpath);
    }
    if(!PARSED_OPTIONS.profilepath.empty()){
        cout<<"Overriding default profile path from cmdline option. new path="+PARSED_OPTIONS.profilepath<<endl;
        config.profileFilepath = PARSED_OPTIONS.profilepath;
        config.profileStorageType = "FILE";
    }
}

[[noreturn]] void terminateApplication(){
    cout<<"Saving user profiles before exiting..."<<endl;
    try{
        recommenderDataHandler.saveUserProfiles();
    }catch(RecommenderException& e){
        cout<<"Could not save user profiles. \nErrorMessage="<<e.getExceptionMessage()<<endl;
    }catch(exception& e){
        cout<<"Could not save user profiles."<<endl;
    }
    cout<<"Done. Press Enter to terminate the application."<<endl;
    cin.get();
    exit(0);
}


void parseInit(){
    if(!parsingSettingsInitialized){
        cout<<"\nInitializing parsing settings...";
        config.initConfig(3);
        parsingSettingsInitialized = true;
        cout<<"done."<<endl;
        enterToContinue();
    }
}

void parsePathsInit(){
    if(!parsingPathSettingsInitialized){
        cout<<"\nInitializing parsing path settings...";
        config.initConfig(4);
        parsingPathSettingsInitialized = true;
        cout<<"done."<<endl;
        enterToContinue();
    }
}

int main(int argc,char* argv[]) {

	try{
        parseCmdLineInputs(argc,argv);
		applicationInit();
        if (!dataLoaded){
			mainMenuInitial();
		}
		else{
			mainMenu();
		}

	}
	catch (RecommenderException& e){
		reportError(e);
        terminateApplication();
	}
	catch (exception& e){
		cout << "An unexpected error has occured: " <<e.what()<<endl;
        terminateApplication();
	}
    terminateApplication();
}

void mainMenuInitial(){

    multipleNewlines();
	cout<<"MOVIE RECOMMENDER"<<endl;
	cout << endl << "Press Enter" << endl;
	cin.get();

    if(config.useDatabase){
        //TODO:implement DB enabled case.
    } else{

        cout << "No data found in memory, proceeding to import menu." << endl;
        cout << "---" << endl;

        importDataMenu();

        //if data was loaded in the meantime, go to the regular main menu.
        if(dataLoaded){
         cout<<"Data was loaded. Will proceed to main menu."<<endl;
         mainMenu();
        }
    }
}

[[noreturn]] void mainMenu() {
	
    multipleNewlines();
	cout<<"MOVIE RECOMMENDER"<<endl;
    //TODO: more messages here?
	
	cout<<endl<<"Press Enter"<<endl;
	cin.get();

	string selection_str;
	char selection;
	
	do{
	
	cout<<"1.Recommend Movies"<<endl;
	cout<<"2.Import Data"<<endl;
	cout<<"3.Export Data"<<endl;
    cout<<"4.Data information"<<endl;
    cout<<"5.Help"<<endl;
    cout<<"6.About"<<endl;
    cout<<"---"<<endl;
    cout<<"a.User Profiles Menu."<<endl;
    cout<<"q.Recalculate user and movie parameters."<<endl;
    cout<<"m.Manage existing data"<<endl;
    cout<<"d.Delete ALL existing data."<<endl;
    cout<<"s.Application Settings"<<endl;
    cout<<"x.Exit"<<endl;
	
	getline(cin,selection_str);
    if(emptyInput(selection_str)){
        continue;
    }
    if(selection_str == "x"){
        break;
    }
	selection = selection_str.at(0);
    cout<<endl;
	
	resolveSelection(selection);
	
    enterToContinue("Press Enter to return to the main menu.");
	
    }while(selection != 'x');

    terminateApplication();
}

void resolveSelection(char selection){
	
	switch(selection){
		
		case '1':
			recommendMoviesMenu();
			break;
		
		case '2':
			importDataMenu();
			break;
			
		case '3':
			exportDataMenu();
			break;

        case '4':
            dataInformationScreen();
            break;

        case '5':
			helpScreen();
			break;
			
        case '6':
			aboutScreen();
			break;

        case 'q':
            recalculateScreen();
            break;

        case 'm':
            manageDataMenu();
            break;

        case 'a':
            userProfileMenu();
            break;

        case 's':
            settingsMenu();
            break;
	}
}

void settingsMenu() {
    multipleNewlines();
    string settings_str;
    char settings_selection = '0';
    bool settingsInputValid = true;
    do{
        settingsInputValid = true;
        cout<<"* APPLICATION SETTINGS *"<<endl;
        cout<<"1.General Settings"<<endl;
        cout<<"2.Parsing Settings"<<endl;
        cout<<"3.Profile Settings"<<endl;
        cout<<"4.Logging Settings"<<endl;
        cout<<"x.Back to previous menu"<<endl;
        getline(cin,settings_str);
        if(emptyInput(settings_str)){
            settingsInputValid = false;
            continue;
        }
        if(settings_str == "x"){
            return;
        }
        settings_selection = settings_str.at(0);
        if(settings_selection != '1' && settings_selection != '2' && settings_selection != '3' && settings_selection != '4'){
            settingsInputValid = false;
        }
    }while(!settingsInputValid);
    if(settings_selection == '1'){
        generalSettingsMenu();
    } else if(settings_selection == '2'){
        parsingSettingsMenu();
    } else if(settings_selection == '3'){
        profileSettingsMenu();
    } else if(settings_selection == '4'){
        loggingSettingsMenu();
    }
}

void generalSettingsMenu(){
    string generalSettingsSelection_str;
    int numericSelection = 0;
    bool generalSettingsSelectionIsValid = true;
    const int max_num_selection = 20;
    do{
        generalSettingsSelectionIsValid = true;
        multipleNewlines();
        cout<<"* GENERAL SETTINGS *"<<endl;
        cout<<"Input the corresponding number of the configuration parameter to modify it."<<endl;
        enterToContinue();
        cout<<"1.initDataAutomaticallyOnStartup="<<boolToAlphaStr(config.initDataAutomaticallyOnStartup)<<endl;
        cout<<"2.useDatabase="<<boolToAlphaStr(config.useDatabase)<<endl;
        cout<<"3.useLimitInWatchedMoviesUserBasedMethod="<<boolToAlphaStr(config.useLimitInWatchedMoviesUserBasedMethod)<<endl;
        cout<<"4.useLimitInWatchedMoviesItemBasedMethod="<<boolToAlphaStr(config.useLimitInWatchedMoviesItemBasedMethod)<<endl;
        cout<<"5.useAdjustedCorrellationForItemSimilarities="<<boolToAlphaStr(config.useAdjustedCorrellationForItemSimilarities)<<endl;
        cout<<"6.useTagSimilarityInItemBasedMethod="<<boolToAlphaStr(config.useTagSimilarityInItemBasedMethod)<<endl;
        cout<<"7.discardNegativeWeightNeighbors="<<boolToAlphaStr(config.discardNegativeWeightNeighbors)<<endl;;
        cout<<"8.maxNeighbors="<<config.maxNeighbors<<endl;
        cout<<"9.neighborWeightThreshold="<<config.neighborWeightThreshold<<endl;
        cout<<"10.recommendationCount="<<config.recommendationCount<<endl;;
        cout<<"11.maxWatchedMovieCountUsedInItemBasedMethod="<<config.maxWatchedMovieCountUsedInItemBasedMethod<<endl;
        cout<<"12.maxWatchedMovieCountUsedInUserBasedMethod="<<config.maxWatchedMovieCountUsedInUserBasedMethod<<endl;
        cout<<"13.significanceWeightingFactor="<<config.significanceWeightingFactor<<endl;
        cout<<"14.stringSimilarityMethod="<<config.stringSimilarityMethod<<endl;
        cout<<"15.similarityThreshold="<<config.similarityThreshold<<endl;
        cout<<"16.minimumCommonElementsForUserNeighbors="<<config.minimumCommonElementsForUserNeighbors<<endl;
        cout<<"17.minimumCommonElementsForMovieNeighbors="<<config.minimumCommonElementsForMovieNeighbors<<endl;
        cout<<"18.terminateApplicationOnCsvImportError="<<boolToAlphaStr(config.terminateApplicationOnCsvImportError)<<endl;
        cout<<"19.terminateApplicationOnJsonImportError="<<boolToAlphaStr(config.terminateApplicationOnJsonImportError)<<endl;
        cout<<"20.minimumRatingCountForAggregations="<<config.minimumRatingCountForAggregations<<endl;
        cout<<"21.writeIntermediateMatricesToFile="<<boolToAlphaStr(config.writeIntermediateMatricesToFile)<<endl;
        cout<<"- - -"<<endl;
        cout<<"x.Back to previous menu"<<endl;
        getline(cin,generalSettingsSelection_str);
        if(emptyInput(generalSettingsSelection_str)){
            generalSettingsSelectionIsValid = false;
            cout<<"Empty input is not allowed, please try again."<<endl;
            enterToContinue();
            continue;
        }
        if(generalSettingsSelection_str.at(0) == 'x'){
            return;
        }
        if(!strIsNumeric(generalSettingsSelection_str)){
            generalSettingsSelectionIsValid = false;
            cout<<"Invalid input, please try again."<<endl;
            enterToContinue();
            continue;
        } else{
           numericSelection = stoi(generalSettingsSelection_str,nullptr,10);
           if(numericSelection > max_num_selection){
               generalSettingsSelectionIsValid = false;
                cout<<"Number does not correspond to valid option, please try again."<<endl;
                enterToContinue();
                continue;
           }
        }
    }
    while(!generalSettingsSelectionIsValid);
    showAndModifyConfigParameterGeneral(numericSelection);
}

void parsingSettingsMenu(){
    string parsingSettingsSelection_str;
    bool parsingSettingsSelectionIsValid = true;
    int numericSelection = 0;
    const int max_numeric_selection = 16;
    do{
        multipleNewlines();
        cout<<"* PARSING SETTINGS *"<<endl;
        if(!parsingSettingsInitialized || !parsingPathSettingsInitialized){
            bool forceinitInputValid = true;
            do{
                forceinitInputValid = true;
                cout<<"The parsing settings have not been initialized. Do you want to initialize them? <y/n>"<<endl;
                string forceinit_str;
                getline(cin,forceinit_str);
                if(emptyInput(forceinit_str)){
                    forceinitInputValid = false;
                    continue;
                }
                if(forceinit_str == "y" || forceinit_str == "Y"){
                    parseInit();
                    parsePathsInit();
                    enterToContinue("Press Enter to continue to parsing settings menu.");
                } else if (forceinit_str == "n" || forceinit_str == "N"){
                    return;
                } else{
                    forceinitInputValid = false;
                    continue;
                }
            }while(!forceinitInputValid);
        }
        cout<<"Input the corresponding number of the configuration parameter to modify it."<<endl;
        string parsingSettingsSelection_str;
        enterToContinue();
        cout<<"1.moviesFileDefaultPath="<<config.moviesFileDefaultPath<<endl;
        cout<<"2.ratingsFileDefaultPath="<<config.ratingsFileDefaultPath<<endl;
        cout<<"3.tagEntriesFileDefaultPath="<<config.tagEntriesFileDefaultPath<<endl;
        cout<<"4.tagIdsFileDefaultPath="<<config.tagIdsFileDefaultPath<<endl;
        cout<<"5.movieNameMaxLength="<<config.movieNameMaxLength<<endl;
        cout<<"6.tagNameMaxLength="<<config.tagNameMaxLength<<endl;
        cout<<"7.genreNameMaxLength="<<config.genreNameMaxLength<<endl;
        cout<<"8.maxGenreCount="<<config.maxGenreCount<<endl;
        cout<<"9.maxTagsPerUser="<<config.maxTagsPerUser<<endl;
        cout<<"10.userEntriesReallocFactor="<<config.userEntriesReallocFactor<<endl;
        cout<<"11.tagsReallocFactor="<<config.tagsReallocFactor<<endl;
        cout<<"12.automaticMultithread="<<config.automaticMultithread<<endl;
        cout<<"13.multithreadParseRatingsEnabled="<<config.multithreadParseRatingsEnabled<<endl;
        cout<<"14.multithreadParseOtherEnabled="<<config.multithreadParseOtherEnabled<<endl;
        cout<<"15.multithreadSearchEnabled="<<config.multithreadSearchEnabled<<endl;
        cout<<"16.defaultThreadcount="<<config.defaultThreadcount<<endl;
        cout<<"- - -"<<endl;
        cout<<"x.Back to previous menu"<<endl;
        getline(cin,parsingSettingsSelection_str);
        if(emptyInput(parsingSettingsSelection_str)){
            parsingSettingsSelectionIsValid = false;
            cout<<"Empty input is not allowed, please try again."<<endl;
            enterToContinue();
            continue;
        }
        if(parsingSettingsSelection_str.at(0) == 'x'){
            return;
        }
        if(!strIsNumeric(parsingSettingsSelection_str)){
            parsingSettingsSelectionIsValid = false;
            cout<<"Invalid input, please try again."<<endl;
            enterToContinue();
            continue;
        } else{
           numericSelection = stoi(parsingSettingsSelection_str,nullptr,10);
           if(numericSelection > max_numeric_selection){
               parsingSettingsSelectionIsValid = false;
                cout<<"Number does not correspond to valid option, please try again."<<endl;
                enterToContinue();
                continue;
           }
        }
    }
    while(!parsingSettingsSelectionIsValid);
    showAndModifyConfigParameterParsing(numericSelection);
}

void profileSettingsMenu(){
    string profileSettingsSelection_str;
    bool profileSettingsSelectionIsValid = true;
    int numericSelection = 0;
    const int max_numeric_selection = 2;
    do{
        multipleNewlines();
        cout<<"* PROFILE SETTINGS *"<<endl;
        cout<<"Input the corresponding number of the configuration parameter to modify it."<<endl;
        string profileSettingsSelection_str;
        enterToContinue();
        cout<<"1.profileFilepath="<<config.profileFilepath<<endl;
        cout<<"2.profileStorageType="<<config.profileStorageType<<endl;
        cout<<"- - -"<<endl;
        cout<<"x.Back to previous menu"<<endl;
        getline(cin,profileSettingsSelection_str);
        if(emptyInput(profileSettingsSelection_str)){
            profileSettingsSelectionIsValid = false;
            cout<<"Empty input is not allowed, please try again."<<endl;
            enterToContinue();
            continue;
        }
        if(profileSettingsSelection_str.at(0) == 'x'){
            return;
        }
        if(!strIsNumeric(profileSettingsSelection_str)){
            profileSettingsSelectionIsValid = false;
            cout<<"Invalid input, please try again."<<endl;
            enterToContinue();
            continue;
        } else{
           numericSelection = stoi(profileSettingsSelection_str,nullptr,10);
           if(numericSelection > max_numeric_selection){
               profileSettingsSelectionIsValid = false;
                cout<<"Number does not correspond to valid option, please try again."<<endl;
                enterToContinue();
                continue;
           }
        }
    }
    while(!profileSettingsSelectionIsValid);
    showAndModifyConfigParameterProfile(numericSelection);
}

void loggingSettingsMenu(){
    string loggingSettingsSelection_str;
    long profileSettingsSelectionIsValid = true;
    int numericSelection = 0;
    const int max_numeric_selection = 3;
    do{
        profileSettingsSelectionIsValid = true;
        multipleNewlines();
        cout<<"* LOGGING SETTINGS *"<<endl;
        cout<<"Input the corresponding number of the "
              "configuration parameter to modify it."<<endl;

        enterToContinue();
        cout<<"1.recommendationLogLevel="<<config.recommendationLogLevel<<endl;
        cout<<"2.parsingLogLevel="<<config.parsingLogLevel<<endl;
        cout<<"3.dataProceduresLogLevel="<<config.dataProceduresLogLevel<<endl;
        cout<<"- - -"<<endl;
        cout<<"x.Back to previous menu"<<endl;
        getline(cin,loggingSettingsSelection_str);

        if(loggingSettingsSelection_str == "x"){
            return;
        }
        if(emptyInput(loggingSettingsSelection_str)){
            profileSettingsSelectionIsValid = false;
            cout<<"Empty input is not allowed, please try again."<<endl;
            enterToContinue();
            continue;
        }
        if(!strIsNumeric(loggingSettingsSelection_str)){
            profileSettingsSelectionIsValid = false;
            cout<<"Invalid input, please try again."<<endl;
            enterToContinue();
            continue;
        } else{
            numericSelection = stoi(loggingSettingsSelection_str,nullptr,10);
            if(numericSelection > max_numeric_selection){
                profileSettingsSelectionIsValid = false;
                 cout<<"Number does not correspond to valid option, please try again."<<endl;
                 enterToContinue();
                 continue;
            }
        }
    }while(!profileSettingsSelectionIsValid);
    showAndModifyConfigParameterLogging(numericSelection);
}

void showAndModifyConfigParameterGeneral(int numericSelection){
    ConfigValue old_value;
    ConfigValue new_value;
    switch(numericSelection){
        case 1:
            cout<<"initDataAutomaticallyOnStartup=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.initDataAutomaticallyOnStartup));
            old_value.boolvalue = config.initDataAutomaticallyOnStartup;
            config.initDataAutomaticallyOnStartup = new_value.boolvalue;
            cout<<"Changed initDataAutomaticallyOnStartup from "<<boolToAlphaStr(old_value.boolvalue)<<" to "<<boolToAlphaStr(config.initDataAutomaticallyOnStartup)<<endl;
            enterToContinue();
            break;
        case 2:
            cout<<"useDatabase=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.useDatabase));
            old_value.boolvalue = config.useDatabase;
            config.useDatabase = new_value.boolvalue;
            cout<<"Changed useDatabase from "<<boolToAlphaStr(old_value.boolvalue)<<" to "<<boolToAlphaStr(config.useDatabase)<<endl;
            enterToContinue();
            break;
        case 3:
            cout<<"useLimitInWatchedMoviesUserBasedMethod=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.useLimitInWatchedMoviesUserBasedMethod));
            old_value.boolvalue = config.useLimitInWatchedMoviesUserBasedMethod;
            config.useLimitInWatchedMoviesUserBasedMethod = new_value.boolvalue;
            cout<<"Changed useLimitInWatchedMoviesUserBasedMethod from "<<boolToAlphaStr(old_value.boolvalue)<<" to "<<boolToAlphaStr(config.useLimitInWatchedMoviesUserBasedMethod)<<endl;
            enterToContinue();
            break;
        case 4:
            cout<<"useLimitInWatchedMoviesItemBasedMethod=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.useLimitInWatchedMoviesItemBasedMethod));
            old_value.boolvalue = config.useLimitInWatchedMoviesItemBasedMethod;
            config.useLimitInWatchedMoviesItemBasedMethod = new_value.boolvalue;
            cout<<"Changed useLimitInWatchedMoviesItemBasedMethod from "<<boolToAlphaStr(old_value.boolvalue)<<" to "<<boolToAlphaStr(config.useLimitInWatchedMoviesItemBasedMethod)<<endl;
            enterToContinue();
            break;
        case 5:
            cout<<"useAdjustedCorrellationForItemSimilarities=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.useAdjustedCorrellationForItemSimilarities));
            old_value.boolvalue = config.useAdjustedCorrellationForItemSimilarities;
            config.useAdjustedCorrellationForItemSimilarities = new_value.boolvalue;
            cout<<"Changed useAdjustedCorrellationForItemSimilarities from "<<boolToAlphaStr(old_value.boolvalue)<<" to "<<boolToAlphaStr(config.useAdjustedCorrellationForItemSimilarities)<<endl;
            enterToContinue();
            break;
        case 6:
            cout<<"useTagSimilarityInItemBasedMethod=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.useTagSimilarityInItemBasedMethod));
            old_value.boolvalue = config.useTagSimilarityInItemBasedMethod;
            config.useTagSimilarityInItemBasedMethod = new_value.boolvalue;
            cout<<"Changed useTagSimilarityInItemBasedMethod from "<<boolToAlphaStr(old_value.boolvalue)<<" to "<<config.useTagSimilarityInItemBasedMethod<<endl;
            enterToContinue();
            break;
        case 7:
            cout<<"discardNegativeWeightNeighbors=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.discardNegativeWeightNeighbors));
            old_value.boolvalue = config.discardNegativeWeightNeighbors;
            config.discardNegativeWeightNeighbors = new_value.boolvalue;
            cout<<"Changed discardNegativeWeightNeighbors from "<<boolToAlphaStr(old_value.boolvalue)<<" to "<<config.discardNegativeWeightNeighbors<<endl;
            enterToContinue();
            break;
        case 8:
            cout<<"maxNeighbors=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.maxNeighbors));
            old_value.ulongvalue = config.maxNeighbors;
            config.maxNeighbors = new_value.ulongvalue;
            cout<<"Changed maxNeighbors from "<<old_value.ulongvalue<<" to "<<config.maxNeighbors<<endl;
            enterToContinue();
            break;
        case 9:
            cout<<"neighborWeightThreshold=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.neighborWeightThreshold));
            old_value.doublevalue = config.neighborWeightThreshold;
            config.neighborWeightThreshold = new_value.doublevalue;
            cout<<"Changed neighborWeightThreshold from "<<old_value.doublevalue<<" to "<<config.neighborWeightThreshold<<endl;
            enterToContinue();
            break;
        case 10:
            cout<<"recommendationCount=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.recommendationCount));
            old_value.ulongvalue = config.recommendationCount;
            config.recommendationCount = new_value.ulongvalue;
            cout<<"Changed recommendationCount from "<<old_value.ulongvalue<<" to "<<config.recommendationCount<<endl;
            enterToContinue();
            break;
        case 11:
            cout<<"maxWatchedMovieCountUsedInItemBasedMethod=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.maxWatchedMovieCountUsedInItemBasedMethod));
            old_value.ulongvalue = config.maxWatchedMovieCountUsedInItemBasedMethod;
            config.maxWatchedMovieCountUsedInItemBasedMethod = new_value.ulongvalue;
            cout<<"Changed maxWatchedMovieCountUsedInItemBasedMethod from "<<old_value.ulongvalue<<" to "<<config.maxWatchedMovieCountUsedInItemBasedMethod<<endl;
            enterToContinue();
            break;
        case 12:
            cout<<"maxWatchedMovieCountUsedInUserBasedMethod=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.maxWatchedMovieCountUsedInUserBasedMethod));
            old_value.ulongvalue = config.maxWatchedMovieCountUsedInUserBasedMethod;
            config.maxWatchedMovieCountUsedInUserBasedMethod = new_value.ulongvalue;
            cout<<"Changed maxWatchedMovieCountUsedInUserBasedMethod from "<<old_value.ulongvalue<<" to "<<config.maxWatchedMovieCountUsedInUserBasedMethod<<endl;
            enterToContinue();
            break;
        case 13:
            cout<<"significanceWeightingFactor=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.significanceWeightingFactor));
            old_value.ulongvalue = config.significanceWeightingFactor;
            config.significanceWeightingFactor = new_value.ulongvalue;
            cout<<"Changed significanceWeightingFactor from "<<old_value.ulongvalue<<" to "<<config.significanceWeightingFactor<<endl;
            enterToContinue();
            break;
        case 14:
            cout<<"stringSimilarityMethod=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.stringSimilarityMethod));
            old_value.ulongvalue = config.stringSimilarityMethod;
            config.stringSimilarityMethod = new_value.ulongvalue;
            cout<<"Changed stringSimilarityMethod from "<<old_value.ulongvalue<<" to "<<config.stringSimilarityMethod<<endl;
            enterToContinue();
            break;
        case 15:
            cout<<"similarityThreshold=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.similarityThreshold));
            old_value.doublevalue = config.similarityThreshold;
            config.similarityThreshold = new_value.doublevalue;
            cout<<"Changed similarityThreshold from "<<old_value.doublevalue<<" to "<<config.similarityThreshold<<endl;
            enterToContinue();
            break;
        case 16:
            cout<<"minimumCommonElementsForUserNeighbors=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.minimumCommonElementsForUserNeighbors));
            old_value.ulongvalue = config.minimumCommonElementsForUserNeighbors;
            config.minimumCommonElementsForUserNeighbors = new_value.ulongvalue;
            cout<<"Changed minimumCommonElementsForUserNeighbors from "<<old_value.ulongvalue<<" to "<<config.minimumCommonElementsForUserNeighbors<<endl;
            enterToContinue();
            break;
        case 17:
            cout<<"minimumCommonElementsForMovieNeighbors=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.minimumCommonElementsForMovieNeighbors));
            old_value.ulongvalue = config.minimumCommonElementsForMovieNeighbors;
            config.minimumCommonElementsForMovieNeighbors = new_value.ulongvalue;
            cout<<"Changed minimumCommonElementsForMovieNeighbors from "<<old_value.ulongvalue<<" to "<<config.minimumCommonElementsForMovieNeighbors<<endl;
            enterToContinue();
            break;
        case 18:
            cout<<"terminateApplicationOnCsvImportError=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.terminateApplicationOnCsvImportError));
            old_value.boolvalue = config.terminateApplicationOnCsvImportError;
            config.terminateApplicationOnCsvImportError = new_value.boolvalue;
            cout<<"Changed terminateApplicationOnCsvImportError from "<<old_value.boolvalue<<" to "<<config.terminateApplicationOnCsvImportError<<endl;
            enterToContinue();
            break;
        case 19:
            cout<<"terminateApplicationOnJsonImportError=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.terminateApplicationOnJsonImportError));
            old_value.boolvalue = config.terminateApplicationOnJsonImportError;
            config.terminateApplicationOnJsonImportError = new_value.boolvalue;
            cout<<"Changed terminateApplicationOnCsvImportError from "<<old_value.boolvalue<<" to "<<config.terminateApplicationOnCsvImportError<<endl;
            enterToContinue();
            break;
        case 20:
            cout<<"minimumRatingCountForAggregations=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.minimumRatingCountForAggregations));
            old_value.ulongvalue = config.minimumRatingCountForAggregations;
            config.minimumRatingCountForAggregations = new_value.ulongvalue;
            cout<<"Changed minimumRatingCountForAggregations from "<<old_value.ulongvalue<<" to "<<config.minimumRatingCountForAggregations<<endl;
            enterToContinue();
            break;
        case 21:
            cout<<"writeIntermediateMatricesToFile=";
            new_value = determinePrintAndModifyConfigValue(boolToAlphaStr(config.writeIntermediateMatricesToFile));
            old_value.boolvalue = config.writeIntermediateMatricesToFile;
            config.writeIntermediateMatricesToFile = new_value.boolvalue;
            cout<<"Changed writeIntermediateMatricesToFile from "<<old_value.boolvalue<<" to "<<config.writeIntermediateMatricesToFile<<endl;
            enterToContinue();
            break;
    }
}

void showAndModifyConfigParameterParsing(int numericSelection){
    ConfigValue old_value;
    ConfigValue new_value;
    switch(numericSelection){
        case 1:
            cout<<"moviesFileDefaultPath=";
            new_value = determinePrintAndModifyConfigValue(config.moviesFileDefaultPath);
            old_value.strvalue = config.moviesFileDefaultPath;
            config.moviesFileDefaultPath = new_value.strvalue;
            cout<<"Changed moviesFileDefaultPath from "<<old_value.strvalue<<" to "<<config.moviesFileDefaultPath<<endl;
            enterToContinue();
            break;
        case 2:
            cout<<"ratingsFileDefaultPath=";
            new_value = determinePrintAndModifyConfigValue(config.ratingsFileDefaultPath);
            old_value.strvalue = config.ratingsFileDefaultPath;
            config.ratingsFileDefaultPath = new_value.strvalue;
            cout<<"Changed ratingsFileDefaultPath from "<<old_value.strvalue<<" to "<<config.ratingsFileDefaultPath<<endl;
            enterToContinue();
            break;
        case 3:
            cout<<"tagEntriesFileDefaultPath=";
            new_value = determinePrintAndModifyConfigValue(config.tagEntriesFileDefaultPath);
            old_value.strvalue = config.tagEntriesFileDefaultPath;
            config.tagEntriesFileDefaultPath = new_value.strvalue;
            cout<<"Changed tagEntriesFileDefaultPath from "<<old_value.strvalue<<" to "<<config.tagEntriesFileDefaultPath<<endl;
            enterToContinue();
            break;
        case 4:
            cout<<"tagIdsFileDefaultPath=";
            new_value = determinePrintAndModifyConfigValue(config.tagIdsFileDefaultPath);
            old_value.strvalue = config.tagIdsFileDefaultPath;
            config.tagIdsFileDefaultPath = new_value.strvalue;
            cout<<"Changed tagIdsFileDefaultPath from "<<old_value.strvalue<<" to "<<config.tagIdsFileDefaultPath<<endl;
            enterToContinue();
            break;
        case 5:
            cout<<"movieNameMaxLength=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.movieNameMaxLength));
            old_value.ulongvalue = config.movieNameMaxLength;
            config.tagNameMaxLength = new_value.ulongvalue;
            cout<<"Changed movieNameMaxLength from "<<old_value.ulongvalue<<" to "<<config.movieNameMaxLength<<endl;
            enterToContinue();
            break;
        case 6:
            cout<<"tagNameMaxLength=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.tagNameMaxLength));
            old_value.ulongvalue = config.tagNameMaxLength;
            config.tagNameMaxLength = new_value.ulongvalue;
            cout<<"Changed tagNameMaxLength from "<<old_value.ulongvalue<<" to "<<config.tagNameMaxLength<<endl;
            enterToContinue();
            break;
        case 7:
            cout<<"genreNameMaxLength=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.genreNameMaxLength));
            old_value.ulongvalue = config.genreNameMaxLength;
            config.genreNameMaxLength = new_value.ulongvalue;
            cout<<"Changed genreNameMaxLength from "<<old_value.ulongvalue<<" to "<<config.genreNameMaxLength<<endl;
            enterToContinue();
            break;
        case 8:
            cout<<"maxGenreCount=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.maxGenreCount));
            old_value.ulongvalue = config.maxGenreCount;
            config.maxGenreCount = new_value.ulongvalue;
            cout<<"Changed maxGenreCount from "<<old_value.ulongvalue<<" to "<<config.maxGenreCount<<endl;
            enterToContinue();
            break;
        case 9:
            cout<<"maxTagsPerUser=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.maxTagsPerUser));
            old_value.ulongvalue = config.maxTagsPerUser;
            config.maxTagsPerUser = new_value.ulongvalue;
            cout<<"Changed maxTagsPerUser from "<<old_value.ulongvalue<<" to "<<config.maxTagsPerUser<<endl;
            enterToContinue();
            break;
        case 10:
            cout<<"userEntriesReallocFactor=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.userEntriesReallocFactor));
            old_value.doublevalue = config.userEntriesReallocFactor;
            config.userEntriesReallocFactor = new_value.doublevalue;
            cout<<"Changed userEntriesReallocFactor from "<<old_value.doublevalue<<" to "<<config.userEntriesReallocFactor<<endl;
            enterToContinue();
            break;
        case 11:
            cout<<"tagsReallocFactor=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.tagsReallocFactor));
            old_value.doublevalue = config.tagsReallocFactor;
            config.tagsReallocFactor = new_value.doublevalue;
            cout<<"Changed tagsReallocFactor from "<<old_value.doublevalue<<" to "<<config.tagsReallocFactor<<endl;
            enterToContinue();
            break;
        case 12:
            cout<<"automaticMultithread=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.automaticMultithread),true);
            old_value.longvalue = config.automaticMultithread;
            config.automaticMultithread = new_value.longvalue;
            cout<<"Changed automaticMultithread from "<<old_value.longvalue<<" to "<<config.automaticMultithread<<endl;
            enterToContinue();
            break;
        case 13:
            cout<<"multithreadParseRatingsEnabled=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.multithreadParseRatingsEnabled),true);
            old_value.longvalue = config.multithreadParseRatingsEnabled;
            config.multithreadParseRatingsEnabled = new_value.longvalue;
            cout<<"Changed multithreadParseRatingsEnabled from "<<old_value.longvalue<<" to "<<config.multithreadParseRatingsEnabled<<endl;
            enterToContinue();
            break;
        case 14:
            cout<<"multithreadParseOtherEnabled=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.multithreadParseOtherEnabled),true);
            old_value.longvalue = config.multithreadParseOtherEnabled;
            config.multithreadParseOtherEnabled = new_value.longvalue;
            cout<<"Changed multithreadParseOtherEnabled from "<<old_value.longvalue<<" to "<<config.multithreadParseOtherEnabled<<endl;
            break;
        case 15:
            cout<<"multithreadSearchEnabled=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.multithreadSearchEnabled),true);
            old_value.longvalue = config.multithreadSearchEnabled;
            config.multithreadSearchEnabled = new_value.longvalue;
            cout<<"Changed multithreadSearchEnabled from "<<old_value.longvalue<<" to "<<config.multithreadSearchEnabled<<endl;
            enterToContinue();
            break;
        case 16:
            cout<<"defaultThreadcount=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.defaultThreadcount));
            old_value.ulongvalue = config.defaultThreadcount;
            config.defaultThreadcount = new_value.ulongvalue;
            cout<<"Changed defaultThreadcount from "<<old_value.ulongvalue<<" to "<<config.defaultThreadcount<<endl;
            enterToContinue();
            break;
    }
}

void showAndModifyConfigParameterProfile(int numericSelection){
    ConfigValue old_value;
    ConfigValue new_value;
    switch(numericSelection){
        case 1:
            cout<<"profileFilepath=";
            new_value = determinePrintAndModifyConfigValue(config.profileFilepath);
            old_value.strvalue = config.profileFilepath;
            config.profileFilepath = new_value.strvalue;
            cout<<"Changed profileFilepath from "<<old_value.strvalue<<" to "<<config.profileFilepath<<endl;
            enterToContinue();
            break;
        case 2:
            cout<<"profileStorageType=";
            new_value = determinePrintAndModifyConfigValue(config.profileStorageType);
            old_value.strvalue = config.profileStorageType;
            config.profileStorageType = new_value.strvalue;
            cout<<"Changed profileStorageType from "<<old_value.strvalue<<" to "<<config.profileStorageType<<endl;
            enterToContinue();
            break;
    }
}

void showAndModifyConfigParameterLogging(int numericSelection){
    ConfigValue old_value;
    ConfigValue new_value;
    switch(numericSelection){
        case 1:
            cout<<"recommendationLogLevel=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.recommendationLogLevel),true);
            old_value.longvalue = config.recommendationLogLevel;
            config.recommendationLogLevel = new_value.longvalue;
            cout<<"Changed recommendationLogLevel from "<<old_value.longvalue<<" to "<<config.recommendationLogLevel<<endl;
            enterToContinue();
            break;
        case 2:
            cout<<"parsingLogLevel=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.parsingLogLevel),true);
            old_value.longvalue = config.parsingLogLevel;
            config.parsingLogLevel = new_value.longvalue;
            cout<<"Changed parsingLogLevel from "<<old_value.longvalue<<" to "<<config.parsingLogLevel<<endl;
            enterToContinue();
            break;
        case 3:
            cout<<"dataProceduresLogLevel=";
            new_value = determinePrintAndModifyConfigValue(to_string(config.dataProceduresLogLevel),true);
            old_value.longvalue = config.dataProceduresLogLevel;
            config.dataProceduresLogLevel = new_value.longvalue;
            cout<<"Changed dataProceduresLogLevel from "<<old_value.longvalue<<" to "<<config.dataProceduresLogLevel<<endl;
            enterToContinue();
            break;
    }
}

ConfigValue determinePrintAndModifyConfigValue(string configValueStr){
    return determinePrintAndModifyConfigValue(configValueStr,false);
}

//1->boolean,2->long,3->double,4->string,5->ulong
ConfigValue determinePrintAndModifyConfigValue(string configValueStr,bool forceLong){

    ConfigValueDataTypeWrapper dataTypeResult =
            determineConfigValueStrDataType(configValueStr,forceLong);

    ConfigValue resultToReturn;

    if(dataTypeResult.dataTypeId == '1'){
        if(dataTypeResult.boolvalue){
            cout<<"true"<<endl;
        } else{
            cout<<"false"<<endl;
        }
        string inputBoolValue_str;
        bool inputBoolValue;
        bool inputBoolValueIsValid = true;
        do{
            inputBoolValueIsValid = true;
            cout<<"New Value (boolean):";
            getline(cin,inputBoolValue_str);
            if(emptyInput(inputBoolValue_str)){
                inputBoolValueIsValid = false;
                continue;
            }
            inputBoolValue_str = strToLower(inputBoolValue_str);
            if(inputBoolValue_str == "true"){
                inputBoolValue = true;
                resultToReturn.boolvalue = inputBoolValue;
                return resultToReturn;
            } else if(inputBoolValue_str == "false"){
                inputBoolValue = false;
                resultToReturn.boolvalue = inputBoolValue;
                return resultToReturn;
            } else{
                inputBoolValueIsValid = false;
                cout<<"Invalid input: expecting boolean value('true' or 'false'). Please try again."<<endl;
                enterToContinue();
                continue;
            }
        }while(!inputBoolValueIsValid);
    } else if(dataTypeResult.dataTypeId == '2'){
        cout<<dataTypeResult.longvalue<<endl;
        string inputLongValue_str;
        long inputLongValue;
        bool inputLongValueIsValid = true;
        do{
            inputLongValueIsValid = true;
            cout<<"New Value (integer):";
            getline(cin,inputLongValue_str);
            if(emptyInput(inputLongValue_str)){
                inputLongValueIsValid = false;
                continue;
            }
            bool numberIsNegative = false;
            //process the sign,if it exists.
            inputLongValue_str = checkIfHasSignAndRemove(inputLongValue_str,&numberIsNegative);
            if(!strIsNumeric(inputLongValue_str)){
                inputLongValueIsValid = false;
                cout<<"Invalid input: expecting integer value (positive or negative). Please try again."<<endl;
                enterToContinue();
                continue;
            }
            inputLongValue = strToPositiveLong(inputLongValue_str);
            resultToReturn.longvalue = inputLongValue;
            return resultToReturn;
        }while(!inputLongValueIsValid);
    } else if(dataTypeResult.dataTypeId == '3'){
        cout<<dataTypeResult.doublevalue<<endl;
        string inputDoubleValue_str;
        double inputDoubleValue;
        bool inputDoubleValueIsValid = true;
        do{
            inputDoubleValueIsValid = true;
            cout<<"New Value (real number):";
            getline(cin,inputDoubleValue_str);
            if(emptyInput(inputDoubleValue_str)){
                inputDoubleValueIsValid = false;
                continue;
            }
            inputDoubleValue = strToDouble(inputDoubleValue_str);
            resultToReturn.doublevalue = inputDoubleValue;
            return resultToReturn;
        }while(!inputDoubleValueIsValid);
    } else if(dataTypeResult.dataTypeId == '4'){
        cout<<dataTypeResult.strvalue<<endl;
        string inputStrValue;
        bool inputStrValueIsValid = true;
        do{
            inputStrValueIsValid = true;
            cout<<"New Value (text):";
            getline(cin,inputStrValue);
            if(emptyInput(inputStrValue)){
                inputStrValueIsValid = false;
                continue;
            }
            resultToReturn.strvalue = inputStrValue;
            return resultToReturn;
        }while(!inputStrValueIsValid);
    } else if(dataTypeResult.dataTypeId == '5'){
        cout<<dataTypeResult.ulongvalue<<endl;
        string inputULongValue_str;
        ulong inputULongValue;
        bool inputULongValueIsValid = true;
        do{
            inputULongValueIsValid = true;
            cout<<"New Value (positive integer):";
            getline(cin,inputULongValue_str);
            if(emptyInput(inputULongValue_str)){
                inputULongValueIsValid = false;
                continue;
            }
            if(!strIsNumeric(inputULongValue_str)){
                inputULongValueIsValid = false;
                cout<<"Invalid input: expecting strictly positive integer value. Please try again."<<endl;
                enterToContinue();
                continue;
            }
            inputULongValue = strToUlong(inputULongValue_str);
            resultToReturn.ulongvalue = inputULongValue;
            return resultToReturn;
        }while(!inputULongValueIsValid);
    }
    return resultToReturn;
}

ConfigValueDataTypeWrapper determineConfigValueStrDataType(string configValueStr){
    return determineConfigValueStrDataType(configValueStr,false);
}

//1->boolean,2->long,3->double,4->string
ConfigValueDataTypeWrapper determineConfigValueStrDataType(string configValueStr,bool forceLong){
    ConfigValueDataTypeWrapper result;
    if(configValueStr=="true"){ //boolean
        result.dataTypeId = '1';
        result.boolvalue = true;
    }
    else if(configValueStr=="false"){ //boolean
        result.dataTypeId = '1';
        result.boolvalue = false;
    }
    else if(strIsNumeric(configValueStr) && !forceLong){ //ulong
        result.dataTypeId = '5';
        result.ulongvalue = strtoul(configValueStr.c_str(),nullptr,10);
    }
    else if((strtod(configValueStr.c_str(),nullptr) != 0.0) && !strIsNumeric(configValueStr)) { //double
        result.dataTypeId = '3';
        result.doublevalue = strtod(configValueStr.c_str(),nullptr);
    }
    else if(strIsNumeric(configValueStr) || strIsNegativeNumber(configValueStr)){ //long
        result.dataTypeId = '2';
        result.longvalue = strtol(configValueStr.c_str(),nullptr,10);
    }
    else{ //string
        result.dataTypeId = '4';
        result.strvalue = configValueStr;
    }
    return result;
}

void dataInformationScreen(){
        //TODO:missing info.
        multipleNewlines();
        cout<<"* DATA INFORMATION *"<<endl;
        cout<<"User count:"<<recommenderDataHandler.getAllUsers().size()<<endl;
        cout<<"Movie count:"<<recommenderDataHandler.getAllMovies().size()<<endl;
        cout<<"--"<<endl;
        cout<<"Rating count:"<<recommenderDataHandler.getRatingCount()<<endl;
        cout<<"Tag entry count:"<<recommenderDataHandler.getTotalTagCount()<<endl;
        cout<<"Unique tag count:"<<recommenderDataHandler.getUniqueTagCount()<<endl;
        cout<<"--"<<endl;
        Movie& highestRatedMovie = recommenderDataHandler.aggregation().highestRatedMovie(config.minimumRatingCountForAggregations);
        Movie& mostWatchedMovie = recommenderDataHandler.aggregation().mostWatchedMovie();
        Movie& highestDeviationMovie = recommenderDataHandler.aggregation().highestDeviationMovie();
        MostTaggedMovieResult mostTaggedMovie = recommenderDataHandler.aggregation().mostTaggedMovie();
        MostUsedTagResult mostUsedTag = recommenderDataHandler.aggregation().mostUsedTag();
        MostCommonGenreResult mostCommonGenre = recommenderDataHandler.aggregation().mostCommonGenre();
        HighestRatedGenreResult highestRatedGenre = recommenderDataHandler.aggregation().highestRatedGenre();
        cout<<"Highest rated Movie: "<<highestRatedMovie.movieName<<" (Mean Rating:"<<highestRatedMovie.meanRating<<")"<<endl;
        cout<<"Movie with highest rating deviation: "<<highestDeviationMovie.movieName<<" (Deviation:"<<highestDeviationMovie.ratingsStandardDeviation<<")"<<endl;
        cout<<"Most watched movie: "<<mostWatchedMovie.movieName<<" (Watch Count:"<<mostWatchedMovie.userEntries.size()<<")"<<endl;
        cout<<"Most tagged movie: "<<mostTaggedMovie.movieName<<" (Tag entry Count:"<<mostTaggedMovie.tagCount<<")"<<endl;
        cout<<"--"<<endl;
        cout<<"Most used Tag: "<<mostUsedTag.tagName<<" (Times Used:"<<mostUsedTag.timesUsed<<")"<<endl;
        cout<<"Most common genre: "<<mostCommonGenre.genre<<" (Times Used:"<<mostCommonGenre.timesUsed<<")"<<endl;
        cout<<"Genre with highest mean rating: "<<highestRatedGenre.genre<<" (Mean Rating:"<<highestRatedGenre.meanRating<<")"<<endl;
        enterToContinue();
        cout<<"Continuing..."<<endl;
}

void manageDataMenu(){
    multipleNewlines();
    bool manageDataSelectionIsValid = true;
    do{
        manageDataSelectionIsValid = true;
        cout<<"*MANAGE DATA*"<<endl;
        cout<<"1.View Data"<<endl;
        cout<<"2.Delete specific users/movies"<<endl;
        cout<<"x.Back to previous menu"<<endl;
        string manageDataSelection_str;
        getline(cin,manageDataSelection_str);
        if(manageDataSelection_str == "x"){
            return;
        }
        if(emptyInput(manageDataSelection_str)){
            manageDataSelectionIsValid = false;
            continue;
        }
        char manageDataSelection = manageDataSelection_str.at(0);
        if(manageDataSelection != '1' && manageDataSelection != '2'){
            manageDataSelectionIsValid = false;
            continue;
        } else{
            manageDataSelectionIsValid = true;
            switch(manageDataSelection){
                case '1':
                    viewDataMenu();
                    break;
                case '2':
                    deleteSpecificMenu();
                    break;
            }
        }
    }while(!manageDataSelectionIsValid);
}

void viewDataMenu(){
    multipleNewlines();
    bool viewDataSelectionIsValid = true;
    do{
        viewDataSelectionIsValid = true;
        cout<<"*VIEW DATA*"<<endl;
        cout<<"1.View specific user"<<endl;
        cout<<"2.View specific movie"<<endl;
        cout<<"x.Back to previous menu"<<endl;
        string viewDataSelection_str;
        char viewDataSelection;
        getline(cin,viewDataSelection_str);
        if(viewDataSelection_str == "x"){
            return;
        }
        if(emptyInput(viewDataSelection_str)){
            viewDataSelectionIsValid = false;
            continue;
        }
        viewDataSelection = viewDataSelection_str.at(0);
        if(viewDataSelection == '1'){
            viewSpecificUserMenu();
        }
        if(viewDataSelection == '2'){
            viewSpecificMovieMenu();
        }
    }while(!viewDataSelectionIsValid);
}

void viewSpecificUserMenu(){
    multipleNewlines();
    bool userIdSelectionIsValid = true;
    do{
        userIdSelectionIsValid = true;
        cout<<"* VIEW SPECIFIC USER *"<<endl;
        cout<<"User ID (input 0 to cancel):";
        string userIdSelection_str;
        getline(cin,userIdSelection_str);
        cout<<"\n";
        if(userIdSelection_str == "0"){
            return;
        }
        if(emptyInput(userIdSelection_str)){
            userIdSelectionIsValid = false;
            continue;
        }
        if(!strIsNumeric(userIdSelection_str)){
            userIdSelectionIsValid = false;
            cout<<"user ID must be numeric (positive integer). Please try again."<<endl;
            enterToContinue();
            continue;
        }
        ulong inputUserId = strToUlong(userIdSelection_str);
        try{
            User foundUser = recommenderDataHandler.extractUserInfoFromId(inputUserId);
            showUserDetails(foundUser);
        }catch(UserNotFoundException& e){
            cout<<"Could not find user with inputUserId="<<inputUserId<<endl;
            cout<<"Please try again with another user."<<endl;
            enterToContinue();
            userIdSelectionIsValid = false;
            continue;
        }
    }while(!userIdSelectionIsValid);
}

void viewSpecificMovieMenu(){
    multipleNewlines();
    bool movieIdSelectionIsValid = true;
    do{
        movieIdSelectionIsValid = true;
        cout<<"* VIEW SPECIFIC MOVIE *"<<endl;
        cout<<"Movie ID (input 0 to cancel):";
        string movieIdSelection_str;
        getline(cin,movieIdSelection_str);
        cout<<"\n";
        if(movieIdSelection_str == "0"){
            return;
        }
        if(emptyInput(movieIdSelection_str)){
            movieIdSelectionIsValid = false;
            continue;
        }
        if(!strIsNumeric(movieIdSelection_str)){
            movieIdSelectionIsValid = false;
            cout<<"movie ID must be numeric (positive integer). Please try again."<<endl;
            enterToContinue();
            continue;
        }
        ulong inputMovieId = strToUlong(movieIdSelection_str);
        try{
            Movie foundMovie = recommenderDataHandler.extractMovieFromMovieId(inputMovieId);
            showMovieDetails(foundMovie);
        }catch(MovieNotFoundException& e){
            cout<<"Could not find movie with inputMovieId="<<inputMovieId<<endl;
            cout<<"Please try again with another movie."<<endl;
            enterToContinue();
            movieIdSelectionIsValid = false;
            continue;
        }
    }while(!movieIdSelectionIsValid);
}

void deleteSpecificMenu(){
    multipleNewlines();
    bool deleteSpecificSelectionIsValid = true;
    do{
        cout<<"*DELETE SPECIFIC DATA*"<<endl;
        cout<<"1.Delete specific user(s)"<<endl;
        cout<<"2.Delete specific movie(s)"<<endl;
        cout<<"x.Back to previous menu"<<endl;
        string deleteSpecificSelection_str;
        getline(cin,deleteSpecificSelection_str);
        if(deleteSpecificSelection_str == "x"){
            return;
        }
        if(emptyInput(deleteSpecificSelection_str)){
            deleteSpecificSelectionIsValid = false;
            continue;
        }
        char deleteSpecificSelection = deleteSpecificSelection_str.at(0);
        if(deleteSpecificSelection != '1' && deleteSpecificSelection != '2'){
            deleteSpecificSelectionIsValid = false;
            continue;
        }
        if(deleteSpecificSelection == '1'){
            deleteSpecificUserMenu();
        }
        if(deleteSpecificSelection == '2'){
            deleteSpecificMovieMenu();
        }
    }while(!deleteSpecificSelectionIsValid);
}

void deleteSpecificUserMenu(){
    multipleNewlines();
    bool deleteSpecificUserSelectionIsValid = true;
    do{
        deleteSpecificUserSelectionIsValid = true;
        cout<<"*DELETE SPECIFIC USER*"<<endl;
        cout<<"User ID  (input 0 to cancel):";
        string deleteSpecificUserSelection_str;
        getline(cin,deleteSpecificUserSelection_str);
        cout<<"\n";
        if(deleteSpecificUserSelection_str == "0"){
            return;
        }
        if(emptyInput(deleteSpecificUserSelection_str)){
            deleteSpecificUserSelectionIsValid = false;
            continue;
        }
        if(!strIsNumeric(deleteSpecificUserSelection_str)){
            deleteSpecificUserSelectionIsValid = false;
            cout<<"user ID must be numeric (positive integer). Please try again."<<endl;
            enterToContinue();
            continue;
        }
        ulong inputUserId = strToUlong(deleteSpecificUserSelection_str);
        try{
            User foundUser = recommenderDataHandler.extractUserInfoFromId(inputUserId);
            bool userDeleted = recommenderDataHandler.deleteSingleUser(foundUser);
            if(!userDeleted){
                cout<<"user with id="<<foundUser.userId<<" was not deleted!"<<endl;
                enterToContinue();
                deleteSpecificUserSelectionIsValid = false;
                continue;
            }else{
                cout<<"user with id="<<foundUser.userId<<" was deleted successfully."<<endl;
                enterToContinue();
            }
        }catch(UserNotFoundException& e){
            cout<<"Could not find user with inputUserId="<<inputUserId<<endl;
            cout<<"Please try again with another user."<<endl;
            enterToContinue();
            deleteSpecificUserSelectionIsValid = false;
            continue;
        }
    }while(!deleteSpecificUserSelectionIsValid);
}

void deleteSpecificMovieMenu(){
    multipleNewlines();
    bool deleteSpecificMovieSelectionIsValid = true;
    do{
        deleteSpecificMovieSelectionIsValid = true;
        cout<<"*DELETE SPECIFIC MOVIE*"<<endl;
        cout<<"Movie ID  (input 0 to cancel):";
        string deleteSpecificMovieSelection_str;
        getline(cin,deleteSpecificMovieSelection_str);
        cout<<"\n";
        if(deleteSpecificMovieSelection_str == "0"){
            return;
        }
        if(emptyInput(deleteSpecificMovieSelection_str)){
            deleteSpecificMovieSelectionIsValid = false;
            continue;
        }
        if(!strIsNumeric(deleteSpecificMovieSelection_str)){
            deleteSpecificMovieSelectionIsValid = false;
            cout<<"movie ID must be numeric (positive integer). Please try again."<<endl;
            enterToContinue();
            continue;
        }
        ulong inputMovieId = strToUlong(deleteSpecificMovieSelection_str);
        try{
            Movie foundMovie = recommenderDataHandler.extractMovieFromMovieId(inputMovieId);
            bool movieDeleted = recommenderDataHandler.deleteSingleMovie(foundMovie);
            if(!movieDeleted){
                cout<<"movie with id="<<foundMovie.movieId<<" was not deleted!"<<endl;
                enterToContinue();
                deleteSpecificMovieSelectionIsValid = false;
                continue;
            }else{
                cout<<"movie with id="<<foundMovie.movieId<<" was deleted successfully."<<endl;
                enterToContinue();
            }
        }catch(MovieNotFoundException& e){
            cout<<"Could not find movie with inputMovieId="<<inputMovieId<<endl;
            cout<<"Please try again with another movie."<<endl;
            enterToContinue();
            deleteSpecificMovieSelectionIsValid = false;
            continue;
        }
    }while(!deleteSpecificMovieSelectionIsValid);
}

void deleteAllDataMenu(){
    multipleNewlines();
    bool deleteallSelectionIsValid = false;
    do{
        deleteallSelectionIsValid = true;
        cout<<"*Are you sure you want to delete ALL data?*"<<endl;
        cout<<"Input '0' to confirm, input anything else to cancel."<<endl;
        string deleteallSelection_str;
        if(emptyInput(deleteallSelection_str)){
            deleteallSelectionIsValid = false;
            continue;
        }
        char deleteallSelection = deleteallSelection_str.at(0);
        if(deleteallSelection == '0'){
            cout<<"Deleting ALL users..."<<endl;
            bool userDeletionSuccess = recommenderDataHandler.deleteALLUsers();
            if(userDeletionSuccess){
                cout<<"users deleted successfully."<<endl;
            } else{
                cout<<"users were NOT deleted successully!"<<endl;
            }
            cout<<"Deleting ALL movies.."<<endl;
            bool movieDeletionSuccess = recommenderDataHandler.deleteALLMovies();
            if(movieDeletionSuccess){
                cout<<"movies deleted successfully."<<endl;
            } else{
                cout<<"movies were NOT deleted successfully!"<<endl;
            }
            enterToContinue();
        }
    }while(!deleteallSelectionIsValid);
}

void recommendMoviesMenu(){

	MovieRecommendationsWrapper results;
    double recommendElapsed = 0;
    RecommendationParameters recommendationParameters;
    char recommendationType;

    bool errorOccured = false;
	
	do {
    multipleNewlines();
	cout<<endl<<"- - -"<<endl;
	cout<<"* Recommend Movies *"<<endl;

    recommendationType = recommendationTypeMenu();
    if(recommendationType == '0'){
        return;
    }
    recommendationParameters = recommendationParametersMenu(recommendationType);

    if(recommendationParameters.profilesExist == false){
        return;
    }
	
    CustomSteadyTimer recommendMoviesTimer;

	try {
        if(recommendationType == '1'){
            recommendMoviesTimer.markStartPoint();
            if(recommendationParameters.standaloneProfileIsUsed){
                cout<<"Starting personalized recommendation procedure for standalone profile."<<endl;
                results = recommendMoviesPersonalizedStandaloneProfile(recommendationParameters.userProfileData,recommendationParameters.profileName,recommenderDataHandler,config);
            } else{
                cout<<"Starting personalized recommendation procedure."<<endl;
                results = recommendMoviesPersonalized(recommendationParameters.userID, recommenderDataHandler, config);
            }
            recommendElapsed = recommendMoviesTimer.markEndPointAndGetTimeElapsed();
        } else if(recommendationType == '2'){
            recommendMoviesTimer.markStartPoint();
            cout<<"Starting general-best-rated recommendation procedure."<<endl;
            cout<<"GenreCount="<<recommendationParameters.genres.size()<<",tagCount="<<recommendationParameters.tagNames.size()<<endl;;
            results = recommendMoviesGeneralBestRated(recommenderDataHandler,recommendationParameters.genres,recommendationParameters.tagNames,recommendationParameters.filteringTypes);
            recommendElapsed = recommendMoviesTimer.markEndPointAndGetTimeElapsed();
        } else{
            throw InvalidStateException("Unexpected 'recommendationType' parameter value="+to_string(recommendationType));
        }
    }
    catch(RecommenderException& e){
        cout<<"- - -"<<endl;
        cout << "An error occured: " << endl;
        cout<<e.getExceptionMessage()<<endl;
        cout<<"Recommendation procedure aborted."<<endl;
        errorOccured = true;
        enterToContinue();
        continue;
    }
    }while(errorOccured);
    cout<<"..."<<endl;
    enterToContinue("Finished. Press Enter to see recommendation results.");
    bool standaloneProfile = recommendationParameters.standaloneProfileIsUsed;
    showRecommendationResults(results,recommendationParameters,recommendationType,recommendElapsed,standaloneProfile);
}

char recommendationTypeMenu(){

    char selection = '0';
    do{
        cout<<"==Choose recommendation type=="<<endl;
        cout<<"1.Personalized"<<endl;
        cout<<"2.General (best rated)"<<endl;
        cout<<"x.Back to previous menu."<<endl;

        string selection_str;
        getline(cin,selection_str);
        if(emptyInput(selection_str)){
            continue;
        }
        if(selection_str == "x"){
            return '0';
        }

        selection = selection_str.at(0);

    }while(selection!='1' && selection!='2');

    return selection;
}

RecommendationParameters recommendationParametersMenu(char recommendationType){

    RecommendationParameters chosenParameters;
    if(recommendationType == '1'){//'1':Personalized
        string userTypeSelection_str;
        char userTypeSelection = 0;
        do{
        cout<<"Choose user type for recommendation:"<<endl;
        cout<<"1.User profile"<<endl;
        cout<<"2.User ID"<<endl;
        getline(cin,userTypeSelection_str);
        if(emptyInput(userTypeSelection_str)){
            continue;
        }
        userTypeSelection = userTypeSelection_str.at(0);
        }while(userTypeSelection != '1' && userTypeSelection != '2');
        if(userTypeSelection == '1'){
            cout<<"'User profile' option selected."<<endl;
            UserProfileFull* selectedProfile = loadProfileMenu(false);
            if(selectedProfile == nullptr){
                chosenParameters.profilesExist = false;
                return chosenParameters;
            }
            chosenParameters.standaloneProfileIsUsed = !selectedProfile->isDataBound();
            if(chosenParameters.standaloneProfileIsUsed){
                cout<<"Selected user profile is 'standalone'."<<endl;
                chosenParameters.userProfileData = selectedProfile->getStandaloneProfile().userData;
                chosenParameters.profileName = selectedProfile->getStandaloneProfile().profileName;
            } else{
                cout<<"Selected user profile is 'databound'."<<endl;
                chosenParameters.userID = selectedProfile->getUserId();
                chosenParameters.profileName = selectedProfile->getDataBoundProfile().profileName;
            }
        } else{
            cout<<"'User ID' option selected."<<endl;
            cout<<"userId:";
            cin>>chosenParameters.userID;
            chosenParameters.standaloneProfileIsUsed = false;
            cout<<endl;
            cin.get();
        }
    }else if(recommendationType == '2'){//'2':General
        inputGenresMenu(chosenParameters);
        inputFilteringMethodMenu(chosenParameters,"genres");
        inputTagsMenu(chosenParameters);
        inputFilteringMethodMenu(chosenParameters,"tags");
    } else{
        throw InvalidStateException("Unexpected 'recommendationType' parameter value="+to_string(recommendationType));
    }
    return chosenParameters;
}

void inputGenresMenu(RecommendationParameters& recParam) {
    cout<<"Input Genres:  (input the $ character to stop)"<<endl;
    string genre;
    do{
        cout<<"-:";
        getline(cin,genre);
        if(emptyInput(genre)){
            continue;
        }
        if(genre == "$"){
            break;
        }
        GenreResultWrapper genreResult = determineGenre(genre,validGenresStrVec);
        if(genreResult.isValid){
            recParam.genres.push_back(genreResult.genre);
            cout<<"Entered recognized genre: "<<genreResult.genre.str<<endl;
        } else{
            cout<<"The entered genre was not recognized as a valid genre, please try again."<<endl;
        }
    }while(true);
}

vector<string> inputGenresMenu(){
    vector<string> genresToAdd;
    cout<<"Input Genres: (input the $ character to stop)"<<endl;
    string genre;
    do{
        cout<<"-:";
        getline(cin,genre);
        if(emptyInput(genre)){
            continue;
        }
        if(genre == "$"){
            break;
        }
        GenreResultWrapper genreResult = determineGenre(genre,validGenresStrVec);
        if(genreResult.isValid){
            genresToAdd.push_back(genreResult.genre.str);
            cout<<"Entered recognized genre: "<<genreResult.genre.str<<endl;
        } else{
            cout<<"The entered genre was not recognized as a valid genre, please try again."<<endl;
        }
    }while(true);
    return genresToAdd;
}

void inputTagsMenu(RecommendationParameters& recParam){
    cout<<"Input tags: (input the $ character to stop)"<<endl;
    string tag;
    do{
        cout<<"-:";
        getline(cin,tag);
        if(emptyInput(tag)){
            continue;
        }
        if(tag == "$"){
            break;
        }

        //all to lowercase
        //(to match with assumption that all imported tags from files are converted to lowercase as well)
        //also remove trailing,leading and extra whitespaces.
        tag = strToLower(tag);
        stripLeadingAndTrailingWhitespaces(tag);
        tag = removeExtraWhitespaces(tag);

        recParam.tagNames.push_back(tag);
    }while(true);
}

vector<TagEntry> inputTagsMenu(){
    vector<TagEntry> tagsToAdd;
    cout<<"Input tags: (input the $ character to stop)"<<endl;
    string tag;
    TagEntry newTagEntry;
    do{
        cout<<"-:";
        getline(cin,tag);
        if(emptyInput(tag)){
            continue;
        }
        if(tag == "$"){
            break;
        }
        //all to lowercase
        //(to match with assumption that all imported tags from files are converted to lowercase as well)
        //also remove trailing,leading and extra whitespaces.
        tag = strToLower(tag);
        stripLeadingAndTrailingWhitespaces(tag);
        tag = removeExtraWhitespaces(tag);
        newTagEntry.tagName = tag;

        unsigned long tagId = recommenderDataHandler.findOrGenerateTagId(tag);
        newTagEntry.tagId = tagId;

        string tagCount_str;
        bool validTagCountInput = true;
        do{
            validTagCountInput = true;
            cout<<"Enter tag count for the inserted tag (positive numeric input required):";
            getline(cin,tagCount_str);
            if(!strIsNumeric(tagCount_str)){
                cout<<"Positive numeric input is required for tag count. Please try again."<<endl;
                validTagCountInput = false;
                enterToContinue();
                continue;
            }
            unsigned long tagCount = strToUlong(tagCount_str);
            if(tagCount == 0){
                cout<<"Zero is an invalid input for tag count. Please try again."<<endl;
                validTagCountInput = false;
                enterToContinue();
                continue;
            }
        }while(emptyInput(tagCount_str) || !validTagCountInput);
        tagsToAdd.push_back(newTagEntry);
    }while(true);
    return tagsToAdd;
}

void inputFilteringMethodMenu(RecommendationParameters& recParam,string forWhat){
    bool validFilteringMethodInput = true;
    char filtering_method = '0';
    do{
        validFilteringMethodInput = true;
        cout<<"\n";
        cout<<"Input filtering method for "<<forWhat<<"::"<<endl;
        cout<<"1.strict"<<endl;
        cout<<"2.lenient"<<endl;
        cout<<"3.very lenient"<<endl;
        cout<<"h.Help"<<endl;
        string filtering_method_str;
        getline(cin,filtering_method_str);
        if(emptyInput(filtering_method_str)){
            validFilteringMethodInput = false;
            continue;
        }
        filtering_method = filtering_method_str.at(0);
        if(filtering_method != '1' && filtering_method != '2' && filtering_method != '3' && filtering_method != 'h'){
            cout<<"Unrecognized option entered. Please select one of the valid options."<<endl;
            enterToContinue();
            validFilteringMethodInput = false;
            continue;
        }
        if(filtering_method == 'h'){
            filteringHelpScreen();
            validFilteringMethodInput = false;
            continue;
        }
    }while(!validFilteringMethodInput);
    pair<string,char> newFilteringType = make_pair(forWhat,filtering_method);
    recParam.filteringTypes.insert(newFilteringType);
}

void strictText(){
    cout<<"*STRICT*"<<endl;
    cout<<"Input genres and/or tags must be exactly matched in both quantity and "
          "values with the genres and/or tags of the movie."<<endl;
}

void lenientText(){
    cout<<"*LENIENT*"<<endl;
    cout<<"Input genres and/or tags must all be contained in the genres and/or tags of the movie."<<endl;
}

void veryLenientText(){
    cout<<"*VERY LENIENT*"<<endl;
    cout<<"From all the input genres and/or tags, at least one must be contained in the genres and/or tags of the movie."<<endl;
}

void filteringHelpScreen() {
    cout<<"Filtering values determine the way that the input genres and tags by the user "
          "will filter the recommendation results."<<endl;
    cout<<"***"<<endl;
    strictText();
    lenientText();
    veryLenientText();
    enterToContinue();
}

void importDataMenu(){
    bool validSelection;
    char selection;
    do{
    multipleNewlines();
	cout<<endl<<"- - -"<<endl;
	cout<<"* Import Data *"<<endl;
	cout<<endl;
    cout<<"1. Import data from csv file"<<endl;
	cout<<"2. Import data from json file"<<endl;
    cout<<"3. Add single user"<<endl;
    cout<<"4. Add single movie"<<endl;
    cout<<"--"<<endl;
    cout<<"a.User profile menu."<<endl;
    cout<<"--"<<endl;
    cout<<"s.Application settings."<<endl;
    cout<<"--"<<endl;
    cout<<"c.Go back to previous menu."<<endl;
    cout<<"x.Exit"<<endl;
	
	string selection_str;
	getline(cin,selection_str);
    if(emptyInput(selection_str)){
        continue;
    }
    selection = selection_str.at(0);

    cout<<endl;
	
    validSelection = importDataMenuSelectionResolve(selection);

    if(!validSelection){
        cout<<"Invalid input, press Enter to try again."<<endl;
        cin.get();
    }

    if(selection == 'x'){
        terminateApplication();
    }

    //if 'previous menu' is selected, only allow it if data was loaded.
    if(selection == 'c'){
        if(!dataLoaded){
            cout<<"Cannot go to main menu without data loaded!"<<endl;
            selection = '0';
            enterToContinue();
            continue;
        }
    }

    }while(selection!='x' && selection!='c');
}

//returns false if an unrecognized selection was chosen.
bool importDataMenuSelectionResolve(char selection){
	
	switch(selection){
		
		case '1':
			importFromCsvMenu();
            return true;
		
		case '2':
			importFromJsonMenu();
            return true;
		case '3':
			addUserMenu();
            return true;
		
		case '4':
			addMovieMenu();
            return true;

       case 'a':
            userProfileMenu();
            return true;

       case 's':
            settingsMenu();
            return true;

       case 'c':
            return true;
       case 'x':
            return true;
       default:
            return false;
	}
	
}

void importFromCsvMenu() {

    bool importFinished = false;

    parseInit();

    CustomSteadyTimer initTimer;

    string moviesFilepath;
    string ratingsFilepath;
    string tagsFilepath;
    string genometagsFilepath;

    char selection;

    do{
    multipleNewlines();
    cout<<endl<<"- - -"<<endl;
    cout<<"* Csv Import *"<<endl;
    cout<<"1.Use default file paths (from config file)"<<endl;
    cout<<"2.Manually insert file paths"<<endl;
    cout<<"x.Back to previous menu"<<endl;

    size_t userLastMemberPosBeforeImport = recommenderDataHandler.getUserCount()-1;
    size_t movieLastMemberPosBeforeImport = recommenderDataHandler.getMovieCount()-1;

    string selection_str;
    getline(cin,selection_str);
    if(emptyInput(selection_str)){
        continue;
    }
    selection = selection_str.at(0);

    if(selection == 'x'){
        return;
    }

    switch(selection){
		
		case '1':
			parsePathsInit();
			moviesFilepath=config.moviesFileDefaultPath;
			ratingsFilepath=config.ratingsFileDefaultPath;
            tagsFilepath=config.tagEntriesFileDefaultPath;
            genometagsFilepath=config.tagIdsFileDefaultPath;
			break;
			
		case '2':
			cout<<endl;
			cout<<"Specify path of files:"<<endl;
            cout<<"Movies file (required):"<<endl;
			getline(cin,moviesFilepath);
            if(emptyInput(moviesFilepath)){
                continue;
            }
            cout<<"Ratings file (required):"<<endl;
			getline(cin,ratingsFilepath);
            if(emptyInput(ratingsFilepath)){
                continue;
            }
            cout<<"Tag entries file (required):"<<endl;
			getline(cin,tagsFilepath);
            if(emptyInput(tagsFilepath)){
                continue;
            }
            cout<<"Tag ids-names file (optional,input '0' if file doesnt exist):"<<endl;
			getline(cin,genometagsFilepath);
            if(emptyInput(genometagsFilepath)){
                continue;
            }
			break;
	}
	
    if(selection=='1' || selection=='2'){
        config.moviesFileActivePath = moviesFilepath;
        config.ratingsFileActivePath = ratingsFilepath;
        config.tagEntriesFileActivePath = tagsFilepath;
        config.tagIdsFileActivePath = genometagsFilepath;
	
        double initElapsed;
        try{
            initTimer.markStartPoint();
            //sets configuration and then initializes data.
            recommenderDataHandler.setConfig(&config);
            recommenderDataHandler.initData();
            initElapsed = initTimer.markEndPointAndGetTimeElapsed();
        }catch(RecommenderException& e){
            if(config.terminateApplicationOnCsvImportError){
                terminateApplication();
            }
            cout<<"An error occured while importing csv files: "<<e.getExceptionMessage()<<endl;
            cout<<"Operation cannot continue. Do you want to keep the so-far loaded data or do you want to reset?"<<endl;
            cout<<"Warning: if data is NOT reset: corrupted,incomplete or duplicate data may remain!"<<endl;
            bool resetSelectionIsValid = true;
            do{
            resetSelectionIsValid = true;
            cout<<"k.Keep data loaded so far."<<endl;
            cout<<"d.Delete all loaded data."<<endl;
            cout<<"x.Terminate application"<<endl;
            string resetSelection_str;
            char resetSelection;
            getline(cin,resetSelection_str);
            if(emptyInput(resetSelection_str)){
                resetSelectionIsValid = false;
            }
            resetSelection = resetSelection_str.at(0);
            if(resetSelection == 'x'){
                throw e;
            }
            if(resetSelection != 'k' && resetSelection != 'd'){
                resetSelectionIsValid = false;
            }
            if(resetSelection == 'd'){
                cout<<"Rolling back users..."<<endl;
                bool usersRollbackSuccess = recommenderDataHandler.rollbackUsers(userLastMemberPosBeforeImport);
                if(usersRollbackSuccess){
                    cout<<"users rollback successful."<<endl;
                } else{
                    cout<<"users rollback NOT successful!"<<endl;
                }
                bool moviesRollbackSuccess = recommenderDataHandler.rollbackMovies(movieLastMemberPosBeforeImport);
                if(moviesRollbackSuccess){
                    cout<<"movies rollback successful"<<endl;
                } else{
                    cout<<"movies rollback NOT successful!"<<endl;
                }
            }
            }while(!resetSelectionIsValid);
        }catch(exception& e){
            throw e;
        }
        cout<<endl;
        cout<<"*FINISHED*"<<endl;
        cout<<"Total time elapsed: "<<initElapsed<<endl;
        cout<<"Time elapsed for parsing:"<<recommenderDataHandler.getElapsedTimes().getParseElapsed()<<endl;
        cout<<"Time elapsed for parsing post-processing:"<<recommenderDataHandler.getElapsedTimes().getParsePostProcessElapsed()<<endl;
        cout<<"Time elapsed for data calculations:"<<recommenderDataHandler.getElapsedTimes().getDataCalcElapsed()<<endl;
        dataLoaded = true;
        cout<<"*"<<endl;
        cout<<"Will check for any pending parameters on existing profiles, and will populate them."<<endl;
        ulong affectedProfileCount = recommenderDataHandler.populateWaitingUserProfiles();
        cout<<"Done. Updated "<<affectedProfileCount<<" profiles."<<endl;
        cout<<endl;
        importFinished = true;
        enterToContinue();
    }
    }while(selection!='1' && selection!='2' && !importFinished);
}

void importFromJsonMenu(){

    bool importFinished = false;
	
    do{
        multipleNewlines();
        cout<<endl<<"- - -"<<endl;
        cout<<"* Json Import *"<<endl;

        size_t userLastMemberPosBeforeImport = recommenderDataHandler.getUserCount()-1;
        size_t movieLastMemberPosBeforeImport = recommenderDataHandler.getMovieCount()-1;

        string usersFilepath;
        string moviesFilepath;

        do{
        cout<<"Specify filepath for users file:"<<endl;
        getline(cin,usersFilepath);
        }while(emptyInput(usersFilepath));

        do{
        cout<<"Specify filepath for movies file:"<<endl;
        getline(cin,moviesFilepath);
        }while(emptyInput(moviesFilepath));

        string importType_str;
        char importType = '0';
        bool validImportTypeInput = true;
        do{
            cout<<"- - -"<<endl;
            cout<<"Select import type:"<<endl;
            cout<<"1.File WITH neighbors and parameters in data."<<endl;
            cout<<"2.File WITHOUT neighbors and parameters in data."<<endl;

            getline(cin,importType_str);
            if(emptyInput(importType_str)){
                continue;
            }
            importType = importType_str.at(0);
            if(importType != '1' && importType != '2'){
                validImportTypeInput = false;
            }
        }while(emptyInput(importType_str) || !validImportTypeInput);

        try{
        if(importType == '1'){
            cout<<"Will not calculate neighbors and parameters for the imported users and moviess."<<endl;
            cout<<"Importing users...";
            JsonParseResultWrapper userParseResult = parseUsersFromJson(usersFilepath.c_str(),true);
            cout<<"done."<<endl;
            cout<<"Imported "<<userParseResult.parsedUsers.size()<<" users."<<endl;
            cout<<"Importing movies...";
            JsonParseResultWrapper movieParseResult = parseMoviesFromJson(moviesFilepath.c_str(),true);
            cout<<"done."<<endl;
            cout<<"Imported "<<movieParseResult.parsedMovies.size()<<" movies."<<endl;
            cout<<"Storing imported data in memory...";
            storeParsedElements(userParseResult);
            storeParsedElements(movieParseResult);
            cout<<"done."<<endl;
        } else{
            JsonParseResultWrapper userParseResult = parseUsersFromJson(usersFilepath.c_str(),false);
            JsonParseResultWrapper movieParseResult = parseMoviesFromJson(moviesFilepath.c_str(),false);
            cout<<"Import complete."<<endl;
            cout<<"Imported "<<userParseResult.parsedUsers.size()<<" users."<<endl;
            cout<<"Imported "<<movieParseResult.parsedMovies.size()<<" movies."<<endl;
            cout<<"Storing imported data in memory...";
            storeParsedElements(userParseResult);
            storeParsedElements(movieParseResult);
            cout<<"done."<<endl;
            //calculate neighbors and parameters for inserted users and movies.
            cout<<"Will calculate neighbors and parameters for all users and movies."<<endl;
            cout<<"Calculating user parameters and neighbors..."<<endl;
            calculateParametersForAllUsers(recommenderDataHandler.getAllUsers(),&config);
            cout<<"Done."<<endl;
            cout<<"Calculating movie parameters and neighbors..."<<endl;
            calculateParametersForAllMovies(recommenderDataHandler.getAllMovies(),
                                            recommenderDataHandler.getAllUsers(),&config);
            cout<<"Done."<<endl;
        }
        }catch(RecJsonParsingException& e){
            if(config.terminateApplicationOnJsonImportError){
                terminateApplication();
            }
            cout<<"An error occured while loading the json files:"<<endl;
            cout<<"Error name:"<<e.getExceptionName()<<endl;
            cout<<"Error message:"<<e.getExceptionMessage()<<endl;
            cout<<"Input 'c' to continue, or anything else to repeat the json import process:"<<endl;
            string errInput_str;
            getline(cin,errInput_str);
            if(errInput_str == "c"){
                importFinished = false;
                cout<<"Do you want to keep the so-far loaded data or do you want to reset?"<<endl;
                cout<<"Warning: if data is NOT reset: corrupted,incomplete or duplicate data may remain!"<<endl;
                bool resetSelectionIsValid = true;
                do{
                resetSelectionIsValid = true;
                cout<<"k.Keep data loaded so far."<<endl;
                cout<<"d.Delete all loaded data."<<endl;
                cout<<"x.Terminate application"<<endl;
                string resetSelection_str;
                char resetSelection;
                getline(cin,resetSelection_str);
                if(emptyInput(resetSelection_str)){
                    resetSelectionIsValid = false;
                }
                resetSelection = resetSelection_str.at(0);
                if(resetSelection == 'x'){
                    throw e;
                }
                if(resetSelection != 'k' && resetSelection != 'd'){
                    resetSelectionIsValid = false;
                }
                if(resetSelection == 'd'){
                    cout<<"Rolling back users..."<<endl;
                    bool usersRollbackSuccess = recommenderDataHandler.rollbackUsers(userLastMemberPosBeforeImport);
                    if(usersRollbackSuccess){
                        cout<<"users rollback successful."<<endl;
                    } else{
                        cout<<"users rollback NOT successful!"<<endl;
                    }
                    bool moviesRollbackSuccess = recommenderDataHandler.rollbackMovies(movieLastMemberPosBeforeImport);
                    if(moviesRollbackSuccess){
                        cout<<"movies rollback successful"<<endl;
                    } else{
                        cout<<"movies rollback NOT successful!"<<endl;
                    }
                }
                }while(!resetSelectionIsValid);
            } else{
                importFinished = true;
            }
            continue;
        }
        importFinished = true;
    }while(!importFinished);
    dataLoaded = true;
    recommenderDataHandler.populateWaitingUserProfiles();
    enterToContinue();
}

void addUserMenu() {
    char selection;
    do{
	cout<<endl<<"- - -"<<endl;
    cout<<"* Add User(s) *"<<endl;
	cout<<endl;
	cout<<"Choose input method."<<endl;
    cout<<"1.File(JSON)"<<endl;
	cout<<"2.Manual"<<endl;
    cout<<"x.Back to previous menu"<<endl;

	string selection_str;
	getline(cin, selection_str);
    if(emptyInput(selection_str)){
        continue;
    }
    selection = selection_str.at(0);

    if(selection == 'x'){
        return;
    }

	switch (selection){

		case '1':
			addUserFromFileMenu();
			break;

		case '2':
			addUserManualMenu();
			break;
	}
    }while(selection != '1' && selection!= '2');
}

void addUserFromFileMenu(){
    cout << endl << "--" << endl;
    string filepath;
    do{
    cout << "Specify file path: " << endl;
    getline(cin, filepath);
    }while(emptyInput(filepath));
    const char* filepath_c = filepath.c_str();

    vector<User> parsedUsers = parseUsersFromJson(filepath_c,true).parsedUsers;
    cout<<"Parsed "<<parsedUsers.size()<<" users from file."<<endl;
    if(parsedUsers.size() != 1){
        cout<<"Can only add 1 user from this menu. To import multiple users, "
              "select 'import data from json menu' in the import data menu"<<endl;
        enterToContinue();
        return;
    }
    User parsedUser = parsedUsers.at(0);
    //CALCULATE USER PARAMETERS FOR NEW USER.
    cout<<"Calculating parameters for the new user...";
    recommenderDataHandler.initParametersForSingleUser(parsedUser,&config);
    cout<<"done!"<<endl;
    cout<<"Adding user to data...";
    ulong addedCount = recommenderDataHandler.addUsers(parsedUsers);
    cout<<"done!"<<endl;
    cout<<"Added "<<addedCount<<" user to data successfully."<<endl;
    enterToContinue();
}

void addUserManualMenu(){

    User userToAdd;

    string userId_str;
    bool validUserIdInput = true;
    do{
        validUserIdInput = true;
        cout<<"Input user ID (no whitespaces, numeric inputs only):";
        getline(cin,userId_str);
        if(userId_str.find(' ') != string::npos){
            cout<<"No whitespaces allowed in user ID"<<endl;
            validUserIdInput = false;
            continue;
        }
        if(!strIsNumeric(userId_str)){
            cout<<"Non-numeric inputs are not allowed for user ID"<<endl;
            validUserIdInput = false;
            continue;
        }
    } while(emptyInput(userId_str) || !validUserIdInput);
    ulong inputUserId = strToUint(userId_str);
    userToAdd.userId = inputUserId;
    //

    string movieIds;
    do{
        cout<<"Input the movie ids of user watched movies (if any) and then press Enter (comma separated, no whitespaces):";
        getline(cin,movieIds);
        if(movieIds.find(' ') != string::npos){
            cout<<"No whitespaces allowed in movie ids input."<<endl;
            cout<<endl;
            continue;
        }
        break;
    } while(true);
    vector<ulong> newMovieIds = parseMovieIds(movieIds);
    cout<<"Εntered "<<newMovieIds.size()<<" movie ids."<<endl;
    vector<MovieEntry> newMovieEntries;
    for(size_t i=0;i<newMovieIds.size();i++){
        MovieEntry newMovieEntry;
        newMovieEntry.movieId = newMovieIds.at(i);
        newMovieEntries.push_back(newMovieEntry);
    }
    cout<<endl;
    if(newMovieEntries.size()>0){
        vector<double> ratings;
        bool hasEmptyInput = false;
        do {
            hasEmptyInput = false;
            cout<<"Input the ratings of the initial watched movies (comma separated, no whitespaces). If a specific watched movie does not have a rating, input '-' or '0' for it. If no ratings exist, press Enter without input:";
            string ratingsStr;
            getline(cin,ratingsStr);
            if(ratingsStr.find(' ') != string::npos){
                cout<<"No whitespaces allowed in ratings input."<<endl;
                cout<<endl;
                hasEmptyInput = true;
                continue;
            }
            ratings = parseRatings(ratingsStr);
            cout<<endl;
            if(ratings.size() > newMovieIds.size()){
                cout<<"Cannot have more ratings than watched movies!"<<endl;
                enterToContinue();
            }
        } while(ratings.size() > newMovieIds.size() && !hasEmptyInput);
        for(size_t i=0;i<ratings.size();i++){ //iterating over ratings,because they may be less than movieIds.
            newMovieEntries[i].rating = ratings.at(i);
            if(newMovieEntries[i].rating > 0){ //considered rated only if it is a valid rating!
                newMovieEntries[i].rated = true;
            }
        }
        for(size_t i=0;i<newMovieEntries.size();i++){
            cout<<"Input tags for movieId: "<<newMovieEntries[i].movieId<<" (if any) and press enter."<<endl;
            string currentTagsStr;
            getline(cin,currentTagsStr);
            vector<TagEntry> tagEntries = parseTagEntries(currentTagsStr);
            if(!tagEntries.empty()){
                newMovieEntries[i].tagged = true;
            }
            newMovieEntries[i].tags = tagEntries;
        }
    }

    //CALCULATE USER PARAMETERS FOR NEW USER.
    cout<<"Calculating parameters for the new user...";
    recommenderDataHandler.initParametersForSingleUser(userToAdd,&config);
    cout<<"done!"<<endl;

    cout<<"Adding user to stored data..."<<endl;
    bool success = recommenderDataHandler.addSingleUser(userToAdd);
    if(success){
        cout<<"User added successfully!"<<endl;
        enterToContinue();
    } else{
        cout<<"User addition failed! Could not add user."<<endl;
        enterToContinue();
    }
}

void addMovieMenu(){
    char selection;
    do{
    cout<<endl<<"- - -"<<endl;
    cout<<"* Add Movie(s) *"<<endl;
    cout<<endl;
    cout<<"Choose input method."<<endl;
    cout<<"1.File(JSON)"<<endl;
    cout<<"2.Manual"<<endl;
    cout<<"x.Back to previous menu"<<endl;

    string selection_str;
    getline(cin, selection_str);
    if(emptyInput(selection_str)){
        continue;
    }
    selection = selection_str.at(0);

    if(selection == 'x'){
        return;
    }

    switch (selection){

        case '1':
            addMovieFromFileMenu();
            break;

        case '2':
            addMovieManualMenu();
            break;
    }
    }while(selection != '1' && selection!= '2');
}

void addMovieFromFileMenu(){
	cout << endl << "--" << endl;
    string filepath;
    do{
	cout << "Specify file path: " << endl;
	getline(cin, filepath);
    }while(emptyInput(filepath));
	const char* filepath_c = filepath.c_str();

    vector<Movie> parsedMovies = parseMoviesFromJson(filepath_c,true).parsedMovies;
    cout<<"Parsed "<<parsedMovies.size()<<" movies from file."<<endl;
    if(parsedMovies.size() != 1){
        cout<<"Can only add 1 movie from this menu. To import multiple movies, "
              "select 'import data from json menu' in the import data menu"<<endl;
        enterToContinue();
        return;
    }
    Movie movieToAdd = parsedMovies.at(0);
    //CALCULATE MOVIE PARAMETERS FOR NEW MOVIE.
    cout<<"Calculating parameters for the new movie...";
    recommenderDataHandler.initParametersForSingleMovie(movieToAdd,&config);
    cout<<"done!"<<endl;
    cout<<"Adding movie to data...";
    ulong addedCount = recommenderDataHandler.addMovies(parsedMovies);
    cout<<"done!"<<endl;
    cout<<"Added "<<addedCount<<" movie to data successfully."<<endl;
    enterToContinue();
}

void addMovieManualMenu(){

    Movie movieToAdd;

    string movieId_str;
    bool validMovieIdInput = true;
    do{
        validMovieIdInput = true;
        cout<<"Input movie ID (no whitespaces, numeric inputs only):";
        getline(cin,movieId_str);
        if(movieId_str.find(' ') != string::npos){
            cout<<"No whitespaces allowed in user ID"<<endl;
            validMovieIdInput = false;
            continue;
        }
        if(!strIsNumeric(movieId_str)){
            cout<<"Non-numeric inputs are not allowed for movie ID"<<endl;
            validMovieIdInput = false;
            continue;
        }
    } while(emptyInput(movieId_str) || !validMovieIdInput);
    ulong inputMovieId = strToUint(movieId_str);
    movieToAdd.movieId = inputMovieId;

    //

    string moviename_str;
    bool validMovieNameInput = true;
    do{
        validMovieNameInput = true;
        cout<<"Input movie name:";
        getline(cin,moviename_str);
        if(moviename_str.find(' ') != string::npos){
            cout<<"No whitespaces allowed in movie name"<<endl;
            validMovieNameInput = false;
            continue;
        }
    }while(emptyInput(moviename_str) || !validMovieNameInput);

    //

    vector<string> inputGenres = inputGenresMenu();
    movieToAdd.genres = inputGenres;

    vector<TagEntry> inputTags = inputTagsMenu();
    movieToAdd.totalTags = inputTags;

    //CALCULATE MOVIE PARAMETERS FOR NEW MOVIE.
    cout<<"Calculating parameters for the new movie...";
    recommenderDataHandler.initParametersForSingleMovie(movieToAdd,&config);
    cout<<"done!"<<endl;

    cout<<"Adding movie to stored data..."<<endl;
    bool success = recommenderDataHandler.addSingleMovie(movieToAdd);
    if(success){
        cout<<"Movie added successfully!"<<endl;
        enterToContinue();
    } else{
        cout<<"Movie addition failed! Could not add movie."<<endl;
        enterToContinue();
    }
}

void exportDataMenu(){

    bool validSelection = true;
    do{
    validSelection = true;
    cout<<endl<<"- - -"<<endl;
    cout<<"* Export Data *"<<endl;
    cout<<endl;
    cout<<"1. Export data to json file"<<endl;
    cout<<"c. Go back to previous menu"<<endl;

    string selection_str;
    getline(cin,selection_str);
    if(emptyInput(selection_str)){
        validSelection = false;
        continue;
    }
    if(selection_str == "c"){
        return;
    }
    char selection = selection_str.at(0);

    validSelection = exportDataMenuSelectionResolve(selection);

    }while(!validSelection);
}

bool exportDataMenuSelectionResolve(char selection){

    switch(selection){
        case '1':
            exportToJsonMenu();
            return true;

        default:
            return false;
    }
}

void exportToJsonMenu(){

    string usersFilepath;
    string moviesFilepath;

    string prettyOut_str;

    bool prettyOutput;

    do{
    cout<<"Specify filepath for users file:"<<endl;
    getline(cin,usersFilepath);
    }while(emptyInput(usersFilepath));
    do{
    cout<<"Specify filepath for movies file:"<<endl;
    getline(cin,moviesFilepath);
    }while(emptyInput(moviesFilepath));

    do{
        cout<<"\nUse pretty output for output files? <y/n>"<<endl;
        cout<<"Note: pretty output may lead to larger output filesize and may be slower for large datasets."<<endl;
        getline(cin,prettyOut_str);
        if(prettyOut_str == "y" || prettyOut_str == "Y"){
            prettyOutput = true;
            cout<<"Will use pretty output for users file."<<endl;
            enterToContinue();
        } else{
            prettyOutput = false;
        }
    }while(emptyInput(prettyOut_str));

    recommenderDataHandler.setConfig(&config);
    exportToJson(recommenderDataHandler,usersFilepath,moviesFilepath,prettyOutput);
    enterToContinue("Operation complete. Press Enter to continue");
}

void helpScreen(){
	
}

void aboutScreen(){
}

void recalculateScreen(){
    cout<<"Currently in data: "<<recommenderDataHandler.getAllUsers().size()
       <<" users and "<<recommenderDataHandler.getAllMovies().size()<<" movies."<<endl;

    enterToContinue("Press enter to manually recalculate parameters for the above.");
    cout<<"Recalculating..."<<endl;
    ElapsedTimes elapsedTimes = recommenderDataHandler.calculateAllParameters();
    cout<<"Recalculation of data parameters finished."<<endl;
    cout<<"Recalculation time elapsed:"<<elapsedTimes.getDataCalcElapsed();
    cout<<"Recalculating all profile parameters."<<endl;
    ulong affectedCount = recommenderDataHandler.recalculateProfileParameters();
    cout<<"Recalculation of profile parameters finished. Updated "<<affectedCount<<" profiles."<<endl;
    enterToContinue();
}

//recommendationType: '1':Personalized , '2':General
void showRecommendationResults(MovieRecommendationsWrapper& results,RecommendationParameters recommendationParameters,char recommendationType,double timeElapsed,bool standaloneProfile){

    multipleNewlines();
    if(results.recommendedFromItemMethod.size() == 0 && results.recommendedFromUserMethod.size() == 0 && recommendationType == '1'){
        cout<<"Warning: There is insufficient data for a personalized recommendation for userID="<<recommendationParameters.userID<<endl;
        enterToContinue();
    }

    string selection_str;
    char selection_c = 0;
    bool selectionFailed = true;
    do{
        if(recommendationType == '1'){
            do{
                if(standaloneProfile){
                    cout<<"USER PROFILE "<<recommendationParameters.profileName<<" RECOMMENDATIONS:"<<endl;
                } else{
                    cout<<"USER "<<recommendationParameters.userID<<" RECOMMENDATIONS:"<<endl;
                }
            unsigned int x = showUserBasedRecommendations(results.recommendedFromUserMethod,0);
            showItemBasedRecommendations(results.recommendedFromItemMethod,x);
            cout<<"Time elapsed: "<<timeElapsed<<endl;
            cout<<"--"<<endl;
            cout<<"Select a recommendation to see its details."<<endl;
            cout<<"x.Exit"<<endl;
            getline(cin,selection_str);
            if(selection_str == "x"){
                selection_c = 'x';
                break;
            }
            if(emptyInput(selection_str)){
                continue;
            }
            selection_c = selection_str.at(0);
            if(isdigit(selection_c)){
                unsigned int selection = static_cast<unsigned int>(selection_c - '0');
                selectionFailed = resolveMovieRecommendationSelectionPersonalized(selection,results.recommendedFromUserMethod,results.recommendedFromItemMethod);
            }
            } while(selectionFailed);
        } else if(recommendationType == '2'){
            do{
            cout<<"GENERAL RECOMMENDATIONS:"<<endl;
            showGeneralRecommendations(results.recommendedFromGeneralMethod);
            cout<<"Time elapsed: "<<timeElapsed<<endl;
            cout<<"--"<<endl;
            cout<<"Select a recommendation to see its details."<<endl;
            cout<<"x.Exit"<<endl;
            getline(cin,selection_str);
            if(selection_str == "x"){
                selection_c = 'x';
                break;
            }
            if(emptyInput(selection_str)){
                continue;
            }
            selection_c = selection_str.at(0);
            if(isdigit(selection_c)){
                unsigned int selection = static_cast<unsigned int>(selection_c - '0');
                selectionFailed = resolveMovieRecommendationSelectionGeneral(selection,results.recommendedFromGeneralMethod);
            }
            }while(selectionFailed);
        }
    }while(selection_c != 'x');
}

//also sets the recommendationIDs for each recommendation.
unsigned int showUserBasedRecommendations(vector<MovieRecommendation>& userBasedRecommendations,uint x){
    cout<<"--"<<endl;
    cout<<"Based on the activity of similar users:"<<endl;
	
	if(userBasedRecommendations.size() == 0){
		cout<<"-No Recommendations-"<<endl;
		return 0;
	}

    for(auto it=userBasedRecommendations.begin();it!=userBasedRecommendations.end();it++){
        it->recommendationID = x;
        cout<<x<<"."<<it->movieName<<" ("<<showGenres(*it)<<")"<<endl;
        x++;
    }
    return x;
}

//also sets the recommendationIDs for each recommendation.
unsigned int showItemBasedRecommendations(vector<MovieRecommendation>& itemBasedRecommendations,uint x){
    cout<<"--"<<endl;
    cout<<"Based on movies that the user liked:"<<endl;
	
	if(itemBasedRecommendations.size() == 0){
		cout<<"-No Recommendations-"<<endl;
		return 0;
	}

    for(auto it=itemBasedRecommendations.begin();it!=itemBasedRecommendations.end();it++){
        it->recommendationID = x;
        cout<<x<<"."<<it->movieName<<" ("<<showGenres(*it)<<")"<<endl;
        x++;
    }
    return x;
}

//also sets the recommendationIDs for each recommendation.
void showGeneralRecommendations(vector<MovieRecommendation>& generalRecommendations){
    cout<<"--"<<endl;
    cout<<""<<endl; //TODO: determine message.

    if(generalRecommendations.size() == 0){
        cout<<"-No Recommendations-"<<endl;
        return;
    }

    uint x = 0;
    for(auto it=generalRecommendations.begin();it!=generalRecommendations.end();it++){
        it->recommendationID = x;
        cout<<x<<"."<<it->movieName<<" ("<<showGenres(*it)<<")"<<endl;
        x++;
    }
}

bool resolveMovieRecommendationSelectionPersonalized(uint selection,vector<MovieRecommendation>& rec1,vector<MovieRecommendation>& rec2){

    vector<MovieRecommendation>::iterator rec1Iter,rec2Iter;
    rec1Iter = find_if(rec1.begin(),rec1.end(),[&selection](MovieRecommendation& rec){return rec.recommendationID == selection;});
    if(rec1Iter == rec1.end()){
        rec2Iter = find_if(rec2.begin(),rec2.end(),[&selection](MovieRecommendation& rec){return rec.recommendationID == selection;});
    } else{
        showRecommendationDetails(*rec1Iter);
        return true;
    }
    if(rec2Iter == rec2.end()){
        cout<<"Invalid input. Press Enter and try again."<<endl;
        cin.get();
        return false;
    } else{
        showRecommendationDetails(*rec2Iter);
        return true;
    }
}

bool resolveMovieRecommendationSelectionGeneral(uint selection,vector<MovieRecommendation>& rec){

    vector<MovieRecommendation>::iterator recIter;
    recIter = find_if(rec.begin(),rec.end(),[&selection](MovieRecommendation& rec){return rec.recommendationID == selection;});
    if(recIter == rec.end()){
        cout<<"Invalid input. Press Enter and try again."<<endl;
        cin.get();
        return false;
    }
    showRecommendationDetails(*recIter);
    return true;
}

void showRecommendationDetails(MovieRecommendation& mvRc){
    switch(mvRc.recommendationType){
        case '0':
            showRecommendationDetailsUserBased(mvRc);
            enterToContinue();
            break;
        case '1':
            showRecommendationDetailsItemBased(mvRc);
            enterToContinue();
            break;
        case '2':
            showRecommendationDetailsGeneral(mvRc);
            enterToContinue();
            break;
        default:
            cout<<"An error occurred: cannot display movie recommendation details."<<endl;
            enterToContinue();
    }
}

void showRecommendationDetailsUserBased(MovieRecommendation& mvRc){

    cout<<"--"<<endl;
    cout<<"Recommendation Details:"<<endl;
    cout<<"Movie Name: "<<mvRc.movieName<<endl;
    cout<<"Genres: "<<showGenres(mvRc)<<endl;
    cout<<"Most Common Tags: "<<showMostCommonTags(mvRc.mostCommonTags)<<endl;
    cout<<"Mean Rating: "<<mvRc.meanRating<<endl;
    cout<<"Mean Rating from similar users: "<<mvRc.meanRatingFromNeighbors<<endl;
}

void showRecommendationDetailsItemBased(MovieRecommendation& mvRc){

    cout<<"--"<<endl;
    cout<<"Recommendation Details:"<<endl;
    cout<<"Movie Name: "<<mvRc.movieName<<endl;
    cout<<"Genres: "<<showGenres(mvRc)<<endl;
    cout<<"Most Common Tags: "<<showMostCommonTags(mvRc.mostCommonTags)<<endl;
    cout<<"Mean Rating: "<<mvRc.meanRating<<endl;
    cout<<"Similar movies watched by user: "<<showSimilarWatchedMovies(mvRc.similarWatchedMovies)<<endl;
}

void showRecommendationDetailsGeneral(MovieRecommendation& mvRc){

    cout<<"--"<<endl;
    cout<<"Recommendation Details:"<<endl;
    cout<<"Movie Name: "<<mvRc.movieName<<endl;
    cout<<"Genres: "<<showGenres(mvRc)<<endl;
    cout<<"Most Common Tags: "<<showMostCommonTags(mvRc.mostCommonTags)<<endl;
    cout<<"Mean Rating: "<<mvRc.meanRating<<endl;
}

string showGenres(MovieRecommendation& mvRc){
    string result = "";
    for(size_t i=0;i<mvRc.genres.size();i++){
        if(i!=0){
           result = result + ",";
        }
        result = result + mvRc.genres[i];
    }
    return result;
}

string showMostCommonTags(vector<TagEntry>& mostCommonTags){
    string result = "";
    for(size_t i=0;i<mostCommonTags.size();i++){
        if(i!=0){
            result = result + ",";
        }
        result = result + mostCommonTags[i].tagName;
    }
    return result;
}

string showSimilarWatchedMovies(vector<string>& similarWatchedMovies){
    string result = "";
    for(size_t i=0;i<similarWatchedMovies.size();i++){
        if(i!=0){
            result = result + ",";
        }
        result = result + similarWatchedMovies[i];
    }
    return result;
}

void userProfileMenu(){

    string selection_str;
    char selection;

    do{
    multipleNewlines();
    cout<<"*User Profile Menu*"<<endl;
    cout<<"1.Create new profile(s)"<<endl;
    cout<<"2.Edit profile"<<endl;
    cout<<"3.Delete profile(s)"<<endl;
    cout<<"4.Show all profiles"<<endl;
    cout<<"5.Save all profiles to custom file"<<endl;
    cout<<"x.Previous menu"<<endl;

    getline(cin,selection_str);
    if(emptyInput(selection_str)){
        continue;
    }
    selection = selection_str.at(0);
    cout<<endl;

    userProfileSelectionResolve(selection);

    }while(selection != 'x');
}

void userProfileSelectionResolve(char selection){

    switch(selection){
    case '1':
        createNewProfileMenu();
        break;
    case '2':
        loadProfileMenu(true);
        break;
    case '3':
        deleteProfileMenu();
        break;
    case '4':
        showAllProfiles();
        break;
    case '5':
        saveProfiles();
        break;
    case 'x':
    default:
        break;
    }
}

void createNewProfileMenu(){

    string selection_str;
    char selection;

    do{
    multipleNewlines();
    cout<<"*Create New Profile*"<<endl;
    cout<<"1.Manually create new STANDALONE user profile."<<endl;
    cout<<"2.Manually create new DATABOUND user profile."<<endl;
    cout<<"3.Load profiles from file."<<endl;
    cout<<"4.Help"<<endl;
    cout<<"x.Back to previous menu"<<endl;

    getline(cin,selection_str);
    if(emptyInput(selection_str)){
        continue;
    }
    selection = selection_str.at(0);
    cout<<endl;

    if(selection == 'x'){
        return;
    }

    }while(selection!='1' && selection!='2' && selection!='3');
    resolveCreateNewProfileMenuSelection(selection);
}

void resolveCreateNewProfileMenuSelection(char selection){
    switch(selection){
        case '1':
            createNewStandaloneProfileMenu();
            break;
        case '2':
            createNewDataBoundProfileMenu();
            break;
        case '3':
            createNewProfileFromFileMenu();
            break;
        case '4':
            profileHelp();
            break;
    }
}

void profileHelp(){

    string selection_str;

    cout<<"*USER PROFILE HELP*"<<endl;

    do{
    cout<<"Select topic"<<endl;
    cout<<"1.Standalone and databound profiles"<<endl;
    cout<<"2.Data available/unavailable modes"<<endl;
    cout<<"x.Back to previous menu"<<endl;

    getline(cin,selection_str);
    }while(emptyInput(selection_str));

    char selection = selection_str.at(0);

    if(selection == 'x'){
        return;
    }

}

void createNewProfileFromFileMenu(){
    cout<<"*LOAD USER PROFILES FROM FILE*"<<endl;
    string standaloneusrprf_selection_str;
    do{
    cout<<"Input filepath:";
    getline(cin,standaloneusrprf_selection_str);
    }while(emptyInput(standaloneusrprf_selection_str));
    size_t sizeBefore = recommenderDataHandler.getAllUserProfiles().size();
    recommenderDataHandler.loadUserProfilesFromFile(standaloneusrprf_selection_str,dataLoaded);
    size_t sizeAfter = recommenderDataHandler.getAllUserProfiles().size();
    size_t newProfileCount = sizeAfter - sizeBefore;
    enterToContinue("Operation complete. Loaded "+to_string(newProfileCount)+" profiles. Press Enter to continue.");
}

void createNewStandaloneProfileMenu(){
    cout<<"*STANDALONE USER PROFILE*"<<endl;
    do{
        cout<<"Input profile name (no whitespaces):";
        string profileName;
        getline(cin,profileName);
        if(emptyInput(profileName)){
            continue;
        }
        if(profileName.find(' ') != string::npos){
            cout<<"No whitespaces allowed in profile name."<<endl;
            cout<<endl;
            continue;
        }
        cout<<"\n";
        cout<<"Input the movie ids of initial watched movies (if any) and then press Enter (comma separated, no whitespaces):";
        string movieIds;
        getline(cin,movieIds);
        if(movieIds.find(' ') != string::npos){
            cout<<"No whitespaces allowed in movie ids input."<<endl;
            cout<<endl;
            continue;
        }
        vector<ulong> newMovieIds = parseMovieIds(movieIds);
        cout<<"User entered "<<newMovieIds.size()<<" movie ids."<<endl;
        vector<MovieEntry> newMovieEntries;
        for(size_t i=0;i<newMovieIds.size();i++){
            MovieEntry newMovieEntry;
            newMovieEntry.movieId = newMovieIds.at(i);
            newMovieEntries.push_back(newMovieEntry);
        }
        cout<<endl;
        if(newMovieEntries.size()>0){
            vector<double> ratings;
            bool hasEmptyInput = false;
            do {
                hasEmptyInput = false;
                cout<<"Input the ratings of the initial watched movies (comma separated, no whitespaces). If a specific watched movie does not have a rating, input '-' or '0' for it. If no ratings exist, press Enter without input:";
                string ratingsStr;
                getline(cin,ratingsStr);
                if(ratingsStr.find(' ') != string::npos){
                    cout<<"No whitespaces allowed in ratings input."<<endl;
                    cout<<endl;
                    hasEmptyInput = true;
                    continue;
                }
                ratings = parseRatings(ratingsStr);
                cout<<endl;
                if(ratings.size() > newMovieIds.size()){
                    cout<<"Cannot have more ratings than watched movies!"<<endl;
                    enterToContinue();
                }
            } while(ratings.size() > newMovieIds.size() && !hasEmptyInput);
            for(size_t i=0;i<ratings.size();i++){ //iterating over ratings,because they may be less than movieIds.
                newMovieEntries[i].rating = ratings.at(i);
                if(newMovieEntries[i].rating > 0){ //considered rated only if it is a valid rating!
                    newMovieEntries[i].rated = true;
                }
            }
            for(size_t i=0;i<newMovieEntries.size();i++){
                cout<<"Input tags for movieId: "<<newMovieEntries[i].movieId<<" (if any) and press enter."<<endl;
                string currentTagsStr;
                getline(cin,currentTagsStr);
                vector<TagEntry> tagEntries = parseTagEntries(currentTagsStr);
                if(!tagEntries.empty()){
                    newMovieEntries[i].tagged = true;
                }
                newMovieEntries[i].tags = tagEntries;
            }
        }
    ulong newUserId = recommenderDataHandler.autogenerateUserId();
    recommenderDataHandler.createNewUserProfileStandalone(profileName,newUserId,newMovieEntries,dataLoaded);
    cout<<"New standalone user profile created successfully."<<endl;
    enterToContinue();
    break;
    }while(true);
}

void createNewDataBoundProfileMenu(){
    cout<<"*DATABOUND USER PROFILE*"<<endl;
    string profileName;
    do{
    cout<<"Input profile name (no whitespaces):";
    cin>>profileName;
    cin.get(); //absorb the newline from the enter press.
    stripLeadingAndTrailingWhitespaces(profileName);
    }while(profileName.empty());
    cout<<endl;
    ulong boundUserId;
    string boundUserIdStr;
    do{
    cout<<"Input the target user ID to bind with:";
    cin>>boundUserIdStr;
    cin.get(); //absorb the newline from the enter press.
    boundUserId = strtoul(boundUserIdStr.c_str(),nullptr,10);
    }while(boundUserId == 0);
    if(dataLoaded){
        while(!recommenderDataHandler.userIdExists(boundUserId)){
            cout<<endl;
            cout<<"The selected user ID does not exist in the data! Please input a different user ID:";
            cin>>boundUserId;
            cin.get(); //absord the newline from the enter press.
        }
        recommenderDataHandler.createNewUserProfileDataBound(profileName,boundUserId,true);
        cout<<"New databound user profile created successfully."<<endl;
        enterToContinue();

    } else{
        recommenderDataHandler.createNewUserProfileDataBound(profileName,boundUserId,false);
        cout<<"\nNew databound user profile created successfully. Warning: the userId with which it binds was not found in the data.";
        cout<<" The details for this profile may not be available until this userId is loaded."<<endl;
        enterToContinue();
    }
}

UserProfileFull* loadProfileMenu(bool editProfile){

    multipleNewlines();
    ShowAllProfilesWrapper showAllProfilesResult = showAllProfiles(false);
    if(!showAllProfilesResult.profilesExist){
        UserProfileFull* emptyProfile = nullptr;
        return emptyProfile;
    }
    vector<UserProfileSelection> profileSelections = bindProfilesToSelectionIds(showAllProfilesResult.userProfiles);
    cout<<"---"<<endl;
    string selection_str;
    do{
    cout<<"Select profile by inputting its corresponding number:";
    getline(cin,selection_str);
    }while(emptyInput(selection_str));
    char selection = selection_str.at(0);
    while(!isdigit(selection) || charToUint(selection) > showAllProfilesResult.max_selection_id){
        cout<<endl;
        do{
        cout<<"Invalid input, please try again:";
        getline(cin,selection_str);
        }while(emptyInput(selection_str));
        selection = selection_str.at(0);
        cout<<endl;
    }
    bool validSelection = false;
    UserProfileFull* selectedUserProfile = nullptr;
    while(!validSelection){
        validSelection = resolveProfileSelection(profileSelections,charToUint(selection),&selectedUserProfile);
        if(!validSelection){
            do{
            cout<<"Invalid input, please try again:";
            getline(cin,selection_str);
            }while(emptyInput(selection_str));
            selection = selection_str.at(0);
            cout<<endl;
        }
    }
    if(editProfile){
        editProfileMenu(selectedUserProfile);
    }
    return selectedUserProfile;
}

//returns true if selection was valid
bool resolveProfileSelection(vector<UserProfileSelection>& profileSelections,uint selectionId,UserProfileFull** selectedUserProfile){

    auto selectIter = find_if(profileSelections.begin(),profileSelections.end(),
                              [&selectionId](UserProfileSelection& profSelection){return selectionId == profSelection.selection_id;});

    bool validSelection = !(selectIter == profileSelections.end());
    if(validSelection){
        *selectedUserProfile = selectIter->profile;
    }
    return validSelection;
}

//returns the maximum selection id(number).
ShowAllProfilesWrapper showAllProfiles(bool onlyShow){
    if(onlyShow){
        multipleNewlines();
    }
    cout<<"*ALL USER PROFILES*"<<endl;
    ShowAllProfilesWrapper result;
    vector<UserProfileFull>& allUserProfiles = recommenderDataHandler.getAllUserProfiles();
    if(allUserProfiles.size() == 0){
        cout<<endl;
        cout<<"-No user profiles available-"<<endl;
        enterToContinue();
        result.profilesExist = false;
        return result;
    }
    uint i;
    for(i=0;i<allUserProfiles.size();i++){
        cout<<(i+1)<<". "<<allUserProfiles[i].getProfileName();
        cout<<endl;
    }
    result.userProfiles = &allUserProfiles;
    result.max_selection_id = i+1;
    cout<<endl;
    if(onlyShow){
        string profdetails_selection_str;
        char profdetails_selection;
        vector<UserProfileSelection> profileSelections = bindProfilesToSelectionIds(result.userProfiles);
        do{
        cout<<"---"<<endl;
        cout<<"View profile details by inputting its corresponding number, \nor input 0 to continue:";
        getline(cin,profdetails_selection_str);
        if(emptyInput(profdetails_selection_str)){
            continue;
        }
        profdetails_selection = profdetails_selection_str.at(0);
        UserProfileFull* selectedUserProfile = nullptr;
        bool validSelection = false;
        while(!validSelection){
            if(profdetails_selection == '0'){
                validSelection = true;
            } else{
                validSelection = resolveProfileSelection(profileSelections,charToUint(profdetails_selection),&selectedUserProfile);
            }
            if(!validSelection){
                do{
                cout<<"Invalid input, please try again:";
                getline(cin,profdetails_selection_str);
                }while(emptyInput(profdetails_selection_str));
                profdetails_selection = profdetails_selection_str.at(0);
                cout<<endl;
            }
        }
        if(profdetails_selection != '0'){
            showProfileDetails(selectedUserProfile);
        }
        if(profdetails_selection!= '0'){
            multipleNewlines();
            for(i=0;i<allUserProfiles.size();i++){
                cout<<(i+1)<<". "<<allUserProfiles[i].getProfileName();
                cout<<endl;
            }
        }
        }while(profdetails_selection != '0');
    }
    if(onlyShow){
        enterToContinue();
    }
    return result;
}

void deleteProfileMenu(){

    multipleNewlines();
    cout<<"*Delete Profile(s)*"<<endl;

    ShowAllProfilesWrapper showAllProfilesResult = showAllProfiles(false);

    if(!showAllProfilesResult.profilesExist){
        return;
    }

    vector<UserProfileSelection> profileSelections = bindProfilesToSelectionIds(showAllProfilesResult.userProfiles);

    string first_selection_str;
    do{
    cout<<"Select one of the below:"<<endl;
    cout<<"1.Delete specific profile."<<endl;
    cout<<"2.Delete ALL profiles."<<endl;
    cout<<"x.Back to the previous menu"<<endl;

    getline(cin,first_selection_str);
    }while(emptyInput(first_selection_str));
    char first_selection = first_selection_str.at(0);

    if(first_selection == 'x'){
        cout<<endl;
        return;
    }

    while(!isdigit(first_selection) || charToUint(first_selection) > showAllProfilesResult.max_selection_id){
        cout<<endl;
        do{
        cout<<"Invalid input, please try again:";
        getline(cin,first_selection_str);
        }while(emptyInput(first_selection_str));
        first_selection = first_selection_str.at(0);
        cout<<endl;
    }

    if(first_selection == '2'){
        string deleteall_selection_str;
        do{
        cout<<"Are you sure you want to delete ALL user profiles? Input 'a' to continue, input anything else to return to the previous menu."<<endl;
        getline(cin,deleteall_selection_str);
        }while(emptyInput(deleteall_selection_str));
        char deleteall_selection = deleteall_selection_str.at(0);
        if(deleteall_selection == 'a'){
            bool deletionSuccess = recommenderDataHandler.deleteALLUserProfiles();
            if(deletionSuccess){
                cout<<"Deletion(s) successful. Press Enter to continue."<<endl;
            } else{
                cout<<"Deletion(s) were NOT successful! Press Enter to continue"<<endl;
            }
            cin.get();
        }
        return;
    }

    if(first_selection == 'x'){
        return;
    }

    cout<<"---"<<endl;
    string selection_str;
    do{
    cout<<"Select profile by inputting its corresponding number:";
    getline(cin,selection_str);
    }while(emptyInput(selection_str));
    char selection = selection_str.at(0);
    while(!isdigit(selection) || charToUint(selection) > showAllProfilesResult.max_selection_id){
        cout<<endl;
        do{
        cout<<"Invalid input, please try again:";
        getline(cin,selection_str);
        }while(emptyInput(selection_str));
        selection = selection_str.at(0);
        cout<<endl;
    }
    bool validSelection = false;
    UserProfileFull* selectedUserProfile = nullptr;
    while(!validSelection){
        validSelection = resolveProfileSelection(profileSelections,charToUint(selection),&selectedUserProfile);
        if(!validSelection){
            do{
            cout<<"Invalid input, please try again:";
            getline(cin,selection_str);
            }while(emptyInput(selection_str));
            selection = selection_str.at(0);
            cout<<endl;
        }
    }
    cout<<"Will delete profile: "<<selectedUserProfile->getProfileName()<<endl;
    bool deleteSuccess = recommenderDataHandler.deleteUserProfile(selectedUserProfile->getUserId());
    if(deleteSuccess){
        cout<<"Deletion successful. Press Enter to continue."<<endl;
    } else{
        cout<<"Deletion was NOT successful! Press Enter to continue."<<endl;
    }
    cin.get();
}

void editProfileMenu(UserProfileFull* targetProfile){

    multipleNewlines();

    if(targetProfile == nullptr){
        throw InvalidStateException("Profile to edit was invalid/empty.");
    }

    string selection_str;
    char selection;

    do{
    cout<<"*Edit Profile : "<<targetProfile->getProfileName()<<endl;
    cout<<"1.Add movie entry."<<endl;
    cout<<"2.Remove movie entry"<<endl;
    cout<<"3.Show all profile information."<<endl;
    cout<<"4.Change profile name"<<endl;
    cout<<"x.Back to previous menu"<<endl;

    getline(cin,selection_str);
    if(emptyInput(selection_str)){
        continue;
    }
    selection = selection_str.at(0);
    resolveEditProfileSelection(selection,targetProfile);
    }while(selection != 'x');
}

void resolveEditProfileSelection(char selection,UserProfileFull* targetProfile){

    switch(selection){
        case '1':
            editProfileAddMovieEntry(targetProfile);
            break;
        case '2':
            editProfileRemoveMovieEntry(targetProfile);
            break;
        case '3':
            editProfileShowAllMovieEntries(targetProfile);
            break;
        case '4':
            editProfileChangeProfileName(targetProfile);
            break;
    }
}

void editProfileAddMovieEntry(UserProfileFull* targetProfile){

    multipleNewlines();
    string mvId_selection_str;
    string rating_selection_str;
    bool mvIdExists = false;
    double rating_selection = 0;

    MovieEntry newMovieEntry;

    cout<<"*Add Movie Entry*"<<endl;

    //movieId
    do{
    cout<<"Input movieId (input 0 to cancel):";
    getline(cin,mvId_selection_str);
    if(emptyInput(mvId_selection_str)){
        continue;
    }

    if(mvId_selection_str == "0"){
        return;
    }

    if(strIsNumeric(mvId_selection_str)){
        mvIdExists = recommenderDataHandler.movieIdExists(strToUint(mvId_selection_str));
        if(!mvIdExists){
            cout<<"Movie ID:"<<mvId_selection_str<<" was not found in data! Please try again with another id."<<endl;
        }
    } else{
        cout<<"Only positive number inputs are valid! Please try again."<<endl;
        enterToContinue();
    }
    }while(!strIsNumeric(mvId_selection_str) || !mvIdExists);
    ulong newMovieId = strToUint(mvId_selection_str);
    newMovieEntry.movieId = newMovieId;

    //rating
    do{
    cout<<"Input rating:";
    getline(cin,rating_selection_str);
    if(emptyInput(rating_selection_str)){
        continue;
    }
    rating_selection = strtod(rating_selection_str.c_str(),nullptr);
    cout<<"Entered rating "<<rating_selection<<endl;
    }while(rating_selection <= 0);
    newMovieEntry.rating = rating_selection;

    //tags
    string new_tag_str;
    do{
    cout<<"Input tag (input 0 to stop entering tags):";
    getline(cin,new_tag_str);
    }while(new_tag_str!="0");

    TagEntry newTag;
    newTag.tagId = static_cast<unsigned long>(rand());
    newTag.tagName = new_tag_str;
    newMovieEntry.tags.push_back(newTag);

    if(targetProfile->isDataBound()){
        targetProfile->getDataBoundProfile().userData->movieEntries.push_back(newMovieEntry);
    } else{
        targetProfile->getStandaloneProfile().userData.movieEntries.push_back(newMovieEntry);
    }
    cout<<"Operation finished. Press Enter to continue.";
    cin.get();
}

void editProfileRemoveMovieEntry(UserProfileFull* targetProfile){

    multipleNewlines();
    string mvId_selection_str;
    ulong mvId_selection_num;
    bool validSelection = false;

    vector<MovieEntry> movieEntries;
    if(targetProfile->isDataBound()){
        if(!targetProfile->isWaitingForData()){
            movieEntries = targetProfile->getDataBoundProfile().userData->movieEntries;
        }
    } else{
        movieEntries = targetProfile->getStandaloneProfile().userData.movieEntries;
    }

    cout<<"*Remove Movie Entry*"<<endl;
    do{
    cout<<"movieEntries(ids):"<<endl;
    cout<<movieEntriesAsStr(movieEntries)<<endl;
    cout<<"Input movieId to remove (input 0 to cancel and return):";
    getline(cin,mvId_selection_str);
    if(emptyInput(mvId_selection_str)){
        continue;
    }
    if(mvId_selection_str == "0"){
        return;
    }
    validSelection = strIsNumeric(mvId_selection_str);
    if(!validSelection){
        cout<<"Only positive number inputs are valid! Please try again."<<endl;
        continue;
    }
    mvId_selection_num = strToUint(mvId_selection_str);
    bool mvIdExists = targetProfile->mvIdExistsInMvEntries(mvId_selection_num);
    validSelection = validSelection && mvIdExists;
    if(!mvIdExists){
        cout<<"Selected movie ID: "<<mvId_selection_str<<" was not found in the profile. Will not delete anything."<<endl;
        enterToContinue();
    }
    }while(!validSelection);
    bool deletionSuccess = targetProfile->removeMovieEntryFromProfile(mvId_selection_num);
    if(deletionSuccess){
        cout<<"Movie entry removed successfully."<<endl;
        enterToContinue();
    } else{
        cout<<"Could not remove movie entry!"<<endl;
        enterToContinue();
    }
}

void editProfileShowAllMovieEntries(UserProfileFull* targetProfile){

    multipleNewlines();
    cout<<"*Show All Movie Entries*"<<endl;
    string firstselect_str;
    char firstselect;
    do{
    cout<<"1.Show all movie entries for selected profile"<<endl;
    cout<<"2.Show all movie entries for another profile"<<endl;
    getline(cin,firstselect_str);
    if(emptyInput(firstselect_str)){
        continue;
    }
    firstselect = firstselect_str.at(0);
    }while(firstselect != '1' && firstselect != '2');
    if(firstselect == '2'){
        ShowAllProfilesWrapper showAllProfilesResult = showAllProfiles(false);
        vector<UserProfileSelection> profileSelections = bindProfilesToSelectionIds(showAllProfilesResult.userProfiles);
        cout<<"---"<<endl;
        string selection_str;
        do{
        cout<<"Select profile by inputting its corresponding number,to see its details:";
        getline(cin,selection_str);
        }while(emptyInput(selection_str));
        char selection = selection_str.at(0);
        while(!isdigit(selection) || charToUint(selection) > showAllProfilesResult.max_selection_id){
            do{
            cout<<endl;
            cout<<"Invalid input, please try again:";
            getline(cin,selection_str);
            }while(emptyInput(selection_str));
            selection = selection_str.at(0);
            cout<<endl;
        }
        bool validSelection = false;
        UserProfileFull* selectedUserProfile = nullptr;
        while(!validSelection){
            validSelection = resolveProfileSelection(profileSelections,charToUint(selection),&selectedUserProfile);
            if(!validSelection){
                do{
                cout<<"Invalid input, please try again:";
                getline(cin,selection_str);
                }while(emptyInput(selection_str));
                selection = selection_str.at(0);
                cout<<endl;
            }
        }
        showProfileDetails(selectedUserProfile);
    } else if(firstselect == '1'){
        showProfileDetails(targetProfile);
    }
}

void editProfileChangeProfileName(UserProfileFull* targetProfile){

    multipleNewlines();
    cout<<"*Change Profile Name*"<<endl;
    cout<<"Current profile name: "<<targetProfile->getProfileName()<<endl;
    string newProfileName;
    bool hasWhitespaces,isEmpty,isInvalid;
    do{
    cout<<"Enter the new profile name (no whitespaces):";
    getline(cin,newProfileName);
    isEmpty = emptyInput(newProfileName);
    hasWhitespaces = newProfileName.find(' ') != string::npos;
    isInvalid = isEmpty || hasWhitespaces;
    if(isEmpty){
        cout<<"Empty input. Try again!"<<endl;
        cout<<endl;
    }
    if(hasWhitespaces){
        cout<<"Whitespaces not allowed! Try again."<<endl;
        cout<<endl;
    }
    }while(isInvalid);
    targetProfile->setProfileName(newProfileName);
    cout<<"New profile name: "<<newProfileName<<endl;
    cout<<"Operation finished. Press Enter to continue."<<endl;
    cin.get();
}

vector<UserProfileSelection> bindProfilesToSelectionIds(vector<UserProfileFull>* userProfiles){
    vector<UserProfileSelection> profileSelections;
    for(uint i=0;i<userProfiles->size();i++){
         UserProfileSelection newSelection;
        newSelection.profile = &((*userProfiles)[i]);
        newSelection.selection_id = i+1;
        profileSelections.push_back(newSelection);
    }
    return profileSelections;
}

void showProfileDetails(UserProfileFull* targetProfile){
    multipleNewlines();
    cout<<"*PROFILE DETAILS*"<<endl;
    cout<<"- - - - - - - - - -"<<endl;
    cout<<"Name:"<<targetProfile->getProfileName()<<endl;
    cout<<"User ID:"<<targetProfile->getUserId()<<endl;
    cout<<"Databound:"<<booleanAsPrettyStr(targetProfile->isDataBound())<<endl;
    cout<<"Waiting for data:"<<booleanAsPrettyStr(targetProfile->isWaitingForData())<<endl;
    cout<<"---"<<endl;
    cout<<"User Details:"<<endl;
    cout<<"---"<<endl;
    if(targetProfile->isDataBound()){
        if(!targetProfile->isWaitingForData()){
            cout<<"meanRating:"<<targetProfile->getDataBoundProfile().userData->meanRating<<endl;
            cout<<"ratingsStandardDeviation:"<<targetProfile->getDataBoundProfile().userData->ratingsStandardDeviation<<endl;
        } else{
            cout<<"meanRating:[Undetermined,waiting for data.]"<<endl;
            cout<<"ratingsStandardDeviation:[Undetermined,waiting for data.]"<<endl;
        }
        if(!targetProfile->isWaitingForData()){
            cout<<"movieEntries:"<<movieEntriesAsStr(targetProfile->getDataBoundProfile().userData->movieEntries)<<endl;
            cout<<"neighbors(ids):"<<neighborsAsStrIdsOnly(targetProfile->getDataBoundProfile().userData->neighbors)<<endl;
        } else{
            cout<<"movieEntries:[Undetermined,waiting for data.]"<<endl;
            cout<<"neighbors(ids):[Undetermined,waiting for data.]"<<endl;
        }
    } else{
        if(!targetProfile->isWaitingForData()){
            cout<<"meanRating:"<<targetProfile->getStandaloneProfile().userData.meanRating<<endl;
            cout<<"ratingsStandardDeviation:"<<targetProfile->getStandaloneProfile().userData.ratingsStandardDeviation<<endl;
        } else{
            cout<<"meanRating:[Undetermined,waiting for data.]"<<endl;
            cout<<"ratingsStandardDeviation:[Undetermined,waiting for data.]"<<endl;
        }
        cout<<"movieEntries:"<<movieEntriesAsStr(targetProfile->getStandaloneProfile().userData.movieEntries)<<endl;
        if(!targetProfile->isWaitingForData()){
            cout<<"neighbors(ids):"<<neighborsAsStrIdsOnly(targetProfile->getStandaloneProfile().userData.neighbors)<<endl;
            cout<<"neighbors(weights):"<<neighborsAsStrWeightsOnly(targetProfile->getStandaloneProfile().userData.neighbors)<<endl;
        } else{
            cout<<"neighbors(ids):[Undetermined,waiting for data]."<<endl;
            cout<<"neighbors(weights):[Undetermined,waiting for data]."<<endl;
        }
    }
    cout<<endl;
    enterToContinue();
}

void saveProfiles(){
    multipleNewlines();
    if(config.profileStorageType == "FILE"){
        cout<<"Note: user profiles are automatically stored upon application termination in the file:"<<config.profileFilepath<<endl;
    }
    string filepath;
    do{
    cout<<"Input filepath (input 'cancel' to go to the previous menu):"<<endl;
    getline(cin,filepath);
    }while(emptyInput(filepath));
    if(filepath == "cancel"){
        return;
    }
    string prettyOrNot;
    do{
    cout<<"Pretty output? <y/n> (Note: pretty output may lead to larger output filesize.) :"<<endl;
    getline(cin,prettyOrNot);
    }while(emptyInput(prettyOrNot));
    bool pretty = (prettyOrNot == "y") || (prettyOrNot == "Y");
    recommenderDataHandler.saveUserProfiles(filepath,pretty);
    cout<<endl;
    cout<<"User profiles saved successfully to filepath="<<filepath<<endl;
    enterToContinue();
}

string movieEntriesAsStr(vector<MovieEntry> movieEntries){
    if(movieEntries.empty()){
        return "-";
    }
    string str= "";
    str += "\n";
    for(auto it=movieEntries.begin();it!=movieEntries.end();it++){
        str += "*";
        str += "-movieId:";
        str += to_string(it->movieId);
        str += "-movieName:";
        bool movieIdExistsInData = recommenderDataHandler.movieIdExists(it->movieId);
        if(movieIdExistsInData){
            string movieName = recommenderDataHandler.extractMovieFromMovieId(it->movieId).movieName;
            str += movieName;
        } else{
            str += "[Undetermined,no data]";
        }
        if(it->rated){
            str += "-rating:";
            str += to_string(it->rating);
        }
        if(it->tagged){
            str += "-tags:";
            str += tagsAsStr(it->tags);
        }
        str += "\n";
    }
    return str;
}

string userEntriesAsStr(vector<UserEntry> userEntries){
    if(userEntries.empty()){
        return "-";
    }
    string str = "";
    str += "\n";
    for(auto it=userEntries.begin();it!=userEntries.end();it++){
        str += "*";
        str += "-userId:";
        str += to_string(it->userId);
        if(it->rated){
            str += "\n-rating:";
            str += to_string(it->rating);
        }
        if(it->tagged){
            str += "\n-tags:";
            str += tagsAsStr(it->tags);
        }
        str += "\n";
    }
    return str;
}

string neighborsAsStrIdsOnly(vector<NeighborUser> neighborUsers){
    string str = "";
    for(size_t i=0;i<neighborUsers.size();i++){
        str += to_string(neighborUsers[i].id);
        if(i+1 < neighborUsers.size()){
            str += ",";
        }
    }
    return str;
}

string neighborsAsStrIdsOnly(vector<NeighborMovie> neighborMovies){
    string str = "";
    for(size_t i=0;i<neighborMovies.size();i++){
        str += to_string(neighborMovies[i].id);
        if(i+1 < neighborMovies.size()){
            str += ",";
        }
    }
    return str;
}

string neighborsAsStrWeightsOnly(vector<NeighborUser> neighborUsers){
    string str = "";
    for(size_t i=0;i<neighborUsers.size();i++){
        str += to_string(neighborUsers[i].weight);
        if(i+1 < neighborUsers.size()){
            str += ",";
        }
    }
    return str;
}

string neighborsAsStrWeightsOnly(vector<NeighborMovie> neighborMovies){
    string str = "";
    for(size_t i=0;i<neighborMovies.size();i++){
        str += to_string(neighborMovies[i].weight);
        if(i+1 < neighborMovies.size()){
            str += ",";
        }
    }
    return str;
}

string tagsAsStr(vector<TagEntry> tags){
    string str = "";
    for(uint i=0;i<tags.size();i++){
        str += tags[i].tagName;
        if(i+1 < tags.size()){
            str += ",";
        }
    }
    return str;
}

//expecting comma separated list of movie ids
vector<ulong> parseMovieIds(string movieIdsStr){
    vector<ulong> parsedMovieIds;
    stripLeadingAndTrailingWhitespaces(movieIdsStr);
    if(movieIdsStr.empty()){
        cout<<"-No movie ids were entered."<<endl;
        return parsedMovieIds;
    }
    vector<string> parsedTokens = splitAroundCharacter(movieIdsStr,',');
    for(auto it=parsedTokens.begin();it!=parsedTokens.end();it++){
        ulong pmvid = strtoul(it->c_str(),nullptr,10);
        if(pmvid == 0){
            cout<<"Warning: the inserted movie id: "<<*it<<" is not valid! It will be ignored."<<endl;
            continue;
        }
        parsedMovieIds.push_back(pmvid);
    }
    return parsedMovieIds;
}

//expecting comma separated list of ratings
vector<double> parseRatings(string ratingsStr){
    vector<double> parsedRatings;
    stripLeadingAndTrailingWhitespaces(ratingsStr);
    if(ratingsStr.empty()){
        return parsedRatings;
    }
    vector<string> parsedTokens = splitAroundCharacter(ratingsStr,',');
    for(auto it=parsedTokens.begin();it!=parsedTokens.end();it++){
        if((*it) == "-"){
            parsedRatings.push_back(0);
            continue;
        }
        double prt = strtod(it->c_str(),nullptr);
        if(realNumEqualWithinError(prt,0,0.000001) || prt < 0){
            cout<<"Warning: the inserted rating: "<<*it<<" is not valid! It will be ignored."<<endl;
            continue;
        }
        parsedRatings.push_back(prt);
    }
    return parsedRatings;
}

//expecting comma separated list of tags.TagIds are generated randomly. TODO: make this optional?
vector<TagEntry> parseTagEntries(string tagEntriesStr){
    vector<TagEntry> parsedTagEntries;
    stripLeadingAndTrailingWhitespaces(tagEntriesStr);
    if(tagEntriesStr.empty()){
        return parsedTagEntries;
    }
    vector<string> parsedTokens = splitAroundCharacter(tagEntriesStr,',');
    for(auto it=parsedTokens.begin();it!=parsedTokens.end();it++){
        //TODO: are tagCounts important here? probably not for the time being.
        TagEntry tgEntr;
        tgEntr.tagName = *it;
        tgEntr.tagId = recommenderDataHandler.autogenerateRandomTagId(rand());
        parsedTagEntries.push_back(tgEntr);
    }
    return parsedTagEntries;
}

string booleanAsPrettyStr(bool b){
    if(b==true){
        return "True";
    } else{
        return "False";
    }
}

void showUserDetails(User& targetUser){
    cout<<"* USER ID : "<<targetUser.userId<<" DETAILS *"<<endl;
    cout<<"* mean rating: "<<targetUser.meanRating<<endl;
    cout<<"* ratings standard deviation: "<<targetUser.ratingsStandardDeviation<<endl;
    cout<<"* movie entries: "<<movieEntriesAsStr(targetUser.movieEntries)<<endl;
    cout<<"* neighbors(ids): "<<neighborsAsStrIdsOnly(targetUser.neighbors)<<endl;
    cout<<"* neighbors(weights): "<<neighborsAsStrWeightsOnly(targetUser.neighbors)<<endl;
}

void showMovieDetails(Movie& targetMovie){
    cout<<"* MOVIE ID : "<<targetMovie.movieId<<" DETAILS *"<<endl;
    cout<<"* movie name: "<<targetMovie.movieName<<endl;
    cout<<"* genres: "<<vecStrAsCommaSeparatedStr(targetMovie.genres)<<endl;
    cout<<"* mean rating: "<<targetMovie.meanRating<<endl;
    cout<<"* ratings standard deviation: "<<targetMovie.ratingsStandardDeviation<<endl;
    cout<<"* user entries: "<<userEntriesAsStr(targetMovie.userEntries)<<endl;
    cout<<"* total tags: "<<tagsAsStr(targetMovie.totalTags)<<endl;
    cout<<"* neighbors(ids): "<<neighborsAsStrIdsOnly(targetMovie.neighbors)<<endl;
    cout<<"* neighbors(weights): "<<neighborsAsStrWeightsOnly(targetMovie.neighbors)<<endl;
}

string checkIfHasSignAndRemove(string value_str,bool* hasSign){
    string toReturn = "";
    if(value_str.empty()){
        return toReturn;
    }
    if(value_str.at(0) == '-'){
        *hasSign = true;
        toReturn = value_str.erase(0,1);
        return toReturn;
    }
    toReturn = value_str;
    *hasSign = false;
    return toReturn;
}

void storeParsedElements(JsonParseResultWrapper& parseResult){
    if(parseResult.parsedUsers.size()>0){
        auto it = next(parseResult.parsedUsers.begin(), parseResult.parsedUsers.size());
        move(parseResult.parsedUsers.begin(), it, back_inserter(recommenderDataHandler.getAllUsers()));
        vector<User>().swap(parseResult.parsedUsers);
    }
    if(parseResult.parsedMovies.size()>0){
        auto it = next(parseResult.parsedMovies.begin() , parseResult.parsedMovies.size());
        move(parseResult.parsedMovies.begin(),it,back_inserter(recommenderDataHandler.getAllMovies()));
        vector<Movie>().swap(parseResult.parsedMovies);
    }
}

void multipleNewlines() {
    for(size_t i =0;i<200;i++){
        cout<<"\n";
    }
}

void enterToContinue() {
    cout<<"Press Enter to continue."<<endl;
    cin.get();
}

void enterToContinue(string customMessage){
    cout<<customMessage<<endl;
    cin.get();
}

bool emptyInput(string in){
    return in.empty();
}
