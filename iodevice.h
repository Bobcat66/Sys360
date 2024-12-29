#ifndef IODEVICE_H
#define IODEVICE_H

#include "helpers.h"
#include <mutex>

class iodevice {
    public:
    virtual ~iodevice() {};
    virtual void acceptCommand(byte opcode) = 0; //Executes the given command on the IOdevice.
    virtual void halt() = 0; //Prematurely halts the currently executing command on the IOdevice;
    virtual void acceptByte(byte data) = 0; //Accepts one byte of data during a Write/Control Operation
    virtual byte getByte() = 0; //Retrieves one byte of data during a Read/Sense Operation
    virtual void open() = 0; //Opens device for IO
    virtual void close() = 0; //Closes device
    virtual bool service_in() = 0; //Returns whether or not the device is ready to either accept data from the channel or transmit data to the channel
    std::mutex mtx;
};

#endif