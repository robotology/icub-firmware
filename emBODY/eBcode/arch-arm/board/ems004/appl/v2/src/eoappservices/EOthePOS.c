
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheServices_hid.h"

#include "stdlib.h"
#include "string.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

#include "EoProtocolAS.h"

#include "EOMtheEMSappl.h"

#include "EOVtheCallbackManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOthePOS.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOthePOS_hid.h"

#if defined(EOTHESERVICES_disable_thePOS)

    // provide empty implementation, so that we dont need to change the caller of the API
    
    extern EOthePOS* eo_pos_Initialise(void) 
    {   
        return NULL; 
    }

    extern EOthePOS* eo_pos_GetHandle(void)   
    { 
        return NULL; 
    }

    extern eOmn_serv_state_t eo_pos_GetServiceState(EOthePOS *p) 
    { 
        return eomn_serv_state_notsupported; 
    }
    
    // in some cases, we need to alert the pc104 that the board does not support this service
    extern eOresult_t eo_pos_SendReport(EOthePOS *p)
    {
        static const char s_eobj_ownname[] = "EOthePOS";
        eOerrmanDescriptor_t errdes = {};
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_pos_failed_notsupported);  
        errdes.sourcedevice = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress = 0;
        errdes.par16 = errdes.par64 = 0;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);
        return eores_OK;
    }


    extern eOresult_t eo_pos_Verify(EOthePOS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
    {
        // we alert the host that the verification of the service has failed
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, eomn_serv_state_failureofverify);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        } 
        
        // we tell that the reason is that this service is not supported
        eo_pos_SendReport(NULL);
               
        return eores_NOK_generic;
    }

    extern eOresult_t eo_pos_Activate(EOthePOS *p, const eOmn_serv_configuration_t * servcfg)
    {
        eo_pos_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_pos_Deactivate(EOthePOS *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_pos_Start(EOthePOS *p)
    {
        eo_pos_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_pos_SetRegulars(EOthePOS *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
    {
        if(NULL != arrayofid32)
        {
            eo_pos_SendReport(NULL);
        }
        return eores_NOK_generic;
    }

    extern eOresult_t eo_pos_Tick(EOthePOS *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_pos_Stop(EOthePOS *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_pos_Transmission(EOthePOS *p, eObool_t on)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_pos_Config(EOthePOS *p, eOas_pos_config_t* config)
    {
        eo_pos_SendReport(NULL);
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_pos_AcceptCANframe(EOthePOS *p, eOcanframe_t *frame, eOcanport_t port)
    {
        return eores_NOK_generic;
    }


#elif !defined(EOTHESERVICES_disable_thePOS)

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_pos_presenceofcanboards_reset(EOthePOS *p);

static eOresult_t s_eo_pos_TXstart(EOthePOS *p);

static eOresult_t s_eo_pos_TXstop(EOthePOS *p);

static eOresult_t s_eo_pos_onstop_search4canboards(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok);

static void s_eo_pos_send_periodic_error_report(void *par);

static eObool_t s_eo_pos_isID32relevant(uint32_t id32);

static void s_eo_pos_transmission(EOthePOS *p, eObool_t on, uint8_t period);

static void s_eo_pos_boards_configure(EOthePOS *p);

static void s_eo_pos_presenceofcanboards_reset(EOthePOS *p);
static void s_eo_pos_presenceofcanboards_build(EOthePOS *p);
static void s_eo_pos_presenceofcanboards_start(EOthePOS *p);
static void s_eo_pos_presenceofcanboards_touch(EOthePOS *p, eObrd_canlocation_t loc);
static void s_eo_pos_presenceofcanboards_tick(EOthePOS *p);

static eOresult_t s_eo_pos_OnServiceAlreadyActive(EOthePOS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOas_pos_config_t s_eo_default_posconfig =
{
    .datarate = 50
};

static EOthePOS s_eo_thepos = 
{
    EO_INIT(.service) 
    {
        EO_INIT(.initted)               eobool_false,
        EO_INIT(.active)                eobool_false,
        EO_INIT(.activateafterverify)   eobool_false,
        EO_INIT(.started)               eobool_false,
        EO_INIT(.onverify)              NULL,
        EO_INIT(.state)                 eomn_serv_state_notsupported,
        EO_INIT(.tmpcfg)                NULL,
        EO_INIT(.servconfig)            { EO_INIT(.type) eomn_serv_NONE },
    },
    EO_INIT(.diagnostics) 
    {
        EO_INIT(.reportTimer)           NULL,
        EO_INIT(.reportPeriod)          0, // 10*EOK_reltime1sec, // with 0 we dont periodically report
        EO_INIT(.errorDescriptor)       {0},
        EO_INIT(.errorType)             eo_errortype_info,
        EO_INIT(.errorCallbackCount)    0, 
        EO_INIT(.repetitionOKcase)      0 // 10 // with 0 we transmit report only once at succesful activation
    },     
    EO_INIT(.sharedcan)
    {
        EO_INIT(.boardproperties)       NULL,
        EO_INIT(.entitydescriptor)      NULL,
        EO_INIT(.discoverytargets)      NULL,
        EO_INIT(.ondiscoverystop)       {0},
        EO_INIT(.command)               {0}, 
    }, 

    .configured = eobool_false,
    .posconfig = {0},

    EO_INIT(.id32)                      eo_prot_ID32dummy,  
    EO_INIT(.pos)                       NULL,
    EO_INIT(.id32ofregulars)            NULL,
    EO_INIT(.numberofowners)            0,
    
    
    .not_heardof_target         = {0},
    .not_heardof_status         = {0},
    .not_heardof_counter        = 0,
    .not_heardof_checkrate      = 100,
    .transmissionisactive       = eobool_false,
    .isalive                    = eobool_false
};

static const char s_eobj_ownname[] = "EOthePOS";


const eOmn_serv_config_data_as_pos_t eo_pos_servconf_data_as_pos = 
{
    .version = 
    {
        .firmware = 
        {
            .major = 1, .minor = 1, .build = 0
        },
        .protocol = 
        {
            .major = 1, .minor = 1
        }
    },
    .boardInfo = 
    {
        .canloc = 
        {
            { 
                .port = eOcanport1, 
                .addr = 1, 
                .insideindex = eobrd_caninsideindex_none, 
                .dummy = 0 
            }        
        }
    }
};

const eOmn_serv_configuration_t eo_pos_servconf = 
{
    .type = eomn_serv_AS_pos,
    .filler = {0},
    .data.as.pos = 
    {
        .version = 
        {
            .firmware = 
            {
                .major = 1, .minor = 1, .build = 0
            },
            .protocol = 
            {
                .major = 1, .minor = 1
            }
        },
        .boardInfo = 
        {
            .canloc = 
            {
                { 
                    .port = eOcanport1, 
                    .addr = 1, 
                    .insideindex = eobrd_caninsideindex_none, 
                    .dummy = 0 
                }        
            }
        }        
        
    }
};

#define REM_WDT

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#warning TODO: increase eOas_pos_boardinfos_maxnumber if you want to support more than one can board
enum { eOas_pos_boardinfos_maxnumber = 1 };

extern EOthePOS* eo_pos_Initialise(void)
{
    EOthePOS* p = &s_eo_thepos;

    if(eobool_true == p->service.initted)
    {
        return(p);
    }
    
    p->service.servconfig.type = eomn_serv_NONE;
    
    p->sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), eo_pos_maxnumberofCANboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_pos_maxnumberofCANboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.discoverytargets = eo_array_New(eOas_pos_boardinfos_maxnumber, sizeof(eOcandiscovery_target_t),  NULL);
    
    p->configured = eobool_false;
    
    p->id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_pos, 0, eoprot_tag_none);
        
    s_eo_pos_presenceofcanboards_reset(p);
    
    p->pos = NULL;
    p->id32ofregulars = eo_array_New(pos_maxRegulars, sizeof(uint32_t), NULL);
    p->numberofowners = 0;
    

    memcpy(&p->posconfig, &s_eo_default_posconfig, sizeof(eOas_pos_config_t));
    
    p->diagnostics.reportTimer = eo_timer_New();
    p->diagnostics.errorType = eo_errortype_error;
    p->diagnostics.errorDescriptor.sourceaddress = 0;
    p->diagnostics.errorDescriptor.sourcedevice = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_pos_not_verified_yet);  
        
    p->service.initted = eobool_true;    
    p->service.active = eobool_false;
    p->service.started = eobool_false;
    p->transmissionisactive = eobool_false;
    p->service.state = eomn_serv_state_idle;    
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);
       
    return(p);   
}


extern EOthePOS* eo_pos_GetHandle(void)
{
    if(eobool_true == s_eo_thepos.service.initted)
    {
        return(&s_eo_thepos);
    }
    
    return(NULL);
}


extern eOmn_serv_state_t eo_pos_GetServiceState(EOthePOS *p)
{
    if(NULL == p)
    {
        return(eomn_serv_state_notsupported);
    } 

    return(p->service.state);
}


extern eOresult_t eo_pos_SendReport(EOthePOS *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    eOerror_value_t errorvalue = eoerror_code2value(p->diagnostics.errorDescriptor.code);
    
    switch(errorvalue)
    {
        case eoerror_value_CFG_pos_failed_candiscovery:
        {
            eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());            
        } break;
        
        default:
        {
            // dont send any additional info
        } break;
    }       
    
    return(eores_OK);      
}


extern eOresult_t eo_pos_Verify(EOthePOS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called: eo_pos_Verify()", s_eobj_ownname);
    
    if((NULL == p) || (NULL == servcfg))
    {
        s_eo_thepos.service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, s_eo_thepos.service.state);
        
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }            
        return(eores_NOK_nullpointer);
    }  
    
    if(eomn_serv_AS_pos != servcfg->type)
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }          
        return(eores_NOK_generic);
    } 


    if(eobool_true == p->service.active)
    {   // if already active: we dont deactivate-reactivate. we just check that the running service is ok with the new servcfg 
        return s_eo_pos_OnServiceAlreadyActive(p, servcfg, onverify);
    }     
 
    
    p->service.state = eomn_serv_state_verifying;   
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);

    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);      

    p->service.onverify = onverify;
    p->service.activateafterverify = activateafterverify;

    eOcandiscovery_target_t trgt = {0};
     
    #warning TODO: adapt to ths case of multiple can boards
    trgt.info.type = eobrd_cantype_pmc; // servcfg->data.as.pos.boardsInfo.canprop[0].type;
    trgt.info.protocol.major = servcfg->data.as.pos.version.protocol.major; 
    trgt.info.protocol.minor = servcfg->data.as.pos.version.protocol.minor;
    trgt.info.firmware.major = servcfg->data.as.pos.version.firmware.major; 
    trgt.info.firmware.minor = servcfg->data.as.pos.version.firmware.minor;
    trgt.info.firmware.build = servcfg->data.as.pos.version.firmware.build;  
    for(uint8_t i=0; i<eo_pos_maxnumberofCANboards; i++)
    {
        trgt.canmap[servcfg->data.as.pos.boardInfo.canloc[i].port] |= (0x0001 << servcfg->data.as.pos.boardInfo.canloc[i].addr);
        //trgt.canmap[servcfg->data.as.pos.boardsInfo.canprop[i].location.port] |= (0x0001 << servcfg->data.as.pos.boardsInfo.canprop[i].location.addr); 
    }

    // force a cleaned discoverytargets before we add the target
    eo_array_Reset(p->sharedcan.discoverytargets);    
    eo_array_PushBack(p->sharedcan.discoverytargets, &trgt);
    
    p->sharedcan.ondiscoverystop.function = s_eo_pos_onstop_search4canboards;
    p->sharedcan.ondiscoverystop.parameter = (void*)servcfg;
        
    // start discovery    
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), p->sharedcan.discoverytargets, &p->sharedcan.ondiscoverystop);   
    
    return(eores_OK);   
}

