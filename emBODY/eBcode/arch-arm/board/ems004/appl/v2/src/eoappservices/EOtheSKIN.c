/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

#include "EOtheServices.h"
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

#include "EoProtocolSK.h"

#include "EOMtheEMSappl.h"

#include "EOVtheCallbackManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSKIN.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSKIN_hid.h"


#if defined(EOTHESERVICES_disable_theSKIN)

    // provide empty implementation, so that we dont need to change the caller of the API
    
    extern EOtheSKIN* eo_skin_Initialise(void) 
    {   
        return NULL; 
    }

    extern EOtheSKIN* eo_skin_GetHandle(void)   
    { 
        return NULL; 
    }

    extern eOmn_serv_state_t eo_skin_GetServiceState(EOtheSKIN *p) 
    { 
        return eomn_serv_state_notsupported; 
    }
    
    // in some cases, we need to alert the pc104 that the board does not support this service
    extern eOresult_t eo_skin_SendReport(EOtheSKIN *p)
    {
        static const char s_eobj_ownname[] = "EOtheSKIN";
        eOerrmanDescriptor_t errdes = {};
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_skin_failed_notsupported);  
        errdes.sourcedevice = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress = 0;
        errdes.par16 = errdes.par64 = 0;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);
        return eores_OK;
    }


    extern eOresult_t eo_skin_Verify(EOtheSKIN *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
    {
        // we alert the host that the verification of the service has failed
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, eomn_serv_state_failureofverify);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        } 
        
        // we tell that the reason is that this service is not supported
        eo_skin_SendReport(NULL);
               
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_Activate(EOtheSKIN *p, const eOmn_serv_configuration_t * servcfg)
    {
        eo_skin_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_Deactivate(EOtheSKIN *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_Start(EOtheSKIN *p)
    {
        eo_skin_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_SetRegulars(EOtheSKIN *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
    {
        if(NULL != arrayofid32)
        {
            eo_skin_SendReport(NULL);
        }
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_Tick(EOtheSKIN *p, eObool_t resetstatus)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_Stop(EOtheSKIN *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_Transmission(EOtheSKIN *p, eObool_t on)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_Config(EOtheSKIN *p, eOas_inertial3_config_t* config)
    {
        eo_skin_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_SetMode(EOtheSKIN *p, uint8_t patchindex, eOsk_sigmode_t mode)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_SetBoardsConfig(EOtheSKIN *p, uint8_t patchindex, eOsk_cmd_boardsCfg_t *brdcfg)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_skin_SetTrianglesConfig(EOtheSKIN *p, uint8_t patchindex, eOsk_cmd_trianglesCfg_t *trgcfg)
    {
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_skin_AcceptCANframe(EOtheSKIN *p, eOcanframe_t *frame, eOcanport_t port)
    {
        return eores_NOK_generic;
    }


#elif !defined(EOTHESERVICES_disable_theSKIN)

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



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

static eOresult_t s_eo_skin_TXstart(EOtheSKIN *p);
static eOresult_t s_eo_skin_TXstop(EOtheSKIN *p);

static eOresult_t s_eo_skin_onstop_search4mtbs(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok);

static void s_eo_skin_send_periodic_error_report(void *par);

static eOsk_skin_t* s_eo_skin_get_entity(EOtheSKIN* p, eOcanframe_t *frame, eOcanport_t port, uint8_t *index);

static eObool_t s_eo_skin_activeskin_can_accept_canframe(void);

static eObool_t s_eo_skin_isID32relevant(uint32_t id32);
    
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

        
static EOtheSKIN s_eo_theskin = 
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
    
    EO_INIT(.patchisrunning)            { eobool_false },    
    EO_INIT(.numofskinpatches)          0,
    EO_INIT(.numofmtbs)                 0,    
    EO_INIT(.rxdata)                    { NULL },
    EO_INIT(.skinpatches)               { NULL },
    EO_INIT(.id32ofregulars)            NULL
};

static const char s_eobj_ownname[] = "EOtheSKIN";

//static char s_trace_string[64] = {0};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheSKIN* eo_skin_Initialise(void)
{
    EOtheSKIN* p = &s_eo_theskin;
    
    if(eobool_true == p->service.initted)
    {
        return(p);
    }

    p->numofskinpatches = 0;
    p->numofmtbs = 0;
    p->service.servconfig.type = eomn_serv_NONE;
    
    
    p->sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), eo_skin_maxnumberofMTBboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), eo_skin_maxnumberofMTBboards, NULL, NULL, NULL, NULL);
    
    p->sharedcan.discoverytargets = eo_array_New(1, sizeof(eOcandiscovery_target_t),  NULL);
    
    uint8_t i=0;
    for(i=0; i<eomn_serv_skin_maxpatches; i++)
    {
        p->skinpatches[i] = NULL;
        p->patchisrunning[i] = eobool_false;
        
        p->rxdata[i] = eo_vector_New(sizeof(eOsk_candata_t), 64, NULL, NULL, NULL, NULL); 
    }
    
    p->id32ofregulars = eo_array_New(skin_maxRegulars, sizeof(uint32_t), NULL);
        
    p->diagnostics.reportTimer = eo_timer_New();
    p->diagnostics.errorType = eo_errortype_error;
    p->diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_skin_not_verified_yet);  

    
    p->service.initted = eobool_true;    
    p->service.active = eobool_false;
    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_idle;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);    
    
    return(p);   
}
 

    
extern EOtheSKIN* eo_skin_GetHandle(void)
{
    if(eobool_true == s_eo_theskin.service.initted)
    {
        return(&s_eo_theskin);
    }
    
    return(NULL);
}


extern eOmn_serv_state_t eo_skin_GetServiceState(EOtheSKIN *p)
{
    if(NULL == p)
    {
        return(eomn_serv_state_notsupported);
    } 

    return(p->service.state);
}


extern eOresult_t eo_skin_SendReport(EOtheSKIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    eOerror_value_t errorvalue = eoerror_code2value(p->diagnostics.errorDescriptor.code);
    
    switch(errorvalue)
    {
        case eoerror_value_CFG_skin_failed_candiscovery:
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


extern eOresult_t eo_skin_Verify(EOtheSKIN *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    if((NULL == p) || (NULL == servcfg))
    {
        s_eo_theskin.service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, s_eo_theskin.service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }                
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_SK_skin != servcfg->type)
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }      
        return(eores_NOK_generic);
    }   
    
 
    if(eobool_true == p->service.active)
    {
        eo_skin_Deactivate(p);        
    }   
    
    p->service.state = eomn_serv_state_verifying;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);
    
    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);  
      
    p->service.onverify = onverify;
    p->service.activateafterverify = activateafterverify;
    
    eOcandiscovery_target_t trgt = {0};


    trgt.info.type = servcfg->data.sk.skin.boardinfo.type;
    trgt.info.protocol.major = servcfg->data.sk.skin.boardinfo.protocol.major; 
    trgt.info.protocol.minor = servcfg->data.sk.skin.boardinfo.protocol.minor;
    trgt.info.firmware.major = servcfg->data.sk.skin.boardinfo.firmware.major; 
    trgt.info.firmware.minor = servcfg->data.sk.skin.boardinfo.firmware.minor;    
    trgt.info.firmware.build = servcfg->data.sk.skin.boardinfo.firmware.build;    
    
    // now i must do discovery of the patches. all patches can be at most on the two can buses ...
    // moreover, we cannot have more than .... eo_skin_maxnumberofMTBboards boards

    uint8_t numofboardsentity[2] = {0};
    trgt.canmap[eOcanport1] = trgt.canmap[eOcanport2] = 0x0000;
    uint8_t i=0;
    for(i=0; i<servcfg->data.sk.skin.numofpatches; i++)
    {
        trgt.canmap[eOcanport1] |= servcfg->data.sk.skin.canmapskin[i][eOcanport1];
        trgt.canmap[eOcanport2] |= servcfg->data.sk.skin.canmapskin[i][eOcanport2];
        
        numofboardsentity[i] = eo_common_hlfword_bitsetcount(servcfg->data.sk.skin.canmapskin[i][eOcanport1]) +
                               eo_common_hlfword_bitsetcount(servcfg->data.sk.skin.canmapskin[i][eOcanport2]);
    }
    
    uint8_t numofboards = eo_common_hlfword_bitsetcount(trgt.canmap[eOcanport1]) +
                          eo_common_hlfword_bitsetcount(trgt.canmap[eOcanport2]);
    
    
    if((numofboards > eo_skin_maxnumberofMTBboards) || (numofboardsentity[0] > eo_skin_maxumberofMTBboardsInEntity) || (numofboardsentity[1] > eo_skin_maxumberofMTBboardsInEntity))
    {        
        p->diagnostics.errorDescriptor.sourcedevice       = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress      = 0;
        p->diagnostics.errorDescriptor.par16              = (numofboards << 8) | (eo_skin_maxnumberofMTBboards & 0x00ff);
        p->diagnostics.errorDescriptor.par64              = (trgt.canmap[eOcanport2] << 16) | (trgt.canmap[eOcanport1]);
       
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_eo_skin_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));        
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_skin_failed_toomanyboards);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);   
        }  

        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }  
        
        return(eores_NOK_generic); 
    }
    
    // force a cleaned discoverytargets before we add the target
    eo_array_Reset(p->sharedcan.discoverytargets);
    eo_array_PushBack(p->sharedcan.discoverytargets, &trgt);
              
    p->sharedcan.ondiscoverystop.function = s_eo_skin_onstop_search4mtbs;
    p->sharedcan.ondiscoverystop.parameter = (void*)servcfg;
    
    // start discovery   
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), p->sharedcan.discoverytargets, &p->sharedcan.ondiscoverystop);   
   
    return(eores_OK);   
}


