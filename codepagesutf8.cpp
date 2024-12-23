#include "codepagesutf8.h"

encodingNativeUTF8::encodingNativeUTF8(const wchar_t* codepage){
    this->codepage = codepage;
}

std::wstring encodingNativeUTF8::operator()(char* src){
    std::wstring out;
    while (*src != '\0') {
        out += codepage[*src];
        src++;
    }
    return out;
}