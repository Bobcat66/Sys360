#include "channel.h"

/*-----------------------------------------------------------------*/
/* PUBLIC                                                          */
/*-----------------------------------------------------------------*/

channel::channel(std::shared_ptr<memory> memPtr,const byte channelAddress)
:channelID(channelAddress){
    core = memPtr;
}

channel::~channel(){
    for (auto& [key,value] : subchannels) {
        delete value;
    }
}

void channel::addSubchannel(byte subchannelID) {
    subchannels.insert(std::make_pair(subchannelID,new subchannel(subchannelID,core)));
}

void channel::addDevice(deviceAddress devaddr,iodevice* devptr) {
    subchannels[devaddr.subchannelID]->addDevice(devaddr.deviceID,devptr);
}

int channel::startIO(deviceAddress devaddr) {
    return subchannels[devaddr.subchannelID]->startChannelProgram(devaddr.deviceID,fetchCAW()%(1<<24),fetchCAW()/(1<<29));
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