extern uint8_t eo_pos_GetNumberOfOwners(EOthePOS *p)
{
    if(NULL == p)
    {
        return(0);
    } 

    return(p->numberofowners);   
}

extern eOresult_t eo_pos_Deactivate(EOthePOS *p)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called: eo_pos_Deactivate()", s_eobj_ownname);
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == p->service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        p->service.state = eomn_serv_state_idle; 
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);
        return(eores_OK);        
    } 
    
    // we call the Stop() which decrements numberofowners and does its stop operations only if numberofowners is 0/
    // in such a case: is send stop messages to pos, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object       
    if(eobool_true == p->service.started)
    {
        eo_pos_Stop(p);
    }
    
    // _Stop() has decremented numberofowners and we can _Deactivate() only when there is no owner anymore
    if(0 != p->numberofowners)
    {   
        return(eores_OK); 
    }
    
    eo_pos_SetRegulars(p, NULL, NULL);
    
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_pos, p->sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfPOSs(eo_entities_GetHandle(), 0);
    
    p->pos = NULL;
    
    memset(&p->service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    p->service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(p->sharedcan.boardproperties);
    eo_vector_Clear(p->sharedcan.entitydescriptor);
    eo_array_Reset(p->sharedcan.discoverytargets);
    
    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer); 
    
    p->service.active = eobool_false;    
    p->service.state = eomn_serv_state_idle; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);
    
    return(eores_OK);
}


