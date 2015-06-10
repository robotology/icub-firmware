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


/* @file       eOcfg_icubCanProto_motorBoardMsgfunction.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"

#include "EOcommon.h"
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"
#include "EOicubCanProto_hid.h"

#include "EOtheProtocolWrapper.h"

#include "EOMtheEMSapplCfg.h"
#include "EOtheEMSapplBody.h"
#include "EOappMeasuresConverter.h"
#include "EOappMeasuresConverter_hid.h"
#include "EOemsController.h"
#include "EOappTheDataBase.h"

//#include "hal_debugPin.h"

#include "OPCprotocolManager_Cfg.h"
#include "EoDiagnostics.h"
#include "EOtheEMSapplDiagnostics.h"
#include "EoError.h"
#include "EOVtheSystem.h"

#ifdef USE_PROTO_PROXY
#include "EOtheBOARDtransceiver.h" //to get proxy pointer
#include "EoProtocolMC.h"
#include "EOlist_hid.h"
#include "EOtheEMSapplDiagnostics.h"
#include "EOproxy.h"
#include "EOtheBOARDtransceiver.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_icubCanProto_motorBoardMsgFunctions.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define EMS_CAN_ADDR                                    0
#define ICUBCANPROTO_POL_MC_ID_DEFAULT                  ((ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL << 8) | (0x0<<4))
#define ICUBCANPROTO_POL_MC_CREATE_ID(destBoardAddr)    ((ICUBCANPROTO_CLASS_POLLING_MOTORCONTROL << 8) | (EMS_CAN_ADDR<<4) | (destBoardAddr&0xF))
#define ICUBCANPROTO_PER_MC_CREATE_ID(cmdId)            ((ICUBCANPROTO_CLASS_PERIODIC_MOTORCONTROL << 8) | (EMS_CAN_ADDR<<4) | (cmdId&0xF))



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__position(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc);
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__pidVal(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc);
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__current(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc);
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__velocity(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc);
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__pidError(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc);
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__print (EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc);
static eOresult_t s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode(icubCanProto_controlmode_t icubcanProto_controlmode, eOmc_controlmode_t *eomc_controlmode);
//static eOresult_t s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode_old(icubCanProto_controlmode_t icubcanProto_controlmode, eOmc_jointId_t jId, uint8_t hw_error_flags, eOmc_controlmode_t *eomc_controlmode);
static eOresult_t s_eo_appTheDB_UpdateMototStatusPtr(eOmc_motorId_t mId, eOcanframe_t *frame, eOmn_appl_runMode_t runmode);
static eOresult_t s_eo_icubCanProto_translate_icubCanProtoInteractionMode2eOmcInteractionMode(icubCanProto_interactionmode_t icubcanProto_intermode,
                                                                                      eOmc_interactionmode_t *eomc_intermode);
static void s_eo_icubCanProto_mb_send_runtime_error_diagnostics(uint8_t parser, uint64_t par64, eOcanframe_t *frame, eOcanport_t port);
   
static eObool_t s_eo_icubCanProto_is_from_unused2foc_in_eb5(eOcanframe_t *frame, eOcanport_t canPortRX);

static void s_eo_icubCanProto_mb_send_up_canframe(eOcanframe_t *frame, eOcanport_t canport);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "icubCanProto";

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern eOresult_t eo_icubCanProto_parser_pol_mb_unexpected_cmd(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    // the caller of this function will manage the NOK and will issue a unrecognised can frame message
    return(eores_NOK_unsupported);
}


extern eOresult_t eo_icubCanProto_parser_test(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_unexpected_cmd(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    // the caller will manage the NOK by issuing the proper diagnostics message
    return(eores_NOK_unsupported);
}



extern eOresult_t eo_icubCanProto_former_pol_mb_dummy_cmd(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = 0;
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;
    memset(&canFrame->data[0], 0, 8);
    return(eores_OK);
}




extern eOresult_t eo_icubCanProto_former_test(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1) <<7) | ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
    canFrame->data[1] = *((uint8_t*)val_ptr);
    canFrame->data[2] = *(((uint8_t*)val_ptr) +1);
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__controllerRun(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__controllerIdle(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE;
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__toggleVerbose(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__calibrateEncoder(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    icubCanProto_calibrator_t *calib_ptr = (icubCanProto_calibrator_t *)val_ptr;
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;

    /* 2 set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER;

    /*3) set command's params */
    canFrame->data[1] = calib_ptr->type;
    

    switch(calib_ptr->type)
    {
        case icubCanProto_calibration_type0_hard_stops:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type0.pwmlimit;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type0.velocity;
            memset(&canFrame->data[6], 0, 2); //pad with 0          
        } break;

        case icubCanProto_calibration_type1_abs_sens_analog:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type1.position;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type1.velocity;
            memset(&canFrame->data[6], 0, 2); //pad with 0          
        } break;

        case icubCanProto_calibration_type2_hard_stops_diff:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type2.pwmlimit;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type2.velocity;
            memset(&canFrame->data[6], 0, 2); //pad with 0          
        } break;


        case icubCanProto_calibration_type3_abs_sens_digital:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type3.position;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type3.velocity;
            *((uint16_t*)(&canFrame->data[6])) = calib_ptr->params.type3.offset;        
        } break;

        case icubCanProto_calibration_type4_abs_and_incremental:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type4.position;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type4.velocity;
            *((uint16_t*)(&canFrame->data[6])) = calib_ptr->params.type4.maxencoder;        
        } break;
        
        default:
        {
            s_eo_icubCanProto_mb_send_runtime_error_diagnostics(0, 1, NULL, 0);
        } break;

    }
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__enablePwmPad(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__disablePwmPad(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getControlMode(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res = eores_NOK_generic;
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    eOmc_joint_status_t                     *jstatus = NULL;
    eOerrmanDescriptor_t des = {0};    
    static uint8_t calledonce = 0;

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 2, frame, canPort);
        return(eores_OK);
    }

    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 3, frame, canPort);
        return(eores_OK);
    }


    if(0 == calledonce)
    {
        calledonce = 1;
                
        des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag06);
        des.par16 = (frame->data[1] << 8) | jstatus->basic.controlmodestatus;
        des.sourceaddress = jId;
        des.sourcedevice = eo_errman_sourcedevice_localboard;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des);       
        
    }

    jstatus->basic.controlmodestatus = ((eOmc_controlmode_t)frame->data[1]);

    if(eomc_controlmode_hwFault == jstatus->basic.controlmodestatus)
    {   
        des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_hwfault1);
        des.par16 = (frame->data[1] << 8) | jstatus->basic.controlmodestatus;
        des.sourceaddress = jId;
        des.sourcedevice = eo_errman_sourcedevice_localboard;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &des);               
    }
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getControlMode(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE;

    /* this command hasn't params */

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__motionDone(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res = eores_NOK_generic;
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    eOmc_joint_status_t                     *jstatus = NULL;
    

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 4, frame, canPort);
        return(eores_OK);
    }

    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 5, frame, canPort);
        return(eores_OK);
    }
    
    eOmc_motionmonitorstatus_t motionmonitorstatus = (eOmc_motionmonitorstatus_t) jstatus->basic.motionmonitorstatus;
    
    if(eomc_motionmonitorstatus_notmonitored == motionmonitorstatus)
    {
        //pc104 isn't interested in motion monitoring
        return(eores_OK);
    }
    
    jstatus->basic.motionmonitorstatus = (eOmc_motionmonitorstatus_t)frame->data[1];

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__motionDone(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__MOTION_DONE;

    /* this command hasn't params */

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setControlMode(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    icubCanProto_controlmode_t *controlMode_ptr = (icubCanProto_controlmode_t*)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;

    /* 3) set command's params */
    canFrame->data[1] = *controlMode_ptr;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getAdditionalInfo(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getAdditionalInfo(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setAdditionalInfo(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setSpeedEtimShift(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOicubCanProto_estimShift_t *estimShift_ptr = (eOicubCanProto_estimShift_t*)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;

    /* 2 set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT;

    /*3) set command's params */
    *((uint8_t*)(&canFrame->data[1])) = estimShift_ptr->estimShiftJointVel;
    *((uint8_t*)(&canFrame->data[2])) = estimShift_ptr->estimShiftJointAcc;
    *((uint8_t*)(&canFrame->data[3])) = estimShift_ptr->estimShiftMotorVel;
    *((uint8_t*)(&canFrame->data[4])) = estimShift_ptr->estimShiftMotorAcc;
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDebugParam(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDebugParam(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDebugParam(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getEncoderPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getEncoderPosition(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__positionMove(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    icubCanProto_setpoint_position_t *pos_setpoint_ptr = (icubCanProto_setpoint_position_t *)val_ptr;
    /*NOTE: here i don't check is the given setpoint is a position one. i trust to it*/

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;

    /* 2 set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE;

    /* 3) set command's params */
    *((icubCanProto_position_t*)(&canFrame->data[1])) = pos_setpoint_ptr->value;
    *((icubCanProto_velocity_t*)(&canFrame->data[5])) = pos_setpoint_ptr->withvelocity;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__velocityMove(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    icubCanProto_setpoint_velocity_t *vel_setpoint_ptr = (icubCanProto_setpoint_velocity_t *)val_ptr;
    /*NOTE: here i don't check is the given setpoint is a velocity one. i trust to it*/

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;

    /* 2 set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE;

    /*3) set command's params */
    *((icubCanProto_velocity_t*)(&canFrame->data[1])) = vel_setpoint_ptr->value;
    *((icubCanProto_acceleration_t*)(&canFrame->data[3])) = vel_setpoint_ptr->withacceleration;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setEncoderPosition(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


//extern eOresult_t send_diagnostics_to_server(const char *str, uint32_t signature, uint8_t plustime);

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDesiredTorque(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    icubCanProto_setpoint_torque_t *torque_setpoint_ptr = (icubCanProto_setpoint_torque_t *)val_ptr;
    /*NOTE: here i don't check is the given setpoint is a velocity one. i trust to it*/

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;

    /* 2 set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE;

    /*3) set command's params */
    //*((icubCanProto_torque_t*)(&canFrame->data[1])) = torque_setpoint_ptr->value;
    
    //static char msg[31];
    //snprintf(msg,sizeof(msg),"SET TRQ %d",torque_setpoint_ptr->value);
    //send_diagnostics_to_server(msg, 0xffffffff, 1); 
    
    canFrame->data[1] = ((uint8_t*)&(torque_setpoint_ptr->value))[0];
    canFrame->data[2] = ((uint8_t*)&(torque_setpoint_ptr->value))[1];
    canFrame->data[4] = canFrame->data[3] = (canFrame->data[2] & 0x80) ? 0xFF : 0;
    
    /* Note: the firware and icubcanproto expect torque value on 32bit. 
       Currently icubInterface and new eo-proto use 16 bits for torque */
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDesiredTorque(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{   
    // marco.accame on 03mar15: the use of teh proxy in here is not correct. the proxy which triggered the
    // ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE can message came from a ask<mc_joint_cmmnds_setpoint> which is not used by robotInterface
    // and moreover cannot in here call the proxy with a id32 with tag eoprot_tag_mc_joint_config_impedance.
    // BETTER REMOVE THE CODE.
    
//#ifdef USE_PROTO_PROXY
//    eOmc_jointId_t                          jId;
//    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
//    EOappTheDB                              *db = eo_appTheDB_GetHandle();
//    eOmc_setpoint_t                         setpoint = {0};
//    
//    canLoc.emscanport = canPort;
//    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
//    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
//    
//    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//    
//    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint,  jId, eoprot_tag_mc_joint_config_impedance);
//    
//#if 0    
////    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
////    if(NULL == li)
////    {
////        eOerrmanDescriptor_t errdes = {0};
////        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
////        errdes.sourceaddress    = 0;
////        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
////        errdes.par16            = 0; 
////        errdes.par64            = id32; 
////        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);        
////  
////        return(eores_NOK_generic);
////    }
////    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
////    
////    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
////    //icubCanProto_torque_t icub_trq =  *((icubCanProto_torque_t*)(&frame->data[1]));
////    
////    icubCanProto_torque_t icub_trq = ((icubCanProto_torque_t)frame->data[1]) | (((icubCanProto_torque_t)frame->data[2])<<8);
////    
////    
////    setpoint.type = eomc_setpoint_torque;
////    setpoint.to.torque.value = eo_appMeasConv_torque_S2I(appMeasConv_ptr, jId, icub_trq);
//
//
////    req->numOfREceivedResp++;
////    res = eores_OK;
////    
////    if(req->numOfREceivedResp == req->numOfExpectedResp)
////    {
////        //send back response
////        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
////        
////        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &setpoint);
////        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
////    }
//#else
//
//    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
//    if(NULL == param)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
//
//        return(eores_NOK_generic);
//    } 
//
//
//    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
//
//    
//    icubCanProto_torque_t icub_trq = ((icubCanProto_torque_t)frame->data[1]) | (((icubCanProto_torque_t)frame->data[2])<<8);
//    
//    
//    setpoint.type = eomc_setpoint_torque;
//    setpoint.to.torque.value = eo_appMeasConv_torque_S2I(appMeasConv_ptr, jId, icub_trq);
//     
//    
//    param->p08_2 ++;
//    res = eores_OK;
//    
//    if(param->p08_1 == param->p08_2)
//    {
//        // send back response
//        res = eo_proxy_ReplyROP_Load(proxy, id32, &setpoint);
//        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());
//
//
//    }
//    
//#endif 
//    
//#endif // USE_PROTO_PROXY  
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDesiredTorque(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE;

    /* this command hasn't params */


    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__stopTrajectory(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY;

    /* this command hasn't params */

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getBoardId(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getBoardId(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMinPosition(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
    *((icubCanProto_position_t*)(&canFrame->data[1])) = *((icubCanProto_position_t*)val_ptr);
    return(eores_OK);


}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMinPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res = eores_OK;
#ifdef USE_PROTO_PROXY
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    EOappTheDB                              *db = eo_appTheDB_GetHandle();
    
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
    
 
    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 6, frame, canPort);
        return(eores_OK);
    }
    
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jId, eoprot_tag_mc_joint_config_limitsofjoint);
   
#if 0    
//    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
//    if(NULL == li)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
//        return(eores_NOK_generic);
//    }
//    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
//    
//    eOmeas_position_limits_t *limits_ptr = (eOmeas_position_limits_t*)req->nvRam_ptr;
//    
//    icubCanProto_position_t icub_pos =  *((icubCanProto_position_t*)(&frame->data[1]));
//    limits_ptr->min = eo_appMeasConv_jntPosition_E2I(eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle()), jId, icub_pos);
//    
//    req->numOfREceivedResp++;
//    res = eores_OK;
//    
//    if(req->numOfREceivedResp == req->numOfExpectedResp)
//    {
//        //send back response
//        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//        eOmeas_position_limits_t limits_aux;
//        
//        
//        if(limits_ptr->max < limits_ptr->min)
//        {
//            limits_aux.max = limits_ptr->min;
//            limits_aux.min = limits_ptr->max;
//        }
//        else
//        {
//            memcpy(&limits_aux, limits_ptr, sizeof(eOmeas_position_limits_t));
//        }
//        
//        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &limits_aux);

//        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
//        
//    }
#else

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmeas_position_limits_t *limits_ptr = (eOmeas_position_limits_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);    
    
    icubCanProto_position_t icub_pos =  *((icubCanProto_position_t*)(&frame->data[1]));
    limits_ptr->min = eo_appMeasConv_jntPosition_E2I(eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle()), jId, icub_pos);

    param->p08_2 ++;
    res = eores_OK;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        eOmeas_position_limits_t limits_aux = {0};
               
        if(limits_ptr->max < limits_ptr->min)
        {
            limits_aux.max = limits_ptr->min;
            limits_aux.min = limits_ptr->max;
        }
        else
        {
            memcpy(&limits_aux, limits_ptr, sizeof(eOmeas_position_limits_t));
        }
        
        res = eo_proxy_ReplyROP_Load(proxy, id32, &limits_aux);  
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());        
    }
    
#endif
    
#endif //USE_PROTO_PROXY
    return(eores_OK);


}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMinPosition(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMaxPosition(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
    *((icubCanProto_position_t*)(&canFrame->data[1])) = *((icubCanProto_position_t*)val_ptr);
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMaxPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
eOresult_t                                  res = eores_OK;
#ifdef USE_PROTO_PROXY
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    EOappTheDB                              *db = eo_appTheDB_GetHandle();
    
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
    
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 7, frame, canPort);
        return(eores_OK);
    }
    
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jId, eoprot_tag_mc_joint_config_limitsofjoint);

#if 0
//    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
//    if(NULL == li)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);        
//        return(eores_NOK_generic);
//    }
//    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
//    
//    eOmeas_position_limits_t *limits_ptr = (eOmeas_position_limits_t*)req->nvRam_ptr;
//    
//    icubCanProto_position_t icub_pos =  *((icubCanProto_position_t*)(&frame->data[1]));
//    limits_ptr->max = eo_appMeasConv_jntPosition_E2I(eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle()), jId, icub_pos);
//    
//    req->numOfREceivedResp++;
//    res = eores_OK;
//    
//    if(req->numOfREceivedResp == req->numOfExpectedResp)
//    {
//        //send back response
//        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//        eOmeas_position_limits_t limits_aux;
//        
//        
//        if(limits_ptr->max < limits_ptr->min)
//        {
//            limits_aux.max = limits_ptr->min;
//            limits_aux.min = limits_ptr->max;
//        }
//        else
//        {
//            memcpy(&limits_aux, limits_ptr, sizeof(eOmeas_position_limits_t));
//        }
//        
//        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &limits_aux);
//        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
//        
//    }
#else

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmeas_position_limits_t *limits_ptr = (eOmeas_position_limits_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);    

    icubCanProto_position_t icub_pos =  *((icubCanProto_position_t*)(&frame->data[1]));
    limits_ptr->max = eo_appMeasConv_jntPosition_E2I(eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle()), jId, icub_pos);
 
    
    param->p08_2 ++;
    res = eores_OK;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        eOmeas_position_limits_t limits_aux = {0};
                       
        if(limits_ptr->max < limits_ptr->min)
        {
            limits_aux.max = limits_ptr->min;
            limits_aux.min = limits_ptr->max;
        }
        else
        {
            memcpy(&limits_aux, limits_ptr, sizeof(eOmeas_position_limits_t));
        }
        
        res = eo_proxy_ReplyROP_Load(proxy, id32, &limits_aux);  
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());              
    }
    
#endif
    
#endif // USE_PROTO_PROXY
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMaxPosition(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION;
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMaxVelocity(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
    *((icubCanProto_velocity_t*)(&canFrame->data[1])) = *((icubCanProto_velocity_t*)val_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMaxVelocity(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMaxVelocity(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentLimit(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmeas_current_t *motor_currentlimit_ptr = (eOmeas_current_t*)val_ptr;
    int32_t maxcurrent = *motor_currentlimit_ptr;
    /* Note: eo-protocol uses 16 bits for current, while icubcanprotocol uses 32bits */

    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    *((int32_t*)(&canFrame->data[1])) = maxcurrent;
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setBcastPolicy(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    uint8_t     *payload_ptr = (uint8_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY;
    
    memcpy(&canFrame->data[1], payload_ptr, 4);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelShift(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
   icubCanProto_velocityShift_t shift = *((icubCanProto_velocityShift_t*)val_ptr);
   canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
   canFrame->id_type = 0; //standard id
   canFrame->frame_type = 0; //data frame
   canFrame->size = 3;
   canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT;
   *((uint16_t*)(&canFrame->data[1])) = shift;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setOffsetAbsEncoder(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getOffsetAbsEncoder(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getOffsetAbsEncoder(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorquePid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *torque_pid_ptr = (eOmc_PID_t *)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID;

    /* 3) set command's params */
    *((uint16_t*)(&canFrame->data[1])) = (int16_t) torque_pid_ptr->kp;
    *((uint16_t*)(&canFrame->data[3])) = (int16_t) torque_pid_ptr->ki;
    *((uint16_t*)(&canFrame->data[5])) = (int16_t) torque_pid_ptr->kd;
    canFrame->data[7] = (int8_t) torque_pid_ptr->scale;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorquePid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
eOresult_t                                  res = eores_OK;
#ifdef USE_PROTO_PROXY
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    EOappTheDB                              *db = eo_appTheDB_GetHandle();
    
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
    
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 8, frame, canPort);
        return(eores_OK);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jId, eoprot_tag_mc_joint_config_pidtorque);
    
#if 0    
//    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
//    if(NULL == li)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);        
//        return(eores_NOK_generic);
//    }
//    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
//    
//    eOmc_PID_t* pid_ptr = (eOmc_PID_t*)req->nvRam_ptr;
//    pid_ptr->kp = *((uint16_t*)&frame->data[1]);
//    pid_ptr->ki = *((uint16_t*)&frame->data[3]);
//    pid_ptr->kd = *((uint16_t*)&frame->data[5]);
//    
//    req->numOfREceivedResp++;
//    res = eores_OK;
//    
//    if(req->numOfREceivedResp == req->numOfExpectedResp)
//    {
//        //send back response
//        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//        eOmc_PID_t pid_aux;
//        memcpy(&pid_aux, pid_ptr, sizeof(eOmc_PID_t));
//        
//        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &pid_aux);
//        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
//        
//    }   
#else

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmc_PID_t* pid_ptr = (eOmc_PID_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    pid_ptr->kp = *((int16_t*)&frame->data[1]);
    pid_ptr->ki = *((int16_t*)&frame->data[3]);
    pid_ptr->kd = *((int16_t*)&frame->data[5]); 
    
    
    param->p08_2 ++;
    res = eores_OK;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        eOmc_PID_t pid_aux = {0};
        memcpy(&pid_aux, pid_ptr, sizeof(eOmc_PID_t));
        
        res = eo_proxy_ReplyROP_Load(proxy, id32, &pid_aux); 
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());
    }
    
#endif
    
#endif // USE_PROTO_PROXY
    
    return(eores_OK);

}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorquePidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *torque_pid_ptr = (eOmc_PID_t *)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8; /*  Currently in messages.h there is a check on frame_len equal to 8, else frame is discard.
                            so i left size=8 even if correct size is 7 */

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS;

    /* 3) set command's params */
    *((uint16_t*)(&canFrame->data[1])) = (int16_t) torque_pid_ptr->offset;
    *((uint16_t*)(&canFrame->data[3])) = (int16_t) torque_pid_ptr->limitonoutput;
    *((uint16_t*)(&canFrame->data[5])) = (int16_t) torque_pid_ptr->limitonintegral;


    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorquePidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res = eores_OK;
#ifdef USE_PROTO_PROXY
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    EOappTheDB                              *db = eo_appTheDB_GetHandle();
    
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 9, frame, canPort);
        return(eores_OK);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint,  jId, eoprot_tag_mc_joint_config_pidtorque);

#if 0
//    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
//    if(NULL == li)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);        
//        return(eores_NOK_generic);
//    }
//    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
//    
//    eOmc_PID_t* pid_ptr = (eOmc_PID_t*)req->nvRam_ptr;
//    
//    pid_ptr->offset = *((uint16_t*)(&frame->data[1]));
//    pid_ptr->limitonoutput = *((uint16_t*)(&frame->data[3]));
//    pid_ptr->limitonintegral = *((uint16_t*)(&frame->data[5]));
//  
//    req->numOfREceivedResp++;
//    res = eores_OK;
//    
//    if(req->numOfREceivedResp == req->numOfExpectedResp)
//    {
//        // send back response
//        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//        eOmc_PID_t pid_aux;
//        memcpy(&pid_aux, pid_ptr, sizeof(eOmc_PID_t));
//        
//        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &pid_aux);
//        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
//    }
#else

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmc_PID_t* pid_ptr = (eOmc_PID_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    pid_ptr->offset = *((int16_t*)(&frame->data[1]));
    pid_ptr->limitonoutput = *((int16_t*)(&frame->data[3]));
    pid_ptr->limitonintegral = *((int16_t*)(&frame->data[5])); 
    
    
    param->p08_2 ++;
    res = eores_OK;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        eOmc_PID_t pid_aux = {0};
        memcpy(&pid_aux, pid_ptr, sizeof(eOmc_PID_t));
        
        res = eo_proxy_ReplyROP_Load(proxy, id32, &pid_aux);  
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());        
    }
    
#endif
    
#endif // USE_PROTO_PROXY
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosPid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *pos_pid_ptr = (eOmc_PID_t *)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_POS_PID;

    /* 3) set command's params */
    *((uint16_t*)(&canFrame->data[1])) = (int16_t) pos_pid_ptr->kp;
    *((uint16_t*)(&canFrame->data[3])) = (int16_t) pos_pid_ptr->ki;
    *((uint16_t*)(&canFrame->data[5])) = (int16_t) pos_pid_ptr->kd;
    canFrame->data[7] = (int8_t) pos_pid_ptr->scale;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{    
    eOresult_t                              res = eores_OK;
#ifdef USE_PROTO_PROXY
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    EOappTheDB                              *db = eo_appTheDB_GetHandle();

    
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
    
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 10, frame, canPort);
        return(eores_OK);
    }
    
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jId, eoprot_tag_mc_joint_config_pidposition);

#if 0
//    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
//    if(NULL == li)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);        
//        return(eores_NOK_generic);

//    }
//    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
//    
//    eOmc_PID_t* pid_ptr = (eOmc_PID_t*)req->nvRam_ptr;
//    pid_ptr->kp = (*((uint16_t*)&frame->data[1]));
//    pid_ptr->ki = (*((uint16_t*)&frame->data[3]));
//    pid_ptr->kd = *((uint16_t*)&frame->data[5]);
//    
//    req->numOfREceivedResp++;
//    res = eores_OK;
//    
//    if(req->numOfREceivedResp == req->numOfExpectedResp)
//    {
//        //send back response
//        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//        eOmc_PID_t pid_aux;
//        memcpy(&pid_aux, pid_ptr, sizeof(eOmc_PID_t));
//        
//        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &pid_aux);
//        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
//        
//    }
#else

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmc_PID_t* pid_ptr = (eOmc_PID_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    pid_ptr->kp = (*((int16_t*)&frame->data[1]));
    pid_ptr->ki = (*((int16_t*)&frame->data[3]));
    pid_ptr->kd = (*((int16_t*)&frame->data[5]));
    
    
    param->p08_2 ++;
    res = eores_OK;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        eOmc_PID_t pid_aux = {0};
        memcpy(&pid_aux, pid_ptr, sizeof(eOmc_PID_t));
        
        res = eo_proxy_ReplyROP_Load(proxy, id32, &pid_aux);   
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());        
    }
    
#endif
    
#endif // USE_PROTO_PROXY
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    
     /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_POS_PID;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosPidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *pos_pid_ptr = (eOmc_PID_t *)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8; /*  Currently in messages.h there is a check on frame_len equal to 8, else frame is discard.
                            so i left size=8 even if correct size is 7 */

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS;

    /* 3) set command's params */
    *((uint16_t*)(&canFrame->data[1])) = (int16_t) pos_pid_ptr->offset;
    *((uint16_t*)(&canFrame->data[3])) = (int16_t) pos_pid_ptr->limitonoutput;
    *((uint16_t*)(&canFrame->data[5])) = (int16_t) pos_pid_ptr->limitonintegral;
    canFrame->data[7] = 0; //add this to clear values becaiuse len is 8 and not 7.

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res = eores_OK;
#ifdef USE_PROTO_PROXY
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    EOappTheDB                              *db = eo_appTheDB_GetHandle();
    
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 11, frame, canPort);
        return(eores_OK);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint,  jId, eoprot_tag_mc_joint_config_pidposition);

#if 0
//    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
//    if(NULL == li)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);        
//        return(eores_NOK_generic);
//    }
//    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
//    
//    eOmc_PID_t* pid_ptr = (eOmc_PID_t*)req->nvRam_ptr;
//    
//    pid_ptr->offset = *((uint16_t*)(&frame->data[1]));
//    pid_ptr->limitonoutput = *((uint16_t*)(&frame->data[3]));
//    pid_ptr->limitonintegral = *((uint16_t*)(&frame->data[5]));
//  
//    req->numOfREceivedResp++;
//    res = eores_OK;
//    
//    if(req->numOfREceivedResp == req->numOfExpectedResp)
//    {
//        //send back response
//        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//        eOmc_PID_t pid_aux = {0};
//        memcpy(&pid_aux, pid_ptr, sizeof(eOmc_PID_t));
//        
//        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &pid_aux);                      
//        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
//    }
#else

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmc_PID_t* pid_ptr = (eOmc_PID_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    pid_ptr->limitonoutput = *((int16_t*)(&frame->data[3]));
    pid_ptr->limitonintegral = *((int16_t*)(&frame->data[5]));
    
    
    param->p08_2 ++;
    res = eores_OK;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        eOmc_PID_t pid_aux = {0};
        memcpy(&pid_aux, pid_ptr, sizeof(eOmc_PID_t));
        
        res = eo_proxy_ReplyROP_Load(proxy, id32, &pid_aux);
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());        
    }
    
#endif    
    
#endif // USE_PROTO_PROXY   
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelTimeout(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT;
    *((uint16_t*)(&canFrame->data[1])) = *((uint16_t*)val_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setImpedanceParams(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    icubCanProto_impedance_t *imp_ptr = (icubCanProto_impedance_t *)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;
    
    memset(&canFrame->data[0], 0, 8);
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS;

    //stiffnes and damping are uint16_t
    *((icubCanProto_stiffness_t*)(&canFrame->data[1])) = (icubCanProto_stiffness_t)imp_ptr->stiffness;
    *((icubCanProto_damping_t*)(&canFrame->data[3])) = (icubCanProto_damping_t)imp_ptr->damping;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getImpedanceParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res = eores_OK;
#ifdef USE_PROTO_PROXY
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    EOappTheDB                              *db = eo_appTheDB_GetHandle();
    
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 12, frame, canPort);
        return(eores_OK);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint,  jId, eoprot_tag_mc_joint_config_impedance);

#if 0
//    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
//    if(NULL == li)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);        
//        return(eores_NOK_generic);
//    }
//    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
//    
//    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
//    
//    eOmc_impedance_t  *impedance_ptr = (eOmc_impedance_t*)req->nvRam_ptr;
//    
//    icubCanProto_stiffness_t icub_stiff = *((icubCanProto_stiffness_t*)(&frame->data[1]));
//    icubCanProto_damping_t   icub_dump = *((icubCanProto_damping_t*)(&frame->data[3]));
//    
//    impedance_ptr->stiffness = eo_appMeasConv_impedenceStiffness_S2I(appMeasConv_ptr, jId, icub_stiff);
//    impedance_ptr->damping = eo_appMeasConv_impedenceDamping_S2I(appMeasConv_ptr, jId, icub_dump);
//    
//    req->numOfREceivedResp++;
//    res = eores_OK;
//    
//    if(req->numOfREceivedResp == req->numOfExpectedResp)
//    {
//        //send back response
//        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//        eOmc_impedance_t imp_aux;
//        memcpy(&imp_aux, impedance_ptr, sizeof(eOmc_impedance_t));
//        
//        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &imp_aux);
//        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
//    }
#else

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmc_impedance_t *impedance_ptr = (eOmc_impedance_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    
    icubCanProto_stiffness_t icub_stiff = *((icubCanProto_stiffness_t*)(&frame->data[1]));
    icubCanProto_damping_t   icub_dump = *((icubCanProto_damping_t*)(&frame->data[3]));
    
    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
    
    impedance_ptr->stiffness = eo_appMeasConv_impedenceStiffness_S2I(appMeasConv_ptr, jId, icub_stiff);
    impedance_ptr->damping = eo_appMeasConv_impedenceDamping_S2I(appMeasConv_ptr, jId, icub_dump);
    
    
    param->p08_2 ++;
    res = eores_OK;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        eOmc_impedance_t imp_aux = {0};
        memcpy(&imp_aux, impedance_ptr, sizeof(eOmc_impedance_t));
        
        res = eo_proxy_ReplyROP_Load(proxy, id32, &imp_aux);
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());
    }
    
#endif  
    
#endif // USE_PROTO_PROXY    
    return(eores_OK);
    
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceParams(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    
    memset(&canFrame->data[0], 0, 8);
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS;
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setImpedanceOffset(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    icubCanProto_torque_t *impOffset_ptr = (icubCanProto_torque_t *)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET;

    *((icubCanProto_torque_t*)(&canFrame->data[1])) = *((icubCanProto_torque_t*)(impOffset_ptr));

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getImpedanceOffset(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res = eores_OK;
#ifdef USE_PROTO_PROXY
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    EOappTheDB                              *db = eo_appTheDB_GetHandle();

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 13, frame, canPort);
        return(eores_OK);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint,  jId, eoprot_tag_mc_joint_config_impedance);

#if 0
//    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
//    if(NULL == li)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);        
//        return(eores_NOK_generic);
//    }
//    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
//    
//    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
//    
//    eOmc_impedance_t  *impedance_ptr = (eOmc_impedance_t*)req->nvRam_ptr;
//    
//    icubCanProto_torque_t icub_trq = *((icubCanProto_torque_t*)(&frame->data[1]));
//    impedance_ptr->offset = eo_appMeasConv_torque_S2I(appMeasConv_ptr, jId, icub_trq);
//
//    req->numOfREceivedResp++;
//    res = eores_OK;
//    
//    if(req->numOfREceivedResp == req->numOfExpectedResp)
//    {
//        //send back response
//        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//        eOmc_impedance_t imp_aux;
//        memcpy(&imp_aux, impedance_ptr, sizeof(eOmc_impedance_t));
//        
//        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &imp_aux);
//        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
//    }
#else

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
    eOmc_impedance_t *impedance_ptr = (eOmc_impedance_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    
    icubCanProto_torque_t icub_trq = *((icubCanProto_torque_t*)(&frame->data[1]));
    impedance_ptr->offset = eo_appMeasConv_torque_S2I(appMeasConv_ptr, jId, icub_trq);
     
    
    param->p08_2 ++;
    res = eores_OK;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        eOmc_impedance_t imp_aux = {0};
        memcpy(&imp_aux, impedance_ptr, sizeof(eOmc_impedance_t));
        
        res = eo_proxy_ReplyROP_Load(proxy, id32, &imp_aux);
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());
    }
    
#endif      
    
#endif // USE_PROTO_PROXY    
    return(eores_OK);
    
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceOffset(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    
    memset(&canFrame->data[0], 0, 8);
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET;
    return(eores_OK);

}

// marco.accame.todo: mettere qui info sulle board ricevute.

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getFirmwareVersion(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res;
    eOappTheDB_board_canlocation_t          canLoc = {0};
    eObrd_boardId_t                         bid;
    char                                    str[56] = {0};
    uint32_t                                canBoardsReady = 0;
    EOappTheDB                              *db = eo_appTheDB_GetHandle();
    eObrd_typeandversions_t                 typeversion = {0};

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id); 
    

    res = eo_appTheDB_GetCanBoardId_ByCanLocation(db, canLoc, &bid); 
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 14, frame, canPort);
        return(eores_OK);
    }
    
    typeversion.boardtype                   = frame->data[1];
    typeversion.firmwareversion.major       = frame->data[2];
    typeversion.firmwareversion.minor       = frame->data[3];
    typeversion.firmwarebuildnumber         = frame->data[4];
    typeversion.protocolversion.major       = frame->data[5];
    typeversion.protocolversion.minor       = frame->data[6];
     
    if(1 != frame->data[7])
    {
        eo_appTheDB_FillCanTypeVersion(db, canLoc, &typeversion);
       
//        #warning --> marco.accame: the expected protocol fw version is different ... tell robotinterface with a proper message
//        uint16_t buildNum = *((uint16_t*)&frame->data[2]);
//        snprintf(str, sizeof(str), "getfwVer id%d: type=0x%x fw_ver=0x%x build=%d prot=%d.%d check=%d", frame->id, frame->data[1], buildNum, frame->data[4], frame->data[5], frame->data[6],frame->data[7]); 
//        //snprintf(str, sizeof(str), "protFWver mismatch: CAN%d ID=%d BRD=0x%x FW=0x%x BLD=%d PROTVER=%d.%d", canPort+1, frame->id, frame->data[1], buildNum, frame->data[4], frame->data[5], frame->data[6]);   
        snprintf(str, sizeof(str), "wrong fwver: %d %d", typeversion.firmwareversion.major, typeversion.firmwareversion.minor); 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, str, s_eobj_ownname, &eo_errman_DescrUnspecified);
    }
    else
    {
        // ok: we impose that the board with the passed can location is ready ...
        eo_appTheDB_setCanBoardReady(db, canLoc, &typeversion);
    }

    
    // if all connected can boards are ready, then we stop the request procedure
    if(eobool_true == eo_appTheDB_areConnectedCanBoardsReady(db, &canBoardsReady, NULL))
    {
        eo_emsapplBody_checkCanBoards_Stop(eo_emsapplBody_GetHandle());
        eo_emsapplBody_sendConfig2canboards(eo_emsapplBody_GetHandle());
        //if MC4, enable MAIS and BCastPolicy
        eOmn_appl_runMode_t appl_run_mode = eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
        if((applrunMode__skinAndMc4 == appl_run_mode) || (applrunMode__mc4Only == appl_run_mode))
        {   
			eo_emsapplBody_MAISstart(eo_emsapplBody_GetHandle());
        }
    }
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getFirmwareVersion(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOicubCanProto_protocolVersion_t *protover_ptr = (eOicubCanProto_protocolVersion_t *)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION;

    canFrame->data[1] = protover_ptr->major;
    canFrame->data[2] = protover_ptr->minor;
    
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosStictionParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosStictionParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosStictionParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorqueStictionParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorqueStictionParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorqueStictionParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setBackemfParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getBackemfParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getBackemfParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setModelParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getModelParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getModelParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}




extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentPid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    //eOmc_PID_t *cur_pid_ptr = (eOmc_PID_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    memcpy(canFrame->data+1, val_ptr, 7);

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getCurrentPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getCurrentPid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentPidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *cur_pid_ptr = (eOmc_PID_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;

    *((uint16_t*)(&canFrame->data[1])) = cur_pid_ptr->offset;
    *((uint16_t*)(&canFrame->data[3])) = cur_pid_ptr->limitonoutput;
    *((uint16_t*)(&canFrame->data[5])) = cur_pid_ptr->limitonintegral;
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getCurrentPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getCurrentPidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelocityPid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{

    eOmc_PID_t *vel_pid_ptr = (eOmc_PID_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID;
    *((uint16_t*)(&canFrame->data[1])) = vel_pid_ptr->kp;
    *((uint16_t*)(&canFrame->data[3])) = vel_pid_ptr->ki;
    *((uint16_t*)(&canFrame->data[5])) = vel_pid_ptr->kd;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getVelocityPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getVelocityPid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelocityPidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *vel_pid_ptr = (eOmc_PID_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PIDLIMITS;

    *((uint16_t*)(&canFrame->data[1])) = vel_pid_ptr->offset;
    *((uint16_t*)(&canFrame->data[3])) = vel_pid_ptr->limitonoutput;
    *((uint16_t*)(&canFrame->data[5])) = vel_pid_ptr->limitonintegral;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getVelocityPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getVelocityPidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDesiredCurrent(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmeas_current_t *currentVal_ptr = (eOmeas_current_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT;
    *((eOmeas_current_t*)(&canFrame->data[1])) = *currentVal_ptr;
    canFrame->data[3]= 0; //LSB Id
    canFrame->data[4]= 0; //MSB Id
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDesiredCurrent(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDesiredCurrent(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPeriodicContents(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setI2TParams(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    
    eOmc_i2tParams_t *i2tParams_ptr = (eOmc_i2tParams_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS;
    *((uint16_t*)(&canFrame->data[1])) = i2tParams_ptr->time;
    *((uint16_t*)(&canFrame->data[3])) = i2tParams_ptr->tresh;
    
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getI2TParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getI2TParams(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getOpenLoopParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res = eores_OK;
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    EOappTheDB                              *db = eo_appTheDB_GetHandle();
    eOmc_joint_status_t                     *jstatus = NULL;
    
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 15, frame, canPort);
        return(eores_OK);
    }
    
    
    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 16, frame, canPort);
        return(eores_OK);
    }
       
    jstatus->ofpid.positionreference = *((int16_t*)&frame->data[1]);

    return(eores_OK);
}





//extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getOpenLoopParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//eOresult_t                                  res = eores_OK;
//#ifdef USE_PROTO_PROXY
//    eOmc_jointId_t                          jId;
//    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
//    EOappTheDB                              *db = eo_appTheDB_GetHandle();
//    int16_t                                 value;
//    eOmc_joint_status_ofpid_t               statusOfPid;
//    
//    canLoc.emscanport = canPort;
//    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
//    canLoc.indexinsidecanboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);;   
//    
//    res = eo_appTheDB_GetJointId_ByJointCanLocation(db, &canLoc, &jId);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//    
//    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint,  jId, eoprot_tag_mc_joint_status_ofpid);
//    EOlistIter * li = eo_appTheDB_searchEthProtoRequest(db, id32);
//    if(NULL == li)
//    {
//        eOerrmanDescriptor_t errdes = {0};
//        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//        errdes.sourceaddress    = 0;
//        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
//        errdes.par16            = 0; 
//        errdes.par64            = id32; 
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
////           char str[128];
////           snprintf(str, sizeof(str)-1, "in ask openLoop errrore j=%d res=%d",jId, res);
////           eo_theEMSdgn_UpdateErrorLog(eo_theEMSdgn_GetHandle(), &str[0], sizeof(str));
////           eom_emsbackdoor_Signal(eom_emsbackdoor_GetHandle(), eodgn_nvidbdoor_errorlog , 3000);
//           return(eores_NOK_generic);
//    }
//    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)li->data;
//    
//    statusOfPid.reference = *((int16_t*)&frame->data[1]);


//    req->numOfREceivedResp++;
//    res = eores_OK;
//    
//    if(req->numOfREceivedResp == req->numOfExpectedResp)
//    {
//        //send back response
//        EOproxy *proxy_ptr = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
//        
//        res = eo_proxy_ReplyROP_Load(proxy_ptr, id32, &setpoint);
//        if(eores_OK != res)
//        {
//           char str[128];
//           snprintf(str, sizeof(str)-1, "error in ReplyROP_Load openLoop j=%d res=%d",jId, res);
//           eo_theEMSdgn_UpdateErrorLog(eo_theEMSdgn_GetHandle(), &str[0], sizeof(str));
//           eom_emsbackdoor_Signal(eom_emsbackdoor_GetHandle(), eodgn_nvidbdoor_errorlog , 3000);
//        }
//        res = eo_appTheDB_removeEthProtoRequest(db, eoprot_entity_mc_joint, jId, li);
//    }
//#endif    
//    return(res);
//}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setOpenLoopParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    icubCanProto_setpoint_current_t *setpoint_ptr = (icubCanProto_setpoint_current_t*)nv_ptr;
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS;
    *((int16_t*)(&canFrame->data[1])) = setpoint_ptr->value;
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getOpenLoopParams(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCmdPos(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION;
    *((icubCanProto_position_t*)(&canFrame->data[1])) = *((icubCanProto_position_t*)nv_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setInteractionMode(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE;
    *((icubCanProto_interactionmode_t*)(&canFrame->data[1])) = *((icubCanProto_interactionmode_t*)nv_ptr);
    return(eores_OK);

}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMotorConfig(EOicubCanProto* p, void *nv_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG;
    canFrame->data[1] = 1; // etc.
    memcpy(canFrame->data+1,nv_ptr,6);
    return(eores_OK);
}


//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__2foc(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    /* 2foc sends: current (2 bytes), velocity(2 bytes), position(4 bytes)*/
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    eOmc_motorId_t   		                mId;
    eOmc_motor_status_t                     *mstatus = NULL;


    // set position about the first motor in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_first;    
    
    res = eo_appTheDB_GetMotorId_ByMotorCanLocation(eo_appTheDB_GetHandle(), canLoc, &mId);
    if(eores_OK != res)
    {
        if(eobool_true == s_eo_icubCanProto_is_from_unused2foc_in_eb5(frame, canPort))
        {   // the board eb5 has an additional 1foc board which sends a can frame of this kind but it does not serve any motor
            // or joint. hence, its can address is not recognised by eo_appTheDB_GetMotorId_ByMotorCanLocation().
            // for this board we must not issue the error but we must do nothing
            return(eores_OK);            
        }
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 17, frame, canPort);
        return(eores_OK);
    }

    mstatus = eo_protocolwrapper_GetMotorStatus(eo_protocolwrapper_GetHandle(), mId);
    if(NULL == mstatus)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 18, frame, canPort);
        return(eores_OK);
    }     
    

    // note of marco.accame: the following code is ok as long as the 2foc has been configured to send up in its periodic message 
    // current, velocity, and position. if so, frame->data contains: [current:2bytes, velocity:2bytes, position:4bytes]. 
    // the following code extract these values. 
    mstatus->basic.mot_current  = ((int16_t*)frame->data)[0];
    mstatus->basic.mot_velocity = ((int16_t*)frame->data)[1];
    mstatus->basic.mot_position = ((int32_t*)frame->data)[1];
     
    eo_emsController_AcquireMotorEncoder(mId, mstatus->basic.mot_current, mstatus->basic.mot_velocity, mstatus->basic.mot_position);

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__position(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};

    
    // set position about the first joint in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_first;
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__position(p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 19, frame, canPort);
        return(eores_OK);
    }


    // set position about the second joint in board
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_second;
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__position(p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 20, frame, canPort);
        return(eores_OK);
    }
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidVal(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};

    
    // set position about the first joint in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_first;    
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__pidVal(p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 21, frame, canPort);
        return(eores_OK);
    }

    // set position about the second joint in board
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_second;
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__pidVal(p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 22, frame, canPort);
        return(eores_OK);
    }
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__status(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res = eores_OK;
    eOmc_jointId_t   		                jId;
    eOmc_motorId_t   		                mId;
    eOmc_joint_status_t                     *jstatus = NULL;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {0};
    eOmc_controlmode_t                      eomc_controlmode;
    eOmn_appl_runMode_t                     runmode; 
        
    eOerrmanDescriptor_t des = {0};
   

    
    // set position about the first joint in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_first;    

    
    runmode = eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
    
    if(applrunMode__default == runmode)
    {
        return(eores_OK); // ignore error
    }
    
    // get mId
    res = eo_appTheDB_GetMotorId_ByMotorCanLocation(eo_appTheDB_GetHandle(), canLoc, &mId);
    if(eores_OK != res)
    {
        if(eobool_true == s_eo_icubCanProto_is_from_unused2foc_in_eb5(frame, canPort))
        {   // see lines 2394-2396
            return(eores_OK);            
        }
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 23, frame, canPort);
        return(eores_OK);
    }
   
    if(applrunMode__2foc == runmode)
    {
        //in this case one can addr =>one motor!!
        res = s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode((icubCanProto_controlmode_t) frame->data[1], &eomc_controlmode);
        if(eores_OK != res)
        {
            s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 24, frame, canPort);
            return(eores_OK);
        }
        eo_emsController_ReadMotorstatus(mId, frame->data);
        //l'aggiornamento delle nv del giunto sara' fatto nel DO.
        //se l'appl e' in config sicuramente i giunti sono in idle e quindi non c'e' ninete da aggiornare
        s_eo_appTheDB_UpdateMototStatusPtr(mId, frame, runmode);
        
        return(eores_OK);
    }
    else
    {
        // first joint
        res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canLoc, &jId);
        if(eores_OK != res)
        {
            s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 25, frame, canPort);
            return(eores_OK);
        }
        
        jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
        if(NULL == jstatus)
        {
            s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 26, frame, canPort);
            return(eores_OK);
        }

    
        // set control mode status
        res = s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode((icubCanProto_controlmode_t) frame->data[1], &eomc_controlmode);
        if(eores_OK != res)
        {
            s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 27, frame, canPort);
            return(eores_OK);
        }
 

        if(eomc_controlmode_hwFault == eomc_controlmode)
        {        
            des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_hwfault2);
            des.par16 = (frame->data[1] << 8) | eomc_controlmode;
            des.sourceaddress = jId;
            des.sourcedevice = eo_errman_sourcedevice_localboard;
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &des);               
        }
    
        jstatus->basic.controlmodestatus = eomc_controlmode;
             
        
        
        //update motor status flag
        s_eo_appTheDB_UpdateMototStatusPtr(mId, frame, runmode);
             
    
        // second joint
        canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_second;
    
        res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canLoc, &jId);
        if(eores_OK != res)
        {
            s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 28, frame, canPort);
            return(eores_OK);
        }
        
        //get mId
        res = eo_appTheDB_GetMotorId_ByMotorCanLocation(eo_appTheDB_GetHandle(), canLoc, &mId);
        if(eores_OK != res)
        {
            s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 29, frame, canPort);
            return(eores_OK);
        }


        jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
        if(NULL == jstatus)
        {
            s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 30, frame, canPort);
            return(eores_OK);
        }

    
        // set control mode status
        res = s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode((icubCanProto_controlmode_t) frame->data[3], &eomc_controlmode);
        if(eores_OK != res)
        {
            s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 31, frame, canPort);
            return(eores_OK);
        }
        
        if(eomc_controlmode_hwFault == eomc_controlmode)
        {        
            des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_hwfault3);
            des.par16 = (frame->data[1] << 8) | eomc_controlmode;
            des.sourceaddress = jId;
            des.sourcedevice = eo_errman_sourcedevice_localboard;
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &des);               
        }
        
        jstatus->basic.controlmodestatus = eomc_controlmode;
        
        
        // update motor status flag
        s_eo_appTheDB_UpdateMototStatusPtr(mId, frame, runmode);
    
        return(eores_OK);
    }

}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__current(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_jointOrMotorCanLocation_t        canLoc = {0};

    
    // set position about the first motor in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_first;    
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__current(p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 32, frame, canPort);
        s_eo_icubCanProto_mb_send_up_canframe(frame, canPort);
        return(eores_OK);
    }


    // set position about the second motor in board
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_second;

    res = s_eo_icubCanProto_parser_per_mb_cmd__current(p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 33, frame, canPort);
        return(eores_OK);
    }    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__overflow(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__print(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    //Invalid CAN print frame
    /*
    if ((frame->data[0] != 0x06) && (frame->data[0] != 0x86))
        return eores_NOK_generic;
    */
    eOresult_t res;
    eOappTheDB_jointOrMotorCanLocation_t canLoc = {0};

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_first;
        
    res = s_eo_icubCanProto_parser_per_mb_cmd__print (p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 34, frame, canPort);
        return(eores_OK);
    } 
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__velocity(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_jointOrMotorCanLocation_t        canLoc = {0};

    
    // set position about the first motor in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_first;    
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__velocity( p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 35, frame, canPort);
        return(eores_OK);
    }

    // set position about the second joint in board
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_second;

    res = s_eo_icubCanProto_parser_per_mb_cmd__velocity( p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 36, frame, canPort);
        return(eores_OK);
    }
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidError(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_jointOrMotorCanLocation_t        canLoc = {0};

    
    // set position about the first motor in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_first;    
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__pidError(p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 37, frame, canPort);
        return(eores_OK);
    }
    
    // set position about the second joint in board
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_second;

    res = s_eo_icubCanProto_parser_per_mb_cmd__pidError( p, frame, canLoc);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 38, frame, canPort);
        return(eores_OK);
    }
    return(eores_OK);

}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__debug(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__motorPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__motorSpeed(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__additionalStatus(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{

    eOresult_t                                  res;
    eOappTheDB_jointOrMotorCanLocation_t        canLoc = {0};
    eOmc_joint_status_t                         *jstatus = NULL;
    eOmc_jointId_t                              jId;

    
    // first motor in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_first;    
    
    //first joint
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 39, frame, canPort);
        return(eores_OK);
    }
        
    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 40, frame, canPort);
        return(eores_OK);
    }
    
    //note i can use dynamic cast because both param have size equal to u8
    res = s_eo_icubCanProto_translate_icubCanProtoInteractionMode2eOmcInteractionMode((icubCanProto_interactionmode_t)(frame->data[0] & 0x0f), (eOmc_interactionmode_t *)&jstatus->interactionmodestatus);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 41, frame, canPort);
        return(eores_OK);
    }


    // second joint in board
    canLoc.indexinsidecanboard = eo_icubCanProto_jm_index_second;

    //first joint
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canLoc, &jId);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 42, frame, canPort);
        return(eores_OK);
    }
        
    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 43, frame, canPort);
        return(eores_OK);
    }
    
    res = s_eo_icubCanProto_translate_icubCanProtoInteractionMode2eOmcInteractionMode((icubCanProto_interactionmode_t)((frame->data[0] & 0xf0) >>4), (eOmc_interactionmode_t *)&jstatus->interactionmodestatus);
    if(eores_OK != res)
    {
        s_eo_icubCanProto_mb_send_runtime_error_diagnostics(1, 44, frame, canPort);
        return(eores_OK);
    }

    return(eores_OK);
}




