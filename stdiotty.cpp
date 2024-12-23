#include "stdiotty.h"
#include <iostream>

void stdiotty::read(char* buffer, byte opcode){
    std::cin >> buffer;
}

void stdiotty::write(char* buffer, byte opcode){
    switch (opcode){
        case 0x09:
            std::cout << buffer << std::endl;
        case 0x01:
            std::cout << buffer;
    }
}