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
//#include "hal_debugPin.h"

//embobj
#include "EoCommon.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"

//embobj-icub
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"

//embobj-icub-cfg
#include "eOcfg_icubCanProto_messagesLookupTbl.h"

//appl 
#include "EOappTheDataBase.h"


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
static eOresult_t s_eo_appCanSP_canPeriphInit(EOappCanSP *p);
static eOresult_t s_eo_appCanSP_formAndSendFrame(EOappCanSP *p, eOcanport_t emscanport, eOicubCanProto_msgDestination_t dest, 
                                                 eOicubCanProto_msgCommand_t msgCmd, void *val_ptr);
static void s_eo_appCanSP_callbackOnTx_port1_waittransmission(void *arg);
static void s_eo_appCanSP_callbackOnTx_port2_waittransmission(void *arg);
static void s_eo_appCanSP_callbackOnTx_portx_waittransmission(void *arg, hal_can_port_t port);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static eOappCanSP_cfg_t defaultcfg = 
{
    .waitallframesaresent   = eobool_true
};

static const char s_eobj_ownname[] = "EOappCanSP";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOappCanSP* eo_appCanSP_New(eOappCanSP_cfg_t *cfg)
{
    eOresult_t      res;
    EOappCanSP      *retptr = NULL;
    EOicubCanProto  *icubCanProto_ptr;

//     if(NULL == cfg)
//     {
//         cfg = &defaultcfg;
//     }
    //I would like use only defalt cfg,
    //in order to enable "wait all frames are sent during tx phase in application's running mode
    cfg = &defaultcfg;
    

    eo_icubCanProto_cfg_t icubCanProto_cfg = 
    {
        EO_INIT(.msgClasses_LUTbl__ptr)     icubCanProto_msgClasses_lookupTbl__ptr 
    };



//1) initialise icubCanProto
    icubCanProto_ptr = eo_icubCanProto_New(&icubCanProto_cfg);
    if(NULL == icubCanProto_ptr)
    {
        return(retptr);
    }

//3) create the obj (i get the memory for the object)
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOappCanSP), 1);

    memcpy(&retptr->cfg, cfg, sizeof(eOappCanSP_cfg_t));
    retptr->icubCanProto_ptr = icubCanProto_ptr;
    retptr->runmode = eo_appCanSP_runMode__onEvent;
    
    //wait machanism
    retptr->waittxdata[hal_can_port1].numoftxframe2send = 0;
    retptr->waittxdata[hal_can_port2].numoftxframe2send = 0;
    
    retptr->waittxdata[hal_can_port1].waitenable = eobool_false;
    retptr->waittxdata[hal_can_port2].waitenable = eobool_false;
    
    if(cfg->waitallframesaresent)
    {
        extern const hal_cfg_t *hal_cfgMINE;
        //can port 1
        //the max num of token in semaphore is equal to the max num of frame in can out queue.
        retptr->waittxdata[hal_can_port1].semaphore = osal_semaphore_new(hal_cfgMINE->can1_txqnorm_num/*maxtokens*/, 0/*current num of token*/);
        if(NULL == retptr->waittxdata[hal_can_port1].semaphore)
        {
            return(NULL);
        }
        
        //can port 2
        //the max num of token in semaphore is equal to the max num of frame in can out queue.
        retptr->waittxdata[hal_can_port2].semaphore = osal_semaphore_new(hal_cfgMINE->can1_txqnorm_num/*maxtokens*/, 0/*current num of token*/);
        if(NULL == retptr->waittxdata[hal_can_port2].semaphore)
        {
            return(NULL);
        }
        
    }
    else
    {
        retptr->waittxdata[hal_can_port1].semaphore = NULL;
        retptr->waittxdata[hal_can_port2].semaphore = NULL;
    }

// 4) initialise peritheral
    res = (eOresult_t)s_eo_appCanSP_canPeriphInit(retptr);
    if(eores_OK != res)
    {
        return(retptr);
    }
    
    return(retptr);
}


