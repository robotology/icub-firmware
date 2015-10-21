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


#include "EOMtheEMSapplCfg_cfg.h"

#include "EOnvSet.h"
#include "EoProtocolMC.h"


#include "EOtheCANservice.h"
#include "EOtheCANprotocol.h"

#include "EOtheEntities.h"
#include "EOtheSTRAIN.h"
#include "EOtheMAIS.h"
#include "EOtheSKIN.h"
#include "EOtheInertial.h"

#include "EOtheBoardConfig.h"


#include "EOconstvector_hid.h"

#include "EOtimer.h"

#include "EOVtheCallbackManager.h"

#include "EOconstarray.h"

#include "EOtheMotionController.h"

#include "EoError.h"


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
   

//static const eOemsapplbody_cfg_t theemsapplbodycfg =
//{
//    .encoderreaderconfig    =
//    {
//        .joints = 
//        {   // at most 6 joints can be managed. one with an encoder each 
//            {   // pos 0
//                .primary_encoder        = eo_appEncReader_enc_type_NONE,
//                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
//                .extra_encoder          = eo_appEncReader_enc_type_NONE,
//                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
//            },
//            {   // pos 1
//                .primary_encoder        = eo_appEncReader_enc_type_NONE,
//                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
//                .extra_encoder          = eo_appEncReader_enc_type_NONE,
//                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
//            },
//            {   // pos 2
//                .primary_encoder        = eo_appEncReader_enc_type_NONE,
//                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
//                .extra_encoder          = eo_appEncReader_enc_type_NONE,
//                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
//            },
//            {   // pos 3
//                .primary_encoder        = eo_appEncReader_enc_type_NONE,
//                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
//                .extra_encoder          = eo_appEncReader_enc_type_NONE,
//                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
//            }, 
//            {   // pos 4
//                .primary_encoder        = eo_appEncReader_enc_type_NONE,
//                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
//                .extra_encoder          = eo_appEncReader_enc_type_NONE,
//                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
//            },
//            {   // pos 5
//                .primary_encoder        = eo_appEncReader_enc_type_NONE,
//                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
//                .extra_encoder          = eo_appEncReader_enc_type_NONE,
//                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
//            }            
//        },
//        .SPI_streams    =
//        {   // at most 2 streams: one over spix and one over spiy
//            {
//                .type       = hal_encoder_tundefined, // or hal_encoder_t1 for aea
//                .numberof   = 0
//            },
//            {
//                .type       = hal_encoder_tundefined, // or hal_encoder_t1 for aea
//                .numberof   = 0
//            }
//        },
//        .SPI_callbackOnLastRead = NULL,
//        .SPI_callback_arg       = NULL       
//    }
//};

// we need to define it ... even if it is useless. we shall remove it later on
const EOVtheEMSapplCfgBody theapplbodyconfig = 
{
    .type               =   0,
    .thetrueconfig      =   NULL
};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void overridden_appl_led_error_init(void);


static void s_overridden_appl_initialise_eb9(EOMtheEMSappl* p);
static void s_debug_eb9(void);
static void callback_eb9(void *p);
static eOresult_t s_on_strain_verify_eb9(EOaService* p, eObool_t verifyisok);
static eOresult_t s_on_mcfoc_verify_eb9(EOaService* p, eObool_t verifyisok);



static void s_overridden_appl_initialise_eb1(EOMtheEMSappl* p);
static void s_debug_eb1(void);
static void callback_eb1(void *p);
static eOresult_t s_on_strain_verify_eb1(EOaService* p, eObool_t verifyisok);
static eOresult_t s_on_mcfoc_verify_eb1(EOaService* p, eObool_t verifyisok);




static void s_overridden_appl_initialise_eb2(EOMtheEMSappl* p);
static void s_debug_eb2(void);
static void callback_eb2(void *p);
static eOresult_t s_on_skin_verify_eb2(EOaService* p, eObool_t verifyisok);
static eOresult_t s_on_mc4_verify_eb2(EOaService* p, eObool_t verifyisok);
static eOresult_t s_on_mais_verify_eb2(EOaService* p, eObool_t verifyisok);
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint8_t s_boardnum = 0;


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
        uint64_t tt = eov_sys_LifeTimeGet(eov_sys_GetHandle());
        uint32_t sec = tt/(1000*1000);
        uint32_t msec = tt%(1000*1000);
        msec /= 1000;
        snprintf(str, sizeof(str), "[%s] at s%d m%d %s in tsk %d: %s", err, sec, msec, eobjstr, taskid, info);
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


extern eObool_t eoprot_b02_b04_mc_isproxied(eOnvID32_t id)
{    
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id);
    
    eOprotEntity_t ent = eoprot_ID2entity(id);
    if(eoprot_entity_mc_joint != ent)
    {
        return(eobool_false);
    }
    
    eOprotTag_t tag = eoprot_ID2tag(id);
    
    switch(tag)
    {
        //VALE get velocity pid not implemented!!!
        case eoprot_tag_mc_joint_config_pidposition:
        // case eoprot_tag_mc_joint_config_pidvelocity:     // marco.accame on 03mar15: the pidvelocity propagation to mc4 is is not implemented, thus i must remove from proxy.
        case eoprot_tag_mc_joint_config_pidtorque:
        case eoprot_tag_mc_joint_config_limitsofjoint:
        case eoprot_tag_mc_joint_config_impedance:
        case eoprot_tag_mc_joint_cmmnds_setpoint:           // marco.accame on 03mar15: the setpoint should not be asked, thus why in here? i may just remove the handler so that no reply is obtained if wrongly used
        {
            return(eobool_true);
        }
        
        default:
        {
            return(eobool_false);
        }
     }
}

//#define DEBUG_INERTIAL
// marco.accame: this function is called inside eom_emsappl_Initialise() just before to run the state machine
// which enters in the CFG state. it is the place where to launch new services

static void s_overridden_appl_initialise2(EOMtheEMSappl* p);

static void s_overridden_appl_initialise3(EOMtheEMSappl* p);

extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p)
{  
    
    s_overridden_appl_initialise_eb2(p);
    
    //s_overridden_appl_initialise_eb1(p);
    
    //s_overridden_appl_initialise2(p);
    
    //s_overridden_appl_initialise3(p);
    
    return;
    
#if 0
    
    EOMtheEMSapplCfg* emsapplcfg = eom_emsapplcfg_GetHandle();
    // the led-pulser is initted as first thing
     
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);
    
    {   // board number is from IP address
        s_boardnum = 0;
        eOipv4addr_t ipaddress = eom_ipnet_GetIPaddress(eom_ipnet_GetHandle());
        s_boardnum = ipaddress >> 24;  
        s_boardnum --;
        if(s_boardnum > 16)
        {
            //return;
            s_boardnum = 0;
        }
#if defined(DEBUG_INERTIAL)        
        s_boardnum = 1; //it imposes that the board is the eb2
#endif
#if defined(TEST_EB2_EB4_WITHOUT_MC)        
        s_boardnum = 1; //it imposes that the board is the eb2
#endif
    }
    
    {   // CAN-MAPPING
        // marco.accame on 19 may 2015: here we load the map of can ... and also we map the entities into some can boards
        // ... if we have any.
        EOtheCANmapping * canmap = eo_canmap_Initialise(NULL);
        // now i load the map of can boards
        EOconstvector *canboards = eoboardconfig_code2canboards(s_boardnum);
        eo_canmap_LoadBoards(canmap, canboards);
        // now i load mc-joints, mc-motors, as-strain, as-mais, as-inertial, sk-skin
        EOconstvector *entitydes = NULL;
#if !defined(TEST_EB2_EB4_WITHOUT_MC)
        // mc-joint
        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, entitydes);
        // mc-motor
        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, entitydes); 
