#include "ttyUTF8.h"
#include <iostream>
#include <string>
#include <cstring>
#include <ios>

ttyUTF8::ttyUTF8(const wchar_t* codepage){
    encoding.setCodepage(codepage);
}

void ttyUTF8::acceptCommand(byte opcode,char* buffer,int count){
    std::wstring temp; //Temporary C-string for processing and encoding
    temp.resize(count);
    switch (opcode) {
        case 0x0A:
            {
                //Read
                sbuffer.seekg(0, std::ios::end);
                int bsize = ((int)sbuffer.tellg())/sizeof(wchar_t);
                sbuffer.seekg(0, std::ios::beg);
                if (bsize < count){
                    std::wstring temp2;
                    std::wcin >> temp2;
                    sbuffer << temp2;
                }// If there isn't enough chars in the buffer, read from the terminal
            }
            sbuffer.read(temp.data(),count);
            encoding.toCodepage(temp.data(),buffer,count);
            break;
        case 0x01:
            //Write w/out carriage return
            encoding.toUTF8(buffer,temp.data(),count);
            std::wcout << temp;
            break;
        case 0x09:
            //Write w/ carriage return
            encoding.toUTF8(buffer,temp.data(),count);
            std::wcout << temp << std::endl;
            break;    
    }
}

//TODO: Implement
void ttyUTF8::open(){
    return;
}

//TODO: Implement
void ttyUTF8::close(){
    return;
}