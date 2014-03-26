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
#ifndef _HAL_SWITCH_HID_H_
#define _HAL_SWITCH_HID_H_


/* @file       hal_switch_hid.h
    @brief      This header file implements hidden interface to ...
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       09/09/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_switch.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{   // used inside the public functions of hal_switch to communicate to the chip, but defined inside brdcfg
    hal_res_fp_voidp_t                      init;       // init(chosendevice_cfg_t* initpar)
    void*                                   initpar;
    hal_res_fp_voidp_t                      config;     // config(hal_ethtransceiver_phymode_t* usedphymode)
} hal_switch_hid_chip_interface_t;


typedef struct
{   
    hal_ethtransceiver_phymode_t            targetphymode;
    hal_switch_hid_chip_interface_t         chipif;
} hal_switch_hid_dev_cfg_t;


typedef struct
{
    hal_bool_t                              supported;
    hal_switch_hid_dev_cfg_t                devcfg;
} hal_switch_hid_brdcfg_t;



// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_switch_hid_brdcfg_t hal_brdcfg_switch__theconfig;


// - declaration of extern hidden macros ------------------------------------------------------------------------------    
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------






#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




