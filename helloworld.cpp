#include "cpu.h"
#include "memory.h"
#include "channel.h"
#include "subchannel.h"
#include "streamdevice.h"
#include "stdisa.h"
#include <fstream>
#include <memory>

//Build command: g++ -std=c++20 -o helloworld.out helloworld.cpp cpu.cpp channel.cpp subchannel.cpp stdisa.cpp cpuclock.cpp memory.cpp stdiodevice.cpp encodingUTF8.cpp

int main() {
    std::shared_ptr<memory> mem = std::make_shared<memory>(20);
    /* Channel Code */
    mem->writeWord(72,0x00000068,0);
    mem->writeDoubleword(104,0x0D0003E80000000D,0);
    std::cout << "CHANNEL CODE LOADED" << std::endl;
    /* Program Code */
    mem->writeDoubleword(0x418,0x5A1003FC5A100404,0);
    mem->writeDoubleword(0x420,0x5A1004085A100408,0);
    mem->writeDoubleword(0x428,0x421003ED5A100408,0);
    mem->writeDoubleword(0x430,0x421003F35B100408,0);
    mem->writeDoubleword(0x438,0x5A1003F8421003F8,0);
    mem->writeDoubleword(0x440,0x5A100404421003EE,0);
    mem->writeDoubleword(0x448,0x5B100400421003E8,0);
    mem->writeDoubleword(0x450,0x5B1004085B100408,0);
    mem->writeDoubleword(0x458,0x5B100408421003E9,0);
    mem->writeDoubleword(0x460,0x5A100400421003EC,0);
    mem->writeDoubleword(0x468,0x421003EF5B100408,0);
    mem->writeDoubleword(0x470,0x421003F25A100408,0);
    mem->writeDoubleword(0x478,0x5A1004045A100408,0);
    mem->writeDoubleword(0x480,0x5A100408421003EA,0);
    mem->writeDoubleword(0x488,0x421003EB421003F1,0);
    mem->writeDoubleword(0x490,0x9C10000000000000,0);
    std::cout << "PROGRAM CODE LOADED" << std::endl;
    /* Memory Initialization */
    mem->writeWord(0x3F8,0x00000032,0);
    mem->writeWord(0x3FC,0x00000019,0);
    mem->writeWord(0x400,0x0000000A,0);
    mem->writeWord(0x404,0x00000005,0);
    mem->writeWord(0x408,0x00000001,0);
    std::cout << "MEMORY INITIALIZATION COMPLETE" << std::endl;
    cpu tcpu(mem,stdisa,std::cout);
    tcpu.setAddr(0x00000418);
    tcpu.setVerbose(true);
    channel tchannel(mem,0);
    stdioDevice tdev;
    tchannel.addSubchannel(0);
    tchannel.addDevice({0,0},&tdev);
    tcpu.registerChannel(0,tchannel);
    std::cout << "BOOTING VIRTUAL SYSTEM" << std::endl;
    tcpu.test(30);
}

/* HELLO WORLD PROGRAM

MEMORY

--CHANNEL PROGRAM--
CAW: 100 (DEC)

DEC: HEX
100-107: 0D0003E80000000D

DEC: CHAR DEC
1000: H 72 -
1001: E 69 -
1002: L 76 -
1003: L 76 -
1004: O 79 -
1005:   32 -
1006: W 87 -
1007: O 79 -
1008: R 82 -
1009: L 76 -
1010: D 68 -
1011: ! 33 -
1012: \0
1016-1019: 50
1020-1023: 25
1024-1027: 10
1028-1031: 5
1032-1035: 1

-- PSEUDOCODE --

ADD 1020 (25) TO R1 (0)
ADD 1028 (5) TO R1 (25)
ADD 1032 (1) TO R1 (30)
ADD 1032 (1) TO R1 (31)
STORE CHAR R1 (32) AT 1005 (0)
ADD 1032 (1) TO R1 (32)
STORE CHAR R1 (33) AT 1011 (0)
SUB 1032 (1) FROM R1 (33)
ADD 1016 (50) TO R1 (32)
STORE CHAR R1 (82) TO 1008 (0)
ADD 1028 (5) TO R1 (82)
STORE CHAR R1 (87) AT 1006
SUB 1024 (10) FROM R1 (82)
STORE CHAR R1 (72) AT 1000 (0)
SUB 1032 (1) FROM R1 (72)
SUB 1032 (1) FROM R1 (71)
SUB 1032 (1) FROM R1 (70)
STORE CHAR R1 (69) AT 1001 (0)
ADD 1024 (10) TO R1 (69)
STORE CHAR R1 (79) AT 1004 (0)
STORE CHAR R1 (79) AT 1007 (0)
SUB 1032 (1) FROM R1 (69)
STORE CHAR R1 (68) AT 1010 (0)
ADD 1032 (1) TO R1 (68)
ADD 1028 (5) TO R1 (69)
ADD 1032 (1) TO R1 (74)
ADD 1032 (1) TO R1 (75)
STORE CHAR R1 (76) AT 1002 (0)
STORE CHAR R1 (76) AT 1003 (0)
STORE CHAR R1 (76) AT 1009 (0)
START I/O

-- ASSEMBLY --

A       R1      R0 3FC
A       R1      R0 404
A       R1      R0 408
A       R1      R0 408
STC     R1      R0 3ED
A       R1      R0 408
STC     R1      R0 3F3
S       R1      R0 408
A       R1      R0 3F8
STC     R1      R0 3F0
A       R1      R0 404
STC     R1      R0 3EE
S       R1      R0 400
STC     R1      R0 3E8
S       R1      R0 408
S       R1      R0 408
S       R1      R0 408
STC     R1      R0 3E9
A       R1      R0 400
STC     R1      R0 3EC
STC     R1      R0 3EF
S       R1      R0 408
STC     R1      R0 3F2
A       R1      R0 408
A       R1      R0 404
A       R1      R0 408
A       R1      R0 408
STC     R1      R0 3EA
STC     R1      R0 3EB
STC     R1      R0 3F1
SIO             R0 000

-- MACHINE CODE --

HALFWORD  | ADR,DOUBLEWORD (2 INSTRUCTIONS PER DOUBLEWORD)
----------+-----------------
5A10 03FC | 406,5A1003FC5A100404
5A10 0404 |
5A10 0408 | 40E,5A1004085A100408
5A10 0408 |
4210 03ED | 416,421003ED5A100408
5A10 0408 | 
4210 03F3 | 41E,421003F35B100408
5B10 0408 |
5A10 03F8 | 426,5A1003F8421003F8
4210 03F0 |
5A10 0404 | 42E,5A100404421003EE
4210 03EE |
5B10 0400 | 436,5B100400421003E8
4210 03E8 |
5B10 0408 | 43E,5B1004085B100408
5B10 0408 |
5B10 0408 | 446,5B100408421003E9
4210 03E9 |
5A10 0400 | 44E,5A100400421003EC
4210 03EC |
4210 03EF | 456,421003EF5B100408
5B10 0408 |
4210 03F2 | 45E,421003F25A100408
5A10 0408 |
5A10 0404 | 466,5A1004045A100408
5A10 0408 |
5A10 0408 | 46E,5A100408421003EA
4210 03EA |
4210 03EB | 476,421003EB421003F1
4210 03F1 |
9C10 0000 | 47E,9C10000000000000
*/