#endif        
        // as-strain
        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_analogsensors, eoprot_entity_as_strain);
        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, entitydes);
#if !defined(TEST_EB2_EB4_WITHOUT_MC)        
        // as-mais
        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_analogsensors, eoprot_entity_as_mais);
        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, entitydes);
#endif   
        // the entity as-inertial is not associated to one can board or a set of can boards. it is a ram-mapped entity. as we can use only one
        // for the limitations of the udp packet, we let the object EOtheInertial manage the proper decoding of the can messages coming from the mtb boards. 
        // THUS, we dont need to call eo_canmap_ConfigEntity(). 
        // in short: if we dont use a eo_canmap_GetEntityIndex() or a eo_canmap_GetEntityLocation() we dont need to call eo_canmap_ConfigEntity() now.
//        // as-inertial
//        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_analogsensors, eoprot_entity_as_inertial);
//        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_analogsensors, eoprot_entity_as_inertial, entitydes);     

        // sk-skin
        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_skin, eoprot_entity_sk_skin);
        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_skin, eoprot_entity_sk_skin, entitydes);      
    }
    
    {   // CAN-PROTOCOL
        EOtheCANprotocol * canprot = eo_canprot_Initialise(NULL);              
    }


    {   // ETH-PROTOCOL   
        // marco.accame on 24 apr 2015: here is how to customise the eth protocol from a generic to a specific board
        // so far, we can keep it in here. but further on we shall customise one endpoint at a time in runtime.
        
        EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
        uint8_t i = 0;
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        eo_nvset_BRDlocalsetnumber(nvset, s_boardnum);
        
        EOconstvector* epcfg_cvector = eoboardconfig_code2EPcfg(s_boardnum);
        
        // 2. load all the endpoints specific to this board. the generic board loads only management
        uint16_t numofepcfgs = eo_constvector_Size(epcfg_cvector);
        for(i=0; i<numofepcfgs; i++)
        {
            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(epcfg_cvector, i);
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                eo_nvset_LoadEP(nvset, epcfg, eobool_true);
            }                        
        }
        
        // now we must define the .... proxy rules
        // if we have board number equal to 1 or 3 ... (eb2 or eb4) then we set it for mc only
        eOprotBRD_t localboard = eoprot_board_local_get();
        if((1 == localboard) || (3 == localboard))
        {
            eoprot_config_proxied_variables(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_b02_b04_mc_isproxied);
        }
    }   


    // now we can initialise the entities object
    eo_entities_Initialise();      
        
    // initialise some other objects such as STRAIN, MAIS, SKIN, Inertial    
    eo_strain_Initialise();
    eo_mais_Initialise();
    eo_skin_Initialise();
    // the inertial is initted but does not know yet which can network it supports
    eo_inertial_Initialise();
    // the can network is loaded in runtime. we need 2x15 values, which for now are taken from its ip address. later on they will be taken from a UDP message
    //eo_inertial_ServiceConfig(eo_inertial_GetHandle(), eoboardconfig_code2inertialCFG(s_boardnum));
    
    // start the application body   
    eOemsapplbody_cfg_t applbodyconfig;
    
    memcpy(&applbodyconfig, &theemsapplbodycfg, sizeof(eOemsapplbody_cfg_t));
    
    // now i get the encoder config
    
    const eOappEncReader_cfg_t *enccfg = eoboardconfig_code2encoderconfig(s_boardnum);
    memcpy(&applbodyconfig.encoderreaderconfig, enccfg, sizeof(eOappEncReader_cfg_t)); 
    
    eo_emsapplBody_Initialise(&applbodyconfig);   

#if defined(DEBUG_INERTIAL)
    // test inertial
    
    //#define TEST_NUM_1
    #define TEST_NUM_2
    
    eOas_inertial_sensorsconfig_t inertialconfig = {0};
    inertialconfig.datarate = 10;
    inertialconfig.enabled = 0;
    
    
 
#if defined(TEST_NUM_1)   
    
    inertialconfig.datarate = 50;
    inertialconfig.enabled = EOAS_ENABLEPOS(eoas_inertial_pos_l_hand);
    eo_inertial_Config(eo_inertial_GetHandle(), &inertialconfig);

    inertialconfig.datarate = 9;
    inertialconfig.enabled = EOAS_ENABLEPOS(eoas_inertial_pos_l_hand) | EOAS_ENABLEPOS(eoas_inertial_pos_l_forearm_1);    
    eo_inertial_SensorsConfig(eo_inertial_GetHandle(), &inertialconfig);
    
    eo_inertial_Stop(eo_inertial_GetHandle());
    
    
    inertialconfig.datarate = 50;
    inertialconfig.enabled = EOAS_ENABLEPOS(eoas_inertial_pos_l_hand);
   
    eo_inertial_SensorsConfig(eo_inertial_GetHandle(), &inertialconfig);   
    eo_inertial_Start(eo_inertial_GetHandle());
    
#endif // defined(TEST_NUM_1)


#if defined(TEST_NUM_2)


    inertialconfig.datarate = 250;
    inertialconfig.enabled = EOAS_ENABLEPOS(eoas_inertial_pos_l_hand);
    eo_inertial_SensorsConfig(eo_inertial_GetHandle(), &inertialconfig);
    
    eo_inertial_Start(eo_inertial_GetHandle());  
    
    
    
    
    eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STrun);

#endif // defined(TEST_NUM_2)

#endif

#endif
}


extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p)
{    
    // pulse led3 forever at 0.50 hz.       
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, 2*EOK_reltime1sec, 0);

    // EOtheCANservice: set straight mode and force parsing of all packets in the RX queues.
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
    eo_canserv_ParseAll(eo_canserv_GetHandle());
}

extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p)
{
}

extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p)
{
    // pulse led3 forever at 1 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/1, 0);  
    
    // EOtheCANservice: set on demand mode. then tx all canframes remained in the tx queues
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_ondemand);    
    eo_canserv_TXstartAll(eo_canserv_GetHandle());
    eo_canserv_TXwaitAllUntilDone(eo_canserv_GetHandle(), 5*eok_reltime1ms);
    
    
    // motion-control:
    eo_motioncontrol_Start(eo_motioncontrol_GetHandle());
    
    
