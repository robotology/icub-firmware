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


#include "EOnvSet.h"
#include "EoProtocolMC.h"

#include "EOtheEntities.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// - now we specialise the eth protocol for each board

//#if     (15 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)     

//    #define NJOMO 1
//    static const eOprot_EPcfg_t s_theEPcfgsOthers[] =
//    {  
//        {           
//            .endpoint           = eoprot_endpoint_motioncontrol,
//            .numberofentities  = {NJOMO, NJOMO, 1, 0, 0, 0, 0}     
//        }    
//    };
//    
//#else
//    #error -> must specify a board number   
//#endif



//static const uint8_t s_boardnum = EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD - 1;
//static const uint8_t s_theEPcfgsOthers_NumberOf = sizeof(s_theEPcfgsOthers) / sizeof(eOprot_EPcfg_t);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
static const eOserv_cfg_t theservicescfg =
{  
    .whatever = 0
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


static uint8_t s_boardnum = 0;

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
    //common data useful for init
    eOmcconfig_cfg_t mc_config = {0};
    const eOmcconfig_jomo_cfg_t* jomos_ptr;

    
    EOMtheEMSapplCfg* emsapplcfg = eom_emsapplcfg_GetHandle();
    // the led-pulser is initted as first thing
     
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);

    
    {   // board number is from IP address
        s_boardnum = 0;
        eOipv4addr_t ipaddress = eom_ipnet_GetIPaddress(eom_ipnet_GetHandle());
        s_boardnum = ipaddress >> 24;  
        s_boardnum --;
        if(s_boardnum > 24)
        {
            //return;
            s_boardnum = 0;
        }
        
        s_boardnum = 98; //it imposes that the board is number x
    }
    
    {   // CAN-MAPPING
//        // marco.accame on 19 may 2015: here we load the map of can ... and also we map the entities into some can boards
//        // ... if we have any.
//        EOtheCANmapping * canmap = eo_canmap_Initialise(NULL);
//        // now i load the map of can boards
//        EOconstvector *canboards = eoboardconfig_code2canboards(s_boardnum);
//        eo_canmap_LoadBoards(canmap, canboards);
//        // now i load mc-joints, mc-motors, as-strain, as-mais, sk-skin
//        EOconstvector *entitydes = NULL;
//        // mc
//        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
//        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, entitydes);
//        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
//        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, entitydes); 
//        // as
//        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_analogsensors, eoprot_entity_as_strain);
//        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, entitydes);
//        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_analogsensors, eoprot_entity_as_mais);
//        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, entitydes);
//        // sk
//        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_skin, eoprot_entity_sk_skin);
//        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_skin, eoprot_entity_sk_skin, entitydes);      
    }
    
    {   // CAN-PROTOCOL
//        EOtheCANprotocol * canprot = eo_canprot_Initialise(NULL);              
    }


    
    {   // ETH-PROTOCOL   
        // marco.accame on 24 apr 2015: here is how to customise the eth protocol from a generic to a specific board
        // so far, we can keep it in here. but further on we shall customise one endpoint at a time in runtime.
        
        EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        eo_nvset_BRDlocalsetnumber(nvset, s_boardnum);
        
        // 2. now retrieve the right MC config using the s_boardnum value as a key
        #warning IMPORTANT: is there a way to know the type of control? here it's MC4plus, but with a unique firmware it could be 2foc-based, mc4-based etc...
        
        //fill the config structure
        uint8_t jomosn = eOmcconfig_board2jomosnumber(s_boardnum);
        if (EOMCCONFIG_BOARD_DUMMY != jomosn)
        {
            mc_config.jomosnumber = jomosn;
            mc_config.type = eOmcconfig_type_mc4plus;
            jomos_ptr = eOmcconfig_board2config(s_boardnum, eOmcconfig_type_mc4plus);
            
            //the configuration exists
            if (jomos_ptr != NULL)
            {
                memcpy(mc_config.jomos, jomos_ptr,jomosn*sizeof(eOmcconfig_jomo_cfg_t));        
            }
        }
//        2. load all the endpoints specific to this board. the generic board loads only management
//        uint8_t i = 0;
//        EOconstvector* epcfg_cvector = eoboardconfig_code2EPcfg(s_boardnum);
//        uint16_t numofepcfgs = eo_constvector_Size(epcfg_cvector);
//        for(i=0; i<numofepcfgs; i++)
//        {
//            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(epcfg_cvector, i);
//            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
//            {
//                eo_nvset_LoadEP(nvset, epcfg, eobool_true);
//                eo_entities_Refresh(eo_entities_GetHandle());
//            }                        
//        }

        // 3. ...
        // how to load the other endpoints? it seems that also the SKIN is needed for MC4plus         
                
    }        
   
    eo_entities_Initialise();

    // config initializer has no effect
    eo_serv_Initialise(NULL);
    
    
    //init the MCService (must be done only if the service is initialized)
    eo_serv_ConfigMC(eo_serv_GetHandle(), &mc_config);  
    eo_mcserv_CheckResources(eo_mcserv_GetHandle());
     
    // to do:
    // 1. read ip adddress and get board number
    // 2. load all the config it is needed:
    //    a. endpoint mc (read for instance the num of joints, etc....)
    //    
    
    //now is done in a callback of an eth message from pc104 
    /*
    //configuration & init for motion control service
    eOmcserv_cfg_t mcconfig = {0};
    mcconfig.jomosnumber  = NJOMO;
    mcconfig.type         = eomcserv_type_mc4plus; 
    
    // done inside the inizializer of the motion control service...or not?
    // need to clarify
    
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
    */
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


