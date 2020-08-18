 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  CAN configuration                                *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#ifndef __CAN_H
#define __CAN_H

#define STANDARD_FORMAT  0
#define EXTENDED_FORMAT  1

#define DATA_FRAME       0
#define REMOTE_FRAME     1

typedef struct  {
  unsigned int   id;                 // 29 bit identifier
  unsigned char  data[8];            // Data field
  unsigned char  len;                // Length of data field in bytes
  unsigned char  format;             // 0 - STANDARD, 1- EXTENDED IDENTIFIER
  unsigned char  type;               // 0 - DATA FRAME, 1 - REMOTE FRAME
} CAN_msg;

// Functions defined in module can.c
void CAN_setup         (void);
void CAN_init          (void);
void CAN_start         (void);
void CAN_waitReady     (void);
void CAN_wrMsg         (CAN_msg *msg);
void CAN_rdMsg         (CAN_msg *msg);
void CAN_wrFilter      (unsigned int id, unsigned char filter_type);

void CAN_testmode      (unsigned int testmode);

extern CAN_msg       CAN_TxMsg;      // CAN messge for sending
extern CAN_msg       CAN_RxMsg;      // CAN message for receiving                                
extern unsigned int  CAN_TxRdy;      // CAN HW ready to transmit a message
extern unsigned int  CAN_RxRdy;      // CAN HW received a message

#endif // __CAN_H


