
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoManagement.h"
#include "EOnv_hid.h"

#include "EOtheBOARDtransceiver.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "eEsharedServices.h"
#include "EOVtheSystem.h"

#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSrunner.h"

#if defined(USE_EMBOT_theServices)
#include "embot_app_eth_theServices.h"
#else
#include "EOtheServices.h"
#endif

#if defined(USE_EMBOT_theHandler)
#include "theApplication_config.h"
#include "theHandler_Config.h"
#include "embot_app_eth_theHandler.h"
#else
#include "EOMtheEMSappl.h"
#include "EOMtheEMSapplCfg.h"
#endif


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolMN.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eoprot_ep_mn_fun_apply_config(uint32_t period, uint16_t rxtime, uint16_t dotime, uint16_t txtime, uint8_t txratedivider);

static void s_eoprot_ep_mn_fun_apply_config_txratedivider(uint8_t txratedivider);


static void s_eoprot_ep_mn_fun_configcommand(eOmn_command_t* command);

static void s_eoprot_ep_mn_fun_querynumofcommand(eOmn_command_t* command);
static void s_eoprot_ep_mn_fun_queryarraycommand(eOmn_command_t* command);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void eoprot_fun_INIT_mn_comm_status(const EOnv* nv)
{
    eOmn_comm_status_t* status = (eOmn_comm_status_t*)nv->ram;
    
    // 1. init the management protocol version
    
    eOversion_t* version = &status->managementprotocolversion;
    
    version->major = eoprot_version_mn_major;
    version->minor = eoprot_version_mn_minor;
    
    
    // 2. init the transceiver
    
    eOmn_transceiver_properties_t* transp = &status->transceiver;
    
#if defined(USE_EMBOT_theHandler)    
    #warning USE_EMBOT_theHandler is defined. solve how to get transceiver properties
    transp->listeningPort = embot::app::eth::theHandler_EOMtheEMSsocket_Config.localport;
//    transp->destinationPort                 = emscfg->transcfg.hostipv4port;
    transp->maxsizeRXpacket                 = embot::app::eth::theHandler_EOMtheEMSsocket_Config.inpdatagramsizeof;
    transp->maxsizeTXpacket                 = embot::app::eth::theHandler_EOMtheEMSsocket_Config.outdatagramsizeof;
//    transp->maxsizeROPframeRegulars         = emscfg->transcfg.sizes.capacityofropframeregulars;
//    transp->maxsizeROPframeReplies          = emscfg->transcfg.sizes.capacityofropframereplies;
//    transp->maxsizeROPframeOccasionals      = emscfg->transcfg.sizes.capacityofropframeoccasionals;
//    transp->maxsizeROP                      = emscfg->transcfg.sizes.capacityofrop;
//    transp->maxnumberRegularROPs            = emscfg->transcfg.sizes.maxnumberofregularrops;
    memset(transp->filler06, 0, sizeof(transp->filler06));     
#else    
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    
    transp->listeningPort                   = emscfg->socketcfg.localport;
    transp->destinationPort                 = emscfg->transcfg.hostipv4port;
    transp->maxsizeRXpacket                 = emscfg->socketcfg.inpdatagramsizeof;
    transp->maxsizeTXpacket                 = emscfg->socketcfg.outdatagramsizeof;
    transp->maxsizeROPframeRegulars         = emscfg->transcfg.sizes.capacityofropframeregulars;
    transp->maxsizeROPframeReplies          = emscfg->transcfg.sizes.capacityofropframereplies;
    transp->maxsizeROPframeOccasionals      = emscfg->transcfg.sizes.capacityofropframeoccasionals;
    transp->maxsizeROP                      = emscfg->transcfg.sizes.capacityofrop;
    transp->maxnumberRegularROPs            = emscfg->transcfg.sizes.maxnumberofregularrops;
    memset(transp->filler06, 0, sizeof(transp->filler06)); 
#endif    
}



