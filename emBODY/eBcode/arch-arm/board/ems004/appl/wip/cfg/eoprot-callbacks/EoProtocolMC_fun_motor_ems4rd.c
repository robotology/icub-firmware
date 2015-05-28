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

/* @file       EoProtocolMC_fun_motor_ems4rd.c
    @brief      This file contains the callback functions used for handling mc-motor eth messages
    @author     marco.accame@iit.it
    @date       05/28/2015
**/





// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "EOnv.h"

#include "EOMotionControl.h"
#include "EoProtocol.h"

#include "EOtheCANservice.h"

//application
#include "EOtheEMSapplBody.h"
#include "EOtheMeasuresConverter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

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

static eObool_t s_motorcontrol_is2foc_based(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

const eOmc_motor_t motor_default_value =
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
            EO_INIT(.filler)                {0xf1, 0xf2, 0xf3}

        },
        EO_INIT(.gearboxratio)              0,
        EO_INIT(.rotorencoder)              0,
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


extern void eoprot_fun_INIT_mc_motor_config(const EOnv* nv)
{
    eOmc_motor_config_t *cfg = (eOmc_motor_config_t*)eo_nv_RAM(nv);
    memcpy(cfg, &motor_default_value.config, sizeof(eOmc_motor_config_t));
}

extern void eoprot_fun_INIT_mc_motor_status(const EOnv* nv)
{
    eOmc_motor_status_t *cfg = (eOmc_motor_status_t*)eo_nv_RAM(nv);
    memcpy(cfg, &motor_default_value.status, sizeof(eOmc_motor_status_t));
}



extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    // must send some can frames. however, in here i need the type of attached board: 1foc or mc4.
    // there are many modes to do this, but in wait of the EOmcManager object we just use a s_motorcontrol_is2foc_based() function
    
    eOmc_motor_config_t *cfg_ptr = (eOmc_motor_config_t*)rd->data;
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    
    // in here i assume that all the mc boards are either 1foc or mc4
    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        // send current pid
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
        command.value = &cfg_ptr->pidcurrent;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);

        // send current pid limits
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
        command.value = &cfg_ptr->pidcurrent;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);             
    }
    
    // set max velocity      
    icubCanProto_velocity_t vel_icubCanProtValue = eo_measconv_jntVelocity_I2E(eo_measconv_GetHandle(), mxx, cfg_ptr->maxvelocityofmotor);           
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
    command.value = &vel_icubCanProtValue;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 

    // set current limit  
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    command.value = &cfg_ptr->maxcurrentofmotor;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 
    
//#if 0    
//    eOresult_t                              res;
//    eOmc_motorId_t                          mxx = eoprot_ID2index(rd->id32);
//    eObrd_cantype_t                         boardType;
//    icubCanProto_velocity_t                 vel_icubCanProtValue;
//    eOmc_motor_config_t                     *cfg_ptr = (eOmc_motor_config_t*)rd->data;
//    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
//    eOicubCanProto_msgDestination_t         msgdest;
//    eOicubCanProto_msgCommand_t             msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) 0
//    };

//    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
//    EOappMeasConv* appMeasConv_ptr = eo_measconv_GetHandle();

//	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, &boardType);
//    if(eores_OK != res)
//    {
//        return;
//    }

//	//set destination of all messages 
//    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
//    

//    if(eobrd_cantype_1foc == boardType)
//    {
//        // 1) send current pid
//        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
//        eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->pidcurrent);

//        // 2) send current pid limits
//        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
//        eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->pidcurrent);
//    }
//    
//    // 2) set max velocity  
//    vel_icubCanProtValue = eo_measconv_jntVelocity_I2E(appMeasConv_ptr, mxx, cfg_ptr->maxvelocityofmotor);           
//    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
//    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&vel_icubCanProtValue);

//    // 3) set current limit  
//    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
//    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->maxcurrentofmotor);
//#endif
}

extern void eoprot_fun_UPDT_mc_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
       
    // send current pid
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    command.value = pid;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);

    // send current pid limits
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
    command.value = pid;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);             
     
    
//#if 0    
//    eOresult_t                              res;
//    eOmc_motorId_t                          mxx = eoprot_ID2index(rd->id32);
//    eOmc_PID_t                              *pid_ptr = (eOmc_PID_t*)rd->data;
//    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
//    eOicubCanProto_msgDestination_t         msgdest;
//    eOicubCanProto_msgCommand_t            msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) 0
//    };

//    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());

//	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, NULL);
//    if(eores_OK != res)
//    {
//        return;
//    }

//  	//set destination of all messages 
//    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);

//    // send current pid
//    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
//    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

//    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
//    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);
//#endif
}


extern void eoprot_fun_UPDT_mc_motor_config_maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_velocity_t *vel = (eOmeas_velocity_t*)rd->data;
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    
    // set max velocity  
    icubCanProto_velocity_t vel_icubCanProtValue = eo_measconv_jntVelocity_I2E(eo_measconv_GetHandle(), mxx, *vel);           
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
    command.value = &vel_icubCanProtValue;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 

    
//#if 0    
//    eOmeas_velocity_t                       *vel_ptr = (eOmeas_velocity_t*)rd->data;
//    eOmc_motorId_t                          mxx = eoprot_ID2index(rd->id32);
//    icubCanProto_velocity_t                 vel_icubCanProtValue;
//    eOicubCanProto_msgCommand_t             msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY
//    };

//    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
//    EOappMeasConv* appMeasConv_ptr = eo_measconv_GetHandle();


//    vel_icubCanProtValue = eo_measconv_jntVelocity_I2E(appMeasConv_ptr, mxx, *vel_ptr);           
//    eo_appCanSP_SendCmd2Motor(appCanSP_ptr, (eOmc_motorId_t)mxx, msgCmd, (void*)&vel_icubCanProtValue);
//#endif
}


extern void eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    eOmeas_current_t *curr = (eOmeas_current_t*)rd->data;
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;

    // set current limit  
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    command.value = curr;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);     
    
 
//#if 0    
//    eOmeas_current_t                        *curr_ptr = (eOmeas_current_t*)rd->data;
//    eOmc_motorId_t                          mxx = eoprot_ID2index(rd->id32);
//    eOicubCanProto_msgCommand_t             msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT
//    };

//    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    eo_appCanSP_SendCmd2Motor(appCanSP_ptr, (eOmc_motorId_t)mxx, msgCmd, (void*)curr_ptr);
//#endif    
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// in the future, there will be 2foc, mc4can, and mc4plus
static eObool_t s_motorcontrol_is2foc_based(void)
{
    static eOmn_appl_runMode_t apprunmode = applrunMode__default;
    static eObool_t is2focbased = eobool_false; 
    if(applrunMode__default == apprunmode)
    {
        apprunmode = eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
        is2focbased = (applrunMode__2foc == apprunmode) ? (eobool_true) : (eobool_false);
    }
    return(is2focbased);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



