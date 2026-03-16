#include "configuration.h"

#include "../../utils/utils.h"
#include "../../logging/recommenderLogger.h"
#include "../../math/mathUtils.h"

#include <fstream>
#include <algorithm>
#include <iostream>

using namespace std;
typedef unsigned long ulong;

static RecommenderLogger LOG;

void initLog(){
    LOG.init("configuration");
}

void Configuration::overrideConfigLocation(std::string configLocation){
    this->configLocation = configLocation;
}

void Configuration::initConfig(short id,std::string filepathPrefix){
    
    initLog();
    
    LOG.info("START: initConfig");
	
	string configFileName;

	switch(id){
		case 1:
            configFileName=filepathPrefix+"CONFIGURATION_BOOLEAN.txt";
			break;
		case 2:
            configFileName=filepathPrefix+"CONFIGURATION_NUMERIC.txt";
			break;
		case 3:
            configFileName=filepathPrefix+"CONFIGURATION_PARSING.txt";
			break;
		case 4:
            configFileName=filepathPrefix+"CONFIGURATION_PARSING_FILEPATHS.txt";
			break;
        case 5:
            configFileName=filepathPrefix+"CONFIGURATION_PROFILES.txt";
            break;
        case 6:
            configFileName=filepathPrefix+"CONFIGURATION_LOGGING.txt";
            break;
		default:
			configFileName="";
			break;
	}
	
    populateValidPropertyNames(); //populate all expected property names.
	parseConfigFromFile(configFileName,id); //parse the configuration file and store in memory the values for each property
	populatePropertyValues(id); //obtain the property-value pairs from memory to populate the configuration object variables.
	
    LOG.info("initConfig finished.");
    LOG.close();
}

void Configuration::initConfig(short id){
	
    initConfig(id,configLocation);
}

Configuration::Configuration(short id) {

	if (id == -1){
		return;
	}
	initConfig(id);
}

Configuration::Configuration(short id,std::string filepathPrefix){
	
	initConfig(id,filepathPrefix);
}

//all valid property names are populated 'manually' here.
void Configuration::populateValidPropertyNames(){
    
    LOG.debug("START: populateValidPropertyNames");

    //USER PROFILE VALUES
    validPropertyNames.push_back("profileFilepath");
    validPropertyNames.push_back("profileStorageType");
	
	//BOOLEAN CONFIG VALUES
    validPropertyNames.push_back("initDataAutomaticallyOnStartup");
	validPropertyNames.push_back("useDatabase");
	validPropertyNames.push_back("realTimeCalculations");
	validPropertyNames.push_back("useLimitInWatchedMoviesUserBasedMethod");
	validPropertyNames.push_back("useLimitInWatchedMoviesItemBasedMethod");
    validPropertyNames.push_back("useAdjustedCorrellationForItemSimilarities");
    validPropertyNames.push_back("useTagSimilarityInItemBasedMethod");
    validPropertyNames.push_back("discardNegativeWeightNeighbors");
    validPropertyNames.push_back("terminateApplicationOnCsvImportError");
    validPropertyNames.push_back("terminateApplicationOnJsonImportError");
    validPropertyNames.push_back("writeIntermediateMatricesToFile");

	//NUMERICAL CONFIG VALUES
	validPropertyNames.push_back("maxNeighbors");
	validPropertyNames.push_back("neighborWeightThreshold");
	validPropertyNames.push_back("recommendationCount");
	validPropertyNames.push_back("maxWatchedMovieCountUsedInItemBasedMethod");
	validPropertyNames.push_back("maxWatchedMovieCountUsedInUserBasedMethod");
	validPropertyNames.push_back("significanceWeightingFactor");
    validPropertyNames.push_back("stringSimilarityMethod");
    validPropertyNames.push_back("similarityThreshold");
    validPropertyNames.push_back("minimumCommonElementsForUserNeighbors");
    validPropertyNames.push_back("minimumCommonElementsForMovieNeighbors");
    validPropertyNames.push_back("minimumRatingCountForAggregations");

	//PARSING CONFIG VALUES
	validPropertyNames.push_back("movieNameMaxLength");
	validPropertyNames.push_back("tagNameMaxLength");
	validPropertyNames.push_back("genreNameMaxLength");
	validPropertyNames.push_back("maxGenreCount");
	validPropertyNames.push_back("maxTagsPerUser");
	validPropertyNames.push_back("userEntriesReallocFactor");
	validPropertyNames.push_back("tagsReallocFactor");
	validPropertyNames.push_back("automaticMultithread");
	validPropertyNames.push_back("multithreadParseRatingsEnabled");
	validPropertyNames.push_back("multithreadParseOtherEnabled");
	validPropertyNames.push_back("multithreadSearchEnabled");
	validPropertyNames.push_back("defaultThreadcount");
	validPropertyNames.push_back("moviesFileDefaultPath");
	validPropertyNames.push_back("ratingsFileDefaultPath");
    validPropertyNames.push_back("tagEntriesFileDefaultPath");
    validPropertyNames.push_back("tagIdsFileDefaultPath");

    //LOGGING CONFIG VALUES
    validPropertyNames.push_back("recommendationLogLevel");
    validPropertyNames.push_back("parsingLogLevel");
    validPropertyNames.push_back("dataProceduresLogLevel");
}

