#ifndef IODEVICE_H
#define IODEVICE_H

#include <array>
#include "helpers.h"

class iodevice {
    public:
    virtual void control(char* buffer, byte opcode); //Buffer contains arguments from memory
    virtual void sense(char* buffer, byte opcode); //Buffer contains arguments from memory
    virtual void read(char* buffer, byte opcode); //Chars are read into buffer
    virtual void write(char* buffer, byte opcode); //Chars are written from buffer
    virtual void readBkwd(char* buffer, byte opcode); //Chars are read into buffer
    virtual void open(); //Opens device for IO
    virtual void close(); //Closes device
};

#endif