extern eOresult_t eo_pos_Activate(EOthePOS *p, const eOmn_serv_configuration_t * servcfg)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called: eo_pos_Activate()", s_eobj_ownname);
    
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eomn_serv_AS_pos != servcfg->type)
    {
        return(eores_NOK_generic);
    }    
    
    if(eobool_true == p->service.active)
    {   // we dont activate if already active. 
        return s_eo_pos_OnServiceAlreadyActive(p, servcfg, NULL);
    }   
    
    eo_entities_SetNumOfPOSs(eo_entities_GetHandle(), 1);
    

    if(0 == eo_entities_NumOfPOSs(eo_entities_GetHandle()))
    {
        p->service.active = eobool_false;
        return(eores_NOK_generic);
    }
    
    p->pos = eo_entities_GetPOS(eo_entities_GetHandle(), 0);    
    
    
    // now service config stuff
    memcpy(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));   

    
    // now i build maps for check of presence    
    s_eo_pos_presenceofcanboards_build(p);

           
    
    
    // now i must build data for the board and entity for EOtheCANmapping object
    
    // boards
    for(uint8_t i=0; i<eo_pos_maxnumberofCANboards; i++)
    {
        eObrd_canproperties_t prop = {0};
        prop.type = eobrd_cantype_pmc;
        prop.location.port = servcfg->data.as.pos.boardInfo.canloc[i].port;
        prop.location.addr = servcfg->data.as.pos.boardInfo.canloc[i].addr;
        prop.location.insideindex = eobrd_caninsideindex_none;
        prop.requiredprotocol.major = servcfg->data.as.pos.version.protocol.major;
        prop.requiredprotocol.minor = servcfg->data.as.pos.version.protocol.minor;
        
        eo_vector_PushBack(p->sharedcan.boardproperties, &prop);
    }
       
    
    // entity
    for(uint8_t i=0; i<eo_pos_maxnumberofCANboards; i++)
    {
        eOcanmap_entitydescriptor_t des = {0};
        des.location.port = servcfg->data.as.pos.boardInfo.canloc[i].port;
        des.location.addr = servcfg->data.as.pos.boardInfo.canloc[i].addr;
        des.location.insideindex = eobrd_caninsideindex_none;
        des.index = entindex00; // we have only one pos        
        
        eo_vector_PushBack(p->sharedcan.entitydescriptor, &des);
    }
    
    // load the boards
    eo_canmap_LoadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties);  
    
    // load the entity 
    eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_pos, p->sharedcan.entitydescriptor);   

    #warning TBD: non preparo la mappa, configuro le board
    // build the maps that can translate a received can message into the correct place of the six values.
    // the location of the board will be used as an index in a map for the positions 0, 1, 2, 3, 4, 5
    // by that i dont need to send any particular configuration to the pos boards to fill the label by themselves.
