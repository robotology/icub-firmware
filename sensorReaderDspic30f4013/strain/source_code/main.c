/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Randazzo, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
/*	@file 	strain_main.c
	@brief 	This file implements strain application.
            IMPORTANT: actually strain application doesn't use IIR filter, so source code about IIR filter is commented
            and strain_IIR.c/.h files has been removed from project.
	@author claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/
//---old comments----
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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <string.h>
#include <libpic30.h>
#include <dsp.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"

#include "hal_can_interface.h"
#include "hal_utils.h"
#include "hal_errors.h"
#include "hal_eeprom.h"
#include "hal_timers.h"

#include "hal_strain_ports.h"
#include "hal_strain_multiplexer.h"
#include "hal_strain_ADC_DAC.h"

#include "strain_config.h"
//#include "strain_IIR_filter.h" //IIR filter not used

#include "iCubCanProtocol.h"
#include "iCubCanProto_types.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


//// CONFIGURATION BITS INITIALIZATION 
// ATTENTION!!! this code must be inserted before the beginning of your C source code

// inizializzazione bit di configurazione (p30f4013.h)
_FOSC(CSW_FSCM_OFF & ECIO_PLL8); 
  // Clock switching disabled Fail safe Clock Monitor disabled
  // External clock with PLL x8 (10MHz*8->Fcycle=80/4=20MIPS)
_FGS(CODE_PROT_OFF); // Code protection disabled

_FWDT(WDT_OFF);  // WD disabled in configuration bits, so it is possible enable wd by software

//_FBORPOR(MCLR_EN & PWRT_64 & PBOR_ON & BORV_27);  // BOR 2.7V POR 64msec
_FBORPOR(MCLR_EN & PWRT_64 & PBOR_ON & BORV20);  // BOR 2.7V POR 64msec @@@ now 2.0V



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
extern version_srcCode_info_t *strain_srcCode_info_ptr;

strain_config_data_t strain_cfg;


/*Al these global varibles are used to modify application behaviour and they can be modified by can messages*/
int16_t  CurrentTare[6] = {0,0,0,0,0,0};
int8_t UseCalibration = 1;
int8_t DebugCalibration = 0;
int8_t filter_enable = 0;
int8_t can_enable = 0;
int8_t mux_enable = 1;
int8_t muxed_chans =5;
int8_t eeprom_status = 1;
int8_t trasmission_counter=0;
uint8_t canProtocol_compatibility_ack = 0;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_parse_can_pollingMsg(hal_canmsg_t *msg, uint8_t *Txdata, int8_t *datalen);
static void s_parse_can_loaderMsg(hal_canmsg_t *msg, uint8_t *Txdata, int8_t *datalen);
static void s_parse_can_msg(void);
static void s_calculate_and_send_data(void);
static void s_timer1_callback(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
int main(void)
{ 
	uint16_t i, res;
	hal_timer_config_datastruct_t timer_configData;

	
	
	hal_strain_ports_init();
	
	// Select analog channel 1 
	hal_strain_mux_channelSelect(1);
	
	// Init SPI for communication with ext ADC/DAC
	hal_strain_AD_init(); 

	hal_error_resetCause_get();

	hal_watchdog_enable();

	strain_config_init(&strain_cfg);
	
	// EEPROM Data Recovery
	// Initialize strain_cfg.ee_data variable in RAM with the Data EEPROM stored values 
	//eeprom_strain_cfg.ee_data_read(&strain_cfg.ee_data, sizeof(strain_cfg.ee_data)); 
	res = strain_config_readFromEE(&strain_cfg.ee_data);
    if(0 == res)// eeprom has been erased
    {
        strain_config_saveInEE(&strain_cfg.ee_data);
    }
// IIR filter not used	
//	// IIR filter coefs
//	strain_IIRFilter_readFromEE();
//
//	// IIR Transposed filters init
//	strain_IIRFilter_init();

	timer_configData.gate_op = 0;
	timer_configData.idle = 0;
	timer_configData.source = TIMER_SOURCE_INTERNAL;
	timer_configData.prescaler = hal_timer_prescaler64;
    timer_configData.call_bkp_fn = s_timer1_callback;
	
	// saturate match_value to 0xFFFF in case of delays larger than 209
	if (strain_cfg.ee_data.EE_AN_ChannelScanningTime >= 2097)
	{
		timer_configData.match_value = 0xFFFF;
	}
	else
	{
		timer_configData.match_value = (34.78 * strain_cfg.ee_data.EE_AN_ChannelScanningTime); //31.25
	}
	
	hal_timer_config(hal_timerT1, &timer_configData);   


	hal_timer_interrupt_config(hal_timerT1, hal_timer_IntPrio7);
	
	hal_timer_start(hal_timerT1);


	

	// Blink Led 
	for(i=1;i<=20;i++)
	{  
		LED = ~LED;
		hal_delay(800000);
	}
	
	// CAN Configuration
	hal_can_init(hal_can_portCAN1);	
	// Cofigure CAN filter
	hal_can_boardFilter_set(hal_can_portCAN1, strain_cfg.ee_data.EE_CAN_BoardAddress);
	
	hal_error_resetCause_send(strain_cfg.ee_data.EE_CAN_BoardAddress);
	
	hal_error_resetRegister_reset();
	
	
  for(;;)
  {
	hal_watchdog_reset();
	s_parse_can_msg();
  }
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// parse board test messages and renspond to the sender 
// response has SID and a variable payload different freom command 2 another
// source and dest are swapped from incoming to outgoing messages
//
// polling messages
//  -------  -------  -------  -------  ------- 
// | 3b     | 4b     | 4b     |                |
// | class  | Source | Dest   |      ....      |
//  -------  -------  -------  -------  ------- 
// recursive messages
//  -------  -------  -------  -------  ------- 
// | 3b     | 4b     | 4b     |      B[0-7]    |
// |class	| Source | Type   |     Payload    |
//  -------  -------  -------  -------  ------- 
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

		default: //UNKNOWN CLASS MESSAGES
		{
			hal_error_canMsg_set(Txdata, &datalen, msg->CAN_Per_Msg_PayLoad[0]);
		break;
		}
	}
	
	  // Load message ID , Data into transmit buffer and set transmit request bit
	  // swap source and destination
	  SID = (msg->CAN_Per_Msg_Class << 8 ) | ( strain_cfg.ee_data.EE_CAN_BoardAddress << 4 ) | ( msg->CAN_Poll_Msg_Source );
	
	  // Send ack messages with datalen > 0
	  // Skip ack messages with datalen < 0
	  // Send ack messages with datalen = 0 and CAN_ACK_EVERY_MESSAGE = 1
	  if ( (datalen > 0) || (datalen == 0 && strain_cfg.can_ack_every_msg == 1) )
	  { 
	    // Wait till previous message is transmitted completely
	   // while(!CAN1IsTXReady(0));
	   hal_can_put(hal_can_portCAN1, strain_cfg.ee_data.EE_CAN_BoardAddress, SID,hal_can_frameType_data,datalen,Txdata); 
	  }
   }



