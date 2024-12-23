#include "asciitty.h"
#include <iostream>

void asciitty::read(char* buffer, byte opcode){
    std::cin >> buffer;
}

void asciitty::write(char* buffer, byte opcode){
    switch (opcode){
        case 0x09:
            std::cout << buffer << std::endl;
        case 0x01:
            std::cout << buffer;
    }
    
}