string Configuration::processPropertyStr(string propertyStr){
    
    LOG.debug("START: processPropertyStr");

	string foundPropertyStr;
	
    for(size_t i=0;i<validPropertyNames.size();i++){
		
		auto it = std::find(validPropertyNames.begin(),validPropertyNames.end(),propertyStr);
		if(it == validPropertyNames.end()){
			string exMsg = "Unrecognized property name found : "+propertyStr;
			throw ConfigurationException(exMsg);
		}
		foundPropertyStr = *it;
	}
	return foundPropertyStr;
}

bool Configuration::processValueBoolean(string valueStr){
    
    LOG.debug("START: processValueBoolean");

    if(valueStr == "true"){
		return true;
	}
	else if(valueStr == "false"){
		return false;
	}
	else{
		string exMsg = "Unrecognized property value :"+valueStr+" . Recognized property values are : 'true' or 'false' ." ;
		throw ConfigurationException(exMsg);
	}
}

long Configuration::processValueNumerical(string valueStr){
	return stol(valueStr);
}

double Configuration::processValueRealNum(string valueStr){
    return stod(valueStr);
}

long Configuration::processValueParsingNumerical(string valueStr){
	return stol(valueStr);
}

string Configuration::processValueParsingStr(string valueStr){
	return valueStr;
}

string Configuration::processValueProfileStr(string valueStr){
    return valueStr;
}

long Configuration::processValueLogging(string valueStr){
    return stol(valueStr);
}

