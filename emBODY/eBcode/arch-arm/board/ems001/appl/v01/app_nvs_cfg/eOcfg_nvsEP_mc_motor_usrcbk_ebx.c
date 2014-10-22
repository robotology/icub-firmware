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
#include "EoProtocol.h"



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
const eOmc_motor_t motor_default_dvalue =
{
    EO_INIT(.config)             
    {
        EO_INIT(.pidcurrent)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.scale)                 0,
            EO_INIT(.offset)                0,
            EO_INIT(.kff)                   0,
            EO_INIT(.stiction_up_val)       0,
            EO_INIT(.stiction_down_val)     0,
            EO_INIT(.filler05)              {0xf1, 0xf2, 0xf3, 0xf4, 0xf5}

        },
        EO_INIT(.maxvelocityofmotor)        0,
        EO_INIT(.maxcurrentofmotor)         0,
        EO_INIT(.filler02)                  {0}
    },
    EO_INIT(.status)                       {0}
}; 



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

/********************************************************************************************************************************/
/*************************************   M O T O R S  ****************************************************************************/
/********************************************************************************************************************************/
//motor init
//joint-init
extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig(const EOnv* nv)
{
    eOmc_motor_config_t             *cfg = (eOmc_motor_config_t*)nv->ram;
    memcpy(cfg, &motor_default_dvalue.config, sizeof(eOmc_motor_config_t));
}

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus(const EOnv* nv)
{
    eOmc_motor_status_t             *cfg = (eOmc_motor_status_t*)nv->ram;
    memcpy(cfg, &motor_default_dvalue.status, sizeof(eOmc_motor_status_t));
}




// motor-update
extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;
    eOmc_motorId_t                          mxx = eoprot_ID2index(rd->id32);
    eObrd_types_t                           boardType;
    icubCanProto_velocity_t                 vel_icubCanProtValue;
    eOmc_motor_config_t                     *cfg_ptr = (eOmc_motor_config_t*)nv->ram;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eOicubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());

	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
    
    if(eobrd_1foc == boardType)
    {
        // 1) send current pid
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->pidcurrent);

        // 2) send current pid limits
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->pidcurrent);
    }
    
    // 2) set max velocity  
    vel_icubCanProtValue = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, mxx, cfg_ptr->maxvelocityofmotor);           
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&vel_icubCanProtValue);

    // 3) set current limit  
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->maxcurrentofmotor);

}

extern void eoprot_fun_UPDT_mc_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;
    eOmc_motorId_t                          mxx = eoprot_ID2index(rd->id32);
    eOmc_PID_t                              *pid_ptr = (eOmc_PID_t*)nv->ram;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eOicubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());

	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }

  	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);

    // send current pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

}


extern void eoprot_fun_UPDT_mc_motor_config_maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_velocity_t                       *vel_ptr = (eOmeas_velocity_t*)nv->ram;
    eOmc_motorId_t                          mxx = eoprot_ID2index(rd->id32);
    icubCanProto_velocity_t                 vel_icubCanProtValue;
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());


    vel_icubCanProtValue = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, mxx, *vel_ptr);           
    eo_appCanSP_SendCmd2Motor(appCanSP_ptr, (eOmc_motorId_t)mxx, msgCmd, (void*)&vel_icubCanProtValue);

}


extern void eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_current_t                        *curr_ptr = (eOmeas_current_t*)nv->ram;
    eOmc_motorId_t                          mxx = eoprot_ID2index(rd->id32);
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appCanSP_SendCmd2Motor(appCanSP_ptr, (eOmc_motorId_t)mxx, msgCmd, (void*)curr_ptr);
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



