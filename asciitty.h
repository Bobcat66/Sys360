#ifndef ASCIITTY_H
#define ASCIITTY_H

#include "helpers.h"
#include "iodevice.h"

class asciitty : public iodevice {
    public:
    void read(char* buffer, byte opcode) override;
    void write(char* buffer, byte opcode) override;
};

#endif