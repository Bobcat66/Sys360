#include "stdiottyUTF8.h"
#include <iostream>
#include <string>
#include <cstring>

stdiottyUTF8::stdiottyUTF8(const wchar_t* codepage){
    encoding.setCodepage(codepage);
}

void stdiottyUTF8::acceptCommand(byte opcode,char* buffer,int count){
    wchar_t* wbuf = new wchar_t[count];
    switch (opcode) {
        case 0x0A:
            //Read
            std::wcin >> wbuf;
            encoding.toCodepage(wbuf,buffer,count);
            break;
        case 0x01:
            //Write w/out carriage return
            encoding.toUTF8(buffer,wbuf,count);
            std::wcout << wbuf;
            break;
        case 0x09:
            //Write w/ carriage return
            encoding.toUTF8(buffer,wbuf,count);
            std::wcout << wbuf << std::endl;
            break;    
    }
    delete[] wbuf;
}

//TODO: Implement
void stdiottyUTF8::open(){
    return;
}

//TODO: Implement
void stdiottyUTF8::close(){
    return;
}