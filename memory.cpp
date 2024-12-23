#include <stdexcept>
#include <thread>
#include "memory.h"

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

void memory::writeByte(word memaddr, byte data, unsigned int key){
    block* blkptr = getBlkPtr(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (key != blkptr->key && key != 0){
        throw 0x04;
    }
    blkptr->core[blkpos] = data;
}

byte memory::getByte(word memaddr,unsigned int key){
    block* blkptr = getBlkPtr(memaddr);
    int blkpos = getBlkPos(memaddr);
    if (key != blkptr->key && key != 0 && blkptr->readProtection){
        throw 0x04;
    }
    return blkptr->core[blkpos];
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

