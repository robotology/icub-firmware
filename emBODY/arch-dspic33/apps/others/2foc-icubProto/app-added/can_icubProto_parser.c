
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

#include "ecan.h"
#include "UserParms.h"
#include "tle5012.h"
#include "controller.h"
#include "system.h"
#include "Status.h"
#include "IControl.h"
#include "DSP.h"
#include "can_icubProto_parser.h"
#include "can_icubProto_trasmitter.h"
#include "can_icubProto_messages.h"
#include "EmuROM.h"
#include "can_icubProto.h"
#include "faults.h"


#define ICUBPROTO_PAYLOAD_ERROR_SET(txpayload_ptr, txlen_ptr, error_code)\
{ \
	*txlen_ptr = 0x1; \
	txpayload->b[0] = error_code; \
}


__inline__ static void s_controlMode_reset(void)
{
    SysStatus.b[0] = SysStatus.b[0] & 0x03;
}

/*the default control mode is current*/
__inline__ static void s_controlMode_setDefault(void) 
{
    SysStatus.b[0] |= (1 << 6);
}


__inline__ static void s_controlMode_setCurrent(void)
{ 
    SysStatus.b[0] |= (1 << 6);
}


__inline__ static void s_controlMode_setVelocity(void)
{
    SysStatus.b[0] |= (1 << 5);
}
 



static unsigned char canprotoparser_bid;

static int s_canIcubProtoParser_parse_pollingMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen);
static int s_canIcubProtoParser_parse_periodicMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen);
static int s_canIcubProtoParser_parse_canLoaderMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen);

