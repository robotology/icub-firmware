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

extern volatile tMotorConfig MotorConfig;

extern volatile int ElDegfbk;
extern volatile int iQprot;
extern volatile int maxCountfbk;

extern volatile int VqFbk;
extern volatile int IqFbk;

extern void CanIcubProtoTrasmitterSendPeriodicData(void)
{
    static tCanData payload; // = {{0}};
    unsigned long msgid;

    payload.w[0]  = IqFbk;
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
/*      Valegagge: 15 July 2024:
 *      use the debug message for qe_encoder info instead of for I2C debug info.
 *      I cannot downsampling the qe info.
 * 
 */ 
        static int noflood = 0;      
        //extern volatile char I2Cdead;
        //extern volatile uint16_t I2Cerrors;
        //extern volatile int I2Cerrcode;
        
        extern volatile int  IKp;
        extern volatile int  IKi;
        extern volatile int  IKff;
        extern volatile int  IKbemf;
        extern volatile char IKs;
        
        extern volatile int  SKp;
        extern volatile int  SKi;
        extern volatile int  SKff;
        extern volatile char SKs;
        
        int Tsend = 500 + canprototransmitter_bid*100;
        
        if (noflood <= 10 || noflood >= Tsend-10) bequiet = TRUE;
        
        if (++noflood >= Tsend)
        {
            noflood = 0;
            
            static BOOL bcurr = FALSE;
            
            payload.w[0] = bcurr ? IKp    >> IKs    : SKp  >> SKs;
            payload.w[1] = bcurr ? IKi    >>(IKs-1) : SKi  >>(SKs-1);
            payload.w[2] = bcurr ? IKff   >> IKs    : SKff >> SKs;
            payload.w[3] = bcurr ? IKbemf >> IKs    : SKs;
            
            bcurr = !bcurr;
            
            msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__DEBUG );

            ECANSend(msgid, 8, &payload);
        }
//        if(gControlMode != icubCanProto_controlmode_notConfigured)
//        {
//            uint8_t bitmask = 0;
//            bitmask |= ((uint8_t)qe_index_found_debug) << 1;
//            bitmask |= ((uint8_t)gEncoderError.dirty) << 2;
//            bitmask |= ((uint8_t)gEncoderError.index_broken) << 3;
//            payload.w[0]  = gQERawPosition;
//            payload.b[2] = bitmask;
//
//            msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__DEBUG );
//
//            ECANSend(msgid, 3, &payload);
//            
//            //commented because I clean the same flags on ICUBCANPROTO_PER_MC_MSG__STATUS sent
//            //gEncoderError.dirty = FALSE; 
//            //gEncoderError.index_broken = FALSE;
//            qe_index_found_debug = FALSE;
//        }
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

            payload.w[1] = VqFbk;

            payload.b[4] = SysError.b[0];
            payload.b[5] = SysError.b[1];
            payload.b[6] = SysError.b[2];
            payload.b[7] = SysError.b[3];

            msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__STATUS);

            ECANSend(msgid, 8, &payload);
            gEncoderError.dirty = FALSE;
            gEncoderError.index_broken = FALSE;
        }

    }
}

extern void CanSendDebug(tCanData* data, int len)
{
    unsigned long msgid = CAN_ICUBPROTO_STDID_MAKE_TX(ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL, canprototransmitter_bid, ICUBCANPROTO_PER_MC_MSG__DEBUG );

    ECANSend(msgid, len, data);
}

extern void CanIcubProtoTransmitterUpdateBoardId(unsigned char bid)
{
    canprototransmitter_bid = bid;
}
