#ifndef HELPERS_H
#define HELPERS_H

#include <cstdint>

using doubleword = uint64_t;
using word = uint32_t;
using halfword = uint16_t;
using byte = uint8_t;
//Most negative constexprs
constexpr doubleword MAXNEG_64 =  0b1000000000000000000000000000000000000000000000000000000000000000; //Most negative int64, expressed as an unsigned int64
constexpr word MAXNEG_32 =        0b10000000000000000000000000000000; //Most negative int32, expressed as an unsigned int32
constexpr halfword MAXNEG_16 =    0b1000000000000000; //Most negative int16, expressed as an unsigned int16
constexpr byte MAXNEG_8 =         0b10000000; //Most negative int8, expressed as an unsigned int8

inline uint8_t leftNibble(byte in){return in/16;}
inline uint8_t rightNibble(byte in){return in%16;}

inline byte leftByte(halfword in){return (byte)(in/(1 << 8));}
inline byte rightByte(halfword in){return (byte)(in%(1 << 8));}

inline halfword leftHalfword(word in){return (halfword)(in/(1 << 16));}
inline halfword rightHalfword(word in){return (halfword)(in%(1 << 16));}

inline word leftWord(doubleword in){return (word)(in/(doubleword)(1 << 32));}
inline word rightWord(doubleword in){return (word)(in%(doubleword)(1 << 32));}

#endif