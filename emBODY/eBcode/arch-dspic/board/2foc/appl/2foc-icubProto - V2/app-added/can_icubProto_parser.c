
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
    SysStatus.b[0] &= 0b01100000;
}

/*the default control mode is open loop*/
__inline__ static void s_controlMode_setDefault(void) 
{
    s_controlMode_reset();
    SysStatus.OpenLoop = 1;
}

/*
__inline__ static void s_controlMode_setCurrent(void)
{ 
    SysStatus.b[0] |= (1 << 6);
}


__inline__ static void s_controlMode_setVelocity(void)
{
    SysStatus.b[0] |= (1 << 5);
}
*/
 

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
  int command_ok=1;
  unsigned char txlen = 0;
  unsigned char msg_class = 255;
  unsigned char source, permsg_type;
  tCanData txpayload;
  unsigned long resp_id=0;
  
   msg_class = CAN_ICUBPROTO_STDID_GETCLASS(rxid);
   source = CAN_ICUBPROTO_STDID_RX_GETSOURCE(rxid);
   permsg_type = CAN_ICUBPROTO_STDID_RX_GETPERMSGTYPE(rxid);
  
    switch(msg_class)
    {
        case ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL:
        {
            command_ok = s_canIcubProtoParser_parse_pollingMsg(rxpayload, rxlen, &txpayload, &txlen);
        }break;
        
        case ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL:
        {
            command_ok = s_canIcubProtoParser_parse_periodicMsg(permsg_type, rxpayload, rxlen, &txpayload, &txlen);
        }break;
        
        case ICUBCANPROTO_CLASS_BOOTLOADER:
        {
            command_ok = s_canIcubProtoParser_parse_canLoaderMsg(rxpayload, rxlen, &txpayload, &txlen);
        }break;
        
        default:
        {
//            txlen = 0x1;
//	        txpayload.b[0] =CAN_PROTO_ERROR_UNKNOWN_COMMAND; 
            txlen = 0x0;
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
     ; //error. What i do??? // CanIcubProtoTxErrorCode(resp_id, txlen, &txpayload);
    }
}
//returns 1 ==> ok, retuns 0 ==> nok
static int s_canIcubProtoParser_parse_pollingMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    unsigned char cmd = rxpayload->b[0];
    
    *txlen = 0x0; 
    switch(cmd)
    {
        case ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN: // DS402 Operation Enable 
        {
            if(1 != rxlen)
            { // incorrect number of parameters
                //*txlen=0x1;
                //txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen=0;
                return 0;
            }
         
            // Command can be accepted only if current ststus is SWITCHED ON 
            if(1 != DS402_Statusword.Flags.SwitchedOn )
            {
                //*txlen = 0x1;
                //txpayload->b[0] = CAN_DS402_INVALID_STATUS_CHANGE;
                *txlen=0;
                return 0;
            }

            if (!sCanProtocolCompatible)
            {
                *txlen=0;
                return 0;
            }
            
            //when start PWM ==> set the default control mode
            s_controlMode_setDefault();

            // go to Operation Enable state
            DS402_Controlword.Flags.EnableOperation = 1;       
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD: // DS402 Operation Disable
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                //*txlen = 0x1;
                //txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0;
                return(0);
            }
         
            FaultReset();

            // Command can be accepted only if current ststus is OPERATION ENABLE
            if ( (1 != DS402_Statusword.Flags.OperationEnabled ) && (1 != DS402_Statusword.Flags.SwitchedOn ) )
            {
                //*txlen = 0x1;
                //txpayload->b[0] = CAN_DS402_INVALID_STATUS_CHANGE;
                *txlen = 0;
                return(0);
            }
			      // go to Ready to Switch On state
            DS402_Controlword.Flags.EnableVoltage = 0;   
            // In ICUB the states DISABLE_OPERATION and SHUTDOWN goes in the same state. 
            // go to Switched On state
            //DS402_Controlword.Flags.EnableOperation = 0;
        }break;
    
        case ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD: // DS402 Switch On 
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
                //*txlen = 0x1;
                //txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen=0;
                return 0;
            }
            
            // Command can be accepted only if current ststus is SWITCH ON DISABLED 
            if(1 != DS402_Statusword.Flags.ReadyToSwitchOn )
            {
                //*txlen = 0x1;
                //txpayload->b[0] = CAN_DS402_INVALID_STATUS_CHANGE;
                *txlen=0;
                return 0;
            }

            if (!sCanProtocolCompatible)
            {
                *txlen=0;
                return 0;
            }
		
		    received_canloader_msg=0; // start to transmit status messages

            // go to Switch On state
            DS402_Controlword.Flags.SwitchOn = 1;
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE: // DS402 Shutdown
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            // Command can be accepted only if current ststus is OPERATION ENABLE or SWITCHED ON 
            if ( (1 != DS402_Statusword.Flags.OperationEnabled ) && (1 != DS402_Statusword.Flags.SwitchedOn ) )
            {
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_DS402_INVALID_STATUS_CHANGE;
                *txlen = 0x0;
                return(0);
            }

            // go to Ready to Switch On state
            //DS402_Controlword.Flags.EnableVoltage = 0;
            DS402_Controlword.Flags.EnableOperation = 0;   
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE:
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            
            txpayload->b[1] = CanIcubProtoGetcontrol_mode();
            if( icubCanProto_controlmode_unknownError == txpayload->b[1])
            {
//                *txlen = 0x1;
//                txpayload->b[0] =  CAN_ERROR_INCONSISTENT_STATE;
                *txlen = 0x0;
                return(0);
            }

            txpayload->b[0] = cmd;
            *txlen = 0x2;

        }break;

        case ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE:
        {
            if(2 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }

            //this command has effect only when i'm in enable operation state
            if(1 != DS402_Controlword.Flags.EnableOperation)
            {
//                *txlen = 0x1;
//                txpayload->b[1] = CAN_ERROR_COMMAND_IN_INVALID_STATE;
                *txlen = 0x0;
                return(0);    
            }

            //let change control mode on fly
            s_controlMode_reset();

            switch(rxpayload->b[1])
            {          
                case icubCanProto_controlmode_openloop:
                {
                    SysStatus.TorqueControl = 0;
                    SysStatus.OpenLoop = 1;
                }break;	 
                case icubCanProto_controlmode_current:
                {
                    SysStatus.OpenLoop = 0;
                    SysStatus.TorqueControl = 1;
                }break;
                case icubCanProto_controlmode_velocity:
                {
                    SysStatus.SpeedControl = 1;
                }break;
                case icubCanProto_controlmode_idle:
                {
                    DS402_Controlword.Flags.EnableOperation = 0; // go to Switched On state
                }break;
                case icubCanProto_controlmode_position:
		            {
                    SysStatus.PositionControl = 1;
		            }break;
                case icubCanProto_controlmode_torque:
                {
                    SysStatus.TorqueSensorLoop = 1;
                }break;
                default:
                {
                    s_controlMode_setDefault();
                }break;
            }

        }break;

        case ICUBCANPROTO_POL_MC_CMD__WRITE_FLASH_MEM:
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
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
        
        case ICUBCANPROTO_POL_MC_CMD__GET_ADDITIONAL_INFO:
        {
            ;   //todo
        }break;

        case ICUBCANPROTO_POL_MC_CMD__SET_ADDITIONAL_INFO:
        {
            ;   //todo
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_VELOCITY: 
        {
            ;  //todo 
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_VELOCITY: 
        {
            ;  //todo 
        }break;

        case ICUBCANPROTO_POL_MC_CMD__SET_ENCODER_POSITION: 
        {
            
		// setta l'offset di fasatura 
		Encoder_SyncPulsePosition=(rxpayload->b[2] << 8 | rxpayload->b[1]);  //todo aspetta maggia
        }break;
        
        
        case ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID: 
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            *txlen = 0x2;
            txpayload->b[0] = cmd;
            txpayload->b[1] = canprotoparser_bid;
        }break;

        case ICUBCANPROTO_POL_MC_CMD__SET_BOARD_ID: 
        {
//            *txlen=0x1;
//             txpayload->b[0] = CAN_PROTO_ERROR_COMMAND_NOT_INPLEMENTED;
              *txlen=0x0;
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
        
        case ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY: 
        {
            ; //aspetta maggia
			//
        }break;

        case ICUBCANPROTO_POL_MC_CMD__GET_MAX_VELOCITY: 
        {
            ; //aspetta maggia
        }break;

        case ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT: 
        {
			      if(5 != rxlen)
            {   // incorrect number of parameters
                // *txlen = 0x1;
                // txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
		
	          ////il dato che arriva è espresso in mA va trasformato in IAD della 2FOC (1A 1310)
            //il dato che arriva è espresso in mA va trasformato in IAD della 2FOC (1A=2000)
	          ApplicationData.CurLimit=(rxpayload->b[2] << 8 | rxpayload->b[1])*2;
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION: 
        {
            uint8_t server_can_protocol_major = rxpayload->b[1]; 
            uint8_t server_can_protocol_minor = rxpayload->b[2]; 
            /*uint8_t can_protocol_ack*/ 
            sCanProtocolCompatible = (CAN_PROTOCOL_VERSION_MAJOR == server_can_protocol_major && 
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

        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID: 
        {
            SFRAC16 pp, pi, pd, pm; //pm non viene passato dal comando 

            if(7 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            //ControllerGetCurrentDPIDParm(&pp,&pi,&pd,&pm);
            pp = (rxpayload->b[2] << 8 | rxpayload->b[1]);
            pi = (rxpayload->b[4] << 8 | rxpayload->b[3]);
            pd = 0;
            pm = (rxpayload->b[6] << 8 | rxpayload->b[5]);
      

            ControllerSetCurrentDPIDParm(pp,pi,pd,pm);
	        ControllerSetCurrentQPIDParm(pp,pi,pd,pm);
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__GET_CURRENT_PID: 
        {
            signed int p, i, d, m; 
            if(1 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            
            ControllerGetCurrentDPIDParm(&p, &i, &d, &m);
            txpayload->b[0] = cmd;
            memcpy(&txpayload->b[1], &p, 2);
            memcpy(&txpayload->b[3], &i, 2);
            memcpy(&txpayload->b[5], &d, 2);
            *txlen = 0x7;   
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID: 
        {
            SFRAC16 pp, pi, pd, pm; //pm non viene passato dal comando 
            
            if(7 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            ControllerGetWPIDParm(&pp,&pi,&pd,&pm);
            pp = (rxpayload->b[2] << 8 | rxpayload->b[1]);
            pi = (rxpayload->b[4] << 8 | rxpayload->b[3]);
            pd = (rxpayload->b[6] << 8 | rxpayload->b[5]); 
            ControllerSetWPIDParm(pp,pi,pd,pm);
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__GET_VELOCITY_PID: 
        {
            signed int p, i, d, m;

            if(1 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            ControllerGetWPIDParm(&p, &i, &d, &m);
            txpayload->b[0] = cmd;
            memcpy(&txpayload->b[1], &p, 2);
            memcpy(&txpayload->b[3], &i, 2);
            memcpy(&txpayload->b[5], &d, 2);
            *txlen = 0x7;
        }break;
                
        case ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT: 
        {
	        SFRAC16 IqRef;
            if(5 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            
#ifndef CAN_CMD_ALWAYS_ACCEPT
            if ( (1 != DS402_Statusword.Flags.OperationEnabled ) && (1 != DS402_Statusword.Flags.SwitchedOn ) )
            {
//              *txlen=0x1;
//              txpayload->b[0] =  CAN_ERROR_COMMAND_IN_INVALID_STATE;
              *txlen = 0x0;
              return(0);
            }
#endif
           // Torque control references
	         IqRef = (rxpayload->b[2] << 8 | rxpayload->b[1]);
	         if (IqRef>ApplicationData.CurLimit)
               IqRef=ApplicationData.CurLimit;
           else if (IqRef<-ApplicationData.CurLimit)
               IqRef=-ApplicationData.CurLimit;
	         CtrlReferences.qIqRef=IqRef;
           // set reference value for toggling torque
           TorqueTogglingReference = CtrlReferences.qIqRef;

           // Flux control references
           CtrlReferences.qIdRef = (rxpayload->b[4] << 8 | rxpayload->b[3]);  

#ifdef SYNC_2FOC_TO_EMS
            CanIcubProtoTrasmitterSendPeriodicData();          
#endif    
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_CURRENT: 
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            
            *txlen=0x5;
            txpayload->b[0] = cmd;
            memcpy(&txpayload->b[1], &CtrlReferences.qIqRef, 2);
            memcpy(&txpayload->b[3], &CtrlReferences.qIdRef, 2);    
        }break;

        case ICUBCANPROTO_POL_MC_CMD__SET_PERIODIC_MSG_CONTENTS: 
        {
            if(5 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
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

                // precalculate this here. This is an optimization
                gulpadr1 = (unsigned int*)PeriodicData[PeriodicMessageContents[0]];
                gulpadr2 = (unsigned int*)PeriodicData[PeriodicMessageContents[1]];
                gulpadr3 = (unsigned int*)PeriodicData[PeriodicMessageContents[2]];
                gulpadr4 = (unsigned int*)PeriodicData[PeriodicMessageContents[3]];
            }
            else
            {
              // parameter out of range
//              *txlen=0x1;
//              txpayload->b[0] = CAN_ERROR_PARAMETER_OUT_OF_RANGE;
                *txlen = 0x0;
              return(0);
            }
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS:
        {
            if(5 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            //extract params from the 1st word
             I2Tdata.Param = (rxpayload->b[2] <<8 | rxpayload->b[1]);
             I2Tdata.IThreshold = (rxpayload->b[4] <<8 | rxpayload->b[3]);
        }break;
        
        case ICUBCANPROTO_POL_MC_CMD__GET_I2T_PARAMS:
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
            *txlen = 5;
            txpayload->b[0] = cmd;
            memcpy(&txpayload->b[1], &I2Tdata.Param, 2); 
            memcpy(&txpayload->b[3], &I2Tdata.IThreshold, 2);
        }break;
        
        default:
        {
            //ICUBPROTO_PAYLOAD_ERROR_SET(txpayload, txlen, CAN_PROTO_ERROR_UNKNOWN_COMMAND);
            *txlen = 0;
            return(0);
        }
    } //end switch
    return(1);
}

static int s_canIcubProtoParser_parse_periodicMsg(unsigned char permsg_type, tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
#ifndef DESIRED_CURR_IN_PER_MSG_BY_EMS
    //ICUBPROTO_PAYLOAD_ERROR_SET(txpayload, txlen, CAN_PROTO_ERROR_UNKNOWN_COMMAND);
    *txlen = 0;
    return(0);
#else
    switch(permsg_type)
    {
        case ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT:
        {
            if((canprotoparser_bid*2) > rxlen)
            {
                //ICUBPROTO_PAYLOAD_ERROR_SET(txpayload, txlen, CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS);
                *txlen = 0;
                return(0);
            }
             
            // Torque control references
            CtrlReferences.qIdRef = 0;
            CtrlReferences.qIqRef = -rxpayload->w[canprotoparser_bid-1];

            /*
            if (CtrlReferences.qIqRef>ApplicationData.CurLimit)
               CtrlReferences.qIqRef= ApplicationData.CurLimit;
            else if (CtrlReferences.qIqRef<-ApplicationData.CurLimit)
               CtrlReferences.qIqRef=-ApplicationData.CurLimit;
            */
            
            // set reference value for toggling torque
            TorqueTogglingReference = CtrlReferences.qIqRef;
#ifdef SYNC_2FOC_TO_EMS
            //if (!SysError.OverCurrentFailure)
            {
              CanIcubProtoTrasmitterSendPeriodicData();
            }          
#endif   
			*txlen = 0;
			return(1);
        }break;
       
        default:
        {
            //ICUBPROTO_PAYLOAD_ERROR_SET(txpayload, txlen, CAN_PROTO_ERROR_UNKNOWN_COMMAND);
            *txlen = 0;
            return(0);
        }

    }

#endif    
}

static int s_canIcubProtoParser_parse_canLoaderMsg(tCanData *rxpayload, unsigned char rxlen, tCanData *txpayload, unsigned char *txlen)
{
    unsigned char cmd;

    cmd = rxpayload->b[0];
    *txlen = 0;
	
	
	switch (cmd)
    {
        case ICUBCANPROTO_BL_BROADCAST: 
        {
            if(1 != rxlen)
            {   // incorrect number of parameters
//                *txlen = 0x1;
//                txpayload->b[0] = CAN_ERROR_INCORRECT_NUMBER_OF_PARAMETERS;
                *txlen = 0x0;
                return(0);
            }
			
			received_canloader_msg=1; // do not transmit status messages

            *txlen = 5;
            txpayload->b[0] = cmd;
            txpayload->b[1] = icubCanProto_boardType__2foc;
            txpayload->b[2] = FW_VERSION_MAJOR;
            txpayload->b[3] = FW_VERSION_MINOR;
            txpayload->b[4] = FW_VERSION_BUILD;            
        } break;
                
    
        case ICUBCANPROTO_BL_BOARD:
        {
            asm ("reset");    // Jump to bootloader code      
        } break;
    
        case ICUBCANPROTO_BL_GET_ADDITIONAL_INFO:
        {
            ;
        } break;
    
        case ICUBCANPROTO_BL_SET_ADDITIONAL_INFO:
        {
            ;
        } break;
        
                
        default:
        {
            //ICUBPROTO_PAYLOAD_ERROR_SET(txpayload, txlen, CAN_PROTO_ERROR_UNKNOWN_COMMAND);
            *txlen = 0;
            return(0);
        } break;
        
        
    } //end switch

    return(1);    

}
