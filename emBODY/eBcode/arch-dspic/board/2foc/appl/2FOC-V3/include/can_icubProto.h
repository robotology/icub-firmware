/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/
#ifndef __CAN_ICUBPROTO_PROTO_H_
#define __CAN_ICUBPROTO_PROTO_H_

#define MIN(a,b) ((a>b)?b:a)

#include "ecan.h"
#include "stdint.h"
#include "iCubCanProto_types.h"
#include "iCubCanProtocol.h"


/************ FIRMWARE AND CAN PROTOCOL VERSION DEFINITION *******************************************/
#define FW_VERSION_MAJOR          3
#define FW_VERSION_MINOR          3
#define FW_VERSION_BUILD          1

#define CAN_PROTOCOL_VERSION_MAJOR      1
#define CAN_PROTOCOL_VERSION_MINOR      6
/*****************************************************************************************************/




#define CAN_PROTO_TX_CMDVALUE_UNUSED 0x00FFFF00



// outgoing rensponse messages Id. 
#define CAN_PROTO_CMD_RESPONSE			      0xfe
// outgoing error message (for example unknown command or wrong params)
#define CAN_PROTO_CMD_ERROR					  0xff

//NOTE: removed because they dont't belong to icub can proto, but are use only for debug porpouse.
//      if they are used with bootloader, flashing process can not terminate with success.
////
////  CAN error codes definition
////
//
//// the command has a wrong number of parameters
//#define CAN_PROTO_ERROR_INCORRECT_NUMBER_OF_PARAMETERS 0x01
//// the message is unknown by the parser
//#define CAN_PROTO_ERROR_UNKNOWN_COMMAND                0x02
//// trying to write a read only parameter
//#define CAN_PROTO_ERROR_WRITE_READ_ONLY_DATA           0x03
//// command parameter out of range
//#define CAN_PROTO_ERROR_PARAM_OUT_OF_RANGE             0x04
//// command not yet implemented
//#define CAN_PROTO_ERROR_COMMAND_NOT_INPLEMENTED        0x05
//// trying to read a write only parameter
//#define CAN_PROTO_ERROR_READ_WRITE_ONLY_DATA           0x06



// utility macros for CAN ID management
#define EMS_CANADDR 0x0 
#define CAN_ICUBPROTO_STDID_MAKE_RX(class, bid) (((class & 0x7)<<8) | (EMS_CANADDR << 4) | bid)
/*Note: in case of periodic message: x(third param) is the command
        in case of response to polling message:  x(third param) is the dest*/
#define CAN_ICUBPROTO_STDID_MAKE_TX(class, bid, x) (((class & 0x7)<<8) | (bid << 4) | x)
#define CAN_ICUBPROTO_STDID_GETCLASS(bid) ((bid & 0x700) >> 8)
#define CAN_ICUBPROTO_ID_ISEXTENDED(rxid) (!!(rxid & ECAN_ID_29))
#define CAN_ICUBPROTO_STDID_RX_GETDEST(rxid) (rxid & 0x000f)
#define CAN_ICUBPROTO_STDID_RX_GETSOURCE(rxid) ((rxid & 0x00f0)>>4)
//get periodic message type. it is equal to get dest for polling msg
#define CAN_ICUBPROTO_STDID_RX_GETPERMSGTYPE(rxid)  CAN_ICUBPROTO_STDID_RX_GETDEST(rxid)

extern unsigned int *gulpadr1;
extern unsigned int *gulpadr2;
extern unsigned int *gulpadr3;
extern unsigned int *gulpadr4;

extern void CanIcubProtoSendResponse(unsigned int stid, unsigned char len, tCanData *errordata);
extern void CanIcubProtoTxAck(unsigned int stid);
extern void CanIcubProtoTxErrorCode(unsigned int stid, unsigned char len, tCanData *errordata);
extern void CanIcubProtoInit(unsigned char bid);
extern void CanIcubProtoSetFilters(unsigned char bid);
extern icubCanProto_controlmode_t CanIcubProtoGetcontrol_mode(void);
//extern const unsigned int * PeriodicData[ELEMENTS_IN_PERIODIC_DATA_LIST];
#endif