extern eOresult_t eo_skin_Deactivate(EOtheSKIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == p->service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        p->service.state = eomn_serv_state_idle; 
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);
        return(eores_OK);        
    } 
    
    if(eobool_true == p->service.started)
    {
        eo_skin_Stop(p);
    }
    
    eo_skin_SetRegulars(p, NULL, NULL);
        
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_skin, eoprot_entity_sk_skin, p->sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfSkins(eo_entities_GetHandle(), 0);
    
    p->numofmtbs = 0;
    
    uint8_t i=0;
    for(i=0; i<eomn_serv_skin_maxpatches; i++)
    {
        p->skinpatches[i] = NULL;
        p->patchisrunning[i] = eobool_false;
        
        eo_vector_Clear(p->rxdata[i]);
    }
    
    memset(&p->service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    p->service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(p->sharedcan.boardproperties);
    eo_vector_Clear(p->sharedcan.entitydescriptor);
    eo_array_Reset(p->sharedcan.discoverytargets);
    
    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);  
    
    p->service.active = eobool_false;    
    p->service.state = eomn_serv_state_idle; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);
    
    return(eores_OK);
}


extern eOresult_t eo_skin_Activate(EOtheSKIN *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    } 

    if(eomn_serv_SK_skin != servcfg->type)
    {
        return(eores_NOK_generic);
    }
    
    if(eobool_true == p->service.active)
    {
        eo_skin_Deactivate(p);        
    }   
    
    eo_entities_SetNumOfSkins(eo_entities_GetHandle(), servcfg->data.sk.skin.numofpatches);
    

    if(0 == eo_entities_NumOfSkins(eo_entities_GetHandle()))
    {
        p->service.active = eobool_false;
        return(eores_NOK_generic);
    }
    else
    {         
        memcpy(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));

        p->numofskinpatches = servcfg->data.sk.skin.numofpatches;
        
        uint8_t i = 0;
        for(i=0; i<p->numofskinpatches; i++)
        {
            p->skinpatches[i] = eo_entities_GetSkin(eo_entities_GetHandle(), i);
        }
        
        // now i must add all the mtb boards. i iterate per patch and then per canbus
        eObrd_canproperties_t prop = {0};
        prop.type = servcfg->data.sk.skin.boardinfo.type;
        prop.location.port = 0;
        prop.location.addr = 0;
        prop.location.insideindex = eobrd_caninsideindex_none;
        prop.requiredprotocol.major = servcfg->data.sk.skin.boardinfo.protocol.major;
        prop.requiredprotocol.minor = servcfg->data.sk.skin.boardinfo.protocol.minor;
                          
        eOcanmap_entitydescriptor_t des = {0};      
        
        p->numofmtbs = 0;
        
        uint8_t j = 0;
        uint8_t k = 0;
        for(i=0; i<p->numofskinpatches; i++)
        {
            for(j=0; j<eOcanports_number; j++)
            {
                uint16_t canbusmapping = servcfg->data.sk.skin.canmapskin[i][j];
                for(k=1; k<15; k++)
                {
                    if(eobool_true == eo_common_hlfword_bitcheck(canbusmapping, k))
                    {   // i pushback. i dont verify vs teh capacity of the vector because eo_skin_Verify() has already done it
                        prop.location.port = j;
                        prop.location.addr = k;
                        eo_vector_PushBack(p->sharedcan.boardproperties, &prop);
                        
                        des.location.port = j;
                        des.location.addr = k;
                        des.index = (eOcanmap_entityindex_t)i;                        
                        eo_vector_PushBack(p->sharedcan.entitydescriptor, &des);
                        
                        p->numofmtbs++;
                    }
                }
            }
        }
        
        // load the can mapping 
        eo_canmap_LoadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties); 
        
        // load the entity mapping.
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_skin, eoprot_entity_sk_skin, p->sharedcan.entitydescriptor);   

        p->service.active = eobool_true;    
        p->service.state = eomn_serv_state_activated;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);        
    }
    
    return(eores_OK);   
}



