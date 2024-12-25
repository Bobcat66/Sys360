#ifndef CPUHELPERS_H
#define CPUHELPERS_H
#include "helpers.h"
#include <string>
#define LOAD_RR_FIELDS uint8_t R1 = leftNibble(b1); uint8_t R2 = rightNibble(b1);
#define LOAD_RX_FIELDS byte R1 = leftNibble(b1); byte X1 = rightNibble(b1); byte B2 = baseRegister(word1); byte D2 = displacement(word1);
#define LOAD_RS_FIELDS byte R1 = leftNibble(b1); byte R3 = rightNibble(b1); byte B2 = baseRegister(word1); byte D2 = displacement(word1);


inline halfword displacement(halfword in){return in%(1<<12);}
inline uint8_t baseRegister(halfword in){return in/(1<<12);}

//Checks for overflow in addition between two 32 bit integers
inline bool chkOverflow(word add1, word add2){return ((add1 & MAXNEG_32) != (add2 & MAXNEG_32)) && (((add1 + add2) & MAXNEG_32) != (add1 & MAXNEG_32));}
//Checks for carry in logical addition between two 32 bit unsigned integers
inline bool chkCarry(word add1, word add2){return add1 > (INT32_MAX - add2);}

class cpu;

struct instruction {
    const unsigned int pst : 1; //0 = privileged instruction, 1 = unprivileged instruction
    const std::string name;
    void (*run)(cpu *cpuptr,byte b1,halfword word1,halfword word2);
};



#endif