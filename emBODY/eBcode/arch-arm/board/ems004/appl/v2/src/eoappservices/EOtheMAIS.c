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

#include "EOVtheCallbackManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMAIS.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMAIS_hid.h"


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

static void s_eo_mais_process_mais_resolution(eOas_maisresolution_t resolution, eOas_mais_status_t* status);

static eOresult_t s_eo_mais_onstop_search4mais(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

static void s_eo_mais_send_periodic_error_report(void *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheMAIS s_eo_themais = 
{
    .service = 
    {
        .servconfig             = { .type = eomn_serv_NONE },
        .initted                = eobool_false,
        .active                 = eobool_false,
        .activateafterverify    = eobool_false,
        .running                = eobool_false,
        .onverify               = NULL        
    },
    .diagnostics = 
    {
        .reportTimer            = NULL,
        .reportPeriod           = 10*EOK_reltime1sec,
        .errorDescriptor        = {0},
        .errorType              = eo_errortype_info,
        .errorCallbackCount     = 0,
        .repetitionOKcase       = 10
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
    .mais                       = NULL    
};

static const char s_eobj_ownname[] = "EOtheMAIS";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMAIS* eo_mais_Initialise(void)
{
    if(eobool_true == s_eo_themais.service.initted)
    {
        return(&s_eo_themais);
    }
    
    s_eo_themais.service.active = eobool_false;
    

    s_eo_themais.id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_mais, 0, eoprot_tag_none);

    s_eo_themais.service.servconfig.type = eomn_serv_NONE;
    
    
    s_eo_themais.sharedcan.boardproperties = eo_vector_New(sizeof(eOcanmap_board_properties_t), 1, NULL, NULL, NULL, NULL);
    
    s_eo_themais.sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), 1, NULL, NULL, NULL, NULL);
    
    s_eo_themais.mais = NULL;
    
    s_eo_themais.diagnostics.reportTimer = eo_timer_New();
        
    s_eo_themais.service.initted = eobool_true;    
       
    return(&s_eo_themais);   
}


extern EOtheMAIS* eo_mais_GetHandle(void)
{
    if(eobool_true == s_eo_themais.service.initted)
    {
        return(&s_eo_themais);
    }
    
    return(NULL);
}


extern eOresult_t eo_mais_Verify(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg, eOservice_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }  
 
// DONT Deactivate ... we may want just to check again ....    
//    if(eobool_true == s_eo_themais.service.active)
//    {
//        eo_mais_Deactivate(p);        
//    }   

    s_eo_themais.service.onverify = onverify;
    s_eo_themais.service.activateafterverify = activateafterverify;


    s_eo_themais.sharedcan.discoverytarget.boardtype = eobrd_cantype_mais;
    s_eo_themais.sharedcan.discoverytarget.protocolversion.major = servcfg->data.as.mais.version.protocol.major; 
    s_eo_themais.sharedcan.discoverytarget.protocolversion.minor = servcfg->data.as.mais.version.protocol.minor;
    s_eo_themais.sharedcan.discoverytarget.firmwareversion.major = servcfg->data.as.mais.version.firmware.major; 
    s_eo_themais.sharedcan.discoverytarget.firmwareversion.minor = servcfg->data.as.mais.version.firmware.minor;    
    s_eo_themais.sharedcan.discoverytarget.canmap[servcfg->data.as.mais.canloc.port] = 0x0001 << servcfg->data.as.mais.canloc.addr; 
    
    s_eo_themais.sharedcan.ondiscoverystop.function = s_eo_mais_onstop_search4mais;
    s_eo_themais.sharedcan.ondiscoverystop.parameter = (void*)servcfg;
        
    // make sure the timer is not running
    eo_timer_Stop(s_eo_themais.diagnostics.reportTimer); 
    
    // start discovery    
    eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &s_eo_themais.sharedcan.discoverytarget, &s_eo_themais.sharedcan.ondiscoverystop);   

    
    return(eores_OK);   
}


extern eOresult_t eo_mais_Deactivate(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == s_eo_themais.service.active)
    {
        return(eores_OK);        
    } 
    
    // send stop messages to mais, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object
       
    if(eobool_true == s_eo_themais.service.running)
    {
        eo_mais_TXstop(&s_eo_themais);
    }
    
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_mais, s_eo_themais.sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), s_eo_themais.sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfMaises(eo_entities_GetHandle(), 0);
    
    s_eo_themais.mais = NULL;
    
    memset(&s_eo_themais.service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    s_eo_themais.service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(s_eo_themais.sharedcan.boardproperties);
    eo_vector_Clear(s_eo_themais.sharedcan.entitydescriptor);
    
    // make sure the timer is not running
    eo_timer_Stop(s_eo_themais.diagnostics.reportTimer); 
    
    s_eo_themais.service.active = eobool_false;
    
    return(eores_OK);
}


