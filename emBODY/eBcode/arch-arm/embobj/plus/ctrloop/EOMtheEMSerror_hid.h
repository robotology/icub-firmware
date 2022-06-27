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
#ifndef _EOMTHEEMSERROR_HID_H_
#define _EOMTHEEMSERROR_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheEMSerror_hid.h
    @brief      This header file implements hidden interface to the error manager
    @author     marco.accame@iit.it
    @date       10/15/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EOMtask.h"
#include "EOpacket.h"
#include "EOtimer.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMSerror.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOMtheEMSerror_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
struct EOMtheEMSerror_hid 
{
    EOMtask*                task;
    EOtimer*                timer;
    uint16_t                numberoffatalerrors;
    eOerrmanDescriptor_t    errordescriptor;
    eOerrmanDescriptor_t    latesterrordesc;
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern void eom_emserror_hid_userdef_DoJustAfterPacketReceived(EOMtheEMSerror *p, EOpacket *rxpkt);

extern void eom_emserror_hid_userdef_OnRXuserdefevent(eOemserror_event_t evt);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