extern eOresult_t eo_icubCanProto_former_per_mb_cmd__emsto2foc_desiredcurrent(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    int16_t *pwmList = (int16_t*)val_ptr;
   
    canFrame->id = ICUBCANPROTO_PER_MC_CREATE_ID(ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;
    *((uint16_t*)(&canFrame->data[0])) = pwmList[0];
    *((uint16_t*)(&canFrame->data[2])) = pwmList[1];
    *((uint16_t*)(&canFrame->data[4])) = pwmList[2];
    *((uint16_t*)(&canFrame->data[6])) = pwmList[3];
    
    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__position(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc)
{
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
	EOappMeasConv                           *appMeasConv_ptr = NULL;
	icubCanProto_position_t                 pos_icubCanProtValue;
    eOmc_joint_status_t                     *jstatus = NULL;

    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canloc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        return(eores_NOK_generic); //error
    }

	appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());

    
    if(eo_icubCanProto_jm_index_first == canloc.indexinsidecanboard)
	{
        pos_icubCanProtValue = *((icubCanProto_position_t*)&(frame->data[0])); 
    }
    else
    {
        pos_icubCanProtValue = *((icubCanProto_position_t*)&(frame->data[4])); 
    }

    jstatus->basic.jnt_position = eo_appMeasConv_jntPosition_E2I(appMeasConv_ptr, jId, pos_icubCanProtValue); 
    
    
    return(eores_OK);
}


