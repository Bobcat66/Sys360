#include "streamdevice.h"

//todo: work on stream device
/*-----------------------------------------------------------------*/
/* PUBLIC                                                          */
/*-----------------------------------------------------------------*/

stdioDevice::stdioDevice() {}

void stdioDevice::acceptCommand(byte opcode) {
    this->opcode = opcode;
    _service_in = true;
}

void stdioDevice::acceptChar(char data) {
    if (data == '\0') {_service_in = false;}
    std::cout << data;
}

char stdioDevice::getChar() {
    char c;
    std::cin >> c;
    return c;
}

void stdioDevice::halt(){}
void stdioDevice::open(){}
void stdioDevice::close(){}

bool stdioDevice::service_in() {
    return _service_in;
}
/*-----------------------------------------------------------------*/
/* PRIVATE                                                         */
/*-----------------------------------------------------------------*/