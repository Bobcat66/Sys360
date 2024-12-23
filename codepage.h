#ifndef CODEPAGE_H
#define CODEPAGE_H

/*Translates an ascii-coded string to an ebcdic-coded string*/
void atoe(char* str);

/*Translates an ebcdic-coded string to an ascii-coded string*/
void etoa(char* str);

#endif