extern void eoprot_fun_UPDT_mn_comm_cmmnds_command_querynumof(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    //eOprotIndex_t index = eoprot_ID2index(nv->ep, nv->id);
    
    eOmn_command_t* command = (eOmn_command_t*)nv->ram;
    
    eOmn_opc_t opc = (eOmn_opc_t)command->cmd.opc;
    
    
    if(eobool_true == eo_nv_hid_isLocal(nv))
    {   // function is called from within the local board
           
        switch(opc)
        {

            case eomn_opc_query_numof_EPs:
            case eomn_opc_query_numof_ENs:
            case eomn_opc_query_numof_REGROPs:          
            {
                s_eoprot_ep_mn_fun_querynumofcommand(command); 
            } break;
            
            default:
            {
            } break;
            
        }        
    }
    else
    {   // function is called from within the remote host because it has received a say or a sig

    }    

}

extern void eoprot_fun_UPDT_mn_comm_cmmnds_command_queryarray(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    //eOprotIndex_t index = eoprot_ID2index(nv->ep, nv->id);
    
    eOmn_command_t* command = (eOmn_command_t*)nv->ram;
    
    eOmn_opc_t opc = (eOmn_opc_t)command->cmd.opc;
    
    
    if(eobool_true == eo_nv_hid_isLocal(nv))
    {   // function is called from within the local board
           
        switch(opc)
        {

            case eomn_opc_query_array_EPs:              
            case eomn_opc_query_array_EPdes:  
            case eomn_opc_query_array_ENdes:
            case eomn_opc_query_array_REGROPs:                
            {
                s_eoprot_ep_mn_fun_queryarraycommand(command); 
            } break;
            
            default:
            {
            } break;
            
        }        
    }
    else
    {   // function is called from within the remote host because it has received a say or a sig

    }    

}

extern void eoprot_fun_UPDT_mn_comm_cmmnds_command_config(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    //eOprotIndex_t index = eoprot_ID2index(nv->ep, nv->id);
    
    eOmn_command_t* command = (eOmn_command_t*)nv->ram;
    
    eOmn_opc_t opc = (eOmn_opc_t)command->cmd.opc;
    
    
    if(eobool_true == eo_nv_hid_isLocal(nv))
    {   // function is called from within the local board
           
        switch(opc)
        {

            case eomn_opc_config_REGROPs_clear:
            case eomn_opc_config_REGROPs_assign:
            case eomn_opc_config_REGROPs_append:
            case eomn_opc_config_REGROPs_remove:
            case eomn_opc_config_PROT_boardnumber:
            case eomn_opc_config_PROT_endpoint:
            {
                s_eoprot_ep_mn_fun_configcommand(command);                  
            } break;
            
            default:
            {
            } break;
            
        }        
    }
    else
    {   // function is called from within the remote host because it has received a say or a sig

    }    

}

extern void eoprot_fun_INIT_mn_appl_config(const EOnv* nv)
{
    eOmn_appl_config_t config = {0};

#if defined(USE_EMBOT_theHandler)    
    #warning USE_EMBOT_theHandler is defined. use it to get the runner config
#else     
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    
    config.cycletime        = emscfg->runobjcfg.period;
    config.txratedivider    = emscfg->runobjcfg.defaultTXdecimationfactor;
#endif          
    // set it
    eo_nv_Set(nv, &config, eobool_true, eo_nv_upd_dontdo);        
}