#if 0  
    eOresult_t res = eores_NOK_generic;    
    // enable joints
    res = eo_emsapplBody_EnableTxAllJointOnCan(eo_emsapplBody_GetHandle());
    if (eores_NOK_generic == res)
    {
        //handle the error
    }
    
    // enable the tx mode of strain, if present and as configured
    eo_strain_SendTXmode(eo_strain_GetHandle());
    
     
     
    // set mode on demand. then tx all canframes remained in the tx queue
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_ondemand);
   
    eo_canserv_TXstart(eo_canserv_GetHandle(), eOcanport1, NULL);    
    eo_canserv_TXstart(eo_canserv_GetHandle(), eOcanport2, NULL);    
    
    eo_canserv_TXwaituntildone(eo_canserv_GetHandle(), eOcanport1, 5*eok_reltime1ms);
    eo_canserv_TXwaituntildone(eo_canserv_GetHandle(), eOcanport2, 5*eok_reltime1ms);

    // Start reading the encoders
    eo_appEncReader_StartRead(eo_emsapplBody_GetEncoderReader(eo_emsapplBody_GetHandle()));
#endif
}


extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p)
{
    eOresult_t res = eores_NOK_generic;
    
    
    // EOtheCANservice: set straigth mode and force parsing of all packets in the RX queues.
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
    eo_canserv_ParseAll(eo_canserv_GetHandle());  
  
    
    // stop services which were started in on_entry_RUN()
    
    // motion-control
    eo_motioncontrol_Stop(eo_motioncontrol_GetHandle());
    
    
    // stop tx activity of services that may have been started by callback function
    
    // strain
    eo_strain_TXstop(eo_strain_GetHandle());
        
    // skin
    eo_skin_TXstop(eo_skin_GetHandle());
        
    // mais
    // we prefer NOT to stop it
    //eo_mais_TXstop(eo_mais_GetHandle());
    
    
#if 0  
    
    // stop skin. the check whether to stop skin or not is done internally.
    eo_skin_DisableTX(eo_skin_GetHandle());


    // stop motion control
    res = eo_emsapplBody_DisableTxAllJointOnCan(eo_emsapplBody_GetHandle());
    if (eores_NOK_generic == res)
	{
		// handle the error ... not important
    }
    
    // stop tx of strain, if present
    eo_strain_TXstop(eo_strain_GetHandle()); 
    //eo_strain_DisableTX(eo_strain_GetHandle());    

    // stop tx of inertial. the check whether to stop mtb boards or not is done internally.
    eo_inertial_Stop(eo_inertial_GetHandle());
#endif
    
}

extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p)
{
    // pulse led3 forever at 4 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/4, 0);
   

    // EOtheCANservice: set straigth mode and force parsing of all packets in the RX queues.
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
    eo_canserv_ParseAll(eo_canserv_GetHandle());  

    
    
    // stop services which may have been started 
    
    // motion-control
    eo_motioncontrol_Stop(eo_motioncontrol_GetHandle());
        
    // stop tx activity of services that may have been started by callback function
    
    // strain
    eo_strain_TXstop(eo_strain_GetHandle());
        
    // skin
    eo_skin_TXstop(eo_skin_GetHandle());
        
    // mais
    // we prefer NOT to stop it
    //eo_mais_TXstop(eo_mais_GetHandle());
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



// -----------------------------------------------------------------------------------------------------------------

static const eOprot_EPcfg_t s_theEPcfgsOthersMAX[] =
{  
    {           
        .endpoint           = eoprot_endpoint_motioncontrol,
        .numberofentities   = {12, 12, 1, 0, 0, 0, 0}     
    },     
    {        
        .endpoint           = eoprot_endpoint_analogsensors,
        .numberofentities   = {1, 1, 1, 1, 0, 0, 0}        
    },
    {        
        .endpoint           = eoprot_endpoint_skin,
        .numberofentities   = {2, 0, 0, 0, 0, 0, 0}        
    }     
};

const EOconstvector s_eo_vectorof_EPcfg_max = 
{
    .capacity       = sizeof(s_theEPcfgsOthersMAX)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_theEPcfgsOthersMAX)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_theEPcfgsOthersMAX,
    .functions      = NULL   
};

const EOconstvector * s_the_vectorof_EPcfgs = &s_eo_vectorof_EPcfg_max;

static void s_can_cbkonrx(void *arg)
{
    EOMtask *task = (EOMtask *)arg;
    eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
}


static void s_debug(void);


static void s_overridden_appl_initialise2(EOMtheEMSappl* p)
{
    
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);    
    
    // board is eb1
    s_boardnum = 0;
    
    // initialise all basic objects
    
    {   // eth-protocol-1: the nvset   
        // marco.accame on 30 sept 2015: i initialise at max capabilities
        
        EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
        uint8_t i = 0;
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        eo_nvset_BRDlocalsetnumber(nvset, s_boardnum);
                
        // 2. load all the endpoints specific to this board. the generic board loads only management
        uint16_t numofepcfgs = eo_constvector_Size(s_the_vectorof_EPcfgs);
        for(i=0; i<numofepcfgs; i++)
        {
            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(s_the_vectorof_EPcfgs, i);
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                eo_nvset_LoadEP(nvset, epcfg, eobool_true);
            }                        
        }
    }
    {   // eth-protocol-2: the callbacks
        // marco.accame on 30 sept 2015: so far i define all the callbacks. however:
        // 1. we may decide to define EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME and thus we must later on to load a proper callback. 
        //    BUT maybe better not.
        // 2. if not, i MUST later on re-write the callbacks, so that:
        //    a. we can understand if a service is configured (use proper object) and we something only if it configured.
        //    b. make sure that when we use a get entity, we use EOtheEntities which does not address joints beyond those configured
         
    }    

    {   // entities   
        // marco.accame on 30 sept 2015: i initialise at max capabilities. however, so far we have 0 entities.
        // when we need to init a service, we call a proper eo_entities_SetNumOf...(num) method
        // to unit we can call a eo_entities_Reset() for all or a proper eo_entities_SetNumOf...(0)  
        eo_entities_Initialise();        
    }
    
    {   // can-mapping
        // marco.accame on 30 sept 2015: so far it is empty. i will load it later on with boards and with entities.
        // i must write a eo_canmap_LoadEntity() with can boards and info about how the can boards.
        // i also MUST write a eo_canmap_UnloadEntity()
        eo_canmap_Initialise(NULL);
    }
    
    {   // can-protocol
        // marco.accame on 30 sept 2015: i init it in a complete way. we dont have a load method for service ...
        // we may add it, so that if we dont init we dont allow it.
        eo_canprot_Initialise(NULL);        
    }
    
    {   // other services: I dont init anything. however i should avoid automatic initialisation for:
        // EOtheMAIS, EOtheSTRAIN, EOtheSKIN, EOtheInertial, EOtheMotionController     

        // changed idea: i initialise them.
        eo_strain_Initialise();        
        eo_motioncontrol_Initialise();        
    }
    
    {   // i init a service handler. for instance to be called EOtheServices
        // so far i do in here what i need without any container
             
        // can-services
        eOcanserv_cfg_t config = {.mode = eocanserv_mode_straight};   
        
        config.mode                 = eocanserv_mode_straight;
        config.canstabilizationtime = 7*OSAL_reltime1sec;
        config.rxqueuesize[0]       = 64;
        config.rxqueuesize[1]       = 64;
        config.txqueuesize[0]       = 64;
        config.txqueuesize[1]       = 64;  
        config.onrxcallback[0]      = s_can_cbkonrx; 
        config.onrxargument[0]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
        config.onrxcallback[1]      = s_can_cbkonrx; 
        config.onrxargument[1]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 
            
        // attenzione alla mc4cplus che ha 1 solo can ... potrei cambiare il eo_canserv_Initialise() in modo che
        // se hal_can_supported_is(canx) e' falso allora non retsituisco errore ma semplicemente non inizializzo.
        eo_canserv_Initialise(&config);   
        
        // can-discovery
        eo_candiscovery2_Initialise(NULL);        
    }
    
    
    // ok, nothing else now .....
    
    
    // for debug: 
    
    // A. start a timer of 20 seconds w/ a callback which:
    // 1. discover the strain,
    // 2. init EOtheStrain (eo_strain_Load()), which in turn will call EOtheEnetities etc. 
    // 3. ask the full-scales,
    // 4. when they are all retrieved, init the strain and start it, and start the control-loop.
    
    // B. start a timer of 60 seconds w/ callback which:
    // 1. stop the strain, deinit the EOtheStrain ((eo_strain_Unload())
    
    // C. start a timer of 70 seconds whcih send the eapplication in run mode.
    
   s_debug();
    
    
}

