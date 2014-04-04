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

#include "hal_common.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hal_switch.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    uint8_t                             numofphysicals;
    hal_ethtransceiver_phymode_t        targetphymode;       
    void*                               initpar; 
} hal_switch_driver_cfg_t;


typedef struct
{   
    hal_res_fp_int32_voidp_t            init;           // init(id, void* initpar)
    hal_res_fp_int32_voidp_t            start;          // start(id, hal_ethtransceiver_phymode_t* usedmiiphymode)
//     hal_res_fp_int32_voidp_t            getmiiphymode;  // getmiiphymode(id, hal_ethtransceiver_phymode_t* usedmiiphymode)
//     hal_res_fp_int32_voidp_t            phylinkupmask;  // phylinkareup(id, uint8_t *mask);
//     hal_res_fp_int32_voidp_uint8_t      phystatus;      // phystatus(id, hal_ethtransceiver_phy_status_t *array, uint8_t sizeofarray)
//     hal_res_fp_int32_uint8_uint32_voidp_t phyerrorinfo;   // phyerrorinfo() see hal_ethtransceiver_phy_errorinfo(). 
} hal_switch_driver_fun_t;


typedef struct
{ 
    hal_switch_driver_cfg_t             cfg;    
    hal_switch_driver_fun_t             fun;
} hal_switch_driver_t;   


typedef struct
{
    uint32_t                            supportedmask;
    hal_switch_driver_t                 driver[hal_switches_number];
} hal_switch_boardconfig_t;


// - declaration of extern hidden variables ---------------------------------------------------------------------------

extern const hal_switch_boardconfig_t hal_switch__theboardconfig;


// - declaration of extern hidden macros ------------------------------------------------------------------------------    
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------






#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




