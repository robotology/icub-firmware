/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:  valentina.gaggero@iit.it
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
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _STRAIN_CONFIG_H_
#define _STRAIN_CONFIG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
// empty-section

// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic_boards_define.h"
#include "dspic30_stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------
#define ANALOG_CHANEL_NUM		13 /* Note: the channels from 6 to 13 are unused on this board, but it is necessary 
									  mantain the size of these arrays to 13 for backward compatibility*/

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 


// EEPROM memorized data for board configuration
typedef struct
{
  // configrazioni relative alla scheda
  uint8_t EE_B_EEErased  :1;  // if 1 the ee has been erased (hopefully ;-)
  uint8_t EE_B_EnableWD  :1;
  uint8_t EE_B_EnableBOR :1;
  
  // configurazioni relative al CAN
  uint8_t EE_CAN_BoardAddress;  
  uint8_t EE_CAN_MessageDataRate;    // framerate of outgoing messages
  uint8_t EE_CAN_Speed;
  
  // configurazioni relative ai canali analogici
  uint8_t EE_AN_ActiveChannels[ANALOG_CHANEL_NUM];  // sequenza di acquisizione canali 0 se canale non usato
  uint8_t EE_AN_SelectedChannel;     // canale attualmente attivo
  uint16_t  EE_AN_ChannelScanningTime; // tempo di scansione dei canali in 100aia di usec
  uint16_t  EE_AN_ChannelOffset[6];    // DAC generated offset
  uint16_t  EE_AN_ChannelValue[ANALOG_CHANEL_NUM];    // ADC values

  // torque/force
  int16_t  EE_TF_TorqueValue[3];      // Torque values
  int16_t  EE_TF_ForceValue[3];       // Force values

  int16_t           EE_TF_TMatrix[6][6];       // SG2TF Transformation Matrix

  // additional info
  uint8_t EE_AdditionalInfo[32]; 

  // matrix calibration gain
  uint8_t EE_MatrixGain;
  int16_t           EE_CalibrationTare[6];
  int16_t           EE_FullScales[6];
  int16_t           EE_debug2[6];
  int16_t           EE_debug3[6];
  int16_t           EE_debug4[6];
  int16_t           EE_debug5[6];
  int16_t           EE_debug6[6];
  int16_t           EE_debug7[6];
  int16_t           EE_debug8[6];
  uint8_t EE_SerialNumber[8]; 
  uint16_t  EE_ChkSum;                 // data validation checksum

}strain_eeprom_data_t;

//complete strain data configuration
typedef struct
{
	strain_eeprom_data_t ee_data;

	uint8_t 		 can_ack_every_msg; //if values 1 , application send response msg for each msg received
	uint8_t 		 save_eeprom_atonce; //if values 1, every change in eeprom data, will be save immediately in eeprom

}strain_config_data_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn     extern void strain_config_init(strain_config_data_t *strain_cfg);
  * @brief  inintialize strain_cfg with deafut values.
  * @param        strain_cfg: pointer to strain_configuration
  * @retval none
  */
extern void strain_config_init(strain_config_data_t *strain_cfg);


/**
  * @fn     extern void strain_config_config_saveInEE(strain_eeprom_data_t *ee_data_ptr );
  * @brief  copies datas pointed by ee_boardConfig_data_ptr of size ee_boardConfig_size words to the eeprom
  * @param        ee_data_ptr: pointer to bard config datas to copy
  * @retval none
  */
extern void strain_config_saveInEE(strain_eeprom_data_t *ee_data_ptr );


/**
  * @fn     extern uint8_t strain_config_readFromEE(strain_eeprom_data_t *ee_data_ptr);
  * @brief  copies datas from eeprom to memory pointed by ee_boardConfig_data_ptr
  * @param        ee_data_ptr: pointer memory wthere the function will copy eemprom data
  * @retval none
  */
extern uint8_t strain_config_readFromEE(strain_eeprom_data_t *ee_data_ptr);


// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


