#include <cstring>
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
};

ckddasd::ckddasd(std::string dataName){
    this->dataName = dataName;
    datastream.open(dataName,std::fstream::in | std::fstream::binary);
    datastream.read(reinterpret_cast<char*>(&header),8);
    datastream.close();
}

ckddasd::~ckddasd(){
    clearTrackRecords();
}

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

void ckddasd::seekBin(){
    int binSize = getTsize(header) * getTracks(header) * getCylinders(header);//hdrInfo.tsize * hdrInfo.tracks * hdrInfo.cylinders;
    pointer = ptrInfo.bin * binSize + 8;
    syncStreamPtrs();
}

void ckddasd::seekCylinder(){
    int cylinderSize = getTsize(header) * getTracks(header);
    pointer += ptrInfo.cylinder * cylinderSize;
    syncStreamPtrs();
}

void ckddasd::seekTrack(){
    int trackSize = getTsize(header);
    pointer += ptrInfo.track * trackSize;
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

void ckddasd::loadTrack(){

    clearTrackRecords();

    seekBin();
    seekCylinder();
    seekTrack();

    trackInfo.indexPointer = pointer;

    datastream.read(reinterpret_cast<char*>(&(trackInfo.flag)),1);
    datastream.read(reinterpret_cast<char*>(&(trackInfo.cylinder)),2);
    datastream.read(reinterpret_cast<char*>(&(trackInfo.track)),2);
    datastream.read(reinterpret_cast<char*>(&(trackInfo.check)),2);

    char nextRecordFlag = 0x00; //A record flag of 0x1E signifies the end of user records on the track

    do {
        record* recPtr = new record();
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
            pointer = rec->dataPtr;
            syncStreamPtrs();
            break;
        }
    }
}

void ckddasd::seekKey(const char* seekkey){
    for (record* rec : trackInfo.records){
        if (std::strcmp(rec->key,seekkey) == 0){
            pointer = rec->dataPtr;
            syncStreamPtrs();
            break;
        }
    }
}