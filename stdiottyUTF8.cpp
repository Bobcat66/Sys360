#include "stdiottyUTF8.h"
#include <iostream>
#include <string>
#include <cstring>

stdiottyUTF8::stdiottyUTF8(const wchar_t* codepage){
    this->encodingPtr = &encodingUTF8(codepage);
}

void stdiottyUTF8::acceptCommand(byte opcode,char* buffer,int count){
    switch (opcode) {
        case 0x0A:
            //Read
            wchar_t* wbuf = new wchar_t[count];
            std::wcin >> wbuf;
            encodingPtr->toCodepage(wbuf,buffer,count);
            delete[] wbuf;
            break;
        case 0x01:
            //Write w/out carriage return
            wchar_t* wbuf = new wchar_t[count];
            encodingPtr->toUTF8(buffer,wbuf,count);
            std::wcout << wbuf;
            delete[] wbuf;
            break;
        case 0x09:
            //Write w/ carriage return
            wchar_t* wbuf = new wchar_t[count];
            encodingPtr->toUTF8(buffer,wbuf,count);
            std::wcout << wbuf << std::endl;
            delete[] wbuf;
            break;    
    }
}

//TODO: Implement
void stdiottyUTF8::open(){
    return;
}

//TODO: Implement
void stdiottyUTF8::close(){
    return;
}