static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__pidVal(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc)
{
#if 1 
    
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
    eOmc_joint_status_t                     *jstatus = NULL;
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canloc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        return(eores_NOK_generic); //error
    }
   
    if(eo_icubCanProto_jm_index_first == canloc.indexinsidecanboard)
	{
        jstatus->ofpid.output = *((uint16_t*)&(frame->data[0]));
    }
    else
    {
        jstatus->ofpid.output = *((uint16_t*)&(frame->data[2]));
    }

    return(eores_OK);
    
#else
    
    // marco.accame on 02apr15: this is the ways it should be after we have changed the type of jstatus->ofpid.output
    // ... and if we consider the content of the can frame as a signed int int16_t
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
    eOmc_joint_status_t                     *jstatus = NULL;
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canloc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        return(eores_NOK_generic); //error
    }
   
    int16_t value = 0;
    if(eo_icubCanProto_jm_index_first == canloc.indexinsidecanboard)
	{
        value = *((int16_t*)&(frame->data[0]));
    }
    else
    {
        value = *((int16_t*)&(frame->data[2]));
    }
    
    jstatus->ofpid.output = value;

    return(eores_OK);    
    
#endif
}

static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__current(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc)
{
    eOresult_t res;
    eOmc_motorId_t   		                mId;
    eOmc_motor_status_t                     *mstatus = NULL;
    
    res = eo_appTheDB_GetMotorId_ByMotorCanLocation(eo_appTheDB_GetHandle(), canloc, &mId);
    if(eores_OK != res)
    {
        return(res);
    }

    mstatus = eo_protocolwrapper_GetMotorStatus(eo_protocolwrapper_GetHandle(), mId);
    if(NULL == mstatus)
    {
        return(eores_NOK_generic); //error
    }      
    
    if(eo_icubCanProto_jm_index_first == canloc.indexinsidecanboard)
    {
        mstatus->basic.mot_current = *((uint16_t*)&(frame->data[0]));
    }
    else
    {
        mstatus->basic.mot_current = *((uint16_t*)&(frame->data[2]));
    }
    return(eores_OK);
}


