/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOTHECANMAPPING_HID_H_
#define _EOTHECANMAPPING_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOtheCANmapping_hid.h
    @brief      This header file implements hidden interface to the can mapping 
    @author     marco.accame@iit.it
    @date       12 mar 2015
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheCANmapping.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOtheCANmapping_hid
    @brief      Hidden definition. 
 **/  
struct EOtheCANmapping_hid 
{
	eOcanmap_cfg_t                  config;
    eOcanmap_board_extended_t***    canmapping;         // [can][adr] -> pointer to 
    eOcanmap_board_extended_t****   entitylocation;     // [ep][ent][index]-> pointer to
    eOcanmap_board_extended_t***    skinlocation;       // [index]-> array[] of up to 8 pointers to 
    uint8_t                         numofskinboardsindex[eocanmap_skins_maxnumberof];
//    eOcanmap_arrayof_locations_t    arrayofboardlocations;
    uint32_t                        tobedefined;
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

