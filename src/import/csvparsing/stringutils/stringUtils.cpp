#include "stringUtils.h"

#include <cctype>
#include <cstring>
#include <cstdlib>

inline bool condition(bool specialchar,bool specialchar2,char c,char target) {
	
	if(specialchar){
		return !isalnum(c);
	}
	else if(specialchar2){
		bool exclusions = !((c == '(') || (c == ')') || (c == '{') || (c == '}') || (c == '[') || (c == ']') || (c == '.') || (c == '?') || (c == '!'));
		return (!isalnum(c) && exclusions);
	}
	else{
		return (c==target);
	}
	
}

///////////////////////////////////////////////////////////////////////

//TODO : explanations (+ explanations of specialchars).

//starts checking from beggining of str.
void strip_trailing(char* str,char target){
	
	char c = str[0];
	uint idx = 0;
	uint mark = 0;
	bool consecutive = false;
	bool specialchar = (target == '*');
	bool specialchar2 = (target == '~');
	
	
	while(c != '\0'){
		
		if(condition(specialchar,specialchar2,c,target) && !consecutive){
			mark = idx;
			consecutive = true;
		}
		
		if(!condition(specialchar,specialchar2,c,target))
			consecutive = false;
		
		idx++;
		c = str[idx];
		
	}
	
	if(consecutive){
		str[mark] = '\0';
	}
	
}

//starts checking from end of str.
void strip_trailing2(char* str,char target){
	
	uint idx = strlen(str) - 1; //strlen will potentially slow down,must be replaced.
    //uint mark = idx;
	char c = str[idx];
	bool specialchar = (target == '*');
	bool specialchar2 = (target == '~');
	
	while(condition(specialchar,specialchar2,c,target)){
		
		idx--;
		c = str[idx];
		
	}
	
	str[idx+1] = '\0';
}

void strip_leading(char** str,char target){
	
	
	char c = (*str)[0];
	uint idx = 0;
	bool specialchar = (target == '*');
	bool specialchar2 = (target == '~');
	
	while(condition(specialchar,specialchar2,c,target)){

		idx++;
		c = (*str)[idx];
		
	}
	
	(*str) = (*str) + idx;
}

void remove_extra_whitespaces(char* str){

    int i, x;
    for(i=x=0; str[i]; ++i)
      if(!isspace(str[i]) || (i > 0 && !isspace(str[i-1])))
        str[x++] = str[i];
    str[x] = '\0';
}

void strip(char** str,char target) {
	
	strip_leading(str,target);
	strip_trailing((*str),target);
	
}

void strip2(char** str,char target){
	
	strip_leading(str,target);
	strip_trailing2((*str),target);

}

void alltolower(char* str){
	
	char c = str[0];
	uint idx=0;
	
	while(c != '\0'){
		
		str[idx] = tolower(c);
		
		idx++;
		c = str[idx];
		
	}
}