extern eOresult_t eo_skin_Start(EOtheSKIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    if(eobool_true == p->service.started)
    {   // it is already running
        return(eores_OK);
    }   
    
    p->service.started = eobool_true;    
    p->service.state = eomn_serv_state_started; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);
 
    // marco.accame: i start the skin but i dont say to the patches to tx. the eth callback will do it.    
//    uint8_t i = 0;
//    for(i=0; i<p->numofskinpatches; i++)
//    {
//        eo_skin_SetMode(p, i, eosk_sigmode_signal);
//    }
    
    return(eores_OK);   
}


extern eOresult_t eo_skin_SetRegulars(EOtheSKIN *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    return(eo_service_hid_SetRegulars(p->id32ofregulars, arrayofid32, s_eo_skin_isID32relevant, numberofthem));
}

extern eOresult_t eo_skin_Stop(EOtheSKIN *p)
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
    {   // it is already stopped
        return(eores_OK);
    }       
    
    s_eo_skin_TXstop(p);     

    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_activated; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);    
    
    
    // reset the various buffers
    for(uint8_t i=0; i<p->numofskinpatches; i++)
    {
        EOarray *array = (EOarray*) (&p->skinpatches[i]->status.arrayofcandata);
        EOvector *vector = (EOvector*) p->rxdata[i];
        
        eo_array_Reset(array);
        eo_vector_Clear(vector);        
    }
    
    // remove all regulars related to skin entity ... no, dont do that
    //eo_skin_SetRegulars(p, NULL, NULL);
    
    return(eores_OK);    
}


