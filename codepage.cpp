#include <cstring>
#include "codepage.h"

void translate(char* string, const unsigned char* codepage){
    int len = strlen(string);
    for (int i = 0; i < len; i++){
        string[i] = *(codepage + (unsigned char)string[i]);
    }
}