void CanIcubProtoParserInit(unsigned char bid)
{
	canprotoparser_bid = bid;
}
// Parse CAN messages complient wit icub can protocol
void CanIcubProtoParserParse(unsigned long rxid, unsigned char rxlen, tCanData *rxpayload)
{
  // Command Syntax ok  
  int command_ok=1;
  unsigned char txlen = 0;
  unsigned char msg_class = 255;
  unsigned char source;
  tCanData txpayload;
  unsigned long resp_id=0;
  
   msg_class = CAN_ICUBPROTO_STDID_GETCLASS(rxid);
   source = CAN_ICUBPROTO_STDID_RX_GETSOURCE(rxid);
  
    switch(msg_class)
    {
        case ICUBPROTO_CLASS_POLLING_MOTORBOARD:
        {
            command_ok = s_canIcubProtoParser_parse_pollingMsg(rxpayload, rxlen, &txpayload, &txlen);
        }break;
        
        case ICUBPROTO_CLASS_PERIODIC_MOTORBOARD:
        {
            command_ok = s_canIcubProtoParser_parse_periodicMsg(rxpayload, rxlen, &txpayload, &txlen);
        }break;
        
        case ICUBPROTO_CLASS_CANLOADER:
        {
            command_ok = s_canIcubProtoParser_parse_canLoaderMsg(rxpayload, rxlen, &txpayload, &txlen);
        }break;
        
        default:
        {
            txlen = 0x1;
	        txpayload.b[0] =CAN_PROTO_ERROR_UNKNOWN_COMMAND; 
            command_ok = 0;
        }
    }
    
    resp_id = CAN_ICUBPROTO_STDID_MAKE_TX(msg_class, canprotoparser_bid, source);
    if(command_ok)
    {
        if(txlen > 0)
        {
            CanIcubProtoSendResponse(resp_id, txlen, &txpayload);
        }
#ifdef CAN_ACK_EACH_COMMAND 
        else
        {
            CanIcubProtoTxAck(resp_id);
        }
#endif
    }
    else
    {
      CanIcubProtoTxErrorCode(resp_id, txlen, &txpayload);
    }
}
//returns 1 ==> ok, retuns 0 ==> nok
static int s_canIcubProtoParser_parse_pollingMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    unsigned char cmd = rxpayload->b[0];
    
    *txlen = 0x0; 
    switch(cmd)
    {
        case ICUBPROTO_POLLINGCMD_CONTROLLER_RUN: // DS402 Operation Enable 
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen=0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
         
            // Command can be accepted only if current ststus is SWITCHED ON 
            if(1 != DS402_Statusword.Flags.SwitchedOn )
            {
                *txlen = 0x1;
                txpayload->b[0] = CAN_DS402_INVALID_STATUS_CHANGE;
                return(0);
            }
            
            //when start PWM ==> set the default control mode
            s_controlMode_reset();
           s_controlMode_setDefault();

            // go to Operation Enable state
            DS402_Controlword.Flags.EnableOperation = 1;       
        }break;
        
        case ICUBPROTO_POLLINGCMD_DISABLE_PWM_PAD: // DS402 Operation Disable
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
         
            // Command can be accepted only if current ststus is OPERATION ENABLE
            if ( (1 != DS402_Statusword.Flags.OperationEnabled ) && (1 != DS402_Statusword.Flags.SwitchedOn ) )
            {
                *txlen = 0x1;
                txpayload->b[0] = CAN_DS402_INVALID_STATUS_CHANGE;
                return(0);
            }
			// go to Ready to Switch On state
            DS402_Controlword.Flags.EnableVoltage = 0;   
            // In ICUB the states DISABLE_OPERATION and SHUTDOWN goes in the same state. 
            // go to Switched On state
            //DS402_Controlword.Flags.EnableOperation = 0;
        }break;
    
        case ICUBPROTO_POLLINGCMD_ENABLE_PWM_PAD: // DS402 Switch On 
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            
            // Command can be accepted only if current ststus is SWITCH ON DISABLED 
            if(1 != DS402_Statusword.Flags.ReadyToSwitchOn )
            {
                *txlen = 0x1;
                txpayload->b[0] = CAN_DS402_INVALID_STATUS_CHANGE;
                return(0);
            }
            // go to Switch On state
            DS402_Controlword.Flags.SwitchOn = 1;
        }break;
        
        case ICUBPROTO_POLLINGCMD_CONTROLLER_IDLE: // DS402 Shutdown
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            // Command can be accepted only if current ststus is OPERATION ENABLE or SWITCHED ON 
            if ( (1 != DS402_Statusword.Flags.OperationEnabled ) && (1 != DS402_Statusword.Flags.SwitchedOn ) )
            {
                *txlen = 0x1;
                txpayload->b[0] = CAN_DS402_INVALID_STATUS_CHANGE;
                return(0);
            }

            // go to Ready to Switch On state
            DS402_Controlword.Flags.EnableVoltage = 0;   
        }break;
        
        case ICUBPROTO_POLLINGCMD_GET_CONTROL_MODE:
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            
            txpayload->b[1] = CanIcubProtoGetcontrol_mode();
            if( controlMode_error == txpayload->b[1])
            {
                *txlen = 0x1;
                txpayload->b[0] =  CAN_ERROR_INCONSISTENT_STATE;
                return(0);
            }

            txpayload->b[0] = cmd;
            *txlen = 0x2;

        }break;

        case ICUBPROTO_POLLINGCMD_SET_CONTROL_MODE:
        {
            if(2 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }

            //this command has effect only when i'm in enable operation state
            if(1 != DS402_Controlword.Flags.EnableOperation)
            {
                *txlen = 0x1;
                txpayload->b[1] = CAN_ERROR_COMMAND_IN_INVALID_STATE;
                return(0);    
            }

            //let change control mode on fly
            s_controlMode_reset();

            switch(rxpayload->b[1])
            {           
                case ICUBPROTO_CONTROLMODE_CURRENT:
                {
                    SysStatus.TorqueControl = 1;
                }break;
                case ICUBPROTO_CONTROLMODE_VELOCITY:
                {
                    SysStatus.SpeedControl = 1;
                }break;
                
                case ICUBPROTO_CONTROLMODE_IDLE:
                {
                    // go to Switched On state
                    DS402_Controlword.Flags.EnableOperation = 0;
                }break;

                case ICUBPROTO_CONTROLMODE_POSITION:
                case ICUBPROTO_CONTROLMODE_OPENLOOP:
                case ICUBPROTO_CONTROLMODE_TORQUE:
                {
                   s_controlMode_setDefault();
                }break;
 
                default:
                {
                    s_controlMode_setDefault();
                }break;
            }

        }break;

        case ICUBPROTO_POLLINGCMD_WRITE_FLASH_MEM:
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
#ifndef CAN_CMD_ALWAYS_ACCEPT
            if ( (1 != DS402_Statusword.Flags.OperationEnabled ) && (1 != DS402_Statusword.Flags.SwitchedOn ) )
            {
              *txlen=0x1;
              txpayload->b[0] =  CAN_ERROR_COMMAND_IN_INVALID_STATE;
              return(0);
            }