extern eOresult_t eo_mais_Activate(EOtheMAIS *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_true == s_eo_themais.service.active)
    {
        eo_mais_Deactivate(p);        
    }   
    
    eo_entities_SetNumOfMaises(eo_entities_GetHandle(), 1);
    

    if(0 == eo_entities_NumOfMaises(eo_entities_GetHandle()))
    {
        s_eo_themais.service.active = eobool_false;
        return(eores_NOK_generic);
    }
    else
    {
        
        s_eo_themais.mais = eo_entities_GetMais(eo_entities_GetHandle(), 0);
        
        memcpy(&s_eo_themais.service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
            
        
        // now... use the servcfg
        eOcanmap_board_properties_t prop = 
        {
            .type               = eobrd_cantype_mais, 
            .location           = { .port = servcfg->data.as.mais.canloc.port, .addr = servcfg->data.as.mais.canloc.addr, .insideindex = eocanmap_insideindex_none },
            .requiredprotocol   = { .major = servcfg->data.as.mais.version.protocol.major, .minor = servcfg->data.as.mais.version.protocol.minor }
        };       
        eo_vector_PushBack(s_eo_themais.sharedcan.boardproperties, &prop);
        
        // load the can mapping ... make an UnloadBoards()
        eo_canmap_LoadBoards(eo_canmap_GetHandle(), s_eo_themais.sharedcan.boardproperties); 
        
        // load the entity mapping.
        eOcanmap_entitydescriptor_t des = 
        {
            .location   = { .port = servcfg->data.as.mais.canloc.port, .addr = servcfg->data.as.mais.canloc.addr, .insideindex = eocanmap_insideindex_none },
            .index      = entindex00 // we have only one mais
        };
        eo_vector_PushBack(s_eo_themais.sharedcan.entitydescriptor, &des);
        
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_mais, s_eo_themais.sharedcan.entitydescriptor);   


        s_eo_themais.service.active = eobool_true;        
    }

    
    return(eores_OK);   
}



extern eOresult_t eo_mais_Start(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.service.active)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }  
    
    return(eo_mais_TXstart(p, p->mais->config.datarate, eoas_maismode_txdatacontinuously, (eOas_maisresolution_t)p->mais->config.resolution));   
}

extern eOresult_t eo_mais_Stop(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.service.active)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }  
    
    return(eo_mais_TXstop(p));        
}


extern eOresult_t eo_mais_Tick(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.service.active)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }  
    
    return(eores_OK);       
}


extern eOresult_t eo_mais_TXstart(EOtheMAIS *p, uint8_t datarate, eOas_maismode_t mode, eOas_maisresolution_t resolution)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.service.active)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }   
    
    eOas_mais_config_t config = {0};
    config.datarate = datarate;
    config.mode = mode;
    config.resolution = resolution;
    
    return(eo_mais_Set(p, &config));      
}


extern eOresult_t eo_mais_TXstop(EOtheMAIS *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.service.active)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }   
    
    s_eo_themais.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    
    // set txmode
    uint8_t mode = eoas_maismode_acquirebutdonttx;
    s_eo_themais.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_themais.sharedcan.command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.sharedcan.command, s_eo_themais.id32);    
    
    
    s_eo_themais.service.running = eobool_false;
    
    return(eores_OK);
}


extern eOresult_t eo_mais_Set(EOtheMAIS *p, eOas_mais_config_t* maiscfg)
{
    if((NULL == p) || (NULL == maiscfg))
    {
        return(eores_NOK_nullpointer);
    }
        
    if(eobool_false == s_eo_themais.service.active)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }  

    // ok, now we do something. 
    
    eOresult_t res = eores_OK;
    
    if(eores_OK != (res = eo_mais_SetMode(p, (eOas_maismode_t)maiscfg->mode)))
    {
        return(res);
    }
    
    if(eores_OK != (res = eo_mais_SetDataRate(p, maiscfg->datarate)))
    {
        return(res);
    }   
    
    if(eores_OK != (res = eo_mais_SetResolution(p, (eOas_maisresolution_t)maiscfg->resolution)))
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
    
    if(eobool_false == s_eo_themais.service.active)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }  

    // ok, now we do something.  i always send the mode, even if i am in CFG state  
 
    s_eo_themais.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_themais.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_themais.sharedcan.command.value = &mode;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.sharedcan.command, s_eo_themais.id32);
    
    
    if(eoas_maismode_txdatacontinuously == mode)
    {
        s_eo_themais.service.running = eobool_true;
    }
    else
    {
        s_eo_themais.service.running = eobool_false;
    }    

    return(eores_OK);
}


