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

/* @file       eOcfg_nvsEP_mc_usrcbk_ebx.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint mc
    @author     valentina.gaggero@iit.it
    @date       05/04/2012
**/





// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "EOnv_hid.h"

#include "EOMotionControl.h"
#include "eOcfg_nvsEP_mc.h"

#include "eOcfg_nvsEP_mc_any_con_jxxdefault.h"
#include "eOcfg_nvsEP_mc_any_con_mxxdefault.h"


//application
#include "EOtheEMSapplBody.h"
#include "EOappTheDataBase.h"
#include "EOicubCanProto_specifications.h"
#include "EOappMeasuresConverter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "eOcfg_nvsEP_mngmnt_usr_hid.h"

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
// empty-section



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
/* TAG_ALE*/

//joint-init
extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eOmc_joint_config_t             *cfg = (eOmc_joint_config_t*)nv->loc;
    memcpy(cfg, &eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig, sizeof(eOmc_joint_config_t));
}

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eOmc_joint_status_t             *cfg = (eOmc_joint_status_t*)nv->loc;
    memcpy(cfg, &eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jstatus, sizeof(eOmc_joint_status_t));
}


//joint-update
extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;    
    eObrd_types_t                           boardType;
    eOmc_joint_config_t                     *cfg = (eOmc_joint_config_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_position_t               pos_icubCanProtValue;
    eOicubCanProto_impedance_t              impedence_icubCanProtValues;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());
    
    /*Since icub can proto uses encoder tack like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsAppBody_GetMeasuresConverterHandle(eo_emsAppBody_GetHandle());andle());

    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    if(eobrd_1foc == boardType)
    {
        return;
    }

    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);
    
    // 1) send pid position 
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_POS_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidposition);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_POS_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidposition);


    // 2) send torque pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidtorque);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidtorque);

    // 3) send velocity pid: currently is not send: neither MC4 nor 2foc use pid velocity.

    // 4) set min position
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION;
    pos_icubCanProtValue = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, cfg->minpositionofjoint);
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&pos_icubCanProtValue);

    // 5) set max position   
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION;
    pos_icubCanProtValue = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, cfg->maxpositionofjoint);
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&pos_icubCanProtValue);

    // 6) set vel timeout
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_VEL_TIMEOUT;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->velocitysetpointtimeout);

    // 7) set impedance
    impedence_icubCanProtValues.stiffness = eo_appMeasConv_impedenceStiffness_I2S(appMeasConv_ptr, jxx, cfg->impedance.stiffness);
    impedence_icubCanProtValues.damping = eo_appMeasConv_impedenceDamping_I2S(appMeasConv_ptr, jxx, cfg->impedance.damping);
    impedence_icubCanProtValues.offset = eo_appMeasConv_torque_I2S(appMeasConv_ptr, jxx, cfg->impedance.offset);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_OFFSET;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues.offset);

}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    eOmc_PID_t                              *pid_ptr = (eOmc_PID_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());

    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    if(eobrd_1foc == boardType)
    {
        float rescaler = 1.0f/(float)(1<<pid_ptr->scale);

        eo_emsController_SetOffset(jxx, pid_ptr->offset);
        eo_emsController_SetPosPidLimits(jxx, pid_ptr->limitonoutput, pid_ptr->limitonintegral);
        eo_emsController_SetPosPid(jxx, pid_ptr->kp*rescaler, pid_ptr->kd*rescaler, pid_ptr->ki*rescaler);
        
        return;
    }

   //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    // send pid position 
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_POS_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_POS_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    eOmc_PID_t                              *pid_ptr = (eOmc_PID_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    if(eobrd_1foc == boardType)
    {
        float rescaler = 1.0f/(float)(1<<pid_ptr->scale);

        eo_emsController_SetOffset(jxx, pid_ptr->offset);
        eo_emsController_SetTrqPidLimits(jxx, pid_ptr->limitonoutput, pid_ptr->limitonintegral);
        eo_emsController_SetTrqPid(jxx, pid_ptr->kp*rescaler, pid_ptr->kd*rescaler, pid_ptr->ki*rescaler);

        return;
    }

   	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    // send pid torque
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);
}




extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__impedance(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    eOicubCanProto_impedance_t              impedence_icubCanProtValues;
    eOmc_impedance_t                        *impedance_ptr = (eOmc_impedance_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());
    /*Since icub can proto uses encoder tack like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsAppBody_GetMeasuresConverterHandle(eo_emsAppBody_GetHandle());


    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    if(eobrd_1foc == boardType)
    {
        // ALE: to be implemented 

        return;
    }

    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    impedence_icubCanProtValues.stiffness = eo_appMeasConv_impedenceStiffness_I2S(appMeasConv_ptr, jxx, impedance_ptr->stiffness);
    impedence_icubCanProtValues.damping = eo_appMeasConv_impedenceDamping_I2S(appMeasConv_ptr, jxx, impedance_ptr->damping);
    impedence_icubCanProtValues.offset = eo_appMeasConv_torque_I2S(appMeasConv_ptr, jxx, impedance_ptr->offset);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_OFFSET;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues.offset);

}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    eOmeas_position_t                       *pos_ptr = (eOmeas_position_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_position_t               pos_icubCanProtValue;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsAppBody_GetMeasuresConverterHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    if(eobrd_1foc == boardType)
    {
        eo_emsController_SetPosMin(jxx, *pos_ptr);
        
        return;
    }
    
    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    // set min position
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION;
    pos_icubCanProtValue = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, *pos_ptr);
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&pos_icubCanProtValue);


}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    eOmeas_position_t                       *pos_ptr = (eOmeas_position_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_position_t               pos_icubCanProtValue;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsAppBody_GetMeasuresConverterHandle(eo_emsAppBody_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    if(eobrd_1foc == boardType)
    {
        eo_emsController_SetPosMax(jxx, *pos_ptr);

        return;
    }
    
  	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    // set min position
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION;
    eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, &pos_icubCanProtValue, *pos_ptr);
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&pos_icubCanProtValue);

}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    eOmeas_time_t                           *time_ptr = (eOmeas_time_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    if(eobrd_1foc == boardType)
    {
        eo_emsController_SetVelTimeout(jxx, *time_ptr);

        return;
    }
	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    // set vel timeout
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_VEL_TIMEOUT;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)time_ptr);
}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__controlmode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
//     eOresult_t                      res;
//     eo_appCanSP_canLocation         canLoc;
//     eObrd_types_t                   boardType;
//     void                            *nv_mem_ptr;
//     eOmc_controlmode_t              *controlmode_ptr = (eOmc_controlmode_t*)nv->loc;
//     eo_icubCanProto_msgCommand_t    msgCmd = 
//     {
//         EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
//         EO_INIT(.cmdId) 0
//     };

//     EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());


//     // 1) set control mode in status nv
//     res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), (eOmc_jointId_t)jxx, jointNVindex_jstatus__basic, &nv_mem_ptr);
//     if(eores_OK != res)
//     {
//         return;
//     }

//     ((eOmc_joint_status_basic_t*)nv_mem_ptr)->controlmodestatus = *controlmode_ptr;



//     // 2) if jxx doesn't belong to 2foc board, send control mode value to CAN-board
//     res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
//     if(eores_OK != res)
//     {
//         return;
//     }

//     //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
//     //(currently no pid velocity is sent to 2foc)
//     if(eobrd_1foc == boardType)
//     {
//         eo_emsController_SetControlMode(jxx, (control_mode_t)(*controlmode_ptr));       

//         return;
//     }
//     // send control mode
//     msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CONTROL_MODE;
//     eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)controlmode_ptr);


}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__motionmonitormode(eOcfg_nvsEP_mc_jointNumber_t jxx,  const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    void              *nv_mem_ptr;
    eOresult_t              res;
    eOmc_joint_status_t     *jstatus_ptr;
    
    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jstatus_ptr);
    if(eores_OK != res)
    {
        return;
    }

    if(eomc_motionmonitormode_dontmonitor == *((eOenum08_t*)nv->loc))
    {
        jstatus_ptr->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        jstatus_ptr->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
    }
}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    eOmc_setpoint_t                         *setPoint = (eOmc_setpoint_t*)nv->loc;
    void                                    *val_ptr = NULL;
    eOmc_joint_status_t                     *jstatus_ptr;
    eOmc_joint_config_t                     *jconfig_ptr;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsAppBody_GetMeasuresConverterHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //set monitor status = notreachedyet if monitormode is forever
    res = eo_appTheDB_GetJointConfigPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jconfig_ptr);
    if(eores_OK != res)
    {
        return; //i should never be here
    }
    
    if(eomc_motionmonitormode_forever == jconfig_ptr->motionmonitormode)
    {
        res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jstatus_ptr);
        if(eores_OK != res)
        {
            return; //i should never be here
        }
        
        /* if monitorstatus values setpointreached means this is a new set point, 
        so i need to start to check is set point is reached because i'm in monitormode = forever */
        if(eomc_motionmonitorstatus_setpointisreached == jstatus_ptr->basic.motionmonitorstatus)
        {
            jstatus_ptr->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
        }
    }
    
    
    if(eobrd_1foc == boardType)
    {
        switch (setPoint->type)
        {
            case eomc_setpoint_position:
            {
                eo_emsController_SetPosRef(jxx, setPoint->to.position.value, setPoint->to.position.withvelocity);
            }break;
            
            case eomc_setpoint_velocity:
            {
                eo_emsController_SetVelRef(jxx, setPoint->to.velocity.value, setPoint->to.velocity.withacceleration);    
            }break;

            case eomc_setpoint_torque:
            {
                eo_emsController_SetTrqRef(jxx, setPoint->to.torque.value);
            }break;

            case eomc_setpoint_current:
            {             
                eo_emsController_SetOffset(jxx, setPoint->to.current.value);
            }break;

            default:
            {
                return;
            }
        }

        return;
    }

    switch(setPoint->type)
    {
        case eomc_setpoint_position:
        {
            eOicubCanProto_setpoint_position_t setpoint_pos;
            
            setpoint_pos.value = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, setPoint->to.position.value);
            setpoint_pos.withvelocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr,jxx, setPoint->to.position.withvelocity);           
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__POSITION_MOVE; 
            val_ptr =  &setpoint_pos;    
        }break;

        case eomc_setpoint_velocity:
        {
            eOicubCanProto_setpoint_velocity_t setpoint_vel;
            
            setpoint_vel.value = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx,, setPoint->to.velocity.value);
            setpoint_vel.withacceleration = eo_appMeasConv_jntAcceleration_I2E(appMeasConv_ptr,jxx, setPoint->to.velocity.withacceleration);           
            
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__VELOCITY_MOVE;                 
            val_ptr =  &setpoint_vel;    
        }break;

        case eomc_setpoint_torque:
        {
            eOicubCanProto_setpoint_torque_t setpoint_torque;
            
            setpoint_torque.value = eo_appMeasConv_torque_I2S(appMeasConv_ptr,jxx, setPoint->to.torque.value);
            
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_DESIRED_TORQUE;           
            val_ptr =  &setpoint_torque;    
        }break;

        case eomc_setpoint_current:
        {
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_DISIRED_CURRENT;                             
            val_ptr =  &(setPoint->to.current.value);    
        }break;

        default:
        {
            return;
        }
    }

    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, val_ptr);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__stoptrajectory(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    eObool_t                                *cmd = (eObool_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    if(eobrd_mc4 == boardType)
    {
        if(1 == *cmd)
        {
            //set destination of all messages 
            msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

            // send stop trajectory
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__STOP_TRAJECTORY;
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
        }
        else
        {
            //does start-traj cmd exist??? 
        }
    }
    else if(eobrd_1foc == boardType)
    {
        eo_emsController_Stop(jxx);
        
        return;    
    }

}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eo_appCanSP_canLocation                 canLoc;
    eObrd_types_t                           boardType;
    eOmc_calibrator_t                       *calibrator = (eOmc_calibrator_t*)nv->loc;
    eOicubCanProto_calibrator_t             iCubCanProtCalibrator;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MB_CMD__CALIBRATE_ENCODER
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsAppBody_GetMeasuresConverterHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    if(eobrd_1foc == boardType)
    {
        eo_emsController_StartCalibration(jxx, 
                                          calibrator->params.type3.position, 
                                          calibrator->params.type3.velocity,
                                          calibrator->params.type3.offset); 

        return;
    }

    iCubCanProtCalibrator.type = (eOicubCanProto_calibration_type_t)calibrator->type;
    switch(calibrator->type)
    {
        case eomc_calibration_type0_hard_stops:
        {
            iCubCanProtCalibrator.params.type0.pwmlimit = calibrator->params.type0.pwmlimit;
            iCubCanProtCalibrator.params.type0.velocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type0.velocity);           
        }break;
            
        case eomc_calibration_type1_abs_sens_analog:
        {
            iCubCanProtCalibrator.params.type1.position = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, calibrator->params.type1.position);
            iCubCanProtCalibrator.params.type1.velocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type1.velocity);           
        }break;

        case eomc_calibration_type2_hard_stops_diff:
        {
            iCubCanProtCalibrator.params.type2.pwmlimit = calibrator->params.type2.pwmlimit;
            iCubCanProtCalibrator.params.type2.velocity = seo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type2.velocity);           
        }break;

        case eomc_calibration_type3_abs_sens_digital:
        {
            iCubCanProtCalibrator.params.type3.position = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, calibrator->params.type3.position);
            iCubCanProtCalibrator.params.type3.velocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type3.velocity);           
            iCubCanProtCalibrator.params.type3.offset = calibrator->params.type3.offset;
        }break;

        case eomc_calibration_type4_abs_and_incremental:
        {
            iCubCanProtCalibrator.params.type4.position = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, calibrator->params.type3.position);
            iCubCanProtCalibrator.params.type4.velocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type4.velocity);           
            iCubCanProtCalibrator.params.type4.maxencoder = calibrator->params.type4.maxencoder;
        }break;
        
    }
    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &iCubCanProtCalibrator);
}


