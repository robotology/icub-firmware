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
#include "EOappTheServicesProvider.h"
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
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOicubCanProto_position_t       pos_icubCanProtValue;
    eOicubCanProto_impedance_t      impedence_icubCanProtValues;
    eOmc_joint_config_t             *cfg = (eOmc_joint_config_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    /*Since icub can proto uses encoder tack like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_appTheSP_GetMeasuresConverterHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
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

    // 1) send pid position 
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_POS_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->pidposition);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_POS_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->pidposition);


    // 2) send torque pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->pidtorque);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->pidtorque);

    // 3) send velocity pid: currently is not send: neither MC4 nor 2foc use pid velocity.

    // 4) set min position
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION;
    eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, &pos_icubCanProtValue, cfg->minpositionofjoint);
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&pos_icubCanProtValue);

    // 5) set max position   
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION;
    eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, &pos_icubCanProtValue, cfg->maxpositionofjoint);
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&pos_icubCanProtValue);

    // 6) set vel timeout
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_VEL_TIMEOUT;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->velocitysetpointtimeout);

    // 7) set impedance
    eo_appMeasConv_impedenceStiffness_I2S(appMeasConv_ptr, jxx, &impedence_icubCanProtValues.stiffness, cfg->impedance.stiffness);
    eo_appMeasConv_impedenceDamping_I2S(appMeasConv_ptr, jxx, &impedence_icubCanProtValues.damping, cfg->impedance.damping);
    eo_appMeasConv_torque_I2S(appMeasConv_ptr, jxx, &impedence_icubCanProtValues.offset, cfg->impedance.offset);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&impedence_icubCanProtValues);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_OFFSET;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&impedence_icubCanProtValues.offset);

    // 8) control mode
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CONTROL_MODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->controlmode);

}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOmc_PID_t                      *pid_ptr = (eOmc_PID_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
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

    // send pid position 
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_POS_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)pid_ptr);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_POS_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)pid_ptr);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOmc_PID_t                      *pid_ptr = (eOmc_PID_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
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

    // send pid torque
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)pid_ptr);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)pid_ptr);
}




extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__impedance(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOicubCanProto_impedance_t      impedence_icubCanProtValues;
    eOmc_impedance_t                *impedance_ptr = (eOmc_impedance_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    /*Since icub can proto uses encoder tack like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_appTheSP_GetMeasuresConverterHandle(eo_appTheSP_GetHandle());


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

    eo_appMeasConv_impedenceStiffness_I2S(appMeasConv_ptr, jxx, &impedence_icubCanProtValues.stiffness, impedance_ptr->stiffness);
    eo_appMeasConv_impedenceDamping_I2S(appMeasConv_ptr, jxx, &impedence_icubCanProtValues.damping, impedance_ptr->damping);
    eo_appMeasConv_torque_I2S(appMeasConv_ptr, jxx, &impedence_icubCanProtValues.offset, impedance_ptr->offset);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&impedence_icubCanProtValues);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_OFFSET;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&impedence_icubCanProtValues.offset);

}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOicubCanProto_position_t       pos_icubCanProtValue;
    eOmeas_position_t               *pos_ptr = (eOmeas_position_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_appTheSP_GetMeasuresConverterHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
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
    // set min position
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION;
    eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, &pos_icubCanProtValue, *pos_ptr);
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&pos_icubCanProtValue);


}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOicubCanProto_position_t       pos_icubCanProtValue;
    eOmeas_position_t               *pos_ptr = (eOmeas_position_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_appTheSP_GetMeasuresConverterHandle(eo_appTheSP_GetHandle());

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
    // set min position
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION;
    eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, &pos_icubCanProtValue, *pos_ptr);
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&pos_icubCanProtValue);

}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOmeas_time_t                   *time_ptr = (eOmeas_time_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
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
    // set vel timeout
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_VEL_TIMEOUT;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)time_ptr);
}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__controlmode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    void                            *nv_mem_ptr;
    eOmc_controlmode_t              *controlmode_ptr = (eOmc_controlmode_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());


    // 1) set control mode in status nv
    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), (eOmc_jointId_t)jxx, jointNVindex_jstatus__basic, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return;
    }

    ((eOmc_joint_status_basic_t*)nv_mem_ptr)->controlmodestatus = *controlmode_ptr;



    // 2) if jxx doesn't belong to 2foc board, send control mode value to CAN-board
    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    if(eobrd_1foc == boardType)
    {
        eo_emsController_SetControlMode(jxx, (control_mode_t)(*controlmode_ptr));       

        return;
    }
    // send control mode
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CONTROL_MODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)controlmode_ptr);


}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__motionmonitormode(eOcfg_nvsEP_mc_jointNumber_t jxx,  const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    void              *nv_mem_ptr;
    eOresult_t        res;
    

    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), (eOmc_jointId_t)jxx, jointNVindex_jstatus__basic, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return;
    }

    if(eomc_motionmonitormode_dontmonitor == *((eOenum08_t*)nv->loc))
    {
        ((eOmc_joint_status_basic_t*)nv_mem_ptr)->motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        ((eOmc_joint_status_basic_t*)nv_mem_ptr)->motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
    }
}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                          res;
    eo_appCanSP_canLocation             canLoc;
    eObrd_types_t                       boardType;
    eOmc_setpoint_t                     *setPoint = (eOmc_setpoint_t*)nv->loc;
    void                                *val_ptr = NULL;
    void                                *nv_mem_ptr;
    eOmc_motionmonitormode_t            *monitormode;
    eOmc_joint_status_basic_t           *jstatusbasic_ptr;
    eo_icubCanProto_msgCommand_t        msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_appTheSP_GetMeasuresConverterHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
    if(eores_OK != res)
    {
        return; //i should never be here
    }

    //set monitor status = notreachedyet if monitormode is forever
    res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jxx, jointNVindex_jconfig__motionmonitormode, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return; //i should never be here
    }
    monitormode = (eOmc_motionmonitormode_t*)nv_mem_ptr;
    if(eomc_motionmonitormode_forever == *monitormode)
    {
        res = eo_appTheNVmapRef_GetJointNVMemoryRef(eo_appTheNVmapRef_GetHandle(), jxx, jointNVindex_jstatus__basic, &nv_mem_ptr);
        if(eores_OK != res)
        {
            return; //i should never be here
        }
        
        jstatusbasic_ptr = (eOmc_joint_status_basic_t*)nv_mem_ptr;
        /* if monitorstatus values setpointreached means this is a new set point, 
        so i need to start to check is set point is reached because i'm in monitormode = forever */
        if(eomc_motionmonitorstatus_setpointisreached == jstatusbasic_ptr->motionmonitorstatus)
        {
            jstatusbasic_ptr->motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
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
            
            eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, &setpoint_pos.value, setPoint->to.position.value);
            eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr,jxx, &setpoint_pos.withvelocity, setPoint->to.position.withvelocity);           
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__POSITION_MOVE; 
            val_ptr =  &setpoint_pos;    
        }break;

        case eomc_setpoint_velocity:
        {
            eOicubCanProto_setpoint_velocity_t setpoint_vel;
            
            eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, &setpoint_vel.value, setPoint->to.velocity.value);
            eo_appMeasConv_jntAcceleration_I2E(appMeasConv_ptr,jxx, &setpoint_vel.withacceleration, setPoint->to.velocity.withacceleration);           
            
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__VELOCITY_MOVE;                 
            val_ptr =  &setpoint_vel;    
        }break;

        case eomc_setpoint_torque:
        {
            eOicubCanProto_setpoint_torque_t setpoint_torque;
            
            eo_appMeasConv_torque_I2S(appMeasConv_ptr,jxx, &setpoint_torque.value, setPoint->to.torque.value);
            
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
   
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, val_ptr);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__stoptrajectory(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eObool_t                        *cmd = (eObool_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
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
            // send stop trajectory
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__STOP_TRAJECTORY;
            eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, NULL);
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
    eOresult_t                          res;
    eo_appCanSP_canLocation             canLoc;
    eObrd_types_t                       boardType;
    eOmc_calibrator_t                   *calibrator = (eOmc_calibrator_t*)nv->loc;
    eOicubCanProto_calibrator_t         iCubCanProtCalibrator;
    eo_icubCanProto_msgCommand_t        msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MB_CMD__CALIBRATE_ENCODER
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_appTheSP_GetMeasuresConverterHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, &boardType);
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
            eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, &iCubCanProtCalibrator.params.type0.velocity, calibrator->params.type0.velocity);           
        }break;
            
        case eomc_calibration_type1_abs_sens_analog:
        {
            eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, &iCubCanProtCalibrator.params.type1.position, calibrator->params.type1.position);
            eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, &iCubCanProtCalibrator.params.type1.velocity, calibrator->params.type1.velocity);           
        }break;

        case eomc_calibration_type2_hard_stops_diff:
        {
            iCubCanProtCalibrator.params.type2.pwmlimit = calibrator->params.type2.pwmlimit;
            eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, &iCubCanProtCalibrator.params.type2.velocity, calibrator->params.type2.velocity);           
        }break;

        case eomc_calibration_type3_abs_sens_digital:
        {
            eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, &iCubCanProtCalibrator.params.type3.position, calibrator->params.type3.position);
            eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, &iCubCanProtCalibrator.params.type3.velocity, calibrator->params.type3.velocity);           
            iCubCanProtCalibrator.params.type3.offset = calibrator->params.type3.offset;
        }break;

        case eomc_calibration_type4_abs_and_incremental:
        {
            eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, &iCubCanProtCalibrator.params.type4.position, calibrator->params.type3.position);
            eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, &iCubCanProtCalibrator.params.type4.velocity, calibrator->params.type4.velocity);           
            iCubCanProtCalibrator.params.type4.maxencoder = calibrator->params.type4.maxencoder;
        }break;
        
    }
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, &iCubCanProtCalibrator);
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
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOicubCanProto_velocity_t       vel_icubCanProtValue;
    eOmc_motor_config_t             *cfg_ptr = (eOmc_motor_config_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_appTheSP_GetMeasuresConverterHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetMotorCanLocation(appCanSP_ptr, mxx, &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    if(eobrd_1foc == boardType)
    {
        // 1) send current pid
        msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID;
        eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg_ptr->pidcurrent);

        // 2) send current pid limits
        msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PIDLIMITS;
        eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg_ptr->pidcurrent);
    }
    
    // 2) set max velocity  
    eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, mxx, &vel_icubCanProtValue, cfg_ptr->maxvelocityofmotor);           
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&vel_icubCanProtValue);

    // 3) set current limit  
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg_ptr->maxcurrentofmotor);

}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__pidcurrent(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eOmc_PID_t                      *pid_ptr = (eOmc_PID_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetMotorCanLocation(appCanSP_ptr, mxx, &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }


    // send current pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)pid_ptr);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)pid_ptr);


}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eOicubCanProto_velocity_t       vel_icubCanProtValue;
    eOmeas_velocity_t               *vel_ptr = (eOmeas_velocity_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_appTheSP_GetMeasuresConverterHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetMotorCanLocation(appCanSP_ptr, mxx, &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }

    // send max velocity
    eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, mxx, &vel_icubCanProtValue, *vel_ptr);           
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&vel_icubCanProtValue);

}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eOmeas_current_t                *curr_ptr = (eOmeas_current_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());

    res = eo_appCanSP_GetMotorCanLocation(appCanSP_ptr, mxx, &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }

    // set current limit  
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&curr_ptr);

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