//    s_eo_pos_build_maps(p, 0xffffffff);
    
    p->service.active = eobool_true;    
    p->service.state = eomn_serv_state_activated;   
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);        

    
    return(eores_OK);   
}

 
    
extern eOresult_t eo_pos_Start(EOthePOS *p)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called: eo_pos_Start()", s_eobj_ownname);
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    if(eobool_true == p->service.started)
    {   // it is already running. however we increment number of owners
        p->numberofowners ++;
        return(eores_OK);
    }   
    
    p->numberofowners = 1; // dont increment. just set to 1.
    p->service.started = eobool_true;    
    p->service.state = eomn_serv_state_started; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);   
    
    return(eores_OK);
}


extern eOresult_t eo_pos_SetRegulars(EOthePOS *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    return(eo_service_hid_SetRegulars(p->id32ofregulars, arrayofid32, s_eo_pos_isID32relevant, numberofthem));
}


extern eOresult_t eo_pos_Stop(EOthePOS *p)
{
    eo_errman_Trace(eo_errman_GetHandle(), "called: eo_pos_Stop()", s_eobj_ownname);
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    if(eobool_false == p->service.started)
    {   // it is already stopped
        return(eores_OK);
    }

    if(0 == p->numberofowners)
    {   // actually we should not get in here because of previous check of running equal false
        return(eores_OK);
    }
    
    p->numberofowners --;
    
    if(0 != p->numberofowners)
    {   // there is someone else owning the object ... i dont stop it.
        return(eores_OK);
    }    
     
    s_eo_pos_TXstop(p);
       
    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);       
    
    // remove all regulars related to pos entity ... no, dont do that
    //eo_pos_SetRegulars(p, NULL, NULL);    

    return(eores_OK);
}


