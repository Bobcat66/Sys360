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
    void startIO(int devaddr);
    word packCSW();
    private:
    int devaddr;
    channelstatus csw;
    memory* coreptr;
    std::unordered_map<int,std::iostream*> devices;
    char buffer[256];
    void readDevToBuf(int devaddr,int numchars);
    void writeBufToDev(int devaddr,int numchars);
    void writeBufToCore(word memaddr,int numbytes);
    void readCoreToBuf(word memaddr,int numbytes);
    void fetchCAW();
    doubleword fetchCCW(); //Fetches channel command word
    void cycle();
};
#endif