static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__velocity(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc)
{
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
    eOmc_joint_status_t                     *jstatus = NULL;
	EOappMeasConv                           *appMeasConv_ptr = NULL;
	icubCanProto_velocity_t                 vel_icubCanProtValue;
    icubCanProto_acceleration_t             acc_icubCanProtValue;
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canloc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        return(eores_NOK_generic); //error
    }
    
	//get app measures converter handle
	appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
    
    //get measures from can frame
    if(eo_icubCanProto_jm_index_first == canloc.indexinsidecanboard)
	{
        vel_icubCanProtValue = *((icubCanProto_velocity_t*)&(frame->data[0]));
        acc_icubCanProtValue = *((icubCanProto_velocity_t*)&(frame->data[2]));
    }
    else
    {
        vel_icubCanProtValue = *((icubCanProto_velocity_t*)&(frame->data[4]));
        acc_icubCanProtValue = *((icubCanProto_velocity_t*)&(frame->data[6]));
    }
    
    //convert measure for icub world and set values in jstatus (nv mem)
    vel_icubCanProtValue = (vel_icubCanProtValue*1000) >> eo_appMeasConv_hid_GetVelEstimShift(appMeasConv_ptr, jId);
	jstatus->basic.jnt_velocity = eo_appMeasConv_jntVelocity_E2I(appMeasConv_ptr, jId, vel_icubCanProtValue);
    
    acc_icubCanProtValue = (acc_icubCanProtValue * 1000000) >> (eo_appMeasConv_hid_GetVelEstimShift(appMeasConv_ptr, jId) + eo_appMeasConv_hid_GetAccEstimShift(appMeasConv_ptr, jId));
    jstatus->basic.jnt_acceleration = eo_appMeasConv_jntAcceleration_E2I(appMeasConv_ptr, jId, acc_icubCanProtValue);
    
    return(eores_OK);
}



