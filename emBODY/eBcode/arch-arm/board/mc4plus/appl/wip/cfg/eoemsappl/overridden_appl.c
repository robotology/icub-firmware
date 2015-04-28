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

#include "EOVtheSystem.h"

#include "EOMtheEMSappl_hid.h"
#include "EOMtheEMSapplCfg.h"


#include "EOaction.h"
#include "hl_bits.h"

#include "EOMtheEMSapplCfg_cfg.h"


#include "EOtheServices.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//joints configuration
#if defined (EOMTHEEMSAPPLCFG_USE_EB15)

#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
static const eOserv_cfg_t theservicescfg =
{  .whatever = 0
    /*
    .enc_reader_cfg =
    {   
        .joints =
        {
#if defined(joint0_enc_primary_type)
            {
                .primary_encoder        = joint0_enc_primary_type, 
                .extra_encoder          = joint0_enc_extra_type,
                .primary_enc_position   = joint0_enc_primary_position,
                .extra_enc_position     = joint0_enc_extra_position,
            },
#else
            {
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE,
            },
#endif
#if defined(joint1_enc_primary_type)
            {
                .primary_encoder        = joint1_enc_primary_type, 
                .extra_encoder          = joint1_enc_extra_type,
                .primary_enc_position   = joint1_enc_primary_position,
                .extra_enc_position     = joint1_enc_extra_position,
            },
#else
            {
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE,
            },
#endif


#if defined(joint2_enc_primary_type)
            {
                .primary_encoder        = joint2_enc_primary_type, 
                .extra_encoder          = joint2_enc_extra_type,
                .primary_enc_position   = joint2_enc_primary_position,
                .extra_enc_position     = joint2_enc_extra_position,
            },
#else
            {
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE,
            },
#endif                
            
            
#if defined(joint3_enc_primary_type)
            {
                .primary_encoder        = joint3_enc_primary_type, 
                .extra_encoder          = joint3_enc_extra_type,
                .primary_enc_position   = joint3_enc_primary_position,
                .extra_enc_position     = joint3_enc_extra_position,
            },
#else
            {
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE,
            },
#endif                                
            
            
#if defined(joint4_enc_primary_type)
            {
                .primary_encoder        = joint4_enc_primary_type, 
                .extra_encoder          = joint4_enc_extra_type,
                .primary_enc_position   = joint4_enc_primary_position,
                .SPI_stream_position    = joint4_enc_extra_position,
            },
#else
            {
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE,
            },
#endif                   
            
#if defined(joint5_enc_primary_type)
            {
                .primary_encoder        = joint5_enc_primary_type, 
                .extra_encoder          = joint5_enc_extra_type,
                .primary_enc_position   = joint5_enc_primary_position,
                .extra_enc_position     = joint5_enc_extra_position,
            }
#else
            {
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE,
            }
#endif    
        },
        .SPI_streams =
        {   
            {   // stream 0
                .type       = encstream0_type,
                .numberof   = encstream0_numberof,
            },
            {   // stream 1
                .type       = encstream1_type,
                .numberof   = encstream1_numberof,
            }            
        },
        .SPI_callbackOnLastRead = NULL,
        .SPI_callback_arg       = NULL
    }
    */
};

const EOVtheEMSapplCfgBody theapplbodyconfig = 
{
    .type               =   0,
    .thetrueconfig      =   (void*) &theservicescfg
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void overridden_appl_led_error_init(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static char debug_string[128];

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// marco.accame on Nov 26 2014
// this is the function used by EOtheErrorManager from startup until redefinition done by the EOMtheEMSappl 
// in this first phase, the EOtheErrorManager shall just print a string into the trace port and in case of a fatal error 
// it stops execution and start blinking leds
// in the second phase, the EOtheEMSappl redefines begaviours so that error messages are inserted into a sig<> ROP which 
// is sent to the remote host inside the standard EOMtheEMSsocket


// this funtion is exactly the same as the default one except for the blinking of leds

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
        snprintf(str, sizeof(str), "[%s] %s in tsk %d: %s", err, eobjstr, taskid, info);
        hal_trace_puts(str);
    }
    if(errtype <= eo_errortype_error)
    {
        return;
    }
    
    eov_sys_Stop(eov_sys_GetHandle());
    
    // init leds
    overridden_appl_led_error_init();
    
    // compute the mask of led to be toggled.
    uint8_t ledmask = 0xff;    
    if((NULL != caller) && (NULL != caller->eobjstr))
    {
        if(0 == strcmp(caller->eobjstr, "HAL"))
        {
            hl_bits_byte_bitclear(&ledmask, 0);           
        }
        else if(0 == strcmp(caller->eobjstr, "OSAL"))
        {
            hl_bits_byte_bitclear(&ledmask, 1);  
        }
        else if(0 == strcmp(caller->eobjstr, "IPAL"))
        {
            hl_bits_byte_bitclear(&ledmask, 2);  
        }
    }

    for(;;)
    {
        hal_sys_delay(100);
        
        if(hl_true == hl_bits_byte_bitcheck(ledmask, 0))
        {
            hal_led_toggle(hal_led0);
        }
        if(hl_true == hl_bits_byte_bitcheck(ledmask, 1))
        {
            hal_led_toggle(hal_led1);
        }
        if(hl_true == hl_bits_byte_bitcheck(ledmask, 2))
        {            
            hal_led_toggle(hal_led2);
        }
        
        hal_led_toggle(hal_led3);
        hal_led_toggle(hal_led4);
        hal_led_toggle(hal_led5);        
    }
}


// marco.accame: this function is called inside eom_emsappl_Initialise() just before to run the state machine
// which enters in the CFG state. it is the place where to launch new services.

extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p)
{  
    EOMtheEMSapplCfg* emsapplcfg = eom_emsapplcfg_GetHandle();
    // the led-pulser is initted as first thing
     
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);

    // start the application body   
    //const eOemsapplbody_cfg_t *applbodycfg = &theemsapplbodycfg;   
    //const eOemsapplbody_cfg_t * applbodycfg   = (const eOemsapplbody_cfg_t *)emsapplcfg->applbodycfg->thetrueconfig;
    //eo_emsapplBody_Initialise(applbodycfg);   
    #warning -> marco.accame: put in here the main builder of the application EOapplication    
    const eOserv_cfg_t * servicescfg   = (const eOserv_cfg_t *)emsapplcfg->applbodycfg->thetrueconfig;
    //so far is doing nothing, so we can pass NULL
    eo_serv_Initialise(NULL);
    
    //configuration & init for motion control service
    eOmcserv_cfg_t mcconfig = {0};
    mcconfig.jomosnumber  = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    mcconfig.type         = eomcserv_type_mc4plus;
  
    //memcpy(&mcconfig.encoder_reader_cfg, &servicescfg->enc_reader_cfg, sizeof(eOappEncReader_cfg_t)); 
    
    // done inside the inizializer of the motion control service...or not?
    // need to clarify
    /*
    // we do it for board 13 in its simplified form: two joints
    // jomo 0
    mcconfig.jomos[0].actuator.any.type = 1;        // on board
    mcconfig.jomos[0].actuator.local.index = 0;  
    mcconfig.jomos[0].encoder.etype = 0;            // aea 
    mcconfig.jomos[0].encoder.index = 0;            // joint index inside the EoappEncoderReader joints array
    // jomo 1
    mcconfig.jomos[1].actuator.any.type = 1;        // on board
    mcconfig.jomos[1].actuator.local.index = 1;   
    mcconfig.jomos[1].encoder.etype = 0;            // aea 
    mcconfig.jomos[1].encoder.index = 1;            // joint index inside the EoappEncoderReader joints array    
    */
    
    // example for board 15 (MC4 Plus JIG) --> one joint controlled by one AEA encoder
    mcconfig.jomos[0].actuator.local.type   = 1;        // on board
    mcconfig.jomos[0].actuator.local.index  = 2;        // motor 2
    mcconfig.jomos[0].encoder.etype         = 0;        // aea
    mcconfig.jomos[0].encoder.index         = 0;        // position index for low level mapping (hal)
    //mcconfig.jomos[0].encoder.etype         = 2;        // incremental
    //mcconfig.jomos[0].encoder.index         = 2;        // position index for low level mapping (hal)
    
    //inside the MC initializer we also define the encoder reader configuration, using jomos info
    eo_serv_ConfigMC(eo_serv_GetHandle(), &mcconfig);  

    eo_mcserv_CheckResources(eo_mcserv_GetHandle());

    //for test we could also send a tick event to the configurator...after 10 seconds it must go to RUN
    //Test -- begin
    /*
    EOaction_strg astg = {0};
    EOaction *action = (EOaction*)&astg;
    EOtimer *tick_timer;
    tick_timer = eo_timer_New();
    eo_action_SetEvent(action, emsconfigurator_evt_userdef01, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    eo_timer_Start(tick_timer, eok_abstimeNOW, 250*eok_reltime1ms, eo_tmrmode_FOREVER, action);
    */
    //Test -- end
}


extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p)
{ 
    // pulse led3 forever at 0.50 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, 2*EOK_reltime1sec, 0);
    
    #warning -> TBD: if CAN is available, add eo_canserv_SetMode(onevent), eo_canserv_Empty_TXqueues()
    uint32_t time_ms = osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "t: %dms, Entering in CFG state",time_ms);
    hal_trace_puts (debug_string);    
}

extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p)
{
    uint32_t time_ms = osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "t: %dms, Exiting from CFG state",time_ms);
    hal_trace_puts (debug_string);    
}

extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p)
{
    // pulse led3 forever at 1 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/1, 0); 
    
    
    eo_mcserv_Start(eo_mcserv_GetHandle());
    
    #warning -> TBD: add an enable of motion control loop, such as eo_mcserv_Start() [it enables the tx of all joints on can, ... or else for mc4plus it also starts enconders]
    #warning -> TBD: if CAN is available, add eo_canserv_SetMode(ondemand), eo_canserv_Empty_TXqueues()
    // also start strain
    
    uint32_t time_ms = osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "t: %dms, Entering in RUN state",time_ms);
    hal_trace_puts (debug_string);     
}

extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p)
{

    eo_mcserv_Stop(eo_mcserv_GetHandle());
    
    #warning -> TBD: if CAN is available, add eo_canserv_SetMode(onevent), eo_canserv_Empty_TXqueues()
    #warning -> TBD: if SKIN is available, add eo_skinserv_Stop()
    #warning -> TBD: if MC is available, add eo_mcserv_Stop() [ its disables the joints on can, etc.].
    // also stop strain
    
    uint32_t time_ms = osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "t: %dms, Exiting from RUN state",time_ms);
    hal_trace_puts (debug_string);     
}

extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p)
{
    // pulse led3 forever at 4 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/4, 0);
   
    #warning -> TBD: if CAN is available, add eo_canserv_SetMode(onevent), eo_canserv_Empty_TXqueues()
        
    uint32_t time_ms = osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "tms: %d, Entering in ERR state",time_ms);
    hal_trace_puts (debug_string);     
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void overridden_appl_led_error_init(void)
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


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


