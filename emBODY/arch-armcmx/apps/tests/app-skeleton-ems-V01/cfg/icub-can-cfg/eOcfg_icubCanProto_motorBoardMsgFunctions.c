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

#include "EOemsCanNetworkTopology.h"
#include "EOappTheMCNVmap.h"
//#include "eOcfg_nvsEP_mc_any_con_jxx.h"
#include "eOcfg_nvsEP_mc_any_con_bodypart.h"

#include "hal_debugPin.h"
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

    eOresult_t res;
    static eOmc_joint_status_t j;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t canLoc;
    eOmc_jointUniqueId_t jUniqueId;
    eOmc_motorUniqueId_t mUniqueId;
    void *memRef1, *memRef2, *pidVel_NVptr;
    eOmc_PID_t pidVel, pidPos1, pidPos2;
    eOmc_joint_config_t jc1, jc2;
    eOmc_motor_config_t  mc;
     
    eo_icubCanProto_canBoardAddress_t boardAddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
DEBUG_PIN4_OFF;

//      res = eo_emsCanNetTopo_GetMotorBoardNV_Status_ByCanLocation(p->emsCanNetTopo_ptr, canPort, boardAddr,
//                                                       eo_icubCanProto_mAxis_0, &nvt_ptr);
    canLoc.emscanport = canPort;
    canLoc.canaddr = boardAddr;
    canLoc.axis = eo_icubCanProto_mAxis_0;
    res = eo_emsCanNetTopo_GetJointUinqueId_ByJointCanLocation(p->emsCanNetTopo_ptr, &canLoc, &jUniqueId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheMCNVmap_GetJointNVMemoryRef_test(eo_appTheMCNVmap_GetHandle(), jUniqueId, jointNVindex_jconfig, &memRef1);
    if(eores_OK != res)
    {
        return(res);
    }


    res = eo_appTheMCNVmap_GetJointNVMemoryRef(eo_appTheMCNVmap_GetHandle(), jUniqueId, jointNVindex_jconfig, &memRef2);
    if(eores_OK != res)
    {
        return(res);
    }

    if(memRef1 == memRef2)
    {
        j.velocity = 1;
    }
    else
    {
        j.velocity = 2;
    }

    memcpy(&jc1, memRef1, sizeof(eOmc_joint_config_t));

    memcpy(&jc2, memRef2, sizeof(eOmc_joint_config_t));


//pidPos
    res = eo_appTheMCNVmap_GetJointNVMemoryRef_test(eo_appTheMCNVmap_GetHandle(), jUniqueId, jointNVindex_jconfig__pidposition, &memRef1);
    if(eores_OK != res)
    {
        return(res);
    }


    res = eo_appTheMCNVmap_GetJointNVMemoryRef(eo_appTheMCNVmap_GetHandle(), jUniqueId, jointNVindex_jconfig__pidposition, &memRef2);
    if(eores_OK != res)
    {
        return(res);
    }

    memcpy(&pidPos1, memRef1, sizeof(eOmc_PID_t)); 
    memcpy(&pidPos2, memRef2, sizeof(eOmc_PID_t)); 



    //pidvel
    res = eo_appTheMCNVmap_GetJointNVMemoryRef(eo_appTheMCNVmap_GetHandle(), jUniqueId, jointNVindex_jconfig__pidvelocity, &pidVel_NVptr);
    memcpy(&pidVel,pidVel_NVptr, sizeof(eOmc_PID_t)); 

    pidVel.kd = 0x1010;




/*************************************************************************************/
/*              motor                      */

    res = eo_emsCanNetTopo_GetMotorUinqueId_ByMotorCanLocation(p->emsCanNetTopo_ptr, &canLoc, &mUniqueId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheMCNVmap_GetMotorNVMemoryRef(eo_appTheMCNVmap_GetHandle(), mUniqueId, motorNVindex_mconfig, &memRef1);
    if(eores_OK != res)
    {
        return(res);
    }
    memcpy(&mc, memRef1, sizeof(eOmc_motor_config_t));

DEBUG_PIN3_OFF;
//    nvJoint_ptr->axis.position = frame->data[1];
//    nvJoint_ptr->axis.velocity = frame->data[2]; 
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_unexpected_cmd(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_NOK_unsupported);
}


extern eOresult_t eo_icubCanProto_former_test(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.axis&0x1) <<8) | ICUBCANPROTO_POL_MB_CMD__CONTROLLER_RUN;
    canFrame->data[1] = *((uint8_t*)nv_ptr);
    canFrame->data[2] = *(((uint8_t*)nv_ptr) +1);
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__controllerRun(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__CONTROLLER_RUN;
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__controllerIdle(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__CONTROLLER_IDLE;
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__toggleVerbose(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__calibrateEncoder(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__enablePwmPad(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 1;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__ENABLE_PWM_PAD;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__disablePwmPad(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
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
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getControlMode(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__motionDone(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__motionDone(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setControlMode(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getAdditionalInfo(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getAdditionalInfo(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setAdditionalInfo(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setSpeedEtimShift(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDebugParam(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDebugParam(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDebugParam(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getEncoderPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getEncoderPosition(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__positionMove(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__velocityMove(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setEncoderPosition(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDesiredTorque(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getDesiredTorque(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDesiredTorque(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__stopTrajectory(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getBoardId(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getBoardId(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMinPosition(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION;
    *((uint32_t*)(&canFrame->data[1])) = *((uint32_t*)nv_ptr);
    return(eores_OK);


}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMinPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMinPosition(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMaxPosition(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION;
    *((uint32_t*)(&canFrame->data[1])) = *((uint32_t*)nv_ptr);
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMaxPosition(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMaxPosition(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setMaxVelocity(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION;
    *((uint16_t*)(&canFrame->data[1])) = *((uint16_t*)nv_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getMaxVelocity(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getMaxVelocity(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentLimit(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 5;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
    *((uint32_t*)(&canFrame->data[1])) = *((uint32_t*)nv_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setBcastPolicy(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelShift(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_VEL_SHIFT;
    *((uint16_t*)(&canFrame->data[1])) = *((uint16_t*)nv_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setOffsetAbsEncoder(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getOffsetAbsEncoder(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getOffsetAbsEncoder(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorquePid(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorquePid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePid(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setTorquePidLimits(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getTorquePidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getTorquePidLimits(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosPid(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosPid(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPid(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPosPidLimits(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getPosPidLimits(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelTimeout(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 3;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_VEL_TIMEOUT;
    *((uint16_t*)(&canFrame->data[1])) = *((uint16_t*)nv_ptr);
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setImpedanceParams(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getImpedanceParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceParams(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setImpedanceOffset(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getImpedanceOffset(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getImpedanceOffset(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getFirmwareVersion(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getFirmwareVersion(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setCurrentPidParam(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmc_PID_t *pidVal_ptr = (eOmc_PID_t*)nv_ptr;
//    uint8_t *u8_ptr = &canFrame->data[1];
    canFrame->id = ICUBCANPROTO_POL_MB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 7;
    canFrame->data[0] = ((dest.axis&0x1)  <<8) | ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID_PARAM;
    *((uint16_t*)(&canFrame->data[1])) = pidVal_ptr->kp;
    *((uint16_t*)(&canFrame->data[3])) = pidVal_ptr->ki;
    *((uint16_t*)(&canFrame->data[5])) = pidVal_ptr->kd;
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getCurrentPidParam(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getCurrentPidParam(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setVelocityPidParam(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getVelocityPidParam(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getVelocityPidParam(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setDesiredCurrent(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOmeas_current_t *currentVal_ptr = (eOmeas_current_t*)nv_ptr;
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


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getDesiredCurrent(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setPeriodicContents(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__setI2TParams(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_parser_pol_mb_cmd__getI2TParams(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}
extern eOresult_t eo_icubCanProto_former_pol_mb_cmd__getI2TParams(EOicubCanProto* p, void *nv_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}



//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__2foc(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__position(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidVal(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__status(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__current(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
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
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_mb_cmd__pidError(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
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



