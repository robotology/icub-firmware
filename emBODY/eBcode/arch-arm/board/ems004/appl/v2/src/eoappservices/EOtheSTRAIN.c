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
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

#include "EoProtocolAS.h"

#include "EOMtheEMSappl.h"

#include "EOtheCANprotocol_functions.h"

#include "EOMtheEMSconfigurator.h"

#include "EOMtheEMStransceiver.h"

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

//static eOresult_t s_eo_thestrain_loadFullscalelikeoccasionalrop(void);

static eOresult_t s_eo_strain_onstop_search4strain(void *par, EOtheCANdiscovery2* p, eObool_t searchisok);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheSTRAIN s_eo_thestrain = 
{
    .initted                = eobool_false,
    .active                 = eobool_false,
    .thereisstrain          = eobool_false,
    .protindex              = 0,
    .id32                   = eo_prot_ID32dummy,
    .command                = {0},
    .canboardproperties     = NULL,
    .canentitydescriptor    = NULL,
    .servconfig             = { .type = eomn_serv_NONE },
    .candiscoverytarget     = {0},
    .onverify               = NULL,
    .activateafterverify    = eobool_false,
    .itistransmitting       = eobool_false,
    .onfullscaleready       = NULL
};

//static const char s_eobj_ownname[] = "EOtheSTRAIN";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheSTRAIN* eo_strain_Initialise(void)
{
    if(eobool_true == s_eo_thestrain.initted)
    {
        return(&s_eo_thestrain);
    }
    
    s_eo_thestrain.active = eobool_false;
    s_eo_thestrain.thereisstrain = eobool_false;
    
    s_eo_thestrain.protindex = 0;
    s_eo_thestrain.id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_thestrain.protindex, eoprot_tag_none);

    s_eo_thestrain.servconfig.type = eomn_serv_NONE;
    
    
    s_eo_thestrain.canboardproperties = eo_vector_New(sizeof(eOcanmap_board_properties_t), 1, NULL, NULL, NULL, NULL);
    
    s_eo_thestrain.canentitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), 1, NULL, NULL, NULL, NULL);
    
    s_eo_thestrain.strain = NULL;
        
    s_eo_thestrain.initted = eobool_true;
    
    return(&s_eo_thestrain);   
}


extern EOtheSTRAIN* eo_strain_GetHandle(void)
{
    if(eobool_true == s_eo_thestrain.initted)
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
//    if(eobool_true == s_eo_thestrain.active)
//    {
//        eo_strain_Deactivate(p);        
//    }   

    s_eo_thestrain.onverify = onverify;
    s_eo_thestrain.activateafterverify = activateafterverify;


    s_eo_thestrain.candiscoverytarget.boardtype = eobrd_cantype_strain;
    s_eo_thestrain.candiscoverytarget.protocolversion.major = servcfg->data.as.strain.versionofstrain.protocol.major; 
    s_eo_thestrain.candiscoverytarget.protocolversion.minor = servcfg->data.as.strain.versionofstrain.protocol.minor;
    s_eo_thestrain.candiscoverytarget.firmwareversion.major = servcfg->data.as.strain.versionofstrain.firmware.major; 
    s_eo_thestrain.candiscoverytarget.firmwareversion.minor = servcfg->data.as.strain.versionofstrain.firmware.minor;    
    s_eo_thestrain.candiscoverytarget.canmap[servcfg->data.as.strain.canloc.port] = 0x0001 << servcfg->data.as.strain.canloc.addr; 
        

    eOcandiscovery_onstop_t onstop = 
    {
        .function   = s_eo_strain_onstop_search4strain,
        .parameter  = (void*)servcfg
    };
        
    eo_candiscovery2_Start(eo_candiscovery2_GetHandle(), &s_eo_thestrain.candiscoverytarget, &onstop);   

    
    return(eores_OK);   
}


