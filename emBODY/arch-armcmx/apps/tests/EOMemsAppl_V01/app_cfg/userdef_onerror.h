/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _USERDEF_ONERROR_H_
#define _USERDEF_ONERROR_H_




/** @file       userdef_onerror.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       25/07/2013
**/

/** @defgroup aaaaa.......
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------
#include "EOtheErrorManager.h"
#include "ipal.h"



// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------
extern void ipal_cfg_on_fatal_error(ipal_fatalerror_t errorcode, const char * errormsg);

extern void osal_cfg_on_fatal_error(void* task, osal_fatalerror_t errorcode, const char * errormsg);

extern void eom_emsapplcfg_hid_userdef_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);
/** @}            
    end of group eo_icubCanProto 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




