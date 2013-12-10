/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Maggiali, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.maggiali@iit.it, valentina.gaggero@iit.it
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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*	@file 	main.c
	@brief 	this file defines mais application.
	@author claudio.lorini@iit.it, marco.maggiali@iit.it, valentina.gaggero@iit.it
	@date	- 
*/
//  A.d.C. M A I S
//  Author C.Lorini and M.Maggiali
//  Rev. 0.0 del 25/09/2007
//
//  Tested on dsPIC30F4013
//  pic30-gcc V4.03
//  MPLAB IDE ver 8.0
// 
//  Revisiom List
//  Revision 0.0: 
// TODO: Buffer SW trasmissione messaggi can (se la linea e`piena si rischia l'overrun)
// TODO: verifica del rumore con 1 segnale forte sul MUX e segnali piccoli
// TODO: verifica della mascheratura dei comandi (risponde a 0x205, 0x605, ma non a 0x105)
// TODO: verifica del comportamento a comandi non gestiti (nel case manca il DEFAULT!)

//  Revision 1.0: (Marco Randazzo)
// TODO:

//  Revision 1.1.2: Marco Maggiali 20/04/09
//
//  The main has been divided in many files
//  The CAN TX is done with interrupt
//
//
//  Revision 1.1.3: Marco Maggiali 30/04/09
//
//  In T2 (from 1ms to 10-20ms) interrupt the CAN messages are prepared and sent
//  T1 (1ms) is used for the for(;;) where is done the acquisition of the AD signals
//  
//
//  Revision 1.1.4: Marco Maggiali 15/05/09
//
//  The yellow led is now switched off, because of power consumption issue 
//  The red led is switched on.
//  maggia: default address 0xf  
//  periodic message id 0x300
//
//  Revision 1.1.5: Marco Maggiali 09/06/09
//
//  The Firmware does not stop to do the broadcast if a connection from the CANLOADER is done. DisableT1,T2 and T3 are in CMD_BOARD only. 
//
//
//  Revision 1.1.6: Marco Maggiali 10/06/09
//
//  The 8bits data messages are organized as following: data 0to 6 in the first message, from 7 to 14 in the second one.
//
//  Revision 2.0: Marco Maggiali 28/01/10
//
//  After SVN installation there was a mass in the code and there is no way to go back
//
//  Revision 2.1: Marco Maggiali 08/09/10
//
//  There is a strange behaviour of the can filters reported in ERRATA of the dspic30f4013     
//

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <string.h>
#include <libpic30.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"

#include "hal_can_interface.h"
#include "hal_errors.h"
#include "hal_utils.h"
#include "hal_timers.h"
#include "hal_mais_internalADC.h"
#include "hal_mais_ports.h"
#include "hal_mais_multiplexer.h"

#include "mais_config.h"
#include "mais_data_types.h"

#include "iCubCanProtocol.h"
#include "iCubCanProto_types.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//  empty section

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


//// CONFIGURATION BITS INITIALIZATION 
// ATTENTION!!! this code must be inserted before the beginning of your C source code

// init configuration bits(p30f4013.h)
_FOSC(CSW_FSCM_OFF & ECIO_PLL8); 
  // Clock switching disabled Fail safe Clock Monitor disabled
  // External clock with PLL x8 (10MHz*8->Fcycle=80/4=20MIPS)
_FGS(CODE_PROT_OFF); // Code protection disabled

_FWDT(WDT_OFF);     // WD disabled

//_FBORPOR(MCLR_EN & PWRT_64 & PBOR_ON & BORV_27);  // BOR 2.7V POR 64msec
_FBORPOR(MCLR_EN & PWRT_64 & PBOR_ON & BORV27);  // BOR 2.7V POR 64msec



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
extern version_srcCode_info_t *mais_srcCode_info_ptr; // info of binary compiled with these source code files


//leds
uint8_t  toggledY=0;

uint8_t  start_read_analogChan = 0;
uint8_t  can_trasmission_enable = 0;

mais_analog_ch_data_t AN_channel_info;

