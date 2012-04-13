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

/* @file       demo-mee-core-gpio-sm-appl.c
	@brief      This file implements a test for embobj
	@author     marco.accame@iit.it
    @date       06/21/2010
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#include "hal.h"
#include "osal.h"



// embobj
#include "EoCommon.h"
#include "EOaction.h"

#include "EOMmutex.h"
#include "EOVmutex.h"

#include "EOMtask.h"
#include "EOVtask.h"

#include "EOMtheSystem.h"
#include "EOVtheSystem.h"

#include "EOtimer.h"
#include "EOMtheTimerManager.h"
#include "EOMtheCallbackManager.h"
#include "EOVtheCallbackManager.h"

#ifdef _USE_GPIO_
#include "EOtheGPIO.h"
#include "eOcfg_GPIO_MCBSTM32c.h"
#include "EOMtheGPIOManager.h"

#include "EOioPinOutputManaged.h"
#include "EOioPinInputManaged.h"
#include "EOioPinOutput.h"
#endif//_GPIO_

#ifdef _USE_STATE_MACHINES_

#include "EOsm.h"
#include "EOumlsm.h"

#include "eOcfg_sm_Ex2.h"
#include "eOcfg_umlsm_Ex1.h"

#endif//_STATE_MACHINES_





// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "demo-mee-core-gpio-sm-appl.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

// must be extern to be visible in uv4
extern void task_example01(void *p);


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




static void s_mytmes_startup(EOMtask *p, uint32_t t);
static void s_mytmes_run(EOMtask *p, uint32_t t);

static void s_testeom_callback_button_wkup(void *arg);

static void s_testeom_callback_timer(void *arg);





     

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static EOaction * s_action;
EOMtask *mytask_message = NULL;

#ifdef _USE_GPIO_
EOioPinOutput *led = NULL;
#endif

#ifdef _USE_STATE_MACHINES_      

static EOsm *s_sm = NULL;
static EOumlsm *s_umlsm = NULL;
#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------





extern void demo_mee_core_gpio_sm_appl_init00(void)
{
    char str[128];

    s_action = eo_action_New(); 


    snprintf(str, sizeof(str)-1, "start a single user-task: msg-based\n\r");
    hal_trace_puts(str);
 
    mytask_message = eom_task_New(eom_mtask_MessageDriven, 68, 3*1024, s_mytmes_startup, s_mytmes_run,  4, eok_reltimeINFINITE, NULL, task_example01, "example01 msg");

    ///////////////////////////////////////////////////////////////////////////

 
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




static void s_mytmes_startup(EOMtask *p, uint32_t t)
{
EOtimer *tmr;
    char str[128];
#ifdef _USE_GPIO_
    EOioPinOutputManaged *om;
    EOioPinInputManaged *im;
#endif

    snprintf(str, sizeof(str)-1, "start a timer which every 5 seconds shall call a callback\n\r");
    hal_trace_puts(str);
    tmr = eo_timer_New();
    eo_action_SetCallback(s_action, s_testeom_callback_timer, NULL, eom_callbackman_GetTask(eom_callbackman_GetHandle()));

    eo_timer_Start(tmr, eok_abstimeNOW, 5*eok_reltime1sec, eo_tmrmode_FOREVER, s_action); 


#ifdef _USE_GPIO_
    // 1. inizializza il gpio manager

    snprintf(str, sizeof(str)-1,"init the EOtheGPIOManager configured for the mcbstm32c\n\r");
    hal_trace_puts(str);

    eo_gpio_Initialise(eo_cfg_gpio_mcbstm32c_Get());
    eom_gpioman_Initialise(eo_gpio_GetHandle() , NULL);

    // 2. inizializza una waveform  sul LED15: iopinID_Oman_mcbstm32c_LED_015

    snprintf(str, sizeof(str)-1, "start a waveform on LED015\n\r");

    om = eo_iopinoutman_GetHandle(iopinID_Oman_mcbstm32c_LED_015);
    eo_iopinoutman_Waveform_Start(om, eo_iopinvalLOW, eok_reltime1sec, 4*eok_reltime1sec, eok_reltimeINFINITE);


    // 3. metti una callback sul pin di ingresso: iopinID_Iman_mcbstm32c_BUTTON_WKUP

    snprintf(str, sizeof(str)-1, "register a callback on BUTTON_WKUP: called if pressed for 500 milli\n\r");
    hal_trace_puts(str);

    eo_action_SetCallback(s_action, s_testeom_callback_button_wkup, NULL, eom_callbackman_GetTask(eom_callbackman_GetHandle()));

    im = eo_iopininpman_GetHandle(iopinID_Iman_mcbstm32c_BUTTON_WKUP);
    eo_iopininpman_ActionOn_Register(im, s_action, eo_iopinTrig_OnRiseStay, 5*eok_reltime100ms);

    led = eo_iopinout_GetHandle(iopinID_Out_mcbstm32c_LED_009);
#endif//_GPIO_

#ifdef _USE_STATE_MACHINES_      

    snprintf(str, sizeof(str)-1, "init a EOsm\n\r");
    hal_trace_puts(str);

    s_sm = eo_sm_New(eo_cfg_sm_ex2_Get());

    eo_sm_Start(s_sm);


    snprintf(str, sizeof(str)-1, "init a EOumlsm\n\r");
    hal_trace_puts(str);

    s_umlsm = eo_umlsm_New(eo_cfg_umlsm_Ex1_Get());

    eo_umlsm_Start(s_umlsm);


#endif
       
}

static void s_mytmes_run(EOMtask *p, uint32_t t)
{
    eOmessage_t msg = (eOmessage_t)t;
    char str[128];

    uint8_t evt = 0;
    eOabstime_t time;

    snprintf(str, sizeof(str)-1, "task msg-based has received msg = 0x%x\n\r", msg);
    hal_trace_puts(str);

    if(0x10001000 == msg)
    {
#ifdef _USE_GPIO_
    snprintf(str, sizeof(str)-1, "the message if from the input button, thus i toggle a led\n\r");
    hal_trace_puts(str);
    eo_iopinout_ToggleVal(led);
#endif
    }


#ifdef _USE_STATE_MACHINES_

    time = eov_sys_LifeTimeGet(eov_sys_GetHandle());

    evt = (time >> 15) % 10;

    snprintf(str, sizeof(str)-1, "... ticked the EOsm with random event # %d\n\r", evt);
    hal_trace_puts(str);

    eo_sm_ProcessEvent(s_sm, evt);


    snprintf(str, sizeof(str)-1, "... ticked the EOumlsm with random event # %d\n\r", evt);
    hal_trace_puts(str);

    eo_umlsm_ProcessEvent(s_umlsm, evt, eo_umlsm_consume_UPTO08);



#endif

}

extern void task_example01(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
}



static void s_testeom_callback_button_wkup(void *arg)
{
    static uint32_t nn = 0;
    static eOmessage_t msg = 0x10001000;
    char str[128];

#ifdef _USE_GPIO_
    snprintf(str, sizeof(str)-1, "callback registered on BUTTON_WKUP has been called for the %d-th time\n\r", ++nn);
    hal_trace_puts(str);
    snprintf(str, sizeof(str)-1, "the callback has sent msg 0x%x to task msg-based\n\r", msg);
    hal_trace_puts(str);
#endif

    eom_task_SendMessage(mytask_message, msg, eok_reltimeINFINITE);
}


static void s_testeom_callback_timer(void *arg)
{
    static uint32_t nn = 0;
    static eOmessage_t msg = 0x10001001;
    char str[128];


    snprintf(str, sizeof(str)-1, "callback registered by timer has been called for the %d-th time\n\r", ++nn);
    hal_trace_puts(str);
    snprintf(str, sizeof(str)-1, "the callback has sent msg 0x%x to task msg-based\n\r", msg);
    hal_trace_puts(str);


    eom_task_SendMessage(mytask_message, msg, eok_reltimeINFINITE);
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



