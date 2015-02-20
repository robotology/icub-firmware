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
#include "stdio.h"

//abs
#include "hal.h"

#if     defined(HAL_USE_VERSION_2) || defined(HAL_IS_VERSION_2)
#include "hal_mpu_name_stm32f407ig.h"
#include "hal_core_cfg.h"
#else
#error --> hal version 1 is not supported anymore
#endif

//embobj
#include "EoCommon.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EoError.h"

//embobj-icub
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"

//embobj-icub-cfg
#include "eOcfg_icubCanProto_messagesLookupTbl.h"

//appl 
#include "EOappTheDataBase.h"

//diagnostics
#include "EOtheEMSapplDiagnostics.h"


#include "EOicubCanProto_hid.h"

#include "osal.h"

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

#define eoappCanSP_timeoutsenddiagnostics           1000
#define eoappCanSP_onEvtMode_timeoutSendFrame       2000
//#define eoappCanSP_onDemandMode_timeoutSendFrame    17000


//#if     defined(HAL_USE_VERSION_2) || defined(HAL_IS_VERSION_2)  
    #define EOAPPCANSP_CAN1_TX_IRQN     hal_mpu_name_stm32f407ig_CAN1_TX_IRQn
    #define EOAPPCANSP_CAN2_TX_IRQN     hal_mpu_name_stm32f407ig_CAN2_TX_IRQn   
//#else
//    #define EOAPPCANSP_CAN1_TX_IRQN     hal_arch_arm_CAN1_TX_IRQn
//    #define EOAPPCANSP_CAN2_TX_IRQN     hal_arch_arm_CAN2_TX_IRQn   
//#endif  


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

//typedef struct
//{
//    uint8_t isrunning;
//    uint8_t numoftxframe[2];
//    osal_semaphore_t*   semafori[2];
//} runnning_data_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_appCanSP_canPeriphInit(EOappCanSP *p);
static eOresult_t s_eo_appCanSP_formAndSendFrame(EOappCanSP *p, eOcanport_t emscanport, eOicubCanProto_msgDestination_t dest, 
                                                 eOicubCanProto_msgCommand_t msgCmd, void *val_ptr);
static void s_eo_appCanSP_callbackOnTx_port1_waittransmission(void *arg);
static void s_eo_appCanSP_callbackOnTx_port2_waittransmission(void *arg);
static void s_eo_appCanSP_callbackOnTx_portx_waittransmission(void *arg, hal_can_port_t port);

static void s_eo_appCanSP_callbackOnRx_port1_alertOnReception(void *arg);
static void s_eo_appCanSP_callbackOnRx_port2_alertOnReception(void *arg);
static void s_eo_appCanSP_callbackOnRx_portx_alertOnReception(void *arg, hal_can_port_t port);

static void s_eo_appCanSP_callbackOnErr_port1(void *arg);
static void s_eo_appCanSP_callbackOnErr_port2(void *arg);
static void s_eo_appCanSP_callbackOnErr_portx(void *arg, eOcanport_t port);

static void s_eo_appCanSP_updateDiagnosticValues(EOappCanSP *p, eOcanport_t port);
static void s_eo_appCanSP_clearDiagnosticValues(EOappCanSP *p, eOcanport_t port);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOappCanSP";

static eOappCanSP_cfg_t defaultcfg = 
{
    .cbkonrx    = 
    {
        // port1
        {
            .fn         = NULL,
            .argoffn    = NULL
        },
        // port2
        {
            .fn         = NULL,
            .argoffn    = NULL
        }
    }
};



