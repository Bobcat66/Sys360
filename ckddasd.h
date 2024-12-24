#ifndef CKDDASD_H
#define CKDDASD_H

#include <fstream>
#include <string>
#include <vector>
#include "helpers.h"
#include "iodevice.h"

struct record;

struct pointerStatus {
    halfword bin;
    halfword cylinder;
    halfword track;
    byte record;
};

struct trackMetadata {
    byte flag;
    halfword cylinder;
    halfword track;
    halfword check;
    doubleword indexPointer;
    std::vector<record*> records;
};

class ckddasd : public iodevice {
    public:
    ckddasd(std::string dataName);
    ~ckddasd();
    void acceptCommand(byte opcode, char* buffer, int count) override;
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
    record* recordInfoPtr;
    bool chainedFromSearch = false; //A flag that tells the device whether or not the current command has been chained from a search command
    void seekBCT(); //sets pointer to the BBCCTT address specified in ptrInfo
    void seekTrackIndex(); //sets pointer to the beginning of the current track;
    void seekRecord(); //sets pointer to the beginning of the record in ptrInfo
    void seekRecordData(); //Sets pointer to the beginning of the current record data area
    void updatePtrR(); //Updates pointer after a read operation
    void updatePtrW(); //Updates pointer after a write operation
    void syncStreamPtrs(); //Synchronizes stream pointers to the ckddasd pointers
    void clearTrackRecords(); //Safely disposes of track record metadata (Improper disposal of track records may cause a memory leak)
    void loadTrack(); //Loads track metadata for simpler seek operations
    void seekKey(const char* seekkey); //Seeks record by its key, sets pointer to the beginning of the record
    void seekRecordKey(); //Seeks the beginning of the key area of the current record;
    void seekNextRecord(); //Seeks next record on the track. NOTE: This function loops back to the beginning of the track if no record is found
};

#endif