extern eOresult_t eo_skin_Tick(EOtheSKIN *p, eObool_t resetstatus)
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
    
    
    for(uint8_t i=0; i<p->numofskinpatches; i++)
    {
        EOarray *array = (EOarray*) (&p->skinpatches[i]->status.arrayofcandata);
        EOvector *vector = (EOvector*) p->rxdata[i];
        
        if(eobool_true == resetstatus)
        {
            eo_array_Reset(array);            
        }
        
        if(eobool_true == eo_array_Full(array))
        {
            continue;
        }
        
        if(eobool_true == eo_vector_Empty(vector))
        {
            continue;
        }
        
        // load all what i can from vector into array.
        
        uint8_t availabledestination = eo_array_Available(array);
        uint8_t sizesource = eo_vector_Size(vector);
        uint8_t numofitems2move = EO_MIN(availabledestination, sizesource);
        uint8_t j = 0;
        for(j=0; j<numofitems2move; j++)
        {
            eOsk_candata_t *candata = (eOsk_candata_t*)eo_vector_Front(vector);
            eo_array_PushBack(array, candata);
            eo_vector_PopFront(vector);            
        }               
    }
    
    return(eores_OK);
}



extern eOresult_t eo_skin_Transmission(EOtheSKIN *p, eObool_t on)
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
    
    if(eobool_true == on)
    {
        s_eo_skin_TXstart(p);   
    }
    else
    {
        s_eo_skin_TXstop(p);
    }

    
    return(eores_OK);   
}


