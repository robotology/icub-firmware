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
#ifndef _EOPACKET_HID_H_
#define _EOPACKET_HID_H_


#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOpacket_hid.h
    @brief      This header file implements hidden interface to a packet object.
    @author     marco.accame@iit.it
    @date       0111/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOpacket.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOpacket_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOpacket_hid 
{
    eOipv4addr_t        remoteaddr;
    eOipv4port_t        remoteport;
    uint8_t             incomplete_flag;
    uint8_t             externaldatastorage; 
    uint16_t            size;
    uint16_t            capacity;
    uint16_t            write_index;
    uint16_t            read_index;
    uint8_t             *data;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern eOresult_t eo_packet_hid_DefInit(void *p, uint32_t a);

extern eOresult_t eo_packet_hid_DefCopy(void *d, void *s);

extern eOresult_t eo_packet_hid_DefClear(void *p);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




