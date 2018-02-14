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

#include <p33FJ128MC802.h>
#include "can_icubProto.h"
#include "can_icubProto_parser.h"
#include "can_icubProto_trasmitter.h"
#include "ecan.h"

#ifndef NULL
#define NULL 0
#endif
//invocata solo nel parser in caso di errore
void CanIcubProtoTxErrorCode(unsigned int stid, unsigned char len, tCanData *errordata)
{
	ECANSend(stid, len, errordata); 
}

void CanIcubProtoTxAck(unsigned int stid)
{
	ECANSend(stid, 0, NULL); 
}

void CanIcubProtoSendResponse(unsigned int stid, unsigned char len, tCanData *respdata)
{
	ECANSend(stid, len, respdata); 
}

// Initializes CAN for communication
void CanIcubProtoInit(unsigned char bid)
{
    if((bid == 0) || (bid >0xE))
    {
        bid = 0xA;
    }
  // init gulp var address with a good address (default GULP var)
  //gulpadr1 = (unsigned int*)PeriodicData[PeriodicMessageContents[0]];
  //gulpadr2 = (unsigned int*)PeriodicData[PeriodicMessageContents[1]];
  //gulpadr3 = (unsigned int*)PeriodicData[PeriodicMessageContents[2]];
  //gulpadr4 = (unsigned int*)PeriodicData[PeriodicMessageContents[3]];

  // Initialize the SW transmitter module
  CanIcubProtoTransmitterInit(bid);
  // Initialize the SW parser module
  CanIcubProtoParserInit(bid);
  // Initialize CAN driver and link to parser RX handler
  ECANInit(CanIcubProtoParserParse);
  // Configure CAN RX filters
  CanIcubProtoSetFilters(bid);
  // Start CAN operation
  ECANStart(); 

//initECAN();
}

void CanIcubProtoSetFilters(unsigned char bid)
{
//    hal_can_receptionfilter_set( 0, 0xfff, 0, CAN_ICUBPROTO_STDID_MAKE_RX(ICUBPROTO_CLASS_PERIODIC_MOTORBOARD, ICUBPROTO_PERIODIC_EMSTO2FOC_SETPOINT_CURRENT), 0/*standard_id*/);
//    hal_can_receptionfilter_set( 0, 0xfff, 0, CAN_ICUBPROTO_STDID_MAKE_RX(ICUBPROTO_CLASS_PERIODIC_MOTORBOARD, ICUBPROTO_PERIODIC_EMSTO2FOC_SETPOINT_VELOCITY), 0/*standard_id*/);
    hal_can_receptionfilter_set( 0, 0xfff, 0, CAN_ICUBPROTO_STDID_MAKE_RX(ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL, bid), 0/*standard_id*/);
    hal_can_receptionfilter_set( 0, 0xfff, 1, CAN_ICUBPROTO_STDID_MAKE_RX(ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL, 0xF/*broadcast*/), 0/*standard_id*/);
    hal_can_receptionfilter_set( 0, 0xfff, 2, CAN_ICUBPROTO_STDID_MAKE_RX(ICUBCANPROTO_CLASS_BOOTLOADER, bid), 0/*standard_id*/);
    hal_can_receptionfilter_set( 0, 0xfff, 3, CAN_ICUBPROTO_STDID_MAKE_RX(ICUBCANPROTO_CLASS_BOOTLOADER, 0xF/*broadcast*/), 0/*standard_id*/);
#ifdef DESIRED_CURR_IN_PER_MSG_BY_EMS
    hal_can_receptionfilter_set( 0, 0xfff, 4, CAN_ICUBPROTO_STDID_MAKE_RX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT), 0/*standard_id*/);
#endif
}

/*
extern icubCanProto_controlmode_t CanIcubProtoGetcontrol_mode(void)
{
    return gControlMode;
}
*/