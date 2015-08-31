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


#include "EOtheEntities.h"

#include "EoError.h"
#include "EOtheErrorManager.h"

#include "EOtheMAIS.h"
#include "EOtheSTRAIN.h"
#include "EOtheInertial.h"

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
    eo_strain_Config(eo_strain_GetHandle(), cfg);
    
//    uint8_t number = eoprot_ID2index(rd->id32);
//    eOsmStatesEMSappl_t currentstate = eo_sm_emsappl_STerr;
//    eOas_strain_config_t *straincfg = (eOas_strain_config_t*)rd->data;
//    eOcanprot_command_t command = {0};
//    command.class = eocanprot_msgclass_pollingAnalogSensor;
//    

//    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
//    if(eo_sm_emsappl_STrun == currentstate)
//    {
//        command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
//        command.value = &(straincfg->mode);
//        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
//    }
//     
//    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
//    command.value = &(straincfg->datarate);
//    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
//    
//    s_signalGetFullScales(rd->id32, straincfg->signaloncefullscale);          
}

extern void eoprot_fun_UPDT_as_strain_config_mode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_strainmode_t *mode = (eOas_strainmode_t*)rd->data;
    eo_strain_ConfigMode(eo_strain_GetHandle(), *mode);
    
//    uint8_t number = eoprot_ID2index(rd->id32);
//    eOsmStatesEMSappl_t currentstate = eo_sm_emsappl_STerr;
//    eOas_strainmode_t *strainmode = (eOas_strainmode_t*)rd->data;
//    eOcanprot_command_t command = {0};
//    command.class = eocanprot_msgclass_pollingAnalogSensor;
//    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
//    command.value = strainmode;

//    
//    if(eoas_strainmode_acquirebutdonttx == *strainmode) 
//    {
//        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
//    }
//    else //if pc104 configures strain mode to send data
//    {
//        //only if the appl is in RUN state enable mais tx
//        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
//        if(eo_sm_emsappl_STrun == currentstate)
//        {
//            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
//        }
//    }   
    
}

extern void eoprot_fun_UPDT_as_strain_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint8_t *datarate = (uint8_t*)rd->data;
    eo_strain_ConfigDataRate(eo_strain_GetHandle(), *datarate);
    
//    uint8_t *straindatarate = (uint8_t*)rd->data;
//    eOcanprot_command_t command = {0};
//    command.class = eocanprot_msgclass_pollingAnalogSensor;
//    command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
//    command.value = straindatarate;
//    
//    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);   
}


extern void eoprot_fun_UPDT_as_strain_config_signaloncefullscale(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eObool_t *signaloncefullscale = (eObool_t*)rd->data;
    if(eobool_true == *signaloncefullscale)
    {
        eo_strain_FullScale_StartRequest(eo_strain_GetHandle());
    }
    
//    eObool_t *signaloncefullscale = (eObool_t*)rd->data;
//    s_signalGetFullScales(rd->id32, *signaloncefullscale);
}



extern void eoprot_fun_UPDT_as_inertial_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_inertial_config_t *cfg = (eOas_inertial_config_t*)rd->data;    
    eo_inertial_Config2(eo_inertial_GetHandle(), cfg);    
}


extern void eoprot_fun_UPDT_as_inertial_cmmnds_enable(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_inertial_commands_t *cmd = (eOmc_inertial_commands_t*)rd->data;  

    if(0 == cmd->enable)
    {
        eo_inertial_Stop2(eo_inertial_GetHandle());    
    }
    else
    {
        eo_inertial_Start2(eo_inertial_GetHandle());    
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//static void s_process_mais_resolution(eOas_maisresolution_t resolution, eOas_mais_status_t* status)
//{
//    uint8_t size = 0;
//    uint8_t itemsize = 1;
//    uint8_t capacity = 0;
//    
//    if(eoas_maisresolution_08 == resolution)
//    {
//        capacity    = 15;
//        itemsize    = 1;
//        size        = 15;
//        //#warning acemor-> nella mais nel caso di risoluzione a 8 bit perche' la capacity di the15values e' 16 e non 15?
//        // penso sia un errore. metto a 15. anche perche' nel parser dei pacchetti can la size viene messa a 15
//        //status->the15values.head.capacity = 16;
//        //status->the15values.head.itemsize = 1;
//        //status->the15values.head.size = 16;
//    }
//    else if(eoas_maisresolution_16 == resolution)
//    {
//        capacity    = 15;
//        itemsize    = 2;
//        size        = 15;
//        //#warning acemor-> nella mais ho messo la capacity di the15values a 15 anche nel caso di risoluzione a 16 bit
//        //status->the15values.head.capacity = 16;
//        //status->the15values.head.itemsize = 2;
//        //status->the15values.head.size = 16;
//    } 

//    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
//    eo_array_Resize(array, size);    
//}


//static void s_signalGetFullScales(uint32_t id32, eObool_t signaloncefullscale)
//{
//    if(eobool_true == signaloncefullscale)
//    {
//        //clear array in strainstatus
//        //uint8_t number = eoprot_ID2index(id32);        
//        eOas_strain_status_t* status = eo_entities_GetStrainStatus(eo_entities_GetHandle(), eoprot_ID2index(id32));
//        if(NULL != status)
//        {
//            // impose that fullscale is an empty array of itemsize 2 and capacity 6. 
//            // we have already done it inside the eoprot_fun_INIT_as_strain_status() function, but we do it again with eo_array_New()
//            EOarray *array = eo_array_New(6, 2, &status->fullscale);
//            
//            // then we send a command to strain to send us the value of channel 0. 
//            // this operation triggers a new request until we receive all the 6 values
//            uint8_t channel = 0;
//            eOcanprot_command_t command = {0};
//            command.class = eocanprot_msgclass_pollingAnalogSensor;
//            command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
//            command.value = &channel;

//            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
//             
//            //s_send_diagnostics(eo_errortype_debug, eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01), 2);                     
//        }
//    }
//    else
//    {
//        //s_send_diagnostics(eo_errortype_debug, eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01), 4);
//    }
//}


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



