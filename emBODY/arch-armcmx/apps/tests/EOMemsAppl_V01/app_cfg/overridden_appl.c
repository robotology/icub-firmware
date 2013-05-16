
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

#include "EOMtheEMSappl_hid.h"
#include "EOMtheEMSapplCfg.h"

#include "EOtheEMSapplBody.h"
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


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
extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p)
{
    EOMtheEMSapplCfg *appCfg =  eom_emsapplcfg_GetHandle();
    eo_emsapplBody_Initialise(&appCfg->applbodycfg);
    
}


extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p)
{
    char str[30];
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appCanSP_SetRunMode(appCanSP_ptr, eo_appCanSP_runMode__onEvent);
  
    eo_appCanSP_EmptyCanInputQueue(appCanSP_ptr, eOcanport1);
    eo_appCanSP_EmptyCanInputQueue(appCanSP_ptr, eOcanport2);
    snprintf(str, sizeof(str)-1, "userdef_on_entry_CFG"); 
    hal_trace_puts(str);
}

extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p)
{
    eOresult_t res;
    char str[30];
    res = eo_emsapplBody_EnableTxAllJointOnCan(eo_emsapplBody_GetHandle());
    if(eores_OK != res)
    {
        ; //gestisci errore
    }
    snprintf(str, sizeof(str)-1, "userdef_on_exit_CFG"); 
    hal_trace_puts(str);
}
extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p)
{
    char str[30];
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    //Before changing appCanRunMode is important be sure can output queues are empty
    eo_appCanSP_EmptyCanOutputQueue(appCanSP_ptr, eOcanport1);
    eo_appCanSP_EmptyCanOutputQueue(appCanSP_ptr, eOcanport2);
 
    eo_appCanSP_SetRunMode(eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle()), eo_appCanSP_runMode__onDemand);
    snprintf(str, sizeof(str)-1, "userdef_on_entry_RUN"); 
    hal_trace_puts(str);
}

extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p)
{
    eOresult_t res;
    char str[30];
    //set run mode on demand in order to send message to disable all joints on can
    eo_appCanSP_SetRunMode(eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle()), eo_appCanSP_runMode__onEvent);

    res = eo_emsapplBody_DisableTxAllJointOnCan(eo_emsapplBody_GetHandle());
    if(eores_OK != res)
    {
        ; //gestisci errore
    }
    snprintf(str, sizeof(str)-1, "userdef_on_exit_RUN"); 
    hal_trace_puts(str);
}

extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p)
{
    eo_appCanSP_SetRunMode(eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle()), eo_appCanSP_runMode__onEvent);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




