#ifndef IODEVICE_H
#define IODEVICE_H

#include "helpers.h"

class iodevice {
    public:
    virtual ~iodevice() = default;
    virtual void acceptCommand(byte opcode,char* buffer,int count) = 0;
    virtual void open() = 0; //Opens device for IO
    virtual void close() = 0; //Closes device
};

#endif