extern void eoprot_fun_INIT_mn_appl_status(const EOnv* nv)
{
    // i init the application status to ...     
    eOmn_appl_status_t status = {0};

#if defined(USE_EMBOT_theHandler)    

    // build date
    status.buildate.year    = embot::app::eth::theApplication_Config.property.date.year;
    status.buildate.month   = embot::app::eth::theApplication_Config.property.date.month;
    status.buildate.day     = embot::app::eth::theApplication_Config.property.date.day;
    status.buildate.hour    = embot::app::eth::theApplication_Config.property.date.hour;
    status.buildate.min     = embot::app::eth::theApplication_Config.property.date.minute;
    
    // version    
    status.version.major    = embot::app::eth::theApplication_Config.property.version.major;
    status.version.minor    = embot::app::eth::theApplication_Config.property.version.minor;
    
    // control loop timings 
    // ...
    
    // name
    // ...
    
    #warning USE_EMBOT_theHandler -> TODO add other info in eoprot_fun_INIT_mn_appl_status()
    
    // curr state
    status.currstate = applstate_config;
    status.boardtype = eobrd_ethtype_amc;
    
#else     
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    
    // build date
    status.buildate.year        = emscfg->applcfg.emsappinfo->info.entity.builddate.year;
    status.buildate.month       = emscfg->applcfg.emsappinfo->info.entity.builddate.month;
    status.buildate.day         = emscfg->applcfg.emsappinfo->info.entity.builddate.day;
    status.buildate.hour        = emscfg->applcfg.emsappinfo->info.entity.builddate.hour;
    status.buildate.min         = emscfg->applcfg.emsappinfo->info.entity.builddate.min;
    
    // version
    status.version.major        = emscfg->applcfg.emsappinfo->info.entity.version.major;
    status.version.minor        = emscfg->applcfg.emsappinfo->info.entity.version.minor;
		
	// control loop timings 
    status.cloop_timings[0]     = emscfg->runobjcfg.execDOafter;
	status.cloop_timings[1]     = emscfg->runobjcfg.execTXafter - emscfg->runobjcfg.execDOafter;
	status.cloop_timings[2]     = emscfg->runobjcfg.period - emscfg->runobjcfg.execTXafter;
    status.txdecimationfactor   = emscfg->runobjcfg.defaultTXdecimationfactor;
    
    uint16_t min = EO_MIN(sizeof(status.name), sizeof(emscfg->applcfg.emsappinfo->info.name));
    memcpy(status.name, emscfg->applcfg.emsappinfo->info.name, min);


    // curr state
    status.currstate = applstate_config;
    status.boardtype = eobrd_ethtype_unknown;
    
#if defined(USE_EMS4RD)
    status.boardtype = eobrd_ethtype_ems4;
#elif defined(USE_MC4PLUS)
    status.boardtype = eobrd_ethtype_mc4plus;
#elif defined(USE_MC2PLUS)
    status.boardtype = eobrd_ethtype_mc2plus;
#elif defined(STM32HAL_BOARD_AMC)
    status.boardtype = eobrd_ethtype_amc;
#endif

#endif    
    // set it
    eo_nv_Set(nv, &status, eobool_true, eo_nv_upd_dontdo);
}

extern void eoprot_fun_UPDT_mn_appl_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmn_appl_config_t *cfg = (eOmn_appl_config_t*)rd->data;
    
    if(1000 != cfg->cycletime)
    {
        cfg->cycletime = 1000;
    }
    
    if((cfg->maxtimeRX + cfg->maxtimeDO + cfg->maxtimeTX) > cfg->cycletime)
    {
        cfg->maxtimeRX = cfg->cycletime/3;
        cfg->maxtimeDO = cfg->cycletime/3;
        cfg->maxtimeTX = cfg->cycletime - cfg->maxtimeRX - cfg->maxtimeDO;        
    }
    
    if(0 == cfg->txratedivider)
    {
        cfg->txratedivider = 1;
    }
    
    s_eoprot_ep_mn_fun_apply_config(cfg->cycletime, cfg->maxtimeRX, cfg->maxtimeDO, cfg->maxtimeTX, cfg->txratedivider);   
}


extern void eoprot_fun_UPDT_mn_appl_config_txratedivider(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint8_t *txratedivider = (uint8_t*)rd->data;
   
    if(0 == *txratedivider)
    {
        *txratedivider = 1;
    }
    
    s_eoprot_ep_mn_fun_apply_config_txratedivider(*txratedivider);   
}


extern void eoprot_fun_UPDT_mn_appl_cmmnds_go2state(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    eOmn_appl_state_t *go2state = (eOmn_appl_state_t *)nv->ram;
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = (eOmn_appl_status_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);

    eOresult_t res = eores_NOK_generic;
    
    switch(*go2state)
    {
        case applstate_config:
        {
#if defined(USE_EMBOT_theHandler)    
            embot::app::eth::theHandler::getInstance().process(embot::app::eth::theHandler::Command::go2IDLE);
#else             
            res = eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STcfg);
            res = res;
#endif
        } break;

        case applstate_running:
        {
            // we always allow entering the control loop. to be in control loop does nothing unless the service is activated.
            
#if defined(USE_EMBOT_theHandler)    
    #warning USE_EMBOT_theHandler is defined. solve how to request transition to RUN state
            embot::app::eth::theHandler::getInstance().process(embot::app::eth::theHandler::Command::go2RUN);
#else 
            res = eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STrun);
#endif
        } break;
        
        case applstate_error:
        {
#if defined(USE_EMBOT_theHandler)    
    #warning USE_EMBOT_theHandler is defined. solve how to request transition to ERR state
            embot::app::eth::theHandler::getInstance().process(embot::app::eth::theHandler::Command::go2FATALERROR);
#else             
            // i don't expect to receive a go2error command
            res = eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STerr);