extern eOresult_t eo_skin_SetMode(EOtheSKIN *p, uint8_t patchindex, eOsk_sigmode_t mode)
{
    if(NULL == p)
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

    if(patchindex >= p->numofskinpatches)
    {
        return(eores_NOK_generic);
    }
        
    p->sharedcan.command.clas = eocanprot_msgclass_pollingSkin;    
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    p->sharedcan.command.value = NULL;   

    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, patchindex, eoprot_tag_none);
    
    switch(mode)
    {
        case eosk_sigmode_dontsignal:
        {
            // in old way it does not exist
            // in new way:
            icubCanProto_as_sigmode_t sigmode2use = icubCanProto_as_sigmode_dontsignal;
            p->sharedcan.command.value = &sigmode2use;
            // and now we send the command to all the skin boards
            eo_canserv_SendCommandToAllBoardsInEntity2(eo_canserv_GetHandle(), &p->sharedcan.command, id32);
            // and we set this patch as not tx
            p->patchisrunning[patchindex] = eobool_false;
            
        } break;
        
        case eosk_sigmode_signal:
        {
            icubCanProto_as_sigmode_t sigmode2use = icubCanProto_as_sigmode_signal;
            p->sharedcan.command.value = &sigmode2use;
            // and now we send the command to all the skin boards
            eo_canserv_SendCommandToAllBoardsInEntity2(eo_canserv_GetHandle(), &p->sharedcan.command, id32);
            // and we set this patch as tx
            p->patchisrunning[patchindex] = eobool_true;

        } break;

        case eosk_sigmode_signal_oldway:
        {
            // we need to change the class and type as well
            p->sharedcan.command.clas = eocanprot_msgclass_pollingSkin;    
            p->sharedcan.command.type  = ICUBCANPROTO_POL_SK_CMD__TACT_SETUP;
            p->sharedcan.command.value = NULL;     
            // and now we send the command to all the skin boards
            eo_canserv_SendCommandToAllBoardsInEntity2(eo_canserv_GetHandle(), &p->sharedcan.command, id32);
            // and we set this patch as tx
            p->patchisrunning[patchindex] = eobool_true;
                       
            
            eOerrmanDescriptor_t errdes = {0};
            errdes.code             = eoerror_code_get(eoerror_category_Skin, eoerror_value_SK_obsoletecommand);
            errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress    = 0;
            errdes.par16            = 0;
            errdes.par64            = 0;
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, s_eobj_ownname, &errdes);   
            
//            #warning TBD: VERY IMPORTANT -> see the following code, where we need also a special message to board with address 0xE
//
//            for(i=skconfig_ptr->boardAddrStart; i<boardEndAddr; i++)
//            {
//                
//                msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, i);
//                res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd, NULL);
//                if(eores_OK != res)
//                {
//                    return;
//                }
//                
//                if(0xE == i)
//                {
//                    eOicubCanProto_msgCommand_t msgCmd2 = 
//                    {
//                        EO_INIT(.clas) icubCanProto_msgCmdClass_skinBoard,
//                        EO_INIT(.cmdId) ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2
//                    };
//                    
//                    res = eo_appCanSP_SendCmd(appCanSP_ptr, skconfig_ptr->connected2emsport, msgdest, msgCmd2, NULL);
//                    if(eores_OK != res)
//                    {
//                        return;
//                    }
//                }
//            }
        } break;
        
        default:
        {
        } break;
    }        
    
    return(eores_OK);
}


extern eOresult_t eo_skin_SetBoardsConfig(EOtheSKIN *p, uint8_t patchindex, eOsk_cmd_boardsCfg_t *brdcfg)
{
    if(NULL == p)
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

    if(patchindex >= p->numofskinpatches)
    {
        return(eores_NOK_generic);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, patchindex, eoprot_tag_none);
    
    icubCanProto_skinboard_config_t canProto_skcfg;
    canProto_skcfg.skintype = (icubCanProto_skinType_t)brdcfg->cfg.skintype; 
    canProto_skcfg.period   = brdcfg->cfg.period;
    canProto_skcfg.noload   = brdcfg->cfg.noload;
    
    p->sharedcan.command.clas = eocanprot_msgclass_pollingSkin;    
    p->sharedcan.command.type  = ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG;
    p->sharedcan.command.value = &canProto_skcfg; 
    
    
    // for this command we send to the whole entity if .... the candestination[] is = { 0xffff, 0xffff }
    // otherwise we send only to those which are not zero and not f or 0 address.
    if((0xffff == brdcfg->candestination[0]) && (0xffff == brdcfg->candestination[1]))
    {
        //snprintf(s_trace_string, sizeof(s_trace_string), "brdcfg: eo_canserv_SendCommandToAllBoardsInEntity2()");    
        //eo_errman_Trace(eo_errman_GetHandle(), s_trace_string, "pippo");
        eo_canserv_SendCommandToAllBoardsInEntity2(eo_canserv_GetHandle(), &p->sharedcan.command, id32);
    }
    else
    {
        eObrd_canlocation_t loc = {0};
        loc.insideindex = eobrd_caninsideindex_none;
        
        for(uint8_t i=0; i<2; i++)
        {
            for(uint8_t j=1; j<15; j++)
            {
                if(eobool_true == eo_common_hlfword_bitcheck(brdcfg->candestination[i], j))
                {
                    loc.port = i;
                    loc.addr = j;
                    //snprintf(s_trace_string, sizeof(s_trace_string), "brdcfg: eo_canserv_SendCommandToLocation(CAN%d:%d)", i+1, j);    
                    //eo_errman_Trace(eo_errman_GetHandle(), s_trace_string, "pippo");
                    eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &p->sharedcan.command, loc);  
                }                    
            }
            
        }                
    }

    return(eores_OK);          
}



