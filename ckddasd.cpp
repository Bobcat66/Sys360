#include <cstring>
#include <string>
#include "ckddasd.h"

inline halfword getBins(doubleword header){return header / (1 << 48);}
inline halfword getCylinders(doubleword header){return (header % (1 << 48)) / (1 << 32);}
inline halfword getTracks(doubleword header){return (header % (1 << 32)) / (1 << 16);}
inline halfword getTsize(doubleword header){return header % (1 << 16);}

inline byte getRecordNum(doubleword count){return (count % (1 << 24)) / (1 << 16);}
inline byte getKeyLen(doubleword count){return (count % (1 << 16))/(1 << 8);}
inline byte getRecordLen(doubleword count){return (count % (1 << 8));}

struct record {
    byte flag;
    halfword cylinder;
    halfword track;
    byte recordNum;
    byte keyLen;
    halfword dataLen;
    halfword check;
    char* key;
    doubleword dataPtr; //Pointer to the beginning of the record's data within the fstream
    doubleword initPtr; //Pointer to the beginning of the record within the fstream
};


/* PUBLIC */

ckddasd::ckddasd(std::string dataName){
    this->dataName = dataName;
    datastream.open(dataName,std::fstream::in | std::fstream::binary);
    datastream.read(reinterpret_cast<char*>(&header),8);
    datastream.close();
}

ckddasd::~ckddasd(){
    clearTrackRecords();
}

void ckddasd::acceptCommand(byte opcode, char* buffer, int count){
    
}

void ckddasd::read(char* buffer, byte opcode){
    switch (opcode) {
        case 0x1A:
            //Read Home address of current track
            seekTrackIndex();
            datastream.read(buffer,5);
        case 0x12:
            //Read count of next record encountered (flag and check are omitted)
            seekNextRecord();
            pointer += 1;
            syncStreamPtrs();
            datastream.read(buffer,8);
        case 0x16:
            //Reads track descriptor record of current track
            seekTrackIndex();
            ptrInfo.record = 0;
            seekRecord();
            seekRecordData();
            datastream.read(buffer,8);
        case 0x06:
            //Reads current record data
            if (!chainedFromSearch){seekNextRecord();}
            seekRecordData();
            datastream.read(buffer,recordInfoPtr->dataLen);
        case 0x0E:
            //Reads current record key and data
            if (!chainedFromSearch){seekNextRecord();}
            seekRecordKey();
            datastream.read(buffer,recordInfoPtr->keyLen + recordInfoPtr->dataLen);
        case 0x1E:
            //Reads current record count, key, and data
            if (!chainedFromSearch){seekNextRecord();}
            pointer += 1;
            syncStreamPtrs();
            datastream.read(buffer,8);
            pointer += 2;
            syncStreamPtrs();
            datastream.read(buffer + 8,recordInfoPtr->keyLen + recordInfoPtr->dataLen);
    }
    chainedFromSearch = false;
    pointer += 1;
    syncStreamPtrs();
}

void ckddasd::write(char* buffer, byte opcode){
    switch (opcode) {
        case 0x19:
            //Write home address of current track
            seekBCT();
            datastream.write(buffer,5);
        case 0x05:
            //Write data into current record. Must be chained from search command.
            seekRecordData();
            datastream.write(buffer,recordInfoPtr->dataLen);
        case 0x0D:
            //Write key and data into current record. Must be chained from search command
            seekRecordKey();
            datastream.write(buffer,recordInfoPtr->keyLen + recordInfoPtr->dataLen);
        case 0x1D:
            //Write count key and data. Must be chained from search command
            pointer += 1;
            syncStreamPtrs();
            datastream.write(buffer,8);
            pointer += 2;
            syncStreamPtrs();
            datastream.write(buffer+8,recordInfoPtr->keyLen + recordInfoPtr->dataLen);
            
        chainedFromSearch = false;
        syncStreamPtrs();
    }
}

void ckddasd::control(char* buffer, byte opcode){
    switch (opcode) {
        case 0x07:
            //Seek
            std::memcpy(reinterpret_cast<char*>(ptrInfo.bin),buffer,2);
            std::memcpy(reinterpret_cast<char*>(ptrInfo.cylinder),buffer + 2,2);
            std::memcpy(reinterpret_cast<char*>(ptrInfo.track),buffer + 4,2);
            seekBCT();
            loadTrack();
            break;
        case 0x1B:
            //Seek track
            std::memcpy(reinterpret_cast<char*>(ptrInfo.bin),buffer,2);
            std::memcpy(reinterpret_cast<char*>(ptrInfo.track),buffer + 4,2);
            seekBCT();
            loadTrack();
            break;
        case 0x0B:
            //Seek cylinder
            std::memcpy(reinterpret_cast<char*>(ptrInfo.bin),buffer,2);
            std::memcpy(reinterpret_cast<char*>(ptrInfo.cylinder),buffer + 2,2);
            seekBCT();
            loadTrack();
            break;
    }
}