extern eOresult_t eo_strain_Deactivate(EOtheSTRAIN *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == s_eo_thestrain.active)
    {
        return(eores_OK);        
    } 
    
    // send stop messages to strain, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object
    // what else .... ?
    
        
    if(eobool_true == s_eo_thestrain.itistransmitting)
    {
        eo_strain_TXstop(&s_eo_thestrain);
    }
    
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_thestrain.canentitydescriptor); 
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), s_eo_thestrain.canboardproperties);
     
    
    eo_entities_SetNumOfStrains(eo_entities_GetHandle(), 0);
    
    s_eo_thestrain.strain = NULL;
    
    memset(&s_eo_thestrain.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    s_eo_thestrain.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(s_eo_thestrain.canboardproperties);
    eo_vector_Clear(s_eo_thestrain.canentitydescriptor);
    
    
    s_eo_thestrain.active = eobool_false;
    
    return(eores_OK);
}


extern eOresult_t eo_strain_Activate(EOtheSTRAIN *p, const eOmn_serv_configuration_t * servcfg)
{
    if((NULL == p) || (NULL == servcfg))
    {
        return(eores_NOK_nullpointer);
    }    
    
    if(eobool_true == s_eo_thestrain.active)
    {
        eo_strain_Deactivate(p);        
    }   
    
    eo_entities_SetNumOfStrains(eo_entities_GetHandle(), 1);
    

    if(0 == eo_entities_NumOfStrains(eo_entities_GetHandle()))
    {
        s_eo_thestrain.thereisstrain = eobool_false;
        s_eo_thestrain.active = eobool_false;
        return(eores_NOK_generic);
    }
    else
    {
        s_eo_thestrain.thereisstrain = eobool_true;
        
        s_eo_thestrain.strain = eo_entities_GetStrain(eo_entities_GetHandle(), s_eo_thestrain.protindex);
        
        memcpy(&s_eo_thestrain.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
            
        
        // now... use the servcfg
        eOcanmap_board_properties_t prop = 
        {
            .type               = eobrd_cantype_strain, 
            .location           = { .port = servcfg->data.as.strain.canloc.port, .addr = servcfg->data.as.strain.canloc.addr, .insideindex = eocanmap_insideindex_none },
            .requiredprotocol   = { .major = servcfg->data.as.strain.versionofstrain.protocol.major, .minor = servcfg->data.as.strain.versionofstrain.protocol.minor }
        };       
        eo_vector_PushBack(s_eo_thestrain.canboardproperties, &prop);
        
        // load the can mapping ... make an UnloadBoards()
        eo_canmap_LoadBoards(eo_canmap_GetHandle(), s_eo_thestrain.canboardproperties); 
        
        // load the entity mapping.
        eOcanmap_entitydescriptor_t des = 
        {
            .location   = { .port = servcfg->data.as.strain.canloc.port, .addr = servcfg->data.as.strain.canloc.addr, .insideindex = eocanmap_insideindex_none },
            .index      = entindex00 // we have only one strain
        };
        eo_vector_PushBack(s_eo_thestrain.canentitydescriptor, &des);
        
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_thestrain.canentitydescriptor);   


        s_eo_thestrain.active = eobool_true;        
    }

    
    return(eores_OK);   
}


extern eOresult_t eo_strain_TXstart(EOtheSTRAIN *p, uint8_t datarate, eOas_strainmode_t mode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.thereisstrain) || (eobool_false == s_eo_thestrain.active))
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }   

    eo_strain_SetDataRate(&s_eo_thestrain, datarate);  
    
    // set txmode
    //eOcanprot_command_t s_eo_thestrain.command = {0};
    s_eo_thestrain.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    s_eo_thestrain.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);
    
    if(eoas_strainmode_acquirebutdonttx == mode)
    {
        s_eo_thestrain.itistransmitting = eobool_false;
    }
    else
    {
        s_eo_thestrain.itistransmitting = eobool_true;
    }

    return(eores_OK);
 
}