extern eOresult_t eo_skin_SetTrianglesConfig(EOtheSKIN *p, uint8_t patchindex, eOsk_cmd_trianglesCfg_t *trgcfg)
{
    if(NULL == p)
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
    
    
    if(patchindex >= p->numofskinpatches)
    {
        return(eores_NOK_generic);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, patchindex, eoprot_tag_none);
    
    icubCanProto_skintriangles_config_t canProto_trgscfg = {0};
    canProto_trgscfg.idstart   = trgcfg->idstart;
    canProto_trgscfg.idend     = trgcfg->idend;
    canProto_trgscfg.flags     = trgcfg->cfg.enable;
    canProto_trgscfg.shift     = trgcfg->cfg.shift;
    canProto_trgscfg.CDCoffset = trgcfg->cfg.CDCoffset;
    
    
    p->sharedcan.command.clas = eocanprot_msgclass_pollingSkin;    
    p->sharedcan.command.type = ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG;
    p->sharedcan.command.value = &canProto_trgscfg; 
    
    
    // for this command we send to the whole entity if .... the candestination[] is = { 0xffff, 0xffff }
    // otherwise we send only to those which are not zero and not f or 0 address.
    if((0xffff == trgcfg->candestination[0]) && (0xffff == trgcfg->candestination[1]))
    {
        //snprintf(s_trace_string, sizeof(s_trace_string), "trgcfg: eo_canserv_SendCommandToAllBoardsInEntity2()");    
        //eo_errman_Trace(eo_errman_GetHandle(), s_trace_string, "pippo");
        
        eo_canserv_SendCommandToAllBoardsInEntity2(eo_canserv_GetHandle(), &p->sharedcan.command, id32);
    }
    else
    {
        eObrd_canlocation_t loc = {0};
        loc.insideindex = eobrd_caninsideindex_none;
        
        for(uint8_t i=0; i<2; i++)
        {
            for(uint8_t j=1; j<15; j++)
            {
                if(eobool_true == eo_common_hlfword_bitcheck(trgcfg->candestination[i], j))
                {
                    loc.port = i;
                    loc.addr = j;
                    //snprintf(s_trace_string, sizeof(s_trace_string), "trgcfg: eo_canserv_SendCommandToLocation(CAN%d:%d)", i+1, j);    
                    //eo_errman_Trace(eo_errman_GetHandle(), s_trace_string, "pippo");
                    eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &p->sharedcan.command, loc);  
                }                    
            }
            
        }                
    }    
       
    return(eores_OK);          
}


extern eOresult_t eo_skin_AcceptCANframe(EOtheSKIN *p, eOcanframe_t *frame, eOcanport_t port)
{
    if((NULL == p) || (NULL == frame))
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
    
    if(eobool_false == s_eo_skin_activeskin_can_accept_canframe())
    {
        return(eores_OK);
    }

 
    uint8_t index = 0;    
    eOsk_skin_t *skin = s_eo_skin_get_entity(p, frame, port, &index);

    if(index >= p->numofskinpatches)
    {
        return(eores_NOK_generic);
    }
    
    eOsk_candata_t candata = {0};
    uint16_t info = EOSK_CANDATA_INFO(frame->size, frame->id);
    candata.info = info;    
    memcpy(candata.data, frame->data, sizeof(candata.data));  
    
    EOarray *array = (EOarray*)(&skin->status.arrayofcandata);
    
    if(eobool_false == eo_array_Full(array))
    {   // put it inside the status array
        eo_array_PushBack(array, &candata);        
    }
    else if(eobool_false == eo_vector_Full(p->rxdata[index]))
    {   // put it inside the vector
        eo_vector_PushBack(p->rxdata[index], &candata);
    }
    else
    {   // damn... a loss of can frames
        eOerrmanDescriptor_t des = {0};
        des.code            = eoerror_code_get(eoerror_category_Skin, eoerror_value_SK_arrayofcandataoverflow);
        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourceaddress   = EOCANPROT_FRAME_GET_SOURCE(frame);
        des.sourcedevice    = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des);         
    }

    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// - in here i put the functions used to initialise the values in ram of the skin ... better in here rather than elsewhere.

// marco.accame: we start with a silent skin and with an empty status_arrayofcandata.
// then we put skin in sigmode eosk_sigmode_signal only if robotInterface orders it to the board.


extern void eoprot_fun_INIT_sk_skin_config_sigmode(const EOnv* nv)
{
    eOsk_sigmode_t *sigmode = (eOsk_sigmode_t*)eo_nv_RAM(nv);
    
    *sigmode = eosk_sigmode_dontsignal; 
}


