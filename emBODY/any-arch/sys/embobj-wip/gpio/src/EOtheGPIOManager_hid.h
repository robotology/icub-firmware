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
#ifndef _EOTHEGPIOMANAGER_HID_H_
#define _EOTHEGPIOMANAGER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOtheGPIOManager_hid.h
    @brief      This header file implements hidden interface to the base gpio manager singleton.
    @author     marco.accame@iit.it
    @date       08/24/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOlist.h"
#include "EOVmutex.h"
#include "EOioPin.h"
#include "EOtheGPIO.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheGPIOManager.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section



// - definition of the hidden struct implementing the object ----------------------------------------------------------


/* @struct     EOtheGPIOManager_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOtheGPIOManager_hid 
{
    EOlist                          *activegpioout;     /*< list of active out. it keeps only a ptr to the mngpin   */
    EOlist                          *activegpioinp;     /*< list of active inp. it keeps only a ptr to the mngpin   */
    EOVmutexDerived                 *mutex;             /*< mutex which guarantees exclusive access to the manager */
    uint8_t                         initted;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------


// used internally by some objects (managed pins)
extern eOresult_t eo_gpioman_hid_CanManagePin(EOtheGPIOManager* p, eOiopinDir_t dir);
// used internally by some objects (managed pins)
extern eOresult_t eo_gpioman_hid_ManagePin(EOtheGPIOManager* p, EOioPinDerived *mp, eOiopinDir_t dir);
// used internally by some objects (managed pins)
extern eOresult_t eo_gpioman_hid_IsPinManaged(EOtheGPIOManager* p, EOioPinDerived *mp, eOiopinDir_t dir);
// used internally by some objects (managed pins)
extern eOresult_t eo_gpioman_hid_UnManagePin(EOtheGPIOManager* p, EOioPinDerived *mp, eOiopinDir_t dir, uint8_t trigger);
// can be used to take the gpio timer, however, other eo_gpioman_* functions are already protected by concurrent access.
extern eOresult_t eo_gpioman_hid_Take(EOtheGPIOManager *p, eOreltime_t tout);
// see above
extern eOresult_t eo_gpioman_hid_Release(EOtheGPIOManager *p);



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




