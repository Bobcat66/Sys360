#ifndef STDIOTTYUTF8_H
#define STDIOTTYUTF8_H

#include <sstream>
#include "helpers.h"
#include "iodevice.h"
#include "encodingUTF8.h"
#include <istream>
#include <ostream>

/*A teletype printer emulator that connects to an arbitrary input stream and arbitrary output stream*/
class ttyUTF8 : public iodevice {
    public:
    ttyUTF8(const wchar_t* codepage,std::wistream &instream, std::wostream &ostream);
    void acceptCommand(byte opcode, char* buffer, int count) override;
    void open() override;
    void close() override;
    private:
    std::wistream &input;
    std::wostream &output;
    encodingUTF8 encoding;
    std::wstringstream sbuffer;
    int sbuffersize = 0;
};

#endif