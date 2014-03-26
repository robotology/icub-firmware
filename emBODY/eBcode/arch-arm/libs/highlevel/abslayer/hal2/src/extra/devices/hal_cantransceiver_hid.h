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

#ifndef _HAL_CANTRANSCEIVER_HID_H_
#define _HAL_CANTRANSCEIVER_HID_H_


/* @file       hal_cantransceiver_hid.h
    @brief      This header file implements hidden interface to ...
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       09/09/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_can.h"
#include "hal_base.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_cantransceiver.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef hal_result_t (*hal_cantransceiver_hid_fn_init_t) (hal_cantransceiver_t id, void* initpar);
typedef hal_result_t (*hal_cantransceiver_hid_fn_enabledisable_t) (hal_cantransceiver_t id);
typedef struct
{   // used inside the public functions of hal_cantransceiver to communicate to the chip, but defined inside brdcfg
    hal_cantransceiver_hid_fn_init_t            init;
    void*                                       initpar;
    hal_cantransceiver_hid_fn_enabledisable_t   enable;
    hal_cantransceiver_hid_fn_enabledisable_t   disable;
} hal_cantransceiver_hid_chip_interface_t;


typedef struct
{   
    hal_cantransceiver_hid_chip_interface_t     chipif;
} hal_cantransceiver_hid_dev_cfg_t;

typedef struct
{
    uint8_t                                     supported_mask;
    hal_cantransceiver_hid_dev_cfg_t            devcfg;
} hal_cantransceiver_hid_brdcfg_t;



// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_cantransceiver_hid_brdcfg_t hal_brdcfg_cantransceiver__theconfig;


// - declaration of extern hidden macros ------------------------------------------------------------------------------    
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