static eOresult_t s_onfullscaleready(EOaService* p, eObool_t operationisok);
static eOresult_t s_on_strain_verify(EOaService* p, eObool_t verifyisok);
static eOresult_t onstop_search4strain(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);
static void callback1(void *p);



static eOresult_t onstop_search4strain(void *par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    if(eobool_true == searchisok)
    {
        const eOmn_serv_configuration_t * strainserv = (const eOmn_serv_configuration_t *)par;
        eo_strain_Activate(eo_strain_GetHandle(), strainserv);        
    }
    
    return(eores_OK);
}


static eOresult_t s_on_strain_verify(EOaService* p, eObool_t verifyisok)
{
    uint8_t rr = 0;
    
    if(eobool_true == verifyisok)
    {
        hal_trace_puts("s_on_strain_verify(p, OK)");
        eo_strain_GetFullScale(eo_strain_GetHandle(), s_onfullscaleready); 
    }
    else
    {
        hal_trace_puts("s_on_strain_verify(p, KO)");
        rr = 0;
    }
    
    rr = rr;
    
    return(eores_OK);
}

static eOresult_t s_onfullscaleready(EOaService* p, eObool_t operationisok)
{
    eOresult_t res = eores_OK;
    
    if(eobool_false == operationisok)
    {
        hal_trace_puts("s_onfullscaleready(p, KO)");
        // send diagnostics
        return(eores_NOK_generic);
    }
    hal_trace_puts("s_onfullscaleready(p, OK)");
    
    eOas_strain_t *strain = eo_entities_GetStrain(eo_entities_GetHandle(), 0);
    eOas_arrayofupto12bytes_t *fullscale = &strain->status.fullscale;
    EOconstarray *c = eo_constarray_Load((EOarray*)fullscale);
    uint8_t size = eo_constarray_Size(c);
    size = size;
    uint8_t i=0;
    uint16_t calibvalues[6] = {1};
    for(i=0; i<size; i++)
    {
        calibvalues[i] = * ((uint16_t*)eo_constarray_At(c, i));        
    }
    size = size;
    
    
    return(res);    
}

static void callback1(void *p)
{
    static uint8_t tick = 0;
    
    const eOmn_serv_configuration_t * strainserv = NULL; 
    
    eObool_t stoptimer = eobool_false;
    
    if(NULL == strainserv)
    {
        strainserv = eoboardconfig_code2strain_serv_configuration(0);
    }
    
    if(0 == tick)
    {   // i enter in run mode.
        eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STrun);        
    }
    else if(1 == tick)
    {
        // start discovery of the strain
        
        //eo_strain_TXstart(eo_strain_GetHandle(), 10, eoas_strainmode_txcalibrateddatacontinuously); 
        
        eo_strain_Verify(eo_strain_GetHandle(), strainserv, s_on_strain_verify, eobool_true);       
    }
    else if(2 == tick)
    {       
        //eo_strain_Activate(eo_strain_GetHandle(), strainserv);  
        //eo_strain_Verify(eo_strain_GetHandle(), strainserv, s_on_strain_verify, eobool_false);      

        //eo_strain_TXstart(eo_strain_GetHandle(), 10, eoas_strainmode_txcalibrateddatacontinuously);  
        
//        eOas_strain_t *strain = eo_entities_GetStrain(eo_entities_GetHandle(), 0);
//        eOas_arrayofupto12bytes_t *fullscale = &strain->status.fullscale;
//        eo_strain_TXstart(eo_strain_GetHandle(), 10, eoas_strainmode_acquirebutdonttx);
//        eo_strain_GetFullScale(eo_strain_GetHandle(), s_onfullscaleready);        
    }
    else if(3 == tick)
    {
        eo_strain_Deactivate(eo_strain_GetHandle());          
    }
    else if(4 == tick)
    {
        eo_strain_Activate(eo_strain_GetHandle(), strainserv);
        eo_strain_TXstart(eo_strain_GetHandle(), 10, eoas_strainmode_txcalibrateddatacontinuously);         
    }
   
    tick++;
   
   
    if(tick >= 8)
    {
        stoptimer = eobool_true;
    }
   
    if(eobool_true == stoptimer)
    {
        eo_timer_Stop((EOtimer*)p);
    }   
}


