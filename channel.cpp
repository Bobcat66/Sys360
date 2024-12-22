#include <mutex>
#include <thread>
#include "channel.h"

channel::channel(std::shared_ptr<memory> coreptr){
    this->coreptr = coreptr;
    csw = {0,0,0,0,0};
}

void channel::addDevice(int addr, iodevice* devptr){
    devices[addr] = devptr;
}

void channel::fetchCAW(){
    std::lock_guard<std::mutex> memlock(coreptr->mtx);
    csw.key = coreptr->getByte((word)72,0);
    csw.pc = ((word)(coreptr->getByte((word)74,0)) << 16) + ((word)(coreptr->getByte((word)75,0)) << 8) + ((word)(coreptr->getByte((word)76,0)));
}

void channel::readDevToBuf(int devaddr, int numchars){
    auto devStreamPtr = devices.at(devaddr);
    devStreamPtr->read(buffer,numchars);
}

void channel::writeBufToDev(int devaddr, int numchars){
    auto devStreamPtr = devices.at(devaddr);
    devStreamPtr->write(buffer,numchars);
}

void channel::writeBufToCore(word memaddr, int numbytes){
    std::lock_guard<std::mutex> memlock(coreptr->mtx);
    for (int i = 0; i < numbytes; i++){
        coreptr->writeByte(memaddr + i,(byte) buffer[i],csw.key);
    }
}

void channel::readCoreToBuf(word memaddr, int numbytes){
    std::lock_guard<std::mutex> memlock(coreptr->mtx);
    for (int i = 0; i < numbytes; i++){
        buffer[i] = (char) coreptr->getByte(memaddr+i,csw.key);
    }
}

doubleword channel::fetchCCW(){
    std::lock_guard<std::mutex> memlock(coreptr->mtx);
    doubleword ccw = 
    ((doubleword)(coreptr->getByte((word)csw.pc,0)) << 56) + 
    ((doubleword)(coreptr->getByte((word)csw.pc+1,0)) << 48) + 
    ((doubleword)(coreptr->getByte((word)csw.pc+2,0)) << 40) + 
    ((doubleword)(coreptr->getByte((word)csw.pc+3,0)) << 32) +
    ((doubleword)(coreptr->getByte((word)csw.pc+4,0)) << 24) + 
    ((doubleword)(coreptr->getByte((word)csw.pc+5,0)) << 16) + 
    ((doubleword)(coreptr->getByte((word)csw.pc+6,0)) << 8) + 
    ((doubleword)(coreptr->getByte((word)csw.pc+7,0)));
    csw.pc += 8;
    return ccw;
}

void channel::cycle(){
    doubleword ccw = fetchCCW();
    
}

//TODO: Add interruptions and exception handling
void channel::run(){
    cycle();
}

void channel::startIO(int devaddr){
    this->devaddr = devaddr;
    std::thread iothread(&channel::run,this);
}





