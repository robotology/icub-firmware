
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
#include <libpic30.h>

#include "can_icubProto.h"
#include "can_icubProto_parser.h"
#include "can_icubProto_trasmitter.h"

#include "UserParms.h"
#include "Status.h"
#include "crc16.h"
#include "2FOC.h"
#include "ecan.h"
#include "qep.h"

typedef struct      // size is 4+60+4+4 = 72
{
    uint32_t        canadr;
    uint8_t         filler60deviceinfo[60];

    uint8_t         mode;
    uint8_t         dummy0;       
    uint16_t        dummy1;
    uint32_t        crc;
} s_deviceinfo_in_flash_t;

static s_deviceinfo_in_flash_t s_devinfo = {1, {0}, 0, 0, 0, 0};

#define LIMIT(v, L) if (v > (L)) v = (L); else if (v < -(L)) v = -(L);

volatile unsigned char gCanProtocolCompatible = 0;
static unsigned char canprotoparser_bid;

static int s_canIcubProtoParser_parse_pollingMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen);
static int s_canIcubProtoParser_parse_periodicMsg(unsigned char permsg_type, tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen);
static int s_canIcubProtoParser_parse_canLoaderMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen);

//extern volatile unsigned char received_canloader_msg;

void CanIcubProtoParserInit(unsigned char bid)
{
    canprotoparser_bid = bid;
}
// Parse CAN messages complient wit icub can protocol

void CanIcubProtoParserParse(unsigned long rxid, unsigned char rxlen, tCanData *rxpayload)
{
    tCanData txpayload;

    // Command Syntax ok
    int command_ok = 0;
    unsigned char txlen = 0;
    
    unsigned char msg_class = CAN_ICUBPROTO_STDID_GETCLASS(rxid);

    switch (msg_class)
    {
    case ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL:
        command_ok = s_canIcubProtoParser_parse_pollingMsg(rxpayload, rxlen, &txpayload, &txlen);
        break;

    case ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL:
        command_ok = s_canIcubProtoParser_parse_periodicMsg(CAN_ICUBPROTO_STDID_RX_GETPERMSGTYPE(rxid), rxpayload, rxlen, &txpayload, &txlen);
        break;

    case ICUBCANPROTO_CLASS_BOOTLOADER:
        command_ok = s_canIcubProtoParser_parse_canLoaderMsg(rxpayload, rxlen, &txpayload, &txlen);
        break;
    }

    if (command_ok)
    {
        if (txlen > 0)
        {
            CanIcubProtoSendResponse(CAN_ICUBPROTO_STDID_MAKE_TX(msg_class, canprotoparser_bid, CAN_ICUBPROTO_STDID_RX_GETSOURCE(rxid)), txlen, &txpayload);
        }
    }
}