static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__pidError(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc)
{
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
    eOmc_joint_status_t                     *jstatus = NULL;
    uint16_t                                pidpos_error, pidtrq_error;
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canloc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatus = eo_protocolwrapper_GetJointStatus(eo_protocolwrapper_GetHandle(), jId);
    if(NULL == jstatus)
    {
        return(eores_NOK_generic); //error
    }
    
    //get measures from can frame
    if(eo_icubCanProto_jm_index_first == canloc.indexinsidecanboard)
	{
        pidpos_error = *((uint16_t*)&(frame->data[0]));
        pidtrq_error = *((uint16_t*)&(frame->data[4]));
    }
    else
    {
        pidpos_error = *((uint16_t*)&(frame->data[2]));
        pidtrq_error = *((uint16_t*)&(frame->data[6]));
    }

    
    if(eomc_controlmode_torque == jstatus->basic.controlmodestatus)
    {
        jstatus->ofpid.error = pidtrq_error;
    }
    else
    {
         jstatus->ofpid.error = pidpos_error;
    }
    
    return(eores_OK);
}

static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__print (EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t canloc)
{    
    eOerrmanDescriptor_t errdes = {0};
    errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_canprint);
    errdes.par16                = frame->size;
    errdes.par64                = eo_common_canframe_data2u64((eOcanframe_t*)frame);
    errdes.sourcedevice         = (eOcanport1 == canloc.emscanport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
    errdes.sourceaddress        = canloc.addr;                
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &errdes);             
    
    return eores_OK;
}

