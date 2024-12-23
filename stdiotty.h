#ifndef STDIOTTY_H
#define STDIOTTY_H

#include "helpers.h"
#include "iodevice.h"

/*A teletype printer emulator that connects to stdin and stdout*/
class stdiotty : public iodevice {
    public:
    void read(char* buffer, byte opcode) override;
    void write(char* buffer, byte opcode) override;
};

#endif