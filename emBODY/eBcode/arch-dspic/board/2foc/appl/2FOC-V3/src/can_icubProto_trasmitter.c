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
#include "System.h"
#include "Faults.h"
#include "2FOC.h"
#include "qep.h"
#include "i2cTsens.h"

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

extern volatile int rotorAfbk;
extern volatile int rotorBfbk;
extern volatile char IKs;

extern volatile tMotorConfig MotorConfig;

extern volatile int Iafbk;
extern volatile int Icfbk;
extern volatile int ElDegfbk;
extern volatile int iQprot;
extern volatile int maxCountfbk;
extern volatile int Vqfbk;

extern void CanIcubProtoTrasmitterSendPeriodicData(void)
{
    static tCanData payload; // = {{0}};
    unsigned long msgid;

    payload.w[0]  = I2Tdata.IQMeasured;
    payload.w[1]  = gQEVelocity;
    payload.dw[1] = gQEPosition;
    
    msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__2FOC);

    ECANSend(msgid, 8, &payload);
    
    BOOL bequiet = FALSE;

    if (MotorConfig.fullcalib)
    {
        static int noflood = 0;
    
        int Tsend = 500 + canprototransmitter_bid*100;
        
        if (noflood <= 10 || noflood >= Tsend-10) bequiet = TRUE;
        
        if (++noflood >= Tsend)
        {
            noflood = 0;
            
            payload.w[0] = gEncoderConfig.offset;
            payload.w[1] = POSCNT;
            payload.w[2] = rotorAfbk;
            payload.w[3] = rotorBfbk;
        
            msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__DEBUG );

            ECANSend(msgid, 8, &payload);
        }
    }
    else if (MotorConfig.verbose)
    {        
        static int noflood = 0;
        
        extern volatile char I2Cdead;
        extern volatile uint16_t I2Cerrors;
        extern volatile int I2Cerrcode;
        
        int Tsend = 500 + canprototransmitter_bid*100;
        
        if (noflood <= 10 || noflood >= Tsend-10) bequiet = TRUE;
        
        if (++noflood >= Tsend)
        {
            noflood = 0;
            
            payload.w[1] = I2Cerrcode;
            payload.w[2] = I2Cdead;
            payload.w[3] = I2Cerrors;
        
            msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__DEBUG );

            ECANSend(msgid, 8, &payload);
        }
    }
    
    if (!bequiet)
    {
        /**
        BOOL transmit_addStatus = FALSE;
        if (MotorConfig.has_tsens && isActiveI2CTsens())
        {
            
            int Tsend = 100 + canprototransmitter_bid*3;
            static int noflood = 0;

            if (isTemperatureRead)
            {
                noflood = 0;
                transmit_addStatus = TRUE;
            }
        }
         * */
        if(isTemperatureRead)
        {
            payload.w[0] = 0;
            payload.w[1] = gTemperature;

            msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__ADDITIONAL_STATUS );
            ECANSend(msgid, 4, &payload);
            isTemperatureRead = FALSE;
        }
        else
        {
            payload.b[0] = gControlMode;
            payload.b[1] = gEncoderError.bitmask;

            payload.w[1] = (VqRef>>(IKs-VOLT_REF_SHIFT));

            payload.b[4] = SysError.b[0];
            payload.b[5] = SysError.b[1];
            payload.b[6] = SysError.b[2];
            payload.b[7] = SysError.b[3];

            msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__STATUS);

            ECANSend(msgid, 8, &payload);
        }

    }
}

extern void CanIcubProtoTransmitterUpdateBoardId(unsigned char bid)
{
    canprototransmitter_bid = bid;
}
