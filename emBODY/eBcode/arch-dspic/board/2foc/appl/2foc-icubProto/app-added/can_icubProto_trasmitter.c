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
#include <string.h>

#include "can_icubProto.h"
#include "UserParms.h"
#include "can_icubProto_trasmitter.h"
#include "system.h"
#include "faults.h"

//CAN_PERIOD_PHASE is used to be sure that all foc don't send status msg in the same time
#define CAN_PERIOD_PHASE (canprototransmitter_bid)*2

static unsigned char canprototransmitter_bid;
//this variable says to know if ems is going on to send msg to 2foc.
//if ems stops to send msg to 2foc then status message is send by _T4Interrupt and not from CanIcubProtoTrasmitterSendPeriodicData
static unsigned char received_msg_from_ems = 0;
volatile unsigned char received_canloader_msg= 0;
extern volatile int gulp_update_request ;
extern tGulp Gulp;
extern tGulp GulpHistoryBuffer[GULP_HISTORY_BUFFER_SIZE];
extern int GulpHistoryBufferIdx;
extern int test_received_data;


static void s_CanIcubProtoTrasmitter_praparePeriodicMsg(tCanData *candata_ptr, unsigned char *len_ptr, unsigned long *txid_ptr);

void CanIcubProtoTransmitterInit(unsigned char bid)
{
	canprototransmitter_bid = bid;
}

//void __attribute__((__interrupt__, no_auto_psv)) _T4Interrupt(void)
//// The periodic CAN transmission buffer is filled and the trasmission is started
//{
//  static unsigned int state_cnt = 0;
//  unsigned long txid;
//  unsigned int len;
//  static tCanData candata = {{0}};
//  // CAN TX buffer for period gulp messages (not status)
//  // This needs to be static because it might be not initialized 
//  // in this function when gulp vars are not updated
//  
//  // clear INT flag
//  IFS1bits.T4IF = 0; 
//
//
////  if( 0 == gulp_update_request)
////  {  
////    candata.w[0] = Gulp.W[0];
////    candata.w[1] = Gulp.W[1];
////	candata.w[2] = Gulp.W[2];
////    candata.w[3] = Gulp.W[3];
////    
////    // we got values and we buffered them. Now we can request 
////    // to the FOC loop to provide new values again
////    gulp_update_request = 1;
////  } 
////
////    txid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__2FOC);
////	
////	ECANSend(txid, 8, &candata);
//
//  // every CAN_STATUS_MSG_PERIOD_SYNC_WITH_EMS send status message
//  if(state_cnt++ == CAN_STATUS_MSG_PERIOD+CAN_PERIOD_PHASE)
//  {
//    // reset counter for next status message
//    state_cnt = 0;
//
//    if(1 == received_msg_from_ems)
//    {
//        //ems is going on to send to 2foc periodic msg, so here don't send status message
//        received_msg_from_ems = 0;
//        return;
//    }
//
//    memset(candata.b, 0, 8);//reset the payload
//   
////  --- prapare the payload ---
//    candata.b[0] = SysError.b[0];
//    //adjust some values;
//    candata.b[0] |= (( SysError.I2TFailure << 3) | (SysError.AS5045CalcError << 5)); 
//  
//    candata.b[1] = CanIcubProtoGetcontrol_mode(); //in case of error??
//    candata.b[4] = SysError.b[1];
//    candata.b[6] = SysStatus.b[3];
//
//    len = 7;
//
//    txid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBPROTO_PERIODICCMD_STATUS);
//	
//	ECANSend(txid, len, &candata);
//  }
//}


void __attribute__((__interrupt__, no_auto_psv)) _T4Interrupt(void)
// The periodic CAN transmission buffer is filled and the trasmission is started
{
  static unsigned int state_cnt = 0;

  // CAN TX buffer for period gulp messages (not status)
  // This needs to be static because it might be not initialized 
  // in this function when gulp vars are not updated
  
  // clear INT flag
  IFS1bits.T4IF = 0; 
#ifndef SYNC_2FOC_TO_EMS 
//here i can not use s_CanIcubProtoTrasmitter_praparePeriodicMsg because thi interrupt ca happen while main loop
//is updateing gulp values.
  if( 0 == gulp_update_request)
  {  
    candata.w[0] = Gulp.W[0];
    candata.w[1] = Gulp.W[1];
	candata.w[2] = Gulp.W[2];
    candata.w[3] = Gulp.W[3];
    
    // we got values and we buffered them. Now we can request 
    // to the FOC loop to provide new values again
    gulp_update_request = 1;
  } 

    txid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__2FOC);
	
	ECANSend(txid, 8, &candata);
