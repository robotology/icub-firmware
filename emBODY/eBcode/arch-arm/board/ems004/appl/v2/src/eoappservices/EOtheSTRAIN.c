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

static void s_eo_thestrain_startGetFullScales(uint32_t id32, eObool_t signaloncefullscale);

static eOresult_t s_eo_thestrain_loadFullscalelikeoccasionalrop(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheSTRAIN s_eo_thestrain = 
{
    .initted            = eobool_false,
    .thereisstrain      = eobool_false,
    .protindex          = 0,
    .id32               = eo_prot_ID32dummy,
    .command            = {0}    
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
    

    if(0 == eo_entities_NumOfStrains(eo_entities_GetHandle()))
    {
        s_eo_thestrain.thereisstrain = eobool_false;
    }
    else
    {
        s_eo_thestrain.thereisstrain = eobool_true;
        s_eo_thestrain.protindex = 0;
        s_eo_thestrain.id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_thestrain.protindex, eoprot_tag_none);

    }

    s_eo_thestrain.initted = eobool_true;
    
    return(&s_eo_thestrain);   
}


extern EOtheSTRAIN* eo_strain_GetHandle(void)
{
    return(eo_strain_Initialise());
}


extern eOresult_t eo_strain_SendTXmode(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_thestrain.thereisstrain)
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things. 
    
    eOas_strain_config_t *cfg = eo_entities_GetStrainConfig(eo_entities_GetHandle(), s_eo_thestrain.protindex);

    if(NULL == cfg)
    {   // we dont have strain
        return(eores_OK);
    }
     
    // set txmode
    //eOcanprot_command_t s_eo_thestrain.command = {0};
    s_eo_thestrain.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    s_eo_thestrain.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.command.value = &cfg->mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);

    return(eores_OK);
}

extern eOresult_t eo_strain_DisableTX(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_thestrain.thereisstrain)
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things. 
    
    eOas_strain_config_t *cfg = eo_entities_GetStrainConfig(eo_entities_GetHandle(), s_eo_thestrain.protindex);

    if(NULL == cfg)
    {   // we dont have strain
        return(eores_OK);
    }

    eOenum08_t mode = eoas_strainmode_acquirebutdonttx;       
    // set txmode
    //eOcanprot_command_t s_eo_thestrain.command = {0};
    s_eo_thestrain.command.class = eocanprot_msgclass_pollingAnalogSensor;    
    s_eo_thestrain.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    s_eo_thestrain.command.value = &mode;                       
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);

    return(eores_OK);      

}

extern eOresult_t eo_strain_Config(EOtheSTRAIN *p, eOas_strain_config_t *cfg)
{
    if((NULL == p) || (NULL == cfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_thestrain.thereisstrain)
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things.  

    eo_strain_ConfigMode(p, (eOas_strainmode_t)cfg->mode);
    
    eo_strain_ConfigDataRate(p, cfg->datarate);
    
    if(eobool_true == cfg->signaloncefullscale)
    {
        eo_strain_FullScale_StartRequest(p);
    }

    return(eores_OK);
}


extern eOresult_t eo_strain_ConfigMode(EOtheSTRAIN *p, eOas_strainmode_t mode)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_thestrain.thereisstrain)
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
    }
    else // if it configures strain mode to send data
    {
        // only if the appl is in RUN state enable tx
        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
        if(eo_sm_emsappl_STrun == currentstate)
        {
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, s_eo_thestrain.id32);
        }
    }   

    return(eores_OK); 
}


extern eOresult_t eo_strain_ConfigDataRate(EOtheSTRAIN *p, uint8_t datarate)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_thestrain.thereisstrain)
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


extern eOresult_t eo_strain_FullScale_StartRequest(EOtheSTRAIN *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_thestrain.thereisstrain)
    {   // nothing to do because we dont have a strain board
        return(eores_OK);
    }
          
    // now, i do things. 
    s_eo_thestrain_startGetFullScales(s_eo_thestrain.id32, eobool_true);

    return(eores_OK); 
}

extern eOresult_t eo_strain_OnDiscoveryStop(EOtheSTRAIN *p, EOtheCANdiscovery2 *discovery2, eObool_t searchisok)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    // wip part. we should: 
    // 1. send up a ok result about the strain ...
    // 2. call eo_strain_Initialise()
    // 3. return OK
    
    
    if(NULL == p)
    {
        eo_strain_Initialise();
    }

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

    // received the last channel: load a rop to tx and then alert someone to tx the ropframe
        
    // prepare occasional rop to send
    eOresult_t res = s_eo_thestrain_loadFullscalelikeoccasionalrop();
    
    if(eores_OK != res)
    {
        // diagnostics
        return(res);
    }
    
    eOsmStatesEMSappl_t status;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &status);
    
    // if application is in cfg state, then we send a request to configurator to send ropframe out
    if(eo_sm_emsappl_STcfg == status)
    {
        eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_ropframeTx); 
    }

    return(ret);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_thestrain_startGetFullScales(uint32_t id32, eObool_t signaloncefullscale)
{
    if(eobool_true == signaloncefullscale)
    {
        //clear array in strainstatus   
        eOas_strain_status_t* status = eo_entities_GetStrainStatus(eo_entities_GetHandle(), eoprot_ID2index(id32));
        if(NULL != status)
        {
            // impose that fullscale is an empty array of itemsize 2 and capacity 6. 
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

            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_thestrain.command, id32);                  
        }
    }
    else
    {
        //s_send_diagnostics(eo_errortype_debug, eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01), 4);
    }
}

static eOresult_t s_eo_thestrain_loadFullscalelikeoccasionalrop(void)
{
    eOresult_t res;
    eOropdescriptor_t ropdesc;

    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));

    ropdesc.ropcode                 = eo_ropcode_sig;
    ropdesc.size                    = sizeof(eOas_arrayofupto12bytes_t);
    ropdesc.id32                    = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, s_eo_thestrain.protindex, eoprot_tag_as_strain_status_fullscale); 
    ropdesc.data                    = NULL;

   
    res = eo_transceiver_OccasionalROP_Load(eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()), &ropdesc); 
    if(eores_OK != res)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, "cannot sig<strain.fullscale>", NULL, &eo_errman_DescrRuntimeErrorLocal);
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


    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