static volatile uint8_t s_eo_appCanSP_numtx[2] = {0 ,0};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOappCanSP* eo_appCanSP_New(eOappCanSP_cfg_t *cfg)
{
    eOresult_t      res;
    EOappCanSP      *retptr = NULL;
    EOicubCanProto  *icubCanProto_ptr;

    if(NULL == cfg)
    {
        cfg = &defaultcfg;
    }
    

    eo_icubCanProto_cfg_t icubCanProto_cfg = 
    {
        EO_INIT(.msgClasses_LUTbl__ptr)     icubCanProto_msgClasses_lookupTbl__ptr 
    };



//1) initialise icubCanProto
    icubCanProto_ptr = eo_icubCanProto_New(&icubCanProto_cfg);
    if(NULL == icubCanProto_ptr)
    {
        return(NULL);
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
    

    extern const hal_cfg_t *hal_cfgMINE;
    //can port 1
    //the max num of token in semaphore is equal to the max num of frame in can out queue.
    retptr->waittxdata[hal_can_port1].semaphore = osal_semaphore_new(255/*maxtokens*/, 0/*current num of token*/);
    if(NULL == retptr->waittxdata[hal_can_port1].semaphore)
    {
        return(NULL);
    }
    osal_semaphore_set(retptr->waittxdata[hal_can_port1].semaphore, 0);
    
    //can port 2
    //the max num of token in semaphore is equal to the max num of frame in can out queue.
    retptr->waittxdata[hal_can_port2].semaphore = osal_semaphore_new(255/*maxtokens*/, 0/*current num of token*/);
    if(NULL == retptr->waittxdata[hal_can_port2].semaphore)
    {
        return(NULL);
    }
    
    osal_semaphore_set(retptr->waittxdata[hal_can_port2].semaphore, 0);
        

// 4) initialise peripheral
    res = (eOresult_t)s_eo_appCanSP_canPeriphInit(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }
    
    
    retptr->run_data.isrunning = 0;
    
    
    retptr->run_data.numoftxframe[0] = 0;
    retptr->run_data.semafori[0] = osal_semaphore_new(255/*maxtokens*/, 0/*current num of token*/);
    if(retptr->run_data.semafori[0] == NULL)
    {
        return(NULL);
    }
    retptr->run_data.semafori[1] = osal_semaphore_new(255/*maxtokens*/, 0/*current num of token*/);
    retptr->run_data.numoftxframe[1] = 0;
    if(retptr->run_data.semafori[1] == NULL)
    {
        return(NULL);
    }

    return(retptr);
}


extern eOresult_t eo_appCanSP_starttransmit_XXX(EOappCanSP *p, eOcanport_t port, uint8_t *numofTXframe)
{
    uint8_t numofoutframe = 0;
    hal_irqn_t irqn = (eOcanport1 == port)? EOAPPCANSP_CAN1_TX_IRQN : EOAPPCANSP_CAN2_TX_IRQN;    
 
   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    //disa tx
    hal_sys_irqn_disable(irqn);
    
    
    // set num of can frame in out queue
    hal_can_out_get((hal_can_port_t)port, &numofoutframe);
    s_eo_appCanSP_numtx[port] = numofoutframe;
    p->run_data.numoftxframe[port] = numofoutframe;
    
    hal_sys_irqn_enable(irqn);
    
    if(numofoutframe != 0)
    {
        hal_can_transmit((hal_can_port_t)port);
    }
    
    if(p->periphstatus[port].isnewvalue)
    {
        s_eo_appCanSP_updateDiagnosticValues(p, port);
        eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsperiph , eoappCanSP_timeoutsenddiagnostics);
        s_eo_appCanSP_clearDiagnosticValues(p, port);
    }

    if(NULL != numofTXframe)
    {
        *numofTXframe = numofoutframe;
    }
    return(eores_OK);
}



extern eOresult_t eo_appCanSP_wait_XXX(EOappCanSP *p, eOcanport_t port)
{
    osal_result_t           osal_res = osal_res_OK ;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(0 != p->run_data.numoftxframe[port])
    {
        osal_res = osal_semaphore_decrement(p->run_data.semafori[port], 3*osal_reltime1ms); 
        //#warning marco.accame: in here i put a timeout of 3 ms as there were problems (before it was 17 ms)
        if(osal_res != osal_res_OK)
        {            
            uint8_t sizeoftxfifo = 0;
            hal_can_out_get((hal_can_port_t)port, &sizeoftxfifo);
            eOerrmanDescriptor_t errdes = {0};
            errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txbusfailure);
            errdes.par16                = 0x0001;
            errdes.par16                |= ((uint16_t)sizeoftxfifo << 8);
            errdes.par64                = 0; // dont knw what to send up           
            errdes.sourcedevice         = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            errdes.sourceaddress        = 0;                                   
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);    
        }
    }
    return((eOresult_t)osal_res);
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
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
    #warning marco.accame: eOicubCanProto_msgDestination_t is a union with a strcut in bitfields .... are we sure that it is safe to use?
    res = s_eo_appCanSP_formAndSendFrame(p, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, val_ptr);
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

    // set destination of message 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
    
    res = s_eo_appCanSP_formAndSendFrame(p, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, val_ptr);
    return(res);
}


extern eOresult_t eo_appCanSP_SendCmd2SnrMais(EOappCanSP *p, eOas_maisId_t sId, eOicubCanProto_msgCommand_t msgCmd, void *val_ptr)
{
    eOresult_t                                  res;
    eOappTheDB_board_canlocation_t              canLoc = {0};
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
    res = s_eo_appCanSP_formAndSendFrame(p, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, val_ptr);
    
    return(res);
    
}

extern eOresult_t eo_appCanSP_SendCmd2SnrStrain(EOappCanSP *p, eOas_strainId_t sId, eOicubCanProto_msgCommand_t msgCmd, void *val_ptr)
{
    eOresult_t                                  res;
    eOappTheDB_board_canlocation_t              canLoc = {0};
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
    res = s_eo_appCanSP_formAndSendFrame(p, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, val_ptr);
    
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


static eObool_t s_eoapp_cansp_is_from_unused2foc_in_eb5(eOcanframe_t *frame, eOcanport_t canPortRX)
{
    eOprotBRD_t localboard = eoprot_board_local_get();
    const eOprotBRD_t eb5board = 4;
    
    if(localboard != eb5board)
    {
        return(eobool_false);
    }
    
    if( (eOcanport1 == canPortRX) &&  (2 == eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id)) )
    {
        return(eobool_true);
    }
    else
    {
        return(eobool_false);
    }    
    
}

