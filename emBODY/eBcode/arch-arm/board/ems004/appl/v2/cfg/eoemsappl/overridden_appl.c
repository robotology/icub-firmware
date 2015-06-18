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

#include "EOtheEMSapplBody.h"

#include "EOaction.h"
#include "hl_bits.h"

#include "EOMtheEMSapplCfg_cfg.h"

#include "EOnvSet.h"
#include "EoProtocolMC.h"

#include "iCubCanProto_motorControlMessages.h"

#include "EOtheCANservice.h"
#include "EOtheCANprotocol.h"

#include "EOtheSTRAIN.h"
#include "EOtheSKIN.h"

#include "EOtheBoardConfig.h"


#include "EOVtheEnvironment.h"
#include "EOtheARMenvironment.h"

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
   

static const eOemsapplbody_cfg_t theemsapplbodycfg =
{
    .encoderreaderconfig    =
    {
        .joints = 
        {   // at most 6 joints can be managed. one with an encoder each 
            {   // pos 0
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
            },
            {   // pos 1
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
            },
            {   // pos 2
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
            },
            {   // pos 3
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
            }, 
            {   // pos 4
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
            },
            {   // pos 5
                .primary_encoder        = eo_appEncReader_enc_type_NONE,
                .primary_enc_position   = eo_appEncReader_encoder_positionNONE,
                .extra_encoder          = eo_appEncReader_enc_type_NONE,
                .extra_enc_position     = eo_appEncReader_encoder_positionNONE                
            }            
        },
        .SPI_streams    =
        {   // at most 2 streams: one over spix and one over spiy
            {
                .type       = hal_encoder_tundefined, // or hal_encoder_t1 for aea
                .numberof   = 0
            },
            {
                .type       = hal_encoder_tundefined, // or hal_encoder_t1 for aea
                .numberof   = 0
            }
        },
        .SPI_callbackOnLastRead = NULL,
        .SPI_callback_arg       = NULL       
    }
};

const EOVtheEMSapplCfgBody theapplbodyconfig = 
{
    .type               =   0,
    .thetrueconfig      =   (void*) &theemsapplbodycfg
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


// marco.accame: this function is called inside eom_emsappl_Initialise() just before to run the state machine
// which enters in the CFG state. it is the place where to launch new services

extern void eom_emsappl_hid_userdef_initialise(EOMtheEMSappl* p)
{  
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
        
        //s_boardnum = 0; //it imposes that the board is the eb1
    }
    
    {   // CAN-MAPPING
        // marco.accame on 19 may 2015: here we load the map of can ... and also we map the entities into some can boards
        // ... if we have any.
        EOtheCANmapping * canmap = eo_canmap_Initialise(NULL);
        // now i load the map of can boards
        EOconstvector *canboards = eoboardconfig_code2canboards(s_boardnum);
        eo_canmap_LoadBoards(canmap, canboards);
        // now i load mc-joints, mc-motors, as-strain, as-mais, sk-skin
        EOconstvector *entitydes = NULL;
        // mc
        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, entitydes);
        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, entitydes); 
        // as
        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_analogsensors, eoprot_entity_as_strain);
        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, entitydes);
        entitydes = eoboardconfig_code2entitydescriptors(s_boardnum, eoprot_endpoint_analogsensors, eoprot_entity_as_mais);
        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, entitydes);
        // sk
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
        
    
    // start the application body   
    eOemsapplbody_cfg_t applbodyconfig;
    
    memcpy(&applbodyconfig, &theemsapplbodycfg, sizeof(eOemsapplbody_cfg_t));
    
    // now i get the encoder config
    
    const eOappEncReader_cfg_t *enccfg = eoboardconfig_code2encoderconfig(s_boardnum);
    memcpy(&applbodyconfig.encoderreaderconfig, enccfg, sizeof(eOappEncReader_cfg_t)); 
    
    eo_emsapplBody_Initialise(&applbodyconfig);       
}


extern void eom_emsappl_hid_userdef_on_entry_CFG(EOMtheEMSappl* p)
{    
    // pulse led3 forever at 0.50 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, 2*EOK_reltime1sec, 0);

    // set the EOtheCANservice to be on event and force parsing of all packets in the RX queues.
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
    const uint8_t maxframes2read = 255; // 255 is the max number possible. the function however exits when all canframes are 
    eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport1, maxframes2read, NULL);    
    eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport2, maxframes2read, NULL);
 
}

extern void eom_emsappl_hid_userdef_on_exit_CFG(EOMtheEMSappl* p)
{
//    #warning -> marco.accame: we enable the tx on can for all joints not on exit-cfg but on enytry-run (i may go to err).
}

extern void eom_emsappl_hid_userdef_on_entry_RUN(EOMtheEMSappl* p)
{
    eOresult_t res = eores_NOK_generic;
    
    // enable joints
    res = eo_emsapplBody_EnableTxAllJointOnCan(eo_emsapplBody_GetHandle());
    if (eores_NOK_generic == res)
    {
        //handle the error
    }
    
    // enable the tx mode of strain, if present and as configured
    eo_strain_SendTXmode(eo_strain_GetHandle());
    
    // pulse led3 forever at 1 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/1, 0); 
     
    // set mode on demand. then tx all canframes remained in the tx queue
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_ondemand);
   
    eo_canserv_TXstart(eo_canserv_GetHandle(), eOcanport1, NULL);    
    eo_canserv_TXstart(eo_canserv_GetHandle(), eOcanport2, NULL);    
    
    eo_canserv_TXwaituntildone(eo_canserv_GetHandle(), eOcanport1, 5*eok_reltime1ms);
    eo_canserv_TXwaituntildone(eo_canserv_GetHandle(), eOcanport2, 5*eok_reltime1ms);

    // Start reading the encoders
    eo_appEncReader_StartRead(eo_emsapplBody_GetEncoderReader(eo_emsapplBody_GetHandle()));
}


extern void eom_emsappl_hid_userdef_on_exit_RUN(EOMtheEMSappl* p)
{
    eOresult_t res = eores_NOK_generic;
    
    // set run mode straigth. then read all can frames received
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
    const uint8_t maxframes2read = 255; // 255 is the max number possible. the function however exits when all canframes are 
    eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport1, maxframes2read, NULL);    
    eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport2, maxframes2read, NULL);
  
    
    // stop skin. the check whether to stop skin or not is done internally.
    eo_skin_DisableTX(eo_skin_GetHandle());


    // stop motion control
    res = eo_emsapplBody_DisableTxAllJointOnCan(eo_emsapplBody_GetHandle());
    if (eores_NOK_generic == res)
	{
		// handle the error ... not important
    }
    
    // stop tx of strain, if present
    eo_strain_DisableTX(eo_strain_GetHandle());        
}

extern void eom_emsappl_hid_userdef_on_entry_ERR(EOMtheEMSappl* p)
{
    // pulse led3 forever at 4 hz.
    eo_ledpulser_Start(eo_ledpulser_GetHandle(), eo_ledpulser_led_three, EOK_reltime1sec/4, 0);
   
    eo_canserv_SetMode(eo_canserv_GetHandle(), eocanserv_mode_straight);
    const uint8_t maxframes2read = 255; // 255 is the max number possible. the function however exits when all canframes are 
    eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport1, maxframes2read, NULL);    
    eo_canserv_Parse(eo_canserv_GetHandle(), eOcanport2, maxframes2read, NULL);
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









