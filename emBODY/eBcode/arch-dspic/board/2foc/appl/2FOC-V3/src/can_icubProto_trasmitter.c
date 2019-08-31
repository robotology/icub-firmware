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
#include "UserParms.h"
#include "can_icubProto_trasmitter.h"
#include "system.h"
#include "faults.h"
#include "2FOC.h"
#include "qep.h"
#include "park.h"

//CAN_PERIOD_PHASE is used to be sure that all foc don't send status msg in the same time
//#define CAN_PERIOD_PHASE (canprototransmitter_bid)*2

static unsigned char canprototransmitter_bid;
//this variable says to know if ems is going on to send msg to 2foc.
//if ems stops to send msg to 2foc then status message is send by _T4Interrupt and not from CanIcubProtoTrasmitterSendPeriodicData
//static unsigned char received_msg_from_ems = 0;
//volatile unsigned char received_canloader_msg= 0;
//extern volatile int gulp_update_request ;
extern tGulp Gulp;
//extern tGulp GulpHistoryBuffer[GULP_HISTORY_BUFFER_SIZE];
//extern int GulpHistoryBufferIdx;
//extern int test_received_data;

//static void s_CanIcubProtoTrasmitter_praparePeriodicMsg(tCanData *candata_ptr, unsigned char *len_ptr, unsigned long *txid_ptr);

void CanIcubProtoTransmitterInit(unsigned char bid)
{
	canprototransmitter_bid = bid;
}

extern volatile long VqRef;
extern volatile int  IqRef;

extern volatile BOOL newencdata;

extern volatile int dataA;
extern volatile int dataB;
extern volatile int dataC;
extern volatile int dataD;

extern volatile tMotorConfig MotorConfig;

//volatile unsigned char HALL = 0xFF;

extern volatile int angle_feedback;
extern volatile int sectr_feedback;
extern volatile short Ia,Ib,Ic;
extern volatile short Va,Vb,Vc;

extern volatile int storeIa, storeIc, storeVd;

extern void CanIcubProtoTrasmitterSendPeriodicData(void)
{
    static tCanData payload; // = {{0}};
    unsigned long msgid;

    payload.w[0] = I2Tdata.IQMeasured;
    payload.w[1] = gQEVelocity;
    payload.w[2] = gQEPosition & 0xFFFF;
    payload.w[3] = gQEPosition >> 16;

    //payload.w[0] = I2Tdata.IQMeasured;
    //payload.w[1] = Ia;
    //payload.w[2] = Ib;
    //payload.w[3] = Ic;
    
    msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__2FOC);

    ECANSend(msgid, 8, &payload);

    //prepare the payload
    payload.b[0] = gControlMode;
    payload.b[1] = gEncoderError.bitmask;
    
    //payload.w[1] = POSCNT;

    payload.w[1] = (VqRef>>5);

    //payload.w[1] = HALL;
    
    //payload.b[2] = 0;
    //payload.b[3] = 0;

    payload.b[4] = SysError.b[0];
    payload.b[5] = SysError.b[1];
    payload.b[6] = SysError.b[2];
    payload.b[7] = SysError.b[3];

    msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__STATUS);

    ECANSend(msgid, 8, &payload);

    if (MotorConfig.verbose)
    {
        if (newencdata)
        {
            payload.w[0] = dataA;
            payload.w[1] = dataB;
            payload.w[2] = dataC;
            payload.w[3] = dataD;
        
            newencdata = FALSE;
        
            msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__DEBUG );

            ECANSend(msgid, 8, &payload);
        }
    }
}

extern void CanIcubProtoTransmitterUpdateBoardId(unsigned char bid)
{
    canprototransmitter_bid = bid;
}

