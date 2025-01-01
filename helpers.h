#ifndef HELPERS_H
#define HELPERS_H

#include <cstdint>
#include <concepts>
#include <bit>
#include <algorithm>
#include <array>

/*-----------------------------------------------------------------*/
/* STANDARD DATA SIZES                                             */
/*-----------------------------------------------------------------*/

using doubleword = uint64_t;
using word = uint32_t;
using halfword = uint16_t;
using byte = uint8_t;

/*-----------------------------------------------------------------*/
/* SHIFTS                                                          */
/*-----------------------------------------------------------------*/

constexpr uint16_t LSHFT8  = (1 << 8);
constexpr uint32_t LSHFT16 = (1 << 16);
constexpr uint64_t LSHFT32 = ((uint64_t)1 << 32);

/*-----------------------------------------------------------------*/
/* MOST NEGATIVE NUMBERS                                           */
/*-----------------------------------------------------------------*/

constexpr uint64_t MAXNEG_64 = 0b1000000000000000000000000000000000000000000000000000000000000000; //Most negative int64, expressed as an unsigned int64
constexpr uint32_t MAXNEG_32 = 0b10000000000000000000000000000000; //Most negative int32, expressed as an unsigned int32
constexpr uint16_t MAXNEG_16 = 0b1000000000000000; //Most negative int16, expressed as an unsigned int16
constexpr uint8_t  MAXNEG_8  = 0b10000000; //Most negative int8, expressed as an unsigned int8

/*-----------------------------------------------------------------*/
/* BITWISE UTILITIES                                               */
/*-----------------------------------------------------------------*/

inline uint8_t leftNibble(byte in){return in/16;}
inline uint8_t rightNibble(byte in){return in%16;}

inline byte leftByte(halfword in){return (in/LSHFT8);}
inline byte rightByte(halfword in){return (in%LSHFT8);}

inline halfword leftHalfword(word in){return (in/LSHFT16);}
inline halfword rightHalfword(word in){return (in%LSHFT16);}

inline word leftWord(doubleword in){return (word)(in/LSHFT32);}
inline word rightWord(doubleword in){return (word)(in%LSHFT32);}

template<std::integral T> constexpr T byteswap(T value) noexcept
{
    static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits");
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    std::ranges::reverse(value_representation);
    return std::bit_cast<T>(value_representation);
}

#endif