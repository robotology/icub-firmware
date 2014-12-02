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
#ifndef _EOTRANSCEIVER_HID_H_
#define _EOTRANSCEIVER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOtransceiver_hid.h
    @brief      This header file implements hidden interface to a packet object.
    @author     marco.accame@iit.it
    @date       0111/2010
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOreceiver.h"
#include "EOtransmitter.h"
#include "EOVmutex.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtransceiver.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#define USE_DEBUG_EOTRANSCEIVER

// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    uint32_t    failuresinloadofreplyropframe;
    uint32_t    cannotloadropinregulars;
    uint32_t    cannotloadropinoccasionals;
} EOtransceiverDEBUG_t;

/** @struct     EOtransceiver_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOtransceiver_hid 
{
    eOtransceiver_cfg_t         cfg;
    EOreceiver*                 receiver;
    EOtransmitter*              transmitter;   
#if defined(USE_DEBUG_EOTRANSCEIVER)    
    EOtransceiverDEBUG_t        debug;
#endif    
    //EOVmutexDerived*            mtx_tx_replies;
    //EOVmutexDerived*            mtx_tx_regulars;
    //EOVmutexDerived*            mtx_tx_occasionals;
}; 



// - declaration of extern hidden functions ---------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




