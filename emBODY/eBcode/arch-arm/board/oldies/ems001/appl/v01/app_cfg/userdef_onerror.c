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

/* @file       userdef_onerror.c
	@brief      This file keeps the function user-defined invoked on application error
	@author     valentina.gaggero@iit.it
    @date       05/21/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------




#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#include "hal.h"
#include "hal_sys.h"

#include "EOMtheSystem.h"

#include "EOMtheEMSapplCfg.h"
#include "EOMtheEMSappl.h"







// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//static void error_base_fatal_manage(char *str);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//extern void ipal_cfg_on_fatal_error(ipal_fatalerror_t errorcode, const char * errormsg)
//{
//    char str[256];
//    EOMtheEMSappl* the_appl = eom_emsappl_GetHandle();
// 
//    #warning --> marco.accame: togliere controllo su the_appl e mettere un altro handler di errore in avvio 
//    // per errore fatale all'avvio: si blinkano tutti i led a 10 hz eccetto: led1 se hal, led2 se osal, led3 se ipal, led4 altrimenti
//    // per errore fatale nel error task: si blinkano tutti i led a 5 hz.
//   
//    if(NULL != the_appl)
//    {   
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.code             = eo_errman_code_sys_ipalerror;
//        errdes.param            = errorcode;
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        
//        snprintf(str, sizeof(str), "ipal_fatalerror_t %d: %s", errorcode, errormsg);

//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, str, "IPAL", &errdes);
//    }
//    else
//    {
//        snprintf(str, sizeof(str), "stack-IPAL: errorcode= %d: %s\n", errorcode, errormsg);
//        //error_base_fatal_manage(str);
//    }
//}



//extern void osal_cfg_on_fatal_error(void* task, osal_fatalerror_t errorcode, const char * errormsg)
//{
//    uint8_t tskid = 0;
//    char str[128];
//    
//    EOMtheEMSappl* the_appl = eom_emsappl_GetHandle();
//    osal_task_id_get(task, &tskid);
//    
//    
//    if(NULL != the_appl)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.code             = eo_errman_code_sys_osalerror;
//        errdes.param            = errorcode;
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//                
//        snprintf(str, sizeof(str), "error %d from taskid %d: %s\n\r", errorcode, tskid, errormsg);
//        
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, str, "OSAL", &errdes);
//    }
//    else
//    {
//        snprintf(str, sizeof(str), "OSAL: error %d from taskid %d: %s\n\r", errorcode, tskid, errormsg);
//        error_base_fatal_manage(str);
//    }
//}


//#warning --> marco.accame: attenzione a qui. ho rimossso questa funzione di errore.

////!!!!!!!PAY ATTENTION!!!!!
///* this function is used in case of error by ems application untile error-task is not initialized!! */
//extern void eom_emsapplcfg_hid_userdef_OnErrorOLD(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
//{
//    const char err[4][16] = {"info", "warning", "weak error", "fatal error"};
//    char str[256];
//
//
//    EOMtheEMSapplCfg *emsapplcfg = eom_emsapplcfg_GetHandle();
//
//
//    snprintf(str, sizeof(str), "userdef_OnError: [eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, err[(uint8_t)errtype], info);
//
//    if(errtype <= eo_errortype_error)
//    {
//        if(emsapplcfg->errmng_haltrace_enabled)
//        {
//            hal_trace_puts(str);
//        }
//        return;
//    }
//    
//    //i'm here only if errtype is fatal!!!
//    error_base_fatal_manage(str);
//
//}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//static void error_base_fatal_manage(char *str)
//{
//    hal_led_cfg_t cfg = {.dummy=0};
//    
//    hal_led_init(hal_led1, &cfg); 
//    hal_led_on(hal_led1);
//
//    hal_trace_puts(str);
//    
//    for(;;)
//    {
//        hal_led_on(hal_led1);
//        hal_sys_delay(300*1000);
//        hal_led_off(hal_led1);
//        hal_sys_delay(200*1000);
//    }
//
//}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



