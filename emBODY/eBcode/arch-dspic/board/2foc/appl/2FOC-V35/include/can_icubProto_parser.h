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

#ifndef __ICUB_ICUBPROTO_PARSER_H
#define __ICUB_ICUBPROTO_PARSER_H

#include "ecan.h"
#include "DSP.h"
//#include "DS402.h"

//NOTE: removed because they dont't belong to icub can proto, but are use only for debug porpouse.
//      if they are used with bootloader, flashing process can not terminate with success.
////
////  CAN error codes definition
////
//// the command has a wrong number of parameters
//#define CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS 0x01
//// the message is unknown by the parser
//#define CAN_ERROR_UNKNOWN_COMMAND                0x02
//// Trying to read write only data
//#define CAN_ERROR_WRITE_ONLY_DATA                0x03
//// Trying to write read only data
//#define CAN_ERROR_READ_ONLY_DATA                 0x04
//// DS402 invalid status change request
//#define CAN_DS402_INVALID_STATUS_CHANGE          0x05
//// DS402 invalid status change request
//#define CAN_ERROR_PARAMETER_OUT_OF_RANGE         0x06
//// the message is invalid in current state
//#define CAN_ERROR_COMMAND_IN_INVALID_STATE       0x07
//// the board is in inconsistent state
//#define CAN_ERROR_INCONSISTENT_STATE             0x08


//extern tSysStatus SysStatus;
//extern unsigned char PeriodicMessageContents[4];
//extern tI2T I2Tdata;
//extern unsigned int *gulpadr1;
//extern unsigned int *gulpadr2;
//extern unsigned int *gulpadr3;
//extern unsigned int *gulpadr4;

extern void CanIcubProtoParserParse(unsigned long rxid, unsigned char rxlen, tCanData *rxpayload);
extern void CanIcubProtoParserInit(unsigned char bid);

#endif