extern eOresult_t eo_strain_SendTXmode(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.thereisstrain) || (eobool_false == s_eo_thestrain.active))
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things.     
    eOas_strain_config_t *cfg = &s_eo_thestrain.strain->config;

     
    // set txmode
    s_eo_thestrain.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    s_eo_thestrain.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.command.value = &cfg->mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);
    
    if(eoas_strainmode_acquirebutdonttx == cfg->mode)
    {
        s_eo_thestrain.itistransmitting = eobool_false;
    }
    else
    {
        s_eo_thestrain.itistransmitting = eobool_true;
    }

    return(eores_OK);
}



extern eOresult_t eo_strain_TXstop(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.thereisstrain) || (eobool_false == s_eo_thestrain.active))
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    
    eOenum08_t mode = eoas_strainmode_acquirebutdonttx;       

    s_eo_thestrain.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    s_eo_thestrain.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);
    
    
    s_eo_thestrain.itistransmitting = eobool_false;

    return(eores_OK);      
}


extern eOresult_t eo_strain_Set(EOtheSTRAIN *p, eOas_strain_config_t *cfg, eOstrain_onendofoperation_fun_t onfullscaleready)
{
    if((NULL == p) || (NULL == cfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.thereisstrain) || (eobool_false == s_eo_thestrain.active))
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things.  

    eo_strain_SetMode(p, (eOas_strainmode_t)cfg->mode);
    
    eo_strain_SetDataRate(p, cfg->datarate);
    
    if(eobool_true == cfg->signaloncefullscale)
    {
        eo_strain_GetFullScale(p, onfullscaleready);
    }

    return(eores_OK);
}


extern eOresult_t eo_strain_SetMode(EOtheSTRAIN *p, eOas_strainmode_t mode)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.thereisstrain) || (eobool_false == s_eo_thestrain.active))
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things. 

    eOsmStatesEMSappl_t currentstate = eo_sm_emsappl_STerr;

    //eOcanprot_command_t s_eo_thestrain.command = {0};
    s_eo_thestrain.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_thestrain.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.command.value = &mode;

    
    if(eoas_strainmode_acquirebutdonttx == mode) 
    {
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);
        s_eo_thestrain.itistransmitting = eobool_false;
    }
    else // if it configures strain mode to send data
    {
//        // only if the appl is in RUN state enable tx
//        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
//        if(eo_sm_emsappl_STrun == currentstate)
        {
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);
            s_eo_thestrain.itistransmitting = eobool_true;
        }
    }   

    return(eores_OK); 
}


extern eOresult_t eo_strain_SetDataRate(EOtheSTRAIN *p, uint8_t datarate)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.thereisstrain) || (eobool_false == s_eo_thestrain.active))
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things. 

    //eOcanprot_command_t s_eo_thestrain.command = {0};
    s_eo_thestrain.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_thestrain.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    s_eo_thestrain.command.value = &datarate;
   
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);

    return(eores_OK); 
}


extern eOprotID32_t eo_strain_GetID32(EOtheSTRAIN *p, eOprotTag_t tag)
{
    return(eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_thestrain.protindex, tag));
}


extern eOresult_t eo_strain_GetFullScale(EOtheSTRAIN *p, eOstrain_onendofoperation_fun_t onfullscaleready)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_thestrain.thereisstrain) || (eobool_false == s_eo_thestrain.active))
    {   // nothing to do because we dont have a strain board
        if(NULL != onfullscaleready)
        {
            onfullscaleready(p, eobool_false);
        }
        return(eores_OK);
    }
    
    s_eo_thestrain.onfullscaleready = onfullscaleready;
          
    // now, i do things. 
    s_eo_thestrain_startGetFullScales();

    return(eores_OK); 
}

