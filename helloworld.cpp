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
    std::ofstream logfile("logfile.txt");
    //mem->coredump(0x400);
    /* Channel Code */
    mem->writeWord(72,0x00000068,0);
    mem->writeDoubleword(104,0x0D0003E80000000D,0);
    std::cout << "CHANNEL CODE LOADED" << std::endl;
    /* Program Code */
    mem->writeWord(0x00000418, 0x5A1003FC, 0);
    mem->writeWord(0x0000041C, 0x5A100404, 0);
    mem->writeWord(0x00000420, 0x5A100408, 0);
    mem->writeWord(0x00000424, 0x5A100408, 0);

    mem->writeWord(0x00000428, 0x421003ED, 0);
    mem->writeWord(0x0000042C, 0x5A100408, 0);
    mem->writeWord(0x00000430, 0x421003F3, 0);
    mem->writeWord(0x00000434, 0x5B100408, 0);

    mem->writeWord(0x00000438, 0x5A1003F8, 0);
    mem->writeWord(0x0000043C, 0x421003F0, 0);
    mem->writeWord(0x00000440, 0x5A100404, 0);
    mem->writeWord(0x00000444, 0x421003EE, 0);

    mem->writeWord(0x00000448, 0x5B100404, 0);
    mem->writeWord(0x0000044C, 0x5B100400, 0);
    mem->writeWord(0x00000450, 0x421003E8, 0);
    mem->writeWord(0x00000454, 0x5B100408, 0);

    mem->writeWord(0x00000458, 0x5B100408, 0);
    mem->writeWord(0x0000045C, 0x5B100408, 0);
    mem->writeWord(0x00000460, 0x421003E9, 0);
    mem->writeWord(0x00000464, 0x5A100400, 0);

    mem->writeWord(0x00000468, 0x421003EC, 0);
    mem->writeWord(0x0000046C, 0x421003EF, 0);
    mem->writeWord(0x00000470, 0x5B100408, 0);
    mem->writeWord(0x00000474, 0x421003F2, 0);

    mem->writeWord(0x00000478, 0x5A100408, 0);
    mem->writeWord(0x0000047C, 0x5A100404, 0);
    mem->writeWord(0x00000480, 0x5A100408, 0);
    mem->writeWord(0x00000484, 0x5A100408, 0);

    mem->writeWord(0x00000488, 0x421003EA, 0);
    mem->writeWord(0x0000048C, 0x421003EB, 0);
    mem->writeWord(0x00000490, 0x421003F1, 0);
    mem->writeWord(0x00000494, 0x9C100000, 0);
    std::cout << "PROGRAM CODE LOADED" << std::endl;
    /* Memory Initialization */
    mem->writeWord(0x000003F8, 0x00000032, 0);
    mem->writeWord(0x000003FC, 0x00000019, 0);
    mem->writeWord(0x00000400, 0x0000000A, 0);
    mem->writeWord(0x00000404, 0x00000005, 0);
    mem->writeWord(0x00000408, 0x00000001, 0);
    //mem->coredump(0x408);
    std::cout << "MEMORY INITIALIZATION COMPLETE" << std::endl;
    //std::cout << mem->getWord(72,0) << std::endl;
    cpu tcpu(mem,stdisa,logfile);
    tcpu.setAddr(0x00000418);
    tcpu.setVerbose(true);
    channel tchannel(mem,0);
    stdioDevice tdev;
    tchannel.addSubchannel(0);
    tchannel.addDevice({0,0},&tdev);
    tcpu.registerChannel(0,tchannel);
    std::cout << "BOOTING VIRTUAL SYSTEM" << std::endl;
    //std::cout << mem->getWord(72,0) << std::endl;
    //mem->writeByte(1001,74,0);
    //mem->writeByte(74,1001,0);
    tcpu.test(32);
    //logfile.close();
    //mem->coredump(0x404);
    return 0;
}

/*

HELLO WORLD PROGRAM

MEMORY

--CHANNEL PROGRAM--
CAW: 104 (DEC)

DEC: HEX
104-111: 0D0003E80000000D


NOTE: BELOW IS THE STATE OF MEMORY AFTER THE PROGRAM COMPLETES. 1000-1011 ARE BLANK WHEN INITIALIZED
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

SUB 1028 (5) FROM  R1 (87)
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

S       R1      R0 404
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

MADDR,INSTRUCTION
----------------
0x418,0x5A1003FC 
0x41C,0x5A100404
0x420,0x5A100408 
0x424,0x5A100408

0x428,0x421003ED 
0x42C,0x5A100408
0x430,0x421003F3 
0x434,0x5B100408

0x438,0x5A1003F8 
0x43C,0x421003F0
0x440,0x5A100404 
0x444,0x421003EE

0x448,0x5B100404 
0x44C,0x5B100400 
0x450,0x421003E8 
0x454,0x5B100408 

0x458,0x5B100408
0x45C,0x5B100408
0x460,0x421003E9
0x464,0x5A100400

0x468,0x421003EC
0x46C,0x421003EF
0x470,0x5B100408
0x474,0x421003F2

0x478,0x5A100408
0x47C,0x5A100404
0x480,0x5A100408
0x484,0x5A100408

0x488,0x421003EA
0x48C,0x421003EB
0x490,0x421003F1
0x494,0x9C100000

*/