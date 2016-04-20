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

#include "EOtheCANprotocol_functions.h"

#include "EOVtheCallbackManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSTRAIN.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheSTRAIN_hid.h"


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

//static eOresult_t s_eo_strain_TXstart_params(EOtheSTRAIN *p, uint8_t datarate, eOas_strainmode_t mode);

static eOresult_t s_eo_strain_TXstart(EOtheSTRAIN *p);
static eOresult_t s_eo_strain_TXstop(EOtheSTRAIN *p);
static eOresult_t s_eo_strain_SendTXmode(EOtheSTRAIN *p);

static void s_eo_thestrain_startGetFullScales(void);

static eOresult_t s_eo_strain_onstop_search4strain(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok);

static void s_eo_strain_send_periodic_error_report(void *par);

static eOresult_t s_eo_thestrain_on_fullscale_ready(EOtheSTRAIN* p, eObool_t operationisok);

static eObool_t s_eo_strain_isID32relevant(uint32_t id32);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheSTRAIN s_eo_thestrain = 
{
    .service = 
    {
        .servconfig             = { .type = eomn_serv_NONE },
        .initted                = eobool_false,
        .active                 = eobool_false,
        .activateafterverify    = eobool_false,
        .running                = eobool_false,
        .onverify               = NULL,
        .state                  = eomn_serv_state_notsupported         
    },
    .diagnostics = 
    {
        .reportTimer            = NULL,
        .reportPeriod           = 0, // 10*EOK_reltime1sec, // with 0 we dont periodically report
        .errorDescriptor        = {0},
        .errorType              = eo_errortype_info,
        .errorCallbackCount     = 0,
        .repetitionOKcase       = 0 // 10 // with 0 we transmit report only once at succesful activation
    },     
    .sharedcan =
    {
        .boardproperties        = NULL,
        .entitydescriptor       = NULL,
        .discoverytarget        = {0},
        .ondiscoverystop        = {0},
        .command                = {0}, 
    },
    .id32                       = eo_prot_ID32dummy,   
    .overrideonfullscaleready   = NULL,
    .strain                     = NULL,
    .id32ofregulars             = NULL
};

static const char s_eobj_ownname[] = "EOtheSTRAIN";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheSTRAIN* eo_strain_Initialise(void)
{
    EOtheSTRAIN* p = &s_eo_thestrain;
    
    if(eobool_true == p->service.initted)
    {
        return(p);
    }
    
    p->id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, 0, eoprot_tag_none);

    p->service.servconfig.type = eomn_serv_NONE;
    
    
    p->sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), 1, NULL, NULL, NULL, NULL);
    
    p->sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), 1, NULL, NULL, NULL, NULL);
    
    p->strain = NULL;
    p->id32ofregulars = eo_array_New(strain_maxRegulars, sizeof(uint32_t), NULL);
    
    p->overrideonfullscaleready = NULL;
    
    p->diagnostics.reportTimer = eo_timer_New();
    p->diagnostics.errorType = eo_errortype_error;
    p->diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_strain_not_verified_yet);      
        
    p->service.initted = eobool_true;
    p->service.active = eobool_false;
    p->service.running = eobool_false;    
    p->service.state = eomn_serv_state_idle;  
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);       
    
    return(p);   
}


extern EOtheSTRAIN* eo_strain_GetHandle(void)
{
    if(eobool_true == s_eo_thestrain.service.initted)
    {
        return(&s_eo_thestrain);
    }
    
    return(NULL);
}


extern eOmn_serv_state_t eo_strain_GetServiceState(EOtheSTRAIN *p)
{
    if(NULL == p)
    {
        return(eomn_serv_state_notsupported);
    } 

    return(p->service.state);
}


