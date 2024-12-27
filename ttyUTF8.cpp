#include "ttyUTF8.h"
#include <iostream>
#include <string>
#include <cstring>
#include <ios>

ttyUTF8::ttyUTF8(const wchar_t* codepage,std::wistream &instream, std::wostream &outstream)
        :input(instream),output(outstream){
    encoding.setCodepage(codepage);
}

void ttyUTF8::acceptCommand(byte opcode,char* buffer,int count){
    std::wstring temp; //Temporary C-string for processing and encoding
    temp.resize(count);
    switch (opcode) {
        case 0x0A:
            //Read
            while (sbuffersize < count){
                std::wstring temp2;
                std::wcin >> temp2;
                sbuffer << temp2;
                sbuffersize += temp2.size();
            }// If there isn't enough chars in the buffer, read from the terminal
            sbuffer.read(temp.data(),count);
            sbuffersize -= count;
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