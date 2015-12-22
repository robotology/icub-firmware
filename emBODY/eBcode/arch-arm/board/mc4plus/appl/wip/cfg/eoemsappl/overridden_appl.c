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
#include "EOconstvector_hid.h"
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
static EOconstvector* overriden_appl_code2canboards (uint32_t code);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static uint8_t s_boardnum = 0;

static char debug_string[128];

/* CAN MAPPING structures */
//board properties
const EOconstvector s_eo_empty_constvector_board = 
{
    .capacity       = 0,
    .size           = 0,
    .item_size      = sizeof(eOcanmap_board_properties_t),
    .dummy          = 0,
    .stored_items   = NULL,
    .functions      = NULL   
};

const eOcanmap_board_properties_t s_boardprops_mais[] = 
{
    {
        .type               = eobrd_cantype_mais,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 14         
        },
        .requiredprotocol   = {.major = 1, .minor = 0}
    },       
      
};

static EOconstvector s_eo_vectorof_boardprops_mais = 
{
    .capacity       = sizeof(s_boardprops_mais)/sizeof(eOcanmap_board_properties_t),
    .size           = sizeof(s_boardprops_mais)/sizeof(eOcanmap_board_properties_t),
    .item_size      = sizeof(eOcanmap_board_properties_t),
    .dummy          = 0,
    .stored_items   = (void*)s_boardprops_mais,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_boardprops_mais = &s_eo_vectorof_boardprops_mais;

//entities descriptor
static const eOcanmap_entitydescriptor_t s_des_mais[] = 
{
    {   
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 14,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }  
};

EOconstvector s_eo_vectorof_des_mais = 
{
    .capacity       = sizeof(s_des_mais)/sizeof(eOcanmap_entitydescriptor_t),
    .size           = sizeof(s_des_mais)/sizeof(eOcanmap_entitydescriptor_t),
    .item_size      = sizeof(eOcanmap_entitydescriptor_t),
    .dummy          = 0,
    .stored_items   = (void*)s_des_mais,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_descriptor_mais = &s_eo_vectorof_des_mais;

//eth endpoint
static eOprot_EPcfg_t ep_mais_cfg =
{     
    .endpoint           = eoprot_endpoint_analogsensors,
    .numberofentities  = {0, 1, 0, 0, 0, 0, 0}        
};


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
     // to do:
    // 1. read ip adddress and get board number
    // 2. load all the config it is needed:
    //    a. endpoint mc (read for instance the num of joints, etc....)
    //    b. activate can (if needed)
    //    c. ...
    
    //common data useful for init
    eOmcconfig_cfg_t mc_config = {0};
    const eOmcconfig_jomo_cfg_t* jomos_ptr;
    eOcanserv_cfg_t can_config = {0};

    
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

        //s_boardnum = 15; //it imposes that the board is number x
    }

    {   // CAN-MAPPING
        // marco.accame on 19 may 2015: here we load the map of can ... and also we map the entities into some can boards
        //... if we have any.
        EOtheCANmapping * canmap = eo_canmap_Initialise(NULL);

        // now i load the map of can boards
        EOconstvector *canboards = overriden_appl_code2canboards(s_boardnum);

        //if at least one board is present, go on
        if (canboards->size != 0)
        {
            eo_canmap_LoadBoards(canmap, canboards);

            // connecting the entity descriptors the canmap
            EOconstvector *entitydes = NULL;

            // only 1 analog sensor is present for mc4plus (mais)
            // the entity descriptor is manually put here...
            entitydes = eo_vectorof_descriptor_mais;
            eo_canmap_ConfigEntity(canmap, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, entitydes);

            // leave out the skin at the moment, we can add it later on...
            // entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_skin, eoprot_entity_sk_skin);
            // eo_canmap_ConfigEntity(canmap, eoprot_endpoint_skin, eoprot_entity_sk_skin, entitydes);
        }
    }          
    {   // CAN-PROTOCOL
        EOtheCANprotocol * canprot = eo_canprot_Initialise(NULL);              
    }

    {   // ETH-PROTOCOL   
        // marco.accame on 24 apr 2015: here is how to customise the eth protocol from a generic to a specific board
        // so far, we can keep it in here. but further on we shall customise one endpoint at a time in runtime.

        EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        eo_nvset_BRDlocalsetnumber(nvset, s_boardnum);

        // 2. now retrieve the right MC config using the s_boardnum value as a key
        //#warning IMPORTANT: is there a way to know the type of control? here it's MC4plus, but with a unique firmware it could be 2foc-based, mc4-based etc...

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
                //use this config for MCservice (which also loads MCendpoint)
                memcpy(mc_config.jomos, jomos_ptr,jomosn*sizeof(eOmcconfig_jomo_cfg_t));        
            }
        }

        //here I manually load the MAIS endpoint (if needed)
        if ((s_boardnum == 15) || (s_boardnum == 16) || (s_boardnum == 18) || (s_boardnum == 19))
        {
            eOprot_EPcfg_t* epcfg = &ep_mais_cfg;   
            eo_nvset_LoadEP(nvset, epcfg, eobool_true);
            eo_entities_Refresh(eo_entities_GetHandle());
        }

        //do the same for the skin, later on
        /*
        if ((s_boardnum == 17) || (s_boardnum == 20))
        {
            //load the skin endpoint
        }
        */
        
        //2. load all the endpoints specific to this board. the generic board loads only management
        //          uint8_t i = 0;
        //          EOconstvector* epcfg_cvector = eoboardconfig_code2EPcfg(s_boardnum);
        //          uint16_t numofepcfgs = eo_constvector_Size(epcfg_cvector);
        //          for(i=0; i<numofepcfgs; i++)
        //          {
        //            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(epcfg_cvector, i);
        //            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
        //            {
        //                eo_nvset_LoadEP(nvset, epcfg, eobool_true);
        //                eo_entities_Refresh(eo_entities_GetHandle());
        //            }                        
        //          }

        // 3. ...
        // how to load the other endpoints? it seems that also the SKIN is needed for MC4plus...

        //refresh the entities basing on eth endpoints
        eo_entities_Initialise();            
    }        
   
    // general services initializer
    eo_serv_Initialise(NULL);
    
    //init the MCService
    eo_serv_ConfigMC(eo_serv_GetHandle(), &mc_config); 
    eo_mcserv_CheckResources(eo_mcserv_GetHandle());
    
    //init the CurrentsWatchdog
    eo_serv_InitializeCurrentsWatchdog(eo_serv_GetHandle());
    
    //init the CANService    
    //CAN must be activated only for boards: 16,17 (left_lower_arm) 19,20 (right_lower_arm) (skin is still not considered at the moment)
    if ((s_boardnum == 15) || (s_boardnum == 16) || (s_boardnum == 18) || (s_boardnum == 19))
    {
        eo_serv_ConfigCAN(eo_serv_GetHandle(), NULL);

        //init and and start the discovery of CANboards
        //davide: now we are doing it once until the board is reset...
        //it could be nice to have a double-check every time I go in RUN state (a wire can get broken during normal usage of the robot)
        eo_serv_StartCANdiscovery(eo_serv_GetHandle());
    }
    //ready straightforward to go in RUN
    else
    {
        eo_serv_SetBoardReadyForControlLoop(eo_serv_GetHandle());
    }
}


extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p)
{ 
    // pulse led3 forever at 0.50 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, 2*EOK_reltime1sec, 0);
    
    #warning -> TBD: if CAN is available, add eo_canserv_SetMode(onevent), eo_canserv_Empty_TXqueues()  
    //if CANservice initialized
    if (eo_canserv_GetHandle() != NULL)
    {
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
        const uint8_t maxframes2read = 255; // 255 is the max number possible. the function however exits when all canframes are 
        eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport1, maxframes2read, NULL);    
    }
    
    float time_ms = (float)osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "t: %.3fms, Entering in CFG state",time_ms);
    hal_trace_puts (debug_string);    
}

extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p)
{
    float time_ms = (float)osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "t: %.3fms, Exiting from CFG state",time_ms);
    hal_trace_puts (debug_string);    
}

extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p)
{
    // pulse led3 forever at 1 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/1, 0); 
    
    #warning -> TBD: add an enable of motion control loop, such as eo_mcserv_Start() [it enables the tx of all joints on can, ... or else for mc4plus it also starts enconders]    
    eo_mcserv_Start(eo_mcserv_GetHandle());
    
    #warning -> TBD: if CAN is available, add eo_canserv_SetMode(ondemand), eo_canserv_Empty_TXqueues()
    //if CANservice initialized
    if (eo_canserv_GetHandle() != NULL)
    {
        
        // set mode on demand. then tx all canframes remained in the tx queue
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_ondemand);
        eo_canserv_TXstart(eo_canserv_GetHandle(), eOcanport1, NULL);    
        eo_canserv_TXwaituntildone(eo_canserv_GetHandle(), eOcanport1, 5*eok_reltime1ms);
    }
    
    float time_ms = (float)osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "t: %.3fms, Entering in RUN state",time_ms);
    hal_trace_puts (debug_string);     
}

extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p)
{
    #warning -> TBD: if MC is available, add eo_mcserv_Stop() [ its disables the joints on can, etc.].
    eo_mcserv_Stop(eo_mcserv_GetHandle());
    
    #warning -> TBD: if SKIN is available, add eo_skinserv_Stop()
    // also stop strain
    
    #warning -> TBD: if CAN is available, add eo_canserv_SetMode(onevent), eo_canserv_Empty_TXqueues() 
    if (eo_canserv_GetHandle() != NULL)
    {
        // set run mode straigth. then read all can frames received
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
        const uint8_t maxframes2read = 255; // 255 is the max number possible. the function however exits when all canframes are 
        eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport1, maxframes2read, NULL);    
    }
    
    float time_ms = (float)osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "t: %.3fms, Exiting from RUN state",time_ms);
    hal_trace_puts (debug_string);     
}

extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p)
{
    // pulse led3 forever at 4 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/4, 0);
   
    #warning -> TBD: if CAN is available, add eo_canserv_SetMode(onevent), eo_canserv_Empty_TXqueues()
    if (eo_canserv_GetHandle() != NULL)
    {
        eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
        const uint8_t maxframes2read = 255; // 255 is the max number possible. the function however exits when all canframes are 
        eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport1, maxframes2read, NULL);    
    }
    
    float time_ms = (float)osal_system_abstime_get()/1000;
    snprintf(debug_string, sizeof(debug_string), "t: %.3fms, Entering in ERR state",time_ms);
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

static EOconstvector* overriden_appl_code2canboards (uint32_t code)
{
    EOconstvector *ret = (EOconstvector*) &s_eo_empty_constvector_board;
    
    switch(code)
    {
        //these MC4plus have the mais
        case 15:    
        case 16:
        case 18:
        case 19:
        {            
            ret = eo_vectorof_boardprops_mais; 
        } break;
       
        //by default, no CAN boards for mc4plus
        default:    
        {
            ret = ret;
        } break;    
    }
    return(ret);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


