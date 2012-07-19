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

#include "EoMotionControl.h"
#include "EOnv_hid.h"
#include "EOemsController.h"

#include "EOemsCanNetworkTopology.h"
#include "EOappTheNVmapRef.h"
#include "eOcfg_nvsEP_mc.h"

//#include "hal_debugPin.h"
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
#define ICUBCANPROTO_POL_MB_ID_DEFAULT                  ((ICUBCANPROTO_CLASS_POLLING_MOTORBOARD << 8) | (0x0<<4))
#define ICUBCANPROTO_POL_MB_CREATE_ID(destBoardAddr)    ((ICUBCANPROTO_CLASS_POLLING_MOTORBOARD << 8) | (EMS_CAN_ADDR<<4) | (destBoardAddr&0xF))



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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


extern eOresult_t eo_icubCanProto_former_pol_mb_unexpected_cmd(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_NOK_unsupported);
}


extern eOresult_t eo_icubCanProto_former_test(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.axis&0x1) <<8) | ICUBCANPROTO_POL_MB_CMD__CONTROLLER_RUN;
    canFrame->data[1] = *((uint8_t*)val_ptr);
    canFrame->data[2] = *(((uint8_t*)val_ptr) +1);
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__controllerRun(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__CONTROLLER_RUN;
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__controllerIdle(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__CONTROLLER_IDLE;
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__toggleVerbose(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__calibrateEncoder(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_calibrator_t *calib_ptr;
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;

    /* 2 set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__CALIBRATE_ENCODER;

    /*3) set command's params */
    calib_ptr = (eOmc_calibrator_t *)val_ptr;
    canFrame->data[1] = calib_ptr->type;
    switch(calib_ptr->type)
    {
        case eomc_calibration_type0_hard_stops:
        {
            *((uint16_t*)(&canFrame->data[2])) = calib_ptr->params.type0.pwmlimit;
            *((uint16_t*)(&canFrame->data[4])) = calib_ptr->params.type0.velocity;
            memset(&canFrame->data[6], 0, 2); //pad with 0          
        }break;

        case eomc_calibration_type1_abs_sens_analog:
        {
            *((uint16_t*)(&canFrame->data[1])) = calib_ptr->params.type1.position;
            *((uint16_t*)(&canFrame->data[3])) = calib_ptr->params.type1.velocity;
            memset(&canFrame->data[6], 0, 2); //pad with 0          
        }break;

        case eomc_calibration_type2_hard_stops_diff :
        {
            *((uint16_t*)(&canFrame->data[1])) = calib_ptr->params.type2.pwmlimit;
            *((uint16_t*)(&canFrame->data[3])) = calib_ptr->params.type2.velocity;
            memset(&canFrame->data[6], 0, 2); //pad with 0          
        }break;


        case eomc_calibration_type3_abs_sens_digital :
        {
            *((uint16_t*)(&canFrame->data[1])) = calib_ptr->params.type3.position;
            *((uint16_t*)(&canFrame->data[3])) = calib_ptr->params.type3.velocity;
            *((uint16_t*)(&canFrame->data[5])) = calib_ptr->params.type3.offset;        
        }break;

        case eomc_calibration_type4_abs_and_incremental  :
        {
            *((uint16_t*)(&canFrame->data[1])) = calib_ptr->params.type4.position;
            *((uint16_t*)(&canFrame->data[3])) = calib_ptr->params.type4.velocity;
            *((uint16_t*)(&canFrame->data[5])) = calib_ptr->params.type4.maxencoder;        
        }break;

    }
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__enablePwmPad(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__ENABLE_PWM_PAD;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__disablePwmPad(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__DISABLE_PWM_PAD;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getControlMode(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t              res;
    eOmc_jointId_t          jId;
    void                    *nv_mem_ptr;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t  canLoc;

    canLoc.emscanport = canPort;
    canLoc.canaddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.jm_idInBoard = eo_icubCanProto_hid_getmotorAxisFromFrame(frame);
    
    res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jconfig__controlmode, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

//    memcopy(nv_mem_ptr, &frame->data[1], sizeof(eOenum08_t));
    *((eOenum08_t*)nv_mem_ptr) = frame->data[1];

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getControlMode(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__GET_CONTROL_MODE;

    /* this command hasn't params */

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__motionDone(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t        res;
    eOmc_jointId_t    jId;
    void              *nv_mem_ptr;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t  canLoc;

    canLoc.emscanport = canPort;
    canLoc.canaddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.jm_idInBoard = eo_icubCanProto_hid_getmotorAxisFromFrame(frame);
    
    res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__basic, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    if(eomc_motionmonitorstatus_notmonitored == ((eOmc_joint_status_basic_t*)nv_mem_ptr)->motionmonitorstatus)
    {
        //pc104 isn't interested in motion monitoring
        return(eores_OK);
    }
    ((eOmc_joint_status_basic_t*)nv_mem_ptr)->motionmonitorstatus = frame->data[1];

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__motionDone(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__MOTION_DONE;

    /* this command hasn't params */

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setControlMode(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOenum08_t *controlMode_ptr = (eOenum08_t *)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;

    /* 2) set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_CONTROL_MODE;

    /* 3) set command's params */
    canFrame->data[1] = *controlMode_ptr;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getAdditionalInfo(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getAdditionalInfo(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setAdditionalInfo(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setSpeedEtimShift(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_estimShift_t *estimShift_ptr = (eOmc_estimShift_t*)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;

    /* 2 set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_SPEED_ESTIM_SHIFT;

    /*3) set command's params */
    *((uint8_t*)(&canFrame->data[1])) = estimShift_ptr->estimShiftJointVel;
    *((uint8_t*)(&canFrame->data[2])) = estimShift_ptr->estimShiftJointAcc;
    *((uint8_t*)(&canFrame->data[3])) = estimShift_ptr->estimShiftMotorVel;
    *((uint8_t*)(&canFrame->data[4])) = estimShift_ptr->estimShiftMotorAcc;
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDebugParam(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDebugParam(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDebugParam(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getEncoderPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getEncoderPosition(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__positionMove(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_setpoint_t *pos_setpoint_ptr = (eOmc_setpoint_t *)val_ptr;
    /*NOTE: here i don't check is the given setpoint is a position one. i trust to it*/

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;

    /* 2 set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__POSITION_MOVE;

    /*3) set command's params */
    *((uint32_t*)(&canFrame->data[1])) = pos_setpoint_ptr->to.position.value;
    *((uint16_t*)(&canFrame->data[5])) = pos_setpoint_ptr->to.position.withvelocity;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__velocityMove(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_setpoint_t *vel_setpoint_ptr = (eOmc_setpoint_t *)val_ptr;
    /*NOTE: here i don't check is the given setpoint is a velocity one. i trust to it*/

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;

    /* 2 set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__VELOCITY_MOVE;

    /*3) set command's params */
    *((uint16_t*)(&canFrame->data[1])) = vel_setpoint_ptr->to.velocity.value;
    *((uint16_t*)(&canFrame->data[5])) = vel_setpoint_ptr->to.velocity.withacceleration;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setEncoderPosition(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDesiredTorque(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_setpoint_t *torque_setpoint_ptr = (eOmc_setpoint_t *)val_ptr;
    /*NOTE: here i don't check is the given setpoint is a velocity one. i trust to it*/

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;

    /* 2 set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_DESIRED_TORQUE;

    /*3) set command's params */
    *((uint32_t*)(&canFrame->data[1])) = torque_setpoint_ptr->to.torque.value;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDesiredTorque(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDesiredTorque(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__GET_DESIRED_TORQUE;

    /* this command hasn't params */


    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__stopTrajectory(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;

    /* 2) set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__STOP_TRAJECTORY;

    /* this command hasn't params */

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getBoardId(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getBoardId(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMinPosition(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION;
    *((uint32_t*)(&canFrame->data[1])) = *((eOmeas_position_t*)val_ptr);
    return(eores_OK);


}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMinPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMinPosition(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMaxPosition(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION;
    *((uint32_t*)(&canFrame->data[1])) = *((eOmeas_position_t*)val_ptr);
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMaxPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMaxPosition(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMaxVelocity(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
    *((uint16_t*)(&canFrame->data[1])) = *((eOmeas_velocity_t*)val_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMaxVelocity(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMaxVelocity(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentLimit(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmeas_current_t *motor_currentlimit_ptr = (eOmeas_current_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
    *((uint32_t*)(&canFrame->data[1])) = *motor_currentlimit_ptr;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setBcastPolicy(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_BCAST_POLICY;
    
    if(NULL == val_ptr)
    {
        canFrame->data[1] = ICUBCANPROTO_PER_MB_CMD_POSITION;
        canFrame->data[2] = ICUBCANPROTO_PER_MB_CMD_PID_VAL;
        canFrame->data[3] = ICUBCANPROTO_PER_MB_CMD_STATUS;
        canFrame->data[4] = ICUBCANPROTO_PER_MB_CMD_OVERFLOW;
    }
    else
    {
            #warning VALE--> bcast policy set default only!!!!
    }
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelShift(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{

   canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
   canFrame->id_type = 0; //standard id
   canFrame->frame_type = 0; //data frame
   canFrame->size = 3;
   canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_VEL_SHIFT;
   *((uint16_t*)(&canFrame->data[1])) = *((uint16_t*)val_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setOffsetAbsEncoder(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getOffsetAbsEncoder(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getOffsetAbsEncoder(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorquePid(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *torque_pid_ptr = (eOmc_PID_t *)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;

    /* 2) set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PID;

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

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePid(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorquePidLimits(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *torque_pid_ptr = (eOmc_PID_t *)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8; /*  Currently in messages.h there is a check on frame_len equal to 8, else frame is discard.
                            so i left size=8 even if correct size is 7 */

    /* 2) set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PIDLIMITS;

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

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePidLimits(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosPid(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *pos_pid_ptr = (eOmc_PID_t *)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;

    /* 2) set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_POS_PID;

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

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPid(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosPidLimits(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *pos_pid_ptr = (eOmc_PID_t *)val_ptr;

    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8; /*  Currently in messages.h there is a check on frame_len equal to 8, else frame is discard.
                            so i left size=8 even if correct size is 7 */

    /* 2) set can command */
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_POS_PIDLIMITS;

    /* 3) set command's params */
    *((uint16_t*)(&canFrame->data[1])) = pos_pid_ptr->offset;
    *((uint16_t*)(&canFrame->data[3])) = pos_pid_ptr->limitonoutput;
    *((uint16_t*)(&canFrame->data[5])) = pos_pid_ptr->limitonintegral;

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelTimeout(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_VEL_TIMEOUT;
    *((uint16_t*)(&canFrame->data[1])) = *((uint16_t*)val_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setImpedanceParams(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_impedance_t *imp_ptr = (eOmc_impedance_t *)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_PARAMS;

    *((uint16_t*)(&canFrame->data[1])) = (uint16_t)imp_ptr->stiffness;
    *((uint16_t*)(&canFrame->data[3])) = (uint16_t)imp_ptr->damping;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getImpedanceParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceParams(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setImpedanceOffset(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_impedance_t *imp_ptr = (eOmc_impedance_t *)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_OFFSET;

    *((uint16_t*)(&canFrame->data[1])) = (uint16_t)imp_ptr->offset;

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getImpedanceOffset(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceOffset(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getFirmwareVersion(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getFirmwareVersion(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentPid(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *cur_pid_ptr = (eOmc_PID_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID;
    *((uint16_t*)(&canFrame->data[1])) = cur_pid_ptr->kp;
    *((uint16_t*)(&canFrame->data[3])) = cur_pid_ptr->ki;
    *((uint16_t*)(&canFrame->data[5])) = cur_pid_ptr->kd;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getCurrentPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getCurrentPid(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentPidLimits(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *cur_pid_ptr = (eOmc_PID_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PIDLIMITS;

    *((uint16_t*)(&canFrame->data[1])) = cur_pid_ptr->offset;
    *((uint16_t*)(&canFrame->data[3])) = cur_pid_ptr->limitonoutput;
    *((uint16_t*)(&canFrame->data[5])) = cur_pid_ptr->limitonintegral;
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getCurrentPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getCurrentPidLimits(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelocityPid(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{

    eOmc_PID_t *vel_pid_ptr = (eOmc_PID_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_VELOCITY_PID;
    *((uint16_t*)(&canFrame->data[1])) = vel_pid_ptr->kp;
    *((uint16_t*)(&canFrame->data[3])) = vel_pid_ptr->ki;
    *((uint16_t*)(&canFrame->data[5])) = vel_pid_ptr->kd;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getVelocityPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getVelocityPid(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelocityPidLimits(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *vel_pid_ptr = (eOmc_PID_t*)val_ptr;

    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_VELOCITY_PIDLIMITS;

    *((uint16_t*)(&canFrame->data[1])) = vel_pid_ptr->offset;
    *((uint16_t*)(&canFrame->data[3])) = vel_pid_ptr->limitonoutput;
    *((uint16_t*)(&canFrame->data[5])) = vel_pid_ptr->limitonintegral;

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getVelocityPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getVelocityPidLimits(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDesiredCurrent(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmeas_current_t *currentVal_ptr = (eOmeas_current_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_DISIRED_CURRENT;
    *((eOmeas_current_t*)(&canFrame->data[1])) = *currentVal_ptr;
    canFrame->data[3]= 0; //LSB Id
    canFrame->data[4]= 0; //MSB Id
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDesiredCurrent(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDesiredCurrent(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPeriodicContents(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setI2TParams(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    
    eOmc_i2tParams_t *i2tParams_ptr = (eOmc_i2tParams_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_I2T_PARAMS;
    *((uint16_t*)(&canFrame->data[1])) = i2tParams_ptr->time;
    *((uint16_t*)(&canFrame->data[3])) = i2tParams_ptr->tresh;
    
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getI2TParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getI2TParams(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}






//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__2foc(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    /* qui devo sapere come interpretare i valori che mi arrivano quindi devo essere in grado di reperire la bcastpolicy*/
    
    
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__position(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t              res;
    eOmc_jointId_t    jId;
    void                    *nv_mem_ptr;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t  canLoc;

    eOmc_joint_status_basic_t  *jstatus_ptr;

// set position about axis 0
    canLoc.emscanport = canPort;
    canLoc.canaddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_first;
    
    res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__basic, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatus_ptr = (eOmc_joint_status_basic_t*)nv_mem_ptr;    
    jstatus_ptr->position =  *((eOmeas_position_t*)&(frame->data[0]));


// set position about axis 1
    canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_second;
    
    res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__basic, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatus_ptr = (eOmc_joint_status_basic_t*)nv_mem_ptr;    
    jstatus_ptr->position =  *((eOmeas_position_t*)&(frame->data[4]));

    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidVal(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t              res;
    eOmc_jointId_t          jId;
    void                    *nv_mem_ptr;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t  canLoc;

    eOmc_joint_status_ofpid_t  *jstatuspid_ptr;

// set position about axis 0
    canLoc.emscanport = canPort;
    canLoc.canaddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_first;
    
    res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__ofpid, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatuspid_ptr = (eOmc_joint_status_ofpid_t*)nv_mem_ptr;    
    jstatuspid_ptr->output =  *((uint16_t*)&(frame->data[0]));


// set position about axis 1
    canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_second;
    
    res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__ofpid, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatuspid_ptr = (eOmc_joint_status_ofpid_t*)nv_mem_ptr;    
    jstatuspid_ptr->output =  *((uint16_t*)&(frame->data[2]));



    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__status(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__current(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOmc_motorId_t                              mId;
    void                                        *nv_mem_ptr;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t  canLoc;
    eObrd_types_t                               boardType;
    eOmc_motor_status_basic_t                   *mstatusbasic_ptr;

// set position about axis 0
    canLoc.emscanport = canPort;
    canLoc.canaddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_first;
    
    res = eo_emsCanNetTopo_GetMotorId_ByMotorCanLocation(p->emsCanNetTopo_ptr, &canLoc, &mId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_emsCanNetTopo_GetBoardOfMotor(p->emsCanNetTopo_ptr, mId, &boardType);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), mId, motorNVindex_mstatus__basic, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    mstatusbasic_ptr = (eOmc_motor_status_basic_t*)nv_mem_ptr;    
    mstatusbasic_ptr->current = ((uint16_t*)frame->data)[0];

    if(eobrd_1foc == boardType)
    {
        // ALE

#ifdef USE_2FOC_FAST_ENCODER
        if (frame->id==0x124)
        {
            eo_emsController_ReadSpeed(mId, 5*(int32_t)((int16_t*)frame->data)[1]);
        }
#endif
    }
    else //mc4
    {
// set position about axis 1
        canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_second;
        
        res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &mId);
        if(eores_OK != res)
        {
            return(res);
        }

        res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), mId, motorNVindex_mstatus__basic, &nv_mem_ptr);
        if(eores_OK != res)
        {
            return(res);
        }

        mstatusbasic_ptr = (eOmc_motor_status_basic_t*)nv_mem_ptr;    
        mstatusbasic_ptr->current =  ((uint16_t*)frame->data)[2];
    }
    return(eores_OK);
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
    eOresult_t              res;
    eOmc_motorId_t          mId;
    void                    *nv_mem_ptr;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t  canLoc;

    eOmc_motor_status_basic_t  *mstatusbasic_ptr;

// set position about jm_idInBoard 0
    canLoc.emscanport = canPort;
    canLoc.canaddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_first;
    
    res = eo_emsCanNetTopo_GetMotorId_ByMotorCanLocation(p->emsCanNetTopo_ptr, &canLoc, &mId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), mId, motorNVindex_mstatus__basic, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    mstatusbasic_ptr = (eOmc_motor_status_basic_t*)nv_mem_ptr;    
    mstatusbasic_ptr->velocity =  *((uint16_t*)&(frame->data[0]));


// set position about axis 1
    canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_second;
    
    res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &mId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), mId, motorNVindex_mstatus__basic, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    mstatusbasic_ptr = (eOmc_motor_status_basic_t*)nv_mem_ptr;    
    mstatusbasic_ptr->velocity =  *((uint16_t*)&(frame->data[2]));


    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidError(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t              res;
    eOmc_jointId_t          jId;
    void                    *nv_mem_ptr;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t  canLoc;

    eOmc_joint_status_ofpid_t  *jstatuspid_ptr;

// set position about jm_idInBoard 0
    canLoc.emscanport = canPort;
    canLoc.canaddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_first;
    
    res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__ofpid, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatuspid_ptr = (eOmc_joint_status_ofpid_t*)nv_mem_ptr;    
    jstatuspid_ptr->error =  *((uint16_t*)&(frame->data[0]));


// set position about jm_idInBoard 1
    canLoc.jm_idInBoard = eo_emsCanNetTopo_jm_index_second;
    
    res = eo_emsCanNetTopo_GetJointId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jId, jointNVindex_jstatus__ofpid, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    jstatuspid_ptr = (eOmc_joint_status_ofpid_t*)nv_mem_ptr;    
    jstatuspid_ptr->error =  *((uint16_t*)&(frame->data[2]));



    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__debug(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