extern eOresult_t eo_pos_Transmission(EOthePOS *p, eObool_t on)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    } 

    if(eobool_false == p->service.started)
    {   // not running, thus we do nothing
        return(eores_OK);
    }     
    
    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    } 
    
    if(eobool_true == on)
    {   // we activate tx according to config of pos
        s_eo_pos_TXstart(p); 
    }
    else
    {
        s_eo_pos_TXstop(p);
    }
    
    return(eores_OK);      
}



extern eOresult_t eo_pos_Tick(EOthePOS *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    } 

    if(eobool_false == p->service.started)
    {   // not running, thus we do nothing
        return(eores_OK);
    }    

    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }    

    if(eobool_true == p->transmissionisactive)
    {
        s_eo_pos_presenceofcanboards_tick(p);
    }    
    
    // now we dont do anything becase pos does not need any action because everything is done by the can parser
    // motor controller performs watchdog check using eo_pos_isAlive
    
    return(eores_OK);       
}



extern eOresult_t eo_pos_Config(EOthePOS *p, eOas_pos_config_t* config)
{
    if((NULL == p) || (NULL == config))
    {
        return(eores_NOK_nullpointer);
    }
        
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
  
// we allow doing things also if we are not in running mode yet    
//    if(eobool_false == p->service.started)
//    {   // not running, thus we do nothing
//        return(eores_OK);
//    }     

    // ok, now we do something. 
    
    
    p->posconfig.datarate = config->datarate;
    
    
    uint8_t originalrate = p->posconfig.datarate;
    if(p->posconfig.datarate < 5)
    {
        p->posconfig.datarate = 5;        
    }
    
    if(p->posconfig.datarate > 200)
    {
        p->posconfig.datarate = 200;          
    }

    if(originalrate != p->posconfig.datarate)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice       = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress      = 0;
        errdes.par16              = (originalrate << 8) | (p->posconfig.datarate);
        errdes.par64              = 0;        
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_pos_changed_requestedrate);
        p->diagnostics.errorType = eo_errortype_warning;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &errdes);        
    }   

    // we receive three can frames every datarate ms. we check the presence at every tick of teh controller
    // which is 1 ms, hence ... a checkrate = datarate is ok. we add a factor 2 for safety
    p->not_heardof_checkrate = p->posconfig.datarate * 2;    
    
    p->configured = eobool_true;
           
    return(eores_OK);     
}

extern eOresult_t eo_pos_AcceptCANframe(EOthePOS *p, eOcanframe_t *frame, eOcanport_t port)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
  
    // we must be in run mode to accept frames, because the tick must be working   
    if(eobool_false == p->service.started)
    {   // not running, thus we dont accept
        return(eores_OK);
    } 

    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }  
    
    
    // i get the content of the frame and i fill the status of the pos
   
    EOarray *array = (EOarray*)&p->pos->status.arrayofdata;
    if(frame->data[0] == icubCanProto_pos_decideg)
    {
        uint8_t firstlabel = frame->data[1] >> 4;
        uint8_t numberofvalues = frame->data[1] & 0xf;
        if(numberofvalues>2) { numberofvalues = 2; }
        
        for(uint8_t i=0; i<numberofvalues; i++)
        {
            uint8_t pos = firstlabel + i;
            int16_t value = ((int16_t)frame->data[2+i*2]) | ((int16_t)(frame->data[3+i*2]) << 8);  
            eo_array_Assign(array, pos, &value, 1);            
        }        
    }

    
    // i touch the board presence
    
    eObrd_canlocation_t loc = {0};    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eobrd_caninsideindex_none;
    
    s_eo_pos_presenceofcanboards_touch(p, loc);
    
    return(eores_OK);      
}