#endif
            EepromSave();
        }break;
        
        case ICUBPROTO_POLLINGCMD_GET_ADDITIONAL_INFO:
        {
            ;   //todo
        }break;

        case ICUBPROTO_POLLINGCMD_SET_ADDITIONAL_INFO:
        {
            ;   //todo
        }break;
        
        case ICUBPROTO_POLLINGCMD_GET_DESIRED_VELOCITY: 
        {
            ;  //todo 
        }break;
        
        case ICUBPROTO_POLLINGCMD_SET_DESIRED_VELOCITY: 
        {
            ;  //todo 
        }break;

        case ICUBPROTO_POLLINGCMD_SET_ENCODER_POSITION: 
        {
            ;  //todo aspetta maggia
        }break;
        
        
        case ICUBPROTO_POLLINGCMD_GET_BOARD_ID: 
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            *txlen = 0x2;
            txpayload->b[0] = cmd;
            txpayload->b[1] = canprotoparser_bid;
        }break;

        case ICUBPROTO_POLLINGCMD_SET_BOARD_ID: 
        {
            *txlen=0x1;
             txpayload->b[0] = CAN_PROTO_ERROR_COMMAND_NOT_INPLEMENTED;
             return(0);
            
            /* currently it is NOT possible change board can address, 
            so the code below is commented, but it is right. */

//            if(2 != rxlen)
//            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
//                return(0);
//            }
//#ifndef CAN_CMD_ALWAYS_ACCEPT
//            if ( (1 != DS402_Statusword.Flags.OperationEnabled ) && (1 != DS402_Statusword.Flags.SwitchedOn ) )
//            {
//              *txlen=0x1;
//              txpayload->b[0] =  CAN_ERROR_COMMAND_IN_INVALID_STATE;
//              return(0);
//            }
//#endif
//            ApplicationData.EepromHeader.EE_CAN_BoardAddress = rxpayload->b[1];
//            canprotoparser_bid = rxpayload->b[1];
//            CanIcubProtoTransmitterUpdateBoardId(canprotoparser_bid);
//            CanIcubProtoSetFilters(canprotoparser_bid);
        }break;
        
        case ICUBPROTO_POLLINGCMD_SET_MAX_VELOCITY: 
        {
            ; //aspetta maggia
        }break;

        case ICUBPROTO_POLLINGCMD_GET_MAX_VELOCITY: 
        {
            ; //aspetta maggia
        }break;

        case ICUBPROTO_POLLINGCMD_SET_CURRENT_LIMIT: 
        {
            ; //aspetta maggia
        }break;
        
        case ICUBPROTO_POLLINGCMD_GET_FIRMWARE_VERSION: 
        {
            ; //todo da leggere dalla partizione giusta della eeprom
        }break;
        
        case ICUBPROTO_POLLINGCMD_SET_CURRENT_PID_PARAM: 
        {
            tCanData auxpayload;
            if(7 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }

            memcpy(&auxpayload.b[0], &rxpayload->b[1], 6); //brutto da vedersi, ma dovrebbe funzionare
            ControllerSetCurrentDPIDParm(auxpayload.w[0],auxpayload.w[1],auxpayload.w[2]);
			ControllerSetCurrentQPIDParm(auxpayload.w[0],auxpayload.w[1],auxpayload.w[2]);
        }break;
        
        case ICUBPROTO_POLLINGCMD_GET_CURRENT_PID_PARAM: 
        {
            signed int p, i, d;
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            
            ControllerGetCurrentDPIDParm(&p, &i, &d);
            txpayload->b[0] = cmd;
            memcpy(&txpayload->b[1], &p, 2);
            memcpy(&txpayload->b[3], &i, 2);
            memcpy(&txpayload->b[5], &d, 2);
            *txlen = 0x7;   
        }break;
        
        case ICUBPROTO_POLLINGCMD_SET_VELOCITY_PID_PARAM: 
        {
            tCanData auxpayload;
            if(7 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }

            memcpy(&auxpayload.b[0], &rxpayload->b[1], 6); //brutto da vedersi, madovrebbe funzionare
            ControllerSetWPIDParm(auxpayload.w[0], auxpayload.w[1], auxpayload.w[2]);
        }break;
        
        case ICUBPROTO_POLLINGCMD_GET_VELOCITY_PID_PARAM: 
        {
            signed int p, i, d;
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            ControllerGetWPIDParm(&p, &i, &d);
            txpayload->b[0] = cmd;
            memcpy(&txpayload->b[1], &p, 2);
            memcpy(&txpayload->b[3], &i, 2);
            memcpy(&txpayload->b[5], &d, 2);
            *txlen = 0x7;
        }break;

        case ICUBPROTO_POLLINGCMD_SET_DESIRED_CURRENT: 
        {
            if(5 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            
#ifndef CAN_CMD_ALWAYS_ACCEPT
            if ( (1 != DS402_Statusword.Flags.OperationEnabled ) && (1 != DS402_Statusword.Flags.SwitchedOn ) )
            {
              *txlen=0x1;
              txpayload->b[0] =  CAN_ERROR_COMMAND_IN_INVALID_STATE;
              return(0);
            }
#endif
            // Torque control references
            CtrlReferences.qIqRef = (rxpayload->b[2] << 8 | rxpayload->b[1]);
            // set reference value for toggling torque
            TorqueTogglingReference = CtrlReferences.qIqRef;
            // Flux control references
            CtrlReferences.qIdRef = (rxpayload->b[4] << 8 | rxpayload->b[3]);  
#ifdef SYNC_2FOC_TO_EMS
            CanIcubProtoTrasmitterSendPeriodicData();          
#endif    
        }break;
        
        case ICUBPROTO_POLLINGCMD_GET_DESIRED_CURRENT: 
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            
            *txlen=0x5;
            txpayload->b[0] = cmd;
            memcpy(&txpayload->b[1], &CtrlReferences.qIqRef, 2);
            memcpy(&txpayload->b[3], &CtrlReferences.qIdRef, 2);    
        }break;

        case ICUBPROTO_POLLINGCMD_SET_PERIODIC_MSG_CONTENTS: 
        {
            if(5 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            
            // check data to transmit if in the acceptable range 
            if ( (rxpayload->b[1] < ELEMENTS_IN_PREIODIC_DATA_LIST )
              && (rxpayload->b[2] < ELEMENTS_IN_PREIODIC_DATA_LIST )
              && (rxpayload->b[3] < ELEMENTS_IN_PREIODIC_DATA_LIST )
              && (rxpayload->b[4] < ELEMENTS_IN_PREIODIC_DATA_LIST ))
            {
                // set the data to transmit
                PeriodicMessageContents[0] = rxpayload->b[1];
                PeriodicMessageContents[1] = rxpayload->b[2];
                PeriodicMessageContents[2] = rxpayload->b[3];
                PeriodicMessageContents[3] = rxpayload->b[4];
            }
            else
            {
              // parameter out of range
              *txlen=0x1;
              txpayload->b[0] = CAN_ERROR_PARAMETER_OUT_OF_RANGE;
              return(0);
            }
        }break;
        
        case ICUBPROTO_POLLINGCMD_SET_I2T_PARAMS:
        {
            if(5 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            //extract params from the 1st word
             I2Tdata.Param = (rxpayload->b[2] <<8 | rxpayload->b[1]);
             I2Tdata.IThreshold = (rxpayload->b[4] <<8 | rxpayload->b[3]);
        }break;
        
        case ICUBPROTO_POLLINGCMD_GET_I2T_PARAMS:
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }
            *txlen = 5;
            txpayload->b[0] = cmd;
            memcpy(&txpayload->b[1], &I2Tdata.Param, 2); 
            memcpy(&txpayload->b[3], &I2Tdata.IThreshold, 2);
        }break;
        
        default:
        {
            ICUBPROTO_PAYLOAD_ERROR_SET(txpayload, txlen, CAN_PROTO_ERROR_UNKNOWN_COMMAND);
            return(0);
        }
    } //end switch
    return(1);
}