static int s_canIcubProtoParser_parse_pollingMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    unsigned char cmd = rxpayload->b[0];

    *txlen=0;

    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE)
    {
        if (rxlen!=2) return 0;
        
        if (!gCanProtocolCompatible) return 0;

        RequestControlMode(rxpayload->b[1]);

        return 1;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE)
    {
        if (rxlen!=1) return 0;

        txpayload->b[0] = cmd;
        txpayload->b[1] = gControlMode;
        *txlen = 2;

        return 1;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID)
    {
        if (rxlen!=1) return 0;
        
        txpayload->b[0] = cmd;
        txpayload->b[1] = canprotoparser_bid;
        *txlen=2;

        return 1;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_BOARD_ID)
    {
        if (rxlen!=2) return 0;
        
        if (gControlMode != icubCanProto_controlmode_idle
         && gControlMode != icubCanProto_controlmode_notConfigured) return 0;
        
        canprotoparser_bid = rxpayload->b[1];
        //ApplicationData.EepromHeader.EE_CAN_BoardAddress = canprotoparser_bid;
        CanIcubProtoTransmitterUpdateBoardId(canprotoparser_bid);
        CanIcubProtoSetFilters(canprotoparser_bid);

        _memcpy_p2d16(&s_devinfo, 0x15000, sizeof(s_deviceinfo_in_flash_t));

        _erase_flash(0x15000);

        s_devinfo.canadr = canprotoparser_bid;

        s_devinfo.mode   = 1;
        s_devinfo.dummy0 = 0;
        s_devinfo.dummy1 = 0xcaac;
        s_devinfo.crc    = crc16(0xFFFF, (const unsigned char*)&s_devinfo, 64);
                  
        int i;
        for (i=0; i<sizeof(s_deviceinfo_in_flash_t); i+=_FLASH_ROW)
        {
            _write_flash16(0x15000+i, ((int*)(&s_devinfo))+i/2);
        }

        return 1;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT)
    {
        if (!gCanProtocolCompatible) return 0;
        
        if (rxlen==5)
        {
            int peak  = ((int)rxpayload->b[1])|(((int)rxpayload->b[2])<<8);

            setMaxCurrent(peak/2, peak, 20000);
            
            return 1;
        }
        else if (rxlen==8)
        {
            int nom  = ((int)rxpayload->b[2])|(((int)rxpayload->b[3])<<8);
            int peak = ((int)rxpayload->b[4])|(((int)rxpayload->b[5])<<8);
            int ovr  = ((int)rxpayload->b[6])|(((int)rxpayload->b[7])<<8);

            setMaxCurrent(nom, peak, ovr);

        return 1;
    }
    
        return 0;
    }
    
    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG)
    {
        if (rxlen < 7) return 0;
        
        if (!gCanProtocolCompatible) return 0;

        MotorConfig.bitmask = rxpayload->b[1];
        MotorConfig.configured = TRUE;
        
        gEncoderConfig.resolution = rxpayload->w[1];
        gEncoderConfig.tolerance  = (rxlen == 8) ? rxpayload->b[7] : 36;
        gEncoderConfig.numPoles   = rxpayload->b[6]/2;
        gEncoderConfig.full_calibration = FALSE;

        if (MotorConfig.has_hall)
        {
            gEncoderConfig.offset = 330;
            MotorConfig.has_index = FALSE;
        }
        else if (MotorConfig.has_qe)
        {
            MotorConfig.has_speed_qe = FALSE;
            
            gEncoderConfig.offset = rxpayload->w[2];
        
            if (MotorConfig.has_index && gEncoderConfig.offset == -1)
            {
                gEncoderConfig.full_calibration = TRUE;
                gEncoderConfig.offset = 0;
                MotorConfig.verbose = TRUE;
            }
        }
        
        return 1;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION)
    {
        if (rxlen != 3) return 0;

        uint8_t server_can_protocol_major = rxpayload->b[1];
        uint8_t server_can_protocol_minor = rxpayload->b[2];

        gCanProtocolCompatible = CAN_PROTOCOL_VERSION_MAJOR == server_can_protocol_major 
                              && CAN_PROTOCOL_VERSION_MINOR == server_can_protocol_minor;

        *txlen = 8;
        txpayload->b[0] = ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION;
        txpayload->b[1] = icubCanProto_boardType__2foc;
        txpayload->b[2] = FW_VERSION_MAJOR;
        txpayload->b[3] = FW_VERSION_MINOR;
        txpayload->b[4] = FW_VERSION_BUILD;
        txpayload->b[5] = CAN_PROTOCOL_VERSION_MAJOR;
        txpayload->b[6] = CAN_PROTOCOL_VERSION_MINOR;
        txpayload->b[7] = gCanProtocolCompatible; // can_protocol_ack;

        if (!gCanProtocolCompatible)
        {
            //SysError.CANInvalidProtocol = 1;
            //FaultConditionsHandler();
        }

        return 1;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID)
    {
        if (!gCanProtocolCompatible) return 0;
        
        if (rxlen==8)
        {
            int  kp=((int)rxpayload->b[1])|(((int)rxpayload->b[2])<<8);
            int  ki=((int)rxpayload->b[3])|(((int)rxpayload->b[4])<<8);
            char ks=rxpayload->b[7];

            setIPid(kp,ki,ks);

            //setIPid(*(int*)(&rxpayload->b[1]),*(int*)(&rxpayload->b[3]),rxpayload->b[7]);
            
            return 1;
        }
        else if (rxlen==6)
        {
            static float fkp = 0.0f;
            static float fki = 0.0f;
            
            switch (rxpayload->b[1])
            {
                case 1: fkp = *(float*)&rxpayload->b[2]; break;
                case 2: fki = *(float*)&rxpayload->b[2]; break;
                default: return 0;
            }
            
            float max = fkp;
            if (fki > max) max = fki;
    
            int exponent = 0;
            
            for (exponent = 0; exponent < 16; ++exponent)
            {
                float power = (float)(1<<exponent);
        
                if (max < power)
                {
                    setIPid((int)(fkp*32768.0f/power),(int)(fki*32768.0f/power),15-exponent);
                    
                    return 1;
                }
            }
            
            return 0;
        }
        
        return 0;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID)
    {
        if (!gCanProtocolCompatible) return 0;
                
        if (rxlen==8)
        {
            int  kp=((int)rxpayload->b[1])|(((int)rxpayload->b[2])<<8);
            int  ki=((int)rxpayload->b[3])|(((int)rxpayload->b[4])<<8);
            char ks=rxpayload->b[7];

            setSPid(kp,ki,ks);
            
            //setSPid(*(int*)(&rxpayload->b[1]),*(int*)(&rxpayload->b[3]),rxpayload->b[7]);
            
            return 1;
        }
        else if (rxlen==6)
        {
            static float fkp = 0.0f;
            static float fki = 0.0f;
            
            switch (rxpayload->b[1])
            {
                case 1: fkp = *(float*)&rxpayload->b[2]; break;
                case 2: fki = *(float*)&rxpayload->b[2]; break;
                default: return 0;
            }
            
            float max = fkp;
            if (fki > max) max = fki;
    
            int exponent = 0;
            
            for (exponent = 0; exponent < 16; ++exponent)
            {
                float power = (float)(1<<exponent);
        
                if (max < power)
                {
                    setSPid((int)(fkp*32768.0f/power),(int)(fki*32768.0f/power),15-exponent);
                    
                    return 1;
                }
            }
            
            return 0;
        }
        
        return 0;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_VELOCITY)
    {
        if (rxlen!=5) return 0;
        
        if (!gCanProtocolCompatible) return 0;

        // acceleration is unused

        if (gControlMode == icubCanProto_controlmode_speed_voltage
         || gControlMode == icubCanProto_controlmode_speed_current)
        {
            CtrlReferences.WRef = (rxpayload->b[2] << 8 | rxpayload->b[1]);
            LIMIT(CtrlReferences.WRef, UDEF_SPEED_MAX)

            return 1;
        }

        return 0;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS)
    {
        if (rxlen!=3) return 0;
        
        if (!gCanProtocolCompatible) return 0;

        if (gControlMode != icubCanProto_controlmode_openloop) return 0;

        CtrlReferences.VqRef = (rxpayload->b[2] << 8 | rxpayload->b[1]);
        LIMIT(CtrlReferences.VqRef, UDEF_PWM_MAX)

        return 1;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT)
    {
        if (rxlen!=5) return 0;

        if (!gCanProtocolCompatible) return 0;
        
        if (gControlMode != icubCanProto_controlmode_current) return 0;

        CtrlReferences.IqRef = (rxpayload->b[2] << 8 | rxpayload->b[1]);
        LIMIT(CtrlReferences.IqRef, Ipeak)

        return 1;
    }

    if (cmd == ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_CURRENT)
    {
        if (rxlen!=1) return 0;

        *txlen = 5;
        txpayload->b[0] = cmd;
        memcpy(&txpayload->b[1], (void*)&(CtrlReferences.IqRef), 2);
        txpayload->b[3] = txpayload->b[4] = 0;

        return 1;
    }

    /*
    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_PERIODIC_MSG_CONTENTS)
    {
        if (rxlen!=5) return 0;

        // check data to transmit if in the acceptable range
        if ((rxpayload->b[1] < ELEMENTS_IN_PERIODIC_DATA_LIST)
         && (rxpayload->b[2] < ELEMENTS_IN_PERIODIC_DATA_LIST)
         && (rxpayload->b[3] < ELEMENTS_IN_PERIODIC_DATA_LIST)
         && (rxpayload->b[4] < ELEMENTS_IN_PERIODIC_DATA_LIST))
        {
            // set the data to transmit
            PeriodicMessageContents[0] = rxpayload->b[1];
            PeriodicMessageContents[1] = rxpayload->b[2];
            PeriodicMessageContents[2] = rxpayload->b[3];
            PeriodicMessageContents[3] = rxpayload->b[4];

            // precalculate this here. This is an optimization
            gulpadr1 = (unsigned int*) PeriodicData[PeriodicMessageContents[0]];
            gulpadr2 = (unsigned int*) PeriodicData[PeriodicMessageContents[1]];
            gulpadr3 = (unsigned int*) PeriodicData[PeriodicMessageContents[2]];
            gulpadr4 = (unsigned int*) PeriodicData[PeriodicMessageContents[3]];

            return 1;
        }
        return 0;
    }
    */

    if (cmd == ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS)
    {
        if (rxlen!=5) return 0;
  
        //I2Tdata.Param = (rxpayload->b[2] << 8 | rxpayload->b[1]);
        //I2Tdata.IThreshold = (rxpayload->b[4] << 8 | rxpayload->b[3]);

        return 1;
    }

    return 0;
}