void Configuration::processConfigLine(string configLine,short id){
    
    LOG.debug("START: processConfigLine. configLine:"+configLine);
    
    stripLeadingAndTrailingWhitespaces(configLine);
    if(configLine.empty()){
        LOG.trace("Encountered empty configLine, will ignore.");
        return;
    }
	
    LOG.trace("Splitting configLine around '='");
	//split around '='
	vector<string> configWords = splitAroundCharacter(configLine,'=');
	if(configWords.size() != 2){
		string exMsg = "Invalid format in configuration line :"+configLine+" . Recognized format is: [property name]=[property value]";
		throw ConfigurationException(exMsg);
	}

	string propertyStr = configWords[0];
	string valueStr = configWords[1];
    
    //remove leading and trailing whitespaces.
    stripLeadingAndTrailingWhitespaces(propertyStr);
    stripLeadingAndTrailingWhitespaces(valueStr);
    
    LOG.trace("Parsed property string:"+propertyStr);
    LOG.trace("Parsed value string:"+valueStr);

	bool valueStrBool;
	long valueStrLong;
    double valueStrDouble;
	
	propertyStr = processPropertyStr(propertyStr);

	pair<string,bool> pbool;
	pair<string, long> plong;
    pair<string,double> pdouble;
	pair<string, string> pstr;
    
    size_t searchResult;
    string exMsg;
	
	switch(id){
        //boolean
		case 1:
			valueStrBool = processValueBoolean(valueStr);
			pbool = make_pair(propertyStr, valueStrBool);
			configBooleanProperties.insert(pbool);
			break;
        //numeric
		case 2:
            searchResult = valueStr.find(".");
            if(searchResult == string::npos){ //'.' NOT found: integer.
                valueStrLong = processValueNumerical(valueStr);
                LOG.trace("'.' was not detected in value string, will parse it as integer= "+to_string(valueStrLong));
                if(valueStrLong == 0){
                    exMsg = "Invalid zero value detected in property: "+propertyStr;
                    throw ConfigurationException(exMsg);
                }
                plong = make_pair(propertyStr, valueStrLong);
                configNumericalProperties.insert(plong);
            } else{ // '.' found: real number.
                valueStrDouble = processValueRealNum(valueStr);
                LOG.trace("'.' detected in value string, will parse it as real number= "+to_string(valueStrDouble));
                if(realNumEqualWithinError(valueStrDouble,0.0,0.001)){
                    exMsg = "Invalid zero value detected in property: "+propertyStr;
                    throw ConfigurationException(exMsg);
                }
                pdouble = make_pair(propertyStr,valueStrDouble);
                configRealNumProperties.insert(pdouble);
            }
			break;
        //parsing_numeric
		case 3:
            searchResult = valueStr.find(".");
            if(searchResult == string::npos){ //'.' NOT found: integer.
                valueStrLong = processValueNumerical(valueStr);
                LOG.trace("'.' was not detected in value string, will parse it as integer= "+to_string(valueStrLong));
                plong = make_pair(propertyStr, valueStrLong);
                configParsingNumericalProperties.insert(plong);
            } else{ // '.' found: real number.
                valueStrDouble = processValueRealNum(valueStr);
                LOG.trace("'.' detected in value string, will parse it as real number= "+to_string(valueStrDouble));
                pdouble = make_pair(propertyStr,valueStrDouble);
                configParsingRealNumProperties.insert(pdouble);
            }
			break;
        //parsing_str
		case 4:
			valueStr = processValueParsingStr(valueStr);
		    pstr = make_pair(propertyStr,valueStr);
            if(valueStr.empty()){
                string exMsg = "Invalid empty value field detected in property: "+propertyStr;
                throw ConfigurationException(exMsg);
            }
			configParsingStrProperties.insert(pstr);
			break;
        //profile
        case 5:
            valueStr = processValueProfileStr(valueStr);
            pstr = make_pair(propertyStr,valueStr);
            if(valueStr.empty()){
                string exMsg = "Invalid empty value field detected in property: "+propertyStr;
                throw ConfigurationException(exMsg);
            }
            configUserProfileProperties.insert(pstr);
            break;
        //logging
        case 6:
            valueStrLong = processValueLogging(valueStr);
            plong = make_pair(propertyStr,valueStrLong);
            if(valueStr.empty()){
                string exMsg = "Invalid empty value field detected in property: "+propertyStr;
                throw ConfigurationException(exMsg);
            }
            configLoggingProperties.insert(plong);
            break;
		default:
            string exMsg = "Unexpected configuration 'id' category.";
			throw ConfigurationException(exMsg);
	}

}

void Configuration::parseConfigFromFile(string filepath,short id){
    
    LOG.debug("START: parseConfigFromFile. filepath="+filepath);
	
	ifstream configFile;
	const char* filepath_c = filepath.c_str();
	configFile.open(filepath_c); 
	if(!configFile){
        throw ConfigurationException("Could not find configuration file in path: "+filepath);
    }
	string configLine;
	
	if (configFile.is_open()){
		while ( getline (configFile,configLine) ){
            LOG.trace("Parsed config line = "+configLine+". Will now attempt to process it.");
			processConfigLine(configLine, id);
		}
    configFile.close();
    }
}

