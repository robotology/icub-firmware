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

/* @file       eom_emsappl_main.cpp
	@brief      This file keeps the main of an application on ems using the embobj
	@author     marco.accame@iit.it
    @date       05/21/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------




#include "stdint.h"
#include "stdlib.h"
#include "string.h"



#include "EOMtheSystem.h"

#include "EOMtheEMSapplCfg.h"
#include "EOMtheEMSappl.h"



#include "EOtheLEDpulser.h"

#include "EoError.h"
#include "EOtheErrorManager.h"



#include "EOtheCANmapping.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------


 
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

static void s_eom_emsappl_main_init(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_Initialise();

    eom_sys_Initialise( &emscfg->wsyscfg.msyscfg, 
                        &emscfg->wsyscfg.mempoolcfg,                         
                        &emscfg->wsyscfg.errmancfg,                 
                        &emscfg->wsyscfg.tmrmancfg, 
                        &emscfg->wsyscfg.cbkmancfg 
                      );  
    
    eom_sys_Start(eom_sys_GetHandle(), s_eom_emsappl_main_init);

}



extern void eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived(EOreceiver *receiver)
{   // the only reason of using the following two variables as static is: to reduce the use of stack.
    static int64_t delta = 0;
    static eOerrmanDescriptor_t errdes = 
    {
        EO_INIT(.code)          EOERRORCODE(eoerror_category_System, eoerror_value_SYS_transceiver_rxseqnumber_error),
        EO_INIT(.sourcedevice)  eo_errman_sourcedevice_localboard,
        EO_INIT(.sourceaddress) 0,
        EO_INIT(.par16)         0,
        EO_INIT(.par64)         0
    };
    
    const eOreceiver_seqnum_error_t *err = eo_receiver_GetSequenceNumberError(receiver); 
    
    if(NULL == err)
    {
        errdes.code  = EOERRORCODE(eoerror_category_System, eoerror_value_SYS_runtimeerror);
        errdes.par64 = 0x123467fabc222;
        errdes.par16 = 1;       
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &errdes);   
        return;        
    }
    
    if(1 == err->rec_seqnum)
    {
        // it is the first packet received from a remote transmitter freshly initted (i.e., robotInterface has just re-started bu this board was alive well before)
        // thus, we dont issue an error but an info: 
        errdes.code  = EOERRORCODE(eoerror_category_System, eoerror_value_SYS_transceiver_rxseqnumber_restarted);
        errdes.par64 = err->exp_seqnum;
        errdes.par16 = 1;       
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes);   
        
        // ok, now we must set back the error code as it is normal.
        errdes.code = EOERRORCODE(eoerror_category_System, eoerror_value_SYS_transceiver_rxseqnumber_error);
    }  
    else
    {    
        delta = err->rec_seqnum - err->exp_seqnum;
        if(delta > EO_INT16_MAX)       delta = EO_INT16_MAX;  //32767
        else if(delta < EO_INT16_MIN)  delta = EO_INT16_MIN;  //-32768;
        
        errdes.code             = EOERRORCODE(eoerror_category_System, eoerror_value_SYS_transceiver_rxseqnumber_error);
        errdes.par16            = (int16_t)delta; 
        errdes.par64            = err->exp_seqnum; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
    }
}


extern void eom_emstransceiver_callback_incaseoferror_invalidframe(EOreceiver *receiver)
{  
    static eOerrmanDescriptor_t errdesinvframe = 
    {
        EO_INIT(.code)          EOERRORCODE(eoerror_category_System, eoerror_value_SYS_transceiver_rxinvalidframe_error),
        EO_INIT(.sourcedevice)  eo_errman_sourcedevice_localboard,
        EO_INIT(.sourceaddress) 0,
        EO_INIT(.par16)         0,
        EO_INIT(.par64)         0
    };

    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdesinvframe);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//#define TESTOS2

#if defined(TESTOS2)

EOMtask * tskevt = NULL;
EOMtask * tskmsg = NULL;

#define PERIODIC 1   
#if (1 == PERIODIC) 
// does not work. maybe timers
void s_testperiodic(void *p)
{
     eom_task_Start((EOMtask*)p);
}
static void s_testperiodic_startup(EOMtask *rt, uint32_t n)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called s_testperiodic_startup()", "TESTOS2");      
}

static void s_testperiodic_run(EOMtask *rt, uint32_t n)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called s_testperiodic_run()", "TESTOS2");     
}
#endif // PERIODIC


#define EVENT 0
// event ok ...  
#if (1 == EVENT)
void s_testevent(void *p)
{
     eom_task_Start((EOMtask*)p);
}

static void s_testevent_startup(EOMtask *rt, uint32_t n)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called s_testevent_startup()", "TESTOS2");      
}

static void s_testevent_run(EOMtask *rt, uint32_t n)
{
    char str[96];
    snprintf(str, sizeof(str), "called s_testevent_run() w/  n = %d", n);
    eo_errman_Trace(eo_errman_GetHandle(), str, "TESTOS2");     
}
#endif // EVENT

// timer is ik
#define TIMER 1   
#if (1 == TIMER) 
uint32_t s_testtimer_counter = 0;
void s_testtimer_callback(osal_timer_t* tmr, void* par)
{
    uint32_t *v = (uint32_t*)par;
    (*v)++;
    char str[96];
    snprintf(str, sizeof(str), "timer triggered value = %d", *v);
    eo_errman_Trace(eo_errman_GetHandle(), str, "xxxx()"); 
    
    if(NULL != tskevt)
    {
        eo_errman_Trace(eo_errman_GetHandle(), "and sending event", "xxx()");
        eom_task_SetEvent(tskevt, 3);
    }
    
    if(NULL != tskmsg)
    {
        eo_errman_Trace(eo_errman_GetHandle(), "and sending message number", "xxx()");
        eom_task_SendMessage(tskmsg, *v, 200*1000);
        eo_errman_Trace(eo_errman_GetHandle(), "and sending message pointer", "xxx()");
        eom_task_SendMessage(tskmsg, (uint32_t)v, 200*1000);        
    }
}
#endif // TIMER


#define MESSAGE 1
// event ok ...  
#if (1 == MESSAGE)
void s_testmessage(void *p)
{
     eom_task_Start((EOMtask*)p);
}

static void s_testmessage_startup(EOMtask *rt, uint32_t n)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called s_testmessage_startup()", "TESTOS2");      
}

static void s_testmessage_run(EOMtask *rt, uint32_t n)
{
    char str[96];
    snprintf(str, sizeof(str), "called s_testmessage_run() w/  n = %d", n);
    eo_errman_Trace(eo_errman_GetHandle(), str, "TESTOS2");     
}
#endif // EVENT

#endif // TESTOS2


/** @fn         static void s_eom_emsappl_main_init(void)
    @brief      It initialises the emsappl 
    @details    bla bla bla.
 **/

