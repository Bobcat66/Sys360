#include <stdexcept>
#include <thread>
#include "memory.h"
#include <cstring>
#include <iostream>
#include <bit>


struct block {
	byte* core;
	unsigned int key : 4;
    bool readProtection;
};

memory::memory(int blknum){
	for (int i=0;i<blknum;i++){
        byte* bcoreptr = (byte*)calloc(2048,1); //Use calloc to initialize all members to zero, in order to avoid garbage values
		blocks.push_back({bcoreptr,0,false});
	}
}

memory::~memory(){
    for (int i=0;i<blocks.size();i++){
        free(blocks[i].core);
    }
}

void memory::writeByteNoSync(word memaddr, byte data, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (key != blk.key && key != 0){throw 0x04;} //Storage protection
    blk.core[blkpos] = data;
}

void memory::writeByte(word memaddr, byte data, unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    writeByteNoSync(memaddr,data,key);
}

void memory::writeHalfword(word memaddr, halfword data, unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    writeHalfwordNoSync(memaddr,data,key);
}

void memory::writeWord(word memaddr, word data, unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    writeWordNoSync(memaddr,data,key);
}

void memory::writeDoubleword(word memaddr, doubleword data, unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    writeDoublewordNoSync(memaddr,data,key);
}

byte memory::getByteNoSync(word memaddr,unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (key != blk.key && key != 0 && blk.readProtection){throw 0x04;}
    return blk.core[blkpos];
}

byte memory::getByte(word memaddr,unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    return getByteNoSync(memaddr,key);
}

halfword memory::getHalfword(word memaddr,unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    return getHalfwordNoSync(memaddr,key);
}

word memory::getWord(word memaddr,unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    return getWordNoSync(memaddr,key);
}

doubleword memory::getDoubleword(word memaddr,unsigned int key){
    std::lock_guard<std::mutex> memguard(mtx);
    return getDoublewordNoSync(memaddr,key);
}

void memory::setKey(word blockaddr,unsigned int newKey){
    getBlk(blockaddr).key = newKey;
}

unsigned int memory::getKey(word blockaddr){
    return getBlk(blockaddr).key;
}

bool memory::getReadProtection(word blockaddr){
    return getBlk(blockaddr).readProtection;
}

void memory::enableReadProtection(word blockaddr, bool enabled){
    getBlk(blockaddr).readProtection = enabled;
}

void memory::coredump(word blockaddr){
    std::lock_guard<std::mutex> memLock(mtx);
    block blk = getBlk(blockaddr);
    std::cout << "CORE DUMP OF BLOCK " << (blockaddr/ (1 << 11)) << std::endl;
    for (int i = 0; i < 2048; i++) {
        if (i % 16 == 0) {
            printf("\n%03X: ",(unsigned int)i);
        }
        printf("%02X ", (unsigned int)*(blk.core + i));
    }
    std::cout << std::endl;
}

//Returns pointer to the block of a physical memory address
block& memory::getBlk(word memaddr){
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

/*-----------------------------------------------------------------*/
/* ARCHITECTURE DEPENDENT                                          */
/*-----------------------------------------------------------------*/

//NOTE: MIDDLE-ENDIAN SYSTEMS ARE NOT SUPPORTED!

#if __BYTE_ORDER__ ==  __ORDER_LITTLE_ENDIAN__

void memory::writeHalfwordNoSync(word memaddr, halfword data, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 2 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0){throw 0x04;} //Storage protection
    data = byteswap(data);
    std::memcpy((blk.core + blkpos),reinterpret_cast<byte*>(&data),2);
}

void memory::writeWordNoSync(word memaddr, word data, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 4 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0){throw 0x04;} //Storage protection
    data = byteswap(data);
    std::memcpy((blk.core + blkpos),reinterpret_cast<byte*>(&data),4);
}

void memory::writeDoublewordNoSync(word memaddr, doubleword data, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 8 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0){throw 0x04;} //Storage protection
    data = byteswap(data);
    std::memcpy((blk.core + blkpos),reinterpret_cast<byte*>(&data),8);
}

halfword memory::getHalfwordNoSync(word memaddr,unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 2 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0 && blk.readProtection){throw 0x04;} //Storage protection
    halfword out alignas(2);
    std::memcpy(reinterpret_cast<byte*>(&out),blk.core + blkpos,2);
    out = byteswap(out);
    return out;
}

word memory::getWordNoSync(word memaddr, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 4 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0 && blk.readProtection){throw 0x04;} //Storage protection
    word out alignas(4);
    std::memcpy(reinterpret_cast<byte*>(&out),(blk.core + blkpos),4);
    out = byteswap(out);
    return out;
}

doubleword memory::getDoublewordNoSync(word memaddr, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 8 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0 && blk.readProtection){throw 0x04;} //Storage protection
    doubleword out alignas(8);
    std::memcpy(reinterpret_cast<byte*>(&out),(blk.core + blkpos),8);
    out = byteswap(out);
    return out;
}

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

void memory::writeHalfwordNoSync(word memaddr, halfword data, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 2 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0){throw 0x04;} //Storage protection
    std::memcpy((blk.core + blkpos),reinterpret_cast<byte*>(&data),2);
}

void memory::writeWordNoSync(word memaddr, word data, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 4 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0){throw 0x04;} //Storage protection
    std::memcpy((blk.core + blkpos),reinterpret_cast<byte*>(&data),4);
}

void memory::writeDoublewordNoSync(word memaddr, doubleword data, unsigned int key){
    block blk = getBlk(memaddr);;
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 8 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0){throw 0x04;} //Storage protection
    std::memcpy((blk.core + blkpos),reinterpret_cast<byte*>(&data),8);
}

halfword memory::getHalfwordNoSync(word memaddr,unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 2 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0 && blk.readProtection){throw 0x04;} //Storage protection
    halfword out alignas(2);
    std::memcpy(reinterpret_cast<byte*>(&out),blk.core + blkpos,2);
    return out;
}

word memory::getWordNoSync(word memaddr, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 4 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0 && blk.readProtection){throw 0x04;} //Storage protection
    word out alignas(4);
    std::memcpy(reinterpret_cast<byte*>(&out),(blk.core + blkpos),4);
    return out;
}

doubleword memory::getDoublewordNoSync(word memaddr, unsigned int key){
    block blk = getBlk(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (memaddr % 8 != 0){throw 0x06;} //Checks for integral boundary
    if (key != blk.key && key != 0 && blk.readProtection){throw 0x04;} //Storage protection
    doubleword out alignas(8);
    std::memcpy(reinterpret_cast<byte*>(&out),(blk.core + blkpos),8);
    return out;
}

#endif