static void s_calculate_and_send_data(void)
{    
  uint16_t SID; //adc;
  // LATBbits.LATB12 = 1; // led on

  // ForceData and TorqueData are defined as 8 bytes arrays, but only 6 bytes are used.
  // The remainaing two should be not trasmitted, unless a particular event occurs (i.e:debug message, saturation warning etc.)
  uint8_t ForceDataCalib[8], TorqueDataCalib[8]; 
  static uint8_t ForceDataCalibSafe[8];
  static uint8_t TorqueDataCalibSafe[8];
  uint8_t ForceDataUncalib[8], TorqueDataUncalib[8]; 
  static uint8_t ChToTransmit=1; 
  uint8_t saturation = 0;
  uint8_t i=0;
  uint8_t length=6;
 
  for (i=0; i<6; i++)
  {
	if ((uint16_t)(strain_cfg.ee_data.EE_AN_ChannelValue[i]+HEX_VALC) > 64000 ||
	    (uint16_t)(strain_cfg.ee_data.EE_AN_ChannelValue[i]+HEX_VALC) <  1000)
		saturation=1; 
  }



  VectorAdd (6,(fractional*) strain_cfg.ee_data.EE_AN_ChannelValue, (fractional*)strain_cfg.ee_data.EE_AN_ChannelValue, strain_cfg.ee_data.EE_CalibrationTare); // ChannelValue = ChannelValue + CalibrationTare 

  //MatrixMultiply (int numRows1,int numCols1Rows2,int numCols2,fractional* dstM,fractional* srcM1,fractional* srcM2, torque_value = Tmatrix * channel value)
       if (UseCalibration==0 || UseCalibration>=4)
    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_A[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]);
  else if (UseCalibration==1)
    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_A[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]);
  else if (UseCalibration==2)
    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_B[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]);
  else if (UseCalibration==3)
    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_C[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]);

