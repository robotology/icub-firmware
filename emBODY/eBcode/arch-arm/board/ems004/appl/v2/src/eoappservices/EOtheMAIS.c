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

#include "EOtheMAIS.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMAIS_hid.h"

#if defined(EOTHESERVICES_disable_theMAIS)

    // provide empty implementation, so that we dont need to change the caller of the API
    
    extern EOtheMAIS* eo_mais_Initialise(void) 
    {   
        return NULL; 
    }

    extern EOtheMAIS* eo_mais_GetHandle(void)   
    { 
        return NULL; 
    }

    extern eOmn_serv_state_t eo_mais_GetServiceState(EOtheMAIS *p) 
    { 
        return eomn_serv_state_notsupported; 
    }
    
    // in some cases, we need to alert the pc104 that the board does not support this service
    extern eOresult_t eo_mais_SendReport(EOtheMAIS *p)
    {
        static const char s_eobj_ownname[] = "EOtheMAIS";
        eOerrmanDescriptor_t errdes = {};
        errdes.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mais_failed_notsupported);  
        errdes.sourcedevice = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress = 0;
        errdes.par16 = errdes.par64 = 0;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);
        return eores_OK;
    }


    extern eOresult_t eo_mais_Verify(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
    {
        // we alert the host that the verification of the service has failed
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, eomn_serv_state_failureofverify);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        } 
        
        // we tell that the reason is that this service is not supported
        eo_mais_SendReport(NULL);
               
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mais_Activate(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg)
    {
        eo_mais_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mais_Deactivate(EOtheMAIS *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mais_Start(EOtheMAIS *p)
    {
        eo_mais_SendReport(NULL);
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mais_SetRegulars(EOtheMAIS *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
    {
        if(NULL != arrayofid32)
        {
            eo_mais_SendReport(NULL);
        }
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mais_Tick(EOtheMAIS *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mais_Stop(EOtheMAIS *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mais_Transmission(EOtheMAIS *p, eObool_t on)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mais_Config(EOtheMAIS *p, eOas_inertial3_config_t* config)
    {
        eo_mais_SendReport(NULL);
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_mais_AcceptCANframe(EOtheMAIS *p, eOcanframe_t *frame, eOcanport_t port, maisProcessMode_t mode)
    {
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_mais_Set(EOtheMAIS *p, eOas_mais_config_t* maiscfg)
    {
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_mais_SetMode(EOtheMAIS *p, eOas_maismode_t mode)
    {
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_mais_SetDataRate(EOtheMAIS *p, uint8_t datarate)
    {
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_mais_SetResolution(EOtheMAIS *p, eOas_maisresolution_t resolution)
    {
        return eores_NOK_generic;
    }
    
    
    extern eObool_t eo_mais_isAlive(EOtheMAIS *p)
    {
        return eores_NOK_generic;
    }


#elif !defined(EOTHESERVICES_disable_theMAIS)

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

static eOresult_t s_eo_mais_TXstart(EOtheMAIS *p, uint8_t datarate, eOas_maismode_t mode, eOas_maisresolution_t resolution);

static eOresult_t s_eo_mais_TXstop(EOtheMAIS *p);

static void s_eo_mais_process_mais_resolution(eOas_maisresolution_t resolution, eOas_mais_status_t* status);

static eOresult_t s_eo_mais_onstop_search4mais(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok);

static void s_eo_mais_send_periodic_error_report(void *par);

static eObool_t s_eo_mais_isID32relevant(uint32_t id32);

static void s_eo_mais_send_diagnostic_on_transmissioninterruption(void);

static eOresult_t s_eocanprotASperiodic_parser_process_maisvalue(eOcanframe_t *frame, eOcanport_t port, maisProcessMode_t mode);

static eOresult_t eo_mais_notifymeOnNewReceivedData(EOtheMAIS *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheMAIS s_eo_themais = 
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

    EO_INIT(.id32)                      eo_prot_ID32dummy,  
    EO_INIT(.mais)                      NULL,
    EO_INIT(.id32ofregulars)            NULL,
    EO_INIT(.numberofowners)            0,
    EO_INIT(.watchdog)                  NULL
};

static const char s_eobj_ownname[] = "EOtheMAIS";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMAIS* eo_mais_Initialise(void)
{
    EOtheMAIS* p = &s_eo_themais;

    if(eobool_true == p->service.initted)
    {
        return(p);
    }
    
    p->id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_mais, 0, eoprot_tag_none);

    p->service.servconfig.type = eomn_serv_NONE;
    
    
    p->sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), 1, NULL, NULL, NULL, NULL);
    
    p->sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), 1, NULL, NULL, NULL, NULL);
    
    p->sharedcan.discoverytargets = eo_array_New(1, sizeof(eOcandiscovery_target_t),  NULL);
    
    p->mais = NULL;
    p->id32ofregulars = eo_array_New(mais_maxRegulars, sizeof(uint32_t), NULL);
    p->numberofowners = 0;
    
    eOwatchdog_cfg_t wd_cfg = {0};
    wd_cfg.diagncfg.numoffailures = 40000;
    wd_cfg.diagncfg.functiononfailure = s_eo_mais_send_diagnostic_on_transmissioninterruption;
    //wd_cfg.period = 10 /*deafult transmission period of mais*/  *100 /*convert in microsec*/ *10 /*before signal error i would wait 10 times mais transmission period*/;
    wd_cfg.period = 10*1000*10; // marco.accame ... there are 1000 usec in 1 ms.
    
    p->watchdog = eo_watchdog_new(&wd_cfg);
    
    p->diagnostics.reportTimer = eo_timer_New();
    p->diagnostics.errorType = eo_errortype_error;
    p->diagnostics.errorDescriptor.sourceaddress = 0;
    p->diagnostics.errorDescriptor.sourcedevice = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mais_not_verified_yet);  
        
    p->service.initted = eobool_true;    
    p->service.active = eobool_false;
    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_idle;    
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);
       
    return(p);   
}


extern EOtheMAIS* eo_mais_GetHandle(void)
{
    if(eobool_true == s_eo_themais.service.initted)
    {
        return(&s_eo_themais);
    }
    
    return(NULL);
}


extern eOmn_serv_state_t eo_mais_GetServiceState(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(eomn_serv_state_notsupported);
    } 

    return(p->service.state);
}


extern eOresult_t eo_mais_SendReport(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    // the report is sent in any state ....

    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    eOerror_value_t errorvalue = eoerror_code2value(p->diagnostics.errorDescriptor.code);
    
    switch(errorvalue)
    {
        case eoerror_value_CFG_mais_failed_candiscovery:
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


extern eOresult_t eo_mais_Verify(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    // eo_errman_Trace(eo_errman_GetHandle(), "called: eo_mais_Verify()", s_eobj_ownname);
    
    if((NULL == p) || (NULL == servcfg))
    {
        s_eo_themais.service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, s_eo_themais.service.state);
        
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }            
        return(eores_NOK_nullpointer);
    }  
    
    if(eomn_serv_AS_mais != servcfg->type)
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }          
        return(eores_NOK_generic);
    } 



    if(eobool_true == p->service.active)
    {
        // it means that some other object has activated the mais. it can be either EOtheMotionController or EOtheServices upon request from embObjMais
        // ... we dont want to interfere, thus we cannot call eo_mais_Deactivate().  
        // we just verify that the eOmn_serv_configuration_t* in argument is the same of (or compatible with) the one already used for activation.
        
        eObool_t verificationOK = eobool_false;
        
        eObool_t iscompatible = eobool_true;
        
        if(servcfg->data.as.mais.canloc.port != p->service.servconfig.data.as.mais.canloc.port)
        {
            iscompatible = eobool_false;
        }
        if(servcfg->data.as.mais.canloc.addr != p->service.servconfig.data.as.mais.canloc.addr)
        {
            iscompatible = eobool_false;
        } 

        if((servcfg->data.as.mais.version.protocol.major != 0) || (servcfg->data.as.mais.version.protocol.minor != 0))
        {
            // check also the protocol
            if(servcfg->data.as.mais.version.protocol.major != p->service.servconfig.data.as.mais.version.protocol.major)
            {
                iscompatible = eobool_false;
            }
            if(servcfg->data.as.mais.version.protocol.minor != p->service.servconfig.data.as.mais.version.protocol.minor)
            {
                iscompatible = eobool_false;
            }
        }
        if((servcfg->data.as.mais.version.firmware.major != 0) || (servcfg->data.as.mais.version.firmware.minor != 0)  || (servcfg->data.as.mais.version.firmware.build != 0))
        {
            // check also the firmaware
            if(servcfg->data.as.mais.version.firmware.major != p->service.servconfig.data.as.mais.version.firmware.major)
            {
                iscompatible = eobool_false;
            }
            if(servcfg->data.as.mais.version.firmware.minor != p->service.servconfig.data.as.mais.version.firmware.minor)
            {
                iscompatible = eobool_false;
            }
            if(servcfg->data.as.mais.version.firmware.build != p->service.servconfig.data.as.mais.version.firmware.build)
            {
                iscompatible = eobool_false;
            }
        }
        
        
        //if(0 == memcmp(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t)))
        //if(0 == memcmp(&p->service.servconfig.data.as.mais, &servcfg->data.as.mais, sizeof(eOmn_serv_config_data_as_mais_t)))
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
            p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mais_failed_verify_because_active);
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
 
    
    p->service.state = eomn_serv_state_verifying;   
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);

    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);      

    p->service.onverify = onverify;
    p->service.activateafterverify = activateafterverify;

    eOcandiscovery_target_t trgt = {0};
     
    trgt.info.type = eobrd_cantype_mais;
    trgt.info.protocol.major = servcfg->data.as.mais.version.protocol.major; 
    trgt.info.protocol.minor = servcfg->data.as.mais.version.protocol.minor;
    trgt.info.firmware.major = servcfg->data.as.mais.version.firmware.major; 
    trgt.info.firmware.minor = servcfg->data.as.mais.version.firmware.minor;
    trgt.info.firmware.build = servcfg->data.as.mais.version.firmware.build;    
    trgt.canmap[servcfg->data.as.mais.canloc.port] = 0x0001 << servcfg->data.as.mais.canloc.addr; 

    // force a cleaned discoverytargets before we add the target
    eo_array_Reset(p->sharedcan.discoverytargets);    
    eo_array_PushBack(p->sharedcan.discoverytargets, &trgt);
    
    p->sharedcan.ondiscoverystop.function = s_eo_mais_onstop_search4mais;
    p->sharedcan.ondiscoverystop.parameter = (void*)servcfg;
        
    // start discovery    
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), p->sharedcan.discoverytargets, &p->sharedcan.ondiscoverystop);   
    
    return(eores_OK);   
}