static void s_debug(void)  
{
    EOtimer *tmrA = eo_timer_New();    
    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    
    eo_action_SetCallback(act, callback1, tmrA, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
    
    eo_timer_Start(tmrA, eok_abstimeNOW, 10*eok_reltime1sec, eo_tmrmode_FOREVER, act);   
}



static void s_debug3(void);

static void s_overridden_appl_initialise3(EOMtheEMSappl* p)
{
    
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);    
    
    // board is eb1
    s_boardnum = 0;
    
    // initialise all basic objects
    
    {   // eth-protocol-1: the nvset   
        // marco.accame on 30 sept 2015: i initialise at max capabilities
        
        EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
        uint8_t i = 0;
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        eo_nvset_BRDlocalsetnumber(nvset, s_boardnum);
                
        // 2. load all the endpoints specific to this board. the generic board loads only management
        uint16_t numofepcfgs = eo_constvector_Size(s_the_vectorof_EPcfgs);
        for(i=0; i<numofepcfgs; i++)
        {
            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(s_the_vectorof_EPcfgs, i);
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                eo_nvset_LoadEP(nvset, epcfg, eobool_true);
            }                        
        }
    }
    {   // eth-protocol-2: the callbacks
        // marco.accame on 30 sept 2015: so far i define all the callbacks. however:
        // 1. we may decide to define EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME and thus we must later on to load a proper callback. 
        //    BUT maybe better not.
        // 2. if not, i MUST later on re-write the callbacks, so that:
        //    a. we can understand if a service is configured (use proper object) and we something only if it configured.
        //    b. make sure that when we use a get entity, we use EOtheEntities which does not address joints beyond those configured
         
    }    

    {   // entities   
        // marco.accame on 30 sept 2015: i initialise at max capabilities. however, so far we have 0 entities.
        // when we need to init a service, we call a proper eo_entities_SetNumOf...(num) method
        // to unit we can call a eo_entities_Reset() for all or a proper eo_entities_SetNumOf...(0)  
        eo_entities_Initialise();        
    }
    
    {   // can-mapping
        // marco.accame on 30 sept 2015: so far it is empty. i will load it later on with boards and with entities.
        // i must write a eo_canmap_LoadEntity() with can boards and info about how the can boards.
        // i also MUST write a eo_canmap_UnloadEntity()
        eo_canmap_Initialise(NULL);
    }
    
    {   // can-protocol
        // marco.accame on 30 sept 2015: i init it in a complete way. we dont have a load method for service ...
        // we may add it, so that if we dont init we dont allow it.
        eo_canprot_Initialise(NULL);        
    }
    
    {   // other services: I dont init anything. however i should avoid automatic initialisation for:
        // EOtheMAIS, EOtheSTRAIN, EOtheSKIN, EOtheInertial, EOtheMotionController     

        // changed idea: i initialise them.
        eo_strain_Initialise();        
        eo_motioncontrol_Initialise();        
    }
    
    {   // i init a service handler. for instance to be called EOtheServices
        // so far i do in here what i need without any container
             
        // can-services
        eOcanserv_cfg_t config = {.mode = eocanserv_mode_straight};   
        
        config.mode                 = eocanserv_mode_straight;
        config.canstabilizationtime = 7*OSAL_reltime1sec;
        config.rxqueuesize[0]       = 64;
        config.rxqueuesize[1]       = 64;
        config.txqueuesize[0]       = 64;
        config.txqueuesize[1]       = 64;  
        config.onrxcallback[0]      = s_can_cbkonrx; 
        config.onrxargument[0]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
        config.onrxcallback[1]      = s_can_cbkonrx; 
        config.onrxargument[1]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 
            
        // attenzione alla mc4cplus che ha 1 solo can ... potrei cambiare il eo_canserv_Initialise() in modo che
        // se hal_can_supported_is(canx) e' falso allora non retsituisco errore ma semplicemente non inizializzo.
        eo_canserv_Initialise(&config);   
        
        // can-discovery
        eo_candiscovery2_Initialise(NULL);        
    }
    
    
    // ok, nothing else now .....
    
    
    // for debug: 
    
    // A. start a timer of 10 seconds w/ a callback which:
    // 1. discover the strain,
    // 2. init EOtheStrain (eo_strain_Load()), which in turn will call EOtheEnetities etc. 
    // 3. ask the full-scales,

    
    // B. start a timer of 15 seconds w/ callback which:
    // 1. start then motioncontrol
    
    
   s_debug3();
    
    
}


static eOresult_t s_on_mcfoc_verify(EOaService* p, eObool_t verifyisok)
{
    uint8_t rr = 0;
    
    if(eobool_true == verifyisok)
    {
        hal_trace_puts("s_on_mcfoc_verify(p, OK)");
        rr++;
        rr = rr;
    }
    else
    {
        hal_trace_puts("s_on_mcfoc_verify(p, KO)");
        rr = 0;
    }
    
    rr = rr;
    
    return(eores_OK);
}

static void callback3(void *p)
{
    static uint8_t tick = 0;
    
    const eOmn_serv_configuration_t * strainserv = NULL; 
    const eOmn_serv_configuration_t * mcfocserv = NULL; 
    
    eObool_t stoptimer = eobool_false;
    
    if(NULL == strainserv)
    {
        strainserv = eoboardconfig_code2strain_serv_configuration(14);
    }
    
    if(NULL == mcfocserv)
    {
        mcfocserv = eoboardconfig_code2motion_serv_configuration(14);
    }
    
    if(0 == tick)
    {   // i enter in run mode.
        //eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STrun);        
    }
    else if(1 == tick)
    {
        // start discovery of the strain. at the end the strain is activated     
        hal_trace_puts("verify strain");         
        eo_strain_Verify(eo_strain_GetHandle(), strainserv, s_on_strain_verify, eobool_true);       
    }
    else if(2 == tick)
    { 
        hal_trace_puts("verify motion control");                
        eo_motioncontrol_Verify(eo_motioncontrol_GetHandle(), mcfocserv, s_on_mcfoc_verify, eobool_true);   
    }
    else if(3 == tick)
    {
        hal_trace_puts("entering run mode");
        eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STrun);         
    }
    else if(4 == tick)
    {
     
    }
   
    tick++;
   
   
    if(tick >= 8)
    {
        stoptimer = eobool_true;
    }
   
    if(eobool_true == stoptimer)
    {
        eo_timer_Stop((EOtimer*)p);
    }   
}


