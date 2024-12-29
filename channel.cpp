#include "channel.h"

/*-----------------------------------------------------------------*/
/* PUBLIC                                                          */
/*-----------------------------------------------------------------*/

channel::channel(std::shared_ptr<memory> memPtr,const byte channelAddress)
:channelID(channelAddress){
    core = memPtr;
}

void channel::addSubchannel(byte subchannelID) {
    subchannels.emplace(std::make_pair(subchannelID,subchannel(subchannelID,core)));
}

void channel::addDevice(deviceAddress devaddr,iodevice* devptr) {
    subchannels[devaddr.subchannel].addDevice(devaddr.device,devptr);
}

int channel::startIO(deviceAddress devaddr) {
    return subchannels[devaddr.subchannel].startChannelProgram(devaddr.device,fetchCAW()%(1<<24),fetchCAW()/(1<<29));
}

int channel::haltIO(deviceAddress devaddr) {

}

std::optional<halfword> channel::getPendingInterrupts() {

}

/*-----------------------------------------------------------------*/
/* PRIVATE                                                         */
/*-----------------------------------------------------------------*/

void channel::storeCSW() {

}

word channel::fetchCAW() {
    return core->getWord(72,0);
}