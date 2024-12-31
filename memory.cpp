#include <stdexcept>
#include <thread>
#include "memory.h"
#include <cstring>
#include <iostream>

struct block {
	byte core[2048];
	unsigned int key : 4;
    bool readProtection = false;
};

memory::memory(int blknum){
	for (int i=0;i<blknum;i++){
        block* blkptr = new block;
        blkptr->key = 0;
		blocks.push_back(blkptr);
	}
}

memory::~memory(){
    for (int i=0;i<blocks.size();i++){
        delete blocks[i];
    }
}

void memory::writeByteNoSync(word memaddr, byte data, unsigned int key){
    block* blkptr = getBlkPtr(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (key != blkptr->key && key != 0){throw 0x04;} //Storage protection
    blkptr->core[blkpos] = data;
}

void memory::writeByte(word memaddr, byte data, unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    writeByteNoSync(memaddr,data,key);
}

void memory::writeHalfwordNoSync(word memaddr, halfword data, unsigned int key){
    block* blkptr = getBlkPtr(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 2 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blkptr->key && key != 0){throw 0x04;} //Storage protection
    std::memcpy((blkptr->core + blkpos),reinterpret_cast<byte*>(&data),2);
}

void memory::writeHalfword(word memaddr, halfword data, unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    writeHalfwordNoSync(memaddr,data,key);
}

void memory::writeWordNoSync(word memaddr, word data, unsigned int key){
    block* blkptr = getBlkPtr(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 4 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blkptr->key && key != 0){throw 0x04;} //Storage protection
    std::memcpy((blkptr->core + blkpos),reinterpret_cast<byte*>(&data),4);
}

void memory::writeWord(word memaddr, word data, unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    writeWordNoSync(memaddr,data,key);
}

void memory::writeDoublewordNoSync(word memaddr, doubleword data, unsigned int key){
    block* blkptr = getBlkPtr(memaddr);;
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 8 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blkptr->key && key != 0){throw 0x04;} //Storage protection
    std::memcpy((blkptr->core + blkpos),reinterpret_cast<byte*>(&data),8);
}

void memory::writeDoubleword(word memaddr, doubleword data, unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    writeDoublewordNoSync(memaddr,data,key);
}

byte memory::getByteNoSync(word memaddr,unsigned int key){
    block* blkptr = getBlkPtr(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (key != blkptr->key && key != 0 && blkptr->readProtection){throw 0x04;}
    return blkptr->core[blkpos];
}

byte memory::getByte(word memaddr,unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    return getByteNoSync(memaddr,key);
}

halfword memory::getHalfwordNoSync(word memaddr,unsigned int key){
    block* blkptr = getBlkPtr(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 2 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blkptr->key && key != 0 && blkptr->readProtection){throw 0x04;} //Storage protection
    halfword out alignas(2);
    std::memcpy(reinterpret_cast<byte*>(&out),blkptr->core + blkpos,2);
    return out;
}

halfword memory::getHalfword(word memaddr,unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    return getHalfwordNoSync(memaddr,key);
}

word memory::getWordNoSync(word memaddr, unsigned int key){
    block* blkptr = getBlkPtr(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 4 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blkptr->key && key != 0 && blkptr->readProtection){throw 0x04;} //Storage protection
    word out alignas(4);
    std::memcpy(reinterpret_cast<byte*>(&out),(blkptr->core + blkpos),4);
    return out;
}

word memory::getWord(word memaddr,unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    return getWordNoSync(memaddr,key);
}

doubleword memory::getDoublewordNoSync(word memaddr, unsigned int key){
    block* blkptr = getBlkPtr(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 8 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blkptr->key && key != 0 && blkptr->readProtection){throw 0x04;} //Storage protection
    doubleword out alignas(8);
    std::memcpy(reinterpret_cast<byte*>(&out),(blkptr->core + blkpos),8);
    return out;
}

doubleword memory::getDoubleword(word memaddr,unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    return getDoublewordNoSync(memaddr,key);
}

void memory::setKey(word blockaddr,unsigned int newKey){
    getBlkPtr(blockaddr)->key = newKey;
}

unsigned int memory::getKey(word blockaddr){
    return getBlkPtr(blockaddr)->key;
}

bool memory::getReadProtection(word blockaddr){
    return getBlkPtr(blockaddr)->readProtection;
}

void memory::enableReadProtection(word blockaddr, bool enabled){
    getBlkPtr(blockaddr)->readProtection = enabled;
}

//Returns pointer to the block of a physical memory address
block* memory::getBlkPtr(word memaddr){
    try {
        return blocks[(memaddr / (1 << 11))]; 
    } catch (std::out_of_range err) {
        throw 0x05; //Throws addressing exception
    }
}
//Returns the position within the block of the physical memory address
int memory::getBlkPos(word memaddr){
	return memaddr % (1 << 11);
}

