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
#ifndef _EOACTION_HID_H_
#define _EOACTION_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOaction_hid.h
    @brief      This header file implements hidden interface to an action object.
    @author     marco.accame@iit.it
    @date       08/04/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOVtask.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOaction.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/* @struct     EOaction_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/
struct EOaction_hid 
{
    eOactiontype_t      actiontype;
    union
    {
        struct 
        { 
            eOevent_t event; 
            EOVtaskDerived *totask; 
        } evt;
        
        struct 
        { 
            eOmessage_t message; 
            EOVtaskDerived *totask; 
        } msg;

        struct 
        { 
            eOcallback_t callback;
            void *argument;
            EOVtaskDerived *exectask; 
        } cbk;

    } data;
};   
 
#define EOACTION_DUMMY      { eo_actypeNONE, {0} }


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