/*Note: anche se EE_TF_TorqueValue è composto da tre elementi, 
si effettua la moltiplicazione su 6 per effettuare in una unica operazione su EE_TF_TorqueValue e EE_TF_ForceValue.
Ovviamente i due array devono essere salvati su memoria contigua.
*/
	//calculate Data for calibrated values...
	VectorAdd (6, (fractional*)strain_cfg.ee_data.EE_TF_TorqueValue, (fractional*)strain_cfg.ee_data.EE_TF_TorqueValue, CurrentTare); //torque value += CurrentTare
	strain_cfg.ee_data.EE_TF_TorqueValue[0]+=HEX_VALC;
	strain_cfg.ee_data.EE_TF_TorqueValue[1]+=HEX_VALC;
	strain_cfg.ee_data.EE_TF_TorqueValue[2]+=HEX_VALC;
	strain_cfg.ee_data.EE_TF_TorqueValue[3]+=HEX_VALC; // is equal to EE_TF_ForceValue[0], because it is saved on contiguos memory with EE_TF_TorqueValue
	strain_cfg.ee_data.EE_TF_TorqueValue[4]+=HEX_VALC; // is equal to EE_TF_ForceValue[1]
	strain_cfg.ee_data.EE_TF_TorqueValue[5]+=HEX_VALC; // is equal to EE_TF_ForceValue[2]

    //Note: in despite of field name in strain_cfg.ee_data, Force value are the first three elements
	memcpy(ForceDataCalib,strain_cfg.ee_data.EE_TF_TorqueValue,6); // TorqueValue is an array of 3 int ==> 6 byte
	memcpy(TorqueDataCalib,strain_cfg.ee_data.EE_TF_ForceValue,6); // ForceeValue is an array of 3 int ==> 6 byte

	//...and for not calibrated ones
	strain_cfg.ee_data.EE_AN_ChannelValue[0]+=HEX_VALC;
	strain_cfg.ee_data.EE_AN_ChannelValue[1]+=HEX_VALC;
	strain_cfg.ee_data.EE_AN_ChannelValue[2]+=HEX_VALC;
	strain_cfg.ee_data.EE_AN_ChannelValue[3]+=HEX_VALC;
	strain_cfg.ee_data.EE_AN_ChannelValue[4]+=HEX_VALC;
	strain_cfg.ee_data.EE_AN_ChannelValue[5]+=HEX_VALC;
	memcpy(ForceDataUncalib,strain_cfg.ee_data.EE_AN_ChannelValue,6);
	memcpy(TorqueDataUncalib,&strain_cfg.ee_data.EE_AN_ChannelValue[3],6);

  // Load message ID , Data into transmit buffer and set transmit request bit
  // class, source, type for periodoc messages
  if (saturation!=0)
  {
	 length=7;
	 ForceDataCalib[0]=ForceDataCalibSafe[0];
	 ForceDataCalib[1]=ForceDataCalibSafe[1];
	 ForceDataCalib[2]=ForceDataCalibSafe[2];
	 ForceDataCalib[3]=ForceDataCalibSafe[3];
	 ForceDataCalib[4]=ForceDataCalibSafe[4];
	 ForceDataCalib[5]=ForceDataCalibSafe[5];
	 TorqueDataCalib[0]=TorqueDataCalibSafe[0];
	 TorqueDataCalib[1]=TorqueDataCalibSafe[1];
	 TorqueDataCalib[2]=TorqueDataCalibSafe[2];
	 TorqueDataCalib[3]=TorqueDataCalibSafe[3];
	 TorqueDataCalib[4]=TorqueDataCalibSafe[4];
	 TorqueDataCalib[5]=TorqueDataCalibSafe[5];
	 ForceDataCalib[6]=1;
	 TorqueDataCalib[6]=1;	
	 ForceDataUncalib[6]=1;
	 TorqueDataUncalib[6]=1;
  }
  else
  {
	 ForceDataCalibSafe[0]=ForceDataCalib[0];
	 ForceDataCalibSafe[1]=ForceDataCalib[1];
	 ForceDataCalibSafe[2]=ForceDataCalib[2];
	 ForceDataCalibSafe[3]=ForceDataCalib[3];
	 ForceDataCalibSafe[4]=ForceDataCalib[4];
	 ForceDataCalibSafe[5]=ForceDataCalib[5];
	 TorqueDataCalibSafe[0]=TorqueDataCalib[0];
	 TorqueDataCalibSafe[1]=TorqueDataCalib[1];
	 TorqueDataCalibSafe[2]=TorqueDataCalib[2];
	 TorqueDataCalibSafe[3]=TorqueDataCalib[3];
	 TorqueDataCalibSafe[4]=TorqueDataCalib[4];
	 TorqueDataCalibSafe[5]=TorqueDataCalib[5];
	 ForceDataCalibSafe[6]=0;
	 ForceDataCalibSafe[7]=0;
	 TorqueDataCalibSafe[6]=0;
	 TorqueDataCalibSafe[7]=0;
  }

  // force data 

  if (DebugCalibration==1)
  {
	  SID = (CAN_MSG_CLASS_PERIODIC) | ((strain_cfg.ee_data.EE_CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR) ;
	  hal_can_put_immediately(hal_can_portCAN1, SID, ForceDataCalib, length, 0 );

	  SID = (CAN_MSG_CLASS_PERIODIC) | ((strain_cfg.ee_data.EE_CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR) ;
	  hal_can_put_immediately(hal_can_portCAN1, SID, TorqueDataCalib,length,1);

	  SID = (CAN_MSG_CLASS_PERIODIC) | ((strain_cfg.ee_data.EE_CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE) ;
	  hal_can_put_immediately(hal_can_portCAN1, SID, ForceDataUncalib,length,2);

	  SID = (CAN_MSG_CLASS_PERIODIC) | ((strain_cfg.ee_data.EE_CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE) ;
	  while(!(hal_can_txHwBuff_isEmpty(hal_can_portCAN1, 0))); // wiat buffer 0
	  hal_can_put_immediately(hal_can_portCAN1, SID, TorqueDataUncalib,length,0);
  }
else
{
  if (UseCalibration==1)
	{
	  SID = (CAN_MSG_CLASS_PERIODIC) | ((strain_cfg.ee_data.EE_CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR) ;
	  hal_can_put_immediately(hal_can_portCAN1, SID, ForceDataCalib, length,0 );
	  // torque data 
	  SID = (CAN_MSG_CLASS_PERIODIC) | ((strain_cfg.ee_data.EE_CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR) ;
	  hal_can_put_immediately(hal_can_portCAN1, SID, TorqueDataCalib, length, 1 );
	}
  else
	{
	  SID = (CAN_MSG_CLASS_PERIODIC) | ((strain_cfg.ee_data.EE_CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR) ;
	  hal_can_put_immediately(hal_can_portCAN1, SID, ForceDataUncalib, length,0 );
	  // torque data 
	  SID = (CAN_MSG_CLASS_PERIODIC) | ((strain_cfg.ee_data.EE_CAN_BoardAddress)<<4) | (ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR) ;
	  hal_can_put_immediately(hal_can_portCAN1, SID, TorqueDataUncalib, length, 1 );
	}  
}

  // Wait till message is transmitted completely
  //  while(!CAN1IsTXReady(0)) 
  //    ;
  
  // Next channel
  if (ChToTransmit == 5)
    ChToTransmit=0;
  else
    ChToTransmit++;


  // LATBbits.LATB12 = 0; // led off

  // WriteTimer2(0x0);
  //IFS0bits.T2IF = 0; // clear flag  
}


static void s_parse_can_pollingMsg(hal_canmsg_t *msg, uint8_t *Txdata, int8_t *datalen)
{
	uint16_t i;
	uint16_t j;

	switch (msg->CAN_Per_Msg_PayLoad[0])
	{
		case ICUBCANPROTO_POL_AS_CMD__SET_BOARD_ADX: // set board CAN address 
		{     
			if ( ( msg->CAN_Per_Msg_PayLoad[1] > 0 ) && ( msg->CAN_Per_Msg_PayLoad[1] <= 15 ))
			{
				strain_cfg.ee_data.EE_CAN_BoardAddress = msg->CAN_Per_Msg_PayLoad[1];
				hal_can_boardFilter_set(hal_can_portCAN1, strain_cfg.ee_data.EE_CAN_BoardAddress);    
				if (strain_cfg.save_eeprom_atonce)
				{
					strain_config_saveInEE(&strain_cfg.ee_data);
					//strain_IIRFilter_saveInEE(); Filter IIR not used
				}
			}
			else 
			{
				hal_error_manage(ERR_CAN_PARAMETERS_INVALID);
				return; 
			} 
			*datalen=0;
		break;
		}
		
		case ICUBCANPROTO_POL_AS_CMD__SET_IIR: // Set IIR Filter parameters: 0x205 len 4  data 1 i MSB LSB 
		{
//IIR filter not used
//			// Set IIR Filter parameters: 0x205 len 4  data 1 i MSB LSB 
//			// N. BQ, <- number of biquads
//			// b0[s], b1[s], a1[s], b2[s], a2[s], <- 1stBQ
//			// b0[s], b1[s], a1[s], b2[s], a2[s], <- 2ndBQ 
//			// b0[s], b1[s], a1[s], b2[s], a2[s], <- 3rdBQ
//			// b0[s], b1[s], a1[s], b2[s], a2[s], <- 4thBQ
//			// b0[s], b1[s], a1[s], b2[s], a2[s], <- 5thBQ
//
//			int index,dat;
//			
//			index = msg->CAN_Per_Msg_PayLoad[1];             // indice del dato del filtro
//			dat = msg->CAN_Per_Msg_PayLoad[2]<<8 | msg->CAN_Per_Msg_PayLoad[3];  // valore del dato
//			
//			if (index==0) // il dato indica il n.di BiQuads
//			{
//				if(dat > IIR_LPF_N_MAX_BQ) // se il numenro di BQ e` superiore a 5 
//				{
//					hal_error_manage(ERR_CAN_IIR_NBQ2BIG);
//					return;
//				}
//				else
//				{
//					IirTransposedCoefs.IIR_N_BQ = dat; 
//				}
//			}
//			if (index > IirTransposedCoefs.IIR_N_BQ * 5 )	// se indirizzo un coeficiente oltre alla dimensione del filtro
//			{
//				hal_error_manage(ERR_CAN_IIR_COEF_INDEXING);
//				return;
//			}
//			else
//			{ 
//				IirTransposedCoefs.IirTransposedCoefs[index-1] = dat;
//			}
//			    
//			strain_IIRFilter_init();
			*datalen=0;
		break;
		}   
		    
	
		case ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_RC: //get MATRIX
		{
			if(msg->CAN_Per_Msg_PayLoad[1] < 6)
			{
				if(msg->CAN_Per_Msg_PayLoad[2] < 6)
				{
                    if (msg->CAN_Per_Msg_PayLoad[3]==0)
                    {
                        Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_RC; 
                        Txdata[1] = msg->CAN_Per_Msg_PayLoad[1]; 
                        Txdata[2] = msg->CAN_Per_Msg_PayLoad[2]; 
                        Txdata[3] = strain_cfg.ee_data.EE_TF_TMatrix_A[msg->CAN_Per_Msg_PayLoad[1]][msg->CAN_Per_Msg_PayLoad[2]] >> 8; 
                        Txdata[4] = strain_cfg.ee_data.EE_TF_TMatrix_A[msg->CAN_Per_Msg_PayLoad[1]][msg->CAN_Per_Msg_PayLoad[2]] & 0xFF;  
                        *datalen=5;	            
                    }
                    else if (msg->CAN_Per_Msg_PayLoad[3]==1)
                    {
                        Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_RC; 
                        Txdata[1] = msg->CAN_Per_Msg_PayLoad[1]; 
                        Txdata[2] = msg->CAN_Per_Msg_PayLoad[2]; 
                        Txdata[3] = strain_cfg.ee_data.EE_TF_TMatrix_B[msg->CAN_Per_Msg_PayLoad[1]][msg->CAN_Per_Msg_PayLoad[2]] >> 8; 
                        Txdata[4] = strain_cfg.ee_data.EE_TF_TMatrix_B[msg->CAN_Per_Msg_PayLoad[1]][msg->CAN_Per_Msg_PayLoad[2]] & 0xFF;  
                        *datalen=5;	            
                    }
                    else if (msg->CAN_Per_Msg_PayLoad[3]==2)
                    {
                        Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_RC; 
                        Txdata[1] = msg->CAN_Per_Msg_PayLoad[1]; 
                        Txdata[2] = msg->CAN_Per_Msg_PayLoad[2]; 
                        Txdata[3] = strain_cfg.ee_data.EE_TF_TMatrix_C[msg->CAN_Per_Msg_PayLoad[1]][msg->CAN_Per_Msg_PayLoad[2]] >> 8; 
                        Txdata[4] = strain_cfg.ee_data.EE_TF_TMatrix_C[msg->CAN_Per_Msg_PayLoad[1]][msg->CAN_Per_Msg_PayLoad[2]] & 0xFF;  
                        *datalen=5;	            
                    }
				}
			}
		break;			
		}
			
	
		case ICUBCANPROTO_POL_AS_CMD__GET_CH_DAC:   //get gain of DAC converter 
		{
			if(msg->CAN_Per_Msg_PayLoad[1] < 6)
			{
				Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_CH_DAC; 
				Txdata[1] = msg->CAN_Per_Msg_PayLoad[1];  
				Txdata[2] = strain_cfg.ee_data.EE_AN_ChannelOffset[msg->CAN_Per_Msg_PayLoad[1]] >> 8; 
				Txdata[3] = strain_cfg.ee_data.EE_AN_ChannelOffset[msg->CAN_Per_Msg_PayLoad[1]] & 0xFF; 
				*datalen=4;	            
			}
		break;
		}
			
	
		case ICUBCANPROTO_POL_AS_CMD__GET_CH_ADC:  //get ADC channel
		{
			if(msg->CAN_Per_Msg_PayLoad[1] < 6)
			{
                hal_timer_interrupt_disa(hal_timerT1);// stop timer to avoid rece condition on strain_cfg.ee_data.EE_AN_ChannelValue
                hal_timer_stop(hal_timerT1);
				if(msg->CAN_Per_Msg_PayLoad[2] == 0 || msg->CAN_Per_Msg_PayLoad[2]>=4)
				{
					Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_CH_ADC; 
					Txdata[1] = msg->CAN_Per_Msg_PayLoad[1];  
					Txdata[2] = msg->CAN_Per_Msg_PayLoad[2];
					VectorAdd (6, (fractional*)strain_cfg.ee_data.EE_AN_ChannelValue, (fractional*)strain_cfg.ee_data.EE_AN_ChannelValue, strain_cfg.ee_data.EE_CalibrationTare);
					Txdata[3] = (strain_cfg.ee_data.EE_AN_ChannelValue[msg->CAN_Per_Msg_PayLoad[1]]+HEX_VALC) >> 8; 
					Txdata[4] = (strain_cfg.ee_data.EE_AN_ChannelValue[msg->CAN_Per_Msg_PayLoad[1]]+HEX_VALC) & 0xFF; 
					*datalen=5;
				}
				else
				{
					Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_CH_ADC; 
					Txdata[1] = msg->CAN_Per_Msg_PayLoad[1];  
					Txdata[2] = msg->CAN_Per_Msg_PayLoad[2];
					
					VectorAdd (6, (fractional*)strain_cfg.ee_data.EE_AN_ChannelValue, (fractional*)strain_cfg.ee_data.EE_AN_ChannelValue, strain_cfg.ee_data.EE_CalibrationTare);
				
			        if(msg->CAN_Per_Msg_PayLoad[2]==1)
					    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_A[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]); // fractional* srcM2 
			        else if(msg->CAN_Per_Msg_PayLoad[2]==2)
					    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_B[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]); // fractional* srcM2 
			        else if(msg->CAN_Per_Msg_PayLoad[2]==3)
					    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_C[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]); // fractional* srcM2 

					VectorAdd (6, (fractional*)strain_cfg.ee_data.EE_TF_TorqueValue, (fractional*)strain_cfg.ee_data.EE_TF_TorqueValue, CurrentTare);
					Txdata[3] = (strain_cfg.ee_data.EE_TF_TorqueValue[msg->CAN_Per_Msg_PayLoad[1]]+HEX_VALC) >> 8; 
					Txdata[4] = (strain_cfg.ee_data.EE_TF_TorqueValue[msg->CAN_Per_Msg_PayLoad[1]]+HEX_VALC) & 0xFF; 
					*datalen=5;
				} 
                hal_timer_start(hal_timerT1);                
                hal_timer_interrupt_ena(hal_timerT1);
		    }
		break;
		}
		

		case ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_G: //get matrix gain
		{
			Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_G; 
			Txdata[1] = strain_cfg.ee_data.EE_MatrixGain; 
			*datalen=2;           
		break;
		}
		
	
		case ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_G: //set matrix gain
	    { 
		  strain_cfg.ee_data.EE_MatrixGain=msg->CAN_Per_Msg_PayLoad[1];
		  eeprom_status=0;
		  *datalen=0;
	    break;
		}  
			

		case ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES: 
		{
			// Note: EE_FullScales isn't used by strain application, but only for store datas.
			if(msg->CAN_Per_Msg_PayLoad[1] < 6)
			{
				Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES; 
				Txdata[1] = msg->CAN_Per_Msg_PayLoad[1]; 
				Txdata[2] = strain_cfg.ee_data.EE_FullScales[msg->CAN_Per_Msg_PayLoad[1]] >> 8; 
				Txdata[3] = strain_cfg.ee_data.EE_FullScales[msg->CAN_Per_Msg_PayLoad[1]] & 0xFF; 
				*datalen=4;
			}           
		break;
		}
		

		case ICUBCANPROTO_POL_AS_CMD__SET_FULL_SCALES: 
		{ 
			if(msg->CAN_Per_Msg_PayLoad[1] < 6)
			{
				strain_cfg.ee_data.EE_FullScales[msg->CAN_Per_Msg_PayLoad[1]]= msg->CAN_Per_Msg_PayLoad[2]<<8 | msg->CAN_Per_Msg_PayLoad[3];
			}
			*datalen=0;
		break;  
		}

		case ICUBCANPROTO_POL_AS_CMD__SET_SERIAL_NO: //set serial number
		{ 
			strain_cfg.ee_data.EE_SerialNumber[0]= msg->CAN_Per_Msg_PayLoad[1];
			strain_cfg.ee_data.EE_SerialNumber[1]= msg->CAN_Per_Msg_PayLoad[2];
			strain_cfg.ee_data.EE_SerialNumber[2]= msg->CAN_Per_Msg_PayLoad[3];
			strain_cfg.ee_data.EE_SerialNumber[3]= msg->CAN_Per_Msg_PayLoad[4];
			strain_cfg.ee_data.EE_SerialNumber[4]= msg->CAN_Per_Msg_PayLoad[5];
			strain_cfg.ee_data.EE_SerialNumber[5]= msg->CAN_Per_Msg_PayLoad[6];
			strain_cfg.ee_data.EE_SerialNumber[6]= msg->CAN_Per_Msg_PayLoad[7];
			strain_cfg.ee_data.EE_SerialNumber[7]= 0;
			
			eeprom_status=0;
			
			*datalen=0;
		break;
		}  
		

		case ICUBCANPROTO_POL_AS_CMD__GET_SERIAL_NO:
		{
			Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_SERIAL_NO; 
			Txdata[1] = strain_cfg.ee_data.EE_SerialNumber[0];
			Txdata[2] = strain_cfg.ee_data.EE_SerialNumber[1]; 
			Txdata[3] = strain_cfg.ee_data.EE_SerialNumber[2]; 
			Txdata[4] = strain_cfg.ee_data.EE_SerialNumber[3];
			Txdata[5] = strain_cfg.ee_data.EE_SerialNumber[4]; 
			Txdata[6] = strain_cfg.ee_data.EE_SerialNumber[5]; 
			Txdata[7] = strain_cfg.ee_data.EE_SerialNumber[6];
			
			*datalen=8;            
		break;
		}
		

		case ICUBCANPROTO_POL_AS_CMD__GET_EEPROM_STATUS: 
		{
			Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_EEPROM_STATUS; 
			Txdata[1] = eeprom_status;

			*datalen=2;        
		break;
		}

		case ICUBCANPROTO_POL_AS_CMD__GET_CALIB_TARE:
		{
			Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_CALIB_TARE; 
			Txdata[1] = msg->CAN_Per_Msg_PayLoad[1];
			Txdata[2] = strain_cfg.ee_data.EE_CalibrationTare[msg->CAN_Per_Msg_PayLoad[1]] >> 8; 
			Txdata[3] = strain_cfg.ee_data.EE_CalibrationTare[msg->CAN_Per_Msg_PayLoad[1]] & 0xFF; 
			
			*datalen=4;          
		break;
		}
		
	
		case ICUBCANPROTO_POL_AS_CMD__SET_CALIB_TARE: 
		{
			eeprom_status=0;
 
			if (msg->CAN_Per_Msg_PayLoad[1]==0)
			{
				for (i=0; i<6; i++)
				{
			  		strain_cfg.ee_data.EE_CalibrationTare[i]=0;
				}
			}
			else if (msg->CAN_Per_Msg_PayLoad[1]==1)
			{
				for (i=0; i<6; i++)
				{
					strain_cfg.ee_data.EE_CalibrationTare[i]=-(strain_cfg.ee_data.EE_AN_ChannelValue[i]);
				}
			}     
			else if (msg->CAN_Per_Msg_PayLoad[1]==2)
			{
				if (msg->CAN_Per_Msg_PayLoad[2]<6)
				{
					strain_cfg.ee_data.EE_CalibrationTare[msg->CAN_Per_Msg_PayLoad[2]] = msg->CAN_Per_Msg_PayLoad[3]<<8 | msg->CAN_Per_Msg_PayLoad[4];
				}
			}
			*datalen=0;
		break;  
		}

		case ICUBCANPROTO_POL_AS_CMD__GET_CURR_TARE: 
		{
			Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_CURR_TARE; 
			Txdata[1] = msg->CAN_Per_Msg_PayLoad[1];
			Txdata[2] = CurrentTare[msg->CAN_Per_Msg_PayLoad[1]] >> 8; 
			Txdata[3] = CurrentTare[msg->CAN_Per_Msg_PayLoad[1]] & 0xFF; 
			
			*datalen=4;           
		break;
		}
	
		case ICUBCANPROTO_POL_AS_CMD__SET_CURR_TARE: 
		{ 
			if (msg->CAN_Per_Msg_PayLoad[1]==0)
			{
				for (i=0; i<6; i++)
				{
					CurrentTare[i]=0;
				}
			}
			else if (msg->CAN_Per_Msg_PayLoad[1]==1)
			{
                hal_timer_interrupt_disa(hal_timerT1);// stop timer to avoid rece condition on strain_cfg.ee_data.EE_AN_ChannelValue
                hal_timer_stop(hal_timerT1);

				VectorAdd (6, (fractional*)strain_cfg.ee_data.EE_AN_ChannelValue, (fractional*)strain_cfg.ee_data.EE_AN_ChannelValue, strain_cfg.ee_data.EE_CalibrationTare);
			    if (UseCalibration==0 || UseCalibration==1)
				    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_A[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]); // fractional* srcM2 
			    else if (UseCalibration==2)
				    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_B[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]); // fractional* srcM2 
			    else if (UseCalibration==3)
				    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_C[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]); // fractional* srcM2 
                else
  				    MatrixMultiply(6,6,1,&strain_cfg.ee_data.EE_TF_TorqueValue[0],&strain_cfg.ee_data.EE_TF_TMatrix_A[0][0],(int*) &strain_cfg.ee_data.EE_AN_ChannelValue[0]); // fractional* srcM2 
              
				for (i=0; i<6; i++)
				{
					CurrentTare[i]=-(strain_cfg.ee_data.EE_TF_TorqueValue[i]);
				}

                hal_timer_start(hal_timerT1);
                hal_timer_interrupt_ena(hal_timerT1);
			}
			else if (msg->CAN_Per_Msg_PayLoad[1]==2)
			{
				if (msg->CAN_Per_Msg_PayLoad[2]<6)
				{
					CurrentTare[msg->CAN_Per_Msg_PayLoad[2]] = msg->CAN_Per_Msg_PayLoad[3]<<8 | msg->CAN_Per_Msg_PayLoad[4];
				}
			}
			*datalen=0;
	    }  
		break;	
	
		case ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_RC: //set i,j value of transform. matrix:
		{ 
			eeprom_status=0;
			//  set i,j value of transform. matrix:
			//  0x205 len 5  data 3 i j vv vv 
			if(msg->CAN_Per_Msg_PayLoad[1] < 6)
			{
				if(msg->CAN_Per_Msg_PayLoad[2] < 6)
				{
                    if      (msg->CAN_Per_Msg_PayLoad[5] == 0)
                    {
					   strain_cfg.ee_data.EE_TF_TMatrix_A[msg->CAN_Per_Msg_PayLoad[1]][msg->CAN_Per_Msg_PayLoad[2]] = msg->CAN_Per_Msg_PayLoad[3]<<8 | msg->CAN_Per_Msg_PayLoad[4];
                    }
                    else if (msg->CAN_Per_Msg_PayLoad[5] == 1)
                    {
					   strain_cfg.ee_data.EE_TF_TMatrix_B[msg->CAN_Per_Msg_PayLoad[1]][msg->CAN_Per_Msg_PayLoad[2]] = msg->CAN_Per_Msg_PayLoad[3]<<8 | msg->CAN_Per_Msg_PayLoad[4];
                    }                   
                    else if (msg->CAN_Per_Msg_PayLoad[5] == 2)
                    {
					   strain_cfg.ee_data.EE_TF_TMatrix_C[msg->CAN_Per_Msg_PayLoad[1]][msg->CAN_Per_Msg_PayLoad[2]] = msg->CAN_Per_Msg_PayLoad[3]<<8 | msg->CAN_Per_Msg_PayLoad[4];
                    }                    
                    else
                       hal_error_manage(ERR_CAN_MATRIX_INDEXING);
				}
			}
			else 
			{
				hal_error_manage(ERR_CAN_MATRIX_INDEXING);
				return;
			}
			*datalen=0;
		break;  
		}		
		
	    case ICUBCANPROTO_POL_AS_CMD__SET_CH_DAC:  //  set DAC value 0x205 len 4  data 4 ch msb lsb
	    {	       
			eeprom_status=0;
			i = msg->CAN_Per_Msg_PayLoad[2]<<8 | msg->CAN_Per_Msg_PayLoad[3];
			j = msg->CAN_Per_Msg_PayLoad[1];
			if ( (j >= 0) && (j <= 5 ))
			{
				strain_cfg.ee_data.EE_AN_ChannelOffset[j]=i;
			}
			else 
			{
				hal_error_manage(ERR_CAN_PARAMETERS_INVALID);
				return; 
			}
			*datalen=0;
	    break;
		}
		
		case ICUBCANPROTO_POL_AS_CMD__SET_TXMODE: // set continuous or on demand tx  0x205 len 2  data 7 0/1
		{
			if(msg->CAN_Per_Msg_PayLoad[1]==0)//Transmit calibrated data continuosly with matrix A
			{ 
				UseCalibration=1;
				DebugCalibration=0;
				can_enable=1;
				hal_timer_interrupt_ena(hal_timerT1);
			}
			else if (msg->CAN_Per_Msg_PayLoad[1]==1) //Do acquisition but do not transmit
			{
				UseCalibration=1;
				DebugCalibration=0;
				can_enable=0; 
				hal_timer_interrupt_ena(hal_timerT1);
			}
			else if (msg->CAN_Per_Msg_PayLoad[1]==3) //Transmit NOT calibrated data continuosly
			{
				UseCalibration=0;
				DebugCalibration=0;
				can_enable=1;
				hal_timer_interrupt_ena(hal_timerT1);
			}
			else if (msg->CAN_Per_Msg_PayLoad[1]==4) //Transmit calibrated data continuosly with matrix B
			{
				UseCalibration=1;
				DebugCalibration=0;
				can_enable=1;
				hal_timer_interrupt_ena(hal_timerT1);
			}
			else if (msg->CAN_Per_Msg_PayLoad[1]==5) //Transmit calibrated data continuosly with matrix C
			{
				UseCalibration=2;
				DebugCalibration=0;
				can_enable=1;
				hal_timer_interrupt_ena(hal_timerT1);
			}
			else if(msg->CAN_Per_Msg_PayLoad[1]==100) //Transmit calibrated (A) and NOT calibrated data continuosly
			{ 
				UseCalibration=1;
				DebugCalibration=1;
				can_enable=1;
				hal_timer_interrupt_ena(hal_timerT1);
			}
			else //Stop acquisition
			{
				UseCalibration=0;
				DebugCalibration=0;
				can_enable=0;
				hal_timer_interrupt_disa(hal_timerT1);
			}
			*datalen=0;
		break;
		}  
	
	
		case ICUBCANPROTO_POL_AS_CMD__MUX_NUM: //set multiplexer number
		{
			muxed_chans=msg->CAN_Per_Msg_PayLoad[1];    
			*datalen=0;	      
		break;
		}
		    

		case ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE: // set datarate for transmission in milliseconds 
				 					  // 0x205 len 2  data 8 n
		{
			uint16_t match_value;
 
			strain_cfg.ee_data.EE_AN_ChannelScanningTime = msg->CAN_Per_Msg_PayLoad[1];
			hal_timer_interrupt_disa(hal_timerT1);
			hal_timer_stop(hal_timerT1);
			
			if (strain_cfg.ee_data.EE_AN_ChannelScanningTime >= 2097)
			{
				match_value = 0xFFFF;
			}
			else
			{
				match_value = ( 34.78 * strain_cfg.ee_data.EE_AN_ChannelScanningTime); //31.25
			}
			hal_timer_matchValue_set(hal_timerT1, hal_timer_prescaler64, match_value);
			hal_timer_start(hal_timerT1);
			hal_timer_interrupt_ena(hal_timerT1);
			*datalen=0;
	    break;
		}
		    
	    case ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION: // set data resolution   
	    							  // 0x205 len 2  data 0x10 n
	    {
	     	*datalen=0;
			hal_error_manage(ERR_CAN_COMMAND_UNAVAILABLE);
			return;
	    }
	    break;

		
		case ICUBCANPROTO_POL_AS_CMD__SAVE2EE: // Save configuration data to EE
								// 0x205 len 1  data 9 
		{
			eeprom_status=1;
			strain_config_saveInEE(&strain_cfg.ee_data);
			//strain_IIRFilter_saveInEE(); // IIR filter not used
			
			// todo: checksum calcuation
			
			*datalen=0;
		break;
		}
		
		case ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION:
		{
			if( (strain_srcCode_info_ptr->canProtocol.version == msg->CAN_Per_Msg_PayLoad[1]) &&
			    (strain_srcCode_info_ptr->canProtocol.release == msg->CAN_Per_Msg_PayLoad[2]) )
				{
					canProtocol_compatibility_ack = 1;
				}
            else
                {
                    canProtocol_compatibility_ack = 0;
                }
	
			Txdata[0] = ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION;
			Txdata[1] = icubCanProto_boardType__strain; 
			Txdata[2] = strain_srcCode_info_ptr->fw_ExeFile.version;
			Txdata[3] = strain_srcCode_info_ptr->fw_ExeFile.release;
			Txdata[4] = strain_srcCode_info_ptr->fw_ExeFile.build;
			Txdata[5] = strain_srcCode_info_ptr->canProtocol.version;
			Txdata[6] = strain_srcCode_info_ptr->canProtocol.release;			
			Txdata[7] = canProtocol_compatibility_ack;
			*datalen=8;           
		break;
		}

		// ==> the following commands are unused actually: <==
	    case ICUBCANPROTO_POL_AS_CMD__SELECT_ACTIVE_CH:    	// select witch channel is sampled and CANsmitted
	   									    // 0x205 len 2  data 5 0bxx000001
	    {
	      if( msg->CAN_Per_Msg_PayLoad[1] & 0x1 )
	        strain_cfg.ee_data.EE_AN_ActiveChannels[0] = 1;
	      else
	        strain_cfg.ee_data.EE_AN_ActiveChannels[0] = 0;
	
	      if( msg->CAN_Per_Msg_PayLoad[1] & 0x2 )
	        strain_cfg.ee_data.EE_AN_ActiveChannels[1] = 1;
	      else
	        strain_cfg.ee_data.EE_AN_ActiveChannels[1] = 0;
	 
	      if( msg->CAN_Per_Msg_PayLoad[1] & 0x4 )
	        strain_cfg.ee_data.EE_AN_ActiveChannels[2] = 1;
	      else
	        strain_cfg.ee_data.EE_AN_ActiveChannels[2] = 0;
	
	      if( msg->CAN_Per_Msg_PayLoad[1] & 0x8 )
	        strain_cfg.ee_data.EE_AN_ActiveChannels[3] = 1;
	      else
	        strain_cfg.ee_data.EE_AN_ActiveChannels[3] = 0;
	
	      if( msg->CAN_Per_Msg_PayLoad[1] & 0x10 )
	        strain_cfg.ee_data.EE_AN_ActiveChannels[4] = 1;
	      else
	        strain_cfg.ee_data.EE_AN_ActiveChannels[4] = 0;
	
	      if( msg->CAN_Per_Msg_PayLoad[1] & 0x20 )
	        strain_cfg.ee_data.EE_AN_ActiveChannels[5] = 1;
	      else
	        strain_cfg.ee_data.EE_AN_ActiveChannels[5] = 0;      
	      *datalen=0;    
		break;  
		}    

   	
		case ICUBCANPROTO_POL_AS_CMD__MUX_EN:  //enabling multiplexer
		{
			if(msg->CAN_Per_Msg_PayLoad[1]==0)
			{ 
				mux_enable=0;
				strain_cfg.ee_data.EE_AN_SelectedChannel=0;
				hal_strain_mux_channelSelect(strain_cfg.ee_data.EE_AN_SelectedChannel); 
			}
			else
			{
				mux_enable=1;
				strain_cfg.ee_data.EE_AN_SelectedChannel=0;
				hal_strain_mux_channelSelect(strain_cfg.ee_data.EE_AN_SelectedChannel);
			}
			*datalen=0; 
		break;
		}
	
		case ICUBCANPROTO_POL_AS_CMD__FILTER_EN: //enabling filter
		{
            // IIR filter not used.
//			if(msg->CAN_Per_Msg_PayLoad[1]==0)
//			{
//				filter_enable=0;
//			}
//			else
//			{
//				filter_enable=1;
//			}
//			*datalen=0;
		break;  
		}
	
		default:
		{
			// UNKNOWN COMMAND FOR THIS CLASS
			hal_error_canMsg_set(Txdata, datalen, msg->CAN_Per_Msg_PayLoad[0]);
		break;
		}
		
	}   	
}



