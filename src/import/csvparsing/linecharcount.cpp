#include "csvparsing.h"

/*
Counts the lines and the characters of a given file.
Detects and stores newline positions.
EOF position is included in newline positions.
EOF is NOT counted as a character.
*/
void linecharcount(file& target_file,int lines_to_ignore){

  target_file.linecount = 0;
  target_file.charcount = 0;
  ulong curr_newlinearray_size = 1;
  target_file.newlinePositions = (ulong*)malloc(curr_newlinearray_size*sizeof(ulong));
  char tempbuffer[MAXLINELEN];
  char c;

  //ignore the first amount of lines given
  int ignorecount = 0;
  while(ignorecount < lines_to_ignore && !feof(target_file.filepointer)){

	 fgets(tempbuffer,MAXLINELEN,target_file.filepointer);
     ignorecount++;

}
  int newlinecounter = -1;
  while(!feof(target_file.filepointer)) {

    c = fgetc(target_file.filepointer);
		if(c!=EOF)
    		target_file.charcount++;
    if(c == '\n' || c == EOF){
      target_file.linecount++;
      newlinecounter++;
      if(newlinecounter + 1 >= curr_newlinearray_size){
        curr_newlinearray_size = curr_newlinearray_size * 2;
        target_file.newlinePositions = (ulong*)realloc(target_file.newlinePositions,curr_newlinearray_size*sizeof(ulong));
      }
      target_file.newlinePositions[newlinecounter] = target_file.charcount - 1;
    }
	}

  target_file.newlinePositions = (ulong*)realloc(target_file.newlinePositions,(newlinecounter+1)*sizeof(ulong));
}