extern eOresult_t eo_appCanSP_read(EOappCanSP *p, eOcanport_t canport, uint8_t numofcanframe, uint8_t *numofreadcanframes)
{
    eOresult_t          res = eores_NOK_generic;
    hal_result_t        halres = hal_res_NOK_nodata;
    hal_can_frame_t     canframe = {0};
    uint8_t             i;
    uint8_t readcanframes = 0;
    

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    for(i=0; i<numofcanframe ; i++)
    {
        memset(&canframe, 0, sizeof(hal_can_frame_t));
        
        halres = hal_can_get((hal_can_port_t)canport, &canframe, NULL);
        if(hal_res_OK != halres)
        {
            break;      // marco.accame on 12 jan 2015: changed the original continue in a break because:
                        // if we have a NOK then we cannot go on because the fifo is surely empty.                     
        }

        readcanframes ++;
                      
        res = eo_icubCanProto_ParseCanFrame(p->icubCanProto_ptr, (eOcanframe_t*)&canframe, (eOcanport_t)canport);

        if((eores_OK != res) && (eobool_false == s_eoapp_cansp_is_from_unused2foc_in_eb5((eOcanframe_t*)&canframe, (eOcanport_t)canport))) 
        {  
            eOerrmanDescriptor_t errdes = {0};
            errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_parsingfailure);
            errdes.par16                = (canframe.id & 0x0fff) | ((canframe.size & 0x000f) << 12);
            errdes.par64                = eo_common_canframe_data2u64((eOcanframe_t*)&canframe);
            errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            errdes.sourceaddress        = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(canframe.id);                
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);             
        }
    }
    
    if(NULL != numofreadcanframes)
    {
        *numofreadcanframes = readcanframes;    
    }

    if(p->periphstatus[canport].isnewvalue)
    {
        s_eo_appCanSP_updateDiagnosticValues(p, canport);
        eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsperiph , eoappCanSP_timeoutsenddiagnostics);
        s_eo_appCanSP_clearDiagnosticValues(p, canport);
    }
    
    return(eores_OK);
}

extern eOresult_t eo_appCanSP_GetNumOfRecCanframe(EOappCanSP *p, eOcanport_t canport, uint8_t *numofRXcanframe)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    // here not check if numofRXcanframe is null because hal function already do it.   
    return((eOresult_t)hal_can_received((hal_can_port_t)canport, numofRXcanframe));
}


extern eOresult_t eo_appCanSP_GetNumOfTxCanframe(EOappCanSP *p, eOcanport_t canport, uint8_t *numofTXcanframe)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    // here not check if numofRXcanframe is null because hal function already do it.   
    return((eOresult_t)hal_can_out_get((hal_can_port_t)canport, numofTXcanframe));
}


extern eOresult_t eo_appCanSP_SetRunMode(EOappCanSP *p, eo_appCanSP_runMode_t runmode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    p->runmode = runmode;
    
//     if(eo_appCanSP_runMode__onDemand ==  p->runmode)
//     {
//         p->waittxdata[hal_can_port1].waitenable = eobool_true;
//         p->waittxdata[hal_can_port2].waitenable = eobool_true;
//     }
//     else
//     {
        p->waittxdata[hal_can_port1].waitenable = eobool_false;
        p->waittxdata[hal_can_port2].waitenable = eobool_false;
//     }
    
    if(eo_appCanSP_runMode__onDemand ==  p->runmode)
    {
        p->run_data.isrunning = 1;
    }
    else
    {
        p->run_data.isrunning = 0;
    }
    
    return(eores_OK);
}

extern eOresult_t eo_appCanSP_StartTransmitCanFrames(EOappCanSP *p, eOcanport_t canport, eOboolvalues_t waitflag)
{
    uint8_t                 numofoutframe = 0;
    eOresult_t              res = eores_OK;
    hal_result_t            halres;


    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    hal_irqn_t irqn = (eOcanport1 == canport)? EOAPPCANSP_CAN1_TX_IRQN : EOAPPCANSP_CAN2_TX_IRQN;  
     

    // disa tx
    hal_sys_irqn_disable(irqn);
    
    // get num of can frame in out queue
    hal_can_out_get((hal_can_port_t)canport, &numofoutframe);

    if((waitflag) && (numofoutframe > 0))     // prepare data for wait sending can frame
    {
        p->waittxdata[canport].waitenable = eobool_true;
    }
    p->waittxdata[canport].numoftxframe2send = numofoutframe;

    // ena tx
    hal_sys_irqn_enable(irqn);

    
    if(0 != numofoutframe)
    {
        halres = hal_can_transmit((hal_can_port_t)canport);
        res = (hal_res_OK == halres) ? (eores_OK):(eores_NOK_generic);
    }
    
    return(res);
}


