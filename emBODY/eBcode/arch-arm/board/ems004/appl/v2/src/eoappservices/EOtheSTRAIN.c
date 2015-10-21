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

static void s_eo_thestrain_startGetFullScales(void);

static eOresult_t s_eo_strain_onstop_search4strain(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);

static void s_eo_strain_send_periodic_error_report(void *p);

static eOresult_t s_eo_thestrain_on_fullscale_ready(EOtheSTRAIN* p, eObool_t operationisok);

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
        .boardproperties     = NULL,
        .entitydescriptor    = NULL,
        .discoverytarget     = {0},
        .ondiscoverystop        = {0},
        .command                = {0}, 
    },
    .id32                       = eo_prot_ID32dummy,   
    .overrideonfullscaleready   = NULL,
    .strain                     = NULL
};

static const char s_eobj_ownname[] = "EOtheSTRAIN";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheSTRAIN* eo_strain_Initialise(void)
{
    if(eobool_true == s_eo_thestrain.service.initted)
    {
        return(&s_eo_thestrain);
    }
    
    s_eo_thestrain.service.active = eobool_false;
    
    s_eo_thestrain.id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, 0, eoprot_tag_none);

    s_eo_thestrain.service.servconfig.type = eomn_serv_NONE;
    
    
    s_eo_thestrain.sharedcan.boardproperties = eo_vector_New(sizeof(eOcanmap_board_properties_t), 1, NULL, NULL, NULL, NULL);
    
    s_eo_thestrain.sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), 1, NULL, NULL, NULL, NULL);
    
    s_eo_thestrain.strain = NULL;
    
    s_eo_thestrain.overrideonfullscaleready = NULL;
    
    s_eo_thestrain.diagnostics.reportTimer = eo_timer_New();
        
    s_eo_thestrain.service.initted = eobool_true;
    
    return(&s_eo_thestrain);   
}


extern EOtheSTRAIN* eo_strain_GetHandle(void)
{
    if(eobool_true == s_eo_thestrain.service.initted)
    {
        return(&s_eo_thestrain);
    }
    
    return(NULL);
}


extern eOresult_t eo_strain_Verify(EOtheSTRAIN *p, const eOmn_serv_configuration_t * servcfg, eOstrain_onendofoperation_fun_t onverify, eObool_t activateafterverify)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }  
 
// DONT Deactivate ... we may want just to check again ....    
//    if(eobool_true == s_eo_thestrain.service.active)
//    {
//        eo_strain_Deactivate(p);        
//    }   

    s_eo_thestrain.service.onverify = onverify;
    s_eo_thestrain.service.activateafterverify = activateafterverify;


    s_eo_thestrain.sharedcan.discoverytarget.boardtype = eobrd_cantype_strain;
    s_eo_thestrain.sharedcan.discoverytarget.protocolversion.major = servcfg->data.as.strain.version.protocol.major; 
    s_eo_thestrain.sharedcan.discoverytarget.protocolversion.minor = servcfg->data.as.strain.version.protocol.minor;
    s_eo_thestrain.sharedcan.discoverytarget.firmwareversion.major = servcfg->data.as.strain.version.firmware.major; 
    s_eo_thestrain.sharedcan.discoverytarget.firmwareversion.minor = servcfg->data.as.strain.version.firmware.minor;    
    s_eo_thestrain.sharedcan.discoverytarget.canmap[servcfg->data.as.strain.canloc.port] = 0x0001 << servcfg->data.as.strain.canloc.addr; 
    
    s_eo_thestrain.sharedcan.ondiscoverystop.function = s_eo_strain_onstop_search4strain;
    s_eo_thestrain.sharedcan.ondiscoverystop.parameter = (void*)servcfg;
    
    // make sure the timer is not running
    eo_timer_Stop(s_eo_thestrain.diagnostics.reportTimer);    

    // start discovery
    eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &s_eo_thestrain.sharedcan.discoverytarget, &s_eo_thestrain.sharedcan.ondiscoverystop);   

    
    return(eores_OK);   
}


