#include "ckddasd.h"

inline halfword getBins(doubleword header){return header / (1 << 48);}
inline halfword getCylinders(doubleword header){return (header % (1 << 48)) / (1 << 32);}
inline halfword getTracks(doubleword header){return (header % (1 << 32)) / (1 << 16);}
inline halfword getTsize(doubleword header){return header % (1 << 16);}

inline doubleword getRecordAddr(doubleword count){return count/(1 << 3);}

ckddasd::ckddasd(std::string dataName){
    this->dataName = dataName;
    datastream.open(dataName,std::fstream::in | std::fstream::binary);
    datastream.read(reinterpret_cast<char*>(&header),8);
    datastream.close();
}

void ckddasd::seekBin(){
    int binSize = getTsize(header) * getTracks(header) * getCylinders(header);//hdrInfo.tsize * hdrInfo.tracks * hdrInfo.cylinders;
    if (writeMode) {
        datastream.seekp((ptrInfo.bin * binSize) + 8);
    } else {
        datastream.seekg((ptrInfo.bin * binSize) + 8);
    }
}

void ckddasd::seekCylinder(){
    int cylinderSize = getTsize(header) * getTracks(header);
    seekBin();
    if (writeMode) {
        datastream.seekp(datastream.tellp() + (ptrInfo.cylinder * cylinderSize));
    } else {
        datastream.seekg(datastream.tellg() + (ptrInfo.cylinder * cylinderSize));
    }
}

void ckddasd::seekTrack(){
    int trackSize = getTsize(header);
    seekCylinder();
    if (writeMode) {
        datastream.seekp(datastream.tellp() + (ptrInfo.track * trackSize));
    } else {
        datastream.seekg(datastream.tellg() + (ptrInfo.track * trackSize));
    }
}

void ckddasd::seekRecord(){
    seekTrack();
    doubleword count;
    char* key;
    datastream.read(reinterpret_cast<char*>(&count),8);
}