extern eOresult_t eo_appCanSP_SendCmd2Joint(EOappCanSP *p, eOmc_jointId_t jId, eOicubCanProto_msgCommand_t msgCmd, void *val_ptr)
{
    eOresult_t                                  res;
    eOappTheDB_jointOrMotorCanLocation_t        canLoc;
    eOicubCanProto_msgDestination_t             msgdest;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jId,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return(res);
    }


    //set destination of message
    msgdest.dest =ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
   
    res = s_eo_appCanSP_formAndSendFrame(p, canLoc.emscanport, msgdest, msgCmd, val_ptr);
    return(res);
}


extern eOresult_t eo_appCanSP_SendCmd2Motor(EOappCanSP *p, eOmc_motorId_t mId, eOicubCanProto_msgCommand_t msgCmd, void *val_ptr)
{
    eOresult_t                                  res;
    eOappTheDB_jointOrMotorCanLocation_t        canLoc;
    eOicubCanProto_msgDestination_t             msgdest;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mId,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return(res);
    }

    //set destination of message 
    msgdest.dest =ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
    
    res = s_eo_appCanSP_formAndSendFrame(p, canLoc.emscanport, msgdest, msgCmd, val_ptr);
    return(res);
}


extern eOresult_t eo_appCanSP_SendCmd2SnrMais(EOappCanSP *p, eOsnsr_maisId_t sId, eOicubCanProto_msgCommand_t msgCmd, void *val_ptr)
{
    eOresult_t                                  res;
    eOappTheDB_SensorCanLocation_t              canLoc;
    eOicubCanProto_msgDestination_t             msgdest;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    res = eo_appTheDB_GetSnsrMaisCanLocation(eo_appTheDB_GetHandle(), sId,  &canLoc);
    if(eores_OK != res)
    {
        return(res);
    }

    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr);
    res = s_eo_appCanSP_formAndSendFrame(p, canLoc.emscanport, msgdest, msgCmd, val_ptr);
    
    return(res);
    
}

extern eOresult_t eo_appCanSP_SendCmd2SnrStrain(EOappCanSP *p, eOsnsr_strainId_t sId, eOicubCanProto_msgCommand_t msgCmd, void *val_ptr)
{
    eOresult_t                                  res;
    eOappTheDB_SensorCanLocation_t              canLoc;
    eOicubCanProto_msgDestination_t             msgdest;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    res = eo_appTheDB_GetSnsrStrainCanLocation(eo_appTheDB_GetHandle(), sId,  &canLoc);
    if(eores_OK != res)
    {
        return(res);
    }

    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr);
    res = s_eo_appCanSP_formAndSendFrame(p, canLoc.emscanport, msgdest, msgCmd, val_ptr);
    
    return(res);
    
}


extern eOresult_t eo_appCanSP_SendCmd(EOappCanSP *p, eOcanport_t emscanport, eOicubCanProto_msgDestination_t dest, eOicubCanProto_msgCommand_t msgCmd, void *val_ptr)
{
    eOresult_t      res;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    res = s_eo_appCanSP_formAndSendFrame(p, emscanport, dest, msgCmd, val_ptr);
    return(res);
}

extern eOresult_t eo_appCanSP_read(EOappCanSP *p, eOcanport_t canport, uint8_t numofcanframe, uint8_t *numofREADcanframe)
{
    eOresult_t          res;
    hal_can_frame_t     rec_frame;
    uint8_t             i;


    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    
    for(i=0; i<numofcanframe ; i++)
    {
        res = (eOresult_t)hal_can_get((hal_can_port_t)canport, &rec_frame, NULL);
        if(eores_OK != res) 
        {
            if(eores_NOK_nodata == res)
            {
                if(NULL != numofREADcanframe)
                {
                    *numofREADcanframe = i;    
                }
                return(eores_OK);
            }
            else
            {
                return(res); 
            }
                                     
        }

        res = eo_icubCanProto_ParseCanFrame(p->icubCanProto_ptr, (eOcanframe_t*)&rec_frame, (eOcanport_t)canport);
        if(eores_OK != res) 
        {
            return(res);                    
        }
    }
    
    if(NULL != numofREADcanframe)
    {
        *numofREADcanframe = i;    
    }

    return(eores_OK);

}


