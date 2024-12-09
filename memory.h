#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include "helpers.h"

struct block;

class memory {
    public:
    memory(int blknum);
    ~memory();
    byte getByte(word memaddr,unsigned int key);
    void writeByte(word memaddr,byte data,unsigned int key);
    void setKey(word blockaddr,unsigned int newkey);
    unsigned int getKey(word blockaddr);
    bool getReadProtection(word blockaddr);
    void enableReadProtection(word blockaddr,bool enabled);
    private:
    std::vector<block*> blocks;
    block* getBlkPtr(word memaddr);
    int getBlkPos(word memaddr);
};
#endif