extern eOresult_t eo_appCanSP_WaitTransmitCanFrames(EOappCanSP *p, eOcanport_t canport)
{
    osal_result_t osal_res;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(p->waittxdata[canport].waitenable == eobool_false)
    {
        return(eores_NOK_nodata);
    }
    
    
    //#warning marco.accame: put a timeout of 5 ms (before it was infinite)
    osal_result_t osalres = osal_semaphore_decrement(p->run_data.semafori[canport], 5*osal_reltime1ms);
    
    if(osal_res_OK != osalres)
    {
        // manage error
        eOerrmanDescriptor_t errdes = {0};        
        uint8_t sizeoftxfifo = 0;
        hal_can_out_get((hal_can_port_t)canport, &sizeoftxfifo);       
        errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txbusfailure);
        errdes.par16                = 0x0002;
        errdes.par16                |= ((uint16_t)sizeoftxfifo << 8);
        errdes.par64                = 0; // dont knw what to send up
        errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        errdes.sourceaddress        = 0;                         
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);    
    }    
    
    
    // if i'm here i wake up
    hal_irqn_t irqn = (eOcanport1 == canport)? EOAPPCANSP_CAN1_TX_IRQN : EOAPPCANSP_CAN2_TX_IRQN;
   
    //disa tx
    hal_sys_irqn_disable(irqn);
    
    p->waittxdata[canport].waitenable = eobool_false;
    p->waittxdata[canport].numoftxframe2send = 0;
    
    //ena tx
    hal_sys_irqn_enable(irqn);
    
    return((eOresult_t)osal_res);

}

extern eOresult_t eo_appCanSP_StartTransmitAndWait(EOappCanSP *p, eOcanport_t canport)
{
    uint8_t numofoutframe = 0;
    uint8_t after = 0;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }


    hal_irqn_t irqn = (eOcanport1 == canport)? EOAPPCANSP_CAN1_TX_IRQN : EOAPPCANSP_CAN2_TX_IRQN;

    // disa tx
    hal_sys_irqn_disable(irqn);
    
    p->waittxdata[canport].waitenable = eobool_true;
    
    // get num of can frame in out queue
    hal_can_out_get((hal_can_port_t)canport, &numofoutframe);
    p->waittxdata[canport].numoftxframe2send = numofoutframe;
    // ena tx
    hal_sys_irqn_enable(irqn);
    
    hal_can_transmit((hal_can_port_t)canport);
    
    //#warning marco.accame: put a timeout of 5 ms (before it was infinite)
    osal_result_t osalres = osal_semaphore_decrement(p->waittxdata[canport].semaphore, 5*osal_reltime1ms);
    
    if(osal_res_OK != osalres)
    {
        // manage error        
        eOerrmanDescriptor_t errdes = {0};
        uint8_t sizeoftxfifo = 0;
        hal_can_out_get((hal_can_port_t)canport, &sizeoftxfifo);
        errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txbusfailure);
        errdes.par16                = 0x0003;
        errdes.par16                |= ((uint16_t)sizeoftxfifo << 8);
        errdes.par64                = 0; // dont knw what to send up
        errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        errdes.sourceaddress        = 0;                  
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);    
    }
    
    // if i'm here i just wake up
    hal_sys_irqn_disable(irqn);
    p->waittxdata[canport].waitenable = eobool_false;    
    hal_can_out_get((hal_can_port_t)canport, &after);
  
    
    hal_sys_irqn_enable(irqn);

    return(eores_OK);
    
}

extern eOresult_t eo_appCanSP_EmptyCanOutputQueue(EOappCanSP *p, eOcanport_t canport)
{
    uint8_t numberofoutcanframe = 0;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    hal_irqn_t irqn = (eOcanport1 == canport)? EOAPPCANSP_CAN1_TX_IRQN : EOAPPCANSP_CAN2_TX_IRQN;
   
    // disa tx
    hal_sys_irqn_disable(irqn);
    
    //set num of can frame in out queue
    hal_can_out_get((hal_can_port_t)canport, &numberofoutcanframe);
    
    if(0 == numberofoutcanframe)
    {
        // ena tx
        hal_sys_irqn_enable(irqn);
        // the queue is empty, so i do nothing.
        return(eores_OK);
    }
    
    p->waittxdata[canport].waitenable = eobool_true;
    p->waittxdata[canport].numoftxframe2send = numberofoutcanframe;
    
    // ena tx
    hal_sys_irqn_enable(irqn);
    
    // wait until all can frame are sent
    //#warning --> put a timeout of 5 ms in here, otherwise there are errors when entering the RUN if the can does not have attached boards.
    osal_result_t osalres = osal_semaphore_decrement(p->waittxdata[canport].semaphore, 5*osal_reltime1ms);
    
    if(osal_res_OK != osalres)
    {
        // manage error        
        eOerrmanDescriptor_t errdes = {0};
        uint8_t sizeoftxfifo = 0;
        hal_can_out_get((hal_can_port_t)canport, &sizeoftxfifo);
        errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txbusfailure);
        errdes.par16                = 0x0004;
        errdes.par16                |= ((uint16_t)sizeoftxfifo << 8);
        errdes.par64                = 0; // dont knw what to send up
        errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        errdes.sourceaddress        = 0;                   
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);    
    }
 
    // if i'm here i just wake up
    hal_sys_irqn_disable(irqn);
    p->waittxdata[canport].waitenable = eobool_false;        
    hal_sys_irqn_enable(irqn);
    
    return(eores_OK);
}