#endif            
        } break;

        case applstate_resetmicro:
        {
            // i just reset the micro ... straight away
            ee_sharserv_sys_restart();
        } break;
        
        case applstate_restartapp:
        {
            ee_sharserv_ipc_gotoproc_set(ee_procApplication);
            ee_sharserv_sys_restart();           
        } break;
        
        
        default:
        {
        } break;        
    }

}



extern void eoprot_fun_UPDT_mn_appl_cmmnds_timeset(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    eOabstime_t *timeset = (eOabstime_t *)nv->ram;
    #warning TODO: review eoprot_fun_UPDT_mn_appl_cmmnds_timesetfill()
    
    // first implementation: if the received time is not much different, then i apply it
    
    eOabstime_t currtime = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    int64_t delta = *timeset - currtime;
    
    char str[96];
    uint32_t sec = *timeset/(1000*1000);
    uint32_t tmp = *timeset%(1000*1000);
    uint32_t msec = tmp / 1000;
    uint32_t usec = tmp % 1000;
    uint32_t years = sec/3600/24/365;
    char str0[64];            
    snprintf(str0, sizeof(str0), "s%d m%d u%d", sec, msec, usec);
    snprintf(str, sizeof(str), "RQST of time change to %s [or years = %d", str0, years);
    eo_errman_Trace(eo_errman_GetHandle(), str, "timeset callback");  
    
    eOerrmanDescriptor_t descriptor = {0};
    char msg[64] = {0};    
    
    descriptor.par16 = 0;
    descriptor.par64 = currtime;
    descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
    descriptor.sourceaddress = 0;
    descriptor.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    snprintf(msg, sizeof(msg), "synch rqst = %lld", *timeset);
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, msg, NULL, &descriptor); 
    
    
    //
    // first implementation: if the received time is not much different, then i apply it

    eObool_t apply = eobool_false;
    if(delta > 0)
    {
        // we go in the future. do we go much?
        if(delta >= eok_reltime1ms)
        {
            apply = eobool_true;
        }        
    }
    else
    {
        // it is either zero or negative (we go in the past)
        delta = -delta;
        if(delta >= eok_reltime1ms)
        {
            apply = eobool_true;
        }        
    }
    
    if(eobool_true == apply)
    {
        eov_sys_LifeTimeSet(eov_sys_GetHandle(), *timeset);
    }
    
    
    currtime = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
    descriptor.par16 = apply;
    descriptor.par64 = currtime;
    descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
    descriptor.sourceaddress = 0;
    descriptor.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
    snprintf(msg, sizeof(msg), "time = %lld", currtime);
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, msg, NULL, &descriptor); 
              
}