static int s_canIcubProtoParser_parse_periodicMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    ICUBPROTO_PAYLOAD_ERROR_SET(txpayload, txlen, CAN_PROTO_ERROR_UNKNOWN_COMMAND);
    return(0);    
}

static int s_canIcubProtoParser_parse_canLoaderMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    unsigned char cmd;

    cmd = rxpayload->b[0];
    *txlen = 0;

    switch (cmd)
    {
        case CMD_BROADCAST: 
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                *txlen = 0x1;
                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                return(0);
            }

            *txlen = 5;
            txpayload->b[0] = cmd;
            txpayload->b[1] = BOARD_TYPE_2FOC; 
            txpayload->b[2] = 7;  //TODO  //Firmware version number for BOOTLOADER c
            txpayload->b[3] = 7;  //TODO   //Firmware build number.
            txpayload->b[4] = 7;  //TODO   //Firmware build number.
            #warning solita incoerenza tra versioni di fw sensori ed motori            
        } break;
                
    
        case CMD_BOARD:
        {
            asm ("reset");    // Jump to bootloader code      
        } break;
    
        case CMD_GET_ADDITIONAL_INFO:
        {
            ;
        } break;
    
        case CMD_SET_ADDITIONAL_INFO:
        {
            ;
        } break;
        
                
        default:
        {
            ICUBPROTO_PAYLOAD_ERROR_SET(txpayload, txlen, CAN_PROTO_ERROR_UNKNOWN_COMMAND);
            return(0);
        } break;
        
        
    } //end switch

    return(1);    

}