extern void eoprot_fun_INIT_sk_skin_status_arrayofcandata(const EOnv* nv)
{
    EOarray_of_skincandata_t *tmp = (EOarray_of_skincandata_t*)eo_nv_RAM(nv);
    tmp = tmp;
    // marco.accame: items of array are eOsk_candata_t. its capacity is:
    uint16_t capacity = sizeof(tmp->data) / sizeof(eOsk_candata_t);    
    // eo_array_New() initialises capacity and itemsize and also sets size to 0 
    EOarray *array = eo_array_New(capacity, sizeof(eOsk_candata_t), eo_nv_RAM(nv));
}

extern eObool_t eocanprotSKperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_SKIN_MSG(eOsk_skin_t *skin)
{    
    if(NULL == skin)
    {
        return(eobool_true);
    }
    
    // marco.accame:        
    // if skin->config.sigmode is eosk_sigmode_dontsignal then we dont need using the payload of the can frame. 
    // however, also if skin->config.sigmode is eosk_sigmode_signal but we are not in RUN mode we should not put 
    // frames inside the arrayofcandata. this latter for example is tha case if we are still in the cfg->run transition 
    // and thus not  yet inside the control-loop which empties the arrayofcandata, or also if  the udp packet with go2run 
    // rop<> gets lost.
    
    // we may decode some canframes of this kind if we pass from run to config mode and we process frame buffered in the rx-fifo    
    if(eosk_sigmode_dontsignal == skin->config.sigmode)
    {
        return(eobool_true);
    } 

    eOsmStatesEMSappl_t applstate = eo_sm_emsappl_STcfg;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &applstate);   
    if(eo_sm_emsappl_STrun != applstate)
    {
        return(eobool_true);
    }
    
    return(eobool_false);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_skin_TXstart(EOtheSKIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == p->service.active))
    {   // nothing to do because we dont have skin service.active
        return(eores_OK);
    }
              
    // now, i do things. 
    
    for(uint8_t i=0; i<p->numofskinpatches; i++)
    {
        eo_skin_SetMode(p, i, eosk_sigmode_signal);
    }
        
    return(eores_OK);    
   
}


static eOresult_t s_eo_skin_TXstop(EOtheSKIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == p->service.active))
    {   // nothing to do because we dont have skin service.active
        return(eores_OK);
    }
    
          
    // now, i do things. 
    
    icubCanProto_as_sigmode_t sigmode = icubCanProto_as_sigmode_dontsignal;
    p->sharedcan.command.clas = eocanprot_msgclass_pollingSkin;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;       
    p->sharedcan.command.value = &sigmode;

    uint8_t i=0;
    eOsk_skin_t *skin = NULL;
    for(i=0; i<p->numofskinpatches; i++)
    {
        
        if(NULL == (skin = p->skinpatches[i]))
        {   
            continue;   // i dont have this skin ... i go the next one
        }
        
        // i stop this skin only if it was started before
        if(p->patchisrunning[i] == eobool_false)
        {
            continue;
        }
         
        //#warning marco.accame: see if we can remove it .....
        // if we are in here it means that robotinterface has activated the skin after that the verify() was done succesfully
        // thus, we can remove the continue. BUT who cares t send the silence command or not if the patch is already silent ....
        if(eosk_sigmode_dontsignal == skin->config.sigmode)
        {   
            // if the skin was not initted by robot-interface, then i dont deinit it. the reason is twofold:
            // 1. if the skin boards dont transmit, there is no need to silence them,
            // 2. in case the .ini file of robotinterface has disable the skin because the skin is not mounted, i dont want to tx on a disconnected can bus.
            continue;   // i dont need to silence this skin ... i go the next one
        }
            
        // i set the skin as not transmitting as soon as possible. because in such a way, can messages being received
        // are not pushed back in skin->status.arrayofcandata and its does not overflow.
        skin->config.sigmode = eosk_sigmode_dontsignal;
              
        // then i stop transmission of each skin can board
       
        // i get the addresses of the can boards of the i-th skin.
        // the simplification we use is that they all are on the same CAN bus and all have consecutive addresses.
        // we send the same p->sharedcan.command to all of them
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, i, 0);
        eo_canserv_SendCommandToAllBoardsInEntity2(eo_canserv_GetHandle(), &p->sharedcan.command, id32);    
        
        p->patchisrunning[i] = eobool_false;
    }
        
    return(eores_OK);

}


