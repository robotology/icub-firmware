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

#include "hal_common.h"
#include "hal_can.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_cantransceiver.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct
{
    void*                               initpar; 
} hal_cantransceiver_driver_cfg_t;

typedef struct
{   
    hal_res_fp_int32_voidp_t            init;       // init(id, void* initpar)
    hal_res_fp_int32_t                  enable;     // enable(id)
    hal_res_fp_int32_t                  disable;    // disable(id)
} hal_cantransceiver_driver_fun_t;


typedef struct
{ 
    hal_cantransceiver_driver_cfg_t     cfg;    
    hal_cantransceiver_driver_fun_t     fun;
} hal_cantransceiver_driver_t;   


typedef struct
{
    uint32_t                            supportedmask;
    hal_cantransceiver_driver_t         driver[hal_cantransceivers_number];
} hal_cantransceiver_boardconfig_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_cantransceiver_boardconfig_t hal_cantransceiver__theboardconfig;


// - declaration of extern hidden macros ------------------------------------------------------------------------------    
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




