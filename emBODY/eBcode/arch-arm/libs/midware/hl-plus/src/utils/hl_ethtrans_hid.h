/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
#ifndef _HL_ETHTRANSCEIVER_HID_H_
#define _HL_ETHTRANSCEIVER_HID_H_


/* @file       hl_ethtrans_hid.h
    @brief      This header file implements hidden interface to ...
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       09/09/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------



// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "hl_ethtrans.h"



// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

#if 0
typedef hl_result_t (*hl_ethtrans_hid_fn_config_t) (hl_ethtrans_phymode_t targetphymode, hl_ethtrans_phymode_t* usedphymode);
typedef hl_result_t (*al_device_ethtrans_hid_fn_getphymode_t) (hl_ethtrans_phymode_t* usedphymode);
typedef struct
{   // used inside the public functions of hl_ethtrans to communicate to the chip, but defined inside brdcfg
    hl_res_fp_voidp_t                               init;
    void*                                           initpar;
    hl_ethtrans_hid_fn_config_t               config;
    al_device_ethtrans_hid_fn_getphymode_t    getphymode;
} hl_ethtrans_hid_chip_interface_t;


typedef struct
{   
    hl_ethtrans_phymode_t                                targetphymode;
    hl_ethtrans_hid_chip_interface_t          chipif;
} hl_ethtrans_hid_dev_cfg_t;

typedef struct
{
    hl_bool_t                                       supported;
    hl_ethtrans_hid_dev_cfg_t                 devcfg;
} hl_ethtrans_hid_brdcfg_t;

#endif

// - declaration of extern hidden variables ---------------------------------------------------------------------------
// empty-section


// - declaration of extern hidden macros ------------------------------------------------------------------------------    
// empty-section


// - declaration of extern hidden functions ---------------------------------------------------------------------------




#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