extern eOresult_t eo_strain_Deactivate(EOtheSTRAIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == s_eo_thestrain.service.active)
    {
        return(eores_OK);        
    } 
    
    // send stop messages to strain, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object

    if(eobool_true == s_eo_thestrain.service.running)
    {
        eo_strain_TXstop(&s_eo_thestrain);
    }
    
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_thestrain.sharedcan.entitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), s_eo_thestrain.sharedcan.boardproperties);
     
    
    eo_entities_SetNumOfStrains(eo_entities_GetHandle(), 0);
    
    s_eo_thestrain.strain = NULL;
    
    memset(&s_eo_thestrain.service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    s_eo_thestrain.service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(s_eo_thestrain.sharedcan.boardproperties);
    eo_vector_Clear(s_eo_thestrain.sharedcan.entitydescriptor);
    
    // make sure the timer is not running
    eo_timer_Stop(s_eo_thestrain.diagnostics.reportTimer);    
    
    s_eo_thestrain.service.active = eobool_false;
    
    return(eores_OK);
}


extern eOresult_t eo_strain_Activate(EOtheSTRAIN *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_true == s_eo_thestrain.service.active)
    {
        eo_strain_Deactivate(p);        
    }   
    
    eo_entities_SetNumOfStrains(eo_entities_GetHandle(), 1);
    

    if(0 == eo_entities_NumOfStrains(eo_entities_GetHandle()))
    {
        s_eo_thestrain.service.active = eobool_false;
        return(eores_NOK_generic);
    }
    else
    {   
        s_eo_thestrain.strain = eo_entities_GetStrain(eo_entities_GetHandle(), 0);
        
        memcpy(&s_eo_thestrain.service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
            
        
        // now... use the servcfg
        eOcanmap_board_properties_t prop = 
        {
            .type               = eobrd_cantype_strain, 
            .location           = { .port = servcfg->data.as.strain.canloc.port, .addr = servcfg->data.as.strain.canloc.addr, .insideindex = eocanmap_insideindex_none },
            .requiredprotocol   = { .major = servcfg->data.as.strain.version.protocol.major, .minor = servcfg->data.as.strain.version.protocol.minor }
        };       
        eo_vector_PushBack(s_eo_thestrain.sharedcan.boardproperties, &prop);
        
        // load the can mapping ... make an UnloadBoards()
        eo_canmap_LoadBoards(eo_canmap_GetHandle(), s_eo_thestrain.sharedcan.boardproperties); 
        
        // load the entity mapping.
        eOcanmap_entitydescriptor_t des = 
        {
            .location   = { .port = servcfg->data.as.strain.canloc.port, .addr = servcfg->data.as.strain.canloc.addr, .insideindex = eocanmap_insideindex_none },
            .index      = entindex00 // we have only one strain
        };
        eo_vector_PushBack(s_eo_thestrain.sharedcan.entitydescriptor, &des);
        
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_thestrain.sharedcan.entitydescriptor);   


        s_eo_thestrain.service.active = eobool_true;        
    }

    
    return(eores_OK);   
}


extern eOresult_t eo_strain_TXstart(EOtheSTRAIN *p, uint8_t datarate, eOas_strainmode_t mode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.service.active))
    {   // nothing to do because we dont have an service.active strain board
        return(eores_OK);
    }   

    eo_strain_SetDataRate(&s_eo_thestrain, datarate);  
    
    // set txmode
    s_eo_thestrain.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    s_eo_thestrain.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.sharedcan.command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.sharedcan.command, s_eo_thestrain.id32);
    
    if(eoas_strainmode_acquirebutdonttx == mode)
    {
        s_eo_thestrain.service.running = eobool_false;
    }
    else
    {
        s_eo_thestrain.service.running = eobool_true;
    }

    return(eores_OK);
 
}


