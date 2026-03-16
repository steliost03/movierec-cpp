#ifndef CMD_LINE_ARGS_H
#define CMD_LINE_ARGS_H

#include <string>
#include <vector>

class CommandLineArgument {

    public:

    static const CommandLineArgument CONFIG_LOCATION;
    static const CommandLineArgument SAVED_PROFILES_LOCATION;
    static const CommandLineArgument UNRECOGNIZED;

    CommandLineArgument(std::string input){
        this->str = input;
    }

    std::string str;
};

void parseCmdLineInputs(std::vector<std::string> args);
void parseCmdLineInputs(int argc,char* argv[]);

struct ParsedCmdLineOptions{
    std::string configpath = "";
    std::string profilepath = "";
};

extern ParsedCmdLineOptions PARSED_OPTIONS;

#endif