/********************************************************************************************************************************/
/*************************************   M O T O R S  ****************************************************************************/
/********************************************************************************************************************************/
//motor init
//joint-init
extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eOmc_motor_config_t             *cfg = (eOmc_motor_config_t*)nv->loc;
    memcpy(cfg, &eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mconfig, sizeof(eOmc_motor_config_t));
}

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv)
{
    eOmc_motor_status_t             *cfg = (eOmc_motor_status_t*)nv->loc;
    memcpy(cfg, &eo_cfg_nvsEP_mc_any_con_mxxdefault_defaultvalue.mstatus, sizeof(eOmc_motor_status_t));
}




// motor-update
extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eo_appCanSP_canLocation                 canLoc;
    eObrd_types_t                           boardType;
    eOicubCanProto_velocity_t               vel_icubCanProtValue;
    eOmc_motor_config_t                     *cfg_ptr = (eOmc_motor_config_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsAppBody_GetMeasuresConverterHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);
    
    if(eobrd_1foc == boardType)
    {
        // 1) send current pid
        msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->pidcurrent);

        // 2) send current pid limits
        msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PIDLIMITS;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->pidcurrent);
    }
    
    // 2) set max velocity  
    vel_icubCanProtValue = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, mxx, cfg_ptr->maxvelocityofmotor);           
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&vel_icubCanProtValue);

    // 3) set current limit  
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->maxcurrentofmotor);

}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__pidcurrent(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eo_appCanSP_canLocation                 canLoc;
    eOmc_PID_t                              *pid_ptr = (eOmc_PID_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

  	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    // send current pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eOmeas_velocity_t                       *vel_ptr = (eOmeas_velocity_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_velocity_t               vel_icubCanProtValue;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsAppBody_GetMeasuresConverterHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

  	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    // send max velocity
    vel_icubCanProtValue = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, mxx, *vel_ptr);           
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&vel_icubCanProtValue);

}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eOmeas_current_t                        *curr_ptr = (eOmeas_current_t*)nv->loc;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eo_icubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsAppBody_GetCanServiceHandle(eo_emsAppBody_GetHandle());

	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

	
	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.jm_indexinboard, canLoc.addr);

    // set current limit  
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&curr_ptr);

}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