extern eOresult_t eo_strain_SendTXmode(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.service.active))
    {   // nothing to do because we dont have an service.active strain board
        return(eores_OK);
    }
          
    // now, i do things.     
    eOas_strain_config_t *cfg = &s_eo_thestrain.strain->config;

     
    // set txmode
    s_eo_thestrain.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    s_eo_thestrain.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.sharedcan.command.value = &cfg->mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.sharedcan.command, s_eo_thestrain.id32);
    
    if(eoas_strainmode_acquirebutdonttx == cfg->mode)
    {
        s_eo_thestrain.service.running = eobool_false;
    }
    else
    {
        s_eo_thestrain.service.running = eobool_true;
    }

    return(eores_OK);
}



extern eOresult_t eo_strain_TXstop(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.service.active))
    {   // nothing to do because we dont have an service.active strain board
        return(eores_OK);
    }
          
    
    eOenum08_t mode = eoas_strainmode_acquirebutdonttx;       

    s_eo_thestrain.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    s_eo_thestrain.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.sharedcan.command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.sharedcan.command, s_eo_thestrain.id32);
    
    
    s_eo_thestrain.service.running = eobool_false;

    return(eores_OK);      
}


extern eOresult_t eo_strain_Set(EOtheSTRAIN *p, eOas_strain_config_t *cfg)
{
    if((NULL == p) || (NULL == cfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.service.active))
    {   // nothing to do because we dont have an service.active strain board
        return(eores_OK);
    }
          
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
    
    if((eobool_false == s_eo_thestrain.service.active))
    {   // nothing to do because we dont have an service.active strain board
        return(eores_OK);
    }
          
    // now, i do things. 

    s_eo_thestrain.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_thestrain.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.sharedcan.command.value = &mode;

    
    if(eoas_strainmode_acquirebutdonttx == mode) 
    {
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.sharedcan.command, s_eo_thestrain.id32);
        s_eo_thestrain.service.running = eobool_false;
    }
    else // if it configures strain mode to send data
    {
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.sharedcan.command, s_eo_thestrain.id32);
        s_eo_thestrain.service.running = eobool_true;
    }   

    return(eores_OK); 
}


extern eOresult_t eo_strain_SetDataRate(EOtheSTRAIN *p, uint8_t datarate)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.service.active))
    {   // nothing to do because we dont have an service.active strain board
        return(eores_OK);
    }
          
    // now, i do things. 

    s_eo_thestrain.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_thestrain.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    s_eo_thestrain.sharedcan.command.value = &datarate;
   
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.sharedcan.command, s_eo_thestrain.id32);

    return(eores_OK); 
}



extern eOresult_t eo_strain_GetFullScale(EOtheSTRAIN *p, eOstrain_onendofoperation_fun_t overrideonfullscaleready)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
  
    if((eobool_false == s_eo_thestrain.service.active))
    {   // nothing to do because we dont have an service.active strain board
        if(NULL != overrideonfullscaleready)
        {
            overrideonfullscaleready(p, eobool_false);
        }
        else
        {
            s_eo_thestrain_on_fullscale_ready(p, eobool_false);
        }
        return(eores_OK);
    }
    
    s_eo_thestrain.overrideonfullscaleready = overrideonfullscaleready;
    
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
    
    if(channel < 5)
    {   // send a request for next channel
        channel++;
        
        s_eo_thestrain.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
        s_eo_thestrain.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
        s_eo_thestrain.sharedcan.command.value = &channel;
        
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.sharedcan.command, s_eo_thestrain.id32);
        
        return(ret);
    }
    
    // received the last channel: call s_eo_thestrain_on_fullscale_ready() or s_eo_thestrain.overrideonfullscaleready()
    
    if(NULL != s_eo_thestrain.overrideonfullscaleready)
    {
        s_eo_thestrain.overrideonfullscaleready(&s_eo_thestrain, eobool_true);
    }
    else
    {
          s_eo_thestrain_on_fullscale_ready(&s_eo_thestrain, eobool_true);
    }
    
    
    return(ret);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_thestrain_startGetFullScales(void)
{
    if(NULL == s_eo_thestrain.strain)
    {
        return;
    }


    eOas_strain_status_t* status = &s_eo_thestrain.strain->status;

    // MUST impose that fullscale is an empty array of itemsize 2 and capacity 6. 
    // we have already done it inside the eoprot_fun_INIT_as_strain_status() function, but we do it again with eo_array_New()
    EOarray *array = eo_array_New(6, 2, &status->fullscale);
    
    // then we send a s_eo_thestrain.command to strain can board to send us the value of channel 0. 
    // when a the value of channel 0 is received by the can parser, we put the value inside the array at position 0
    // and we trigger a request for channel 1. then we repeat until we receive all the 6 values.
    // at the end of that, the full scale is signalled to  robotInterface
    
    uint8_t channel = 0;
    s_eo_thestrain.sharedcan.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_thestrain.sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
    s_eo_thestrain.sharedcan.command.value = &channel;

    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.sharedcan.command, s_eo_thestrain.id32);                  
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