extern void eoprot_fun_UPDT_mn_service_cmmnds_command(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmn_service_cmmnds_command_t *command = (eOmn_service_cmmnds_command_t *)nv->ram;
    
    // ok, we have received a command for a given service. we ask the object theservices to manage the thing
#if defined(USE_EMBOT_theServices)
    embot::app::eth::theServices::getInstance().process(command);
#else    
    eo_services_ProcessCommand(eo_services_GetHandle(), command);
#endif    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eoprot_ep_mn_fun_querynumofcommand(eOmn_command_t* command)
{
    EOtransceiver* theems00transceiver; 
    eOmn_cmd_querynumof_t* cmdquerynumof = (eOmn_cmd_querynumof_t*)&command->cmd;
    eOmn_cmd_replynumof_t* cmdreplynumof = (eOmn_cmd_replynumof_t*)&command->cmd;

    eOropdescriptor_t ropdesc;
    
    eOmn_opc_t opc = (eOmn_opc_t) cmdquerynumof->opcpar.opc;

    
    if(NULL == (theems00transceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle())))
    {
        return;
    }
    
    // retrieve all useful parametes
    uint8_t endpoint    = cmdquerynumof->opcpar.endpoint;

    // then clean data to be sent back:
    memset(command, 0, sizeof(eOmn_command_t));

       
    switch(opc)
    {
    
        case eomn_opc_query_numof_EPs:
        {   // must give back the number of endpoints                   
            cmdreplynumof->opcpar.opc       = eomn_opc_reply_numof_EPs;  
            cmdreplynumof->opcpar.endpoint  = eoprot_endpoint_all;
            cmdreplynumof->opcpar.numberof  = eoprot_endpoints_numberof_get(eoprot_board_localboard);

            
            // ok, now i send the occasional rop
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replynumof);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.
#if defined(USE_EMBOT_theHandler)    
//            #warning USE_EMBOT_theHandler is defined. SOLVED how to transmit an occasional ROP
            eom_emstransceiver_Transmit_OccasionalROP(eom_emstransceiver_GetHandle(), &ropdesc);
#else             
            eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);    // it also alert someone to send the rop   
#endif            
        } break;
              

        case eomn_opc_query_numof_ENs:
        {   // must give back the number of entities            
            cmdreplynumof->opcpar.opc       = eomn_opc_reply_numof_ENs;
            cmdreplynumof->opcpar.endpoint  = endpoint;
            if(eoprot_endpoint_all == endpoint)
            {
                cmdreplynumof->opcpar.numberof  = eoprot_entities_numberof_get(eoprot_board_localboard);
            }
            else
            {
                cmdreplynumof->opcpar.numberof = eoprot_entities_in_endpoint_numberof_get(eoprot_board_localboard, endpoint);
            }
           
            // ok, now i send the occasional rop
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replynumof);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.
#if defined(USE_EMBOT_theHandler)    
//            #warning USE_EMBOT_theHandler is defined. SOLVED how to transmit an occasional ROP
            eom_emstransceiver_Transmit_OccasionalROP(eom_emstransceiver_GetHandle(), &ropdesc);
#else             
            eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);           
#endif            
        } break;
       
        
        case eomn_opc_query_numof_REGROPs:
        {   
            cmdreplynumof->opcpar.opc       = eomn_opc_reply_numof_REGROPs;
            cmdreplynumof->opcpar.endpoint  = endpoint;
            
            if(eoprot_endpoint_all == endpoint)
            {
                cmdreplynumof->opcpar.numberof  = eo_transceiver_RegularROP_ArrayID32Size(theems00transceiver);
            }
            else
            {
                cmdreplynumof->opcpar.numberof  = eo_transceiver_RegularROP_ArrayID32SizeWithEP(theems00transceiver, endpoint);    
            }
            
            // ok, now i send the occasional rop
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replynumof);
            ropdesc.data    = NULL; // so that data from the EOnv is retrieved.
#if defined(USE_EMBOT_theHandler)    
//            #warning USE_EMBOT_theHandler is defined. SOLVED how to transmit an occasional ROP
            eom_emstransceiver_Transmit_OccasionalROP(eom_emstransceiver_GetHandle(), &ropdesc);
#else             
            eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
#endif            
           
        } break;
 
        
        default:
        {
            
        } break;
    }
    
    
}


