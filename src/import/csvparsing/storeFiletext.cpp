#include "csvparsing.h"

//stores the entirety of the text of a file to a chunk in memory.
//Assumes known charcount and linecount.
void storeFiletext(file& target_file,int lines_to_ignore){

  ulong fsize = (target_file.charcount)*sizeof(char);
  
  //+1 because terminating character is not included in the charcount.
  target_file.filetext = (char*)malloc(fsize+sizeof(char));

  fseek(target_file.filepointer,0,SEEK_SET);
  if(lines_to_ignore>0){

    char tempbuffer[MAXLINELEN];
    for(int i=0;i<lines_to_ignore;i++)
      fgets(tempbuffer,MAXLINELEN,target_file.filepointer);
      
    fseek(target_file.filepointer,0,SEEK_CUR);

  }

  fread(target_file.filetext,fsize,1,target_file.filepointer);
  
  //add terminating character at the end
  target_file.filetext[target_file.charcount] = '\0';
  
}