static void s_parse_can_loaderMsg(hal_canmsg_t *msg, uint8_t *Txdata, int8_t *datalen)
{
	uint16_t SID;
	uint8_t tmp, j;

	hal_timer_interrupt_disa(hal_timerT1);
	
	switch (msg->CAN_Per_Msg_PayLoad[0])
	{
		case ICUBCANPROTO_BL_BROADCAST: 
		{
			//Create ID for CAN message
			SID = ICUBCANPROTO_CLASS_BOOTLOADER  | ( strain_cfg.ee_data.EE_CAN_BoardAddress << 4 ) | (0);
			Txdata[0] = ICUBCANPROTO_BL_BROADCAST;
			Txdata[1] = icubCanProto_boardType__strain; 
			Txdata[2] = strain_srcCode_info_ptr->fw_ExeFile.version;	//Firmware version number for BOOTLOADER
			Txdata[3] = strain_srcCode_info_ptr->fw_ExeFile.release;	//Firmware build number.
			Txdata[4] = strain_srcCode_info_ptr->fw_ExeFile.build;		//Firmware build number.
			*datalen=5; 
		break;				
		}
				
	
		case ICUBCANPROTO_BL_BOARD:
		{
			asm ("reset");	//Jump to bootlader code
		break;
		}
	
		case ICUBCANPROTO_BL_GET_ADDITIONAL_INFO:
		{
			SID = CAN_MSG_CLASS_LOADER | ( strain_cfg.ee_data.EE_CAN_BoardAddress << 4 ) | (0);
			Txdata[0] = 0x0C; 
			Txdata[1] = 0x00; 
			*datalen=6;
			for (tmp = 0; tmp < 8; tmp++)
			{
				Txdata[1]=tmp;
				for (j=0; j<4; j++)	
				{	
					Txdata[2+j] = strain_cfg.ee_data.EE_AdditionalInfo[j+tmp*4]; 
				}
				hal_can_put(hal_can_portCAN1, strain_cfg.ee_data.EE_CAN_BoardAddress,SID,hal_can_frameType_data,*datalen,Txdata);
			}
			*datalen = -1;
		break;
		}
	
		case ICUBCANPROTO_BL_SET_ADDITIONAL_INFO:
		{
			static uint8_t addinfo_part=0; 
			
			for (j=0; j<4; j++)
			{
				strain_cfg.ee_data.EE_AdditionalInfo[msg->CAN_Per_Msg_PayLoad[1]*4+j] = msg->CAN_Per_Msg_PayLoad[2+j]; 
			}
			
			if (strain_cfg.save_eeprom_atonce)
			{
				if (addinfo_part==7)
				{
					addinfo_part=0;
					strain_config_saveInEE(&strain_cfg.ee_data);
					//strain_IIRFilter_saveInEE(); IIR filter not used
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



static void s_timer1_callback(void)
{    

  uint16_t adc=0;
    
  // LED_ON;

  // channel active
  if (strain_cfg.ee_data.EE_AN_SelectedChannel <= muxed_chans)
	 {
		// read from ADC.
		//strain_cfg.ee_data.EE_AN_ChannelOffset[0] is the value to set int DAC, but it is meaningless.
		//the right value will be set at the end of this function
		hal_strain_AD_setDAC_getADC((uint8_t)PDM_NORMAL, strain_cfg.ee_data.EE_AN_ChannelOffset[0], &adc);
				
		adc-=HEX_VALC;

		strain_cfg.ee_data.EE_AN_ChannelValue[strain_cfg.ee_data.EE_AN_SelectedChannel] = adc;
        //IIR filter not used
		/* if (filter_enable)
	    {

			// IIR LPFilter durata circa 4uSec 
	    	IIRTransposed( 1, (fractional*) &strain_cfg.ee_data.EE_AN_ChannelValue[strain_cfg.ee_data.EE_AN_SelectedChannel], 
	      	(fractional*) &adc, &iirt[strain_cfg.ee_data.EE_AN_SelectedChannel]);

		}

		else
		{
			strain_cfg.ee_data.EE_AN_ChannelValue[strain_cfg.ee_data.EE_AN_SelectedChannel] = adc;
		}*/
	}
	

	// select next channel 
	if (strain_cfg.ee_data.EE_AN_SelectedChannel >= muxed_chans+3)
	{
		strain_cfg.ee_data.EE_AN_SelectedChannel=0;
	}
	else
	{  
		strain_cfg.ee_data.EE_AN_SelectedChannel++;
	}
	if (strain_cfg.ee_data.EE_AN_SelectedChannel==0) 
	{
		TEST_PIN_ON; 
	}
  	//if (!mux_enable) strain_cfg.ee_data.EE_AN_SelectedChannel=0;

	
	if (strain_cfg.ee_data.EE_AN_SelectedChannel <= muxed_chans)
	{
		hal_strain_mux_channelSelect(strain_cfg.ee_data.EE_AN_SelectedChannel);
		// set the correct DAC value for the next reading
		hal_strain_AD_setDAC_getADC((uint8_t)PDM_NORMAL, strain_cfg.ee_data.EE_AN_ChannelOffset[strain_cfg.ee_data.EE_AN_SelectedChannel], &adc);
	}
 	
	TEST_PIN_OFF; 

	if( (strain_cfg.ee_data.EE_AN_SelectedChannel== muxed_chans+1) && (can_enable) )
	{
		s_calculate_and_send_data();
	} 
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