static void s_eoprot_ep_mn_fun_queryarraycommand(eOmn_command_t* command)
{
    EOtransceiver* theems00transceiver; 
    eOmn_cmd_queryarray_t* cmdqueryarray = (eOmn_cmd_queryarray_t*)&command->cmd;
    eOmn_cmd_replyarray_t* cmdreplyarray = (eOmn_cmd_replyarray_t*)&command->cmd;

    eOropdescriptor_t ropdesc;
    
    eOmn_opc_t opc = (eOmn_opc_t) cmdqueryarray->opcpar.opc;

    
    if(NULL == (theems00transceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle())))
    {
        return;
    }
    
    // retrieve all useful parametes
    uint8_t endpoint    = cmdqueryarray->opcpar.endpoint;
    uint8_t setnumber   = cmdqueryarray->opcpar.setnumber;
    uint8_t setsize     = cmdqueryarray->opcpar.setsize; 
    // then clean data to be sent back:
    memset(command, 0, sizeof(eOmn_command_t));

       
    switch(opc)
    {
           
        case eomn_opc_query_array_EPs:
        {
            uint8_t capacity = (sizeof(cmdreplyarray->array) - sizeof(eOarray_head_t)) / sizeof(eOnvEP8_t);
            if(0 == setsize)
            {
                setsize = capacity;   
            }                
            
            // ok... now i form a rop to send back. at first i write into the nv.
            EOarray* ep08array = eo_array_New(setsize, sizeof(eOnvEP8_t), cmdreplyarray->array);
            //uint8_t total = eoprot_endpoints_numberof_get(eoprot_board_localboard);
                                  
            // ok, prepare the occasional rops
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replyarray);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.            

            eo_array_Reset(ep08array);
            cmdreplyarray->opcpar.opc       = eomn_opc_reply_array_EPs;
            cmdreplyarray->opcpar.endpoint  = eoprot_endpoint_all;
            cmdreplyarray->opcpar.setnumber = setnumber;
            cmdreplyarray->opcpar.setsize   = setsize;
            eoprot_endpoints_array_get(eoprot_board_localboard, ep08array, setnumber*setsize);    
#if defined(USE_EMBOT_theHandler)    
//            #warning USE_EMBOT_theHandler is defined. SOLVED how to transmit an occasional ROP
            eom_emstransceiver_Transmit_OccasionalROP(eom_emstransceiver_GetHandle(), &ropdesc);
#else             
            eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
#endif           
        } break;      

        
        case eomn_opc_query_array_EPdes:
        {   
            uint8_t capacity = (sizeof(cmdreplyarray->array) - sizeof(eOarray_head_t)) / sizeof(eoprot_endpoint_descriptor_t);
            if(0 == setsize)
            {
                setsize = capacity;   
            }  
            
            EOarray* epdesarray = eo_array_New(setsize, sizeof(eoprot_endpoint_descriptor_t), cmdreplyarray->array);
            //uint8_t total = eoprot_endpoints_numberof_get(eoprot_board_localboard);                 
            
            // ok, prepare the occasional rops
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replyarray);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.            

            eo_array_Reset(epdesarray);
            cmdreplyarray->opcpar.opc       = eomn_opc_reply_array_EPdes;
            cmdreplyarray->opcpar.endpoint  = eoprot_endpoint_all;
            cmdreplyarray->opcpar.setnumber = setnumber;
            cmdreplyarray->opcpar.setsize   = setsize;
            eoprot_endpoints_arrayofdescriptors_get(eoprot_board_localboard, epdesarray, setnumber*setsize);   
#if defined(USE_EMBOT_theHandler)    
//            #warning USE_EMBOT_theHandler is defined. SOLVED how to transmit an occasional ROP
            eom_emstransceiver_Transmit_OccasionalROP(eom_emstransceiver_GetHandle(), &ropdesc);
#else             
            eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
#endif           
        } break;           

        
        case eomn_opc_query_array_ENdes:
        {            
            uint8_t capacity = (sizeof(cmdreplyarray->array) - sizeof(eOarray_head_t)) / sizeof(eoprot_entity_descriptor_t);
            if(0 == setsize)
            {
                setsize = capacity;   
            }              
            
            EOarray* endesarray = eo_array_New(setsize, sizeof(eoprot_entity_descriptor_t), cmdreplyarray->array);
            //uint8_t total = eoprot_entities_numberof_get(eoprot_board_localboard);
            
            // ok, prepare the occasional rops
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replyarray);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.            

            eo_array_Reset(endesarray);
            cmdreplyarray->opcpar.opc       = eomn_opc_reply_array_ENdes;
            cmdreplyarray->opcpar.endpoint  = endpoint;
            cmdreplyarray->opcpar.setnumber = setnumber;
            cmdreplyarray->opcpar.setsize   = setsize;
            
            if(eoprot_endpoint_all == endpoint) 
            {                            
                eoprot_entities_arrayofdescriptors_get(eoprot_board_localboard, endesarray, setnumber*setsize); 
            }
            else
            {
                eoprot_entities_in_endpoint_arrayofdescriptors_get(eoprot_board_localboard, endpoint, endesarray, setnumber*setsize);
            }
#if defined(USE_EMBOT_theHandler)    
//            #warning USE_EMBOT_theHandler is defined. SOLVED how to transmit an occasional ROP
            eom_emstransceiver_Transmit_OccasionalROP(eom_emstransceiver_GetHandle(), &ropdesc);
#else             
            eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