extern eObool_t eo_pos_isAlive(EOthePOS *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }

    return(p->isalive);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to POS are in a unique place


extern void eoprot_fun_INIT_as_pos_config(const EOnv* nv)
{
    eOas_pos_config_t* poscfg = (eOas_pos_config_t*) eo_nv_RAM(nv);
    
    poscfg->datarate = 10;
}


extern void eoprot_fun_INIT_as_pos_status(const EOnv* nv)
{
    eOas_pos_status_t *status = (eOas_pos_status_t*) eo_nv_RAM(nv);  
    
    uint8_t capacity    = eOas_pos_data_maxnumber;
    uint8_t size        = eOas_pos_data_maxnumber;
    EOarray* array = eo_array_New(capacity, sizeof(eOas_pos_data_t), &status->arrayofdata);
    eo_array_Resize(array, size);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eo_pos_TXstart(EOthePOS *p)
{ 
    if(eobool_false == p->configured)
    {   // dont have a configured service
        return(eores_OK);
    }   

    s_eo_pos_boards_configure(p); 

    s_eo_pos_transmission(p, eobool_true, p->posconfig.datarate);
    
    
    p->transmissionisactive = eobool_true;
    s_eo_pos_presenceofcanboards_start(p);    
    
   return(eores_OK);      
}


static eOresult_t s_eo_pos_TXstop(EOthePOS *p)
{
    if(eobool_false == p->configured)
    {   // nothing to do because we dont have a configured service 
        return(eores_OK);
    }     
    
    // stop the pos
    s_eo_pos_transmission(p, eobool_false, p->posconfig.datarate);
    

    p->transmissionisactive = eobool_false;
    s_eo_pos_presenceofcanboards_reset(p);
               
    return(eores_OK);
}

static void s_eo_pos_transmission(EOthePOS *p, eObool_t on, uint8_t period)
{

    icubCanProto_POS_TRANSMIT_t transmit = {0};
    transmit.rate = (eobool_true == on) ? (period) : (0);
    
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__POS_TRANSMIT;
    p->sharedcan.command.value = &transmit;
    
    // now i must transmit this message to every board in the entity ...    
    eo_canserv_SendCommandToAllBoardsInEntity2(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);    
}



static eOresult_t s_eo_pos_onstop_search4canboards(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{  
    eo_errman_Trace(eo_errman_GetHandle(), "called: s_eo_pos_onstop_search4canboards()", s_eobj_ownname);
    
    EOthePOS* p = &s_eo_thepos;
    
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    
    if(eobool_true == searchisok)
    {
        p->service.state = eomn_serv_state_verified;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);   
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_pos, p->service.state);   
    }    
    
    if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
    {
        eo_pos_Activate(&s_eo_thepos, servcfg);        
    }
    
    p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress    = 0;
    p->diagnostics.errorDescriptor.par16            = (servcfg->data.as.pos.boardInfo.canloc[0].addr) | (servcfg->data.as.pos.boardInfo.canloc[0].port << 8);
    p->diagnostics.errorDescriptor.par64            = (servcfg->data.as.pos.version.firmware.minor << 0) | (servcfg->data.as.pos.version.firmware.major << 8) |
                                                      (servcfg->data.as.pos.version.protocol.minor << 16) | (servcfg->data.as.pos.version.protocol.major << 24);    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_pos_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
    
    if(eobool_true == searchisok)
    {           
        p->diagnostics.errorType = eo_errortype_debug;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_pos_ok);
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
        {
            p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }    
    
    if(eobool_false == searchisok)
    {
        const eOcandiscovery_target_t *ptrgt = (const eOcandiscovery_target_t*) eo_array_At(p->sharedcan.discoverytargets, 0);
        
        if(NULL == ptrgt)
        {
            return eores_NOK_generic;
        }        
        // in case we fail the discovery, it is better to put inside par64 the failure of discovery.
        // maskofmissing, maskofwronglydetected, 
        
        uint16_t totalcanmap[2] = {0};        
        for(uint8_t i=0; i<eo_array_Size(p->sharedcan.discoverytargets); i++)
        {
            const eOcandiscovery_target_t *ptrgt = (const eOcandiscovery_target_t*) eo_array_At(p->sharedcan.discoverytargets, i);
            if(NULL != ptrgt)
            {
                totalcanmap[0] |= ptrgt->canmap[0];
                totalcanmap[1] |= ptrgt->canmap[1];
            }            
        }
        
        const eOcandiscovery_detection_t* detection = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
                
        uint16_t maskofmissingCAN1 = totalcanmap[0] & (~detection->replies[0]);
        uint16_t maskofmissingCAN2 = totalcanmap[1] & (~detection->replies[1]);
        uint16_t maskofincompatibleCAN1 = detection->incompatibilities[0]; 
        uint16_t maskofincompatibleCAN2 = detection->incompatibilities[1]; 
        
        p->diagnostics.errorDescriptor.par64 = ((uint64_t)maskofmissingCAN1) | ((uint64_t)maskofmissingCAN2<<16) | ((uint64_t)maskofincompatibleCAN1<<32) | ((uint64_t)maskofincompatibleCAN2<<32);
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_pos_failed_candiscovery);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }        
    }      
    
    if(NULL != p->service.onverify)
    {
        p->service.onverify(&s_eo_thepos, searchisok); 
    }    
    
    return(eores_OK);   
}