mais_config_data_t mais_cfg; // mais application configuration

uint8_t canProtocol_compatibility_ack = 0; //actually it is use to send compatibility information
                                           //in the future only if it values 1 mais application runs in normal mode
                                           //in case it values 0 mais application runs in safe mode.


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_parse_can_pollingMsg(hal_canmsg_t *msg, uint8_t *Txdata, int8_t *datalen);
static void s_parse_can_loaderMsg(hal_canmsg_t *msg, uint8_t *Txdata, int8_t *datalen);
static void s_parse_can_msg(void);
static void s_idle(void);
static void s_test(void);
static void s_timer1_callback(void);
static void s_timer2_callback(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
 
int main(void)
{ 
	uint16_t i;
	int16_t ch = 0; //the channel of the mux
	uint16_t Chn = 0;
	uint8_t flag=0;
    uint8_t res;
	hal_error_reset_t error;
	hal_timer_config_datastruct_t timer_config_data;
	
	
	error = hal_error_resetCause_get(); 
	hal_error_manage(error);
  
	// turn IO ports
	hal_mais_ports_init();
	
	//init internal AD12 and mutex in order to sample
	hal_mais_internalADC12_init();

	hal_mais_mux_init();
 	
	//enable watchdog
	hal_watchdog_enable();

	mais_config_init(&mais_cfg); //init main configuration with default values

	res = mais_config_readFromEE(&mais_cfg); // EEPROM Data Recovery
    //if eeprom has been resetted, write defalut CAN address in eeprom.
    if(0 == res)
    {
        mais_config_saveInEE(&mais_cfg);   
    }
	// todo: checksum verification



	/* all timers used in this application:
	- don't used gate operation 
	- stop during cpu idle status
	- using inernal source
	- prescaler 64
	How to choose match value:
	fosc (frequency oscillator) = 10MHz
	PLL = 8;
	For do one insruction,it needs 4 clocks.
	we choose prescaler 64, because it gives us good values. It may be changed.

	freq = (fosc*PLL)/4 = 20MHz
	
	freq/prescaler = 5/16*10^6 Hz ==> DeltaT = 16/5*10^-6 sec ~= 3,2*10^-6 sec
	
	So, you need do choos a correct match value, in order to: 
	match_value * 3,2*10^-6 sec = desired clock period.	

	3906 -> 0,0125 sec 
	1953 -> 0,00625 sec
	312 -> 1ms
	156 -> 500us
	31 -> 100ns  
	*/
	timer_config_data.gate_op = 0;
	timer_config_data.idle = 0;
	timer_config_data.source = TIMER_SOURCE_INTERNAL;
	timer_config_data.prescaler = hal_timer_prescaler64; 

	//timer 1 configuration: it establishes when sampling and parse received can cmd
	timer_config_data.match_value = 312;
    timer_config_data.call_bkp_fn = s_timer1_callback;
	hal_timer_config(hal_timerT1, &timer_config_data);

	hal_timer_interrupt_config(hal_timerT1, hal_timer_IntPrio7); //highest priority
	hal_timer_start(hal_timerT1);

	// timer 2: it establishes when sends adc values on can.
	// its interrupt is disabled; it is enabled only by can command message.
	if(mais_cfg.ee_data.CAN_MessageDataRate >= 209)
	{
		timer_config_data.match_value = 0xFFFF;
	}
	else
	{
		timer_config_data.match_value = (312.5 * mais_cfg.ee_data.CAN_MessageDataRate);
	}
	timer_config_data.call_bkp_fn = s_timer2_callback;
	hal_timer_config(hal_timerT2, &timer_config_data);
	
	hal_timer_interrupt_config(hal_timerT2, hal_timer_IntPrio2);
	
	hal_timer_start(hal_timerT2);
	
	
	
	// blink leds
  
	for(i=1;i<=20;i++)
	{  
		ledY  = ~ledY; //toggLED  
		ledR  = ~ledR; //toggLED
		hal_delay(800000);
	}

	//can initialise
	hal_can_init(hal_can_portCAN1);
	// Cofigure CAN filter
	hal_can_boardFilter_set(hal_can_portCAN1, mais_cfg.ee_data.CAN_BoardAddress);


	// enable interrupt Timer1	
	hal_timer_interrupt_ena(hal_timerT1);

	ledR_ON; //Set led red on
	for(;;)
	{ 
		
		while(start_read_analogChan) 
		{
			start_read_analogChan = 0; //reset of the T1 timer flag
			
			s_parse_can_msg(); 
			
			s_idle();
			if (toggledY>=((3-mais_cfg.hesdata_resolution)<<6))
			{
				toggledY=0;
			}
			
			// reading of the 15 HES signals	    
			for(i=0;i<ANALOG_CHANEL_NUM;i++)
			{
				// select analog channel 
				if(Chn>=11)
				{ 
					ch = 11;
					hal_mais_mux_channelSelect(Chn);
				}
				else
				{
					ch = Chn;
				}
				
				hal_mais_internalADC12_channelSet(ch); 
				
				hal_mais_internalADC12_prepare();
				hal_delay(50);       //<--- Don't remove: application without it crash.
				
				
				hal_mais_internalADC12_startSampling(); 
				hal_delay(34);      // minimum time for sample/hold
				
				hal_mais_internalADC12_startConvert(); 
				while( !hal_mais_internalADC12_IsConvertionDone() );
				
				AN_channel_info.values[Chn] =  hal_mais_internalADC12_read(0); 
				
				Chn++;
				if(Chn==ANALOG_CHANEL_NUM)
				{
					Chn=0;
					flag=0;
				}  
			}// end for reading 15 channels
		}// end while
	}
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//
// This function clear the wachtdog
// put hear things that can be done during idle time
//void s_idle(void)
//{
//static uint8_t count = 0;
//uint8_t SID;
//uint8_t data1[8] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
//uint8_t data2[8] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
//
//SID = (CAN_MSG_CLASS_PERIODIC) | ((mais_cfg.ee_data.CAN_BoardAddress)<<4) | (CAN_CMD_HES0TO6) ;
//
//hal_watchdog_reset(); // Note: actually watchdog is disabled
//count++;
//data1[count] = count;
//if(count>2)
//{
//	data1[7] = count;
//	hal_can_put_immediately(hal_can_portCAN1,SID, data1, 8, 0 );
//	while(1)
//	{
//		hal_delay(8000000);
//		hal_can_put_immediately(hal_can_portCAN1,SID, data2, 8, 1);
//	}
//}
//
//}

void s_idle(void)
{
	hal_watchdog_reset();
}

//can msg parser 
static void s_parse_can_msg(void)
{
	hal_canmsg_t *msg;
	hal_canmsg_t CAN_Msg;
	uint8_t Txdata[9]; 
	int8_t datalen;
	uint16_t SID;


	if (hal_can_buff_rx_isEmpty(hal_can_portCAN1))
	{
		return;
	}
	// Something in the CAN message buffer
	// Read received data from receive buffer and store it into user defined dataarray
		
	hal_can_get(hal_can_portCAN1, &CAN_Msg);
	msg=&CAN_Msg;

	switch (msg->CAN_Per_Msg_Class)
	{
		case (ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR):
		{
  			s_parse_can_pollingMsg(msg, Txdata, &datalen);
			break;		
		}
		
		case (ICUBCANPROTO_CLASS_BOOTLOADER):
		{
			s_parse_can_loaderMsg(msg, Txdata, &datalen);
			break;
		}
		
		default:
		{
			// UNKNOWN COMMAND FOR THIS CLASS
			hal_error_canMsg_set(Txdata, &datalen, msg->CAN_Per_Msg_PayLoad[0]);
			break;
		}
		
	  }
	
	  // Load message ID , Data into transmit buffer and set transmit request bit
	  // swap source and destination
	  SID = (msg->CAN_Per_Msg_Class << 8 ) | ( mais_cfg.ee_data.CAN_BoardAddress << 4 ) | ( msg->CAN_Poll_Msg_Source );
	
	  // Send ack messages with datalen > 0
	  // Skip ack messages with datalen < 0
	  // Send ack messages with datalen = 0 and CAN_ACK_EVERY_MESSAGE = 1
	  if ( (datalen > 0) || (datalen == 0 && mais_cfg.can_ack_every_msg == 1) )
	  { 
	    hal_can_put(hal_can_portCAN1, mais_cfg.ee_data.CAN_BoardAddress, SID,hal_can_frameType_data,datalen,Txdata); 
	  }
}


static void s_parse_can_pollingMsg(hal_canmsg_t *msg, uint8_t *Txdata, int8_t *datalen)
{
	uint16_t match_value;

	
	switch (msg->CAN_Per_Msg_PayLoad[0])
	{
		case ICUBCANPROTO_POL_AS_CMD__SET_BOARD_ADX: // set board CAN address 
		{     
			*datalen=0;
			if ( ( msg->CAN_Per_Msg_PayLoad[1] > 0 ) && ( msg->CAN_Per_Msg_PayLoad[1] <= 15 ))
			{
				mais_cfg.ee_data.CAN_BoardAddress = msg->CAN_Per_Msg_PayLoad[1];
				hal_can_boardFilter_set(hal_can_portCAN1, mais_cfg.ee_data.CAN_BoardAddress);   
				if (mais_cfg.save_eeprom_atonce)
				{
					mais_config_address_set(&mais_cfg);
				}
			}
			else 
			{	*datalen=-1;
				hal_error_manage(ERR_CAN_PARAMETERS_INVALID);
				return; 
			} 
			
			break;
		}  
		    

		case ICUBCANPROTO_POL_AS_CMD__GET_CH_ADC:  //get ADC channel
		{
			if(msg->CAN_Per_Msg_PayLoad[1] < ANALOG_CHANEL_NUM)
			{
				Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_CH_ADC; 
				Txdata[1] = msg->CAN_Per_Msg_PayLoad[1];  
				Txdata[2] = AN_channel_info.values[msg->CAN_Per_Msg_PayLoad[1]] >> 8; 
				Txdata[3] = AN_channel_info.values[msg->CAN_Per_Msg_PayLoad[1]] & 0xFF; 
				*datalen=4;	            
			}

			break;
		}  
	
		
		case ICUBCANPROTO_POL_AS_CMD__SET_TXMODE: // set continuous or on demand tx  0x205 len 2  data 7 0/1
		{
			if(msg->CAN_Per_Msg_PayLoad[1]==0)
			{
                can_trasmission_enable = 1;
                hal_timer_interrupt_ena(hal_timerT2);
				hal_timer_interrupt_ena(hal_timerT1);                
			}
			else if (msg->CAN_Per_Msg_PayLoad[1]==1)
			{
                can_trasmission_enable = 0;
				hal_timer_interrupt_disa(hal_timerT2);
			}
			else 
			{
                can_trasmission_enable = 0;
				hal_timer_interrupt_disa(hal_timerT1);
				hal_timer_interrupt_disa(hal_timerT2);
			}

			*datalen=0;

			break;
		}  
	
		    
		case ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE: // set datarate for transmission in milliseconds 
			  // 0x205 len 2  data 8 n
		{
			mais_cfg.ee_data.CAN_MessageDataRate=msg->CAN_Per_Msg_PayLoad[1];
			
			hal_timer_interrupt_disa(hal_timerT2);
			hal_timer_stop(hal_timerT2);
			
			// saturate match_value to 0xFFFF in case of delays larger than 209
			if (mais_cfg.ee_data.CAN_MessageDataRate >= 209)
			{
				match_value = 0xFFFF;
			}
			else
			{
				match_value = ( 312.5 * mais_cfg.ee_data.CAN_MessageDataRate);
			}
			
			hal_timer_matchValue_set(hal_timerT2, hal_timer_prescaler64, match_value);
			
			hal_timer_start(hal_timerT2);
			hal_timer_interrupt_ena(hal_timerT2);
			*datalen=0;

			break;
		}  
		    
		case ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION: // set data resolution   
				  					// 0x205 len 2  data 0x10 n
		{
		
			if (hesdata_16bit == msg->CAN_Per_Msg_PayLoad[1]  )
			{
				mais_cfg.hesdata_resolution = hesdata_16bit;
			}
			if (hesdata_12bit == msg->CAN_Per_Msg_PayLoad[1]) //test
			{
				mais_cfg.hesdata_resolution = hesdata_12bit;
			}
			if (hesdata_8bit == msg->CAN_Per_Msg_PayLoad[1])
			{
				mais_cfg.hesdata_resolution=hesdata_8bit;
			}

			*datalen=0;

			break;
		}  

		
		case ICUBCANPROTO_POL_AS_CMD__SAVE2EE: // Save configuration data to EE
								// 0x205 len 1  data 9 
		{
			 mais_config_saveInEE(&mais_cfg);
		
			// todo: checksum calcuation
			
			*datalen=0;
		
			break;
		}
		
		case ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION:
		{
			if( (mais_srcCode_info_ptr->canProtocol.version == msg->CAN_Per_Msg_PayLoad[1]) &&
			    (mais_srcCode_info_ptr->canProtocol.release == msg->CAN_Per_Msg_PayLoad[2]) )
				{
					canProtocol_compatibility_ack = 1;
				}
            else
                {
                    canProtocol_compatibility_ack = 0;
                }
	
			Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION;
			Txdata[1] = icubCanProto_boardType__mais;
			Txdata[2] = mais_srcCode_info_ptr->fw_ExeFile.version;
			Txdata[3] = mais_srcCode_info_ptr->fw_ExeFile.release;
			Txdata[4] = mais_srcCode_info_ptr->fw_ExeFile.build;
			Txdata[5] = mais_srcCode_info_ptr->canProtocol.version;
			Txdata[6] = mais_srcCode_info_ptr->canProtocol.release;			
			Txdata[7] = canProtocol_compatibility_ack;
			*datalen=8;           
		break;
		}

		case ICUBCANPROTO_POL_AS_CMD__SET_CURR_TARE:
		case ICUBCANPROTO_POL_AS_CMD__GET_CURR_TARE:
		case ICUBCANPROTO_POL_AS_CMD__SET_CALIB_TARE: 
		case ICUBCANPROTO_POL_AS_CMD__GET_CALIB_TARE:
		case ICUBCANPROTO_POL_AS_CMD__SET_SERIAL_NO:
		case ICUBCANPROTO_POL_AS_CMD__GET_SERIAL_NO:
		case ICUBCANPROTO_POL_AS_CMD__SET_FULL_SCALES:
		case ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES:
		case ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_G:	
		case ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_G: 
	    case ICUBCANPROTO_POL_AS_CMD__SET_CH_DAC:
		case ICUBCANPROTO_POL_AS_CMD__GET_CH_DAC:   
		case ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_RC:
	    case ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_RC: 
	    case ICUBCANPROTO_POL_AS_CMD__MUX_EN:
	    case ICUBCANPROTO_POL_AS_CMD__MUX_NUM:
		case ICUBCANPROTO_POL_AS_CMD__SET_IIR: 
		case ICUBCANPROTO_POL_AS_CMD__CALIBRATE_OFFSET:
	    case ICUBCANPROTO_POL_AS_CMD__SELECT_ACTIVE_CH:
	    case ICUBCANPROTO_POL_AS_CMD__FILTER_EN:
	    {
			  *datalen=-1;
		      hal_error_manage(ERR_CAN_COMMAND_UNAVAILABLE);
		      return;
		}  
		
		
		default:
		{
			// UNKNOWN COMMAND FOR THIS CLASS
			hal_error_canMsg_set(Txdata, datalen, msg->CAN_Per_Msg_PayLoad[0]);
			break;
		}
		
	}// end switch 

}

static void s_parse_can_loaderMsg(hal_canmsg_t *msg, uint8_t *Txdata, int8_t *datalen)
{
	uint16_t SID;
	int8_t j;
	uint8_t tmp;

	hal_timer_interrupt_disa(hal_timerT2); 
	
	switch (msg->CAN_Per_Msg_PayLoad[0])
	{
		case ICUBCANPROTO_BL_BROADCAST: 
		{
			//Create ID for CAN message
			SID = CAN_MSG_CLASS_LOADER | ( mais_cfg.ee_data.CAN_BoardAddress << 4 ) | (0);
			Txdata[0] = ICUBCANPROTO_BL_BROADCAST;
			Txdata[1] = icubCanProto_boardType__mais; 
			Txdata[2] = mais_srcCode_info_ptr->fw_ExeFile.version;            //Firmware version number for BOOTLOADER
			Txdata[3] = mais_srcCode_info_ptr->fw_ExeFile.release;              //Firmware build number.
			Txdata[4] = mais_srcCode_info_ptr->fw_ExeFile.build;              //Firmware build number.
			*datalen=5;
 
			break;
		}
	
		case ICUBCANPROTO_BL_BOARD:
		{
			//Jump to bootlader code
			asm ("reset");
			break;
		}
	
		case ICUBCANPROTO_BL_GET_ADDITIONAL_INFO:
		{
			SID = (ICUBCANPROTO_CLASS_BOOTLOADER<<8) | ( mais_cfg.ee_data.CAN_BoardAddress << 4 ) | (0);
			Txdata[0] = 0x0C; 
			Txdata[1] = 0x00; 
			*datalen=6;
			for (tmp = 0; tmp < 8; tmp++)
			{
				Txdata[1]=tmp;
				for (j=0; j<4; j++)	
				{
					Txdata[2+j] = mais_cfg.ee_data.AdditionalInfo[j+tmp*4];
				}
				hal_can_put(hal_can_portCAN1, mais_cfg.ee_data.CAN_BoardAddress, SID,hal_can_frameType_data, *datalen,Txdata);		
			}
			*datalen = -1;

			break;
		}
	
		case ICUBCANPROTO_BL_SET_ADDITIONAL_INFO:
		{
			static unsigned char addinfo_part=0;
 
		
			for (j=0; j<4; j++)
			{
				mais_cfg.ee_data.AdditionalInfo[msg->CAN_Per_Msg_PayLoad[1]*4+j] = msg->CAN_Per_Msg_PayLoad[2+j]; 
			}

			if (mais_cfg.save_eeprom_atonce)
			{
			
				if (addinfo_part==7)
				{
					addinfo_part=0;
					mais_config_additionalInfo_set(&mais_cfg);
				}
				else
				{					
					addinfo_part++;
				}
				*datalen = 0;
			}

			break;
		}
	
		case ICUBCANPROTO_BL_ADDRESS: 
		case ICUBCANPROTO_BL_DATA: 
		case ICUBCANPROTO_BL_START: 
		case ICUBCANPROTO_BL_END: 
		{
			// IGNORE THESE COMMANDS
			*datalen = -1;
			break;
		}
	
		default:
		{
			// UNKNOWN COMMAND FOR THIS CLASS
			hal_error_canMsg_set(Txdata, datalen, msg->CAN_Per_Msg_PayLoad[0]);
			break;
		}
		
	}//end switch

}


static void s_test(void)
{
	static uint8_t msg1[8] ={0xFF,0xFA,0xF5,0xF0,0xEB,0xE6,0xE1, 0};	
	static uint8_t msg2[8] ={0x82,0x7D,0x78,0x73,0x6E,0x69,0x64, 0};
							//{130, 125, 120, 115, 110, 105,100,0};
								//{0x82,0x7D,0x78,0x73,0x6E,0x69,0x64}
	uint8_t i;
	uint16_t SID;

	for(i=0; i<7; i++)
	{
		msg1[i] = ~msg1[i]; // square wave
		msg2[i] = ~msg2[i]; // square wave
	}
	
	msg1[7]++; //traingular
	msg2[7]++; //traingular
	if(255==msg1[7])
	{
		msg1[7]=0;
		msg2[7]=0;
	}
	
	SID = (CAN_MSG_CLASS_PERIODIC) | ((mais_cfg.ee_data.CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__HES0TO6) ;
	hal_can_put_immediately(hal_can_portCAN1,SID, msg1, 8, 0 );
			
	SID = (CAN_MSG_CLASS_PERIODIC) | ((mais_cfg.ee_data.CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__HES7TO14) ;		
	hal_can_put_immediately(hal_can_portCAN1, SID, msg2, 8, 1 );

}


static void s_timer1_callback(void)
{    
  start_read_analogChan = 1; 
}


static void s_timer2_callback(void)
{    
	uint16_t SID;
	uint8_t HESData1[8], HESData2[8], HESData3[8],HESData4[8];
	
    if(0 == can_trasmission_enable)
    {
        return;
    }

	switch (mais_cfg.hesdata_resolution)
	{
		case hesdata_8bit:
		{
			HESData1[0] = AN_channel_info.values[0] >>4;  
			HESData1[1] = AN_channel_info.values[1] >>4;  
			HESData1[2] = AN_channel_info.values[2] >>4;  
			HESData1[3] = AN_channel_info.values[3] >>4;  
			HESData1[4] = AN_channel_info.values[4] >>4;  
			HESData1[5] = AN_channel_info.values[5] >>4;  
			HESData1[6] = AN_channel_info.values[6] >>4;  
			
			
			HESData2[0] = AN_channel_info.values[7] >>4;  
			HESData2[1] = AN_channel_info.values[8 ] >>4;  
			HESData2[2] = AN_channel_info.values[9 ] >>4;  
			HESData2[3] = AN_channel_info.values[10] >>4;  
			HESData2[4] = AN_channel_info.values[11] >>4;  
			HESData2[5] = AN_channel_info.values[12] >>4;  
			HESData2[6] = AN_channel_info.values[13] >>4;  
			HESData2[7] = AN_channel_info.values[14] >>4;
		
			SID = (CAN_MSG_CLASS_PERIODIC) | ((mais_cfg.ee_data.CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__HES0TO6) ;
			hal_can_put_immediately(hal_can_portCAN1,SID, HESData1, 7, 0 );
			
			SID = (CAN_MSG_CLASS_PERIODIC) | ((mais_cfg.ee_data.CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__HES7TO14) ;
			
			hal_can_put_immediately(hal_can_portCAN1, SID, HESData2, 8, 1 );
		}
		break;
 	
		case hesdata_16bit:
		{
			memcpy(HESData1,&AN_channel_info.values[0],8);
			memcpy(HESData2,&AN_channel_info.values[4],8);
			
			memcpy(HESData3,&AN_channel_info.values[8],8);
			memcpy(HESData4,&AN_channel_info.values[12],6);
			
			SID = (CAN_MSG_CLASS_PERIODIC) | ((mais_cfg.ee_data.CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__HES0TO3) ;
			hal_can_put_immediately(hal_can_portCAN1, SID, HESData1, 8, 0 );	  
			
			SID = (CAN_MSG_CLASS_PERIODIC) | ((mais_cfg.ee_data.CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__HES4TO7) ;
			hal_can_put_immediately(hal_can_portCAN1, SID, HESData2, 8, 1 );
			
			SID = (CAN_MSG_CLASS_PERIODIC) | ((mais_cfg.ee_data.CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__HES8TO11) ;
			hal_can_put_immediately(hal_can_portCAN1, SID, HESData3, 8, 2 );	  
			
			SID = (CAN_MSG_CLASS_PERIODIC) | ((mais_cfg.ee_data.CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__HES12TO14) ;
			while( !(hal_can_txHwBuff_isEmpty(hal_can_portCAN1, 0)) );
			hal_can_put_immediately(hal_can_portCAN1, SID, HESData4, 8, 0 );
		
		}
		break;

		case hesdata_12bit://send square waves and triangular waves
		{
			s_test();
		}
		default:
		break;
 	
}
  toggledY++;   
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
