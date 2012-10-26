
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"
#include "EOMtask.h"

#include "EOMtheEMSappl.h"
#include "EOtheEMSApplBody.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
eOboolvalues_t go2run_received = eobool_false;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eom_emsconfigurator_hid_userdef_DoJustAfterPacketParsing(EOMtheEMSconfigurator *p)
{
    eOresult_t  res;
    uint8_t     numofRXcanframe = 0;
    EOappCanSP  *appcanSP = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
#warning VALE --> read or not read can frame in configuration state??    
//     res = eo_appCanSP_GetNumOfRecCanframe(appcanSP, eOcanport1, &numofRXcanframe);
//     if(eores_OK != res)
//     {
//         return;
//     }
//     eo_appCanSP_read(appcanSP, eOcanport1, numofRXcanframe, NULL);

//     
//     res = eo_appCanSP_GetNumOfRecCanframe(appcanSP, eOcanport2, &numofRXcanframe);
//     if(eores_OK != res)
//     {
//         return;
//     }
//     eo_appCanSP_read(appcanSP, eOcanport2, numofRXcanframe, NULL);
    
    if(eobool_true == go2run_received)
    {
            eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_go2runner);
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




