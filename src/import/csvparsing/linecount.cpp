#include "csvparsing.h"

//counts the lines of a given file.
uint linecount(FILE* file,int lines_to_ignore){

	uint count = 0;
  char tempbuffer[1000];

	//ignore the first amount of lines given
  int ignorecount = 0;
  while(ignorecount < lines_to_ignore && !feof(file)){
	   fgets(tempbuffer,1000,file);
     ignorecount++;
}

	while(!feof(file)) {

    fgets(tempbuffer,1000,file);
    count++;

	}

	return count;
}