extern eOresult_t eo_strain_SendReport(EOtheSTRAIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
            
    eOerror_value_t errorvalue = eoerror_code2value(p->diagnostics.errorDescriptor.code);
    
    switch(errorvalue)
    {
        case eoerror_value_CFG_strain_failed_candiscovery:
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


extern eOresult_t eo_strain_Verify(EOtheSTRAIN *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    if((NULL == p) || (NULL == servcfg))
    {
        s_eo_thestrain.service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, s_eo_thestrain.service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }                 
        return(eores_NOK_nullpointer);
    }  
 
    if(eomn_serv_AS_strain != servcfg->type)
    {
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);
        if(NULL != onverify)
        {
            onverify(p, eobool_false); 
        }          
        return(eores_NOK_generic);
    } 
    
 
    if(eobool_true == p->service.active)
    {
        eo_strain_Deactivate(p);        
    }  

    p->service.state = eomn_serv_state_verifying;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);

    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);  
    
    p->service.onverify = onverify;
    p->service.activateafterverify = activateafterverify;


    p->sharedcan.discoverytarget.info.type = eobrd_cantype_strain;
    p->sharedcan.discoverytarget.info.protocol.major = servcfg->data.as.strain.version.protocol.major; 
    p->sharedcan.discoverytarget.info.protocol.minor = servcfg->data.as.strain.version.protocol.minor;
    p->sharedcan.discoverytarget.info.firmware.major = servcfg->data.as.strain.version.firmware.major; 
    p->sharedcan.discoverytarget.info.firmware.minor = servcfg->data.as.strain.version.firmware.minor; 
    p->sharedcan.discoverytarget.info.firmware.build = servcfg->data.as.strain.version.firmware.build;     
    p->sharedcan.discoverytarget.canmap[servcfg->data.as.strain.canloc.port] = 0x0001 << servcfg->data.as.strain.canloc.addr; 
    
    p->sharedcan.ondiscoverystop.function = s_eo_strain_onstop_search4strain;
    p->sharedcan.ondiscoverystop.parameter = (void*)servcfg;
    
    // start discovery
    eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &p->sharedcan.discoverytarget, &p->sharedcan.ondiscoverystop);   
    
    return(eores_OK);   
}


extern eOresult_t eo_strain_Deactivate(EOtheSTRAIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == p->service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        p->service.state = eomn_serv_state_idle; 
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);
        return(eores_OK);        
    } 
    
    // send stop messages to strain, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object

    if(eobool_true == p->service.running)
    {
        eo_strain_Stop(p);
    }
    
    eo_strain_SetRegulars(p, NULL, NULL);
    
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_strain, p->sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfStrains(eo_entities_GetHandle(), 0);
    
    p->strain = NULL;
    
    memset(&p->service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    p->service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(p->sharedcan.boardproperties);
    eo_vector_Clear(p->sharedcan.entitydescriptor);
    
    // make sure the timer is not running
    eo_timer_Stop(p->diagnostics.reportTimer);    
    
    p->service.active = eobool_false;    
    p->service.state = eomn_serv_state_idle; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);
    
    return(eores_OK);
}


extern eOresult_t eo_strain_Activate(EOtheSTRAIN *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }    

    if(eomn_serv_AS_strain != servcfg->type)
    {
        return(eores_NOK_generic);
    }  
    
    if(eobool_true == p->service.active)
    {
        eo_strain_Deactivate(p);        
    }   
    
    eo_entities_SetNumOfStrains(eo_entities_GetHandle(), 1);
    

    if(0 == eo_entities_NumOfStrains(eo_entities_GetHandle()))
    {
        p->service.active = eobool_false;
        return(eores_NOK_generic);
    }
    else
    {   
        p->strain = eo_entities_GetStrain(eo_entities_GetHandle(), 0);
        
        memcpy(&p->service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
            
        
        // now... use the servcfg
        eObrd_canproperties_t prop = 
        {
            .type               = eobrd_cantype_strain, 
            .location           = { .port = servcfg->data.as.strain.canloc.port, .addr = servcfg->data.as.strain.canloc.addr, .insideindex = eobrd_caninsideindex_none },
            .requiredprotocol   = { .major = servcfg->data.as.strain.version.protocol.major, .minor = servcfg->data.as.strain.version.protocol.minor }
        };       
        eo_vector_PushBack(p->sharedcan.boardproperties, &prop);
        
        // load the can mapping ... make an UnloadBoards()
        eo_canmap_LoadBoards(eo_canmap_GetHandle(), p->sharedcan.boardproperties); 
        
        // load the entity mapping.
        eOcanmap_entitydescriptor_t des = 
        {
            .location   = { .port = servcfg->data.as.strain.canloc.port, .addr = servcfg->data.as.strain.canloc.addr, .insideindex = eobrd_caninsideindex_none },
            .index      = entindex00 // we have only one strain
        };
        eo_vector_PushBack(p->sharedcan.entitydescriptor, &des);
        
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_strain, p->sharedcan.entitydescriptor);   


        p->service.active = eobool_true;
        p->service.state = eomn_serv_state_activated; 
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);        
    }
   
    return(eores_OK);   
}


