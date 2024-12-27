#ifndef CKDDASD_H
#define CKDDASD_H

#include "iodevice.h"
#include <string>

class ckddasd : public iodevice {
    public:
    ckddasd(std::string dataName);
    ~ckddasd();
    void acceptCommand(byte opcode) override;
    void open() override;
    void close() override;
    byte generateSenseByte() override;
    std::stringstream* getDatastreamPtr() override;
    private:
    std::string filePath;
    std::stringstream datastream;
};

#endif