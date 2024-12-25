#include "encodingUTF8.h"
#include "codepagesutf8.h"
#include <cstring>
#include <cwchar>

encodingUTF8::encodingUTF8(const wchar_t* codepage){
    setCodepage(codepage);
}

encodingUTF8::encodingUTF8(){}

void encodingUTF8::setCodepage(const wchar_t* codepage){
    this->codepage = codepage;
    for (int i = 0; i < wcslen(codepage); i++){
        reverseCodepage[codepage[i]] = i;
    }
}

void encodingUTF8::toUTF8(char* src, wchar_t* dest,int count){
    for (int i = 0; i < count; i++) {
        dest[i] = codepage[src[i]];
    }
}

void encodingUTF8::toCodepage(wchar_t* src, char* dest,int count){
    for (int i = 0; i < count; i++){
        dest[i] = reverseCodepage[src[i]];
    }
}