extern eOresult_t eo_appCanSP_SetRunMode(EOappCanSP *p, eo_appCanSP_runMode_t runmode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    p->runmode = runmode;
    
    if(eo_appCanSP_runMode__onDemand ==  p->runmode)
    {
        p->waittxdata[hal_can_port1].waitenable = eobool_true;
    }
    else
    {
        p->waittxdata[hal_can_port1].waitenable = eobool_false;
    }
    return(eores_OK);
}

extern eOresult_t eo_appCanSP_StartTransmitCanFrames(EOappCanSP *p, eOcanport_t canport)
{
    uint8_t numofoutframe = 0;
    eOresult_t res;
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    //Commented because always true
//     if(p->cfg.waitallframesaresent)
//     {
        res = (eOresult_t) hal_can_out_get((hal_can_port_t)canport, &numofoutframe);
        if(eores_OK != res)
        {
            return(res);
        }
        p->waittxdata[canport].numoftxframe2send = numofoutframe;
//    }
    return((eOresult_t)hal_can_transmit((hal_can_port_t)canport));
}

extern void eo_appCanSP_WaitTransmitCanFrames(EOappCanSP *p, eOcanport_t canport)
{
    if(NULL == p)
    {
        return;
    }
    //Commented because always true
//     if(!p->cfg.waitallframesaresent)
//     {
//         return;
//     }
    
    if(p->waittxdata[canport].numoftxframe2send > 0) //if some frames wait to be sent, then suspend me
    {
        osal_semaphore_decrement(p->waittxdata[canport].semaphore, osal_reltimeINFINITE);
    }
}



extern eOresult_t eo_appCanSP_SendMessage_TEST(EOappCanSP *p, uint8_t *payload_ptr)
{
    eOresult_t                                  res;
    eOcanframe_t                                canFrame;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    //form can frame
    canFrame.id = 10;
    canFrame.id_type = 0; //standard id
    canFrame.frame_type = 0; //data frame
    canFrame.size = 8;

    memcpy(canFrame.data, payload_ptr, 8);

    res = (eOresult_t)hal_can_put(hal_can_port1, (hal_can_frame_t*)&canFrame, 
                                 ((p->runmode == eo_appCanSP_runMode__onEvent)? hal_can_send_normprio_now : hal_can_send_normprio_later));
    
     return(res);
}






#ifdef _USE_PROTO_TEST_
extern eOresult_t eo_appCanSP_ConfigMotor(EOappCanSP *p, eOmc_motorId_t mId, eOmc_motor_config_t *cfg)
{
    eOresult_t res;
    eOcanframe_t canFrame;
    eo_icubCanProto_msgDestination_t dest;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t canLoc;
    eo_icubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    res = eo_emsCanNetTopo_GetMotorCanLocation_ByMotorId(p->emsCanNetTopo_ptr, mId, &canLoc, NULL);
    if(eores_OK != res)
    {
        return(res);
    }

    //set destination of message (one for all msg)
    dest.axis = canLoc.jm_idInBoard;
    dest.canAddr = canLoc.canaddr;


    // 1) send current pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PID;
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->pidcurrent, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame,
                                  ((p->runmode == eo_appCanSP_runMode__onEvent)? hal_can_send_normprio_now : hal_can_send_normprio_later));
    if(eores_OK != res)
    {
        return(res);
    }

//     // 2) send current pid limits
//     msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_PIDLIMITS;
//     res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->pidcurrent, &canFrame);
//     if(eores_OK != res)
//     {
//         return(res);
//     }
//     res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame,
//                                    ((p->runmode == eo_appCanSP_runMode__onEvent)? hal_can_send_normprio_now : hal_can_send_normprio_later));
//     if(eores_OK != res)
//     {
//         return(res);
//     }

//     // 3) set max velocity   
//     msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_MAX_VELOCITY;
//     res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->maxvelocityofmotor, &canFrame);
//     if(eores_OK != res)
//     {
//         return(res);
//     }
//     res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame,
//                                    ((p->runmode == eo_appCanSP_runMode__onEvent)? hal_can_send_normprio_now : hal_can_send_normprio_later));
//     if(eores_OK != res)
//     {
//         return(res);
//     }

