#include <iostream>
#include <unordered_map>
#include "helpers.h"
#include "memory.h"

using std::iostream;

class Channel {
    public:
    Channel(byte *core){
        coreptr = core;
    }
    private:

    byte* coreptr; //Pointer to CPU core memory

    int inBufferLen;
    char* inBuffer;

    int outBufferLen;
    char* outBuffer;

    std::unordered_map<int,iostream*> devices;

    /*Reads bytes from device to inbuffer. TODO: Add error handling*/
    void readDevToIBuf(int address,int numchars){
        auto readStream = devices.at(address);
        readStream->read(inBuffer,numchars);
    }

    /*Writes bytes from outbuffer to device. TODO: Add error handling*/
    void writeOBufToDev(int address,int numchars){
        auto writeStream = devices.at(address);
        writeStream->write(outBuffer,numchars);
    }

    /*Writes bytes to CPU memory from inBuffer. TODO: Add error handling*/
    void writeIBufToMem(word memAddr,int numchars){
        for(int i = 0; i < numchars; i++){
            *(coreptr + memAddr + i) = (byte)*(inBuffer + i);
        }
    }

    /*Reads bytes from CPU memory to outBuffer. TODO: Add error handling*/
    void readMemToOBuf(word memAddr,int numchars){
        for(int i = 0; i < numchars; i++){
            *(outBuffer + i) = (char)*(coreptr + memAddr + i);
        }
    }
    /*Resizes inBuffer*/
    void rszIBuf(int size){
        delete[] inBuffer;
        inBufferLen = size;
        inBuffer = new char[size];
    }

    /*Resizes outBuffer*/
    void rszOBuf(int size){
        delete[] outBuffer;
        outBufferLen = size;
        outBuffer = new char[size];
    }
};