extern uint8_t eo_mais_GetNumberOfOwners(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(0);
    } 

    return(p->numberofowners);   
}

extern eOresult_t eo_mais_Deactivate(EOtheMAIS *p)
{
    // eo_errman_Trace(eo_errman_GetHandle(), "called: eo_mais_Deactivate()", s_eobj_ownname);
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == p->service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        p->service.state = eomn_serv_state_idle; 
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);
        return(eores_OK);        
    } 
    
    // send stop messages to mais, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object
       
    if(eobool_true == p->service.started)
    {
        eo_mais_Stop(p);
    }
    
    if(0 != p->numberofowners)
    {   // _Stop() decrements numberofowners and we can _Deactivate() only when there is no owner anymore
        return(eores_OK); 
    }
    
    eo_mais_SetRegulars(p, NULL, NULL);
    
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_mais, p->sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfMaises(eo_entities_GetHandle(), 0);
    
    p->mais = NULL;
    
    memset(&p->service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    p->service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(p->sharedcan.boardproperties);
    eo_vector_Clear(p->sharedcan.entitydescriptor);
    eo_array_Reset(p->sharedcan.discoverytargets);
    
    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer); 
    
    p->service.active = eobool_false;    
    p->service.state = eomn_serv_state_idle; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);
    
    return(eores_OK);
}


