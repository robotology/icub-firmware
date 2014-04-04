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
#ifndef _HAL_EEPROM_HID_H_
#define _HAL_EEPROM_HID_H_


/* @file       hal_eeprom_hid.h
    @brief      This header file keeps hidden interface to eeprom.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"
#include "hal_i2c.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_eeprom.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{   
    uint32_t                            baseaddress;
    uint32_t                            totalsize;
} hal_eeprom_driver_cfg_fls_t;

typedef struct
{   
    uint32_t                            baseaddress;
    uint32_t                            totalsize;
    hal_i2c_t                           i2cid;
    void*                               initpar;
} hal_eeprom_driver_cfg_i2c_t;


typedef union
{
    hal_eeprom_driver_cfg_fls_t         fls;
    hal_eeprom_driver_cfg_i2c_t         i2c;
} hal_eeprom_driver_cfg_t; 


typedef struct
{   // used inside the public functions of hal_eeprom to communicate to the chip, but defined inside brdcfg
    hal_res_fp_int32_voidp_t                        init;   // init(id, initpar)
    hal_res_fp_int32_uint32_uint32_voidp_voidp_t    read;   // read(id, addr, size, data, readbytes)
    hal_res_fp_int32_uint32_uint32_voidp_voidp_t    write;  // write(id, addr, size, data, writtenbytes)
} hal_eeprom_driver_fun_t;


typedef struct
{ 
    hal_eeprom_driver_cfg_t             cfg;    
    hal_eeprom_driver_fun_t             fun;
} hal_eeprom_driver_t;   


typedef struct
{
    uint32_t                            supportedmask;
    hal_eeprom_driver_t                 driver[hal_eeproms_number];
} hal_eeprom_boardconfig_t;



// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_eeprom_boardconfig_t hal_eeprom__theboardconfig; 

// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




