#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "../../exceptions/recommenderExceptions.h"
#include <string>
#include <unordered_map>
#include <exception>
#include <vector>

class ConfigurationException : public RecommenderException {
	
	protected:
	
	std::string exceptionName = "ConfigurationException";
	
	public:
	
	//create with default message.
	ConfigurationException() {
            exceptionMessage = "An error occured while attempting to load configuration.";
	}
	
	//create with user-defined message.
	ConfigurationException(std::string message){
            exceptionMessage = message;
	}
	
	std::string getExceptionMessage(){
            return exceptionMessage;
	}
        
        //overrides
        std::string getExceptionName(){
            return exceptionName;
        }
};

class Configuration {

	private:
	
        std::vector<std::string> validPropertyNames;

        //this default can be overriden by cmdline option.
        std::string configLocation = "../movrec_config/";

        //logging
        std::unordered_map<std::string,long> configLoggingProperties;

        //profiles
        std::unordered_map<std::string,std::string> configUserProfileProperties;
	
        //boolean
        std::unordered_map<std::string, bool> configBooleanProperties;
        
        //numerical
        std::unordered_map<std::string, long> configNumericalProperties;
        std::unordered_map<std::string,double> configRealNumProperties;
        
        //parsing numerical
        std::unordered_map<std::string, long> configParsingNumericalProperties;
        std::unordered_map<std::string,double> configParsingRealNumProperties;
        
        //parsing str
        std::unordered_map<std::string,std::string> configParsingStrProperties;
	
        void parseConfigFromFile(std::string filepath, short id);
	
        void populateValidPropertyNames();
        std::string processPropertyStr(std::string propertyStr);
	
        bool processValueBoolean(std::string valueStr);
        long processValueNumerical(std::string valueStr);
        double processValueRealNum(std::string valueStr);
        long processValueParsingNumerical(std::string valueStr);
        std::string processValueParsingStr(std::string valueStr);
        std::string processValueProfileStr(std::string profileStr);
        long processValueLogging(std::string loggingStr);
	
        void processConfigLine(std::string configLine, short id);
	
        void populatePropertyValuesForBoolean();
        void populatePropertyValuesForNumerical();
        void populatePropertyValuesForParsing();
        void populatePropertyValuesForParsingStr();
        void populatePropertyValuesForUserProfiles();
        void populatePropertyValuesForLogging();
        void populatePropertyValues(short id);

        void updateAllProfileValues();
        void updateAllBoolValues();
        void updateAllNumericalValues();
        void updateAllParsingNumericalValues();
        void updateAllParsingStrValues();
        void updateAllLoggingValues();

	public:
	
        void overrideConfigLocation(std::string);

        void initConfig(short id);

        void initConfig(short id,std::string filepathPrefix);

        //default-constructor
        Configuration(){}
	
        //constructors:
        Configuration(short id); //1=boolean , 2=numerical , 3=parsing , 4=parsing_filepaths , 5=profiles
	
        Configuration(short id,std::string filepathPrefix);

        //
        void setConfigValueBool(std::string configPropertyName,bool value);
        void setConfigValueNumerical(std::string configPropertyName,long value);
        void setConfigValueNumerical(std::string configPropertyName,double value);
        void setConfigValueForProfiles(std::string configPropertyName,std::string value);
        void setConfigValueForParsingNumerical(std::string configPropertyName,long value);
        void setConfigValueForParsingNumerical(std::string configPropertyName,double value);
        void setConfigValueForParsingStr(std::string configPropertyName,std::string value);
        void setConfigValueForLogging(std::string configPropertyName,long value);
        //

        //LOGGING CONFIG VALUES
        //log levels: 1->ERROR,2->WARN,3->INFO,4->DEBUG,5->TRACE
        long recommendationLogLevel;
        long parsingLogLevel;
        long dataProceduresLogLevel;

        //USER PROFILE CONFIG VALUES
        std::string profileFilepath; //this path is used for both storing and loading profiles by default.
        std::string profileStorageType;
	
        //BOOLEAN CONFIG VALUES
        bool initDataAutomaticallyOnStartup;
        bool useDatabase;
        bool realTimeCalculations;
        bool useLimitInWatchedMoviesUserBasedMethod;
        bool useLimitInWatchedMoviesItemBasedMethod;
        bool useAdjustedCorrellationForItemSimilarities;
        bool useTagSimilarityInItemBasedMethod;
        bool discardNegativeWeightNeighbors;
        bool terminateApplicationOnCsvImportError;
        bool terminateApplicationOnJsonImportError;
        bool writeIntermediateMatricesToFile;
	
        //NUMERICAL CONFIG VALUES
        unsigned long maxNeighbors;
        double neighborWeightThreshold;
        unsigned long recommendationCount;
        unsigned long maxWatchedMovieCountUsedInItemBasedMethod;
        unsigned long maxWatchedMovieCountUsedInUserBasedMethod;
        unsigned long significanceWeightingFactor ;
        unsigned long stringSimilarityMethod;
        double similarityThreshold;
        unsigned long minimumCommonElementsForUserNeighbors;
        unsigned long minimumCommonElementsForMovieNeighbors;
        unsigned long minimumRatingCountForAggregations;
	
        //PARSING CONFIG VALUES
        unsigned long movieNameMaxLength;
        unsigned long tagNameMaxLength;
        unsigned long genreNameMaxLength;
        unsigned long maxGenreCount;
        unsigned long maxTagsPerUser;
        double userEntriesReallocFactor;
        double tagsReallocFactor;
        long automaticMultithread;
        long multithreadParseRatingsEnabled;
        long multithreadParseOtherEnabled;
        long multithreadSearchEnabled;
        unsigned long defaultThreadcount;
        std::string moviesFileDefaultPath;
        std::string ratingsFileDefaultPath;
        std::string tagEntriesFileDefaultPath;
        std::string tagIdsFileDefaultPath; //formerly->genomeTagsFile
	
        //the active values for the paths. Using separate variables for active paths because they can be set
        //manually instead of from file.
        std::string moviesFileActivePath;
        std::string ratingsFileActivePath;
        std::string tagEntriesFileActivePath;
        std::string tagIdsFileActivePath; //formerly->genomeTagsFile
};

#endif