extern eOresult_t eo_strain_Start(EOtheSTRAIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    if(eobool_true == p->service.running)
    {   // it is already running
        return(eores_OK);
    }   
    
    p->service.running = eobool_true;    
    p->service.state = eomn_serv_state_running; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);
    
    // start() does not force the tx of the board. 
    // this function just config the strain according to values in strain.config (default are: 10 ms, acquire but dont tx)
    // unless someone has changed the strain.config with eth protocol messages) 
    s_eo_strain_SendTXmode(p);   
    
    return(eores_OK);     
}


extern eOresult_t eo_strain_SetRegulars(EOtheSTRAIN *p, eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    return(eo_service_hid_SetRegulars(p->id32ofregulars, arrayofid32, s_eo_strain_isID32relevant, numberofthem));
}


extern eOresult_t eo_strain_Stop(EOtheSTRAIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
   
   
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    if(eobool_false == p->service.running)
    {   // it is already stopped
        return(eores_OK);
    }  
    
    s_eo_strain_TXstop(p);   

    p->service.running = eobool_false;
    p->service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);    
    
    // remove all regulars related to strain entity ... no, dont do that.
    //eo_strain_SetRegulars(p, NULL, NULL);    

    return(eores_OK);    
}


extern eOresult_t eo_strain_Tick(EOtheSTRAIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    } 

    if(eobool_false == p->service.running)
    {   // not running, thus we do nothing
        return(eores_OK);
    }     
    
    // strain does not need any action because everything is done by the can parser
    
    return(eores_OK);         
}



extern eOresult_t eo_strain_Transmission(EOtheSTRAIN *p, eObool_t on)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    } 

    if(eobool_false == p->service.running)
    {   // not running, thus we do nothing
        return(eores_OK);
    }     
    
    // we force transmission
    if(eobool_true == on)
    {
        s_eo_strain_TXstart(p);
    }
    else
    {
        s_eo_strain_TXstop(p);
    }
    
    return(eores_OK);        
}



extern eOresult_t eo_strain_Set(EOtheSTRAIN *p, eOas_strain_config_t *cfg)
{
    if((NULL == p) || (NULL == cfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  

// we allow doing things also if we are not in running mode yet     
//    if(eobool_false == p->service.running)
//    {   // not running, thus we do nothing
//        return(eores_OK);
//    }     
          
    // now, i do things.  

    eo_strain_SetMode(p, (eOas_strainmode_t)cfg->mode);
    
    eo_strain_SetDataRate(p, cfg->datarate);
    
    if(eobool_true == cfg->signaloncefullscale)
    {
        eo_strain_GetFullScale(p, NULL);
    }

    return(eores_OK);
}


extern eOresult_t eo_strain_SetMode(EOtheSTRAIN *p, eOas_strainmode_t mode)
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
//    if(eobool_false == p->service.running)
//    {   // not running, thus we do nothing
//        return(eores_OK);
//    }     
          
    // now, i do things. 

    p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    p->sharedcan.command.value = &mode;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);

    return(eores_OK); 
}


extern eOresult_t eo_strain_SetDataRate(EOtheSTRAIN *p, uint8_t datarate)
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
//    if(eobool_false == p->service.running)
//    {   // not running, thus we do nothing
//        return(eores_OK);
//    }     
          
    // now, i do things. 

    p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    p->sharedcan.command.value = &datarate;
   
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);

    return(eores_OK); 
}



extern eOresult_t eo_strain_GetFullScale(EOtheSTRAIN *p, eOservice_onendofoperation_fun_t overrideonfullscaleready)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
      

    if((eobool_false == p->service.active))
    {   // nothing to do because object must be first activated
//        if(NULL != overrideonfullscaleready)
//        {
//            overrideonfullscaleready(p, eobool_false);
//        }
//        else
//        {
//            s_eo_thestrain_on_fullscale_ready(p, eobool_false);
//        }
        return(eores_OK);
    }
    
// we allow doing things also if we are not in running mode yet     
//    if(eobool_false == p->service.running)
//    {   // not running, thus we do nothing
//        return(eores_OK);
//    }     
    
    p->overrideonfullscaleready = overrideonfullscaleready;
    
    // now, i do things. 
    s_eo_thestrain_startGetFullScales();

    return(eores_OK); 
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// - in here i put the functions used to initialise the values in ram of the strain ... better in here rather than elsewhere.


