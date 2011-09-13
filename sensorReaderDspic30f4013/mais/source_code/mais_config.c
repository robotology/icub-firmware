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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*	@file 	mais_config.c
	@brief 	This file defines eeprom bufer where save information(configuration and something else) and 
			static variable where set release version. 
			Moreover implements function for read/write mais data configuration from/to eeprom.
	@author valentina.gaggero@iit.it
	@date	10/03/2011
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <libpic30.h>
#include <string.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"

#include "mais_config.h"
#include "hal_eeprom.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

const version_srcCode_info_t mais_srcCode_info = 
{
	//fw_ExeFile
	{
	0x03, //version
	0x00, //release
	0x00  //build
	},
	//canProtocol
	{
		0x1, //mejor
		0x0	 //minor
	}
};

const version_srcCode_info_t *mais_srcCode_info_ptr = &mais_srcCode_info;
// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

HAL_EE_DECLARE_VAR_IN_EE(mais_EE_cfg, mais_eeprom_data_t);

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

void mais_config_init(mais_config_data_t *mais_cfg)
{
	mais_cfg->ee_data.erased = 0x0;
	mais_cfg->ee_data.enable_WD = 0x0;
	mais_cfg->ee_data.enable_BOR = 0x1;
	
	mais_cfg->ee_data.CAN_BoardAddress = 0x0E;  
	mais_cfg->ee_data.CAN_MessageDataRate = 0x1;   
	mais_cfg->ee_data.CAN_Speed = 0x04;
  
	mais_cfg->hesdata_resolution = hesdata_8bit;
	mais_cfg->can_ack_every_msg = 1; 
	mais_cfg->save_eeprom_atonce = 1;
}


void mais_config_saveInEE(mais_config_data_t *mais_cfg)
{
    EE_address_t mais_ee_cfg_addr;
    HAL_EE_GET_ADDR(mais_ee_cfg_addr, mais_EE_cfg);

    hal_eeprom_write((uint8_t*)&(mais_cfg->ee_data), mais_ee_cfg_addr, sizeof(mais_eeprom_data_t));
}



uint8_t mais_config_readFromEE(mais_config_data_t *mais_cfg)
{
	mais_eeprom_data_t aux;

    EE_address_t mais_ee_cfg_addr;
    HAL_EE_GET_ADDR(mais_ee_cfg_addr, mais_EE_cfg);

    hal_eeprom_read((uint8_t*)&(aux), mais_ee_cfg_addr, sizeof(mais_eeprom_data_t));
	if (0 == aux.CAN_BoardAddress)//if eeprom was been erased, the application can use dafault values.
	{
		return(0);
	}
	
	memcpy(&(mais_cfg->ee_data), &aux, sizeof(mais_eeprom_data_t));
	return(1);

}


void mais_config_additionalInfo_set(mais_config_data_t *mais_cfg)
{

    EE_address_t mais_ee_cfg_addr;
    HAL_EE_GET_ADDR(mais_ee_cfg_addr, mais_EE_cfg.AdditionalInfo);

    hal_eeprom_write((uint8_t*)(&(mais_cfg->ee_data.AdditionalInfo)), mais_ee_cfg_addr, ADDINFO_MAXLEN);
}

void mais_config_address_set(mais_config_data_t *mais_cfg)
{
	EE_address_t mais_ee_cfg_addr;
    HAL_EE_GET_ADDR(mais_ee_cfg_addr, mais_EE_cfg);

    hal_eeprom_write((uint8_t*)(&(mais_cfg->ee_data)), mais_ee_cfg_addr, 1);

}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

