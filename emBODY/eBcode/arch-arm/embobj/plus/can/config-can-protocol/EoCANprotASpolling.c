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

#include "EOtheSTRAIN.h"

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



// this function receives the full scale of a channel and asks object EOtheSTRAIN to take care of it
extern eOresult_t eocanprotASpolling_parser_POL_AS_CMD__GET_FULL_SCALES(eOcanframe_t *frame, eOcanport_t port)
{
    eo_strain_AcceptCANframe(eo_strain_GetHandle(), frame, port, processFullScale);
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
    eObrd_canlocation_t loc = {0};
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

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__IMU_CONFIG_GET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_parser_POL_AS_CMD__IMU_CONFIG_GET(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__IMU_CONFIG_SET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 8, ICUBCANPROTO_POL_AS_CMD__IMU_CONFIG_SET);    

    icubCanProto_imu_config_t *cfg = (icubCanProto_imu_config_t*)descriptor->cmd.value;
    frame->data[1] = cfg->enabledsensors & 0xff; 
    frame->data[2] = (cfg->enabledsensors >> 8) & 0xff;  
    frame->data[3] = cfg->fusionmode;
    frame->data[4] = cfg->ffu[0];
    frame->data[5] = cfg->ffu[1];
    frame->data[6] = cfg->ffu[2];
    frame->data[7] = cfg->ffu[3];    
    
    return(eores_OK);    
}

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__IMU_TRANSMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 2, ICUBCANPROTO_POL_AS_CMD__IMU_TRANSMIT);    

    icubCanProto_imu_transmit_t *trs = (icubCanProto_imu_transmit_t*)descriptor->cmd.value;
    frame->data[1] = trs->period; 

    return(eores_OK);
}


extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__THERMOMETER_CONFIG_SET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 2, ICUBCANPROTO_POL_AS_CMD__THERMOMETER_CONFIG_SET);    

    icubCanProto_thermo_config_t *tc = (icubCanProto_thermo_config_t*)descriptor->cmd.value;
    frame->data[1] = tc->sensormask;
    
    return(eores_OK);    
}

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__THERMOMETER_TRANSMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 2, ICUBCANPROTO_POL_AS_CMD__THERMOMETER_TRANSMIT);    

    icubCanProto_thermo_transmit_t *tt = (icubCanProto_thermo_transmit_t*)descriptor->cmd.value;
    frame->data[1] = tt->periodsec;
    
    return(eores_OK);
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

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__POS_CONFIG_GET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    //#warning see if we have anything to set ... maybe we dont ask and it does not tell
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_parser_POL_AS_CMD__POS_CONFIG_GET(eOcanframe_t *frame, eOcanport_t port)
{
    //#warning see if we have anything to retrieve ... maybe it does not tell because we dont ask
    return(eores_OK);
}

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__POS_CONFIG_SET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 8, ICUBCANPROTO_POL_AS_CMD__POS_CONFIG_SET);    

    icubCanProto_POS_CONFIG_t *cfg = (icubCanProto_POS_CONFIG_t*)descriptor->cmd.value;
    frame->data[1] = cfg->type & 0xff; 
    if(icubCanProto_pos_decideg == cfg->type)
    {
        frame->data[1] = icubCanProto_pos_decideg;
        // we have settings for two sensors ...
        frame->data[2] = (cfg->setting.decideg[0].enabled << 7) | (cfg->setting.decideg[0].invertdirection << 6) | (cfg->setting.decideg[0].rotation << 4) | (cfg->setting.decideg[0].label & 0x0f);
        frame->data[3] = (cfg->setting.decideg[0].zero & 0x00ff);           // lsb of zero
        frame->data[4] = (cfg->setting.decideg[0].zero & 0xff00) >> 8;      // msb of zero
        
        frame->data[5] = (cfg->setting.decideg[1].enabled << 7) | (cfg->setting.decideg[1].invertdirection << 6) | (cfg->setting.decideg[1].rotation << 4) | (cfg->setting.decideg[1].label & 0x0f);
        frame->data[6] = (cfg->setting.decideg[1].zero & 0x00ff);           // lsb of zero
        frame->data[7] = (cfg->setting.decideg[1].zero & 0xff00) >> 8;      // msb of zero             
    }
    else
    {
        frame->data[1] = icubCanProto_pos_unkwown;
        memmove(&frame->data[2], cfg->setting.unknown, sizeof(cfg->setting.unknown));        
    }
      
    return(eores_OK);    
}

extern eOresult_t eocanprotASpolling_former_POL_AS_CMD__POS_TRANSMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_AS_prepare_frame(descriptor, frame, 2, ICUBCANPROTO_POL_AS_CMD__POS_TRANSMIT);    

    icubCanProto_POS_TRANSMIT_t *trs = (icubCanProto_POS_TRANSMIT_t*)descriptor->cmd.value;
    frame->data[1] = trs->rate; 

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
    eObrd_canlocation_t loc = {0};
    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eobrd_caninsideindex_none;
    
    ii = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, endpoint, entity);
    
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


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