extern eOresult_t eo_appCanSP_EmptyCanInputQueue(EOappCanSP *p, eOcanport_t canport)
{
    eOresult_t res;
    uint8_t numofRXcanframe = 0;
    
    res = eo_appCanSP_GetNumOfRecCanframe(p, canport, &numofRXcanframe);
    if(eores_OK != res)
    {
        return(res);
    }
    res = eo_appCanSP_read(p, canport, numofRXcanframe, NULL);
    return(res);
}


extern eOresult_t eo_appCanSP_SendMessage_TEST(EOappCanSP *p, uint8_t *payload_ptr)
{
    eOresult_t                                  res;
    eOcanframe_t                                canFrame;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    // form can frame
    canFrame.id = 10;
    canFrame.id_type = 0; // standard id
    canFrame.frame_type = 0; //data frame
    canFrame.size = 8;

    memcpy(canFrame.data, payload_ptr, 8);

    res = (eOresult_t)hal_can_put(hal_can_port1, (hal_can_frame_t*)&canFrame, 
                                 ((p->runmode == eo_appCanSP_runMode__onEvent)? hal_can_send_normprio_now : hal_can_send_normprio_later));
    
     return(res);
}





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
//#if     defined(HAL_USE_VERSION_2) || defined(HAL_IS_VERSION_2)    
    can_cfg_port1.capacityofrxfifoofframes   = HALCAN1capacityofrxfifoofframes;
    can_cfg_port1.capacityoftxfifoofframes   = HALCAN1capacityoftxfifoofframes;
    can_cfg_port1.capacityoftxfifohighprio   = HALCAN1capacityoftxfifohighprio;
//#endif       
    can_cfg_port1.callback_on_rx     = s_eo_appCanSP_callbackOnRx_port1_alertOnReception;
    can_cfg_port1.arg_cb_rx          = (void*)p;
    can_cfg_port1.callback_on_tx     = s_eo_appCanSP_callbackOnTx_port1_waittransmission;
    can_cfg_port1.arg_cb_tx          = (void*)p;
    can_cfg_port1.callback_on_err    = s_eo_appCanSP_callbackOnErr_port1;
    can_cfg_port1.arg_cb_err         = (void*)p;


    
    can_cfg_port2.runmode            = hal_can_runmode_isr_1txq1rxq;
    can_cfg_port2.baudrate           = hal_can_baudrate_1mbps; 
    can_cfg_port2.priorx             = hal_int_priority11;
    can_cfg_port2.priotx             = hal_int_priority11;
//#if     defined(HAL_USE_VERSION_2) || defined(HAL_IS_VERSION_2)    
    can_cfg_port2.capacityofrxfifoofframes   = HALCAN2capacityofrxfifoofframes;
    can_cfg_port2.capacityoftxfifoofframes   = HALCAN2capacityoftxfifoofframes;
    can_cfg_port2.capacityoftxfifohighprio   = HALCAN2capacityoftxfifohighprio;    
//#endif    
    can_cfg_port2.callback_on_rx     = s_eo_appCanSP_callbackOnRx_port2_alertOnReception;
    can_cfg_port2.arg_cb_rx          = (void*)p;
    can_cfg_port2.callback_on_tx     = s_eo_appCanSP_callbackOnTx_port2_waittransmission;
    can_cfg_port2.arg_cb_tx          = (void*)p;
    can_cfg_port2.callback_on_err    = s_eo_appCanSP_callbackOnErr_port2;
    can_cfg_port2.arg_cb_err         = (void*)p;

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


//extern uint8_t numofsentframe;