extern eOresult_t eo_mais_Activate(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg)
{
    // eo_errman_Trace(eo_errman_GetHandle(), "called: eo_mais_Activate()", s_eobj_ownname);
    
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eomn_serv_AS_mais != servcfg->type)
    {
        return(eores_NOK_generic);
    }    
    
    if(eobool_true == p->service.active)
    {
        // it means that some other object has activated the mais. it can be either EOtheMotionController or EOtheServices upon request from embObjMais
        // ... we dont want to interfere, thus we cannot call eo_mais_Deactivate().  
        // we just verify that the eOmn_serv_configuration_t* in argument is the same of (or compatible with) the one already used for activation.
        
        eObool_t verificationOK = eobool_false;
        
        eObool_t iscompatible = eobool_true;
        
        if(servcfg->data.as.mais.canloc.port != p->service.servconfig.data.as.mais.canloc.port)
        {
            iscompatible = eobool_false;
        }
        if(servcfg->data.as.mais.canloc.addr != p->service.servconfig.data.as.mais.canloc.addr)
        {
            iscompatible = eobool_false;
        } 

        if((servcfg->data.as.mais.version.protocol.major != 0) || (servcfg->data.as.mais.version.protocol.minor != 0))
        {
            // check also the protocol
            if(servcfg->data.as.mais.version.protocol.major != p->service.servconfig.data.as.mais.version.protocol.major)
            {
                iscompatible = eobool_false;
            }
            if(servcfg->data.as.mais.version.protocol.minor != p->service.servconfig.data.as.mais.version.protocol.minor)
            {
                iscompatible = eobool_false;
            }
        }
        if((servcfg->data.as.mais.version.firmware.major != 0) || (servcfg->data.as.mais.version.firmware.minor != 0)  || (servcfg->data.as.mais.version.firmware.build != 0))
        {
            // check also the firmaware
            if(servcfg->data.as.mais.version.firmware.major != p->service.servconfig.data.as.mais.version.firmware.major)
            {
                iscompatible = eobool_false;
            }
            if(servcfg->data.as.mais.version.firmware.minor != p->service.servconfig.data.as.mais.version.firmware.minor)
            {
                iscompatible = eobool_false;
            }
            if(servcfg->data.as.mais.version.firmware.build != p->service.servconfig.data.as.mais.version.firmware.build)
            {
                iscompatible = eobool_false;
            }
        }        
        
        //if(0 == memcmp(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t)))
        //if(0 == memcmp(&p->service.servconfig.data.as.mais, &servcfg->data.as.mais, sizeof(eOmn_serv_config_data_as_mais_t)))
        if(eobool_true == iscompatible)
        {
            verificationOK = eobool_true;
        }
        else
        {
            verificationOK = eobool_false;
        }
        
        // in here we dont activate because it is already active ...
        
        return((eobool_true == verificationOK) ? (eores_OK) : (eores_NOK_generic));        
    }   
    
    eo_entities_SetNumOfMaises(eo_entities_GetHandle(), 1);
    

    if(0 == eo_entities_NumOfMaises(eo_entities_GetHandle()))
    {
        p->service.active = eobool_false;
        return(eores_NOK_generic);
    }
    else
    {
        
        p->mais = eo_entities_GetMais(eo_entities_GetHandle(), 0);
        
        memcpy(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
            
        
        // now... use the servcfg
        eObrd_canproperties_t prop = {0};
        prop.type = eobrd_cantype_mais;
        prop.location.port = servcfg->data.as.mais.canloc.port;
        prop.location.addr = servcfg->data.as.mais.canloc.addr;
        prop.location.insideindex = eobrd_caninsideindex_none;
        prop.requiredprotocol.major = servcfg->data.as.mais.version.protocol.major;
        prop.requiredprotocol.minor = servcfg->data.as.mais.version.protocol.minor;
        
        eo_vector_PushBack(p->sharedcan.boardproperties, &prop);
        
        // load the can mapping ... make an UnloadBoards()
        eo_canmap_LoadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties); 
        
        // load the entity mapping.
        eOcanmap_entitydescriptor_t des = {0};
        des.location.port = servcfg->data.as.mais.canloc.port;
        des.location.addr = servcfg->data.as.mais.canloc.addr;
        des.location.insideindex = eobrd_caninsideindex_none;
        des.index = entindex00; // we have only one mais        
        
        eo_vector_PushBack(p->sharedcan.entitydescriptor, &des);
        
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_mais, p->sharedcan.entitydescriptor);   


        p->service.active = eobool_true;    
        p->service.state = eomn_serv_state_activated;   
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);        
    }
    
    return(eores_OK);   
}


