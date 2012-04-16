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

// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file       EOMappCanServicesProvider.c
    @brief      This file implements CAN services.
    @author    valentina.gaggero@iit.it
    @date       02/14/2012
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

//sys
#include "stdlib.h"
#include "string.h"

//abs
#include "hal.h"
#include "hal_debugPin.h"

//embobj
#include "EoCommon.h"
#include "EOtheMemoryPool.h"
#include "EOtheBOARDTransceiver_hid.h" //to get nvscfg

//embobj-icub
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"
#include "EOemsCanNetworkTopology.h"
#include "EoMotionControl.h"

//embobj-icub-cfg
#include "eOcfg_emsCanNetworkTopology.h"
#include "eOcfg_icubCanProto_messagesLookupTbl.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOappCanServicesProvider.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOappCanServicesProvider_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_appCanSP_canPeriphInit(void);
static eOresult_t s_eo_ap_canModule_formAndSendFrame(eo_icubCanProto_msgCommand_t cmd, 
                                                     void *val_ptr,
                                                     eo_icubCanProto_msgDestination_t boardAddr,
                                                     eOcanport_t canPort,
                                                     eOcanframe_t *frame);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOappCanSP* eo_appCanSP_New(eOappCanSP_cfg_t *cfg)
{
    eOresult_t      res;
    EOappCanSP      *retptr = NULL;
    EOicubCanProto  *icubCanProto_ptr;
    EOemsCanNetTopo *emsCanNetTopo_ptr;


    eo_emsCanNetTopo_cfg_t   emsCanNetTopo_cfg = 
    {
        EO_INIT(.nvsCfg)                                NULL,
        EO_INIT(.joint2BoardCanLocation_LUTbl_ptr)      emsCanNetTopo_joint2BoardCanLocation_LUTbl__ptr,
        EO_INIT(.motorBoardCanLoc2NvsRef_LUTbl_ptr)     emsCanNetTopo_motorBoardCanLoc2NvsRef_LUTbl__ptr
    };

    eo_icubCanProto_cfg_t icubCanProto_cfg = 
    {
        EO_INIT(.msgClasses_LUTbl__ptr)     icubCanProto_msgClasses_lookupTbl__ptr 
    };


// 1) initialise peritheral
    res = (eOresult_t)s_eo_appCanSP_canPeriphInit();
    if(eores_OK != res)
    {
        return(retptr);
    }

//2) initialise emsCanNetTopology (the EOtheBoardTransceiver MUST be inited)
    emsCanNetTopo_cfg.nvsCfg = eo_boardtransceiver_hid_GetNvsCfg();
    emsCanNetTopo_ptr = eo_emsCanNetTopo_New(&emsCanNetTopo_cfg);
    if(NULL == emsCanNetTopo_ptr)
    {
        return(retptr);
    }


//3) initialise icubCanProto
    icubCanProto_cfg.emsCanNetTopo__ptr = emsCanNetTopo_ptr; 
    icubCanProto_ptr = eo_icubCanProto_New(&icubCanProto_cfg);
    if(NULL == icubCanProto_ptr)
    {
        return(retptr);
    }

//4) create the obj (i get the memory for the object)
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOappCanSP), 1);

    retptr->icubCanProto_ptr = icubCanProto_ptr;
    retptr->emsCanNetTopo_ptr = emsCanNetTopo_ptr;

    return(retptr);
}


extern eOresult_t eo_appCanSP_ConfigJoint(EOappCanSP *p, eOmc_jointUniqueId_t jUniqueId, eOmc_joint_config_t *cfg)
{
    eOcanport_t canPort;
    eo_icubCanProto_canBoardAddress_t boardAddr;
    eo_icubCanProto_motorAxis_t axis;
    eOresult_t res;
    eOcanframe_t canFrame;
    eo_icubCanProto_msgDestination_t dest;
    eo_icubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    res = eo_emsCanNetTopo_GetCanLocation_ByJointUniqueId(p->emsCanNetTopo_ptr, jUniqueId, &canPort, &boardAddr, &axis);
    if(eores_OK != res)
    {
        return(res);
    }

    //set destination of message (one for all msg)
    dest.axis = axis;
    dest.canAddr = boardAddr;

    // 1) send pid position 
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_POS_PID;

    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->pidposition, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }


    // 2) send velocity pid 
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_VELOCITY_PID_PARAM;

    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->pidvelocity, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }

    // 3) send torque pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_TORQUE_PID;

    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->pidtorque, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }

    // 4) set min position
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MIN_POSITION;
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->minpositionofjoint, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }

    // 5) set max position   
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_POSITION;
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->maxpositionofjoint, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }

//    // 6) set vel shift    ==>REMOVED BECAIUSE OBSOLETE
//    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_VEL_SHIFT;
//    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->velocityshiftfactor, &canFrame);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
//    if(eores_OK != res)
//    {
//        return(res);
//    }

    // 6) set vel timeout
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_VEL_TIMEOUT;
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->velocitysetpointtimeout, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }













//    // 4) set max velocity   
//    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
//    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->maxvelocityofmotor, &canFrame);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//
//    // 5) set current limit  
//    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
//    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->maxcurrentofmotor, &canFrame);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
//    if(eores_OK != res)
//    {
//        return(res);
//    }

    // 6) set bcast policy  ??? DOVE PRENDO L'INFO
//    res = eo_icubCanProto_formCanFrame4MotorBoard(ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT, (void*)&cfg->maxcurrentofmotor,
//                                                  boardAddr, axis, &canFrame);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
//    if(eores_OK != res)
//    {
//        return(res);
//    }



//    // 8) set speed etim shift    COSA METTO COME CAMPO DELL NVS???
//    res = eo_icubCanProto_formCanFrame4MotorBoard(ICUBCANPROTO_POL_MB_CMD__SET_SPEED_ESTIM_SHIFT, (void*)&cfg->velocitysetpointtimeout,
//                                                  boardAddr, axis, &canFrame);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
//    if(eores_OK != res)
//    {
//        return(res);
//    }


    // 9) set controller idle
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__CONTROLLER_IDLE;
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, NULL, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }

    // 10) set pwm pad
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__DISABLE_PWM_PAD;
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, NULL, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }

    return(eores_OK);
}


extern eOresult_t eo_appCanSP_ConfigMotor(EOappCanSP *p, eOmc_motorUniqueId_t mUniqueId, eOmc_motor_config_t *cfg)
{
    eOcanport_t canPort;
    eo_icubCanProto_canBoardAddress_t boardAddr;
    eo_icubCanProto_motorAxis_t axis;
    eOresult_t res;
    eOcanframe_t canFrame;
    eo_icubCanProto_msgDestination_t dest;
    eo_icubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    res = eo_emsCanNetTopo_GetCanLocation_ByMotorUniqueId(p->emsCanNetTopo_ptr, mUniqueId, &canPort, &boardAddr, &axis);
    if(eores_OK != res)
    {
        return(res);
    }

    //set destination of message (one for all msg)
    dest.axis = axis;
    dest.canAddr = boardAddr;

    // 1) send current pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID_PARAM;
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->pidcurrent, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }


    // 2) set max velocity   
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->maxvelocityofmotor, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }

    // 3) set current limit  
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->maxcurrentofmotor, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }

    // 4) set perriodic msg contents  ??? DOVE PRENDO L'INFO
//    res = eo_icubCanProto_formCanFrame4MotorBoard(ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT, (void*)&cfg->maxcurrentofmotor,
//                                                  boardAddr, axis, &canFrame);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
//    if(eores_OK != res)
//    {
//        return(res);
//    }

#warning: VALE-> manca il campo i2t in motor config!!!!
    // 5) set i2t param      MANCANO!!!
//    res = eo_icubCanProto_formCanFrame4MotorBoard(ICUBCANPROTO_POL_MB_CMD__SET_I2T_PARAMS, (void*)&cfg->velocityshiftfactor,
//                                                  boardAddr, axis, &canFrame);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
//    if(eores_OK != res)
//    {
//        return(res);
//    }


    return(eores_OK);
}







//extern eOresult_t eo_ap_canMod_readMotorBoardMessages(uint8_t *readMsg, uint8_t *remainingMsg)
//{
//    hal_result_t res = hal_res_OK;
//    hal_can_frame_t rec_frame;
//    uint8_t  i;
//
//    for(i=0; i<MOTOR_MSG_MAX_RX; i++)
//    {
//        res = hal_can_get(hal_can_port1, &rec_frame, remainingMsg);
//        if(hal_res_OK != res) 
//        {
//            if(hal_res_NOK_nodata == res)
//            {
//                *remainingMsg = 0;
//                return(eores_OK);
//            }
//            else
//            {
//                return((eOresult_t)res); //error management
//            }
//            
//        }
//#warning X_TEST
//        rec_frame.id = 0x040;
//        rec_frame.data[0] = 0;
//        DEBUG_PIN3_ON;
//        eo_icubCanProto_parseCanFrame((eOcanframe_t*)&rec_frame, (eOcanport_t)hal_can_port1);
//    }
//    *readMsg = i;
//
//    return(eores_OK);
//}

//extern eOresult_t eo_ap_canMod_readSensorBoardMessages(uint8_t *readMsg, uint8_t *remainingMsg)
//{
//
//    hal_can_frame_t rec_frame;
//    uint8_t i;
//    hal_result_t res = hal_res_OK;
//
//    for(i=0; i<SENSOR_MSG_MAX_RX; i++)
//    {
//        res = hal_can_get(hal_can_port2, &rec_frame, remainingMsg);
//        if(hal_res_OK != res) 
//        {
//            if(hal_res_NOK_nodata != res)
//            {
//                *remainingMsg = 0;
//                return(eores_OK);
//            }
//            else
//            {
//                return((eOresult_t)res); //error management
//            }
//            
//        }
//        eo_icubCanProto_parseCanFrame((eOcanframe_t*)&rec_frame, (eOcanport_t)hal_can_port1);
//    }
//    *readMsg = i;
//
//    return(eores_OK);
//}