extern void eoprot_fun_INIT_as_strain_config(const EOnv* nv)
{
    eOas_strain_config_t* straincfg = eo_nv_RAM(nv);
    
    straincfg->datarate = 10;
    straincfg->mode = eoas_strainmode_acquirebutdonttx;
    straincfg->signaloncefullscale = eobool_false;
}


extern void eoprot_fun_INIT_as_strain_status(const EOnv* nv)
{
    eOas_strain_status_t *status = eo_nv_RAM(nv);
    uint8_t capacity = 0;
    uint8_t itemsize = 0;
    uint8_t size = 0;
    EOarray* array = NULL;
    
    // marco.accame: cannot understand why in here size is zero and in others it was not zero. see why. 
    //               probably fullscale, being of itemsize = 2 is treated without the proper eo_array_ methods
    capacity    = 6;
    itemsize    = 2;
    size        = 0; 
    array = eo_array_New(capacity, itemsize, &status->fullscale);
    eo_array_Resize(array, size);


    // marco.accame: size can be 0 or 6. now i use teh proper eo_array_Assign method for calibratedvalues/uncalibratedvalues 
    //
    capacity    = 6;
    itemsize    = 2;
    size        = 6;
    array = eo_array_New(capacity, itemsize, &status->calibratedvalues);
    eo_array_Resize(array, size);   

    capacity    = 6;
    itemsize    = 2;
    size        = 6;
    array = eo_array_New(capacity, itemsize, &status->uncalibratedvalues);
    eo_array_Resize(array, size); 
}


// this function is called by the parser of message ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES after it has added the
// value of the specified channel inside the relevant position of strain->status.fullscale.
// in this function is implemented the chain of requests from channel 0 up to 5 with successive signalling to robotinterface
// VERY IMPORTANT: the function must return eobool_true
extern eObool_t eocanprotASpolling_redefinable_alert_reception_of_POL_AS_CMD__GET_FULL_SCALES(uint8_t channel, uint16_t *value, eOas_strain_t* strain)
{
    const eObool_t ret = eobool_true;
    EOtheSTRAIN *p = eo_strain_GetHandle();
    if(NULL == p)
    {
        return(ret);
    }
    
    if(channel < 5)
    {   // send a request for next channel
        channel++;
        
        p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
        p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
        p->sharedcan.command.value = &channel;
        
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);
        
        return(ret);
    }
    
    // received the last channel: call s_eo_thestrain_on_fullscale_ready() or p->overrideonfullscaleready()
    
    if(NULL != p->overrideonfullscaleready)
    {
        p->overrideonfullscaleready(p, eobool_true);
    }
    else
    {
        s_eo_thestrain_on_fullscale_ready(p, eobool_true);
    }
    
    
    return(ret);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


//static eOresult_t s_eo_strain_TXstart_params(EOtheSTRAIN *p, uint8_t datarate, eOas_strainmode_t mode)
//{
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    if(eobool_false == p->service.active)
//    {   // nothing to do because we dont have a service.active board
//        return(eores_OK);
//    }   

//    eo_strain_SetDataRate(&s_eo_thestrain, datarate);  
//    
//    // set txmode
//    p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;    
//    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
//    p->sharedcan.command.value = &mode;                       
//    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);
//    
//    if(eoas_strainmode_acquirebutdonttx == mode)
//    {
//        p->service.running = eobool_false;
//    }
//    else
//    {
//        p->service.running = eobool_true;
//    }

//    return(eores_OK);
// 
//}


static eOresult_t s_eo_strain_SendTXmode(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
//    if((eobool_false == p->service.active))
//    {   // nothing to do because we dont have a service.active board
//        return(eores_OK);
//    }
          
    // now, i do things.     
    eOas_strain_config_t *cfg = &p->strain->config;

     
    // set txmode
    p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    p->sharedcan.command.value = &cfg->mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);
    
    return(eores_OK);
}


static eOresult_t s_eo_strain_TXstart(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
//    if((eobool_false == p->service.active))
//    {   // nothing to do because we dont have a service.active board
//        return(eores_OK);
//    }
          
    eOenum08_t mode = eoas_strainmode_txcalibrateddatacontinuously;
    
    p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    p->sharedcan.command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);
    
    return(eores_OK);
}

