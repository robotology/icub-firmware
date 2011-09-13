/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _MAIS_CONFIG_H_
#define _MAIS_CONFIG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/*	@file 	mais_config.h
	@brief 	This file defines configuration data types and functions to read/write mais data 
			configuration from/to eeprom memory.
	@author valentina.gaggero@iit.it
	@date	10/03/2011
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"
#include "dspic_boards_define.h"

// - public #define  --------------------------------------------------------------------------------------------------
#define ANALOG_CHANEL_NUM		15	//num of analog. channel read by mais.
#define ADDINFO_MAXLEN			32 //max length of additionalInfo array


 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

/* This enum type show which hse resolution are possible.
   Since hsedata_12bit is deprecated, we use it only for test*/
typedef enum
{
	hesdata_16bit = 0,
	hesdata_12bit = 1,
	hesdata_8bit = 2
}hsedata_resolution_t;


// mais configuration datas that must be saved in eeprom. 
typedef struct
{
  uint8_t erased  :1;  // if 1 the ee has been erased. Actually Bootloader doesnt't set this bit when it erases eeprom.
  uint8_t enable_WD  :1; // enable watch dog
  uint8_t enable_BOR :1; // Brown-out reset
  
  uint8_t CAN_BoardAddress;  
  uint8_t CAN_MessageDataRate; 
  uint8_t CAN_Speed;

  uint8_t AdditionalInfo[ADDINFO_MAXLEN];

}mais_eeprom_data_t;

//complete mais data configuration
typedef struct
{
	mais_eeprom_data_t ee_data;

	hsedata_resolution_t hesdata_resolution;
	uint8_t 		 can_ack_every_msg; //if values 1 , application send response msg for each msg received
	uint8_t 		 save_eeprom_atonce; //if values 1, every change in eeprom data, will be save immediately in eeprom

}mais_config_data_t;





    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

// - declaration of extern public functions ---------------------------------------------------------------------------

/**
  * @fn     extern void mais_config_init(mais_config_data_t *mais_cfg);
  * @brief  Initializes mais_cfg with default values
  * @param        mais_cfg: pointer to mais configuration
  * @retval none
  */
extern void mais_config_init(mais_config_data_t *mais_cfg);


/**
  * @fn     extern void mais_config_saveInEE(mais_config_data_t *mais_cfg);
  * @brief  saves all eeprom datas contained in mais_cfg in eeprom 
  * @param        mais_cfg: pointer to mais configuration
  * @retval none
  */
extern void mais_config_saveInEE(mais_config_data_t *mais_cfg);


/**
  * @fn     extern unsigned char mais_config_readFromEE(mais_config_data_t *mais_cfg);
  * @brief  read eeprom datas from eeprom.
  * @param        mais_cfg: pointer to mais configuration
  * @retval 0 if the eeprom is empty, else 1.
  */
extern uint8_t mais_config_readFromEE(mais_config_data_t *mais_cfg);


/**
  * @fn     extern void mais_config_additionalInfo_set(mais_config_data_t *mais_cfg);
  * @brief  write additionalInfo field in eeprom.
  * @param        mais_cfg: pointer to mais configuration
  * @retval none
  */
extern void mais_config_additionalInfo_set(mais_config_data_t *mais_cfg);


/**
  * @fn     extern void mais_config_address_set(mais_config_data_t *mais_cfg);
  * @brief  write can address field in eeprom.
  * @param        mais_cfg: pointer to mais configuration
  * @retval none
  */
extern void mais_config_address_set(mais_config_data_t *mais_cfg);

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