static void s_debug3(void)  
{
    EOtimer *tmrA = eo_timer_New();    
    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    
    eo_action_SetCallback(act, callback3, tmrA, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
    
    eo_timer_Start(tmrA, eok_abstimeNOW, 5*eok_reltime1sec, eo_tmrmode_FOREVER, act);   
}


static void s_overridden_appl_initialise_eb1(EOMtheEMSappl* p)
{
    
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);    
    
    // board is eb1
    s_boardnum = 0;
    
    // initialise all basic objects
    
    {   // eth-protocol-1: the nvset   
        // marco.accame on 30 sept 2015: i initialise at max capabilities
        
        EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
        uint8_t i = 0;
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        eo_nvset_BRDlocalsetnumber(nvset, s_boardnum);
                
        // 2. load all the endpoints specific to this board. the generic board loads only management
        uint16_t numofepcfgs = eo_constvector_Size(s_the_vectorof_EPcfgs);
        for(i=0; i<numofepcfgs; i++)
        {
            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(s_the_vectorof_EPcfgs, i);
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                eo_nvset_LoadEP(nvset, epcfg, eobool_true);
            }                        
        }
    }
    {   // eth-protocol-2: the callbacks
        // marco.accame on 30 sept 2015: so far i define all the callbacks. however:
        // 1. we may decide to define EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME and thus we must later on to load a proper callback. 
        //    BUT maybe better not.
        // 2. if not, i MUST later on re-write the callbacks, so that:
        //    a. we can understand if a service is configured (use proper object) and we something only if it configured.
        //    b. make sure that when we use a get entity, we use EOtheEntities which does not address joints beyond those configured
         
    }    

    {   // entities   
        // marco.accame on 30 sept 2015: i initialise at max capabilities. however, so far we have 0 entities.
        // when we need to init a service, we call a proper eo_entities_SetNumOf...(num) method
        // to unit we can call a eo_entities_Reset() for all or a proper eo_entities_SetNumOf...(0)  
        eo_entities_Initialise();        
    }
    
    {   // can-mapping
        // marco.accame on 30 sept 2015: so far it is empty. i will load it later on with boards and with entities.
        // i must write a eo_canmap_LoadEntity() with can boards and info about how the can boards.
        // i also MUST write a eo_canmap_UnloadEntity()
        eo_canmap_Initialise(NULL);
    }
    
    {   // can-protocol
        // marco.accame on 30 sept 2015: i init it in a complete way. we dont have a load method for service ...
        // we may add it, so that if we dont init we dont allow it.
        eo_canprot_Initialise(NULL);        
    }
    
    {   // other services: I dont init anything. however i should avoid automatic initialisation for:
        // EOtheMAIS, EOtheSTRAIN, EOtheSKIN, EOtheInertial, EOtheMotionController     

        // changed idea: i initialise them.
        eo_strain_Initialise();  
        eo_mais_Initialise();        
        eo_motioncontrol_Initialise();    
        eo_skin_Initialise();          
    }
    
    {   // i init a service handler. for instance to be called EOtheServices
        // so far i do in here what i need without any container
             
        // can-services
        eOcanserv_cfg_t config = {.mode = eocanserv_mode_straight};   
        
        config.mode                 = eocanserv_mode_straight;
        config.canstabilizationtime = 7*OSAL_reltime1sec;
        config.rxqueuesize[0]       = 64;
        config.rxqueuesize[1]       = 64;
        config.txqueuesize[0]       = 64;
        config.txqueuesize[1]       = 64;  
        config.onrxcallback[0]      = s_can_cbkonrx; 
        config.onrxargument[0]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
        config.onrxcallback[1]      = s_can_cbkonrx; 
        config.onrxargument[1]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 
            
        // attenzione alla mc4cplus che ha 1 solo can ... potrei cambiare il eo_canserv_Initialise() in modo che
        // se hal_can_supported_is(canx) e' falso allora non retsituisco errore ma semplicemente non inizializzo.
        eo_canserv_Initialise(&config);   
        
        // can-discovery
        eo_candiscovery2_Initialise(NULL);        
    }
    
    
    // ok, nothing else now .....
    
    
    // for debug: 
    
    // A. start a timer of 10 seconds w/ a callback which:
    // 1. discover the strain,
    // 2. init EOtheStrain (eo_strain_Load()), which in turn will call EOtheEnetities etc. 
    // 3. ask the full-scales,

    
    // B. start a timer of 5 seconds w/ callback which:

       
   s_debug_eb1();
       
}

static void s_debug_eb1(void)  
{
    EOtimer *tmrA = eo_timer_New();    
    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    
    eo_action_SetCallback(act, callback_eb1, tmrA, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
    
    eo_timer_Start(tmrA, eok_abstimeNOW, 5*eok_reltime1sec, eo_tmrmode_FOREVER, act);   
}

static void callback_eb1(void *p)
{
    static uint8_t tick = 0;
    
    const eOmn_serv_configuration_t * strainserv = NULL; 
    const eOmn_serv_configuration_t * mcfocserv = NULL; 
    
    eObool_t stoptimer = eobool_false;
    
    if(NULL == strainserv)
    {
        strainserv = eoboardconfig_code2strain_serv_configuration(0);
    }
    
    if(NULL == mcfocserv)
    {
        mcfocserv = eoboardconfig_code2motion_serv_configuration(0);
    }
    
    if(0 == tick)
    {   
        eo_strain_Verify(eo_strain_GetHandle(), strainserv, s_on_strain_verify_eb1, eobool_true);              
    }
    else if(1 == tick)
    { 
        eo_motioncontrol_Verify(eo_motioncontrol_GetHandle(), mcfocserv, s_on_mcfoc_verify_eb1, eobool_true);      
    }
    else if(2 == tick)
    { 

    }
    else if(3 == tick)
    {
                
    }
    else if(4 == tick)
    {
     
    }
   
    tick++;
   
   
    if(tick >= 8)
    {
        stoptimer = eobool_true;
    }
   
    if(eobool_true == stoptimer)
    {
        eo_timer_Stop((EOtimer*)p);
    }   
}

static eOerrmanDescriptor_t errdes = {0};

static eOresult_t s_on_mcfoc_verify_eb1(EOaService* p, eObool_t verifyisok)
{
    uint8_t rr = 0;
    
    
    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;
    errdes.par16            = 0xf2f2;
    
       
    
    if(eobool_true == verifyisok)
    {
        errdes.par64            = 0xffffffffffffffff;       
        rr++;
        rr = rr;
    }
    else
    {
        errdes.par64            = 0;
        rr = 0;
    }
    
    rr = rr;
    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes); 
    
    return(eores_OK);
}

static eOresult_t s_on_strain_verify_eb1(EOaService* p, eObool_t verifyisok)
{    
    uint8_t rr = 0;
    
    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;
    errdes.par16            = 0xf1f1;
    
    if(eobool_true == verifyisok)
    {
        errdes.par64            = 0xffffffffffffffff;
        rr++;
        rr = rr;
    }
    else
    {
        errdes.par64            = 0;
        rr = 0;
    }
    
    rr = rr;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes);
    
    return(eores_OK);    
    
}



// --------------------------------------------------------------------------------------------------------------------
// - eb9 section
// --------------------------------------------------------------------------------------------------------------------


