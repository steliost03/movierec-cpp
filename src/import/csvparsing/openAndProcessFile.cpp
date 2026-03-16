#include "csvparsing.h"

//wrapper that calls filecheckedopen and processFile, for a file.
bool openAndProcessFile(const char* path,file& target_file,bool terminateOnError,int lines_to_ignore,bool ignore_trailing_newlines) {
  
  bool success = filecheckedopen(path,"r",&target_file.filepointer,terminateOnError);
  if(!success){
      return false;
  }
  processFile(target_file,lines_to_ignore,ignore_trailing_newlines);
  return true;
}