static int s_canIcubProtoParser_parse_periodicMsg(unsigned char permsg_type, tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    *txlen=0;

    if (permsg_type == ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT)
    {
        SETPOINT_WATCHDOG_REARM()

        if ((canprotoparser_bid<<1) > rxlen) return 0;

        int ref = rxpayload->w[canprotoparser_bid-1];

        switch (gControlMode)
        {
        case icubCanProto_controlmode_velocity:
        case icubCanProto_controlmode_speed_voltage:
        case icubCanProto_controlmode_speed_current:
            LIMIT(ref, UDEF_SPEED_MAX)
            CtrlReferences.WRef  = ref;
            break;

        case icubCanProto_controlmode_current:
            LIMIT(ref, Ipeak)
            CtrlReferences.IqRef = ref;
            //CtrlReferences.VqRef = CtrlReferences.WRef = 0;
            break;

        case icubCanProto_controlmode_openloop:
            LIMIT(ref, UDEF_PWM_MAX)
            CtrlReferences.VqRef = ref;
            //CtrlReferences.IqRef = CtrlReferences.WRef = 0;
            break;

        case icubCanProto_controlmode_idle:
            ZeroControlReferences();
            break;
        }

        CanIcubProtoTrasmitterSendPeriodicData();
        
        return 1;
    }

    return 0;
}

static int s_canIcubProtoParser_parse_canLoaderMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    unsigned char cmd = rxpayload->b[0];
    
    *txlen=0;

    if (cmd == ICUBCANPROTO_BL_BROADCAST)
    {
        if (rxlen!=1) return 0;
       
        *txlen=5;
        txpayload->b[0] = cmd;
        txpayload->b[1] = icubCanProto_boardType__2foc;
        txpayload->b[2] = FW_VERSION_MAJOR;
        txpayload->b[3] = FW_VERSION_MINOR;
        txpayload->b[4] = FW_VERSION_BUILD;

        return 1;
    }

    if (cmd == ICUBCANPROTO_BL_BOARD)
    {
        asm ("reset"); // Jump to bootloader code
        return 1;
    }

    if (cmd == ICUBCANPROTO_BL_GET_ADDITIONAL_INFO) return 1;
    if (cmd == ICUBCANPROTO_BL_SET_ADDITIONAL_INFO) return 1;

    return 0;
}
