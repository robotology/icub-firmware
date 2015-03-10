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


/* @file       eOcfg_icubCanProto_sensorBoardMsgfunction.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"
#include "EOcommon.h"
#include "EOtheErrorManager.h"

#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"
#include "EOicubCanProto_hid.h"
#include "EOtheEMSApplBody.h"

#include "EOtheProtocolWrapper.h"


//icub api
#include "EoAnalogSensors.h"
#include "EoProtocol.h"
#include "EoProtocolAS.h"

//to load occasional rop
#include "EOMtheEMStransceiver.h"
#include "EOtransceiver.h"

//to send evt to configurator task
#include "EOMtask.h"
#include "EOMtheEMSconfigurator.h"
#include "EOMtheEMSappl.h"

#include "EOtheEMSapplDiagnostics.h"

#include "EOtransceiver.h"

#include "EoError.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_icubCanProto_sensorBoardMsgFunctions.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define EMS_CAN_ADDR                                            0
#define ICUBCANPROTO_POL_AS_ID_DEFAULT                          ((ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR << 8) | (0x0<<4))
#define ICUBCANPROTO_POL_AS_CREATE_ID(destBoardAddr)            ((ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR << 8) | (EMS_CAN_ADDR<<4) | (destBoardAddr&0xF))
#define ICUBCANPROTO_POL_SK_CREATE_ID(destBoardAddr)            ((ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR << 8)|  (EMS_CAN_ADDR<<4) | (destBoardAddr&0xF))
#define ICUBCANPROTO_PER_AS_CREATE_ID(cmdId, addr)              ((ICUBCANPROTO_CLASS_PERIODIC_ANALOGSENSOR << 8) | (addr<<4) | (cmdId&0xF))

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_loadFullscalelikeoccasionalrop(eOas_strainId_t sId);

static void s_eo_icubCanProto_sb_send_runtime_error_diagnostics(uint64_t par64);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//********************** P A R S E R       POLLING     F U N C T I O N S  ******************************************************

extern eOresult_t eo_icubCanProto_parser_pol_sb_unexpected_cmd(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_sb_unexpected_cmd(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setTxMode(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOenum08_t *txmode = (eOenum08_t *)val_ptr;
    
    canFrame->id = ICUBCANPROTO_POL_AS_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;
    canFrame->data[0] = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    canFrame->data[1] = *txmode;
    
//     switch(*maismode)
//     {
//         case eoas_maismode_txdatacontinuously:
//         {
//                 canFrame->data[1] = 0;
//         }break;
//         
//         case eoas_maismode_acquirebutdonttx:
//         {
//                 canFrame->data[1] = 1;
//             
//         }break;

//         case eoas_maismode_dontacquiredonttx:
//         {
//                 canFrame->data[1] = 2;
//             
//         }break;

//     };
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setCanDatarate(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    uint8_t *datarate = (uint8_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_POL_AS_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;
    canFrame->data[0] = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    canFrame->data[1] = *datarate;
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setResolution(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOas_maisresolution_t *maisresolution = (eOas_maisresolution_t*)val_ptr;
    
    canFrame->id = ICUBCANPROTO_POL_AS_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;
    canFrame->data[0] = ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
    switch(*maisresolution)
    {
        case eoas_maisresolution_08:
        {
            canFrame->data[1] = 2;
        } break;
        
        case eoas_maisresolution_16:
        {
            canFrame->data[1] = 0;
        } break;

#ifdef _MAIS_TEST_
        case eoas_maisresolution_debug:
        {
            canFrame->data[1] = 1;
        } break;
#endif
        default:
        {
            
        } break;

    }
    
    return(eores_OK);
}

#warning --> marco.accame: review mechanism: in here the board receives the can frames about the full scales from strain. one at a time.
#define TEST_EOARRAY
extern eOresult_t eo_icubCanProto_parser_pol_sb_cmd__getFullScales(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    uint8_t                                     channel;
    eOresult_t                                  res;
    eOappTheDB_board_canlocation_t              canLoc = {0};
    eOas_strainId_t                             sId;
    eOas_strain_config_t                        *config = NULL;
    eOas_strain_status_t                        *status = NULL;
    eOicubCanProto_msgDestination_t             msgdest;
    eOsmStatesEMSappl_t                         appl_st;
    eOicubCanProto_msgCommand_t                 msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES
    };
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    /* 1) get can location */
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    
    res = eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(eo_appTheDB_GetHandle(), canLoc, &sId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(1);
        return(eores_OK);
    }

    /* 2) get signaloncefullscale, contained in strain config nv. */
    config = eo_protocolwrapper_GetStrainConfig(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == config)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(2);
        return(eores_OK);
    } 
