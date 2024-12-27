#include "ttyUTF8.h"
#include "codepagesutf8.h"
#include <iostream>
//Build command: g++ -o test.out testprogram.cpp ttyUTF8.cpp encodingUTF8.cpp 

int main(){
    ttyUTF8 foo(iso8859,std::wcin,std::wcout);
    char readBuf;
    foo.acceptCommand(0x09,"Hello, World!",13);
    while (readBuf != '\r'){
        foo.acceptCommand(0x0A,&readBuf,1);
        foo.acceptCommand(0x01,&readBuf,1);
    }
}