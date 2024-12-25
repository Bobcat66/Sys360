#include "stdiottyUTF8.h"
#include "codepagesutf8.h"

//Build command: g++ -o test.out testprogram.cpp stdiottyUTF8.cpp encodingUTF8.cpp 

int main(){
    stdiottyUTF8 foo(iso8859);
    foo.acceptCommand(0x09,"Hello, World!",13);
}