#ifndef STREAMDEVICE_H
#define STREAMDEVICE_H

#include "iodevice.h"
#include <iostream>


//A class that encapsulates stdin and stdout in an iodevice, for testing purposes
class stdioDevice : public iodevice {
    public:
    stdioDevice();
    void acceptCommand(byte opcode) override; 
    void halt() override;
    void acceptByte(byte data) override;
    byte getByte() override;
    void open() override;
    void close() override;
    bool service_in() override;

    private:
    byte opcode;
};

#endif