#endif
           
        } break;  
        
        
        case eomn_opc_query_array_REGROPs:
        {
            uint8_t capacity = (sizeof(cmdreplyarray->array) - sizeof(eOarray_head_t)) / sizeof(eOnvID32_t);
            if(0 == setsize)
            {
                setsize = capacity;   
            }  

            // ok... now i form a rop to send back. at first i write into the nv.
            EOarray* id32array = eo_array_New(setsize, sizeof(eOnvID32_t), cmdreplyarray->array);
            // we have total rops ...
            uint8_t total = eo_transceiver_RegularROP_ArrayID32Size(theems00transceiver);            

            
            // ok, prepare the occasional rops
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replyarray);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.            

            eo_array_Reset(id32array);
            cmdreplyarray->opcpar.opc       = eomn_opc_reply_array_REGROPs;
            cmdreplyarray->opcpar.endpoint  = endpoint;
            cmdreplyarray->opcpar.setnumber = setnumber;
            cmdreplyarray->opcpar.setsize   = setsize;
            
            if(eoprot_endpoint_all == endpoint)
            {
                eo_transceiver_RegularROP_ArrayID32Get(theems00transceiver, setnumber*setsize, id32array);    
            }
            else
            {    
                eo_transceiver_RegularROP_ArrayID32GetWithEP(theems00transceiver, endpoint, setnumber*setsize, id32array);
            }
#if defined(USE_EMBOT_theHandler)    
//            #warning USE_EMBOT_theHandler is defined. SOLVED how to transmit an occasional ROP
            eom_emstransceiver_Transmit_OccasionalROP(eom_emstransceiver_GetHandle(), &ropdesc);
#else            
            eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
#endif           
        } break;         
        
        default:
        {
            
        } break;
    }
    
    
}