//TODO: Implement
void ckddasd::sense(char* buffer, byte opcode){
    return;
}

//TODO: Implement
void ckddasd::readBkwd(char* buffer, byte opcode){
    return;
}

/* PRIVATE */

void ckddasd::clearTrackRecords(){
    for (record* rec : trackInfo.records){
        delete rec;
    }
}

void ckddasd::open(){
    datastream.open(dataName,std::fstream::in | std::fstream::out | std::fstream::binary);
}

void ckddasd::close(){
    datastream.close();
}

void ckddasd::seekBCT(){
    //seek bin
    int binSize = getTsize(header) * getTracks(header) * getCylinders(header);//hdrInfo.tsize * hdrInfo.tracks * hdrInfo.cylinders;
    pointer = ptrInfo.bin * binSize + 8;
    //seek cylinder
    int cylinderSize = getTsize(header) * getTracks(header);
    pointer += ptrInfo.cylinder * cylinderSize;
    //seek track
    int trackSize = getTsize(header);
    pointer += ptrInfo.track * trackSize;
    syncStreamPtrs();
}

void ckddasd::seekTrackIndex(){
    pointer = trackInfo.indexPointer;
    syncStreamPtrs();
}

void ckddasd::updatePtrR(){
    pointer = datastream.tellg();
}

void ckddasd::updatePtrW(){
    pointer = datastream.tellp();
}

void ckddasd::syncStreamPtrs(){
    datastream.seekg(pointer);
    datastream.seekp(pointer);
}

//Loads a track into metadata. Should be preceded by a seekTrack command
void ckddasd::loadTrack(){

    clearTrackRecords();

    trackInfo.indexPointer = pointer;

    datastream.read(reinterpret_cast<char*>(&(trackInfo.flag)),1);
    datastream.read(reinterpret_cast<char*>(&(trackInfo.cylinder)),2);
    datastream.read(reinterpret_cast<char*>(&(trackInfo.track)),2);
    datastream.read(reinterpret_cast<char*>(&(trackInfo.check)),2);

    char nextRecordFlag = 0x00; //A record flag of 0x1E signifies the end of user records on the track

    do {
        record* recPtr = new record();
        recPtr->initPtr = datastream.tellg();
        datastream.read(reinterpret_cast<char*>(&(recPtr->flag)),1);
        datastream.read(reinterpret_cast<char*>(&(recPtr->cylinder)),2);
        datastream.read(reinterpret_cast<char*>(&(recPtr->track)),2);
        datastream.read(reinterpret_cast<char*>(&(recPtr->recordNum)),1);
        datastream.read(reinterpret_cast<char*>(&(recPtr->keyLen)),1);
        datastream.read(reinterpret_cast<char*>(&(recPtr->dataLen)),2);
        datastream.read(reinterpret_cast<char*>(&(recPtr->check)),2);
        datastream.read(recPtr->key,recPtr->keyLen);
        recPtr->dataPtr = datastream.tellg();
        trackInfo.records.push_back(recPtr);
        datastream.seekg(recPtr->dataLen,std::fstream::cur);
        nextRecordFlag = datastream.peek();
    } while (nextRecordFlag != 0x1E);
    syncStreamPtrs(); //Returns read pointer to the beginning of the track
}

void ckddasd::seekRecord(){
    for (record* rec : trackInfo.records){
        if (rec->recordNum = ptrInfo.record){
            pointer = rec->initPtr;
            syncStreamPtrs();
            recordInfoPtr = rec;
            break;
        }
    }
}

void ckddasd::seekRecordData(){
    pointer = recordInfoPtr->dataPtr;
    syncStreamPtrs();
}

void ckddasd::seekRecordKey(){
    pointer = (recordInfoPtr->initPtr) + (recordInfoPtr->keyLen);
    syncStreamPtrs();
}

void ckddasd::seekKey(const char* seekkey){
    for (record* rec : trackInfo.records){
        if (std::strcmp(rec->key,seekkey) == 0){
            ptrInfo.record = rec->recordNum;
            pointer = rec->initPtr;
            syncStreamPtrs();
            recordInfoPtr = rec;
            break;
        }
    }
}

void ckddasd::seekNextRecord(){
    for (record* rec : trackInfo.records){
        if (rec->initPtr >= pointer){
            ptrInfo.record = rec->recordNum;
            seekRecord();
            return;
        }
    }
    ptrInfo.record = 0;
    seekRecord();
}