static void s_eo_pos_send_periodic_error_report(void *par)
{
    EOthePOS* p = (EOthePOS*) par;
    
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    if(eoerror_value_CFG_pos_failed_candiscovery == eoerror_code2value(p->diagnostics.errorDescriptor.code))
    {   // if i dont find the pos, i keep on sending the discovery results up. it is a temporary diagnostics tricks until we use the verification of services at bootstrap
        eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());
    }
    
    if(EOK_int08dummy != p->diagnostics.errorCallbackCount)
    {
        p->diagnostics.errorCallbackCount--;
    }
    if(0 == p->diagnostics.errorCallbackCount)
    {
        eo_timer_Stop(p->diagnostics.reportTimer);
    }
}


static eObool_t s_eo_pos_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_analogsensors) << 24) | (((uint32_t)eoprot_entity_as_pos) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    
    return(eobool_false); 
}



static void s_eo_pos_boards_configure(EOthePOS *p)
{
    icubCanProto_POS_CONFIG_t posconfig; // = {0};
    
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__POS_CONFIG_SET;
    p->sharedcan.command.value = &posconfig;
    
    // now we send a command to each board. 
    // we have the can location either in the EOtheCANmapping object or in p->service.servconfig
    // we use this latter
    posconfig.type = icubCanProto_pos_decideg;
    posconfig.setting.decideg[0].enabled =          posconfig.setting.decideg[1].enabled = 1;
    posconfig.setting.decideg[0].invertdirection =  posconfig.setting.decideg[1].invertdirection = 0;
    posconfig.setting.decideg[0].zero =             posconfig.setting.decideg[1].zero = 0;
    posconfig.setting.decideg[0].rotation =         posconfig.setting.decideg[1].rotation = icubCanProto_pos_rot_none;
    
    for(uint8_t i=0; i<eo_pos_maxnumberofCANboards; i++)
    {
        posconfig.setting.decideg[0].label = 2*i;
        posconfig.setting.decideg[1].label = 2*i+1;
        
        // we send a config with no compensation but with labels already fixed for the position of the pos array.         
        eObrd_canlocation_t loc = p->service.servconfig.data.as.pos.boardInfo.canloc[i];        
        eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &p->sharedcan.command, loc);
    }    
}



static void s_eo_pos_presenceofcanboards_reset(EOthePOS *p)
{
    memset(p->not_heardof_target, 0, sizeof(p->not_heardof_target));
    memset(p->not_heardof_status, 0, sizeof(p->not_heardof_status));
    p->not_heardof_counter = 0;
    p->isalive = eobool_true;
}

static void s_eo_pos_presenceofcanboards_build(EOthePOS *p)
{
    s_eo_pos_presenceofcanboards_reset(p);
  
    for(uint8_t i=0; i<eo_pos_maxnumberofCANboards; i++)
    {
        eObrd_canlocation_t loc = p->service.servconfig.data.as.pos.boardInfo.canloc[i]; 
        eo_common_hlfword_bitset(&p->not_heardof_target[loc.port], loc.addr);
    }    
}


static void s_eo_pos_presenceofcanboards_start(EOthePOS *p)
{
    memcpy(p->not_heardof_status, p->not_heardof_target, sizeof(p->not_heardof_status));
    p->not_heardof_counter = 0;
    p->isalive = eobool_true;
}