//     // 4) set current limit  
//     msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT;
//     res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, (void*)&cfg->maxcurrentofmotor, &canFrame);
//     if(eores_OK != res)
//     {
//         return(res);
//     }
//     res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame,
//                                   ((p->runmode == eo_appCanSP_runMode__onEvent)? hal_can_send_normprio_now : hal_can_send_normprio_later));
//     if(eores_OK != res)
//     {
//         return(res);
//     }


//     // 5) set perriodic msg contents  ??? DOVE PRENDO L'INFO
// //    res = eo_icubCanProto_formCanFrame4MotorBoard(ICUBCANPROTO_POL_MB_CMD__SET_CURRENT_LIMIT, (void*)&cfg->maxcurrentofmotor,
// //                                                  boardAddr, axis, &canFrame);
// //    if(eores_OK != res)
// //    {
// //        return(res);
// //    }
// //    res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
// //    if(eores_OK != res)
// //    {
// //        return(res);
// //    }


//     // 6) set i2t param      MANCANO!!!
// //    res = eo_icubCanProto_formCanFrame4MotorBoard(ICUBCANPROTO_POL_MB_CMD__SET_I2T_PARAMS, (void*)&cfg->velocityshiftfactor,
// //                                                  boardAddr, axis, &canFrame);
// //    if(eores_OK != res)
// //    {
// //        return(res);
// //    }
// //    res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
// //    if(eores_OK != res)
// //    {
// //        return(res);
// //    }


// //enable pwm and control loop

// //     // 5) set pwm pad
// //     msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__ENABLE_PWM_PAD;
// //     res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, NULL, &canFrame);
// //     if(eores_OK != res)
// //     {
// //         return(res);
// //     }
// //     res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
// //     if(eores_OK != res)
// //     {
// //         return(res);
// //     }


// //     // 6) set controller run
// //     msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__CONTROLLER_RUN;
// //     res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, NULL, &canFrame);
// //     if(eores_OK != res)
// //     {
// //         return(res);
// //     }
// //     res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
// //     if(eores_OK != res)
// //     {
// //         return(res);
// //     }

// //#warning aggiunto un messaggio x test!!!    
// //    // 7) set controller run
// //    msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__DISABLE_PWM_PAD;
// //    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, NULL, &canFrame);
// //    if(eores_OK != res)
// //    {
// //        return(res);
// //    }
// //    res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now);
// //    if(eores_OK != res)
// //    {
// //        return(res);
// //    }
    return(eores_OK);
}
#endif