static void s_eom_emsappl_main_init(void)
{  
    char str[96];
    snprintf(str, sizeof(str), "just started the user-defined function s_eom_emsappl_main_init()");
    eo_errman_Trace(eo_errman_GetHandle(), str, "eom_sys_Start()");  

#if defined(TESTOS2)
    
// periodic fails ...   
#if (1 == TIMER)    
    osal_timer_timing_t tm;
    osal_timer_onexpiry_t onexp;
    
    tm.count = 1000*eok_reltime1ms;
    tm.mode = osal_tmrmodeFOREVER;
    tm.startat = eok_abstimeNOW;
    
    onexp.cbk = s_testtimer_callback;
    onexp.par = &s_testtimer_counter;
    
    osal_timer_t *ot = osal_timer_new();
    osal_timer_start(ot, &tm, &onexp, osal_callerAUTOdetect); 
#endif
 
// periodic ok ...   
#if (1 == PERIODIC)    
    eom_task_New(   eom_mtask_Periodic, 
                    30, //prio
                    4*1024, // cfg->stacksize,
                    s_testperiodic_startup, s_testperiodic_run,
                    0, 
                    1000*eok_reltime1ms, // cfg->taskperiod,
                    NULL, 
                    s_testperiodic,
                    "testperiodic"
                    );    
#endif


// event ok ...  
#if (1 == EVENT)
    tskevt = eom_task_New(   eom_mtask_EventDriven, 
                    31, //cfg->priority, 
                    4*1024, //cfg->stacksize,
                    s_testevent_startup, s_testevent_run,
                    0, 
                    5000*eok_reltime1ms, // timeout,
                    NULL, 
                    s_testevent,
                    "testevent"
                    );
#endif

// event ok ...  
#if (1 == MESSAGE)
    tskmsg = eom_task_New(   eom_mtask_MessageDriven, 
                    33, //cfg->priority, 
                    4*1024, //cfg->stacksize,
                    s_testmessage_startup, s_testmessage_run,
                    8, //  maxmessages
                    5000*eok_reltime1ms, // timeout,
                    NULL, 
                    s_testmessage,
                    "testmessage"
                    );
#endif


#else    
    
    snprintf(str, sizeof(str), "s_eom_emsappl_main_init() is about to init EOtheLEDpulser");
    eo_errman_Trace(eo_errman_GetHandle(), str, "eom_sys_Start()");     
    
    // init leds via the EOtheLEDpulser object
    eOledpulser_cfg_t ledpulsercfg =  
    {
        EO_INIT(.led_enable_mask)   (1 << eo_ledpulser_led_zero | 1 << eo_ledpulser_led_one | 1 << eo_ledpulser_led_two | 1 << eo_ledpulser_led_three | 1 << eo_ledpulser_led_four | 1 << eo_ledpulser_led_five),
        EO_INIT(.led_init)          (eOint8_fp_uint8_cvoidp_t) hal_led_init,
        EO_INIT(.led_on)            (eOint8_fp_uint8_t) hal_led_on,
        EO_INIT(.led_off)           (eOint8_fp_uint8_t) hal_led_off,
        EO_INIT(.led_toggle)        (eOint8_fp_uint8_t) hal_led_toggle
    };

    eo_ledpulser_Initialise(&ledpulsercfg);
    
    
    eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_one);
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_five, 2000*eok_reltime1ms, 0);

    
    // init the ems application
//    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();    
//    eom_emsappl_Initialise(&emscfg->applcfg);

#endif
    
    snprintf(str, sizeof(str), "quitting the user-defined function s_eom_emsappl_main_init()");
    eo_errman_Trace(eo_errman_GetHandle(), str, "eom_sys_Start()");  
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



