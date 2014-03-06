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

//#include "EoMotionControl.h"
//#include "EOnv_hid.h"
//#include "eOcfg_nvsEP_mc.h"

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
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__position(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr);
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__pidVal(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr);
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__current(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr);
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__velocity(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr);
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__pidError(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr);
static eOresult_t s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode(icubCanProto_controlmode_t icubcanProto_controlmode, eOmc_controlmode_t *eomc_controlmode);
static eOresult_t s_eo_appTheDB_UpdateMototStatusPtr(eOmc_motorId_t mId, eOcanframe_t *frame, eOmn_appl_runMode_t runmode);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern eOresult_t eo_icubCanProto_parser_pol_mb_unexpected_cmd(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_NOK_unsupported);
}


extern eOresult_t eo_icubCanProto_parser_test(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{

//     eOresult_t res;
//     static eOmc_joint_status_t j;
//     eo_emsCanNetTopo_jointOrMotorCanLocation_t canLoc;
//     eOmc_jointId_t jId;
//     eOmc_motorId_t mId;
//     void *memRef1, *memRef2, *pidVel_NVptr;
//     eOmc_PID_t pidVel, pidPos1, pidPos2;
//     eOmc_joint_config_t /*jc1,*/ jc2;
//     eOmc_motor_config_t  mc;
//      
//     eo_icubCanProto_canBoardAddress_t boardAddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
// //DEBUG_PIN4_OFF;

// //      res = eo_emsCanNetTopo_GetMotorBoardNV_Status_ByCanLocation(p->emsCanNetTopo_ptr, canPort, boardAddr,
// //                                                       eo_emsCanNetTopo_jm_index_first, &nvt_ptr);
//     canLoc.emscanport = canPort;
//     canLoc.canaddr = boardAddr;
//     canLoc.jm_idInBoard = 0;
//     res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jId);
//     if(eores_OK != res)
//     {
//         return(res);
//     }

// //    res = eo_appTheNVmapRef_GetJointNVMemoryRef_test(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jconfig, &memRef1);
// //    if(eores_OK != res)
// //    {
// //        return(res);
// //    }


//     res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jconfig, &memRef2);
//     if(eores_OK != res)
//     {
//         return(res);
//     }

//     if(memRef1 == memRef2)
//     {
//         j.basic.velocity = 1;
//     }
//     else
//     {
//         j.basic.velocity = 2;
//     }
//     j=j;
// //    memcpy(&jc1, memRef1, sizeof(eOmc_joint_config_t));  memref1 uset for test

//     memcpy(&jc2, memRef2, sizeof(eOmc_joint_config_t));
// //DEBUG_PIN3_OFF;

// //pidPos
//     res = eo_appTheNVmapRef_GetJointNVMemoryRef_test(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jconfig__pidposition, &memRef1);
//     if(eores_OK != res)
//     {
//         return(res);
//     }


//     res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jconfig__pidposition, &memRef2);
//     if(eores_OK != res)
//     {
//         return(res);
//     }

//     memcpy(&pidPos1, memRef1, sizeof(eOmc_PID_t)); 
//     memcpy(&pidPos2, memRef2, sizeof(eOmc_PID_t)); 



//     //pidvel
//     res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jconfig__pidvelocity, &pidVel_NVptr);
//     memcpy(&pidVel,pidVel_NVptr, sizeof(eOmc_PID_t)); 

//     pidVel.kd = 0x1010;




// /*************************************************************************************/
// /*              motor                      */

//     res = eo_emsCanNetTopo_GetMotorId_ByMotorCanLocation(p->emsCanNetTopo_ptr, &canLoc, &mId);
//     if(eores_OK != res)
//     {
//         return(res);
//     }

//     res = eo_appTheNVmapRef_GetMotorNVMemoryRef(eo_appTheNVmapRef_GetHandle(), mId, motorNVindex_mconfig, &memRef1);
//     if(eores_OK != res)
//     {
//         return(res);
//     }
//     memcpy(&mc, memRef1, sizeof(eOmc_motor_config_t));

// //DEBUG_PIN3_OFF;
// //    nvJoint_ptr->jm_idInBoard.position = frame->data[1];
// //    nvJoint_ptr->jm_idInBoard.velocity = frame->data[2]; 
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_unexpected_cmd(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
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
        }break;

        case icubCanProto_calibration_type1_abs_sens_analog:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type1.position;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type1.velocity;
            memset(&canFrame->data[6], 0, 2); //pad with 0          
        }break;

        case icubCanProto_calibration_type2_hard_stops_diff:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type2.pwmlimit;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type2.velocity;
            memset(&canFrame->data[6], 0, 2); //pad with 0          
        }break;


        case icubCanProto_calibration_type3_abs_sens_digital:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type3.position;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type3.velocity;
            *((uint16_t*)(&canFrame->data[6])) = calib_ptr->params.type3.offset;        
        }break;

        case icubCanProto_calibration_type4_abs_and_incremental:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type4.position;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type4.velocity;
            *((uint16_t*)(&canFrame->data[6])) = calib_ptr->params.type4.maxencoder;        
        }break;

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
    eOresult_t                              res;
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
//    eOmc_controlmode_t                      *jcmdcontrolmode_ptr;
    eOmc_joint_status_t                     *jstatus_ptr;

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId, &jstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

