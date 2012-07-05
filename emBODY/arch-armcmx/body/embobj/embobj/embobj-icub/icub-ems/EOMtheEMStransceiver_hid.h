/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _EOMTHEEMSTRANSCEIVER_HID_H_
#define _EOMTHEEMSTRANSCEIVER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheEMStransceiver_hid.h
    @brief      This header file implements hidden interface to the error manager
    @author     marco.accame@iit.it
    @date       10/15/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheBOARDtransceiver.h"
#include "EOtransceiver.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMStransceiver.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOMtheEMStransceiver_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
struct EOMtheEMStransceiver_hid 
{
    EOtransceiver*  transceiver;
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------

//extern eOboardtransceiver_cfg_t* eom_emstransceiver_hid_userdef_get_cfg(const eOemstransceiver_cfg_t *cfg);

//extern void eom_emstransceiver_hid_userdef_add_endpoints(eOboardtransceiver_cfg_t *brdcfg);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