static eOresult_t s_eo_strain_TXstop(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
//    if((eobool_false == p->service.active))
//    {   // nothing to do because we dont have a service.active board
//        return(eores_OK);
//    }
          
    
    eOenum08_t mode = eoas_strainmode_acquirebutdonttx;       

    p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    p->sharedcan.command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);
    
    
    return(eores_OK);      
}

static void s_eo_thestrain_startGetFullScales(void)
{
    EOtheSTRAIN *p = eo_strain_GetHandle();
    if(NULL == p)
    {
        return;
    }
    
    if(NULL == p->strain)
    {
        return;
    }


    eOas_strain_status_t* status = &p->strain->status;

    // MUST impose that fullscale is an empty array of itemsize 2 and capacity 6. 
    // we have already done it inside the eoprot_fun_INIT_as_strain_status() function, but we do it again with eo_array_New()
    EOarray *array = eo_array_New(6, 2, &status->fullscale);
    
    // then we send a p->command to strain can board to send us the value of channel 0. 
    // when a the value of channel 0 is received by the can parser, we put the value inside the array at position 0
    // and we trigger a request for channel 1. then we repeat until we receive all the 6 values.
    // at the end of that, the full scale is signalled to  robotInterface
    
    uint8_t channel = 0;
    p->sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    p->sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
    p->sharedcan.command.value = &channel;

    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &p->sharedcan.command, p->id32);                  
}


static eOresult_t s_eo_thestrain_on_fullscale_ready(EOtheSTRAIN* p, eObool_t operationisok)
{
    eOresult_t res = eores_OK;
    
    if(eobool_false == operationisok)
    {
        return(eores_NOK_generic);
    }

    eOropdescriptor_t ropdesc = {0};

    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));

    ropdesc.ropcode                 = eo_ropcode_sig;
    ropdesc.size                    = sizeof(eOas_arrayofupto12bytes_t);
    ropdesc.id32                    = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, 0, eoprot_tag_as_strain_status_fullscale); //eo_strain_GetID32(eo_strain_GetHandle(), eoprot_tag_as_strain_status_fullscale); 
    ropdesc.data                    = NULL;

   
    res = eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
    if(eores_OK != res)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, "cannot sig<strain.fullscale>", NULL, &eo_errman_DescrRuntimeErrorLocal);        
        return(res);
    }
    else
    {
        //eOerrmanDescriptor_t des = {0};
        //des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag03);
        //des.param = 0x1111;
        //des.sourceaddress = 0;
        //des.sourcedevice = eo_errman_sourcedevice_localboard;
        //eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &des);
    }

    return(eores_OK);
}



static eOresult_t s_eo_strain_onstop_search4strain(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{   
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    EOtheSTRAIN* p = &s_eo_thestrain;

    if(eobool_true == searchisok)
    {
        p->service.state = eomn_serv_state_verified;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_strain, p->service.state);
    }
    
    if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
    {
        eo_strain_Activate(&s_eo_thestrain, servcfg);        
    }
    
    p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress    = 0;
    p->diagnostics.errorDescriptor.par16            = (servcfg->data.as.strain.canloc.addr) | (servcfg->data.as.strain.canloc.port << 8);
    p->diagnostics.errorDescriptor.par64            = (servcfg->data.as.strain.version.firmware.minor)       | (servcfg->data.as.strain.version.firmware.major << 8) |
                                                      (servcfg->data.as.strain.version.protocol.minor << 16) | (servcfg->data.as.strain.version.protocol.major << 24);    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_strain_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        
    if(eobool_true == searchisok)
    {        
        p->diagnostics.errorType = eo_errortype_debug;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_strain_ok);
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
        {
            p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }
    
    if(eobool_false == searchisok)
    {   
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_strain_failed_candiscovery);
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
        p->service.onverify(&s_eo_thestrain, searchisok); 
    }    
    
    return(eores_OK);   
}


static void s_eo_strain_send_periodic_error_report(void *par)
{
    EOtheSTRAIN* p = (EOtheSTRAIN*)par;
    
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    if(eoerror_value_CFG_strain_failed_candiscovery == eoerror_code2value(p->diagnostics.errorDescriptor.code))
    {   // if i dont find the strain, i keep on sending the discovery results up. it is a temporary diagnostics tricks until we use the verification of services at bootstrap
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

static eObool_t s_eo_strain_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_analogsensors) << 24) | (((uint32_t)eoprot_entity_as_strain) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    
    return(eobool_false); 
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



