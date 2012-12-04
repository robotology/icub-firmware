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
#ifndef _HAL_STM32_SWITCH_HID_H_
#define _HAL_STM32_SWITCH_HID_H_


/* @file       hal_device_switch_hid.h
    @brief      This header file implements hidden interface to ...
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       09/09/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "hal_mpu_stm32xx_include.h"
#include "hal_base.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_device_switch.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef hal_result_t (*hal_device_switch_hid_fn_config_t) (hal_eth_phymode_t targetphymode, hal_eth_phymode_t* usedphymode);
typedef struct
{   // used inside the public functions of hal_device_switch to communicate to the chip, but defined inside brdcfg
    hal_res_fp_voidp_t                      init;
    void*                                   initpar;
    hal_device_switch_hid_fn_config_t       config;
} hal_device_switch_hid_chip_interface_t;



typedef struct
{   
//    hal_i2c_port_t                          i2cport;
    hal_eth_phymode_t                       targetphymode;
    hal_device_switch_hid_chip_interface_t  chipif;
} hal_device_switch_hid_dev_cfg_t;

typedef struct
{
    hal_bool_t                              supported;
    hal_device_switch_hid_dev_cfg_t         devcfg;
} hal_device_switch_hid_brdcfg_t;



// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section


// - declaration of extern hidden macros ------------------------------------------------------------------------------    
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint32_t hal_device_switch_hid_getsize(const hal_cfg_t *cfg);

extern hal_result_t hal_device_switch_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory);




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




