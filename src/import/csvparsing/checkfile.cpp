#include "csvparsing.h"

bool checkfile(FILE* file,const char* path,bool terminateOnError){

  if(file == NULL){
	char message[200] = "Could not open file at path : ";
    if(terminateOnError){
        throwErrorAndTerminate(strcat(message,path));
    } else{
        printf("- - -\n");
        printf("Could not open file at path : %s\n",path);
        return false;
    }
  }
  return true;
}