static void s_eo_pos_presenceofcanboards_touch(EOthePOS *p, eObrd_canlocation_t loc)
{
    eo_common_hlfword_bitclear(&p->not_heardof_status[loc.port], loc.addr);
}


static void s_eo_pos_presenceofcanboards_tick(EOthePOS *p)
{
    if((p->not_heardof_counter++) > p->not_heardof_checkrate)
    {        
        if((0 != p->not_heardof_status[0]) || (0 != p->not_heardof_status[1]))
        {
            p->isalive = eobool_false;
            
            eOerrmanDescriptor_t errdes = {0};
            errdes.sourcedevice       = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress      = 0;
            errdes.par16              = eobrd_unknown;
            errdes.par64              = (p->not_heardof_status[0] << 16) | p->not_heardof_status[1];        
            errdes.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_boards_lostcontact);
            p->diagnostics.errorType = eo_errortype_warning;                
            eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &errdes);
        } 
        else
        {
            p->isalive = eobool_true;
        }

        memcpy(p->not_heardof_status, p->not_heardof_target, sizeof(p->not_heardof_status));
        p->not_heardof_counter = 0;    
    }
}

static eOresult_t s_eo_pos_OnServiceAlreadyActive(EOthePOS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify)
{    
    
    // it means that some other object has activated the pos. it can be either EOtheMotionController or EOtheServices upon request from embObjPOS
    // ... we dont want to interfere, thus we cannot call eo_pos_Deactivate().  
    // we just verify that the eOmn_serv_configuration_t* in argument is the same of (or compatible with) the one already used for activation.
    
    eObool_t verificationOK = eobool_false;
    
    eObool_t iscompatible = eobool_true;
    
    for(uint8_t i=0; i<eo_pos_maxnumberofCANboards; i++)
    {
    
        if(servcfg->data.as.pos.boardInfo.canloc[i].port != p->service.servconfig.data.as.pos.boardInfo.canloc[i].port)
        {
            iscompatible = eobool_false;
        }
        if(servcfg->data.as.pos.boardInfo.canloc[i].addr != p->service.servconfig.data.as.pos.boardInfo.canloc[i].addr)
        {
            iscompatible = eobool_false;
        } 
    }

    if((servcfg->data.as.pos.version.protocol.major != 0) || (servcfg->data.as.pos.version.protocol.minor != 0))
    {
        // check also the protocol
        if(servcfg->data.as.pos.version.protocol.major != p->service.servconfig.data.as.pos.version.protocol.major)
        {
            iscompatible = eobool_false;
        }
        if(servcfg->data.as.pos.version.protocol.minor != p->service.servconfig.data.as.pos.version.protocol.minor)
        {
            iscompatible = eobool_false;
        }
    }
    if((servcfg->data.as.pos.version.firmware.major != 0) || (servcfg->data.as.pos.version.firmware.minor != 0)  || (servcfg->data.as.pos.version.firmware.build != 0))
    {
        // check also the firmaware
        if(servcfg->data.as.pos.version.firmware.major != p->service.servconfig.data.as.pos.version.firmware.major)
        {
            iscompatible = eobool_false;
        }
        if(servcfg->data.as.pos.version.firmware.minor != p->service.servconfig.data.as.pos.version.firmware.minor)
        {
            iscompatible = eobool_false;
        }
        if(servcfg->data.as.pos.version.firmware.build != p->service.servconfig.data.as.pos.version.firmware.build)
        {
            iscompatible = eobool_false;
        }
    }
    
    
    if(eobool_true == iscompatible)
    {
        verificationOK = eobool_true;
    }
    else
    {
        verificationOK = eobool_false;
        p->diagnostics.errorType = eo_errortype_error;
        p->diagnostics.errorDescriptor.par16 = 0;
        p->diagnostics.errorDescriptor.par64 = 0;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_pos_failed_verify_because_active);
        p->diagnostics.errorDescriptor.sourceaddress = 0;
        p->diagnostics.errorDescriptor.sourcedevice = eo_errman_sourcedevice_localboard; 
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor); 
    }
    
    // in here we dont activate because it is already active ...

    if(NULL != onverify)
    {
        onverify(p, verificationOK); 
    }          
    return((eobool_true == verificationOK) ? (eores_OK) : (eores_NOK_generic));      
}       
        
#endif // #elif !defined(EOTHESERVICES_disable_thePOS)        

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