static void s_eo_appCanSP_callbackOnTx_portx_waittransmission(void *arg, hal_can_port_t port)
{
    EOappCanSP *p = (EOappCanSP *)arg;
    
    if(eobool_true == p->waittxdata[port].waitenable)
    {
        p->waittxdata[port].numoftxframe2send --;
        
        if(0 == p->waittxdata[port].numoftxframe2send)
        {
            p->waittxdata[port].waitenable = eobool_false;
            
            #warning --> marco.accame: i think that we should set p->waittxdata[port].waitenable to eobool_false in here and not inside user-level code
            // reason is ... if we dont change it to false quick enough, it may be that numoftxframe2send is further decremented.    
            
            if(osal_res_OK != osal_semaphore_increment(p->waittxdata[port].semaphore, osal_callerISR))
            {
                // marco.accame: osal_semaphore_increment() always returns osal_res_OK unless: called with NULL semaphore, 
                // or reached its maximum count. which is unlikely.
                #warning --> marco.accame: shall we put any eo_errman_Error() in here ? we are inside an ISR. better NO
                // but we may set a flag which will trigger an error later
                hal_trace_puts("error in s_eo_appCanSP_callbackOnTx_portx_waittransmission"); 
            }
        }
    }

    #warning --> marco.accame: review the s_eo_appCanSP_callbackOnTx_portx_waittransmission() function in this part
    // semafori ....        
    if(p->run_data.isrunning)
    {
        if(p->run_data.numoftxframe[port] == 0)
        {
            char str[100];
            snprintf(str, sizeof(str)-1, "Err isr numoftxframe[%d]=0 and numtx[port]=%d", port, s_eo_appCanSP_numtx[port]);        
            hal_trace_puts(str); 
        }    
        s_eo_appCanSP_numtx[port]--;
        if(0 == s_eo_appCanSP_numtx[port])
        {
            osal_result_t osal_res;
            osal_res  = osal_semaphore_increment(p->run_data.semafori[port], osal_callerISR);
            osal_res = osal_res;
//             if(osal_res != osal_res_OK)
//             {
//                 char str[100];
//                 snprintf(str, sizeof(str)-1, "Err osal incr = %d", osal_res );        
//                 hal_trace_puts(str); 
//             }

        }
    }
}

static void s_eo_appCanSP_callbackOnRx_port1_alertOnReception(void *arg)
{
    s_eo_appCanSP_callbackOnRx_portx_alertOnReception(arg, hal_can_port1);
}

static void s_eo_appCanSP_callbackOnRx_port2_alertOnReception(void *arg)
{
    s_eo_appCanSP_callbackOnRx_portx_alertOnReception(arg, hal_can_port2);
}


static void s_eo_appCanSP_callbackOnRx_portx_alertOnReception(void *arg, hal_can_port_t port)
{
    EOappCanSP *p = (EOappCanSP *)arg;
    
    if(eo_appCanSP_runMode__onDemand == p->runmode)
    {
        return;
    }
    //eom_task_isrSetEvent(p->alertonrxdata[port].task, p->alertonrxdata[port].evt);
    if(NULL == p->cfg.cbkonrx[port].fn)
    {
        return;
    }
    p->cfg.cbkonrx[port].fn(p->cfg.cbkonrx[port].argoffn);
}