// static eOresult_t s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode_old(icubCanProto_controlmode_t icubcanProto_controlmode, eOmc_jointId_t jId,
//                                                                                           uint8_t hw_error_flags, eOmc_controlmode_t *eomc_controlmode)
// {
//      eOmc_joint_commands_t          *jcmds_ptr;
//      eOresult_t                      res;
//     
//     // get joint can location
//     res = eo_appTheDB_GetJointCommandsPtr(eo_appTheDB_GetHandle(), jId,  &jcmds_ptr);
//     if(eores_OK != res)
//     {
//         return(res);
//     }
//     
//     if(hw_error_flags)
//     {
//         *eomc_controlmode = eomc_controlmode_hwFault;
//         return(eores_OK);
//     }

//     switch(icubcanProto_controlmode)
//     {
//         case icubCanProto_controlmode_idle:
//         {
//             *eomc_controlmode = eomc_controlmode_idle;
//         }break;

//         case icubCanProto_controlmode_position:
//         case icubCanProto_controlmode_impedance_pos:
//         {
//             if(eomc_controlmode_cmd_mixed == jcmds_ptr->controlmode ) 
//             {
//                 *eomc_controlmode = eomc_controlmode_mixed; 
//             }
//             else if(eomc_controlmode_cmd_direct == jcmds_ptr->controlmode)
//             {
//                 *eomc_controlmode = eomc_controlmode_direct; 
//             }
//             else
//             {
//                 *eomc_controlmode = eomc_controlmode_position;
//             }
//         }break;

