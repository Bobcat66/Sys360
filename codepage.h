#ifndef CODEPAGE_H
#define CODEPAGE_H

#include <unordered_map>
#include <string>
class encode {
    public:
    encode(unsigned char* codepage);
    void operator()(char* string);
    private:
    unsigned char* codepage;
};

//Because this is a map of FUNCTOR POINTERS, not functions, the functors need to be explicitly dereferenced in order to call them
extern std::unordered_map<std::string,encode> codepages;

#endif