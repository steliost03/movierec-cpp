#include "recommenderLogger.h"
#include <ctime>
using namespace std;

void generateFormattedDateStr(string& dateStr,const char* format){
    
    time_t rawtime;
    time(&rawtime);
    tm* timeinfo = localtime(&rawtime);
    char cDateStr[500];
    strftime(cDateStr,500*sizeof(char),format,timeinfo);
    
    dateStr = cDateStr;
}

string RecommenderLogger::generateFilename(){
    
    string timestampStr;
    generateFormattedDateStr(timestampStr,"%Y%m%d");
    string fullname = "log_"+timestampStr+".txt";
    
    return fullname;
}

void RecommenderLogger::log(string level,string message){
    
    string formattedDateStr;
    generateFormattedDateStr(formattedDateStr,"%Y-%m-%d %H:%M:%S");
    
    string fullStr = formattedDateStr +" " + level + " "+ name+" : "+message +"\n";
    
    outputStream << fullStr;
    outputStream.flush(); //flushing after each message may cause performance penalty.
}
