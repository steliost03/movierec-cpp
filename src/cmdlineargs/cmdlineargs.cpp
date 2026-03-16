#include "cmdlineargs.h"

#include "../exceptions/recommenderExceptions.h"

using namespace std;

ParsedCmdLineOptions PARSED_OPTIONS;

vector<string> convertCmdLineArgsToVec(int argc,char* argv[]){
    vector<string> argsVec;
    for(int i=0;i<argc;i++){
        argsVec.push_back(argv[i]);
    }
    return argsVec;
}

const CommandLineArgument CommandLineArgument::CONFIG_LOCATION = CommandLineArgument("-configpath");
const CommandLineArgument CommandLineArgument::SAVED_PROFILES_LOCATION = CommandLineArgument("-profilepath");

void parseCmdLineInputs(vector<string> args){
    for(auto it=args.begin();it!=args.end();it++){
        //do not attempt to parse shell command to start app (first cmdline argument).
        if(it==args.begin()){
            continue;
        }
        string arg = *it;
        if(arg == CommandLineArgument::CONFIG_LOCATION.str){
            it++;
            if(it==args.end() || it->empty()){
                break;
            }
            PARSED_OPTIONS.configpath = *it;
        }
        else if(arg == CommandLineArgument::SAVED_PROFILES_LOCATION.str){
            it++;
            if(it==args.end() || it->empty()){
                break;
            }
            PARSED_OPTIONS.profilepath = *it;
        }
        else{
            throw InvalidCmdLineArgumentException(arg);
        }
    }
}

void parseCmdLineInputs(int argc,char* argv[]){
    parseCmdLineInputs(convertCmdLineArgsToVec(argc,argv));
}
