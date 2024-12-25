#ifndef ENCODINGUTF8_H
#define ENCODINGUTF8_H

#include <unordered_map>

class encodingUTF8 {
    public:
    encodingUTF8();
    encodingUTF8(const wchar_t* codepage);
    void setCodepage(const wchar_t* codepage);
    void toUTF8(char* source, wchar_t* dest, int count); //Translates from encoding to UTF-8
    void toCodepage(wchar_t* source, char* dest, int count); //Translates from UTF-8 to encoding
    private:
    const wchar_t* codepage;
    std::unordered_map<wchar_t,unsigned char> reverseCodepage; //Reverse codepage for translating from UTF to encoding
};

#endif