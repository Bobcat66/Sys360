#ifndef STDIOTTYUTF8_H
#define STDIOTTYUTF8_H

#include <vector>
#include "helpers.h"
#include "iodevice.h"
#include "encodingUTF8.h"

/*A teletype printer emulator that connects to stdin and stdout*/
class stdiottyUTF8 : public iodevice {
    public:
    stdiottyUTF8(const wchar_t* codepage);
    void acceptCommand(byte opcode, char* buffer, int count) override;
    void open() override;
    void close() override;
    private:
    encodingUTF8 encoding;
    std::vector<wchar_t> inbuffer;
};

#endif