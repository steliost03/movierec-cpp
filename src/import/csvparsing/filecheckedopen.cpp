#include "csvparsing.h"

//attempts to open file,then checks if operation was successful.
bool filecheckedopen(const char* path,const char* op,FILE** filepointer,bool terminateOnError){

  *filepointer = fopen(path,op);
  return checkfile(*filepointer,path,terminateOnError);
}