extern eOresult_t eo_mais_Start(EOtheMAIS *p)
{
    // eo_errman_Trace(eo_errman_GetHandle(), "called: eo_mais_Start()", s_eobj_ownname);
    
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
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);   
    
    // now we activate tx according to config of mais
    //s_eo_mais_TXstart(p, p->mais->config.datarate, eoas_maismode_txdatacontinuously, (eOas_maisresolution_t)p->mais->config.resolution);   
    
    return(eores_OK);
}


extern eOresult_t eo_mais_SetRegulars(EOtheMAIS *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    return(eo_service_hid_SetRegulars(p->id32ofregulars, arrayofid32, s_eo_mais_isID32relevant, numberofthem));
}


extern eOresult_t eo_mais_Stop(EOtheMAIS *p)
{
    // eo_errman_Trace(eo_errman_GetHandle(), "called: eo_mais_Stop()", s_eobj_ownname);
    
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
     
    s_eo_mais_TXstop(p);
       
    p->service.started = eobool_false;
    p->service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);       
    
    // remove all regulars related to mais entity ... no, dont do that
    //eo_mais_SetRegulars(p, NULL, NULL);    

    return(eores_OK);
}


extern eOresult_t eo_mais_Tick(EOtheMAIS *p)
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
    
    // now we dont do anything becase mais does not need any action because everything is done by the can parser
    // motor controller performs watchdog check using eo_mais_isAlive
    
    return(eores_OK);       
}


