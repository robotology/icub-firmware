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
#include "EOMappTheSysController.h"
#include "EOappCanServicesProvider.h"
#include "EOicubCanProto_specifications.h"

#include "EOappTheNVmapRef.h"



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
    eOmc_joint_config_t             *cfg = (eOmc_joint_config_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->minpositionofjoint);

    // 5) set max position   
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->maxpositionofjoint);

    // 6) set vel timeout
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_VEL_TIMEOUT;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->velocitysetpointtimeout);

    // 7) set impedance
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->impedance);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_OFFSET;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->impedance);

    // 8) control mode
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CONTROL_MODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg->controlmode);

    // 9) set speed etim shift DA CHIEDERE!!!TOLGO ANCHE QUESTA XKE' OBSOLETA?????COME SPEED SHIFT???

    
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

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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
    eOmc_impedance_t                *impedance_ptr = (eOmc_impedance_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)impedance_ptr);

    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_IMPEDANCE_OFFSET;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)impedance_ptr);

}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOmeas_position_t               *pos_ptr = (eOmeas_position_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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
    // set min position
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)pos_ptr);


}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eOmeas_position_t               *pos_ptr = (eOmeas_position_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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
    // set min position
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)pos_ptr);

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

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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
    eOenum08_t                      *controlmode_ptr = (eOenum08_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());


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

    // TODO: add something to notify the application to check motion done


}
extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                          res;
    eo_appCanSP_canLocation             canLoc;
    eOmc_setpoint_t                     *setPoint = (eOmc_setpoint_t*)nv->loc;
    void                                *val_ptr = NULL;
    eo_icubCanProto_msgCommand_t        msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

    res = eo_appCanSP_GetJointCanLocation(appCanSP_ptr, jxx, &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }

    switch( setPoint->type)
    {
        case eomc_setpoint_position:
        {
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__POSITION_MOVE; 
            val_ptr =  &(setPoint->to.position);    
        }break;

        case eomc_setpoint_velocity:
        {
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__VELOCITY_MOVE;                 
            val_ptr =  &(setPoint->to.velocity);    
        }break;

        case eomc_setpoint_torque:
        {
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_DESIRED_TORQUE;           
            val_ptr =  &(setPoint->to.torque.value);    
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

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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
    else
    {
        //ems stuff
    }

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
    eOmc_motor_config_t             *cfg_ptr = (eOmc_motor_config_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

    res = eo_appCanSP_GetMotorCanLocation(appCanSP_ptr, mxx, &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }


    // 1) send current pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg_ptr->pidcurrent);

    // 2) send current pid limits
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg_ptr->pidcurrent);

    // 2) set max velocity   
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg_ptr->maxvelocityofmotor);

    // 3) set current limit  
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&cfg_ptr->maxcurrentofmotor);

#warning: VALE-> give default values to i2t !!!!
    // 5) set i2t param      MISSING!!! 

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

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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
    eOmeas_velocity_t               *vel_ptr = (eOmeas_velocity_t*)nv->loc;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

    res = eo_appCanSP_GetMotorCanLocation(appCanSP_ptr, mxx, &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }

    // send max velocity
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)vel_ptr);

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

    EOappCanSP *appCanSP_ptr = (EOappCanSP*)eom_appTheSysController_Services_Can_GetHandle(eom_appTheSysController_GetHandle());

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



