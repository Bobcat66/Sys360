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
    void acceptChar(char data) override;
    char getChar() override;
    void open() override;
    void close() override;
    bool service_in() override;

    private:
    bool _service_in;
    int count;
    byte opcode;
};

#endif