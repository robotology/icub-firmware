
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
#include "can_icubProto_parser.h"
#include "can_icubProto_trasmitter.h"

//#include "Controller.h"
#include "UserParms.h"
#include "Status.h"
//#include "IControl.h"
#include "EmuROM.h"
#include "crc16.h"
#include "I2T.h"

extern tCtrlReferences CtrlReferences;

typedef struct              // size is 4+60+4+4 = 72
{
    uint32_t                canadr;
    uint8_t                 filler60deviceinfo[60];

    uint8_t         mode;
    uint8_t         dummy0;       
    uint16_t        dummy1;
    uint32_t        crc;
} s_deviceinfo_in_flash_t;

static s_deviceinfo_in_flash_t s_devinfo = {1, {0}, 0, 0, 0, 0};

/*
#define ICUBPROTO_PAYLOAD_ERROR_SET(txpayload_ptr, txlen_ptr, error_code)\
{ \
	*txlen_ptr = 0x1; \
	txpayload->b[0] = error_code; \
}
*/

#define LIMIT(v, L) if (v > (L)) v = (L); else if (v < -(L)) v = -(L);

static unsigned char sCanProtocolCompatible = 0;

static unsigned char canprotoparser_bid;

static int s_canIcubProtoParser_parse_pollingMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen);
static int s_canIcubProtoParser_parse_periodicMsg(unsigned char permsg_type, tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen);
static int s_canIcubProtoParser_parse_canLoaderMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen);

extern volatile unsigned char received_canloader_msg;

void CanIcubProtoParserInit(unsigned char bid)
{
    canprotoparser_bid = bid;
}
// Parse CAN messages complient wit icub can protocol

void CanIcubProtoParserParse(unsigned long rxid, unsigned char rxlen, tCanData *rxpayload)
{
    // Command Syntax ok
    int command_ok = 0;
    unsigned char txlen = 0;
    tCanData txpayload;
    //unsigned long resp_id = 0;
    //unsigned char source; 
    //unsigned char permsg_type;

    unsigned char msg_class = CAN_ICUBPROTO_STDID_GETCLASS(rxid);
    //source = CAN_ICUBPROTO_STDID_RX_GETSOURCE(rxid);
    //permsg_type = CAN_ICUBPROTO_STDID_RX_GETPERMSGTYPE(rxid);

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

    //resp_id = CAN_ICUBPROTO_STDID_MAKE_TX(msg_class, canprotoparser_bid, source);

    if (command_ok)
    {
        if (txlen > 0)
        {
            CanIcubProtoSendResponse(CAN_ICUBPROTO_STDID_MAKE_TX(msg_class, canprotoparser_bid, CAN_ICUBPROTO_STDID_RX_GETSOURCE(rxid)), txlen, &txpayload);
        }
        #ifdef CAN_ACK_EACH_COMMAND 
        else
        {
            CanIcubProtoTxAck(CAN_ICUBPROTO_STDID_MAKE_TX(msg_class, canprotoparser_bid, CAN_ICUBPROTO_STDID_RX_GETSOURCE(rxid)));
        }
        #endif
    }
    //else
    //{
    //    ; //error. What i do??? // CanIcubProtoTxErrorCode(resp_id, txlen, &txpayload);
    //}
}
//returns 1 ==> ok, retuns 0 ==> nok

