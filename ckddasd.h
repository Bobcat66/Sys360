#ifndef CKDDASD_H
#define CKDDASD_H

#include <fstream>
#include <string>
#include "helpers.h"
#include "iodevice.h"

struct pointerStatus {
    unsigned int bin : 8;
    unsigned int cylinder : 16;
    unsigned int track : 16;
    unsigned int record : 8;
};

class ckddasd : public iodevice {
    public:
    ckddasd(std::string dataName);
    ~ckddasd();
    void read(char* buffer, byte opcode) override;
    void write(char* buffer, byte opcode) override;
    void sense(char* buffer, byte opcode) override;
    void readBkwd(char* buffer, byte opcode) override;
    void control(char* buffer, byte opcode) override;
    void open() override;
    void close() override;
    private:
    doubleword header;
    pointerStatus ptrInfo;
    bool writeMode;
    char internalBuf[256]; //Internal char buffer
    std::string dataName; //File name of DASD image
    std::fstream datastream;
    void seekBin(); //sets fstream pointer to the beginning of the bin in ptrInfo
    void seekCylinder(); //sets fstream pointer to the beginning of the cylinder in ptrInfo
    void seekTrack(); //sets fstream pointer to the beginning of the track in ptrInfo
    void seekRecord(); //sets fstream pointer to the beginning of the record in ptrInfo
};

#endif
