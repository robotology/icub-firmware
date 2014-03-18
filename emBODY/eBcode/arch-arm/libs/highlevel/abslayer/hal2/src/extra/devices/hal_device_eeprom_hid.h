/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _HAL_DEVICE_EEPROM_HID_H_
#define _HAL_DEVICE_EEPROM_HID_H_


/* @file       hal_device_eeprom_hid.h
    @brief      This header file keeps hidden interface to eeprom.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"
#include "hal_i2c.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_eeprom.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{   
    uint32_t                                baseaddress;
    uint32_t                                totalsize;
} hal_device_eeprom_hid_flashemul_cfg_t;

typedef hal_result_t (*hal_device_eeprom_hid_fn_read_t) (uint32_t, uint32_t, uint8_t*, uint32_t*);
typedef hal_result_t (*hal_device_eeprom_hid_fn_write_t) (uint32_t, uint32_t, uint8_t*, uint32_t*);

typedef struct
{   // used inside the public functions of hal_device_eeprom to communicate to the chip, but defined inside brdcfg
    hal_res_fp_voidp_t                      init;
    void*                                   initpar;
    hal_device_eeprom_hid_fn_read_t         read;
    hal_device_eeprom_hid_fn_write_t        write; 
} hal_device_eeprom_hid_chip_interface_t;

typedef struct
{   
    uint32_t                                baseaddress;
    uint32_t                                totalsize;
    hal_i2c_t                               i2cid;
    hal_device_eeprom_hid_chip_interface_t  chipif;
} hal_device_eeprom_hid_i2cbased_cfg_t;

typedef union
{
    hal_device_eeprom_hid_flashemul_cfg_t   flashemul;
    hal_device_eeprom_hid_i2cbased_cfg_t    i2cbased;
} hal_device_eeprom_hid_dev_cfg_t; 

typedef struct
{
    uint8_t                                 supported_mask;
    hal_device_eeprom_hid_dev_cfg_t         devcfg[hal_eeproms_number];
} hal_device_eeprom_hid_brdcfg_t;



// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section

// - declaration of extern hidden functions ---------------------------------------------------------------------------




///* @fn			extern hal_result_t hal_hid_eeprom_waitstandbystate(void);
//  * @brief  	Wait for EEPROM Standby state
//  */
//extern hal_result_t hal_device_eeprom_hid_waitstandbystate(hal_device_eeprom_t eep);



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