static int s_canIcubProtoParser_parse_pollingMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    unsigned char cmd = rxpayload->b[0];

    *txlen=0;
    
    switch (cmd)
    {
    case ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN: // DS402 Operation Enable
    {
        if (rxlen!=1 || !DS402_Statusword.Flags.SwitchedOn || !sCanProtocolCompatible) return 0;

        //when start PWM ==> set the default control mode
        SysStatus.b[0] = 0;

        // go to Operation Enable state
        DS402_Controlword.Flags.EnableOperation = 1;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD: // DS402 Operation Disable
    {
        if (rxlen!=1) return 0;

        FaultReset();

        // Command can be accepted only if current ststus is OPERATION ENABLE
        if (!DS402_Statusword.Flags.OperationEnabled && !DS402_Statusword.Flags.SwitchedOn) return 0;

        // In ICUB the states DISABLE_OPERATION and SHUTDOWN goes in the same state.
        // go to Ready to Switch On state
        DS402_Controlword.Flags.EnableVoltage = 0;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD: // DS402 Switch On
    {
        if (rxlen!=1 || !DS402_Statusword.Flags.ReadyToSwitchOn || !sCanProtocolCompatible) return 0;

        received_canloader_msg = 0; // start to transmit status messages

        // go to Switch On state
        DS402_Controlword.Flags.SwitchOn = 1;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE: // DS402 Shutdown
    {
        if (rxlen!=1 || (!DS402_Statusword.Flags.OperationEnabled && !DS402_Statusword.Flags.SwitchedOn)) return 0;

        // go to Ready to Switch On state
        //DS402_Controlword.Flags.EnableVoltage = 0;
        DS402_Controlword.Flags.EnableOperation = 0;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE:
    {
        if (rxlen!=1) return 0;

        txpayload->b[1] = CanIcubProtoGetcontrol_mode();

        if (txpayload->b[1] == icubCanProto_controlmode_unknownError) return 0;

        *txlen = 2;
        txpayload->b[0] = cmd;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE:
    {
        if (rxlen!=2 || !sCanProtocolCompatible) return 0;

        received_canloader_msg = 0; // start to transmit status messages

        iCubProtControlMode = rxpayload->b[1];

        CtrlReferences.IqRef = 0;
        CtrlReferences.VqRef = 0;
        CtrlReferences.WRef = 0;

        if (rxpayload->b[1] != icubCanProto_controlmode_idle)
        {
            DS402_Controlword.Flags.SwitchOn = 1;
            DS402_Controlword.Flags.EnableOperation = 1;
        }

        switch (rxpayload->b[1])
        {
        case icubCanProto_controlmode_openloop:
            SysStatus.b[0] = 0;
            break;
        case icubCanProto_controlmode_current:
            SysStatus.b[0] = 1;
            break;
        case icubCanProto_controlmode_velocity:
        case icubCanProto_controlmode_speed_voltage:
            SysStatus.b[0] = 2;
            break;
        case icubCanProto_controlmode_speed_current:
            SysStatus.b[0] = 3;
            break;
        case icubCanProto_controlmode_idle:
            FaultReset();
            if (!Fault()) LED_status.GreenBlinkRate = BLINKRATE_FAST;
            DS402_Controlword.Flags.EnableOperation = 0; // go to Switched On state
        }
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__WRITE_FLASH_MEM:
        //EepromSave();
        break;

    case ICUBCANPROTO_POL_MC_CMD__GET_ADDITIONAL_INFO:
    case ICUBCANPROTO_POL_MC_CMD__SET_ADDITIONAL_INFO:
    case ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_VELOCITY:
    case ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_VELOCITY:
    break;

    case ICUBCANPROTO_POL_MC_CMD__SET_ENCODER_POSITION:
    {
        // setta l'offset di fasatura
        //Encoder_SyncPulsePosition = (rxpayload->b[2] << 8 | rxpayload->b[1]); //todo aspetta maggia
    }
    break;


    case ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID:
    {
        if (rxlen!=1) return 0;
        
        *txlen=2;
        txpayload->b[0] = cmd;
        txpayload->b[1] = canprotoparser_bid;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__SET_BOARD_ID:
    {
        if (rxlen!=2) return 0;

        #ifndef CAN_CMD_ALWAYS_ACCEPT
        if (!DS402_Statusword.Flags.OperationEnabled && !DS402_Statusword.Flags.SwitchedOn) return 0;
        #endif
        
        canprotoparser_bid = rxpayload->b[1];
        ApplicationData.EepromHeader.EE_CAN_BoardAddress = canprotoparser_bid;
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
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY:
    case ICUBCANPROTO_POL_MC_CMD__GET_MAX_VELOCITY:
        break;

    case ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT:
    {
        if (rxlen!=5) return 0;

        ////il dato che arriva è espresso in mA va trasformato in IAD della 2FOC (1A 1310)
        //il dato che arriva è espresso in mA va trasformato in IAD della 2FOC (1A=2000)
        ApplicationData.CurLimit = (rxpayload->b[2] << 8 | rxpayload->b[1])*2;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION:
    {
        uint8_t server_can_protocol_major = rxpayload->b[1];
        uint8_t server_can_protocol_minor = rxpayload->b[2];

        sCanProtocolCompatible = (
            CAN_PROTOCOL_VERSION_MAJOR == server_can_protocol_major &&
            CAN_PROTOCOL_VERSION_MINOR == server_can_protocol_minor);

        *txlen = 0x8;
        txpayload->b[0] = ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION;
        txpayload->b[1] = icubCanProto_boardType__2foc;
        txpayload->b[2] = FW_VERSION_MAJOR;
        txpayload->b[3] = FW_VERSION_MINOR;
        txpayload->b[4] = FW_VERSION_BUILD;
        txpayload->b[5] = CAN_PROTOCOL_VERSION_MAJOR;
        txpayload->b[6] = CAN_PROTOCOL_VERSION_MINOR;
        txpayload->b[7] = sCanProtocolCompatible; // can_protocol_ack;

        if (!sCanProtocolCompatible)
        {
            // go to fault state
            SysError.CANInvalidProtocol = 1;
            // call fault handler
            FaultConditionsHandler();
        }

    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID:
    {
        SFRAC16 pp, pi, pd, pm; //pm non viene passato dal comando

        if (rxlen!=7) return 0;

        //ControllerGetCurrentDPIDParm(&pp,&pi,&pd,&pm);
        pp = (rxpayload->b[2] << 8 | rxpayload->b[1]);
        pi = (rxpayload->b[4] << 8 | rxpayload->b[3]);
        pd = 0;
        pm = (rxpayload->b[6] << 8 | rxpayload->b[5]);


        //ControllerSetCurrentDPIDParm(pp, pi, pd, pm);
        //ControllerSetCurrentQPIDParm(pp, pi, pd, pm);
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PID:
    {
        signed int p, i, d;

        if (rxlen!=1) return 0;

        //ControllerGetCurrentDPIDParm(&p, &i, &d, &m);
        txpayload->b[0] = cmd;
        memcpy(&txpayload->b[1], &p, 2);
        memcpy(&txpayload->b[3], &i, 2);
        memcpy(&txpayload->b[5], &d, 2);
        *txlen = 7;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID:
    {
        SFRAC16 pp, pi, pd; //pm non viene passato dal comando

        if (rxlen!=7) return 0;
        
        //ControllerGetWPIDParm(&pp, &pi, &pd, &pm);
        pp = (rxpayload->b[2] << 8 | rxpayload->b[1]);
        pi = (rxpayload->b[4] << 8 | rxpayload->b[3]);
        pd = (rxpayload->b[6] << 8 | rxpayload->b[5]);
        //ControllerSetWPIDParm(pp, pi, pd, pm);
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PID:
    {
        signed int p, i, d;

        if (1 != rxlen) return (0);
        
        //ControllerGetWPIDParm(&p, &i, &d, &m);
        txpayload->b[0] = cmd;
        memcpy(&txpayload->b[1], &p, 2);
        memcpy(&txpayload->b[3], &i, 2);
        memcpy(&txpayload->b[5], &d, 2);
        *txlen = 7;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT:
    {
        if (rxlen!=5) return 0;

#ifndef CAN_CMD_ALWAYS_ACCEPT
        if (!DS402_Statusword.Flags.OperationEnabled && !DS402_Statusword.Flags.SwitchedOn) return 0;
#endif
        // Torque control references
        int ctrlRef = (rxpayload->b[2] << 8 | rxpayload->b[1]);

        switch (iCubProtControlMode)
        {
        case icubCanProto_controlmode_velocity:
        case icubCanProto_controlmode_speed_voltage:
        case icubCanProto_controlmode_speed_current:
            CtrlReferences.WRef = ctrlRef;
            LIMIT(CtrlReferences.WRef, UDEF_SPEED_MAX)
            break;

        case icubCanProto_controlmode_current:
            CtrlReferences.IqRef = ctrlRef;
            LIMIT(CtrlReferences.IqRef, UDEF_CURRENT_MAX)
            break;

        case icubCanProto_controlmode_openloop:
            CtrlReferences.VqRef = ctrlRef;
            LIMIT(CtrlReferences.VqRef, UDEF_PWM_MAX)
            break;

        case icubCanProto_controlmode_idle:
            break;
        }

#ifdef SYNC_2FOC_TO_EMS
        CanIcubProtoTrasmitterSendPeriodicData();
#endif    
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_CURRENT:
    {
        if (rxlen!=1) return 0;

        *txlen = 5;
        txpayload->b[0] = cmd;
        memcpy(&txpayload->b[1], &CtrlReferences.IqRef, 2);
        txpayload->b[3] = txpayload->b[4] = 0;
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__SET_PERIODIC_MSG_CONTENTS:
    {
        if (rxlen!=5) return 0;

        // check data to transmit if in the acceptable range
        if ((rxpayload->b[1] < ELEMENTS_IN_PREIODIC_DATA_LIST)
                && (rxpayload->b[2] < ELEMENTS_IN_PREIODIC_DATA_LIST)
                && (rxpayload->b[3] < ELEMENTS_IN_PREIODIC_DATA_LIST)
                && (rxpayload->b[4] < ELEMENTS_IN_PREIODIC_DATA_LIST))
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
        }
        else
        {
            return 0;
        }
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS:
    {
        if (rxlen!=5) return 0;
     
        //extract params from the 1st word
        I2Tdata.Param = (rxpayload->b[2] << 8 | rxpayload->b[1]);
        I2Tdata.IThreshold = (rxpayload->b[4] << 8 | rxpayload->b[3]);
    }
    break;

    case ICUBCANPROTO_POL_MC_CMD__GET_I2T_PARAMS:
    {
        if (rxlen!=1) return 0;
 
        *txlen = 5;
        txpayload->b[0] = cmd;
        memcpy(&txpayload->b[1], &I2Tdata.Param, 2);
        memcpy(&txpayload->b[3], &I2Tdata.IThreshold, 2);
    }
    break;

    default:
        return 0;
    } //end switch
    return 1;
}

static int s_canIcubProtoParser_parse_periodicMsg(unsigned char permsg_type, tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    *txlen=0;

    if (permsg_type == ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT)
    {
        if ((canprotoparser_bid<<1) > rxlen) return 0;

        switch (iCubProtControlMode)
        {
        case icubCanProto_controlmode_velocity:
        case icubCanProto_controlmode_speed_voltage:
        case icubCanProto_controlmode_speed_current:
            CtrlReferences.WRef =  rxpayload->w[canprotoparser_bid-1];
            LIMIT(CtrlReferences.WRef, UDEF_SPEED_MAX)
            CtrlReferences.IqRef = CtrlReferences.VqRef = 0;
            break;

        case icubCanProto_controlmode_current:
            CtrlReferences.IqRef =  rxpayload->w[canprotoparser_bid-1];
            LIMIT(CtrlReferences.IqRef, UDEF_CURRENT_MAX)
            CtrlReferences.VqRef = CtrlReferences.WRef = 0;
            break;

        case icubCanProto_controlmode_openloop:
            CtrlReferences.VqRef =  rxpayload->w[canprotoparser_bid-1];
            LIMIT(CtrlReferences.VqRef, UDEF_PWM_MAX)
            CtrlReferences.IqRef = CtrlReferences.WRef = 0;
            break;

        case icubCanProto_controlmode_idle:
            CtrlReferences.VqRef = CtrlReferences.IqRef = CtrlReferences.WRef = 0;
            break;
        }
        
        #ifdef SYNC_2FOC_TO_EMS
            CanIcubProtoTrasmitterSendPeriodicData();
        #endif   
        
        return 1;
    }

    return 0;
}

static int s_canIcubProtoParser_parse_canLoaderMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    unsigned char cmd = rxpayload->b[0];
    
    *txlen=0;

    switch (cmd)
    {
    case ICUBCANPROTO_BL_BROADCAST:
    {
        if (rxlen!=1) return 0;
       
        received_canloader_msg = 1; // do not transmit status messages

        *txlen=5;
        txpayload->b[0] = cmd;
        txpayload->b[1] = icubCanProto_boardType__2foc;
        txpayload->b[2] = FW_VERSION_MAJOR;
        txpayload->b[3] = FW_VERSION_MINOR;
        txpayload->b[4] = FW_VERSION_BUILD;
    }
    break;

    case ICUBCANPROTO_BL_BOARD:
    {
        asm ("reset"); // Jump to bootloader code
    }
    break;

    case ICUBCANPROTO_BL_GET_ADDITIONAL_INFO:
    case ICUBCANPROTO_BL_SET_ADDITIONAL_INFO:
    break;


    default:
        return 0;
    } //end switch

    return 1;
}