static eOresult_t s_eo_strain_onstop_search4strain(void *par, EOtheCANdiscovery2* p, eObool_t searchisok)
{   
    const eOmn_serv_configuration_t * servcfg = (const eOmn_serv_configuration_t *)par;
    
    if((eobool_true == searchisok) && (eobool_true == s_eo_thestrain.service.activateafterverify))
    {
        eo_strain_Activate(&s_eo_thestrain, servcfg);        
    }
    
    s_eo_thestrain.diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    s_eo_thestrain.diagnostics.errorDescriptor.sourceaddress    = 0;
    s_eo_thestrain.diagnostics.errorDescriptor.par16            = (servcfg->data.as.strain.canloc.addr) | (servcfg->data.as.strain.canloc.port << 8);
    s_eo_thestrain.diagnostics.errorDescriptor.par64            = (servcfg->data.as.strain.version.firmware.minor)       | (servcfg->data.as.strain.version.firmware.major << 8) |
                                                      (servcfg->data.as.strain.version.protocol.minor << 16) | (servcfg->data.as.strain.version.protocol.major << 24);    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_eo_strain_send_periodic_error_report, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        
    if(eobool_true == searchisok)
    {        
        s_eo_thestrain.diagnostics.errorType = eo_errortype_debug;
        s_eo_thestrain.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_strain_ok);
        eo_errman_Error(eo_errman_GetHandle(), s_eo_thestrain.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_thestrain.diagnostics.errorDescriptor);
        
        if((0 != s_eo_thestrain.diagnostics.repetitionOKcase) && (0 != s_eo_thestrain.diagnostics.reportPeriod))
        {
            s_eo_thestrain.diagnostics.errorCallbackCount = s_eo_thestrain.diagnostics.repetitionOKcase;        
            eo_timer_Start(s_eo_thestrain.diagnostics.reportTimer, eok_abstimeNOW, s_eo_thestrain.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }
    
    if(eobool_false == searchisok)
    {   
        s_eo_thestrain.diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_strain_failed_candiscovery);
        s_eo_thestrain.diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), s_eo_thestrain.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_thestrain.diagnostics.errorDescriptor);
        
        if(0 != s_eo_thestrain.diagnostics.reportPeriod)
        {
            s_eo_thestrain.diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(s_eo_thestrain.diagnostics.reportTimer, eok_abstimeNOW, s_eo_thestrain.diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }    
    
    if(NULL != s_eo_thestrain.service.onverify)
    {
        s_eo_thestrain.service.onverify(&s_eo_thestrain, searchisok); 
    }    
    
    return(eores_OK);   
}


static void s_eo_strain_send_periodic_error_report(void *p)
{
    eo_errman_Error(eo_errman_GetHandle(), s_eo_thestrain.diagnostics.errorType, NULL, s_eobj_ownname, &s_eo_thestrain.diagnostics.errorDescriptor);
    
    if(EOK_int08dummy != s_eo_thestrain.diagnostics.errorCallbackCount)
    {
        s_eo_thestrain.diagnostics.errorCallbackCount--;
    }
    if(0 == s_eo_thestrain.diagnostics.errorCallbackCount)
    {
        eo_timer_Stop(s_eo_thestrain.diagnostics.reportTimer);
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



