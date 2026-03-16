#include "csvparsing.h"

//wrapper that calls linecharcount and storeFiletext for a file.
void processFile(file& target_file,int lines_to_ignore,bool ignore_trailing_newlines){

  linecharcount(target_file,lines_to_ignore);
  storeFiletext(target_file,lines_to_ignore);

  if (ignore_trailing_newlines){
	  adjustForTrailingNewlines(target_file);
  }

}