extern eOresult_t eo_mais_Transmission(EOtheMAIS *p, eObool_t on)
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
    {   // we activate tx according to config of mais
        s_eo_mais_TXstart(p, p->mais->config.datarate, eoas_maismode_txdatacontinuously, (eOas_maisresolution_t)p->mais->config.resolution); 
    }
    else
    {
        s_eo_mais_TXstop(p);
    }
    
    return(eores_OK);      
}


extern eOresult_t eo_mais_AcceptCANframe(EOtheMAIS *p, eOcanframe_t *frame, eOcanport_t port, maisProcessMode_t mode)
{
    if((NULL == p) || (NULL == frame))
    {
        return(eores_NOK_nullpointer);
    }
    s_eocanprotASperiodic_parser_process_maisvalue(frame, port, mode);
    return eores_OK;
}


extern eOresult_t eo_mais_Set(EOtheMAIS *p, eOas_mais_config_t* maiscfg)
{
    if((NULL == p) || (NULL == maiscfg))
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
    
    eOresult_t res = eores_OK;
    
    if(eores_OK != (res = eo_mais_SetDataRate(p, maiscfg->datarate)))
    {
        return(res);
    }   
    
    if(eores_OK != (res = eo_mais_SetResolution(p, (eOas_maisresolution_t)maiscfg->resolution)))
    {
        return(res);
    }
    
    if(eores_OK != (res = eo_mais_SetMode(p, (eOas_maismode_t)maiscfg->mode)))
    {
        return(res);
    }
       
    return(eores_OK);     
}


