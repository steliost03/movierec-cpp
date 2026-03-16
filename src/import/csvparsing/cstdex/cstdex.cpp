#include "cstdex.h"

/*
 * * * C STANDARD LIBRARY EXTENSION * * *
*/


void throwErrAndTerm(const char* message){

	fprintf(stderr,"ERROR : ");
	fprintf(stderr,"%s", message);
	fprintf(stderr,"\nThe program will now terminate.");
	fprintf(stderr,"\n");
	exit(0);

}

/*
Provides similar functionality with fgets,but for a string input not belonging in a file stream.
input_str is read char-by-char until a user-supplied character, a newline or a terminating character is encountered.
input_str is assumed to be non-null and to have a prior appropriate memory allocation.
output_str does not contain newlines after the operation, but it will contain the user-supplied character, if it is encountered.
returns the value of input_idx corresponding to the first character after newline/terminating.
*/

int sgets_general(const char* input_str,char* output_str,ulong input_idx,char stoppingch) {

  if(output_str == NULL){
    throwErrAndTerm("Encountered NULL pointer in sgets function.");
  }

  int output_idx = 0;
  char c;

  c = input_str[input_idx];
  while(c!='\0' && c!='\n' && c!=stoppingch){
    output_str[output_idx] = c;
    output_idx++;
    input_idx++;
    c = input_str[input_idx];

}
  if(c == stoppingch){
	  output_str[output_idx] = c;
	  output_idx++;
  }
  output_str[output_idx] = '\0';
  input_idx++;
  return input_idx;
}

/*
Provides similar functionality with fgets,but for a string input not belonging in a file stream.
input_str is read char-by-char until a newline or terminating character is encountered.
input_str is assumed to be non-null and to have a prior appropriate memory allocation.
input_idx is the index of the first character of input_str to be scanned.
output_str does not contain newlines after the operation.
returns the value of input_idx corresponding to the first character after newline/terminating.
*/
int sgets(const char* input_str,char* output_str,ulong input_idx) {

  if(output_str == NULL){
    throwErrAndTerm("Encountered NULL pointer in sgets function.");
  }

  int output_idx = 0;
  char c;

  c = input_str[input_idx];
  while(c!='\0' && c!='\n'){
    output_str[output_idx] = c;
    output_idx++;
    input_idx++;
    c = input_str[input_idx];

}
  output_str[output_idx] = '\0';
  input_idx++;
  return input_idx;
}

//

/*
Expands upon the functionality of strcpy, enabling the copying of only a part of the src string.
assumes dest is properly allocated and not null.
end_idx is inclusive and should not point to a terminating character.
The terminating character is added manually to the end of dest.
*/
void partialstrcpy(char* dest,const char* src,ulong start_idx,ulong end_idx,bool ignore_newlines){

  if(dest == NULL){
    throwErrAndTerm("Encountered NULL input while copying strings.");
  }

  ulong dest_idx = 0;
  ulong src_idx = start_idx;

  while(src_idx <= end_idx){
	
	if(!ignore_newlines){
		dest[dest_idx] = src[src_idx];
	}
	else{
		if(src[src_idx] != '\n'){
			dest[dest_idx] = src[src_idx];
		}
	}
    src_idx++;
    dest_idx++;

  }
  dest[dest_idx] = '\0'; 
}

/*
Detects whether or not a character 'ch' is contained in the string 'str'. 
*/
bool chinstr(char ch,const char* str){
	
	unsigned int idx = 0;
	char curr = str[idx];
	
	while(curr != 0){
		
		if(curr == ch){
			return true;
		}
		
		idx++;
		curr = str[idx];
		
	}	
	return false;	
}

//
char* strchr_n(char* str,const char* unwantedchars){
	
	uint idx = 0;
	char curr = str[idx];
	
	while(curr != 0){
	
		if(!chinstr(curr,unwantedchars)){
			return (str + idx);
		}
		
		idx++;
		curr = str[idx];
		
	}
	return NULL;	
}


/*
Returns the position of the first character of 'str' NOT being one of the given 'unwantedchars'.
*/
int strchr_nidx(char* str,const char* unwantedchars){
	
	
	uint idx = 0;
	char curr = str[idx];
	
	while(curr != 0){
	
		if(!chinstr(curr,unwantedchars)){
			return idx;
		}
		
		idx++;
		curr = str[idx];
		
	}
	return -1;	
}

