#ifndef IODEVICE_H
#define IODEVICE_H

#include "helpers.h"

class iodevice {
    public:
    virtual ~iodevice() = default;
    virtual void control(char* buffer, byte opcode) = 0; //Buffer contains arguments from memory
    virtual void sense(char* buffer, byte opcode) = 0; //Buffer contains arguments from memory
    virtual void read(char* buffer, byte opcode) = 0; //Chars are read into buffer
    virtual void write(char* buffer, byte opcode) = 0; //Chars are written from buffer
    virtual void readBkwd(char* buffer, byte opcode) = 0; //Chars are read into buffer
    virtual void open() = 0; //Opens device for IO
    virtual void close() = 0; //Closes device
};

#endif