extern eOresult_t eo_mais_SetDataRate(EOtheMAIS *p, uint8_t datarate)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.service.active)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }   

    // ok, now we do something.     
 
    s_eo_themais.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_themais.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    s_eo_themais.sharedcan.command.value = &datarate;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.sharedcan.command, s_eo_themais.id32);    
    
    return(eores_OK);  
}



extern eOresult_t eo_mais_SetResolution(EOtheMAIS *p, eOas_maisresolution_t resolution)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themais.service.active)
    {   // nothing to do because we dont have a mais board
        return(eores_OK);
    }    

    // ok, now we do something.     
 
    s_eo_themais.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_themais.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
    s_eo_themais.sharedcan.command.value = &resolution;
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themais.sharedcan.command, s_eo_themais.id32);    
    
    eOas_mais_status_t *status = eo_entities_GetMaisStatus(eo_entities_GetHandle(), 0);
    if(NULL == status)
    {
        return(eores_NOK_generic); //error
    }     

    s_eo_mais_process_mais_resolution(resolution, status);  
    
    return(eores_OK);          
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to MAIS are in a unique place


extern void eoprot_fun_INIT_as_mais_config(const EOnv* nv)
{
    eOas_mais_config_t* maiscfg = eo_nv_RAM(nv);
    
    maiscfg->datarate = 10;
    maiscfg->mode = eoas_maismode_txdatacontinuously;
    maiscfg->resolution = eoas_maisresolution_08;
}


extern void eoprot_fun_INIT_as_mais_status(const EOnv* nv)
{
    eOas_mais_status_t *status = eo_nv_RAM(nv);  
    
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


static eOresult_t s_eo_mais_onstop_search4mais(void *par, EOtheCANdiscovery2* p, eObool_t searchisok)
{    
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    
    if((eobool_true == searchisok) && (eobool_true == s_eo_themais.service.activateafterverify))
    {
        eo_mais_Activate(&s_eo_themais, servcfg);        
    }
    
    s_eo_themais.diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    s_eo_themais.diagnostics.errorDescriptor.sourceaddress    = 0;
    s_eo_themais.diagnostics.errorDescriptor.par16            = (servcfg->data.as.mais.canloc.addr) | (servcfg->data.as.mais.canloc.port << 8);
    s_eo_themais.diagnostics.errorDescriptor.par64            = (servcfg->data.as.mais.version.firmware.minor)       | (servcfg->data.as.mais.version.firmware.major << 8) |
                                                    (servcfg->data.as.mais.version.protocol.minor << 16) | (servcfg->data.as.mais.version.protocol.major << 24);    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_mais_send_periodic_error_report, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
    
    if(eobool_true == searchisok)
    {           
        s_eo_themais.diagnostics.errorType = eo_errortype_debug;
        s_eo_themais.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mais_ok);
        eo_errman_Error(eo_errman_GetHandle(), s_eo_themais.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themais.diagnostics.errorDescriptor);
        
        if((0 != s_eo_themais.diagnostics.repetitionOKcase) && (0 != s_eo_themais.diagnostics.reportPeriod))
        {
            s_eo_themais.diagnostics.errorCallbackCount = s_eo_themais.diagnostics.repetitionOKcase;        
            eo_timer_Start(s_eo_themais.diagnostics.reportTimer, eok_abstimeNOW, s_eo_themais.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }    
    
    if(eobool_false == searchisok)
    {
        s_eo_themais.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mais_failed_candiscovery);
        s_eo_themais.diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), s_eo_themais.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themais.diagnostics.errorDescriptor);
        
        if(0 != s_eo_themais.diagnostics.reportPeriod)
        {
            s_eo_themais.diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(s_eo_themais.diagnostics.reportTimer, eok_abstimeNOW, s_eo_themais.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);  
        }
    }      
    
    if(NULL != s_eo_themais.service.onverify)
    {
        s_eo_themais.service.onverify(&s_eo_themais, searchisok); 
    }    
    
    return(eores_OK);   
}


static void s_eo_mais_send_periodic_error_report(void *p)
{
    eo_errman_Error(eo_errman_GetHandle(), s_eo_themais.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_themais.diagnostics.errorDescriptor);
    
    if(EOK_int08dummy != s_eo_themais.diagnostics.errorCallbackCount)
    {
        s_eo_themais.diagnostics.errorCallbackCount--;
    }
    if(0 == s_eo_themais.diagnostics.errorCallbackCount)
    {
        eo_timer_Stop(s_eo_themais.diagnostics.reportTimer);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



