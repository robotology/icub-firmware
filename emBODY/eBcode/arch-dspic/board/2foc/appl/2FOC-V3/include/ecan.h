#ifndef __ecan_H
#define __ecan_H

#include "UserTypes.h" 
#include "status.h" 
#include "faults.h"

// max message parsed in each CAN interrupt
#define MAX_RX_INT                  5

// CAN Baud Rate Configuration
#define FCAN        40000000 
#define BITRATE      1000000  
#define NTQ               20  // 20 Time Quanta in a Bit Time
#define BRP_VAL ((FCAN/(2*NTQ*BITRATE))-1)

/* defines used in ecan.c */
// ECAN message buffer length
#define ECAN1_MSG_BUF_LENGTH 8

/* ECAN message type identifiers */
#define CAN_MSG_DATA	0x01
#define CAN_MSG_RTR		0x02
#define CAN_FRAME_EXT	0x03
#define CAN_FRAME_STD	0x04
#define CAN_BUF_FULL	0x05
#define CAN_BUF_EMPTY	0x06



#define ECAN_ID_29 0x80000000
// Incoiming commands are formatted as follows
// For 11 bit Id
// ... to be completed



// filter and mask defines 
// Macro used to write filter/mask ID to Register CiRXMxSID and 
// CiRXFxSID. For example to setup the filter to accept a value of 
// 0x123, the macro when called as CAN_FILTERMASK2REG_SID(0x123) will 
// write the register space to accept message with ID ox123 
// USE FOR STANDARD MESSAGES ONLY 
#define CAN_FILTERMASK2REG_SID(x) ((x & 0x07FF)<< 5)
/* the Macro will set the "MIDE" bit in CiRXMxSID */
#define CAN_SETMIDE(sid) (sid | 0x0008)
/* the macro will set the EXIDE bit in the CiRXFxSID to 
accept extended messages only */
#define CAN_FILTERXTD(sid) (sid | 0x0008)
/* the macro will clear the EXIDE bit in the CiRXFxSID to 
accept standard messages only */
#define CAN_FILTERSTD(sid) (sid & 0xFFF7)
/* Macro used to write filter/mask ID to Register CiRXMxSID, CiRXMxEID and 
CiRXFxSID, CiRXFxEID. For example to setup the filter to accept a value of 
0x123, the macro when called as CAN_FILTERMASK2REG_SID(0x123) will 
write the register space to accept message with ID 0x123 
USE FOR EXTENDED MESSAGES ONLY */
#define CAN_FILTERMASK2REG_EID0(x) (x & 0xFFFF)
#define CAN_FILTERMASK2REG_EID1(x) (((x & 0x1FFC)<< 3)|(x & 0x3))



typedef union
{
    unsigned long dw[2];
    unsigned short w[4];
    unsigned char b[8];
}__attribute__((__packed__)) tCanData;

/*typedef struct
{
 unsigned CAN_RTRFlagActive:1;
 unsigned CAN_BufferOverRun:1;
 unsigned CAN_BusOff:1;
 unsigned CAN_TXPasv:1;
 unsigned CAN_RXPasv:1;
 unsigned CAN_Warn:1;
 unsigned CAN_DLCError:1;	
} tCanErrors;
*/
extern volatile int CAN_BusOff;

// function prototypes as defined in can.c
extern void ECANInit (void (*RxCb)(unsigned long id, unsigned char len, tCanData *payload));
extern int ECANSend(unsigned long id, unsigned char len,tCanData *payload);
extern int ECANRxFilterAdd(unsigned long id, unsigned long mask);
extern void ECANStart();
extern void ECANCheckState();

extern void hal_can_receptionfilter_set(unsigned char mask_num, 
                                        unsigned long mask_val, 
                                        unsigned char identifier_num,
                                        unsigned long identifier_val, 
                                        unsigned char extid);
#endif