static void s_overridden_appl_initialise_eb9(EOMtheEMSappl* p)
{
    
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);    
    
    // board is eb9
    s_boardnum = 8;
    
    // initialise all basic objects
    
    {   // eth-protocol-1: the nvset   
        // marco.accame on 30 sept 2015: i initialise at max capabilities
        
        EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
        uint8_t i = 0;
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        eo_nvset_BRDlocalsetnumber(nvset, s_boardnum);
                
        // 2. load all the endpoints specific to this board. the generic board loads only management
        uint16_t numofepcfgs = eo_constvector_Size(s_the_vectorof_EPcfgs);
        for(i=0; i<numofepcfgs; i++)
        {
            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(s_the_vectorof_EPcfgs, i);
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                eo_nvset_LoadEP(nvset, epcfg, eobool_true);
            }                        
        }
    }
    {   // eth-protocol-2: the callbacks
        // marco.accame on 30 sept 2015: so far i define all the callbacks. however:
        // 1. we may decide to define EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME and thus we must later on to load a proper callback. 
        //    BUT maybe better not.
        // 2. if not, i MUST later on re-write the callbacks, so that:
        //    a. we can understand if a service is configured (use proper object) and we something only if it configured.
        //    b. make sure that when we use a get entity, we use EOtheEntities which does not address joints beyond those configured
         
    }    

    {   // entities   
        // marco.accame on 30 sept 2015: i initialise at max capabilities. however, so far we have 0 entities.
        // when we need to init a service, we call a proper eo_entities_SetNumOf...(num) method
        // to unit we can call a eo_entities_Reset() for all or a proper eo_entities_SetNumOf...(0)  
        eo_entities_Initialise();        
    }
    
    {   // can-mapping
        // marco.accame on 30 sept 2015: so far it is empty. i will load it later on with boards and with entities.
        // i must write a eo_canmap_LoadEntity() with can boards and info about how the can boards.
        // i also MUST write a eo_canmap_UnloadEntity()
        eo_canmap_Initialise(NULL);
    }
    
    {   // can-protocol
        // marco.accame on 30 sept 2015: i init it in a complete way. we dont have a load method for service ...
        // we may add it, so that if we dont init we dont allow it.
        eo_canprot_Initialise(NULL);        
    }
    
    {   // other services: I dont init anything. however i should avoid automatic initialisation for:
        // EOtheMAIS, EOtheSTRAIN, EOtheSKIN, EOtheInertial, EOtheMotionController     

        // changed idea: i initialise them.
        eo_strain_Initialise();  
        //eo_mais_Initialise();        
        eo_motioncontrol_Initialise();    
        //eo_skin_Initialise();          
    }
    
    {   // i init a service handler. for instance to be called EOtheServices
        // so far i do in here what i need without any container
             
        // can-services
        eOcanserv_cfg_t config = {.mode = eocanserv_mode_straight};   
        
        config.mode                 = eocanserv_mode_straight;
        config.canstabilizationtime = 7*OSAL_reltime1sec;
        config.rxqueuesize[0]       = 64;
        config.rxqueuesize[1]       = 64;
        config.txqueuesize[0]       = 64;
        config.txqueuesize[1]       = 64;  
        config.onrxcallback[0]      = s_can_cbkonrx; 
        config.onrxargument[0]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
        config.onrxcallback[1]      = s_can_cbkonrx; 
        config.onrxargument[1]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 
            
        // attenzione alla mc4cplus che ha 1 solo can ... potrei cambiare il eo_canserv_Initialise() in modo che
        // se hal_can_supported_is(canx) e' falso allora non retsituisco errore ma semplicemente non inizializzo.
        eo_canserv_Initialise(&config);   
        
        // can-discovery
        eo_candiscovery2_Initialise(NULL);        
    }
    
    
    // B. start a timer of 5 seconds w/ callback which:

       
   s_debug_eb9();
       
}


static eObool_t eb9verified = eobool_false;
static eOerrmanDescriptor_t errdes_mc_eb9 = {0};
static eOerrmanDescriptor_t errdes_st_eb9 = {0};

static void s_debug_eb9(void)  
{
    EOtimer *tmrA = eo_timer_New();    
    
    errdes_st_eb9.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    errdes_st_eb9.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes_st_eb9.sourceaddress    = 0;
    errdes_st_eb9.par16            = 0x0000;
    errdes_st_eb9.par64            = 0xffff000000000000;        
    
    errdes_mc_eb9.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
    errdes_mc_eb9.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes_mc_eb9.sourceaddress    = 0;
    errdes_mc_eb9.par16            = 0x0000;
    errdes_mc_eb9.par64            = 0xffff000000000000;
    
    
    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    
    eo_action_SetCallback(act, callback_eb9, tmrA, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
    
    eo_timer_Start(tmrA, eok_abstimeNOW, 5*eok_reltime1sec, eo_tmrmode_FOREVER, act);   
}


static void callback_eb9(void *p)
{
    static uint8_t tick = 0;
    uint8_t boardnumber = 8;
    
    static const eOmn_serv_configuration_t * mcfocserv = NULL; 
    
    static const eOmn_serv_configuration_t * strainserv = NULL; 
    
    eObool_t stoptimer = eobool_false;
    
    
    if(NULL == mcfocserv)
    {
        mcfocserv = eoboardconfig_code2motion_serv_configuration(boardnumber);
    }
    
    if(NULL == strainserv)
    {
        strainserv = eoboardconfig_code2strain_serv_configuration(boardnumber);
    }    
    
    if(0 == tick)
    {  
        eo_strain_Verify(eo_strain_GetHandle(), strainserv, s_on_strain_verify_eb9, eobool_true);                    
    }
    else if(1 == tick)
    { 
        eo_motioncontrol_Verify(eo_motioncontrol_GetHandle(), mcfocserv, s_on_mcfoc_verify_eb9, eobool_true);  
    }
//    else if(2 == tick)
//    { 
//        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);
//    }
//    else if(3 == tick)
//    {
//        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);
//    }
//    else if(4 == tick)
//    {
//        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);
//    }
//    else if(5 == tick)
//    {
//        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);
//    }
    else
    {
        errdes_st_eb9.par64            = 0xffff000000000000 | tick;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_st_eb9);  
        
        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);         
    }
   
    tick++;
   
   
    if(tick >= 8)
    {
        stoptimer = eobool_true;
    }
   
    if(eobool_true == stoptimer)
    {
        eo_timer_Stop((EOtimer*)p);
    }   
}



static eOresult_t s_on_strain_verify_eb9(EOaService* p, eObool_t verifyisok)
{    
    uint8_t rr = 0;
    
    
    if(eobool_true == verifyisok)
    {
        errdes_st_eb9.par16            = 0xffff; 
        rr++;
        rr = rr;
    }
    else
    {
        errdes_st_eb9.par16            = 0xff00;
        rr = 0;
    }
    
    rr = rr;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_st_eb9);
    
    return(eores_OK);    
    
}


static eOresult_t s_on_mcfoc_verify_eb9(EOaService* p, eObool_t verifyisok)
{
    uint8_t rr = 0;
          
    
    if(eobool_true == verifyisok)
    {
        
        eb9verified = eobool_true;
        errdes_mc_eb9.par16            = 0xffff;       
        rr++;
        rr = rr;
    }
    else
    {
        errdes_mc_eb9.par16            = 0xff00;
    }
    
    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9); 
    
    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - eb2 section
// --------------------------------------------------------------------------------------------------------------------