static void s_eoprot_ep_mn_fun_configcommand(eOmn_command_t* command)
{
    uint8_t size, i;
    eOropSIGcfg_t *sigcfg;
    eOropdescriptor_t ropdesc;
    EOtransceiver* theems00transceiver; 
    
    //eOmn_cmd_config_t* cmdconfig = (eOmn_cmd_config_t*)&command->cmd; /////// not correct.
    eOmn_cmd_config_t* cmdconfig = &command->cmd.config;
    EOarray *array = (EOarray*)cmdconfig->array;
    
    uint16_t targetcapacity = (sizeof(cmdconfig->array) - sizeof(eOarray_head_t))  / sizeof(eOropSIGcfg_t);
    
    
    eOmn_opc_t opc = (eOmn_opc_t) cmdconfig->opcpar.opc;

    eOresult_t res;
    
    if(NULL == (theems00transceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle())))
    {
        return;
    }
    
    
    switch(opc)
    {
    
        case eomn_opc_config_REGROPs_clear:
        {   // just clear
            eo_transceiver_RegularROPs_Clear(theems00transceiver);
        } break;
        
        case eomn_opc_config_REGROPs_assign:
        {   // clear and load all the sigcfg in the array
      
            if((eo_array_ItemSize(array) != sizeof(eOropSIGcfg_t)) || (targetcapacity != eo_array_Capacity(array)) || ((size = eo_array_Size(array)) > targetcapacity))
            {
                return;
            }  
            
            eo_transceiver_RegularROPs_Clear(theems00transceiver);

            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
                ropdesc.control.plustime        = (eobool_true == cmdconfig->opcpar.plustime) ? (1) : (0);
                ropdesc.control.plussign        = (eobool_true == cmdconfig->opcpar.plussign) ? (1) : (0);
                ropdesc.ropcode                 = eo_ropcode_sig;
                ropdesc.id32                    = sigcfg->id32;    
                ropdesc.signature               = cmdconfig->opcpar.signature;   
                res = eo_transceiver_RegularROP_Load(theems00transceiver, &ropdesc);
                res = res;
                if(eores_OK != res)
                {
                    eOerrmanDescriptor_t errdes = {0};
                    errdes.sourcedevice = eo_errman_sourcedevice_localboard;
                    errdes.sourceaddress = 0;
                    errdes.par16 = 0;
                    errdes.par64 = sigcfg->id32; 
                    errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_comm_cannotloadaregularrop);              
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
                }
            }        
        } break;
        
        case eomn_opc_config_REGROPs_append:
        {   // dont clear and load all the sigcfg in the array
            if((eo_array_ItemSize(array) != sizeof(eOropSIGcfg_t)) || (targetcapacity != eo_array_Capacity(array)) || ((size = eo_array_Size(array)) > targetcapacity))
            {
                return;
            }            
            
            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
                ropdesc.control.plustime        = (eobool_true == cmdconfig->opcpar.plustime) ? (1) : (0);
                ropdesc.control.plussign        = (eobool_true == cmdconfig->opcpar.plussign) ? (1) : (0);
                ropdesc.ropcode                 = eo_ropcode_sig;
                ropdesc.id32                    = sigcfg->id32;
                ropdesc.signature               = cmdconfig->opcpar.signature;
                res = eo_transceiver_RegularROP_Load(theems00transceiver, &ropdesc);
                res = res;
                if(eores_OK != res)
                {
                    eOerrmanDescriptor_t errdes = {0};
                    errdes.sourcedevice = eo_errman_sourcedevice_localboard;
                    errdes.sourceaddress = 0;
                    errdes.par16 = 0;
                    errdes.par64 = sigcfg->id32; 
                    errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_comm_cannotloadaregularrop);              
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
                }
            }         
        } break;        

        case eomn_opc_config_REGROPs_remove:
        {   // remove all the sigcfg in the array
            if((eo_array_ItemSize(array) != sizeof(eOropSIGcfg_t)) || (targetcapacity != eo_array_Capacity(array)) || ((size = eo_array_Size(array)) > targetcapacity))
            {
                return;
            }            
            
            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
                ropdesc.control.plustime        = (eobool_true == cmdconfig->opcpar.plustime) ? (1) : (0);
                ropdesc.control.plussign        = (eobool_true == cmdconfig->opcpar.plussign) ? (1) : (0);
                ropdesc.ropcode                 = eo_ropcode_sig;
                ropdesc.id32                    = sigcfg->id32;
                ropdesc.signature               = cmdconfig->opcpar.signature;
                res = eo_transceiver_RegularROP_Unload(theems00transceiver, &ropdesc);
                res = res;
            }         
        } break;          

        case eomn_opc_config_PROT_boardnumber:
        {   // simply set the byte in array[0] as the new localboard number
            //eOprotBRD_t brdnum = command->cmd.config.array[0];
            eoprot_config_board_local(command->cmd.config.array[0]);                
        } break;
        
        case eomn_opc_config_PROT_endpoint:
        {   // simply use the bytes in array[0->7] as a eOprot_EPcfg_t. but only if the EP is valid and not loaded yet.
            eOprot_EPcfg_t *epcfg = (eOprot_EPcfg_t*) &command->cmd.config.array[0];
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                if(eobool_false == eoprot_endpoint_configured_is(eoprot_board_localboard, epcfg->endpoint))
                {
                    EOnvSet* nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle());
                    eo_nvset_LoadEP(nvset, epcfg, eobool_true);                        
                }                  
            }        
        } break;

            
        default:
        {
            
        } break;
    }


}


static void s_eoprot_ep_mn_fun_apply_config_txratedivider(uint8_t txratedivider)
{
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = (eOmn_appl_status_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    status->txdecimationfactor = txratedivider;   
   
    eom_emsrunner_Set_TXdecimationFactor(eom_emsrunner_GetHandle(), txratedivider);    
}

static void s_eoprot_ep_mn_fun_apply_config(uint32_t period, uint16_t rxtime, uint16_t dotime, uint16_t txtime, uint8_t txratedivider)
{
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_appl, 0, eoprot_tag_mn_appl_status);
    eOmn_appl_status_t *status = (eOmn_appl_status_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);

    status->cloop_timings[0] = rxtime;
    status->cloop_timings[1] = dotime;
    status->cloop_timings[2] = txtime;
    status->txdecimationfactor = txratedivider;   
    
    eOemsrunner_timing_t timing = {0};
    timing.period = period; 
    timing.safetygap = 0;
    timing.rxstartafter = 0;
    timing.dostartafter = rxtime;    
    timing.txstartafter = rxtime + dotime;
    
    eom_emsrunner_SetTiming(eom_emsrunner_GetHandle(), &timing); 
    
    eom_emsrunner_Set_TXdecimationFactor(eom_emsrunner_GetHandle(), txratedivider);    
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

