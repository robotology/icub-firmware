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
#ifndef _EOSTHEFOOP_HID_H_
#define _EOSTHEFOOP_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOStheFOOP_hid.h
    @brief      This header file implements hidden interface to the foop singleton.
    @author     marco.accame@iit.it
    @date       08/04/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOVtask_hid.h" 
#include "EOfifoWord.h"
#include "EOfifo.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOStheFOOP.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section



// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOStheFOOP_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
struct EOStheFOOP_hid
{
    // base object
    EOVtask                         *tsk;

    // other stuff
    eOsfoop_cfg_t                   cfg;
    eOsfoop_hal_fn_t                hfn;
    eOvoid_fp_void_t                ontick;
    volatile uint32_t               flags;
    volatile uint32_t               events_mask;
    EOfifoWord                      *message_fifo;
    EOfifo                          *callback_fifo;        
    EOfifo                          *argument_fifo;  
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------


extern eOresult_t eos_foop_hid_SetOnTick(EOStheFOOP *p, eOvoid_fp_void_t ontick);

extern eOresult_t eos_foop_hid_Tick(EOStheFOOP *p);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