//     if(!config->signaloncefullscale)
//     {
//         //some error occured
//         return(eores_NOK_nodata);
//     }
    
    /* 3) get pointer to nv var where save incoming force values */

    status = eo_protocolwrapper_GetStrainStatus(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == status)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(3);
        return(eores_OK);
    }      
    
    
#if defined(TEST_EOARRAY)
    channel = eo_array_Size((EOarray*)&status->fullscale);
#else
    channel = (status->fullscale.head.size);
#endif
    
    //eOerrmanDescriptor_t des = {0};
         
    if(channel != frame->data[1])
    {
        // something wrong: i asked full scale for channel "channel" and i have instead received channel for "frame.data[1]" 
        //des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag02);
        //des.param = channel | (frame->data[1] << 8);
        //des.sourceaddress = 0;
        //des.sourcedevice = eo_errman_sourcedevice_localboard;
        //eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &des);
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(4);
        return(eores_OK);
    }
    else
    {
        //des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag02);
        //des.param = channel | (frame->data[1] << 8);
        //des.sourceaddress = 0;
        //des.sourcedevice = eo_errman_sourcedevice_localboard;
        //eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &des);        
    }
    
    /* status->fullscale.head.size*status->fullscale.head.itemsize  == (status->fullscale.head.size<<1) 
     since itemsize is equal to 2 i used shift insted of moltiplication*/
#if defined(TEST_EOARRAY)
    eo_array_PushBack((EOarray*)&status->fullscale, &(frame->data[2]));
#else    
    memcpy(&(status->fullscale.data[(status->fullscale.head.size<<1)]), &(frame->data[2]), 2);
    status->fullscale.head.size++;