#endif
  // every CAN_STATUS_MSG_PERIOD_SYNC_WITH_EMS send status message
  if(state_cnt++ == CAN_STATUS_MSG_PERIOD+CAN_PERIOD_PHASE)
  {
    // reset counter for next status message
    state_cnt = 0;

    if(1 == received_msg_from_ems)
    {
        //ems is going on to send to 2foc periodic msg, so here don't send status message
        received_msg_from_ems = 0;
        return;
    }

	if (received_canloader_msg==1) 
	{
		return;
	}

    CanIcubProtoTrasmitter_SendStatusMsg();

  }
}

//extern void CanIcubProtoTrasmitterSendPeriodicData(void)
//{
//    unsigned char txlen = 0;
//    tCanData txpayload;
//    unsigned long txid;
//    static tCanData candata = {{0}};
//    static unsigned int state_cnt = 0;
//
//    //set i received periodic can msg from ems
//    received_msg_from_ems = 1;  
//
//    txlen = 8;
//
//    gulp_update_request = 0;
//    
//    memcpy(&txpayload.b[0], &Gulp.B[0], 8);
//        
//    // we got values and we buffered them. Now we can request 
//    // to the FOC loop to provide new values again
//    gulp_update_request = 1;
//
//    if(txlen>0)
//    {
//        txid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__2FOC);
//        ECANSend(txid, 8, &txpayload);
//    }
//
//    // every CAN_STATE_PER gulp messages send out a status message 
//    if(state_cnt++ == CAN_STATUS_MSG_PERIOD_SYNC_WITH_EMS+CAN_PERIOD_PHASE)
//    {
//        // reset counter for next status message
//        state_cnt = 0;
//        memset(candata.b, 0, 8);//reset the payload
//       
//    //  --- prapare the payload ---
//        candata.b[0] = SysError.b[0];
//        //adjust some values;
//        candata.b[0] |= (( SysError.I2TFailure << 3) | (SysError.AS5045CalcError << 5)); 
//      
//        candata.b[1] = CanIcubProtoGetcontrol_mode(); //in case of error??
//        candata.b[4] = SysError.b[1];
//        candata.b[6] = SysStatus.b[3];
//
//    
//        txid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBPROTO_PERIODICCMD_STATUS);
//    	
//    	ECANSend(txid, 7, &candata);
//    }
//}


extern void CanIcubProtoTrasmitterSendPeriodicData(void)
{
    unsigned char txlen = 0;
    unsigned long txid;
    tCanData perMsg_payload; // = {{0}};
  
    
    static unsigned int state_cnt = 0;

    //set i received periodic can msg from ems
    received_msg_from_ems = 1;  

    gulp_update_request = 0;
    
    s_CanIcubProtoTrasmitter_praparePeriodicMsg(&perMsg_payload, &txlen, &txid);
        
    // we got values and we buffered them. Now we can request 
    // to the FOC loop to provide new values again
    gulp_update_request = 1;

    ECANSend(txid, txlen, &perMsg_payload);


    // every CAN_STATE_PER gulp messages send out a status message 
    if(state_cnt++ == CAN_STATUS_MSG_PERIOD_SYNC_WITH_EMS+CAN_PERIOD_PHASE)
    {
        // reset counter for next status message
        state_cnt = 0;
        
		CanIcubProtoTrasmitter_SendStatusMsg();

    }
}

extern void CanIcubProtoTransmitterUpdateBoardId(unsigned char bid)
{
    canprototransmitter_bid = bid;
}
static void s_CanIcubProtoTrasmitter_praparePeriodicMsg(tCanData *candata_ptr, unsigned char *len_ptr, unsigned long *txid_ptr)
{
    memcpy(&candata_ptr->b[0], &Gulp.B[0], 8);
    *len_ptr = 8;
    *txid_ptr = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__2FOC);
}

extern void CanIcubProtoTrasmitter_SendStatusMsg()
{
	tCanData candata;
	unsigned char len;
	unsigned long txid;
    //reset the payload
     memset(&candata.b[0], 0, 8);
       
    //prapare the payload
    candata.b[0] = SysError.b[0];    
    candata.b[0] |= (( SysError.I2TFailure << 3) | (SysError.AS5045CalcError << 5)); //adjust some values;
    candata.b[1] = CanIcubProtoGetcontrol_mode(); //in case of error??
    candata.b[4] = SysError.b[1];
    candata.b[5] = (SysError.I2TFailure << 5);
    candata.b[6] = SysStatus.b[3];
    
    txid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__STATUS);
    len = 7;
	
	ECANSend(txid, len, &candata);

}

