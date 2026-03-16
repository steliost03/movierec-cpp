#include "csvparsing.h"

//TODO: also adjust newlinePositions.
void adjustForTrailingNewlines(file& target_file){
	
	bool consecutive_newlines = true;

	uint curr_idx = target_file.charcount - 1; //start from last character.
	
	while (consecutive_newlines){

		if (target_file.filetext[curr_idx] == '\n'){
			target_file.filetext[curr_idx] = '\0';
			target_file.linecount--;
			target_file.charcount--;
			curr_idx--;
		}
		else{
			consecutive_newlines = false;
		}
	}
}
