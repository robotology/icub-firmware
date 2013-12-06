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
// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/*	@file 	strain_config.c
	@brief 	This file defines strain configuration variables and and implements functions for save
			datas in eeprom.
	@author valentina.gaggero@iit.it
	@date	-
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
#include "strain_config.h"
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
// empty-section

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

HAL_EE_DECLARE_VAR_IN_EE(strain_EE_cfg, strain_eeprom_data_t);


static const strain_eeprom_data_t strain_EE_cfg_default = 
{
  0x0,           // EE_B_EEErased             :1
  0x0,           // EE_B_EnableWD             :1
  0x1,           // EE_B_EnableBOR            :1
  0x0F,          // EE_CAN_BoardAddress;      :8
  0x01,          // EE_CAN_MessageDataRate    :8
  0x04,          // EE_CAN_Speed;             :8
  {
    0x01,0x01,   // EE_AN_ActiveChannels[ANALOG_CHANEL_NUM];
    0x01,0x01,
    0x01,0x01,
    0x01,0x01,
    0x01,0x01,
    0x01,0x01,
    0x01
  },
  0x00,          // EE_AN_Selected channel
  0x0001,        // EE_AN_ChannelScanningTime :16
  {
    0x01FF,0x01FF, // EE_AN_ChannelOffset[6]  :6*16
    0x01FF,0x01FF,
    0x01FF,0x01FF 
  },
  {
    0x0000,0x0000, //EE_AN_ChannelValue[ANALOG_CHANEL_NUM];    :13*16
    0x0000,0x0000,
    0x0000,0x0000, 
    0x0000,0x0000, 
    0x0000,0x0000,
    0x0000,0x0000,
    0x0000
  },
  {
    0x0000,0x0000,0x0000 // EE_TF_TorqueValue :3*16
  },
  {
    0x0000,0x0000,0x0000 // EE_TF_ForceValue[3] :3*16
  },
  {
    {0x7FFF,0,0,0,0,0},  // TT_TF_TMatrix
    {0,0x7FFF,0,0,0,0},
    {0,0,0x7FFF,0,0,0},
    {0,0,0,0x7FFF,0,0},
    {0,0,0,0,0x7FFF,0},
    {0,0,0,0,0,0x7FFF},
  },
  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},  //Additional Info
  1,  // Matrix Gain
  {0,0,0,0,0,0},  //Calibration Tare
  {0,0,0,0,0,0},  //Debug1
  {0,0,0,0,0,0},  //Debug2
  {0,0,0,0,0,0},  //Debug3
  {0,0,0,0,0,0},  //Debug4
  {0,0,0,0,0,0},  //Debug5
  {0,0,0,0,0,0},  //Debug6
  {0,0,0,0,0,0},  //Debug7
  {0,0,0,0,0,0},  //Debug8
  {0,0,0,0,0,0,0,0},  //Serial number
  0x0000 // Checksum
};

const version_srcCode_info_t strain_srcCode_info =
{
	//fw_ExeFile
	{
		0x3, //version
		0x0, //release
		0x1  //build
	},
	//canProtocol
	{
		0x1, //major
		0x0	//minor
	}
};
const version_srcCode_info_t *strain_srcCode_info_ptr = &strain_srcCode_info;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
void strain_config_init(strain_config_data_t *strain_cfg)
{
	memcpy(&(strain_cfg->ee_data), &strain_EE_cfg_default, sizeof(strain_eeprom_data_t));
	strain_cfg->can_ack_every_msg = 1;
	strain_cfg->save_eeprom_atonce = 1;
}

void strain_config_saveInEE(strain_eeprom_data_t *ee_data_ptr )
{

    EE_address_t strain_ee_cfg_addr;


    HAL_EE_GET_ADDR(strain_ee_cfg_addr, strain_EE_cfg);

    hal_eeprom_write((uint8_t*)ee_data_ptr, strain_ee_cfg_addr, sizeof(strain_eeprom_data_t));

}



uint8_t strain_config_readFromEE(strain_eeprom_data_t *ee_data_ptr)
{
	strain_eeprom_data_t aux;
    EE_address_t strain_ee_cfg_addr;


    HAL_EE_GET_ADDR(strain_ee_cfg_addr, strain_EE_cfg);

    hal_eeprom_read((uint8_t*)&(aux), strain_ee_cfg_addr, sizeof(strain_eeprom_data_t));
	if( (0 == aux.EE_CAN_BoardAddress) && (0 == aux.EE_CAN_MessageDataRate) )//if eeprom was been erased, the application can use dafault values.
	{
		return(0);
	}
	
	memcpy(ee_data_ptr, &aux, sizeof(strain_eeprom_data_t));
	return(1);

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



