#ifndef CKDDASD_H
#define CKDDASD_H

#include <fstream>
#include <string>
#include <vector>
#include "helpers.h"
#include "iodevice.h"

struct record;

struct pointerStatus {
    unsigned int bin : 8;
    unsigned int cylinder : 16;
    unsigned int track : 16;
    unsigned int record : 8;
};

struct trackMetadata {
    byte flag;
    halfword cylinder;
    halfword track;
    halfword check;
    doubleword indexPointer;
    std::vector<record*> records;
};

typedef void (ckddasd::*iocom)(char* buffer);

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
    trackMetadata trackInfo;
    doubleword pointer;
    char internalBuf[256]; //Internal char buffer
    std::string dataName; //File name of DASD image
    std::fstream datastream;
    void seekBin(); //sets pointer to the beginning of the bin in ptrInfo
    void seekCylinder(); //sets pointer to the beginning of the cylinder in ptrInfo
    void seekTrack(); //sets pointer to the beginning of the track in ptrInfo
    void seekRecord(); //sets pointer to the beginning of the record in ptrInfo
    void updatePtrR(); //Updates pointer after a read operation
    void updatePtrW(); //Updates pointer after a write operation
    void syncStreamPtrs(); //Synchronizes stream pointers to the ckddasd pointers
    void clearTrackRecords(); //Safely disposes of track record metadata (Improper disposal of track records may cause a memory leak)
    void loadTrack(); //Loads track metadata for simpler seek operations
    void seekKey(const char* seekkey); //Seeks record by its key
};

#endif
