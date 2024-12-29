#include "streamdevice.h"


/*-----------------------------------------------------------------*/
/* PUBLIC                                                          */
/*-----------------------------------------------------------------*/

stdioDevice::stdioDevice() {}

void stdioDevice::acceptCommand(byte opcode) {
    this->opcode = opcode;
}

void stdioDevice::acceptByte(byte data) {
    std::cout << (char) data;
}

byte stdioDevice::getByte() {
    char c;
    std::cin >> c;
    return (byte) c;
}

void stdioDevice::open(){}
void stdioDevice::close(){}

bool stdioDevice::service_in() {
    return true;
}
/*-----------------------------------------------------------------*/
/* PRIVATE                                                         */
/*-----------------------------------------------------------------*/