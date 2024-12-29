#ifndef IOHELPERS_H
#define IOHELPERS_H

#include "helpers.h"
/*-----------------------------------------------------------------*/
/* DEVICE STATUS BITMASKS                                          */
/*-----------------------------------------------------------------*/

#define ATTNTN 0b10000000    //Attention
#define STAMOD 0b01000000    //Status Modification    
#define CTREND 0b00100000    //Control Unit End
#define CTRBSY 0b00010000    //Control Unit Busy
#define CHAEND 0b00001000    //Channel End
#define DEVEND 0b00000100    //Device End
#define UNTCHK 0b00000010    //Unit Check
#define UNTEXC 0b00000001    //Unit Exception

/*-----------------------------------------------------------------*/
/* CHANNEL STATUS BITMASKS                                         */
/*-----------------------------------------------------------------*/

#define PRGCI 0b10000000   //Program Controlled Interruption
#define INCLN 0b01000000   //Incorrect Length
#define PRGRM 0b00100000   //Program Check
#define PRTCT 0b00010000   //Protection Check
#define CHNDT 0b00001000   //Channel Data Check
#define CHNCN 0b00000100   //Channel Control Check
#define INTCN 0b00000010   //Interface Control Check
#define CHAIN 0b00000001   //Chaining check

/*-----------------------------------------------------------------*/
/* CCW FLAG BITMASKS                                               */
/*-----------------------------------------------------------------*/

#define CHDT_FLAG 0b10000     //Chain Data
#define CHCM_FLAG 0b01000     //Chain Command
#define SPLN_FLAG 0b00100     //Suppress Length Interruption
#define SKIP_FLAG 0b00010     //Skip
#define PGCI_FLAG 0b00001     //Program-Controlled 

/*-----------------------------------------------------------------*/
/* IO INTERRUPT                                                    */
/*-----------------------------------------------------------------*/

struct IOInterrupt {
    bool finished; //Whether or not the IO process has terminated
    halfword address; //The address of the channel and device
};

/*-----------------------------------------------------------------*/
/* CHANNEL STATUS                                                  */
/*-----------------------------------------------------------------*/

struct channelstatus {
    unsigned int key : 4; //Storage Protection Key
    unsigned int pc : 24; //Program counter
    unsigned int usc : 8; //Unit status conditions
    unsigned int csc : 8; //Channel status conditions
    unsigned int count : 16; //Residual count of last CCW used
};

/*-----------------------------------------------------------------*/
/* DEVICE ADDRESS                                                  */
/*-----------------------------------------------------------------*/

struct deviceAddress {
    unsigned int subchannelID : 4;
    unsigned int deviceID : 4;
};

#endif