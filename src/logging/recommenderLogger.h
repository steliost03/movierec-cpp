#ifndef REC_LOGGER_H
#define REC_LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

class RecommenderLogger {
    
    private:
        
        //log levels: 1->ERROR,2->WARN,3->INFO,4->DEBUG,5->TRACE
        int loglevel = 5;
        std::ofstream outputStream;
        std::string name;

        std::string filename = "";
        bool streamIsOpen = false;
        
        void log(std::string level,std::string message);
        
        std::string generateFilename();
        
    public:
        
        RecommenderLogger(){}
        
        void init(std::string name,std::string filename,int loglevel){
            this->name = name;

            //do not attempt to open again if the stream is already open for the same file.
            bool doNotOpenAgain = (streamIsOpen && this->filename == filename);
            if(!doNotOpenAgain){
                //if stream is open, close first then reopen to avoid failures.
                if(streamIsOpen){
                    close();
                }
                outputStream.open(filename,std::ios_base::app);
            }

            this->loglevel = loglevel;

            this->filename = filename;
            streamIsOpen = true;
        }

        void init(std::string name,std::string filename){
            init(name,filename,5);
        }

        void init(std::string name,int loglevel){
            init(name,generateFilename(),loglevel);
        }

        void init(std::string name){
            init(name,generateFilename(),5);
        }

        void close(){
            outputStream.flush();
            outputStream.close();
            streamIsOpen = false;
            filename = "";
        }
        
        void trace(std::string message){if(loglevel>=5){log(std::string("TRACE"),message);}}
        void debug(std::string message){if(loglevel>=4){log(std::string("DEBUG"),message);}}
        void info(std::string message){if(loglevel>=3){log(std::string("INFO"),message);}}
        void warn(std::string message){if(loglevel>=2){log(std::string("WARN"),message);}}
        void error(std::string message){log(std::string("ERROR"),message);}
};
#endif
