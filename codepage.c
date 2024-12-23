#include <codepage.h>

//Table for converting ASCII to EBCDIC
unsigned char atoecp[] = {
//  BCD     ASCII
    0x00,   //0x00
    0x01,   //0x01
    0x02,   //0x02
    0x03,   //0x03
    0x1A,   //0x04
    0x09,   //0x05
    0x1A,   //0x06
    0x7F,   //0x07
    0x1A,   //0x08
    0x1A,   //0x09
    0x1A,   //0x0A
    0x0B,   //0x0B
    0x0C,   //0x0C
    0x0D,   //0x0D
    0x0E,   //0x0E
    0x0F,   //0x0F
    0x10,   //0x10
    0x11,   //0x11
    0x12,   //0x12
    0x13,   //0x13
    0x3C,   //0x14
    0x3D,   //0x15
    0x32,   //0x16
    0x26,   //0x17
    0x18,   //0x18
    0x19,   //0x19
    0x3F,   //0x1A
    0x27,   //0x1B
    0x1C,   //0x1C
    0x1D,   //0x1D
    0x1E,   //0x1E
    0x1F,   //0x1F
    0x40,   //0x20
    0x4F,   //0x21
    0x7F,   //0x22
    0x7B,   //0x23
    0x5B,   //0x24
    0x6C,   //0x25
    0x50,   //0x26
    0x7D,   //0x27
    0x4D,   //0x28
    0x5D,   //0x29
    0x5C,   //0x2A
    0x4E,   //0x2B
    0x6B,   //0x2C
    0x60,   //0x2D
    0x4B,   //0x2E
    0x61,   //0x2F
    0xF0,   //0x30
    0xF1,   //0x31
    0xF2,   //0x32
    0xF3,   //0x33
    0xF4,   //0x34
    0xF5,   //0x35
    0xF6,   //0x36
    0xF7,   //0x37
    0xF8,   //0x38
    0xF9,   //0x39
    0x5E,   //0x3A
    0x4C,   //0x3B
    0x7E,   //0x3C
    0x6E,   //0x3D
    0x6F,   //0x3E
    0x7C,   //0x3F
    0xC1,   //0x40
    0xC2,   //0x41
    0xC3,   //0x42
    0xC4,   //0x43
    0xC5,   //0x44
    0xC6,   //0x45
    0xC7,   //0x46
    0xC8,   //0x47
    0xC9,   //0x48
};