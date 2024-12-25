#include "ttyUTF8.h"
#include "codepagesutf8.h"
#include <iostream>
//Build command: g++ -o test.out testprogram.cpp ttyUTF8.cpp encodingUTF8.cpp 

int main(){
    ttyUTF8 foo(iso8859);
    char readBuf[5];
    foo.acceptCommand(0x09,"Hello, World!",13);
    foo.acceptCommand(0x0A,readBuf,5);
    foo.acceptCommand(0x09,readBuf,5);
    foo.acceptCommand(0x0A,readBuf,5);
    foo.acceptCommand(0x09,readBuf,5);
}