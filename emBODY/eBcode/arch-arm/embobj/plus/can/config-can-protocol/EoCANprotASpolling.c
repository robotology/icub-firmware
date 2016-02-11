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
#include "EoCommon.h"

// in here is whatever is required to offer parsing of can frames and copying into protocol data structures.

#include "EoProtocol.h"
#include "EoProtocolAS.h"

#include "EOtheCANmapping.h"
#include "EOMtheEMSappl.h"


#include "EOtheCANservice.h"
#include "EOtheCANdiscovery2.h"

#include "EOMtheEMStransceiver.h"

#include "EOtheErrorManager.h"

#include "EoError.h"

// but also to retrieve information of other things ...

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANprotocol_functions.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_former_POL_AS_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type);

static void* s_eocanprotASpolling_get_entity(eOprotEndpoint_t endpoint, eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, uint8_t *index);


//static void s_eocanprotASpolling_getfullscale_nextstep(uint8_t channel, eOas_strain_t *strain, uint8_t index);
//static eOresult_t s_loadFullscalelikeoccasionalrop(eOas_strainId_t sId);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__SET_TXMODE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 2, ICUBCANPROTO_POL_AS_CMD__SET_TXMODE);    
    eOenum08_t *txmode = (eOenum08_t *)descriptor->cmd.value;    
    frame->data[1] = *txmode;  
    return(eores_OK);    
}

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__SET_CANDATARATE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 2, ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE);    
    uint8_t *datarate = (uint8_t*)descriptor->cmd.value;    
    frame->data[1] = *datarate;  
    return(eores_OK);       
}

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__SET_RESOLUTION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 2, ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION);       
    eOas_maisresolution_t *maisresolution = (eOas_maisresolution_t*)descriptor->cmd.value;    
    eOresult_t res = eores_OK;
    
    switch(*maisresolution)
    {
        case eoas_maisresolution_08:
        {
            frame->data[1] = 2;
        } break;
        
        case eoas_maisresolution_16:
        {
            frame->data[1] = 0;
        } break;

        case eoas_maisresolution_debug:
        {
            frame->data[1] = 1;
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
    }    
    
    return(res);       
}

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__GET_FULL_SCALES(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 2, ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES);    
    uint8_t *channel = (uint8_t*)descriptor->cmd.value;    
    frame->data[1] = *channel;  
    return(eores_OK);          
}


__weak extern eObool_t eocanprotASpolling_redefinable_alert_reception_of_POL_AS_CMD__GET_FULL_SCALES(uint8_t channel,  uint16_t *data, eOas_strain_t* strain)
{
    // if anybody wants to redefine this, it must make return eobool_true, so the caller knows it has been redefined
    return(eobool_false);
}

// this funtion receives the full scale of a channel, stores it into the strain entity and then 
// it calls fucntion eocanprotASpolling_redefinable_alert_reception_of_POL_AS_CMD__GET_FULL_SCALES()
// the default eocanprotASpolling_redefinable_alert_reception_of_POL_AS_CMD__GET_FULL_SCALES() does nothing.
// the procedure of asking the next full scale until completion must be done inside the redefined function.
extern eOresult_t eocanprotASpolling_parser_POL_AS_CMD__GET_FULL_SCALES(eOcanframe_t *frame, eOcanport_t port)
{
    // get the strain
    eOas_strain_t *strain = NULL;
    eOprotIndex_t index = EOK_uint08dummy;
    
    if(NULL == (strain = s_eocanprotASpolling_get_entity(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, frame, port, &index)))
    {
        return(eores_OK);  
    }      
    
    // now i get the channel and the data to be put inside the strain.
    uint8_t channel = frame->data[1];
#if defined(USE_OLD_BUGGY_MODE_TO_SEND_UP_FULLSCALE_WITH_INVERTED_BYTES)    
    uint16_t *data = (uint16_t *)&(frame->data[2]);
#else
    // MSB is in data[2], LSB in data[3]
    uint16_t value = (uint16_t)(frame->data[3]) + (uint16_t)(frame->data[2] << 8);
    uint16_t *data = &value;
#endif    
    // i put data into the array at the specified location. the check vs channel being lower than capacity 6 of array is done inside
    EOarray* array = (EOarray*)&strain->status.fullscale;
    eo_array_Assign(array, channel, data, 1);
    
    // ok ... done the basic stuff. i have put inside the array the data it arrives in the correct position.
    
    // now we alert someone that the message has arrived for the given channel. 
    eocanprotASpolling_redefinable_alert_reception_of_POL_AS_CMD__GET_FULL_SCALES(channel, data, strain);
    
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__GET_FIRMWARE_VERSION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    eObrd_protocolversion_t *reqprot = (eObrd_protocolversion_t*)descriptor->cmd.value;
    
    s_former_POL_AS_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION);
    frame->data[1] = reqprot->major;
    frame->data[2] = reqprot->minor;
    
    return(eores_OK);      
}

extern eOresult_t eocanprotASpolling_parser_POL_AS_CMD__GET_FIRMWARE_VERSION(eOcanframe_t *frame, eOcanport_t port)
{   
    eOcanmap_location_t loc = {0};
    loc.port                = port;
    loc.addr                = EOCANPROT_FRAME_GET_SOURCE(frame);
    loc.insideindex         = EOCANPROT_FRAME_POLLING_MC_GET_INTERNALINDEX(frame);
    
    eObool_t match = (1 == frame->data[7]) ? eobool_true : eobool_false;    
    
    eObrd_info_t detected = {0};
    detected.type               = frame->data[1];
    detected.firmware.major     = frame->data[2];
    detected.firmware.minor     = frame->data[3];
    detected.firmware.build     = frame->data[4];
    detected.protocol.major     = frame->data[5];
    detected.protocol.minor     = frame->data[6];   
    
    eo_candiscovery2_OneBoardIsFound(eo_candiscovery2_GetHandle(), loc, match, &detected);   

    return eores_OK;
}


extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__TACT_SETUP(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 8, ICUBCANPROTO_POL_SK_CMD__TACT_SETUP);    

    /* 2) set can command (see SkinPrototype::calibrateSensor)*/
    frame->data[1] = 0x01;  //==> risoluzione 8 bit   e 12 isure indipendenti
    frame->data[2] = 0x01;  //==> invia ogni 40 milli
    frame->data[3] = 0x03;
    frame->data[4] = 0;
    frame->data[5] = 0x20;
    frame->data[6] = 0;
    frame->data[7] = 0x35;    
    
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__SET_BRD_CFG(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 4, ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG);    
    
    icubCanProto_skinboard_config_t* bcfg = (icubCanProto_skinboard_config_t *)descriptor->cmd.value;    
    /* 2) set can command (see SkinPrototype::calibrateSensor)*/
    frame->data[0] = ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG;  
    frame->data[1] = (bcfg->skintype &0x07);
    frame->data[2] = bcfg->period;
    frame->data[3] = bcfg->noload;

    return(eores_OK);    
}

extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__TACT_SETUP2(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 7, ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2);    

    /* 2) set can command (see SkinPrototype::calibrateSensor)*/
    frame->data[1] = 0x00;  
    frame->data[2] = 0x22;
    frame->data[3] = 0xf0;
    frame->data[4] = 0x00;
    frame->data[5] = 0xFF;
    frame->data[6] = 0xff;    
    
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__ACC_GYRO_SETUP(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_SK_CMD__ACC_GYRO_SETUP);    

    icubCanProto_inertial_config_t *config = (icubCanProto_inertial_config_t*)descriptor->cmd.value;
    frame->data[1] = config->enabledsensors;  
    frame->data[2] = config->period;
       
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_former_POL_SK_CMD__SET_TRIANG_CFG(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 7, ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG);    

    icubCanProto_skintriangles_config_t * tcfg = (icubCanProto_skintriangles_config_t *)descriptor->cmd.value;

    /* 2) set can command (see SkinPrototype::calibrateSensor)*/

    frame->data[1] = tcfg->idstart;
    frame->data[2] = tcfg->idend;
    frame->data[3] = tcfg->shift;
    frame->data[4] = tcfg->flags;
    *((uint16_t*)(&frame->data[5])) = tcfg->CDCoffset;
    
    return(eores_OK);
}


extern eOresult_t eocanprotASpolling_parser_00(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_former_00(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_parser_01(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_former_01(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_parser_02(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_former_02(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_former_POL_AS_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type)
{   // every message coming from the ems has actually id 0
    frame->id           = EOCANPROT_CREATE_CANID(eocanprot_msgclass_pollingAnalogSensor, 0, descriptor->loc.addr);
    frame->id_type      = eocanframeID_std11bits;
    frame->frame_type   = eocanframetype_data; 
    frame->size         = len;
    frame->data[0]      = EOCANPROT_CREATE_POLLING_AS_DATA0(type);    
}


static void* s_eocanprotASpolling_get_entity(eOprotEndpoint_t endpoint, eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, uint8_t *index)
{
    void * ret = NULL;
    uint8_t ii = 0;
    eOcanmap_location_t loc = {0};
    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eocanmap_insideindex_none;
    
    ii = eo_canmap_GetEntityIndexExtraCheck(eo_canmap_GetHandle(), loc, endpoint, entity);
    
    if(EOK_uint08dummy == ii)
    {     
        //#warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        return(NULL);
    }
    
    ret = eoprot_entity_ramof_get(eoprot_board_localboard, endpoint, entity, ii);
    
    if(NULL != index)
    {
        *index = ii;        
    }  

    return(ret);   
}


//static void s_eocanprotASpolling_getfullscale_nextstep(uint8_t channel, eOas_strain_t *strain, uint8_t index)
//{
//    eOresult_t res = eores_OK;
//    
//    if(5 == channel)
//    {   // received the last channel
//        
////        // prepare occasional rop to send
////        res = s_loadFullscalelikeoccasionalrop(index);
////        if(eores_OK != res)
////        {
////            // diagnostics
////            return;
////        }
//    }
//    else
//    {   // send request for next channel
//        channel++;
//        
//        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, index, 0);
//        eOcanprot_command_t command = {0};
//        command.class = eocanprot_msgclass_pollingAnalogSensor;
//        command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
//        command.value = &channel;
//        
//        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
//    }
//}    

// marco.accame: removed because ... it is the redefined function eocanprotASpolling_redefinable_alert_reception_of_POL_AS_CMD__GET_FULL_SCALES() which
// will transmit the status to robotInterface. in this file we just do nothing.
//static eOresult_t s_loadFullscalelikeoccasionalrop(eOas_strainId_t sId)
//{
//    eOresult_t res;
//    eOropdescriptor_t ropdesc;

//    // marco.accame on 3 feb 15: better using the eok_ropdesc_basic instead of all 0s.
//    // memset(&ropdesc, 0, sizeof(eOropdescriptor_t));
//    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));

//    ropdesc.ropcode                 = eo_ropcode_sig;
//    ropdesc.size                    = sizeof(eOas_arrayofupto12bytes_t);
//    ropdesc.id32                    = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, sId, eoprot_tag_as_strain_status_fullscale); 
//    ropdesc.data                    = NULL;

//    res = eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
//   
//    return(res);
//}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





