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
#ifndef _EUPDATERINFO_H_
#define _EUPDATERINFO_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eupdater-info.h
    @brief      This header file implements ....
    @author     marco.accame@iit.it
    @date       01/11/2012
**/

/** @defgroup eupdaterinfo cedcew fcevw
    The embENV allows ...... 
 
    @todo acemor-facenda: do documentation.
    

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"

#include "ipal_base.h"
#include "EOMtheSystem.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//extern const eEmoduleInfo_t             eupdater_modinfo;
extern const eEmoduleExtendedInfo_t     eupdater_modinfo_extended;

extern const eOmsystem_cfg_t            eupdater_syscfg;

extern const ipal_cfg_t*  const         eupdater_ipal_cfg;


// - declaration of extern public functions ---------------------------------------------------------------------------

extern void eupdater_info_trace(const char *caller, char *format, ...);
 

/** @}            
    end of group eupdaterinfo 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