static eOresult_t s_eo_appCanSP_formAndSendFrame(EOappCanSP *p, eOcanport_t canport, eOicubCanProto_msgDestination_t dest, eOicubCanProto_msgCommand_t msgCmd, void *val_ptr)
{
    eOresult_t          res;
    eOcanframe_t        canframe = {0};
    osal_result_t       osal_res;
    hal_result_t        hal_res = hal_res_NOK_generic;
    
#ifdef _GET_CANQUEUE_STATISTICS_
    uint8_t             numofoutframe=0;
#endif

    eOerrmanDescriptor_t errdes = {0};  
    uint8_t sizeoftxfifo = 0;    
    
    res = eo_icubCanProto_FormCanFrame(p->icubCanProto_ptr, msgCmd, dest, val_ptr, &canframe);
    if(eores_OK != res)
    {
        errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_formingfailure);
        errdes.par16                = (msgCmd.class << 8) | (msgCmd.cmdId);
        errdes.par64                = 0;
        errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        errdes.sourceaddress        = dest.s.canAddr;           
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);   
        
        return(res);
    }

    // tx the can frame
    if(eo_appCanSP_runMode__onEvent == p->runmode)
    {   
        // marco.accame: 
        // in here we have transmission of can frame just after it is put inside the tx-fifo
        // we typically are in cfg-mode. failure of tx if due to ... 
        // a. the isr not being too fast and the fifo getting full? very unlikely. 
        // b. can-bus wires are not connected and the fifo is never emptied. that is more likely.
        // in here there is an attempt of recovery but i believe that is hardy successful. i keep it nevertheless.
        
#ifdef _GET_CANQUEUE_STATISTICS_
        hal_can_out_get((hal_can_port_t)canport, &numofoutframe);
        eo_theEMSdgn_updateCanTXqueueStatisticsOnConfigMode(canport, numofoutframe+1);
#endif
        // marco.accame on nov 28, 2014
        // i try to put a frame into the fifo-tx queue. the only good result is hal_res_OK. 
        // other results can be:
        // - hal_res_NOK_generic (wrong parameters), which it does not happen because the port is valid and frame is not NULL
        // - hal_res_NOK_busy (full fifo-tx), which can happen.
        hal_res = hal_can_put((hal_can_port_t)canport, (hal_can_frame_t*)&canframe, hal_can_send_normprio_now );
        
        if(hal_res_OK == hal_res)
        {
            // marco.accame: we all are happy as hal_can_put() succesfully accepts a canframe in its tx fifo.
        }
        else
        {
            // problems ... it is worth send at least a warning.
            //hal_can_out_get((hal_can_port_t)canport, &sizeoftxfifo);
            errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txfifooverflow);
            errdes.par16                = (canframe.id & 0x0fff) | ((canframe.size & 0x000f) << 12);
            errdes.par64                = eo_common_canframe_data2u64((eOcanframe_t*)&canframe);
            errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            errdes.sourceaddress        = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(canframe.id);                             
            
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, s_eobj_ownname, &errdes);               
             
            
            if(hal_res_NOK_busy == hal_res)
            {   // marco.accame: it is the normal case in which the tx fifo is full    
                // i try a small recovery: wait until one frame is sent (by semaphore wait), and then attempt again with hal_can_put()
                
                hal_irqn_t irqn = (eOcanport1 == canport)? EOAPPCANSP_CAN1_TX_IRQN : EOAPPCANSP_CAN2_TX_IRQN;                
                
                hal_sys_irqn_disable(irqn);
                p->waittxdata[canport].waitenable        = eobool_true;
                p->waittxdata[canport].numoftxframe2send = 1; // one frame only
                hal_sys_irqn_enable(irqn);

                // wait for some time, but NOT FOREVER. otherwise the application hangs up
                osal_res = osal_semaphore_decrement(p->waittxdata[canport].semaphore, eoappCanSP_onEvtMode_timeoutSendFrame);
                
                // marco accame: i must reset these values only in case if osal_res is timed-out because otherwise the isr did that
                // .... but for now i keep the code as i have found it and i reset them anyway
                hal_sys_irqn_disable(irqn);
                p->waittxdata[canport].waitenable = eobool_false;
                p->waittxdata[canport].numoftxframe2send = 0; 
                hal_sys_irqn_enable(irqn);
                
                if(osal_res_OK == osal_res)
                {
                    // if the semaphore decremented succesfully, it means that the isr has removed at least one frame, 
                    // thus i can attempt a second trial of hal_can_put()
                    hal_res = hal_can_put((hal_can_port_t)canport, (hal_can_frame_t*)&canframe, hal_can_send_normprio_now );
                    if(eores_OK != res)
                    {
                        // marco.accame: it should not happen ... but we issue an error anyway.                        
                        //hal_can_out_get((hal_can_port_t)canport, &sizeoftxfifo);                        
                        errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txfifooverflow);
                        errdes.par16                = (canframe.id & 0x0fff) | ((canframe.size & 0x000f) << 12);
                        errdes.par64                = eo_common_canframe_data2u64((eOcanframe_t*)&canframe);
                        errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                        errdes.sourceaddress        = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(canframe.id);                         
                        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);   
                    }
                }
                else
                {
                    // the semaphore was not decremented correctly because the can-isr did not send a frame out
                    // issue an error of tx can bus failure
                    hal_can_out_get((hal_can_port_t)canport, &sizeoftxfifo);
                    errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txbusfailure);
                    errdes.par16                = 0x0005;
                    errdes.par16                |= ((uint16_t)sizeoftxfifo << 8);
                    errdes.par64                = 0; // dont knw what to send up
                    errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                    errdes.sourceaddress        = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(canframe.id); ;                     
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);                                           
                }
            }
            else
            {   // marco.accame: unlikely error which happens only in case of invalid parameters inside hal_can_put()
                errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_genericerror);
                errdes.par16                = 0x0001;
                errdes.par64                = 0;
                errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                errdes.sourceaddress        = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(canframe.id);    
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, s_eobj_ownname, &errdes);                   
            }
            
        }
        
    }
    else // eo_appCanSP_runMode__onDemand
    {
        // marco.accame: 
        // in here we just put the can frame inside the tx-fifo and we tramsmit the whole fifo only later
        // we typically are in run-mode. failure of tx if due to the fifo getting full.
        // this situation can happens, expecially if a received udp packet contains several ROPs which all produce many can-frames each.
        // that is the case of configuring joints/motors on mc4 boards or the skin boards
        
        hal_res = hal_can_put((hal_can_port_t)canport, (hal_can_frame_t*)&canframe, hal_can_send_normprio_later);
        
        //#warning --> capita qui il problema del can con skin-cfg... vedere di aumentare la dimensione del buffer se accade. cercare di capire sull skin-cfg
       
        if(hal_res_OK == hal_res)
        {
            // marco.accame: we all are happy as hal_can_put() succesfully accepts a canframe in its tx fifo.       
//            static uint16_t count = 0;
//            errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txfifoputisok);
//            errdes.par16                = (canframe.id & 0x0fff) | ((canframe.size & 0x000f) << 12);
//            errdes.par64                = eo_common_canframe_data2u64((eOcanframe_t*)&canframe);
//            errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
//            errdes.sourceaddress        = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(canframe.id);  
//            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);
        }
        else
        {
            if(hal_res_NOK_busy == hal_res)
            {
                // marco.accame: it is the normal case in which the tx fifo is full. we cannot do much but tell to increase capacity of tx fifo
                //hal_can_out_get((hal_can_port_t)canport, &sizeoftxfifo);
                errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_txfifooverflow);
                errdes.par16                = (canframe.id & 0x0fff) | ((canframe.size & 0x000f) << 12);
                errdes.par64                = eo_common_canframe_data2u64((eOcanframe_t*)&canframe);
                errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                errdes.sourceaddress        = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(canframe.id);                  
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);  
            }
            else
            {
                // marco.accame: unlikely error which happens only in case of invalid parameters inside hal_can_put()
                errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_genericerror);
                errdes.par16                = 0x0002;
                errdes.par64                = 0;
                errdes.sourcedevice         = (eOcanport1 == canport) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                errdes.sourceaddress        = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(canframe.id);                  
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);                   
            }
        }
    }
    
    if(p->periphstatus[canport].isnewvalue)
    {
        s_eo_appCanSP_updateDiagnosticValues(p, canport);
        eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsperiph , eoappCanSP_timeoutsenddiagnostics);
        s_eo_appCanSP_clearDiagnosticValues(p, canport);
    }
    
    res = (hal_res == hal_res_OK) ? (eores_OK) : (eores_NOK_generic);
    
    return(res);
}


