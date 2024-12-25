#ifndef STDIOTTYUTF8_H
#define STDIOTTYUTF8_H

#include <sstream>
#include "helpers.h"
#include "iodevice.h"
#include "encodingUTF8.h"

/*A teletype printer emulator that connects to stdin and stdout*/
class ttyUTF8 : public iodevice {
    public:
    ttyUTF8(const wchar_t* codepage);
    void acceptCommand(byte opcode, char* buffer, int count) override;
    void open() override;
    void close() override;
    private:
    encodingUTF8 encoding;
    std::wstringstream sbuffer;
};

#endif