void Configuration::populatePropertyValuesForBoolean(){
    
    LOG.debug("START: populatePropertyValuesForBoolean");
    
    //boolean properties

	auto useDatabaseIter = configBooleanProperties.find("useDatabase");
	if(useDatabaseIter == configBooleanProperties.end()){
		string exMsg = "Could not set property: useDatabase. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		useDatabase = useDatabaseIter->second;
	}

	auto realTimeCalculationsIter = configBooleanProperties.find("realTimeCalculations");
	if(realTimeCalculationsIter == configBooleanProperties.end()){
		string exMsg = "Could not set property: realTimeCalculations. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else {
		realTimeCalculations = realTimeCalculationsIter->second;
	}

	auto useLimitInWatchedMoviesUserBasedMethodIter = configBooleanProperties.find("useLimitInWatchedMoviesUserBasedMethod");
	if(useLimitInWatchedMoviesUserBasedMethodIter == configBooleanProperties.end()){
		string exMsg = "Could not set property: useLimitInWatchedMoviesUserBasedMethod. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		useLimitInWatchedMoviesUserBasedMethod = useLimitInWatchedMoviesUserBasedMethodIter->second;
	}

	auto useLimitInWatchedMoviesItemBasedMethodIter = configBooleanProperties.find("useLimitInWatchedMoviesItemBasedMethod");
	if(useLimitInWatchedMoviesItemBasedMethodIter == configBooleanProperties.end()){
		string exMsg = "Could not set property: useLimitInWatchedMoviesItemBasedMethod. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		useLimitInWatchedMoviesItemBasedMethod = useLimitInWatchedMoviesItemBasedMethodIter->second;
	}

	auto useAdjustedCorrellationForItemSimilaritiesIter = configBooleanProperties.find("useAdjustedCorrellationForItemSimilarities");
	if(useAdjustedCorrellationForItemSimilaritiesIter == configBooleanProperties.end()){
		string exMsg = "Could not set property: useAdjustedCorrellationForItemSimilarities. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		useAdjustedCorrellationForItemSimilarities = useAdjustedCorrellationForItemSimilaritiesIter->second;
	}

    auto useTagSimilarityInItemBasedMethodIter = configBooleanProperties.find("useTagSimilarityInItemBasedMethod");
    if(useTagSimilarityInItemBasedMethodIter == configBooleanProperties.end()){
        string exMsg = "Could not set property: useTagSimilarityInItemBasedMethod. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        useTagSimilarityInItemBasedMethod = useTagSimilarityInItemBasedMethodIter->second;
    }

    auto discardNegativeWeightNeighborsIter = configBooleanProperties.find("discardNegativeWeightNeighbors");
    if(discardNegativeWeightNeighborsIter == configBooleanProperties.end()){
        string exMsg = "Could not set property: discardNegativeWeightNeighbors. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        discardNegativeWeightNeighbors = discardNegativeWeightNeighborsIter->second;
    }

    auto initDataAutomaticallyOnStartupIter = configBooleanProperties.find("initDataAutomaticallyOnStartup");
    if(initDataAutomaticallyOnStartupIter == configBooleanProperties.end()){
        string exMsg = "Could not set property: initDataAutomaticallyOnStartup. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        initDataAutomaticallyOnStartup = initDataAutomaticallyOnStartupIter->second;
    }

    auto writeIntermediateMatricesToFileIter = configBooleanProperties.find("writeIntermediateMatricesToFile");
    if(writeIntermediateMatricesToFileIter == configBooleanProperties.end()){
        string exMsg = "Could not set property: writeIntermediateMatricesToFile. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        writeIntermediateMatricesToFile = writeIntermediateMatricesToFileIter->second;
    }

    auto terminateApplicationOnCsvImportErrorIter = configBooleanProperties.find("terminateApplicationOnCsvImportError");
    if(terminateApplicationOnCsvImportErrorIter == configBooleanProperties.end()){
        string exMsg = "Could not set property: terminateApplicationOnCsvImportError. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        terminateApplicationOnCsvImportError = terminateApplicationOnCsvImportErrorIter->second;
    }

    auto terminateApplicationOnJsonImportErrorIter = configBooleanProperties.find("terminateApplicationOnJsonImportError");
    if(terminateApplicationOnJsonImportErrorIter == configBooleanProperties.end()){
        string exMsg = "Could not set property: terminateApplicationOnJsonImportError. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        terminateApplicationOnJsonImportError = terminateApplicationOnJsonImportErrorIter->second;
    }
    
}
void Configuration::populatePropertyValuesForNumerical(){
   
    LOG.debug("START: populatePropertyValuesForNumerical");
   
    //numerical properties

    auto minimumRatingCountForAggregationsIter = configNumericalProperties.find("minimumRatingCountForAggregations");
    if(minimumRatingCountForAggregationsIter == configNumericalProperties.end()){
        string exMsg = "Could not set property: minimumRatingCountForAggregations. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        if(minimumRatingCountForAggregationsIter->second <= 0){
            string exMsg = "minimumRatingCountForAggregations has an invalid value: "+to_string(minimumRatingCountForAggregationsIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        minimumRatingCountForAggregations = static_cast<ulong>(minimumRatingCountForAggregationsIter->second);
    }

    auto maxNeighborsIter = configNumericalProperties.find("maxNeighbors");
    if(maxNeighborsIter == configNumericalProperties.end()){
        string exMsg = "Could not set property: maxNeighbors. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        if(maxNeighborsIter->second <= 0){
            string exMsg = "maxNeighbors has an invalid value: "+to_string(maxNeighborsIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        maxNeighbors = static_cast<ulong>(maxNeighborsIter->second);
    }

    auto neighborWeightThresholdIter = configRealNumProperties.find("neighborWeightThreshold");
    if(neighborWeightThresholdIter == configRealNumProperties.end()){
        string exMsg = "Could not set property: neighborWeightThreshold. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
		neighborWeightThreshold = neighborWeightThresholdIter->second;
	}

	auto recommendationCountIter = configNumericalProperties.find("recommendationCount");
	if(recommendationCountIter == configNumericalProperties.end()){
		string exMsg = "Could not set property: recommendationCount. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(recommendationCountIter->second <=0){
            string exMsg = "recommendationCount has an invalid value: "+to_string(recommendationCountIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        recommendationCount = static_cast<ulong>(recommendationCountIter->second);
	}

	auto maxWatchedMovieCountUsedInItemBasedMethodIter = configNumericalProperties.find("maxWatchedMovieCountUsedInItemBasedMethod");
	if(maxWatchedMovieCountUsedInItemBasedMethodIter == configNumericalProperties.end()){
		string exMsg = "Could not set property: maxWatchedMovieCountUsedInItemBasedMethod. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(maxWatchedMovieCountUsedInItemBasedMethodIter->second <= 0){
            string exMsg = "maxWatchedMovieCountUsedInItemBasedMethod has an invalid value: "+to_string(maxWatchedMovieCountUsedInItemBasedMethodIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        maxWatchedMovieCountUsedInItemBasedMethod = static_cast<ulong>(maxWatchedMovieCountUsedInItemBasedMethodIter->second);
	}

	auto maxWatchedMovieCountUsedInUserBasedMethodIter = configNumericalProperties.find("maxWatchedMovieCountUsedInUserBasedMethod");
	if(maxWatchedMovieCountUsedInUserBasedMethodIter == configNumericalProperties.end()){
		string exMsg = "Could not set property: maxWatchedMovieCountUsedInUserBasedMethod. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(maxWatchedMovieCountUsedInUserBasedMethodIter->second <= 0){
            string exMsg = "maxWatchedMovieCountUsedInUserBasedMethod has an invalid value: "+to_string(maxWatchedMovieCountUsedInUserBasedMethodIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        maxWatchedMovieCountUsedInUserBasedMethod = static_cast<ulong>(maxWatchedMovieCountUsedInUserBasedMethodIter->second);
	}

    auto significanceWeightingFactorIter = configNumericalProperties.find("significanceWeightingFactor");
    if(significanceWeightingFactorIter == configNumericalProperties.end()){
		string exMsg = "Could not set property: significanceWeightingFactor. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(significanceWeightingFactorIter->second <=0){
            string exMsg = "significanceWeightingFactor has an invalid value: "+to_string(significanceWeightingFactorIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        significanceWeightingFactor = static_cast<ulong>(significanceWeightingFactorIter->second);
    }

    auto stringSimilarityMethodIter = configNumericalProperties.find("stringSimilarityMethod");
    if(stringSimilarityMethodIter == configNumericalProperties.end()){
        string exMsg = "Could not set property: stringSimilarityMethod. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        if(stringSimilarityMethodIter->second <=0){
            string exMsg = "stringSimilarityMethod has an invalid value: "+to_string(stringSimilarityMethodIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        stringSimilarityMethod = static_cast<ulong>(stringSimilarityMethodIter->second);
    }

    auto similarityThresholdIter = configRealNumProperties.find("similarityThreshold");
    if(similarityThresholdIter == configRealNumProperties.end()){
        string exMsg = "Could not set property: similarityThreshold. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        if(similarityThresholdIter->second <=0){
            string exMsg = "similarityThreshold has an invalid value: "+to_string(similarityThresholdIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        similarityThreshold = static_cast<double>(similarityThresholdIter->second);
    }

    auto minimumCommonElementsForUserNeighborsIter = configNumericalProperties.find("minimumCommonElementsForUserNeighbors");
    if(minimumCommonElementsForUserNeighborsIter == configNumericalProperties.end()){
        string exMsg = "Could not set property: minimumCommonElementsForUserNeighbors. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        if(minimumCommonElementsForUserNeighborsIter->second <=0){
            string exMsg = "minimumCommonElementsForUserNeighbors has an invalid value: "+to_string(minimumCommonElementsForUserNeighborsIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        minimumCommonElementsForUserNeighbors = static_cast<ulong>(minimumCommonElementsForUserNeighborsIter->second);
    }

    auto minimumCommonElementsForMovieNeighborsIter = configNumericalProperties.find("minimumCommonElementsForMovieNeighbors");
    if(minimumCommonElementsForMovieNeighborsIter == configNumericalProperties.end()){
        string exMsg = "Could not set property: minimumCommonElementsForMovieNeighbors. Its value was not found in memory after parsing the configuration file.";
      throw ConfigurationException(exMsg);
    } else{
        if(minimumCommonElementsForMovieNeighborsIter->second <=0){
            string exMsg = "minimumCommonElementsForMovieNeighbors has an invalid value: "+to_string(minimumCommonElementsForMovieNeighborsIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        minimumCommonElementsForMovieNeighbors = static_cast<ulong>(minimumCommonElementsForMovieNeighborsIter->second);
    }

}
void Configuration::populatePropertyValuesForParsing(){

    LOG.debug("START: populatePropertyValuesForParsing");
    
    //parsing numerical properties

	auto movieNameMaxLengthIter = configParsingNumericalProperties.find("movieNameMaxLength");
	if(movieNameMaxLengthIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: movieNameMaxLength. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(movieNameMaxLengthIter->second <= 0){
            string exMsg = "movieNameMaxLength has an invalid value: "+to_string(movieNameMaxLengthIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        movieNameMaxLength = static_cast<ulong>(movieNameMaxLengthIter->second);
	}

	auto tagNameMaxLengthIter = configParsingNumericalProperties.find("tagNameMaxLength");
	if(tagNameMaxLengthIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: tagNameMaxLength. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(tagNameMaxLengthIter->second <= 0){
            string exMsg = "tagNameMaxLength has an invalid value: "+to_string(tagNameMaxLengthIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        tagNameMaxLength = static_cast<ulong>(tagNameMaxLengthIter->second);
	}

	auto genreNameMaxLengthIter = configParsingNumericalProperties.find("genreNameMaxLength");
	if(genreNameMaxLengthIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: genreNameMaxLength. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(genreNameMaxLengthIter->second <= 0){
            string exMsg = "genreNameMaxLength has an invalid value: "+to_string(genreNameMaxLengthIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        genreNameMaxLength = static_cast<ulong>(genreNameMaxLengthIter->second);
	}

	auto maxGenreCountIter = configParsingNumericalProperties.find("maxGenreCount");
	if(maxGenreCountIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: maxGenreCount. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(maxGenreCountIter->second <= 0){
            string exMsg = "maxGenreCount has an invalid value: "+to_string(maxGenreCountIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        maxGenreCount = static_cast<ulong>(maxGenreCountIter->second);
	}

	auto maxTagsPerUserIter = configParsingNumericalProperties.find("maxTagsPerUser");
	if(maxTagsPerUserIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: maxTagsPerUser. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(maxTagsPerUserIter->second <= 0){
            string exMsg = "maxTagsPerUser has an invalid value: "+to_string(maxTagsPerUserIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        maxTagsPerUser = static_cast<ulong>(maxTagsPerUserIter->second);
	}

	auto userEntriesReallocFactorIter = configParsingRealNumProperties.find("userEntriesReallocFactor");
	if(userEntriesReallocFactorIter == configParsingRealNumProperties.end()){
		string exMsg = "Could not set property: userEntriesReallocFactor. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(userEntriesReallocFactorIter->second <= 0){
            string exMsg = "userEntriesReallocFactor has an invalid value: "+to_string(userEntriesReallocFactorIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        userEntriesReallocFactor = static_cast<double>(userEntriesReallocFactorIter->second);
	}

	auto tagsReallocFactorIter = configParsingRealNumProperties.find("tagsReallocFactor");
	if(tagsReallocFactorIter == configParsingRealNumProperties.end()){
		string exMsg = "Could not set property: tagsReallocFactor. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(tagsReallocFactorIter->second <=0 ){
            string exMsg = "tagsReallocFactor has an invalid value: "+to_string(tagsReallocFactorIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        tagsReallocFactor = static_cast<double>(tagsReallocFactorIter->second);
	}

	auto automaticMultithreadIter = configParsingNumericalProperties.find("automaticMultithread");
	if(automaticMultithreadIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: automaticMultithread. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		automaticMultithread = automaticMultithreadIter->second;
	}

	auto multithreadParseRatingsEnabledIter = configParsingNumericalProperties.find("multithreadParseRatingsEnabled");
	if(multithreadParseRatingsEnabledIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: multithreadParseRatingsEnabled. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		multithreadParseRatingsEnabled = multithreadParseRatingsEnabledIter->second;
	}

	auto multithreadParseOtherEnabledIter = configParsingNumericalProperties.find("multithreadParseOtherEnabled");
	if(multithreadParseOtherEnabledIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: multithreadParseOtherEnabled. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		multithreadParseOtherEnabled = multithreadParseOtherEnabledIter->second;
	}

	auto multithreadSearchEnabledIter = configParsingNumericalProperties.find("multithreadSearchEnabled");
	if(multithreadSearchEnabledIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: multithreadSearchEnabled. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		multithreadSearchEnabled = multithreadSearchEnabledIter->second;
	}

	auto defaultThreadcountIter = configParsingNumericalProperties.find("defaultThreadcount");
	if(defaultThreadcountIter == configParsingNumericalProperties.end()){
		string exMsg = "Could not set property: defaultThreadcount. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        if(defaultThreadcountIter->second <= 0){
            string exMsg = "defaultThreadcount has an invalid value: "+to_string(defaultThreadcountIter->second)+". The value should be a positive number.";
            throw ConfigurationException(exMsg);
        }
        defaultThreadcount = static_cast<ulong>(defaultThreadcountIter->second);
	}
}
void Configuration::populatePropertyValuesForParsingStr(){
    
    LOG.debug("START: populatePropertyValuesForParsingStr");
    
    //parsing text properties

	auto moviesFileDefaultPathIter = configParsingStrProperties.find("moviesFileDefaultPath");
	if(moviesFileDefaultPathIter == configParsingStrProperties.end()){
		string exMsg = "Could not set property: moviesFileDefaultPath. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		moviesFileDefaultPath = moviesFileDefaultPathIter->second;
	}

	auto ratingsFileDefaultPathIter = configParsingStrProperties.find("ratingsFileDefaultPath");
	if(ratingsFileDefaultPathIter == configParsingStrProperties.end()){
		string exMsg = "Could not set property: ratingsFileDefaultPath. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
		ratingsFileDefaultPath = ratingsFileDefaultPathIter->second;
	}

    auto tagEntriesFileDefaultPathIter = configParsingStrProperties.find("tagEntriesFileDefaultPath");
    if(tagEntriesFileDefaultPathIter == configParsingStrProperties.end()){
        string exMsg = "Could not set property: tagEntriesFileDefaultPath. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        tagEntriesFileDefaultPath = tagEntriesFileDefaultPathIter->second;
	}

    auto tagIdsFileDefaultPathIter = configParsingStrProperties.find("tagIdsFileDefaultPath");
    if(tagIdsFileDefaultPathIter == configParsingStrProperties.end()){
        string exMsg = "Could not set property: tagIdsFileDefaultPath. Its value was not found in memory after parsing the configuration file.";
		throw ConfigurationException(exMsg);
	} else{
        tagIdsFileDefaultPath = tagIdsFileDefaultPathIter->second;
	}
}

void Configuration::populatePropertyValuesForUserProfiles(){

    LOG.debug("START populatePropertyValuesForUserProfiles");

    auto profilesFilepathIter = configUserProfileProperties.find("profileFilepath");
    if(profilesFilepathIter == configUserProfileProperties.end()){
        string exMsg = "Could not set property: profileFilepath. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        //if already populated, it is overriden by cmdline options, so do not populate.
        if(profileFilepath.empty()){
            profileFilepath = profilesFilepathIter->second;
        }
    }

    auto profileStorageTypeIter = configUserProfileProperties.find("profileStorageType");
    if(profileStorageTypeIter == configUserProfileProperties.end()){
        string exMsg = "Could not set property: profileStorageType. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        //if already populated, it is overriden by cmdline options, so do not populate.
        if(profileStorageType.empty()){
            profileStorageType = profileStorageTypeIter->second;
            //profileStorageType should be uppercase.
            transform(profileStorageType.begin(),profileStorageType.end(),profileStorageType.begin(),::toupper);
        }
    }
}

void Configuration::populatePropertyValuesForLogging(){

    LOG.debug("START populatePropertyValuesForLogging");

    auto recommendationLogLevelIter = configLoggingProperties.find("recommendationLogLevel");
    if(recommendationLogLevelIter == configLoggingProperties.end()){
        string exMsg = "Could not set property: recommendationLogLevel. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        recommendationLogLevel = recommendationLogLevelIter->second;
    }

    auto parsingLogLevelIter = configLoggingProperties.find("parsingLogLevel");
    if(parsingLogLevelIter == configLoggingProperties.end()){
        string exMsg = "Could not set property: recommendationLogLevel. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        parsingLogLevel = parsingLogLevelIter->second;
    }

    auto dataProceduresLogLevelIter = configLoggingProperties.find("dataProceduresLogLevel");
    if(dataProceduresLogLevelIter == configLoggingProperties.end()){
        string exMsg = "Could not set property: recommendationLogLevel. Its value was not found in memory after parsing the configuration file.";
        throw ConfigurationException(exMsg);
    } else{
        dataProceduresLogLevel = dataProceduresLogLevelIter->second;
    }
}

void Configuration::populatePropertyValues(short id) {
    
    LOG.debug("START: populatePropertyValues");

    switch(id){
        case 1:
			LOG.debug("Will populate property values for boolean.");
			populatePropertyValuesForBoolean();
			break;
		case 2:
			LOG.debug("Will populate property values for numerical.");
			populatePropertyValuesForNumerical();
			break;
		case 3:
			LOG.debug("Will populate property values for parsing.");
			populatePropertyValuesForParsing();
			break;
		case 4:
			LOG.debug("Will populate property values for parsing paths/text.");
			populatePropertyValuesForParsingStr();
			break;
        case 5:
            LOG.debug("Will populate property values for user profiles.");
            populatePropertyValuesForUserProfiles();
            break;
        case 6:
            LOG.debug("Will populate property values for logging.");
            populatePropertyValuesForLogging();
            break;
		default:
            LOG.debug("Will populate ALL property values.");
			populatePropertyValuesForBoolean();
			populatePropertyValuesForNumerical();
			populatePropertyValuesForParsing();
			populatePropertyValuesForParsingStr();
            populatePropertyValuesForUserProfiles();
			break;
    }
}


void Configuration::setConfigValueBool(std::string configPropertyName,bool value){
    pair<std::string,bool> pbool;
    pbool = make_pair(configPropertyName,value);
    configBooleanProperties.insert(pbool);
    populatePropertyValuesForBoolean();
}

void Configuration::setConfigValueNumerical(std::string configPropertyName,long value){
    pair<std::string,long> plong;
    plong = make_pair(configPropertyName,value);
    configNumericalProperties.insert(plong);
    populatePropertyValuesForNumerical();
}

void Configuration::setConfigValueNumerical(std::string configPropertyName,double value){
    pair<std::string,double> pdouble;
    pdouble = make_pair(configPropertyName,value);
    configRealNumProperties.insert(pdouble);
    populatePropertyValuesForNumerical();
}

void Configuration::setConfigValueForProfiles(std::string configPropertyName,std::string value){
    pair<std::string,std::string> pstr;
    pstr = make_pair(configPropertyName,value);
    configUserProfileProperties.insert(pstr);
    populatePropertyValuesForUserProfiles();
}

void Configuration::setConfigValueForParsingNumerical(std::string configPropertyName,long value){
    pair<std::string,long> plong;
    plong = make_pair(configPropertyName,value);
    configParsingNumericalProperties.insert(plong);
    populatePropertyValuesForParsing();
}

void Configuration::setConfigValueForParsingNumerical(std::string configPropertyName,double value){
    pair<std::string,double> pdouble;
    pdouble = make_pair(configPropertyName,value);
    configParsingRealNumProperties.insert(pdouble);
    populatePropertyValuesForParsing();
}

void Configuration::setConfigValueForParsingStr(std::string configPropertyName,std::string value){
    pair<std::string,std::string> pstr;
    pstr = make_pair(configPropertyName,value);
    configParsingStrProperties.insert(pstr);
    populatePropertyValuesForParsingStr();
}

void Configuration::setConfigValueForLogging(std::string configPropertyName,long value){
    pair<std::string,long> plong;
    plong = make_pair(configPropertyName,value);
    configLoggingProperties.insert(plong);
    populatePropertyValuesForLogging();
}