//#define EOSKIN_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE
static eObool_t s_eo_skin_activeskin_can_accept_canframe(void)
{
#if     defined(EOSKIN_ALWAYS_ACCEPT_CANFRAMES_IN_ACTIVE_MODE)
    // in this case the service.active skin always accepts can frames, even if the run mode is not.
    // it is up to robotInterface to put system in run mode, so that the overflow of fifo is avoided
    return(eobool_true);
#else
    // in this mode the service.active skin accepts can frames only if we are in run mode, thus the fifo can be emptied.    
    eOsmStatesEMSappl_t applstate = eo_sm_emsappl_STerr;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &applstate);   
    if(eo_sm_emsappl_STrun == applstate)
    {
        return(eobool_true);
    }    
    return(eobool_false);    
#endif       
}


static eOsk_skin_t* s_eo_skin_get_entity(EOtheSKIN* p, eOcanframe_t *frame, eOcanport_t port, uint8_t *index)
{
    eOsk_skin_t * ret = NULL;
    uint8_t ii = 0;
    eObrd_canlocation_t loc = {0};
    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eobrd_caninsideindex_none;
    
    ii = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, eoprot_endpoint_skin, eoprot_entity_sk_skin);
    
    if(EOK_uint08dummy == ii)
    {  
        *index = ii;
        return(NULL);
    }
    
    ret = p->skinpatches[ii];
    
    *index = ii;        

    return(ret);   
}



static eOresult_t s_eo_skin_onstop_search4mtbs(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{
    EOtheSKIN* p = &s_eo_theskin;
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    const eOcandiscovery_target_t *ptrgt = (const eOcandiscovery_target_t*) eo_array_At(p->sharedcan.discoverytargets, 0);
    
    if(NULL == ptrgt)
    {
        return eores_NOK_generic;
    }

    if(eobool_true == searchisok)
    {
        p->service.state = eomn_serv_state_verified;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_skin, p->service.state);
    }
    
    if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
    {
        eo_skin_Activate(p, servcfg);        
    }

    p->diagnostics.errorDescriptor.sourcedevice       = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress      = 0;
    p->diagnostics.errorDescriptor.par16              = servcfg->data.sk.skin.numofpatches;
    p->diagnostics.errorDescriptor.par64              = (servcfg->data.sk.skin.boardinfo.firmware.minor)      | (servcfg->data.sk.skin.boardinfo.firmware.major << 8)  |
                                                                 (servcfg->data.sk.skin.boardinfo.protocol.minor << 16) | (servcfg->data.sk.skin.boardinfo.protocol.major << 24) |
                                                                 ((uint64_t)ptrgt->canmap[eOcanport1] << 32) | ((uint64_t)ptrgt->canmap[eOcanport2] << 48);
   
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_skin_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));

    if(eobool_true == searchisok)
    {        
        p->diagnostics.errorType = eo_errortype_debug;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_skin_ok);
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
        {
            p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }
    
    
    if(eobool_false == searchisok)
    {   
        // in case we fail the discovery, it is better to put inside par64 the failure of discovery.
        // maskofmissing, maskofwronglydetected, 
        
        const eOcandiscovery_detection_t* detection = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
                
        uint16_t maskofmissingCAN1 = ptrgt->canmap[0] & (~detection->replies[0]);
        uint16_t maskofmissingCAN2 = ptrgt->canmap[1] & (~detection->replies[1]);
        uint16_t maskofincompatibleCAN1 = detection->incompatibilities[0]; 
        uint16_t maskofincompatibleCAN2 = detection->incompatibilities[1]; 
        
        p->diagnostics.errorDescriptor.par64 = ((uint64_t)maskofmissingCAN1) | ((uint64_t)maskofmissingCAN2<<16) | ((uint64_t)maskofincompatibleCAN1<<32) | ((uint64_t)maskofincompatibleCAN2<<32);
        
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_skin_failed_candiscovery);
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
        p->service.onverify(p, searchisok); 
    }    
    
    return(eores_OK);
    
}


static void s_eo_skin_send_periodic_error_report(void *par)
{
    EOtheSKIN* p = (EOtheSKIN*)par;
    
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    if(eoerror_value_CFG_skin_failed_candiscovery == eoerror_code2value(p->diagnostics.errorDescriptor.code))
    {   // if i dont find the mtbs, i keep on sending the discovery results up. it is a temporary diagnostics tricks until we use the verification of services at bootstrap
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

static eObool_t s_eo_skin_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_skin) << 24) | (((uint32_t)eoprot_entity_sk_skin) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    
    return(eobool_false); 
}

#endif // #elif !defined(EOTHESERVICES_disable_theSKIN)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