#ifdef _USE_PROTO_TEST_
extern eOresult_t eo_appCanSP_SendSetPoint(EOappCanSP *p, eOmc_jointId_t jId, eOmc_setpoint_t *setPoint)
{
    eOresult_t res;
    eOcanframe_t canFrame;
    eo_icubCanProto_msgDestination_t dest;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t canLoc;
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
    res = eo_emsCanNetTopo_GetJointCanLocation_ByJointId(p->emsCanNetTopo_ptr, jId, &canLoc, NULL);
    if(eores_OK != res)
    {
        return(res);
    }

    dest.axis = canLoc.jm_idInBoard;
    dest.canAddr = canLoc.canaddr;


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

    res = (eOresult_t)hal_can_put((hal_can_port_t)canLoc.emscanport, (hal_can_frame_t*)&canFrame,
                                  ((p->runmode == eo_appCanSP_runMode__onEvent)? hal_can_send_normprio_now : hal_can_send_normprio_later));

    if(eores_OK != res)
    {
        return(res);
    }

    return(eores_OK);
}
#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_appCanSP_canPeriphInit(EOappCanSP *p)
{
    hal_can_cfg_t can_cfg_port1, can_cfg_port2;
    eOresult_t res;

    can_cfg_port1.runmode            = hal_can_runmode_isr_1txq1rxq;
    can_cfg_port1.baudrate           = hal_can_baudrate_1mbps; 
    can_cfg_port1.priorx             = hal_int_priority11;
    can_cfg_port1.priotx             = hal_int_priority11;
    can_cfg_port1.callback_on_rx     = NULL;
    can_cfg_port1.arg_cb_rx          = NULL;
    if(p->cfg.waitallframesaresent)
    {
        can_cfg_port1.callback_on_tx     = s_eo_appCanSP_callbackOnTx_port1_waittransmission;
        can_cfg_port1.arg_cb_tx          = (void*)p;
    }
    else
    {
        can_cfg_port1.callback_on_tx     = NULL;
        can_cfg_port1.arg_cb_tx          = NULL;
    }

    
    can_cfg_port2.runmode            = hal_can_runmode_isr_1txq1rxq;
    can_cfg_port2.baudrate           = hal_can_baudrate_1mbps; 
    can_cfg_port2.priorx             = hal_int_priority11;
    can_cfg_port2.priotx             = hal_int_priority11;
    can_cfg_port2.callback_on_rx     = NULL;
    can_cfg_port2.arg_cb_rx          = NULL;
    if(p->cfg.waitallframesaresent)
    {
        can_cfg_port2.callback_on_tx     = s_eo_appCanSP_callbackOnTx_port2_waittransmission;
        can_cfg_port2.arg_cb_tx          = (void*)p;
    }
    else
    {
        can_cfg_port2.callback_on_tx     = NULL;
        can_cfg_port2.arg_cb_tx          = NULL;

    }


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



static void s_eo_appCanSP_callbackOnTx_port1_waittransmission(void *arg)
{
    s_eo_appCanSP_callbackOnTx_portx_waittransmission(arg, hal_can_port1);
}

static void s_eo_appCanSP_callbackOnTx_port2_waittransmission(void *arg)
{
    s_eo_appCanSP_callbackOnTx_portx_waittransmission(arg, hal_can_port2);
}


static void s_eo_appCanSP_callbackOnTx_portx_waittransmission(void *arg, hal_can_port_t port)
{
    EOappCanSP *p = (EOappCanSP *)arg;
    if(eobool_false == p->waittxdata[port].waitenable)
    {
        return;
    }
    
    p->waittxdata[port].numoftxframe2send --;
    
    if(0 == p->waittxdata[port].numoftxframe2send)
    {
        osal_semaphore_increment(p->waittxdata[port].semaphore, osal_callerISR);
    }
}


static eOresult_t s_eo_appCanSP_formAndSendFrame(EOappCanSP *p, eOcanport_t emscanport, eOicubCanProto_msgDestination_t dest, eOicubCanProto_msgCommand_t msgCmd, void *val_ptr)
{
    eOresult_t          res;
    eOcanframe_t        canFrame;

    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, val_ptr, &canFrame);
    if(eores_OK != res)
    {
        return(res);
    }
    if(eo_appCanSP_runMode__onEvent == p->runmode)
    {
        res = (eOresult_t)hal_can_put((hal_can_port_t)emscanport, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now );
        if(eores_OK != res)
        {
            if(eores_NOK_busy == res)
            {
                hal_arch_arm_irqn_t irqn = (eOcanport1 == emscanport)? hal_arch_arm_CAN1_TX_IRQn : hal_arch_arm_CAN2_TX_IRQn;
                
                hal_sys_irqn_disable(irqn);
                p->waittxdata[emscanport].waitenable = eobool_true;
                p->waittxdata[emscanport].numoftxframe2send = 1;
                hal_sys_irqn_enable(irqn);
                
                osal_semaphore_decrement(p->waittxdata[emscanport].semaphore, osal_reltimeINFINITE);
                
                //if i'm here i just wake up
                hal_sys_irqn_disable(irqn);
                p->waittxdata[emscanport].waitenable = eobool_false;
                hal_sys_irqn_enable(irqn);
                res = (eOresult_t)hal_can_put((hal_can_port_t)emscanport, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_now );
                if(eores_OK != res)
                {
                   eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "error in hal_can_put!"); 
                }
            }
            else
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "error in hal_can_put");
            }
        }
    }
    else // eo_appCanSP_runMode__onDemand
    {
        res = (eOresult_t)hal_can_put((hal_can_port_t)emscanport, (hal_can_frame_t*)&canFrame, hal_can_send_normprio_later);
        if(eores_NOK_busy == res)
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "lost can frame (out-queue full)");
        }
    }
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