extern eOresult_t eo_appCanSP_SendSetPoint(EOappCanSP *p, eOmc_jointUniqueId_t jUniqueId, eOmc_setpoint_t *setPoint)
{
    eOresult_t res;
    eOcanport_t canPort;
    eo_icubCanProto_canBoardAddress_t boardAddr;
    eo_icubCanProto_motorAxis_t axis;
    eOcanframe_t canFrame;
    eo_icubCanProto_msgDestination_t dest;
    void *val_ptr = NULL;
    eo_icubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    res = eo_emsCanNetTopo_GetCanLocation_ByJointUniqueId(p->emsCanNetTopo_ptr, jUniqueId, &canPort, &boardAddr, &axis);
    if(eores_OK != res)
    {
        return(res);
    }

    dest.axis = axis;
    dest.canAddr = boardAddr;


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
            return(eores_NOK_unsupported);
        }
    }

    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)val_ptr, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }

    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
    if(eores_OK != res)
    {
        return(res);
    }

    return(eores_OK);
}



extern eOresult_t eo_appCanSP_read(EOappCanSP *p)
{
#define MOTOR_MSG_MAX_RX 2

    eOresult_t res;
    hal_can_frame_t rec_frame;
    uint8_t  i;
    uint8_t remainingMsg;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    for(i=0; i<MOTOR_MSG_MAX_RX; i++)
    {
        res = (eOresult_t)hal_can_get(hal_can_port1, &rec_frame, &remainingMsg);
        if(eores_OK != res) 
        {
            if(eores_NOK_nodata == res)
            {
                return(eores_OK);
            }
            else
            {
                return(res); //error management
            }
                                     
        }
        rec_frame.id = 0x040;
        rec_frame.data[0] = 0;
        DEBUG_PIN3_ON;
        DEBUG_PIN4_ON;
        eo_icubCanProto_ParseCanFrame(p->icubCanProto_ptr, (eOcanframe_t*)&rec_frame, (eOcanport_t)hal_can_port1);
    }
    return(eores_OK);

}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_appCanSP_canPeriphInit(void)
{
    hal_can_cfg_t can_cfg_port1, can_cfg_port2;
    eOresult_t res;

    can_cfg_port1.runmode            = hal_can_runmode_isr_1txq1rxq;
    can_cfg_port1.baudrate           = hal_can_baudrate_1mbps; 
    can_cfg_port1.priorx             = hal_int_priority05;
    can_cfg_port1.priotx             = hal_int_priority05;
    can_cfg_port1.callback_on_rx     = NULL;
    can_cfg_port1.arg                = NULL;
    
    can_cfg_port2.runmode            = hal_can_runmode_isr_1txq1rxq;
    can_cfg_port2.baudrate           = hal_can_baudrate_1mbps; 
    can_cfg_port2.priorx             = hal_int_priority06;
    can_cfg_port2.priotx             = hal_int_priority06;
    can_cfg_port2.callback_on_rx     = NULL;
    can_cfg_port2.arg                = NULL;

    res = (eOresult_t)hal_can_init(hal_can_port1, &can_cfg_port1);

    if(eores_OK != res)
    {
       return(res);
    }

    res = (eOresult_t)hal_can_init(hal_can_port2, &can_cfg_port2);

    if(eores_OK != res)
    {
        return(res);
    }

    res = (eOresult_t)hal_can_enable(hal_can_port1);

    if(eores_OK != res)
    {
        return(res);
    }

    res = (eOresult_t)hal_can_enable(hal_can_port2);

    if(eores_OK != res)
    {
        return(res);
    }

    return(eores_OK);
}




__inline static eOresult_t s_eo_ap_canModule_formAndSendFrame(eo_icubCanProto_msgCommand_t cmd, 
                                                              void *val_ptr,
                                                              eo_icubCanProto_msgDestination_t dest,
                                                              eOcanport_t canPort,
                                                              eOcanframe_t *frame)
{
//CURRENTLY NOT USED!!!
//    eOresult_t res;
//
//    res = eo_icubCanProto_FormCanFrame(icubCanProto_ptr, cmd, dest, val_ptr, frame);
//    if(eores_OK != res)
//    {
//        return(res);
//    }
//
//    res = (eOresult_t)hal_can_put((hal_can_port_t)canPort, (hal_can_frame_t*)&frame, hal_can_send_normprio_now);
//    if(eores_OK != res)
//    {
//        return(res);
//    }

    return(eores_OK);
    
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