/*
*/
int strchr_idx(char* str,const char* characters,int start_idx){
	
	uint idx = start_idx;
	char curr = str[idx];
	
	while(curr != 0){
	
		if(chinstr(curr,characters)){
			return idx;
		}
		
		idx++;
		curr = str[idx];
		
	}
	return -1;	
}

//TODO : handle the case of consecutive calls to strtok_ignr with different str values.

/*
Extension of c standard library strtok, which also accepts the 'spch' parameter.
The 'spch' parameter represents the special characters that signal the beggining and/or the end
of the parts of the input str that will not be scanned for delimiters and will be taken
as is.

For example, for str == "A,list,"of,comma,separated",values" , delimiters == "," and spch == "\""

the resulting tokens will be :
A
list
of,comma,separated
values
*/
char* strtok_ignr(char* str,const char* delimiters,const char* spch){
	
	static uint stored_idx = 0;
	static bool term_found = false;
	static char stored_str[LINE_MAX_LEN] = "";
	static char token[TOK_MAX_LEN] = "";
	uint start_idx = 0;
	char curr;
	if(str != NULL){
		strcpy(stored_str,str);
		term_found = false;
		stored_idx = strchr_nidx(str,delimiters); //finds the first character in str that is not a delimiter.
		start_idx = stored_idx;
	
		if(stored_idx>=0){ //non-delimiter character found.
			curr = str[stored_idx];
		}
		else{ //non-delimiter character not found / reached end.
			term_found = true;
			return NULL;
		}
	}
	else{
		if(term_found){
			//end has already been reached,return null.
			return NULL;
		}
		//end not reached, continuing from where we left off.
		start_idx = stored_idx;
		curr = stored_str[start_idx];
	}
	
	//
	while(curr != 0){
		if(chinstr(curr,spch)){
			
			stored_idx = strchr_idx(stored_str,spch,stored_idx+1); 
			if(stored_idx<0){
				partialstrcpy(token,stored_str,start_idx,strlen(stored_str)-1);
				term_found = true;
				stored_idx = 0;
				return token;
			}
		}
		stored_idx++;
		curr = stored_str[stored_idx];
		if(chinstr(curr,delimiters)){
			partialstrcpy(token,stored_str,start_idx,stored_idx-1);
			stored_idx++;
			if(stored_str[stored_idx] == 0){
				term_found = true;
				stored_idx = 0;
			}
			return token;
			
		}
	}
	
	term_found = true;
	partialstrcpy(token,stored_str,start_idx,stored_idx-1);
	stored_idx = 0;
	return token;
}

/*
Thread safe version of strtok_ignr
*/
char* strtok_ignr_mthread(char* str,const char* delimiters,const char* spch,uint* stored_idx,
						  bool* term_found,char* stored_str,char* token){
	
	uint start_idx = 0;
	char curr;
	if(str != NULL){
		strcpy(stored_str,str);
		*term_found = false;
		*stored_idx = strchr_nidx(str,delimiters);
		start_idx = *stored_idx;

		if( (*stored_idx) >=0){
			curr = str[(*stored_idx)];
		}
		else{
			*term_found = true;
			return NULL;
		}
	}
	else{
		if((*term_found)){
			return NULL;
		}
		start_idx = *stored_idx;
		curr = stored_str[start_idx];
	}
	
	//
	while(curr != 0){
		if(chinstr(curr,spch)){
			
			*stored_idx = strchr_idx(stored_str,spch,(*stored_idx)+1); 
			if( (*stored_idx) <0){
				partialstrcpy(token,stored_str,start_idx,strlen(stored_str)-1);
				*term_found = true;
				*stored_idx = 0;
				return token;
			}
		}
		*stored_idx = *stored_idx + 1;
		curr = stored_str[(*stored_idx)];
		if(chinstr(curr,delimiters)){
			partialstrcpy(token,stored_str,start_idx,*stored_idx-1);
			*stored_idx = *stored_idx + 1;
			if(stored_str[(*stored_idx)] == 0){
				*term_found = true;
				*stored_idx = 0;
			}
			return token;
			
		}
	}
	
	*term_found = true;
	partialstrcpy(token,stored_str,start_idx,*stored_idx-1);
	*stored_idx = 0;
	return token;
}
