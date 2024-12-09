
#include "channel.h"

channel::channel(memory* coreptr){
    this->coreptr = coreptr;
    csw = {0,0,0,0,0};
};

void channel::addDevice(int addr, std::iostream* devptr){
    devices[addr] = devptr;
}

void channel::fetchCAW(){
    csw.key = coreptr->getByte((word)72,0);
    csw.pc = ((word)(coreptr->getByte((word)74,0)) << 16) + ((word)(coreptr->getByte((word)75,0)) << 8) + ((word)(coreptr->getByte((word)76,0)));
}






