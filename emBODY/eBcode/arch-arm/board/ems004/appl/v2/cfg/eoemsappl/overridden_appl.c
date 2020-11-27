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

#include "hal_led.h"
#include "EOtheLEDpulser.h"
#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOVtheSystem.h"

#include "EOMtheEMSappl_hid.h"

#include "EOMtheEMSapplCfg.h"

#include "EOaction.h"
#include "EOtimer.h"

#include "EOMtheEMSapplCfg_cfg.h"

#include "EOtheCANservice.h"

#include "EOtheServices.h"
#include "EOtheMotionController.h"
#include "EOtheSTRAIN.h"
#include "EOtheMAIS.h"
#include "EOtheSKIN.h"
#include "EOtheInertials2.h"
#include "EOtheInertials3.h"
#include "EOtheTemperatures.h"
#include "EOthePSC.h"
#include "EOthePOS.h"
#include "EOtheETHmonitor.h"
#include "EOVtheCallbackManager.h"


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


// we need to define it ... even if it is useless. we shall remove it later on
const EOVtheEMSapplCfgBody theapplbodyconfig = 
{
    EO_INIT(.type)          0,
    EO_INIT(.thetrueconfig) NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_overridden_appl_led_error_init(void);

static void s_overridden_appl_initialise_services(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSappl";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// it is called by the init task before scheduling is enabled and before we activate the CFG-RUN-ERR state machine
extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p)
{   
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0); 
    
    // do whatever is needed to start services.
    s_overridden_appl_initialise_services();    
}


extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p)
{   
//    eo_errman_Trace(eo_errman_GetHandle(), "called: eom_emsappl_hid_userdef_on_entry_CFG()", s_eobj_ownname);
        
    // pulse led3 forever at 0.50 hz.       
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, 2*EOK_reltime1sec, 0);

    // EOtheCANservice: set straight mode and force parsing of all packets in the RX queues.
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
    eo_canserv_ParseAll(eo_canserv_GetHandle());
    
    // tell the ethmonitor to alert the task of the configurator
    eo_ethmonitor_SetAlert(eo_ethmonitor_GetHandle(), eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_userdef02);
    
    // prefer sending a tx request just in case. because cfg state transmit only if requested an we dont want to have missed a previous request.
    eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_ropframeTx);
}


extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p)
{
    // tell the ethmonitor to alert no task. the relevant on_entry_xxx function will set a new action
    eo_ethmonitor_SetAlert(eo_ethmonitor_GetHandle(), NULL, 0);
}


extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p)
{
//    eo_errman_Trace(eo_errman_GetHandle(), "called: eom_emsappl_hid_userdef_on_entry_RUN()", s_eobj_ownname);
    
    // pulse led3 forever at 1 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/1, 0);  
    
    // EOtheCANservice: set on demand mode. then tx all canframes remained in the tx queues
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_ondemand);    
    eo_canserv_TXstartAll(eo_canserv_GetHandle(), NULL, NULL);
    eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), 5*eok_reltime1ms);
    
    // tell the ethmonitor to alert no task, because the runner will tick it now at every cycle
    eo_ethmonitor_SetAlert(eo_ethmonitor_GetHandle(), NULL, 0);

    // we dont start services
}


extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p)
{       
    // EOtheCANservice: set straigth mode and force parsing of all packets in the RX queues.
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
    eo_canserv_ParseAll(eo_canserv_GetHandle());  
  
    
    // stop services which were started in on_entry_RUN()
    
    // motion-control
    eo_motioncontrol_Stop(eo_motioncontrol_GetHandle());
    eo_motioncontrol_Deactivate(eo_motioncontrol_GetHandle());
        
    // stop tx activity of services that may have been started by callback function
    
    // strain
    eo_strain_Stop(eo_strain_GetHandle());
    eo_strain_Deactivate(eo_strain_GetHandle());
        
    // skin
    eo_skin_Stop(eo_skin_GetHandle());
    eo_skin_Deactivate(eo_skin_GetHandle());
        
    // mais
    eo_mais_Stop(eo_mais_GetHandle());
    eo_mais_Deactivate(eo_mais_GetHandle());

    // inertials
    eo_inertials2_Stop(eo_inertials2_GetHandle());    
    eo_inertials2_Deactivate(eo_inertials2_GetHandle());
    
    // inertials3
    eo_inertials3_Stop(eo_inertials3_GetHandle());    
    eo_inertials3_Deactivate(eo_inertials3_GetHandle());
    
    // temperatures
    eo_temperatures_Stop(eo_temperatures_GetHandle());    
    eo_temperatures_Deactivate(eo_temperatures_GetHandle());    
    
    // psc
    eo_psc_Stop(eo_psc_GetHandle());
    eo_psc_Deactivate(eo_psc_GetHandle());
    
    // pos
    eo_pos_Stop(eo_pos_GetHandle());
    eo_pos_Deactivate(eo_pos_GetHandle());
}

extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p)
{
//    eo_errman_Trace(eo_errman_GetHandle(), "called: eom_emsappl_hid_userdef_on_entry_ERR()", s_eobj_ownname);

    // pulse led3 forever at 4 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/4, 0);
   

    // EOtheCANservice: set straight mode and force parsing of all packets in the RX queues.
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
    eo_canserv_ParseAll(eo_canserv_GetHandle());  

    
    
    // stop services which may have been started 
    
    // motion-control
    eo_motioncontrol_Stop(eo_motioncontrol_GetHandle());
    eo_motioncontrol_Deactivate(eo_motioncontrol_GetHandle());
        
    // stop tx activity of services that may have been started by callback function
    
    // strain
    eo_strain_Stop(eo_strain_GetHandle());
    eo_strain_Deactivate(eo_strain_GetHandle());
        
    // skin
    eo_skin_Stop(eo_skin_GetHandle());
    eo_skin_Deactivate(eo_skin_GetHandle());
        
    // mais
    eo_mais_Stop(eo_mais_GetHandle());
    eo_mais_Deactivate(eo_mais_GetHandle());

    // inertials
    eo_inertials2_Stop(eo_inertials2_GetHandle());
    eo_inertials2_Deactivate(eo_inertials2_GetHandle());
    
    // inertials3
    eo_inertials3_Stop(eo_inertials3_GetHandle());
    eo_inertials3_Deactivate(eo_inertials3_GetHandle());    
    
    // temperatures
    eo_temperatures_Stop(eo_temperatures_GetHandle());    
    eo_temperatures_Deactivate(eo_temperatures_GetHandle()); 
    
    // psc
    eo_psc_Stop(eo_psc_GetHandle());
    eo_psc_Deactivate(eo_psc_GetHandle());
    
    // pos
    eo_pos_Stop(eo_pos_GetHandle());
    eo_pos_Deactivate(eo_pos_GetHandle());    
}



// marco.accame on Nov 26 2014
// this is the function used by EOtheErrorManager from start-up until redefinition done by the EOMtheEMSappl 
// in this first phase, the EOtheErrorManager shall just print a string into the trace port and in case of a fatal error 
// it stops execution and start blinking leds
// in the second phase, the EOtheEMSappl redefines begaviours so that error messages are inserted into a sig<> ROP which 
// is sent to the remote host inside the standard EOMtheEMSsocket


// this function is exactly the same as the default one except for the blinking of leds

extern void eom_emsapplcfg_hid_userdef_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    const char empty[] = "EO?";
    const char *err = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
    char str[128];
    EOMtheEMSapplCfg *emsapplcfg = eom_emsapplcfg_GetHandle();
    
    const char *eobjstr = (NULL != caller) ? (caller->eobjstr) : (empty);
    uint32_t taskid = (NULL != caller) ? (caller->taskid) : (0);

    if(emsapplcfg->errmng_haltrace_enabled)
    {
        uint64_t tt = eov_sys_LifeTimeGet(eov_sys_GetHandle());
        uint32_t sec = tt/(1000*1000);
        uint32_t tmp = tt%(1000*1000);
        uint32_t msec = tmp / 1000;
        uint32_t usec = tmp % 1000;
                
        snprintf(str, sizeof(str), "[%s] @ s%d m%d u%d %s in tsk %d: %s", err, sec, msec, usec, eobjstr, taskid, info);
        hal_trace_puts(str);
    }
    if(errtype <= eo_errortype_error)
    {
        return;
    }
    
    eov_sys_Stop(eov_sys_GetHandle());
    
    // init leds
    s_overridden_appl_led_error_init();
    
    // compute the mask of led to be toggled.
    uint8_t ledmask = 0xff;    
    if((NULL != caller) && (NULL != caller->eobjstr))
    {
        if(0 == strcmp(caller->eobjstr, "HAL"))
        {
            eo_common_byte_bitclear(&ledmask, 0);           
        }
        else if(0 == strcmp(caller->eobjstr, "OSAL"))
        {
            eo_common_byte_bitclear(&ledmask, 1);  
        }
        else if(0 == strcmp(caller->eobjstr, "IPAL"))
        {
            eo_common_byte_bitclear(&ledmask, 2);  
        }
    }

    for(;;)
    {
        hal_sys_delay(100);
        
        if(eobool_true == eo_common_byte_bitcheck(ledmask, 0))
        {
            hal_led_toggle(hal_led0);
        }
        if(eobool_true == eo_common_byte_bitcheck(ledmask, 1))
        {
            hal_led_toggle(hal_led1);
        }
        if(eobool_true == eo_common_byte_bitcheck(ledmask, 2))
        {            
            hal_led_toggle(hal_led2);
        }
        
        hal_led_toggle(hal_led3);
        hal_led_toggle(hal_led4);
        hal_led_toggle(hal_led5);        
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_overridden_appl_led_error_init(void)
{
    // marco.accame: we init all the leds and we switch them off
    hal_led_init(hal_led0, NULL);
    hal_led_init(hal_led1, NULL);
    hal_led_init(hal_led2, NULL);
    hal_led_init(hal_led3, NULL);
    hal_led_init(hal_led4, NULL);
    hal_led_init(hal_led5, NULL);
    
    hal_led_off(hal_led0);
    hal_led_off(hal_led1);
    hal_led_off(hal_led2);
    hal_led_off(hal_led3);
    hal_led_off(hal_led4);
    hal_led_off(hal_led5);   
}



static void s_overridden_appl_initialise_services(void)
{    
    eOipv4addr_t ipaddress = eom_ipnet_GetIPaddress(eom_ipnet_GetHandle());
    // if we want to use another board ... uncomment the following for board 10.0.1.2 ... or change to other address.
    // ipaddress = eo_common_ipv4addr(10, 0, 1, 2);

    // initialise services ...
    eo_services_Initialise(ipaddress);
    
    // and start them on the basis of the boardnumber 
    // ..... DONT DO IT ANYMORE because now we use runtime configuration.
    // eo_services_Start(eo_services_GetHandle());
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------









