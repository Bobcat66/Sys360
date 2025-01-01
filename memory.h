#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <array>
#include <mutex>
#include "helpers.h"

struct block;

class memory {
    public:
    memory(int blknum);
    ~memory();
    std::mutex mtx;
    byte getByte(word memaddr,unsigned int key);
    halfword getHalfword(word memaddr,unsigned int key);
    word getWord(word memaddr,unsigned int key);
    doubleword getDoubleword(word memaddr,unsigned int key);
    byte getByteNoSync(word memaddr,unsigned int key);
    halfword getHalfwordNoSync(word memaddr,unsigned int key);
    word getWordNoSync(word memaddr,unsigned int key);
    doubleword getDoublewordNoSync(word memaddr,unsigned int key);
    void writeByte(word memaddr,byte data,unsigned int key);
    void writeHalfword(word memaddr,halfword data,unsigned int key);
    void writeWord(word memaddr,word data,unsigned int key);
    void writeDoubleword(word memaddr,doubleword data,unsigned int key);
    void writeByteNoSync(word memaddr,byte data,unsigned int key);
    void writeHalfwordNoSync(word memaddr,halfword data,unsigned int key);
    void writeWordNoSync(word memaddr,word data,unsigned int key);
    void writeDoublewordNoSync(word memaddr,doubleword data,unsigned int key);
    void coredump(word blockaddr); //Dumps a block of memory to std::cout
    void setKey(word blockaddr,unsigned int newkey);
    unsigned int getKey(word blockaddr);
    bool getReadProtection(word blockaddr);
    void enableReadProtection(word blockaddr,bool enabled);
    private:
    std::vector<block> blocks;
    block& getBlk(word memaddr);
    int getBlkPos(word memaddr);
};
#endif