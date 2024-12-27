#ifndef IOHELPERS_H
#define IOHELPERS_H

#include <atomic>

//A struct for interfacing with physical hardware over a parallel interface
typedef volatile struct busTagStruct {

    char bus_in;
    char bus_out;

    unsigned int address_in : 1;
    unsigned int address_out : 1;
    unsigned int status_in : 1;
    unsigned int command_out : 1;
    unsigned int service_in : 1;
    unsigned int service_out : 1;

    unsigned int operation_out : 1;
    unsigned int operational_in : 1;
    unsigned int hold_out : 1;
    unsigned int select_out : 1;
    unsigned int select_in : 1;
    unsigned int suppress_out : 1;
    unsigned int request_in : 1;

} dataline;

#endif