#endif
    
    /* 4) if i received last channel's full scale i prepare a occasional rop to send
          else request full scale of next channel */
    if(5 == channel)
    {
        // prepare occasional rop to send
        res = s_loadFullscalelikeoccasionalrop(sId);
        if(eores_OK != res)
        {
            s_eo_icubCanProto_sb_send_runtime_error_diagnostics(5);
            return(eores_OK);
        }
        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &appl_st);
        
        // if application is in cfg state, then we send a request to configurator to send ropframe out
        if(eo_sm_emsappl_STcfg == appl_st)
        {
            eom_task_SetEvent(eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()), emsconfigurator_evt_ropframeTx); 
        }
    }
    else
    {
        channel++;
        msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr); 
        eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &channel);
    }
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__getFullScales(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    uint8_t *channel = (uint8_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_POL_AS_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;
    canFrame->data[0] = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
    canFrame->data[1] = *channel;

    return(eores_OK);
}


//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__forceVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_board_canlocation_t              canLoc = {0};
    eOas_strainId_t                             sId;
    eOas_strain_config_t                        *config = NULL;
    eOas_strain_status_t                        *status = NULL;

    /* 1) get can location */
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    
    res = eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(eo_appTheDB_GetHandle(), canLoc, &sId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(6);
        return(eores_OK);
    }

    /* 2) get strain transmission mode, contained in strain config nv.
          (parsing depends on transmission mode)*/
    config = eo_protocolwrapper_GetStrainConfig(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == config)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(7);
        return(eores_OK);
    } 
    /* 3) get pointer to nv var where save incoming force values */
    status = eo_protocolwrapper_GetStrainStatus(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == status)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(8);
        return(eores_OK);
    } 
    
    
    /* 4) set incoming force values */    
    switch(config->mode)
    {
        case eoas_strainmode_txcalibrateddatacontinuously:
        case eoas_strainmode_txalldatacontinuously:
        {
            eo_array_Assign((EOarray*)(&status->calibratedvalues), 0, &(frame->data[0]), 3);
            //memcpy(&(status->calibratedvalues.data[0]), &(frame->data[0]), 6);
        } break;

        case eoas_strainmode_txuncalibrateddatacontinuously:
        {
            eo_array_Assign((EOarray*)(&status->uncalibratedvalues), 0, &(frame->data[0]), 3);
            //memcpy(&(status->uncalibratedvalues.data[0]), &(frame->data[0]), 6);
        } break;
        
        case eoas_strainmode_acquirebutdonttx:
        {
            // i dont do anything in here. but i dont return nok. because it may be that we must empty a rx buffer of canframes rx just before
            // that we have silenced the strain.
        } break;
        
        default:
        {
            //i must never be here!
            s_eo_icubCanProto_sb_send_runtime_error_diagnostics(9);
        }
    };
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__torqueVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_board_canlocation_t              canLoc = {0};
    eOas_strainId_t                             sId;
    eOas_strain_config_t                        *config = NULL;
    eOas_strain_status_t                        *status = NULL;

    /* 1) get can location */
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    
    res = eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(eo_appTheDB_GetHandle(), canLoc, &sId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(10);
        return(eores_OK);
    }

    /* 2) get strain transmission mode, contained in straion config nv.
          (parsing depends on transmission mode)*/    
    config = eo_protocolwrapper_GetStrainConfig(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == config)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(11);
        return(eores_OK);
    }     
    
    /* 3) get pointer to nv var where save incoming force values */
    status = eo_protocolwrapper_GetStrainStatus(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == status)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(12);
        return(eores_OK);
    }      
    
    /* 4) set incoming force values */
    
    switch(config->mode)
    {
        case eoas_strainmode_txcalibrateddatacontinuously:
        case eoas_strainmode_txalldatacontinuously:
        {
            eo_array_Assign((EOarray*)(&status->calibratedvalues), 3, &(frame->data[0]), 3);
            //memcpy(&(status->calibratedvalues.data[6]), &(frame->data[0]), 6);
        } break;

        case eoas_strainmode_txuncalibrateddatacontinuously:
        {
            eo_array_Assign((EOarray*)(&status->uncalibratedvalues), 3, &(frame->data[0]), 3);
            //memcpy(&(status->uncalibratedvalues.data[6]), &frame->data[0], 6);
        } break;
        
        case eoas_strainmode_acquirebutdonttx:
        {
            // i dont do anything in here. but i dont return nok. because it may be that we must empty a rx buffer of canframes rx just before
            // that we have silenced the strain.
        } break;
        
        default:
        {
            //i must never be here!
            s_eo_icubCanProto_sb_send_runtime_error_diagnostics(13);
        }
    };
    
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__uncalibForceVectorDebugmode(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_board_canlocation_t              canLoc = {0};
    eOas_strainId_t                             sId;
    eOas_strain_status_t                        *status = NULL;

    /* 1) get can location */
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    
    res = eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(eo_appTheDB_GetHandle(), canLoc, &sId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(14);
        return(eores_OK);
    }

    /* 1) get pointer to nv var where save incoming force values */
    status = eo_protocolwrapper_GetStrainStatus(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == status)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(15);
        return(eores_OK);
    } 

    // use the eoarray
    memcpy(&(status->uncalibratedvalues.data[0]), &frame->data[0], 6);

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__uncalibTorqueVectorDebugmode(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_board_canlocation_t              canLoc = {0};
    eOas_strainId_t                             sId;
    eOas_strain_status_t                        *status = NULL;

    /* 1) get can location */
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    
    res = eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(eo_appTheDB_GetHandle(), canLoc, &sId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(16);
        return(eores_OK);
    }

    /* 1) get pointer to nv var where save incoming force values */
    status = eo_protocolwrapper_GetStrainStatus(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == status)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(17);
        return(eores_OK);
    } 

    // use the eoarray
    memcpy(&(status->uncalibratedvalues.data[6]), &frame->data[0], 6);

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes0to6(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOas_maisId_t                               sId;
    eOappTheDB_board_canlocation_t              canLoc = {0};
    eOas_mais_status_t                          *status = NULL;

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    res = eo_appTheDB_GetSnsrMaisId_BySensorCanLocation(eo_appTheDB_GetHandle(), canLoc, &sId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(18);
        return(eores_OK);
    }

    status = eo_protocolwrapper_GetMaisStatus(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == status)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(19);
        return(eores_OK);
    }     

    // copy the canframe data values inside the array
    #if 1
    // not inserted juts to avoid too many changes on re-ordering of protocol v1 on date 6 dec 13, but it is the correct way to to it
    EOarray* array = (EOarray*)&status->the15values;
    eo_array_Assign(array, 0, &(frame->data[0]), 7); // 7 bytes of frame->data starting from array position 0
    #else
    #error -> old mode
    status->the15values.head.size = 15;
    memcpy(&(status->the15values.data[0]), &(frame->data[0]), 7); 
    #endif

    return(eores_OK);    
}


extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes7to14(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOas_maisId_t                               sId;
    eOappTheDB_board_canlocation_t              canLoc = {0};
    eOas_mais_status_t                          *status = NULL;

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    res = eo_appTheDB_GetSnsrMaisId_BySensorCanLocation(eo_appTheDB_GetHandle(), canLoc, &sId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(20);
        return(eores_OK);
    }
    
    status = eo_protocolwrapper_GetMaisStatus(eo_protocolwrapper_GetHandle(), sId);
    if(NULL == status)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(21);
        return(eores_OK);
    } 
    
    // copy the canframe data values inside the array
    #if 1
    // not inserted juts to avoid too many changes on re-ordering of protocol v1 on date 6 dec 13, but it is the correct way to to it
    EOarray* array = (EOarray*)&status->the15values;
    eo_array_Assign(array, 7, &(frame->data[0]), 8); // 8 bytes of frame->data starting from array position 8
    #else
    #error -> old mode
    status->the15values.head.size = 15;
    memcpy(&(status->the15values.data[7]), &(frame->data[0]), 8); 
    #endif
    
    return(eores_OK);
}



//********************** F O R M E R       PERIODIC     F U N C T I O N S  ******************************************************

extern eOresult_t eo_icubCanProto_former_per_sb_cmd__forceVector(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{

    uint8_t *data_ptr = (uint8_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_PER_AS_CREATE_ID(ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR, dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 6;
    memcpy(&canFrame->data[0], data_ptr, 6);
    return(eores_OK);

}
extern eOresult_t eo_icubCanProto_former_per_sb_cmd__torqueVector(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    uint8_t *data_ptr = (uint8_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_PER_AS_CREATE_ID(ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR, dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 6;
    memcpy(&canFrame->data[0], data_ptr, 6);
    return(eores_OK);

}


/***********************************************************************************************************************************/
/*******************************  S K I N     F U N C T I O N S    *******************************************************************/
/***********************************************************************************************************************************/

extern eOresult_t eo_icubCanProto_parser_per_sk_cmd__allSkinMsg(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                      res = eores_NOK_generic;
    eOsk_skinId_t                   skId = 0;
    EOappTheDB                      *db = eo_appTheDB_GetHandle();
    eOappTheDB_SkinCanLocation_t    canloc = {0};
    eOsk_candata_t                  candata = {0};

    canloc.emscanport = canPort;
    res = eo_appTheDB_GetSkinId_BySkinCanLocation(db, canloc, &skId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(22);
        return(eores_OK);
    }
    
    //#warning -> marco.accame: we have only two skins (at most), thus we could pre-compute the skin0 and skin1 pointers onece for all.
    //            maybe as static values.   
    
    
    eOsk_skin_t *skin = (eOsk_skin_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_skin, eosk_entity_skin, (eOprotIndex_t)skId);
    if(NULL == skin)
    {   // i dont have a skin on skId
        s_eo_icubCanProto_sb_send_runtime_error_diagnostics(23);
        return(eores_OK);
    }
            
    // if skin->config.sigmode is dontsignal then we dont use the payload of the can frame. 
    // we may decode some canframes of this kind if we pass from run to config mode and we process frame buffrede in the rx-fifo    
    if(eosk_sigmode_dontsignal == skin->config.sigmode)
    {
        return(eores_OK);
    }        
    
    // otherwise we put the canframe content inside teh arrayofcandata
    
    uint16_t info = EOSK_CANDATA_INFO(frame->size, frame->id);
    candata.info = info;    
    memcpy(candata.data, frame->data, sizeof(candata.data));
    
    
    res = eo_array_PushBack((EOarray*)(&skin->status.arrayofcandata), &candata);
    if(eores_OK != res)
    {   
        eOerrmanDescriptor_t des = {0};
        des.code            = eoerror_code_get(eoerror_category_Skin, eoerror_value_SK_arrayofcandataoverflow);
        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourceaddress   = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
        des.sourcedevice    = (eOcanport1 == canPort) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des); 
        //s_eo_icubCanProto_sb_send_runtime_error_diagnostics(24);
    }

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__tactSetup(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_SK_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;

    /* 2) set can command (see SkinPrototype::calibrateSensor)*/

    canFrame->data[0] = ICUBCANPROTO_POL_SK_CMD__TACT_SETUP;  
    canFrame->data[1] = 0x01;  //==> risoluzione 8 bit   e 12 isure indipendenti
    canFrame->data[2] = 0x01;  //==> invia ogni 40 milli
    canFrame->data[3] = 0x03;
    canFrame->data[4] = 0;
    canFrame->data[5] = 0x20;
    canFrame->data[6] = 0;
    canFrame->data[7] = 0x35;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__tactSetup2(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_SK_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;

    /* 2) set can command (see SkinPrototype::calibrateSensor)*/
    canFrame->data[0] = ICUBCANPROTO_POL_SK_CMD__TACT_SETUP2; 
    canFrame->data[1] = 0x00;  
    canFrame->data[2] = 0x22;
    canFrame->data[3] = 0xf0;
    canFrame->data[4] = 0x00;
    canFrame->data[5] = 0xFF;
    canFrame->data[6] = 0xff;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__setBrdCfg(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_SK_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 4;
    icubCanProto_skinboard_config_t* bcfg = (icubCanProto_skinboard_config_t *)val_ptr;

    /* 2) set can command (see SkinPrototype::calibrateSensor)*/

    canFrame->data[0] = ICUBCANPROTO_POL_SK_CMD__SET_BRD_CFG;  
    canFrame->data[1] = (bcfg->skintype &0x07);
    canFrame->data[2] = bcfg->period;
    canFrame->data[3] = bcfg->noload;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__accGyroSetup(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__setTriangCfg(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_SK_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard idicubCanProto_board_config_t
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    icubCanProto_skintriangles_config_t * tcfg = (icubCanProto_skintriangles_config_t *)val_ptr;

    /* 2) set can command (see SkinPrototype::calibrateSensor)*/

    canFrame->data[0] = ICUBCANPROTO_POL_SK_CMD__SET_TRIANG_CFG;  
    canFrame->data[1] = tcfg->idstart;
    canFrame->data[2] = tcfg->idend;
    canFrame->data[3] = tcfg->shift;
    canFrame->data[4] = tcfg->flags;
    *((uint16_t*)(&canFrame->data[5])) = tcfg->CDCoffset;

    return(eores_OK);
}

//unused
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes0to3(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
//
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes4to7(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
//
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes8to11(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
//
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes12to15(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_loadFullscalelikeoccasionalrop(eOas_strainId_t sId)
{
    eOresult_t res;
    eOropdescriptor_t ropdesc;
    const eOemsapplbody_cfg_t* bodycfg = eo_emsapplBody_GetConfig(eo_emsapplBody_GetHandle());    
    
    if(NULL == bodycfg)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, "eo_emsapplBody_GetConfig() fails tag_12cerf", NULL, &eo_errman_DescrRuntimeErrorLocal);
        return(eores_NOK_generic);
    }

    // marco.accame on 3 feb 15: better using the eok_ropdesc_basic instead of all 0s.
    // memset(&ropdesc, 0, sizeof(eOropdescriptor_t));
    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));

    ropdesc.ropcode                 = eo_ropcode_sig;
    ropdesc.size                    = sizeof(eOas_arrayofupto12bytes_t);
    ropdesc.id32                    = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, sId, eoprot_tag_as_strain_status_fullscale); 
    ropdesc.data                    = NULL;

   
    res = eo_transceiver_OccasionalROP_Load( eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()), &ropdesc); 
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

    if(eores_OK != res)
    {
        eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
        eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , 1000);
    }
    
    return(res);
}


static void s_eo_icubCanProto_sb_send_runtime_error_diagnostics(uint64_t par64)
{
    eOerrmanDescriptor_t des = {0};
    des.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runtimeerror);
    des.par16 = 0x1111;
    des.par64 = 0x1100000000000000 + par64;
    des.sourceaddress = 0;
    des.sourcedevice = eo_errman_sourcedevice_localboard;
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &des);    
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



