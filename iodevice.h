#ifndef IODEVICE_H
#define IODEVICE_H

#include "helpers.h"
#include <sstream>
#include <atomic>

class iodevice {
    public:
    virtual ~iodevice() = default;
    virtual void acceptCommand(byte opcode) = 0; //Runs the iodevice on a separate thread
    virtual void open() = 0; //Opens device for IO
    virtual void close() = 0; //Closes device
    virtual byte generateSenseByte() = 0;
    virtual std::stringstream* getDatastreamPtr() = 0;
    //std::atomic_bool service_in; //A flag that signals the Channel, either to request data, or to signal that data is being transferred from the iodevice
    std::mutex mtx; //Mutex to control thread access to data
};

#endif