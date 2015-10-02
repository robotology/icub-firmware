/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

/* @file       EoProtocolAS_fun_ems4rd.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint as
    @author     valentina.gaggero@iit.it
    @date       06/11/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"
#include "hal.h"

#include "EoCommon.h"
#include "EOarray.h"

#include "EoAnalogSensors.h"


#include "EOMtheEMSappl.h"

#include "EOtheCANService.h"


#include "EoProtocol.h"

#include "EoProtocolAS.h"


#include "EOtheEntities.h"

#include "EoError.h"
#include "EOtheErrorManager.h"

#include "EOtheMAIS.h"
#include "EOtheSTRAIN.h"
#include "EOtheInertial.h"

#include "EOMtheEMSconfigurator.h"

#include "EOMtheEMStransceiver.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_on_fullscaleofstrain_received(EOtheSTRAIN* p, eObool_t operationisok);
    
//static void s_process_mais_resolution(eOas_maisresolution_t resolution, eOas_mais_status_t* status);
//static void s_signalGetFullScales(uint32_t id32, eObool_t signaloncefullscale);

// useful for any debug activity. comment it out if not used
//static void s_send_diagnostics(eOerrmanErrorType_t type, uint32_t code, uint16_t param, uint64_t par64);


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


//extern void eoprot_fun_INIT_as_mais_config(const EOnv* nv)
//{
//    eOas_mais_config_t* cfg = eo_nv_RAM(nv);
//    
//    cfg->datarate = 10;
//    cfg->mode = eoas_maismode_txdatacontinuously;
//    cfg->resolution = eoas_maisresolution_08;
//}


//extern void eoprot_fun_INIT_as_mais_status(const EOnv* nv)
//{
//    eOas_mais_status_t *status = eo_nv_RAM(nv);  
//    
//    // marco.accame: i init as for eoas_maisresolution_08 
//    //               the array the15values can be initted for size 0 or 15 as i now use teh proper eo_array_Assign() method
//    uint8_t capacity    = 15;
//    uint8_t itemsize    = 1;
//    uint8_t size        = 15;
//    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
//    eo_array_Resize(array, size);
//}



extern void eoprot_fun_UPDT_as_mais_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_mais_config_t *cfg = (eOas_mais_config_t*)rd->data;    
    eo_mais_Config(eo_mais_GetHandle(), cfg);
    
//    uint8_t number = eoprot_ID2index(rd->id32);
//    eOsmStatesEMSappl_t currentstate = eo_sm_emsappl_STerr;    
//    eOcanprot_command_t command = {0};
//    command.class = eocanprot_msgclass_pollingAnalogSensor;
//    
//    //if pc104 tell me to enable maistx, before to send cmd verify if i'm in RUN state:
//    // if yes ==> ok no problem
//    // if no ==> i'll send cmd when go to RUN state
//    if(eoas_maismode_txdatacontinuously == cfg->mode)
//    {
//        // only if the appl is in RUN state we enable mais tx
//        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
//        if(eo_sm_emsappl_STrun == currentstate)
//        {
//            command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
//            command.value = &(cfg->mode);
//            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
//        }
//    }    
//    
//    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
//    command.value = &(cfg->datarate);
//    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);

//    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
//    command.value = &(cfg->resolution);
//    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);    
//    
//    
//    eOas_mais_status_t *status = eo_entities_GetMaisStatus(eo_entities_GetHandle(), number);
//    if(NULL == status)
//    {
//        return; //error
//    }     

//    s_process_mais_resolution((eOas_maisresolution_t)cfg->resolution, status);
}


extern void eoprot_fun_UPDT_as_mais_config_mode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_maismode_t *maismode = (eOas_maismode_t*)rd->data;
    eo_mais_ConfigMode(eo_mais_GetHandle(), *maismode);
    
//    //uint8_t number = eoprot_ID2index(rd->id32);
//    eOas_maismode_t *maismode = (eOas_maismode_t*)rd->data;
//    eOsmStatesEMSappl_t currentstate = eo_sm_emsappl_STerr;
//    eOcanprot_command_t command = {0};
//    command.class = eocanprot_msgclass_pollingAnalogSensor;
//    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
//    command.value = maismode;
//    
//    //if pc104 tell me to enable maistx, before to send cmd verify if i'm in RUN state:
//    // if yes ==> ok no problem
//    // if no ==> i'll send cmd when go to RUN state
//    if(eoas_maismode_txdatacontinuously == *maismode)
//    {
//        //only if the appl is in RUN state enable mais tx
//        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
//        if(eo_sm_emsappl_STrun == currentstate)
//        {
//            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
//        }
//    }
//    else
//    {
//         eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
//    }
}


extern void eoprot_fun_UPDT_as_mais_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint8_t *datarate = (uint8_t*)rd->data;
    eo_mais_ConfigDataRate(eo_mais_GetHandle(), *datarate);
    
//    eOcanprot_command_t command = {0};
//    command.class = eocanprot_msgclass_pollingAnalogSensor;
//    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
//    command.value = datarate;
//    
//    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);    
}


extern void eoprot_fun_UPDT_as_mais_config_resolution(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_maisresolution_t *resolution = (eOas_maisresolution_t*)rd->data;    
    eo_mais_ConfigResolution(eo_mais_GetHandle(), *resolution);
    
//    uint8_t number = eoprot_ID2index(rd->id32);
//    eOcanprot_command_t command = {0};
//    command.class = eocanprot_msgclass_pollingAnalogSensor;
//    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
//    command.value = resolution;
//    
//    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);    
//    
//    eOas_mais_status_t *status = eo_entities_GetMaisStatus(eo_entities_GetHandle(), number);
//    if(NULL == status)
//    {
//        return; //error
//    }     

//    s_process_mais_resolution(*resolution, status);   
}


//extern void eoprot_fun_INIT_as_strain_config(const EOnv* nv)
//{
//    eOas_strain_config_t* straincfg = eo_nv_RAM(nv);
//    
//    straincfg->datarate = 10;
//    straincfg->mode = eoas_strainmode_acquirebutdonttx;
//    straincfg->signaloncefullscale = eobool_false;
//}


//extern void eoprot_fun_INIT_as_strain_status(const EOnv* nv)
//{
//    eOas_strain_status_t *status = eo_nv_RAM(nv);
//    uint8_t capacity = 0;
//    uint8_t itemsize = 0;
//    uint8_t size = 0;
//    EOarray* array = NULL;
//    
//    // marco.accame: cannot understand why in here size is zero and in others it was not zero. see why. 
//    //               probably fullscale, being of itemsize = 2 is treated without the proper eo_array_ methods
//    capacity    = 6;
//    itemsize    = 2;
//    size        = 0; 
//    array = eo_array_New(capacity, itemsize, &status->fullscale);
//    eo_array_Resize(array, size);


//    // marco.accame: size can be 0 or 6. now i use teh proper eo_array_Assign method for calibratedvalues/uncalibratedvalues 
//    //
//    capacity    = 6;
//    itemsize    = 2;
//    size        = 6;
//    array = eo_array_New(capacity, itemsize, &status->calibratedvalues);
//    eo_array_Resize(array, size);   

//    capacity    = 6;
//    itemsize    = 2;
//    size        = 6;
//    array = eo_array_New(capacity, itemsize, &status->uncalibratedvalues);
//    eo_array_Resize(array, size); 
//}




extern void eoprot_fun_UPDT_as_strain_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_strain_config_t *cfg = (eOas_strain_config_t*)rd->data;
    eo_strain_Set(eo_strain_GetHandle(), cfg, s_on_fullscaleofstrain_received);       
}

extern void eoprot_fun_UPDT_as_strain_config_mode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_strainmode_t *mode = (eOas_strainmode_t*)rd->data;
    eo_strain_SetMode(eo_strain_GetHandle(), *mode);
    
}

extern void eoprot_fun_UPDT_as_strain_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint8_t *datarate = (uint8_t*)rd->data;
    eo_strain_SetDataRate(eo_strain_GetHandle(), *datarate);  
}


extern void eoprot_fun_UPDT_as_strain_config_signaloncefullscale(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eObool_t *signaloncefullscale = (eObool_t*)rd->data;
    if(eobool_true == *signaloncefullscale)
    {
        eo_strain_GetFullScale(eo_strain_GetHandle(), s_on_fullscaleofstrain_received);
    }
}

extern void eoprot_fun_UPDT_as_inertial_config_service(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_inertial_serviceconfig_t *cfg = (eOas_inertial_serviceconfig_t*)rd->data;    
    eo_inertial_ServiceConfig(eo_inertial_GetHandle(), cfg);    
}


extern void eoprot_fun_UPDT_as_inertial_config_sensors(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_inertial_sensorsconfig_t *cfg = (eOas_inertial_sensorsconfig_t*)rd->data;    
    eo_inertial_SensorsConfig(eo_inertial_GetHandle(), cfg);    
}


extern void eoprot_fun_UPDT_as_inertial_cmmnds_enable(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_inertial_commands_t *cmd = (eOmc_inertial_commands_t*)rd->data;  

    if(0 == cmd->enable)
    {
        eo_inertial_Stop(eo_inertial_GetHandle());    
    }
    else
    {
        eo_inertial_Start(eo_inertial_GetHandle());    
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//static eOresult_t s_eo_thestrain_loadFullscalelikeoccasionalrop(void)
//{
//    eOresult_t res;
//    eOropdescriptor_t ropdesc;

//    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));

//    ropdesc.ropcode                 = eo_ropcode_sig;
//    ropdesc.size                    = sizeof(eOas_arrayofupto12bytes_t);
//    ropdesc.id32                    = eo_strain_GetID32(eo_strain_GetHandle(), eoprot_tag_as_strain_status_fullscale); 
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


static eOresult_t s_on_fullscaleofstrain_received(EOtheSTRAIN* p, eObool_t operationisok)
{
    eOresult_t res = eores_OK;
    
    if(eobool_false == operationisok)
    {
        // send diagnostics
        return(eores_NOK_generic);
    }
    
    // signal the fullscale only once.
    
    // load a rop to tx and then alert someone to tx the ropframe
        
    // 1. prepare occasional rop to send    
    
    eOropdescriptor_t ropdesc = {0};

    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));

    ropdesc.ropcode                 = eo_ropcode_sig;
    ropdesc.size                    = sizeof(eOas_arrayofupto12bytes_t);
    ropdesc.id32                    = eo_strain_GetID32(eo_strain_GetHandle(), eoprot_tag_as_strain_status_fullscale); 
    ropdesc.data                    = NULL;

   
    res = eo_transceiver_OccasionalROP_Load(eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()), &ropdesc); 
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
    

    // 2. now i alert someone to transmit it
    
    eOsmStatesEMSappl_t status;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &status);
    
    // if application is in cfg state, then we send a request to configurator to send ropframe out. otherwise, if in RUN mode, the control-loop wills end it 
    if(eo_sm_emsappl_STcfg == status)
    {
        eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_ropframeTx); 
    }
    
    return(eores_OK);
}


// useful for any debug activity. comment it out if not used
//static void s_send_diagnostics(eOerrmanErrorType_t type, uint32_t code, uint16_t param, uint64_t par64)
//{
//    eOerrmanDescriptor_t des = {0};
//    des.code = code;
//    des.param = param;
//    des.par64 = par64; 
//    des.sourceaddress = 0;
//    des.sourcedevice = eo_errman_sourcedevice_localboard;

//    eo_errman_Error(eo_errman_GetHandle(), type, NULL, NULL, &des);
//}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



