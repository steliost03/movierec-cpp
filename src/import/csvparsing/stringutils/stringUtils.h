#ifndef STRUTIL_H
#define STRUTIL_H

typedef unsigned int uint;
typedef unsigned long ulong;

void strip_trailing(char* str,char target);
void strip_trailing2(char* str,char target);

void strip_leading(char** str,char target);

void strip(char** str,char target);
void strip2(char** str,char target);

void alltolower(char* str);

void remove_extra_whitespaces(char* str);

#endif