//         case icubCanProto_controlmode_velocity:
//         case icubCanProto_controlmode_impedance_vel:
//         {
//             if(eomc_controlmode_cmd_mixed == jcmds_ptr->controlmode ) 
//             {
//                 *eomc_controlmode = eomc_controlmode_mixed; 
//             }
//             else
//             {
//                 *eomc_controlmode = eomc_controlmode_velocity;
//             }
//         }break;
//         case icubCanProto_controlmode_torque:
//         {
//             *eomc_controlmode = eomc_controlmode_torque;
//         }break;

// //         case icubCanProto_controlmode_impedance_pos:
// //         {
// //             *eomc_controlmode = eomc_controlmode_position;
// //         }break;

// //         case icubCanProto_controlmode_impedance_vel:
// //         {
// //             *eomc_controlmode = eomc_controlmode_velocity;
// //         }break;

//         case  icubCanProto_controlmode_calibration:
//         {
//             *eomc_controlmode = eomc_controlmode_calib;
//         }break;
//         
//         case icubCanProto_controlmode_openloop:
//         {
//             *eomc_controlmode = eomc_controlmode_openloop;
//         }break;
//         
//         default:
//         {
//             return(eores_NOK_generic);
//         }break;
//     }
//     return(eores_OK);
// }


static eOresult_t s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode(icubCanProto_controlmode_t icubcanProto_controlmode,
                                                                                      eOmc_controlmode_t *eomc_controlmode)
{
    eOresult_t res = eores_OK;

    switch(icubcanProto_controlmode)
    {
        case icubCanProto_controlmode_idle:
        {
            *eomc_controlmode = eomc_controlmode_idle;
        } break;
        
        case icubCanProto_controlmode_position:
        {
            *eomc_controlmode = eomc_controlmode_position;
        } break;
        
        case icubCanProto_controlmode_velocity:
        {
            *eomc_controlmode = eomc_controlmode_velocity;
        } break;
        
        case icubCanProto_controlmode_torque:
        {
            *eomc_controlmode = eomc_controlmode_torque;
        } break;
        
        case icubCanProto_controlmode_impedance_pos:
        {
            *eomc_controlmode = eomc_controlmode_impedance_pos;
        } break;

        case icubCanProto_controlmode_impedance_vel:
        {
            *eomc_controlmode = eomc_controlmode_impedance_vel;
        } break;

        case icubCanProto_controlmode_current:
        {
            *eomc_controlmode = eomc_controlmode_current;
        } break;
        case icubCanProto_controlmode_mixed:
        {
            *eomc_controlmode = eomc_controlmode_mixed;
        } break;
        
        case icubCanProto_controlmode_direct:
        {
            *eomc_controlmode = eomc_controlmode_direct;
        }break;

        case icubCanProto_controlmode_openloop:
        {
            *eomc_controlmode = eomc_controlmode_openloop;
        } break;
        
        case  icubCanProto_controlmode_calibration:
        {
            *eomc_controlmode = eomc_controlmode_calib;
        } break;
        
        case icubCanProto_controlmode_hwFault:
        {
            *eomc_controlmode = eomc_controlmode_hwFault;
        } break;
        
        case icubCanProto_controlmode_notConfigured:
        {
            *eomc_controlmode = eomc_controlmode_notConfigured;
        } break;
        
        case icubCanProto_controlmode_configured:
        {
            *eomc_controlmode = eomc_controlmode_configured;
        } break;
        
        case icubCanProto_controlmode_unknownError:
        {
            *eomc_controlmode = eomc_controlmode_unknownError;
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
    }
    
    return(res);
}



static eOresult_t s_eo_icubCanProto_translate_icubCanProtoInteractionMode2eOmcInteractionMode(icubCanProto_interactionmode_t icubcanProto_intermode,
                                                                                      eOmc_interactionmode_t *eomc_intermode)
{
    eOresult_t res = eores_OK;
    
    switch(icubcanProto_intermode)
    {
        case icubCanProto_interactionmode_stiff:
        {
            *eomc_intermode = eOmc_interactionmode_stiff;
        } break;
        
        case icubCanProto_interactionmode_compliant:
        {
            *eomc_intermode = eOmc_interactionmode_compliant;
        } break;
    
        default:
        {
            res = eores_NOK_generic;
        } break;
    }
    
    return(res);
}

static eOresult_t s_eo_appTheDB_UpdateMototStatusPtr(eOmc_motorId_t mId, eOcanframe_t *frame, eOmn_appl_runMode_t runmode)
{
    eOmc_motor_status_t     *mstatus = NULL;
    uint8_t                 flag0 = frame->data[0];
    uint8_t                 flag5 = frame->data[5];
    uint8_t                 flag4 = frame->data[4];
    EOTheEMSdiagnostics_t*  dgn_ptr = NULL;
    

    mstatus = eo_protocolwrapper_GetMotorStatus(eo_protocolwrapper_GetHandle(), mId);
    if(NULL == mstatus)
    {
        return(eores_NOK_generic); //error
    }  
    
    #warning --> marco.accame: if we use motorstatus->filler04[0] to vehiculate errors then we must send diagnostics into proper rop.
    
    mstatus->filler04[0] = 0;
    dgn_ptr = eo_theEMSdgn_GetHandle();
    
    if(EO_COMMON_CHECK_FLAG(flag0, ICUBCANPROTO_PER_MC_STATUS_FLAG_UNDERVOLTAGE)) //undervoltage
    {
        mstatus->filler04[0] |= DGN_MOTOR_FAULT_UNDERVOLTAGE;
    }
    
    if(EO_COMMON_CHECK_FLAG(flag0, ICUBCANPROTO_PER_MC_STATUS_FLAG_OVERVOLTAGE)) //overvoltage    
    {
        mstatus->filler04[0] |= DGN_MOTOR_FAULT_OVERVOLTAGE;
    }
    
    
    if(eo_theEMSdgn_IsExtFault2Signal(dgn_ptr))
    {
        if(EO_COMMON_CHECK_FLAG(flag0, ICUBCANPROTO_PER_MC_STATUS_FLAG_EXTERNAL))//external
        {
            mstatus->filler04[0] |= DGN_MOTOR_FAULT_EXTERNAL;
        }
    }
    
    if(EO_COMMON_CHECK_FLAG(flag0, ICUBCANPROTO_PER_MC_STATUS_FLAG_OVERCURRENT)) //over current
    {
        mstatus->filler04[0] |= DGN_MOTOR_FAULT_OVERCURRENT;
    }
    
    if(EO_COMMON_CHECK_FLAG(flag5, ICUBCANPROTO_PER_MC_STATUS_FLAG_I2TFAILURE))
    {
        mstatus->filler04[0] |= DGN_MOTOR_FAULT_I2TFAILURE;
    }

    
    if(EO_COMMON_CHECK_FLAG(flag4, ICUBCANPROTO_PER_MC_STATUS_FLAG_CANRECWARNING)) //can receive warning   
    {
        mstatus->filler04[0] |= DGN_MOTOR_FAULT_CANRECWARNING;
    }
    
    if(EO_COMMON_CHECK_FLAG(flag4, ICUBCANPROTO_PER_MC_STATUS_FLAG_CANRECERROR)) //can receive error   
    {
        mstatus->filler04[0] |= DGN_MOTOR_FAULT_CANRECERROR;
    }
    
    
    if(EO_COMMON_CHECK_FLAG(flag4, ICUBCANPROTO_PER_MC_STATUS_FLAG_CANRECHWOVERRUN)) //can hw over-run   
    {
        mstatus->filler04[0] |= DGN_MOTOR_FAULT_CANRECHWOVERRUN;
    }
        
    if(0 != mstatus->filler04[0])
    {
        eo_theEMSdgn_UpdateMotorStFlags(dgn_ptr, mId, mstatus->filler04[0]);
        eo_theEMSdgn_Signalerror(dgn_ptr, eodgn_nvidbdoor_motorstatus , 0);
        eo_theEMSdgn_ClearMotorStFlags(dgn_ptr);
    }
    
    return(eores_OK);
    
}



static void s_eo_icubCanProto_mb_send_runtime_error_diagnostics(uint8_t parser, uint64_t par64, eOcanframe_t *frame, eOcanport_t port)
{
    eOerrmanDescriptor_t des = {0};  
    
    if(1 == parser)
    {
        des.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_rxfromwrongboard);
        des.par16                = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64                = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourcedevice         = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        des.sourceaddress        = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);  
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des); 
    }
    else
    {
        des.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_runtimeerror);
        des.par16 = 0x1111;
        des.par64 = 0x2200000000000000 + par64;
        des.sourceaddress = 0;
        des.sourcedevice = eo_errman_sourcedevice_localboard;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &des);        
    }       
}

static eObool_t s_eo_icubCanProto_is_from_unused2foc_in_eb5(eOcanframe_t *frame, eOcanport_t canPortRX)
{    
    const eOprotBRD_t eb5board = 4;
    eOprotBRD_t localboard = eoprot_board_local_get();
    
    if(localboard != eb5board)
    {
        return(eobool_false);
    }
    
    if( (eOcanport1 == canPortRX) &&  (2 == eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id)) )
    {
        return(eobool_true);
    }
    else
    {
        return(eobool_false);
    }    
    
}

static void s_eo_icubCanProto_mb_send_up_canframe(eOcanframe_t *frame, eOcanport_t canport)
{
    eOerrmanDescriptor_t des = {0};
    des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    des.par16 = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
    des.par64 = eo_common_canframe_data2u64(frame);
    des.sourcedevice = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
    des.sourceaddress = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);

    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &des);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