static void s_eo_appCanSP_callbackOnErr_port1(void *arg)
{
    s_eo_appCanSP_callbackOnErr_portx(arg, eOcanport1);
}

static void s_eo_appCanSP_callbackOnErr_port2(void *arg)
{
    s_eo_appCanSP_callbackOnErr_portx(arg, eOcanport2);
}

static void s_eo_appCanSP_callbackOnErr_portx(void *arg, eOcanport_t port)
{
    EOappCanSP *p = (EOappCanSP *)arg;
    
    hal_can_status_t status = {0};
    
    hal_can_getstatus((hal_can_port_t)port, &status);

    if(1 == p->periphstatus[port].st.u.s.sw_status.rxqueueisfull)
    {
        // send msg about rx-fifo-overflow
        eOerrmanDescriptor_t errdes = {0};  
        uint8_t sizeofrxfifo = (eOcanport1 == port) ? (HALCAN1capacityofrxfifoofframes) : (HALCAN2capacityofrxfifoofframes);
        errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_rxfifooverflow);       
        errdes.par16                = 0x0001;
        errdes.par16                |= ((uint16_t)sizeofrxfifo << 8);
        errdes.par64                = 0; // dont knw what to send up           
        errdes.sourcedevice         = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        errdes.sourceaddress        = 0;          
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes);           
    }    

    // marco.accame: diagnostics about internals of can passes through here.
    #warning --> marco.accame: now can diagnostics is enabled only for rxfifo oveflow and txfifo overflow
#if 0    
    memcpy(&(p->periphstatus[port].st), &status, sizeof(hal_can_status_t));
    p->periphstatus[port].isnewvalue = 1;
#endif
}

static void s_eo_appCanSP_updateDiagnosticValues(EOappCanSP *p, eOcanport_t port)
{
    eo_dgn_emsperiph.can_dev[port].hw.warning = p->periphstatus[port].st.u.s.hw_status.warning;
    eo_dgn_emsperiph.can_dev[port].hw.passive = p->periphstatus[port].st.u.s.hw_status.passive;
    eo_dgn_emsperiph.can_dev[port].hw.busoff = p->periphstatus[port].st.u.s.hw_status.busoff;
    eo_dgn_emsperiph.can_dev[port].sw.txqueueisfull = p->periphstatus[port].st.u.s.sw_status.txqueueisfull;
    eo_dgn_emsperiph.can_dev[port].sw.rxqueueisfull = p->periphstatus[port].st.u.s.sw_status.rxqueueisfull;
}

static void s_eo_appCanSP_clearDiagnosticValues(EOappCanSP *p, eOcanport_t port)
{
    memset(&p->periphstatus[port].st, 0, sizeof(eOappCanSP_periphstatus_t));
    p->periphstatus[port].isnewvalue = 0;
}



// --------------------------------------------------------------------------------------------------------------------
// - old code
// --------------------------------------------------------------------------------------------------------------------


#ifdef _USE_PROTO_TEST_
extern eOresult_t eo_appCanSP_ConfigMotor(EOappCanSP *p, eOmc_motorId_t mId, eOmc_motor_config_t *cfg)
{
    eOresult_t res;
    eOcanframe_t canFrame;
    eo_icubCanProto_msgDestination_t dest;
    eo_emsCanNetTopo_jointOrMotorCanLocation_t canLoc;
    eo_icubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
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
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
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
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