//extern eOresult_t eo_strain_OnDiscoveryStop(EOtheSTRAIN *p, EOtheCANdiscovery2 *discovery2, eObool_t searchisok)
//{   
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }
//    
//    // wip part. we should: 
//    // 1. send up a ok result about the strain ...
//    // 2. call eo_strain_Initialise()
//    // 3. return OK
//    
//    
//    if(NULL == p)
//    {
//        eo_strain_Initialise();
//    }

//    return(eores_OK); 
//}


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
// VERY IMPORTANT: the function must return eobool_true
extern eObool_t eocanprotASpolling_redefinable_alert_reception_of_POL_AS_CMD__GET_FULL_SCALES(uint8_t channel, uint16_t *value, eOas_strain_t* strain)
{
    const eObool_t ret = eobool_true;
    
    if(channel < 5)
    {   // send a request for next channel
        channel++;
        
        //eOcanprot_command_t s_eo_thestrain.command = {0};
        s_eo_thestrain.command.class = eocanprot_msgclass_pollingAnalogSensor;
        s_eo_thestrain.command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
        s_eo_thestrain.command.value = &channel;
        
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);
        
        return(ret);
    }
    
    // received the last channel: call s_eo_thestrain.onfullscaleready()
    
    if(NULL != s_eo_thestrain.onfullscaleready)
    {
        s_eo_thestrain.onfullscaleready(&s_eo_thestrain, eobool_true);
    }
    
//#if 0
//    // received the last channel: load a rop to tx and then alert someone to tx the ropframe
//        
//    // prepare occasional rop to send
//    eOresult_t res = s_eo_thestrain_loadFullscalelikeoccasionalrop();
//    
//    if(eores_OK != res)
//    {
//        // diagnostics
//        return(res);
//    }
//    
//    eOsmStatesEMSappl_t status;
//    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &status);
//    
//    // if application is in cfg state, then we send a request to configurator to send ropframe out
//    if(eo_sm_emsappl_STcfg == status)
//    {
//        eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_ropframeTx); 
//    }
//#endif
    
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
    //eOcanprot_command_t s_eo_thestrain.command = {0};
    s_eo_thestrain.command.class = eocanprot_msgclass_pollingAnalogSensor;
    s_eo_thestrain.command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
    s_eo_thestrain.command.value = &channel;

    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);                  
}

//static eOresult_t s_eo_thestrain_loadFullscalelikeoccasionalrop(void)
//{
//    eOresult_t res;
//    eOropdescriptor_t ropdesc;

//    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));

//    ropdesc.ropcode                 = eo_ropcode_sig;
//    ropdesc.size                    = sizeof(eOas_arrayofupto12bytes_t);
//    ropdesc.id32                    = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_thestrain.protindex, eoprot_tag_as_strain_status_fullscale); 
//    ropdesc.data                    = NULL;

//   
//    res = eo_transceiver_OccasionalROP_Load(eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()), &ropdesc); 
//    if(eores_OK != res)
//    {
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, "cannot sig<strain.fullscale>", NULL, &eo_errman_DescrRuntimeErrorLocal);
//    }
//    else
//    {
//        //eOerrmanDescriptor_t des = {0};
//        //des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag03);
//        //des.param = 0x1111;
//        //des.sourceaddress = 0;
//        //des.sourcedevice = eo_errman_sourcedevice_localboard;
//        //eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &des);
//    }


//    return(res);
//}

static eOresult_t s_eo_strain_onstop_search4strain(void *par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    
    if((eobool_true == searchisok) && (eobool_true == s_eo_thestrain.activateafterverify))
    {
        const eOmn_serv_configuration_t * strainserv = (const eOmn_serv_configuration_t *)par;
        eo_strain_Activate(&s_eo_thestrain, strainserv);        
    }
    
    if(NULL != s_eo_thestrain.onverify)
    {
        s_eo_thestrain.onverify(&s_eo_thestrain, searchisok); 
    }    
    
    return(eores_OK);
    
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