//     res = eo_appTheDB_GetJointCmdControlmodePtr(eo_appTheDB_GetHandle(), jId,  &jcmdcontrolmode_ptr);
//     if(eores_OK != res)
//     {
//         return(res);
//     }

//     *jcmdcontrolmode_ptr = ((eOmc_controlmode_t)frame->data[1]);
    jstatus_ptr->basic.controlmodestatus = ((eOmc_controlmode_t)frame->data[1]);
    
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
    eOresult_t                              res;
    eOmc_jointId_t                          jId;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOmc_joint_status_t                     *jstatus_ptr;
    

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinboard = eo_icubCanProto_hid_getjmIndexInBOardFromFrame(frame);
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(),  jId,  &jstatus_ptr);
    
    if(eomc_motionmonitorstatus_notmonitored == jstatus_ptr->basic.motionmonitorstatus)
    {
        //pc104 isn't interested in motion monitoring
        return(eores_OK);
    }
    jstatus_ptr->basic.motionmonitorstatus = (eOmc_motionmonitorstatus_t)frame->data[1];

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
    *((icubCanProto_torque_t*)(&canFrame->data[1])) = torque_setpoint_ptr->value;

    /* Note: the firware and icubcanproto expect torque value on 32bit. 
       Currently icubInterface and new eo-proto use 16 bits for torque */
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDesiredTorque(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
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
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMinPosition(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
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
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMaxPosition(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
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
    *((uint16_t*)(&canFrame->data[1])) = torque_pid_ptr->kp;
    *((uint16_t*)(&canFrame->data[3])) = torque_pid_ptr->ki;
    *((uint16_t*)(&canFrame->data[5])) = torque_pid_ptr->kd;
    canFrame->data[7] = torque_pid_ptr->scale;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorquePid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
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
    *((uint16_t*)(&canFrame->data[1])) = torque_pid_ptr->offset;
    *((uint16_t*)(&canFrame->data[3])) = torque_pid_ptr->limitonoutput;
    *((uint16_t*)(&canFrame->data[5])) = torque_pid_ptr->limitonintegral;


    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorquePidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
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
    *((uint16_t*)(&canFrame->data[1])) = pos_pid_ptr->kp;
    *((uint16_t*)(&canFrame->data[3])) = pos_pid_ptr->ki;
    *((uint16_t*)(&canFrame->data[5])) = pos_pid_ptr->kd;
    canFrame->data[7] = pos_pid_ptr->scale;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
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
    *((uint16_t*)(&canFrame->data[1])) = pos_pid_ptr->offset;
    *((uint16_t*)(&canFrame->data[3])) = pos_pid_ptr->limitonoutput;
    *((uint16_t*)(&canFrame->data[5])) = pos_pid_ptr->limitonintegral;
    canFrame->data[7] = 0; //add this to clear values becaiuse len is 8 and not 7.

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
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
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceParams(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
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
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceOffset(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getFirmwareVersion(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res;
    eOappTheDB_canBoardCanLocation_t        canLoc;
    eObrd_boardId_t                         bid;
    char                                    str[120];
    uint32_t                                canBoardsReady;
    EOappTheDB                              *db = eo_appTheDB_GetHandle();

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id); 
    
    res = eo_appTheDB_GetCanBoardId_ByCanLocation(db, &canLoc, &bid); 
    if(eores_OK != res)
    {
        return(res);
    }
     
    if(1 != frame->data[7])
    {
        uint16_t buildNum = *((uint16_t*)&frame->data[2]);
        snprintf(str, sizeof(str), "getfwVer bId%d: bType=%d fw_ver=0x%x build=%d proto=%d.%d check=%d", bid, frame->data[1], buildNum, frame->data[4], frame->data[5], frame->data[6],frame->data[7]);   
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "parse can cmd", str);
    }
    
    
    eo_appTheDB_setCanBoardReady(db, &canLoc);
    
    if(eo_appTheDB_areConnectedCanBoardsReady(db,&canBoardsReady))
    {
        eo_emsapplBody_setCanBoardsAreReady(eo_emsapplBody_GetHandle());
        eo_emsapplBody_sendConfig2canboards(eo_emsapplBody_GetHandle());
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

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentPid(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *cur_pid_ptr = (eOmc_PID_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MC_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.s.jm_indexInBoard&0x1)  <<7) | ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    *((uint16_t*)(&canFrame->data[1])) = cur_pid_ptr->kp;
    *((uint16_t*)(&canFrame->data[3])) = cur_pid_ptr->ki;
    *((uint16_t*)(&canFrame->data[5])) = cur_pid_ptr->kd;

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




//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__2foc(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    /* 2foc sends: current (2 bytes), velocity(2 bytes), position(4 bytes)*/
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOmc_motorId_t   		                mId;
    eOmc_motor_status_t                     *mstatus_ptr;


    // set position about the first motor in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinboard = eo_icubCanProto_jm_index_first;    
    
    res = eo_appTheDB_GetMotorId_ByMotorCanLocation(eo_appTheDB_GetHandle(), &canLoc, &mId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheDB_GetMotorStatusPtr(eo_appTheDB_GetHandle(), mId,  &mstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    #warning VALE--> before use current, vel and pos of 2foc be sure to had configured 2foc periodic msg content!!!
    mstatus_ptr->basic.current = *((int16_t*)&(frame->data[0]));
    mstatus_ptr->basic.velocity = *((int16_t*)&(frame->data[2]));
    mstatus_ptr->basic.position = *((int32_t*)&(frame->data[4]));
     
    
    
    /* TAG_ALE */
#ifdef USE_2FOC_FAST_ENCODER
    if ((frame->id & 0xFFFFFF0F) == 0x104)
    {
        eo_emsController_ReadSpeed(mId, SPEED_2FOC_TO_EMS(((int16_t*)frame->data)[1]));
    }
#endif

    
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__position(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;

    
    // set position about the first joint in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinboard = eo_icubCanProto_jm_index_first;
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__position(p, frame, &canLoc);
    if(eores_OK != res)
    {
        return(res);
    }


    // set position about the second joint in board
    canLoc.indexinboard = eo_icubCanProto_jm_index_second;
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__position(p, frame, &canLoc);

    return(res);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidVal(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;

    
    // set position about the first joint in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinboard = eo_icubCanProto_jm_index_first;    
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__pidVal(p, frame, &canLoc);
    if(eores_OK != res)
    {
        return(res);
    }

    // set position about the second joint in board
    canLoc.indexinboard = eo_icubCanProto_jm_index_second;
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__pidVal(p, frame, &canLoc);
    
    return(res);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__status(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
    eOmc_motorId_t   		                mId;
    eOmc_joint_status_t                     *jstatus_ptr;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOmc_controlmode_t                      eomc_controlmode;
    eOmn_appl_runMode_t                     runmode; 
   

    
    // set position about the first joint in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinboard = eo_icubCanProto_jm_index_first;    

    
    runmode = eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
    
    if(applrunMode__default == runmode)
    {
        return(eores_OK); //ignore error
    }
    
    //get mId
    res = eo_appTheDB_GetMotorId_ByMotorCanLocation(eo_appTheDB_GetHandle(), &canLoc, &mId);
    if(eores_OK != res)
    {
        return(res);
    }
   
    if(applrunMode__2foc == runmode)
    {
        //in this case one can addr =>one motor!!
        s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode((icubCanProto_controlmode_t) frame->data[1],
                                                                            &eomc_controlmode);
        eo_emsController_ReadMotorstatus(mId, frame->data[0], frame->data[4], eomc_controlmode);
        //l'aggiornamento delle nv del giunto sara' fatto nel DO.
        //se l'appl e' in config sicuramente i giunti sono in idle e quindi non c'e' ninete da aggiornare
        s_eo_appTheDB_UpdateMototStatusPtr(mId, frame, runmode);
        
        return(eores_OK);
    }
    else
    {
        //forst joint
        res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), &canLoc, &jId);
        if(eores_OK != res)
        {
            return(res);
        }
        
        res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId,  &jstatus_ptr);
        if(eores_OK != res)
        {
            return(res);
        }

    
        //set control mode status
        s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode((icubCanProto_controlmode_t) frame->data[1],
                                                                             &eomc_controlmode);

        jstatus_ptr->basic.controlmodestatus = eomc_controlmode;
        
        //update motor status flag
        s_eo_appTheDB_UpdateMototStatusPtr(mId, frame, runmode);
        
        

        
    
        // second joint
        canLoc.indexinboard = eo_icubCanProto_jm_index_second;
    
        res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), &canLoc, &jId);
        if(eores_OK != res)
        {
            return(res);
        }
        
        //get mId
        res = eo_appTheDB_GetMotorId_ByMotorCanLocation(eo_appTheDB_GetHandle(), &canLoc, &mId);
        if(eores_OK != res)
        {
            return(res);
        }


        res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId,  &jstatus_ptr);
        if(eores_OK != res)
        {
            return(res);
        }

    
        //set control mode status
        s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode((icubCanProto_controlmode_t) frame->data[3],
                                                                             &eomc_controlmode);

        jstatus_ptr->basic.controlmodestatus = eomc_controlmode;
        
        
        //update motor status flag
        s_eo_appTheDB_UpdateMototStatusPtr(mId, frame, runmode);
    
        return(eores_OK);
    }

}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__current(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;

    
    // set position about the first motor in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinboard = eo_icubCanProto_jm_index_first;    
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__current(p, frame, &canLoc);
    if(eores_OK != res)
    {
        return(res);
    }


    // set position about the second motor in board
    canLoc.indexinboard = eo_icubCanProto_jm_index_second;

    res = s_eo_icubCanProto_parser_per_mb_cmd__current(p, frame, &canLoc);
    
    return(res);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__overflow(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__print(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__velocity(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;

    
    // set position about the first motor in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinboard = eo_icubCanProto_jm_index_first;    
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__velocity( p, frame, &canLoc);
    if(eores_OK != res)
    {
        return(res);
    }

    // set position about the second joint in board
    canLoc.indexinboard = eo_icubCanProto_jm_index_second;

    res = s_eo_icubCanProto_parser_per_mb_cmd__velocity( p, frame, &canLoc);

    return(res);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidError(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;

    
    // set position about the first motor in board
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.indexinboard = eo_icubCanProto_jm_index_first;    
    
    res = s_eo_icubCanProto_parser_per_mb_cmd__pidError(p, frame, &canLoc);
    if(eores_OK != res)
    {
        return(res);
    }
    
    // set position about the second joint in board
    canLoc.indexinboard = eo_icubCanProto_jm_index_second;

    res = s_eo_icubCanProto_parser_per_mb_cmd__pidError( p, frame, &canLoc);

    return(res);

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
static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__position(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr)
{
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
	EOappMeasConv                           *appMeasConv_ptr = NULL;
	icubCanProto_position_t                 pos_icubCanProtValue;
    eOmc_joint_status_t                     *jstatus_ptr;

    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canloc_ptr, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId,  &jstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

	appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());

    
    if(eo_icubCanProto_jm_index_first == canloc_ptr->indexinboard)
	{
        pos_icubCanProtValue = *((icubCanProto_position_t*)&(frame->data[0])); 
    }
    else
    {
        pos_icubCanProtValue = *((icubCanProto_position_t*)&(frame->data[4])); 
    }

    jstatus_ptr->basic.position = eo_appMeasConv_jntPosition_E2I(appMeasConv_ptr, jId, pos_icubCanProtValue); 
    
    
//    jstatus_ptr->basic.position++;
    return(eores_OK);
}


static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__pidVal(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr)
{
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
    eOmc_joint_status_t                     *jstatus_ptr;
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canloc_ptr, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId,  &jstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
   
    if(eo_icubCanProto_jm_index_first == canloc_ptr->indexinboard)
	{
        jstatus_ptr->ofpid.output = *((uint16_t*)&(frame->data[0]));
    }
    else
    {
        jstatus_ptr->ofpid.output = *((uint16_t*)&(frame->data[2]));
    }

    return(eores_OK);

}

static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__current(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr)
{
    eOresult_t res;
    eOmc_motorId_t   		                mId;
    eOmc_motor_status_t                     *mstatus_ptr;
    
    res = eo_appTheDB_GetMotorId_ByMotorCanLocation(eo_appTheDB_GetHandle(), canloc_ptr, &mId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheDB_GetMotorStatusPtr(eo_appTheDB_GetHandle(), mId,  &mstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    
    if(eo_icubCanProto_jm_index_first == canloc_ptr->indexinboard)
    {
        mstatus_ptr->basic.current = *((uint16_t*)&(frame->data[0]));
				
        #ifdef USE_2FOC_FAST_ENCODER
        if ((frame->id & 0xFFFFFF0F) == 0x104)
        {
            if(applrunMode__2foc == eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle()))
            {
                eo_emsController_ReadSpeed(mId, SPEED_2FOC_TO_EMS(((int16_t*)frame->data)[1]));
            }
        }
        #endif
    }
    else
    {
        mstatus_ptr->basic.current = *((uint16_t*)&(frame->data[2]));
    }
    return(eores_OK);
}


static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__velocity(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr)
{
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
    eOmc_joint_status_t                     *jstatus_ptr;
	EOappMeasConv                           *appMeasConv_ptr = NULL;
	icubCanProto_velocity_t                 vel_icubCanProtValue;
    icubCanProto_acceleration_t             acc_icubCanProtValue;
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canloc_ptr, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId,  &jstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    
	//get app measures converter handle
	appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
    
    //get measures from can frame
    if(eo_icubCanProto_jm_index_first == canloc_ptr->indexinboard)
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
	jstatus_ptr->basic.velocity = eo_appMeasConv_jntVelocity_E2I(appMeasConv_ptr, jId, vel_icubCanProtValue);
    
    acc_icubCanProtValue = (acc_icubCanProtValue * 1000000) >> (eo_appMeasConv_hid_GetVelEstimShift(appMeasConv_ptr, jId) + eo_appMeasConv_hid_GetAccEstimShift(appMeasConv_ptr, jId));
    jstatus_ptr->basic.acceleration = eo_appMeasConv_jntAcceleration_E2I(appMeasConv_ptr, jId, acc_icubCanProtValue);
    
    return(eores_OK);
}



static eOresult_t s_eo_icubCanProto_parser_per_mb_cmd__pidError(EOicubCanProto* p, eOcanframe_t *frame, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr)
{
    eOresult_t                              res;
    eOmc_jointId_t   		                jId;
    eOmc_joint_status_t                     *jstatus_ptr;
    uint16_t                                pidpos_error, pidtrq_error;
    
    res = eo_appTheDB_GetJointId_ByJointCanLocation(eo_appTheDB_GetHandle(), canloc_ptr, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId,  &jstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    
    //get measures from can frame
    if(eo_icubCanProto_jm_index_first == canloc_ptr->indexinboard)
	{
        pidpos_error = *((uint16_t*)&(frame->data[0]));
        pidtrq_error = *((uint16_t*)&(frame->data[4]));
    }
    else
    {
        pidpos_error = *((uint16_t*)&(frame->data[2]));
        pidtrq_error = *((uint16_t*)&(frame->data[6]));
    }

    
    if(eomc_controlmode_torque == jstatus_ptr->basic.controlmodestatus)
    {
        jstatus_ptr->ofpid.error = pidtrq_error;
    }
    else
    {
         jstatus_ptr->ofpid.error = pidpos_error;
    }
    
    return(eores_OK);
}


static eOresult_t s_eo_icubCanProto_translate_icubCanProtoControlMode2eOmcControlMode(icubCanProto_controlmode_t icubcanProto_controlmode,
                                                                                      eOmc_controlmode_t *eomc_controlmode)
{
    switch(icubcanProto_controlmode)
    {
        case icubCanProto_controlmode_idle:
        {
            *eomc_controlmode = eomc_controlmode_idle;
        }break;

        case icubCanProto_controlmode_position:
        {
            *eomc_controlmode = eomc_controlmode_position;
        }break;

        case icubCanProto_controlmode_velocity:
        {
            *eomc_controlmode = eomc_controlmode_velocity;
        }break;
        case icubCanProto_controlmode_torque:
        {
            *eomc_controlmode = eomc_controlmode_torque;
        }break;

        case icubCanProto_controlmode_impedance_pos:
        {
            *eomc_controlmode = eomc_controlmode_impedance_pos;
        }break;

        case icubCanProto_controlmode_impedance_vel:
        {
            *eomc_controlmode = eomc_controlmode_impedance_vel;
        }break;

        case  icubCanProto_controlmode_calib_abs_pos_sens:
        case  icubCanProto_controlmode_calib_hard_stops:
        case  icubCanProto_controlmode_handle_hard_stops:
        case  icubCanProto_controlmode_margin_reached:
        case  icubCanProto_controlmode_calib_abs_and_incremental:
        {
            *eomc_controlmode = eomc_controlmode_calib;
        }break;
        
        case icubCanProto_controlmode_openloop:
        {
            *eomc_controlmode = eomc_controlmode_openloop;
        }break;
        
        default:
        {
            *eomc_controlmode = eomc_controlmode_idle;
        }break;
    }
    return(eores_OK);
}


static eOresult_t s_eo_appTheDB_UpdateMototStatusPtr(eOmc_motorId_t mId, eOcanframe_t *frame, eOmn_appl_runMode_t runmode)
{
    eOmc_motor_status_t     *mstatus_ptr;
    eOresult_t              res;
    uint8_t                 flag0 = frame->data[0];
    uint8_t                 flag5 = frame->data[5];
    uint8_t                  flag4 = frame->data[4];
    EOTheEMSdiagnostics_t*  dgn_ptr = NULL;
    
    res = eo_appTheDB_GetMotorStatusPtr(eo_appTheDB_GetHandle(), mId,  &mstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    
    mstatus_ptr->filler04[0] = 0;
    dgn_ptr = eo_theEMSdgn_GetHandle();
    
    if(EO_COMMON_CHECK_FLAG(flag0, ICUBCANPROTO_PER_MC_STATUS_FLAG_UNDERVOLTAGE)) //undervoltage
    {
        mstatus_ptr->filler04[0] |= DGN_MOTOR_FAULT_UNDERVOLTAGE;
    }
    
    if(EO_COMMON_CHECK_FLAG(flag0, ICUBCANPROTO_PER_MC_STATUS_FLAG_OVERVOLTAGE)) //overvoltage    
    {
        mstatus_ptr->filler04[0] |= DGN_MOTOR_FAULT_OVERVOLTAGE;
    }
    
    
    if(eo_theEMSdgn_IsExtFault2Signal(dgn_ptr))
    {
        if(EO_COMMON_CHECK_FLAG(flag0, ICUBCANPROTO_PER_MC_STATUS_FLAG_EXTERNAL))//external
        {
            mstatus_ptr->filler04[0] |= DGN_MOTOR_FAULT_EXTERNAL;
        }
    }
    
    if(EO_COMMON_CHECK_FLAG(flag0, ICUBCANPROTO_PER_MC_STATUS_FLAG_OVERCURRENT)) //over current
    {
        mstatus_ptr->filler04[0] |= DGN_MOTOR_FAULT_OVERCURRENT;
    }
    
    if(EO_COMMON_CHECK_FLAG(flag5, ICUBCANPROTO_PER_MC_STATUS_FLAG_I2TFAILURE))
    {
        mstatus_ptr->filler04[0] |= DGN_MOTOR_FAULT_I2TFAILURE;
    }

    
    if(EO_COMMON_CHECK_FLAG(flag4, ICUBCANPROTO_PER_MC_STATUS_FLAG_CANRECWARNING)) //can receive warning   
    {
        mstatus_ptr->filler04[0] |= DGN_MOTOR_FAULT_CANRECWARNING;
    }
    
    if(EO_COMMON_CHECK_FLAG(flag4, ICUBCANPROTO_PER_MC_STATUS_FLAG_CANRECERROR)) //can receive error   
    {
        mstatus_ptr->filler04[0] |= DGN_MOTOR_FAULT_CANRECERROR;
    }
    
    
    if(EO_COMMON_CHECK_FLAG(flag4, ICUBCANPROTO_PER_MC_STATUS_FLAG_CANRECHWOVERRUN)) //can hw over-run   
    {
        mstatus_ptr->filler04[0] |= DGN_MOTOR_FAULT_CANRECHWOVERRUN;
    }
        
    
    if(0 != mstatus_ptr->filler04[0])
    {
        eo_theEMSdgn_UpdateMotorStFlags(dgn_ptr, mId, mstatus_ptr->filler04[0]);
        eo_theEMSdgn_Signalerror(dgn_ptr, eodgn_nvidbdoor_motorstatus , 0);
        eo_theEMSdgn_ClearMotorStFlags(dgn_ptr);
    }
    return(eores_OK);
    
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