extern eOresult_t eo_mais_SetMode(EOtheMAIS *p, eOas_maismode_t mode)
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

    // ok, now we do something.
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    p->sharedcan.command.value = &mode;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);
    
    if(eoas_maismode_txdatacontinuously == mode)
    {
        eo_watchdog_start(p->watchdog);
    }
    else
    {
        eo_watchdog_stop(p->watchdog);
    }
        
    return(eores_OK);
}


extern eOresult_t eo_mais_SetDataRate(EOtheMAIS *p, uint8_t datarate)
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

    // ok, now we do something.     
 
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    p->sharedcan.command.value = &datarate;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);    
    
    
//    eo_watchdog_updateconfigperiod(p->watchdog, datarate*10*100); //I multiply *10 ==> so I wait a period ten tiems bigger than datarate befor signal error
//                                                                               //I multiply *100 ==> datarate is in millisec while period is in microsecs.
    eo_watchdog_updateconfigperiod(p->watchdog, datarate*10*1000);; // marco.accame ... there are 1000 usec in 1 ms.
    return(eores_OK);  
}



extern eOresult_t eo_mais_SetResolution(EOtheMAIS *p, eOas_maisresolution_t resolution)
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

    // ok, now we do something.     
 
    p->sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
    p->sharedcan.command.value = &resolution;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);    
    
    eOas_mais_status_t *status = eo_entities_GetMaisStatus(eo_entities_GetHandle(), 0);
    if(NULL == status)
    {
        return(eores_NOK_generic); //error
    }     

    s_eo_mais_process_mais_resolution(resolution, status);  
    
    return(eores_OK);          
}



extern eObool_t eo_mais_isAlive(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }

    return(eo_watchdog_check(p->watchdog));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to MAIS are in a unique place


extern void eoprot_fun_INIT_as_mais_config(const EOnv* nv)
{
    eOas_mais_config_t* maiscfg = (eOas_mais_config_t*) eo_nv_RAM(nv);
    
    maiscfg->datarate = 10;
    maiscfg->mode = eoas_maismode_txdatacontinuously;
    maiscfg->resolution = eoas_maisresolution_08;
}


extern void eoprot_fun_INIT_as_mais_status(const EOnv* nv)
{
    eOas_mais_status_t *status = (eOas_mais_status_t*) eo_nv_RAM(nv);  
    
    // marco.accame: i init as for eoas_maisresolution_08 
    //               the array the15values can be initted for size 0 or 15 as i now use teh proper eo_array_Assign() method
    uint8_t capacity    = 15;
    uint8_t itemsize    = 1;
    uint8_t size        = 15;
    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
    eo_array_Resize(array, size);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_mais_TXstart(EOtheMAIS *p, uint8_t datarate, eOas_maismode_t mode, eOas_maisresolution_t resolution)
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
    
    eOas_mais_config_t config = {0};
    config.datarate = datarate;
    config.mode = mode;
    config.resolution = resolution;
    
    return(eo_mais_Set(p, &config));      
}


static eOresult_t s_eo_mais_TXstop(EOtheMAIS *p)
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
    
    // marco.accame on 28apr16: i prefer to NEVER stop the tx of the mais can board to avoid the mc4 can board may go in hw fault
    // eo_mais_SetMode(p, eoas_maismode_acquirebutdonttx);
 
    return(eores_OK);
}


static void s_eo_mais_process_mais_resolution(eOas_maisresolution_t resolution, eOas_mais_status_t* status)
{
    uint8_t size = 0;
    uint8_t itemsize = 1;
    uint8_t capacity = 0;
    
    if(eoas_maisresolution_08 == resolution)
    {
        capacity    = 15;
        itemsize    = 1;
        size        = 15;
    }
    else if(eoas_maisresolution_16 == resolution)
    {
        capacity    = 15;
        itemsize    = 2;
        size        = 15;
    } 

    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
    eo_array_Resize(array, size);      
}


