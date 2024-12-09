#ifndef CHANNEL_H
#define CHANNEL_H

#include "helpers.h"
#include "memory.h"
#include <vector>
#include <iostream>
#include <unordered_map>

struct channelstatus {
    unsigned int key : 4; //Storage Protection Key
    unsigned int pc : 24; //Program counter
    unsigned int usc : 8; //Unit status conditions
    unsigned int csc : 8; //Channel status conditions
    unsigned int count : 16;
};

class channel {
    public:
    channel(memory* coreptr);
    ~channel();
    void addDevice(int addr, std::iostream* devptr);
    private:
    channelstatus csw;
    memory* coreptr;
    std::unordered_map<int,std::iostream*> devices;
    std::vector<byte> inBuffer;
    std::vector<byte> outBuffer;
    void readDevToIBuf(int devaddr,int numbytes);
    void writeOBufToDev(int devaddr,int numbytes);
    void writeIBufToCore(word memaddr,int numbytes);
    void readCoreToOBuf(word memaddr,int numbytes);
    void fetchCAW();
    void cycle();
};
#endif