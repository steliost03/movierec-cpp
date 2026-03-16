#include "csvparsing.h"

//counts the amount of characters in a file.
ulong charcount(FILE* file,int lines_to_ignore){

  ulong count = 0;
  char tempbuffer[1000];

  //ignore the first amount of lines given
  int ignorecount = 0;
  while(ignorecount < lines_to_ignore && !feof(file)){
	   fgets(tempbuffer,1000,file);
     ignorecount++;
}

while(!feof(file)) {

  fgetc(file);
  count++;

	}

  return count;
}