static eOresult_t s_eo_mais_onstop_search4mais(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{  
    EOtheMAIS* p = &s_eo_themais;
    
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    
    if(eobool_true == searchisok)
    {
        p->service.state = eomn_serv_state_verified;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);   
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_mais, p->service.state);   
    }    
    
    if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
    {
        eo_mais_Activate(&s_eo_themais, servcfg);        
    }
    
    p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress    = 0;
    p->diagnostics.errorDescriptor.par16            = (servcfg->data.as.mais.canloc.addr) | (servcfg->data.as.mais.canloc.port << 8);
    p->diagnostics.errorDescriptor.par64            = (servcfg->data.as.mais.version.firmware.minor << 0) | (servcfg->data.as.mais.version.firmware.major << 8) |
                                                      (servcfg->data.as.mais.version.protocol.minor << 16) | (servcfg->data.as.mais.version.protocol.major << 24);    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_mais_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
    
    if(eobool_true == searchisok)
    {           
        p->diagnostics.errorType = eo_errortype_debug;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mais_ok);
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
        {
            p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }    
    
    if(eobool_false == searchisok)
    {
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mais_failed_candiscovery);
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
        p->service.onverify(&s_eo_themais, searchisok); 
    }    
    
    return(eores_OK);   
}


static void s_eo_mais_send_periodic_error_report(void *par)
{
    EOtheMAIS* p = (EOtheMAIS*) par;
    
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    if(eoerror_value_CFG_mais_failed_candiscovery == eoerror_code2value(p->diagnostics.errorDescriptor.code))
    {   // if i dont find the mais, i keep on sending the discovery results up. it is a temporary diagnostics tricks until we use the verification of services at bootstrap
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


static eObool_t s_eo_mais_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_analogsensors) << 24) | (((uint32_t)eoprot_entity_as_mais) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    
    return(eobool_false); 
}



static void s_eo_mais_send_diagnostic_on_transmissioninterruption(void)
{
    eOerrmanDescriptor_t descriptor = {0};
    descriptor.par16 = 0;
    descriptor.par64 = 0;
    descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
    descriptor.sourceaddress = 0;
    descriptor.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag02);
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, "mais timeout", NULL, &descriptor);
}

static eOresult_t s_eocanprotASperiodic_parser_process_maisvalue(eOcanframe_t *frame, eOcanport_t port, maisProcessMode_t mode)
{
    // this can frame is from mais only ... i dont do the check that the board must be a mais
    // i retrieve the mais entity related to the frame    
    eOas_mais_t *mais = s_eo_themais.mais;
    //eOprotIndex_t index = EOK_uint08dummy;    
    //if(NULL == (mais = (eOas_mais_t*) s_eocanprotASperiodic_get_entity(eoprot_endpoint_analogsensors, eoprot_entity_as_mais, frame, port, &index)))
    
    if(NULL == mais)
    {
        return(eores_OK);  
    }  

    EOarray* array = (EOarray*)&mais->status.the15values;
    if(processHES0TO6 == mode)
    {
        eo_array_Assign(array, 0, &(frame->data[0]), 7); // 7 bytes of frame->data starting from array position 0 (0, 1, .. , 6)
    }
    else //if(processHES7TO14 == mode)
    {
        eo_array_Assign(array, 7, &(frame->data[0]), 8); // 8 bytes of frame->data starting from array position 7 (7, 8, .. , 14)
    }
        
    eo_mais_notifymeOnNewReceivedData(eo_mais_GetHandle());
    
    return(eores_OK);       
}

static eOresult_t eo_mais_notifymeOnNewReceivedData(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    eo_watchdog_rearm(p->watchdog);

    return(eores_OK);
}

#endif // #elif !defined(EOTHESERVICES_disable_theMAIS)


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