static void s_overridden_appl_initialise_eb2(EOMtheEMSappl* p)
{
    
    // pulse led3 forever at 20 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/20, 0);    
    
    // board is eb1
    s_boardnum = 1;
    
    // initialise all basic objects
    
    {   // eth-protocol-1: the nvset   
        // marco.accame on 30 sept 2015: i initialise at max capabilities
        
        EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
        uint8_t i = 0;
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        eo_nvset_BRDlocalsetnumber(nvset, s_boardnum);
                
        // 2. load all the endpoints specific to this board. the generic board loads only management
        uint16_t numofepcfgs = eo_constvector_Size(s_the_vectorof_EPcfgs);
        for(i=0; i<numofepcfgs; i++)
        {
            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(s_the_vectorof_EPcfgs, i);
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                eo_nvset_LoadEP(nvset, epcfg, eobool_true);
            }                        
        }
    }
    {   // eth-protocol-2: the callbacks
        // marco.accame on 30 sept 2015: so far i define all the callbacks. however:
        // 1. we may decide to define EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME and thus we must later on to load a proper callback. 
        //    BUT maybe better not.
        // 2. if not, i MUST later on re-write the callbacks, so that:
        //    a. we can understand if a service is configured (use proper object) and we something only if it configured.
        //    b. make sure that when we use a get entity, we use EOtheEntities which does not address joints beyond those configured
         
    }    

    {   // entities   
        // marco.accame on 30 sept 2015: i initialise at max capabilities. however, so far we have 0 entities.
        // when we need to init a service, we call a proper eo_entities_SetNumOf...(num) method
        // to unit we can call a eo_entities_Reset() for all or a proper eo_entities_SetNumOf...(0)  
        eo_entities_Initialise();        
    }
    
    {   // can-mapping
        // marco.accame on 30 sept 2015: so far it is empty. i will load it later on with boards and with entities.
        // i must write a eo_canmap_LoadEntity() with can boards and info about how the can boards.
        // i also MUST write a eo_canmap_UnloadEntity()
        eo_canmap_Initialise(NULL);
    }
    
    {   // can-protocol
        // marco.accame on 30 sept 2015: i init it in a complete way. we dont have a load method for service ...
        // we may add it, so that if we dont init we dont allow it.
        eo_canprot_Initialise(NULL);        
    }
    
    {   // other services: I dont init anything. however i should avoid automatic initialisation for:
        // EOtheMAIS, EOtheSTRAIN, EOtheSKIN, EOtheInertial, EOtheMotionController     

        // changed idea: i initialise them.
        eo_strain_Initialise();  
        eo_mais_Initialise();        
        eo_motioncontrol_Initialise();    
        eo_skin_Initialise();          
    }
    
    {   // i init a service handler. for instance to be called EOtheServices
        // so far i do in here what i need without any container
             
        // can-services
        eOcanserv_cfg_t config = {.mode = eocanserv_mode_straight};   
        
        config.mode                 = eocanserv_mode_straight;
        config.canstabilizationtime = 7*OSAL_reltime1sec;
        config.rxqueuesize[0]       = 64;
        config.rxqueuesize[1]       = 64;
        config.txqueuesize[0]       = 64;
        config.txqueuesize[1]       = 64;  
        config.onrxcallback[0]      = s_can_cbkonrx; 
        config.onrxargument[0]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
        config.onrxcallback[1]      = s_can_cbkonrx; 
        config.onrxargument[1]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 
            
        // attenzione alla mc4cplus che ha 1 solo can ... potrei cambiare il eo_canserv_Initialise() in modo che
        // se hal_can_supported_is(canx) e' falso allora non retsituisco errore ma semplicemente non inizializzo.
        eo_canserv_Initialise(&config);   
        
        // can-discovery
        eo_candiscovery2_Initialise(NULL);        
    }
    
    
    // B. start a timer of 5 seconds w/ callback which:

       
   s_debug_eb2();
       
}



static void s_debug_eb2(void)  
{
    EOtimer *tmrA = eo_timer_New();    
    
//    errdes_st_eb9.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
//    errdes_st_eb9.sourcedevice     = eo_errman_sourcedevice_localboard;
//    errdes_st_eb9.sourceaddress    = 0;
//    errdes_st_eb9.par16            = 0x0000;
//    errdes_st_eb9.par64            = 0xffff000000000000;        
//    
//    errdes_mc_eb9.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
//    errdes_mc_eb9.sourcedevice     = eo_errman_sourcedevice_localboard;
//    errdes_mc_eb9.sourceaddress    = 0;
//    errdes_mc_eb9.par16            = 0x0000;
//    errdes_mc_eb9.par64            = 0xffff000000000000;
    
    
    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    
    eo_action_SetCallback(act, callback_eb2, tmrA, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
    
    eo_timer_Start(tmrA, eok_abstimeNOW, 10*eok_reltime1sec, eo_tmrmode_FOREVER, act);   
}


static void callback_eb2(void *p)
{
    static uint8_t tick = 0;
    uint8_t boardnumber = 1;
    
    static const eOmn_serv_configuration_t * mc4serv = NULL; 
    
    static const eOmn_serv_configuration_t * maisserv = NULL; 
    
    static const eOmn_serv_configuration_t * skinserv = NULL; 
    
    eObool_t stoptimer = eobool_false;
    
    
    if(NULL == mc4serv)
    {
        mc4serv = eoboardconfig_code2motion_serv_configuration(boardnumber);
    }
    
    if(NULL == maisserv)
    {
        maisserv = eoboardconfig_code2mais_serv_configuration(boardnumber);
    }    


    if(NULL == skinserv)
    {
        skinserv = eoboardconfig_code2skin_serv_configuration(boardnumber);
    }    
    
    if(0 == tick)
    {  
//        hal_trace_puts("eo_mais_Verify() is called"); 
        eo_mais_Verify(eo_mais_GetHandle(), maisserv, s_on_mais_verify_eb2, eobool_true);                    
    }
    else if(1 == tick)
    { 
//        hal_trace_puts("eo_motioncontrol_Verify() is called");
        eo_motioncontrol_Verify(eo_motioncontrol_GetHandle(), mc4serv, s_on_mc4_verify_eb2, eobool_true);  
    }
    else if(2 == tick)
    { 
        eo_skin_Verify(eo_skin_GetHandle(), skinserv, s_on_skin_verify_eb2, eobool_true);  
    }
//    else if(2 == tick)
//    { 
//        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);
//    }
//    else if(3 == tick)
//    {
//        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);
//    }
//    else if(4 == tick)
//    {
//        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);
//    }
//    else if(5 == tick)
//    {
//        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);
//    }
    else
    {
//        errdes_st_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_st_eb9);  
//        
//        errdes_mc_eb9.par64            = 0xffff000000000000 | tick;
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_mc_eb9);         
    }
   
    tick++;
   
   
    if(tick >= 8)
    {
        stoptimer = eobool_true;
    }
   
    if(eobool_true == stoptimer)
    {
        eo_timer_Stop((EOtimer*)p);
    }   
}



static eOresult_t s_on_skin_verify_eb2(EOaService* p, eObool_t verifyisok)
{    
    uint8_t rr = 0;
    
    
    if(eobool_true == verifyisok)
    {
        rr++;
        rr = rr;
    }
    else
    {
        rr = 0;
    }
    
    rr = rr;
    //eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes_st_eb9);
    
    return(eores_OK);    
    
}


static eOresult_t s_on_mc4_verify_eb2(EOaService* p, eObool_t verifyisok)
{
    uint8_t rr = 0;
          
    
    if(eobool_true == verifyisok)
    {
        
        eb9verified = eobool_true;     
        rr++;
        rr = rr;
    }
    else
    {

    }
    
  
    return(eores_OK);
}

static eOresult_t s_on_mais_verify_eb2(EOaService* p, eObool_t verifyisok)
{
    uint8_t rr = 0;
          
    
    if(eobool_true == verifyisok)
    {
        
        eb9verified = eobool_true;     
        rr++;
        rr = rr;
    }
    else
    {

    }
    
  
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------









