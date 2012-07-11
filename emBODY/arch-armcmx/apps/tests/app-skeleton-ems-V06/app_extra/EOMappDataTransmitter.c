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

/* @file       EOMappDataTransmitter.c
    @brief      This file contains internal implementation for EOMappDataTransmitter object
    @author     valentina.gaggero@iit.it
    @date       02/20/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"     // to see NULL, calloc etc.
#include "string.h"     //memcpy

// abstraction layers
#include "hal.h"

//embobj
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
//nvs
#include "EOtransceiver.h"
#include "EOtheBOARDtransceiver.h"

//only for debug purpose
#include "EOpacket_hid.h"

// application
#include "EoMotionControl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOMappDataTransmitter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOMappDataTransmitter_hid.h" 




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define EVT_CHECK(var, EVTMASK)             (EVTMASK == (var&EVTMASK))



#define TASK_DATATRANSMITTER_PRIO          60   // task piority

//************************ system controller task events **************************************************
#define EVT_START          (1 << 0)

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
extern  uint32_t ena_tx_onrx;
extern  int16_t pwm_out;
extern int32_t encoder_can;
extern int32_t posref_can;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
//function used to configure eOMtask obj
void s_eom_appDataTransmitter_taskInit(void *p);
static void s_eom_appDataTransmitter_taskStartup(EOMtask *p, uint32_t t);
static void s_eom_appDataTransmitter_taskRun(EOMtask *tsk, uint32_t evtmsgper); 

static void s_eom_appDataTransmitter_taskRun_skinOnly_mode(EOMappDataTransmitter *p, eOevent_t evt);
static void s_eom_appDataTransmitter_taskRun_2foc_mode(EOMappDataTransmitter *p, eOevent_t evt);
static void s_eom_appDataTransmitter_taskRun_skinAndMc4_mode(EOMappDataTransmitter *p, eOevent_t evt);
static eOresult_t s_eom_appDataTransmitter_SendRops(EOMappDataTransmitter *p);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static const char s_eobj_ownname[] = "EOMappDataTransmitter";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOMappDataTransmitter* eom_appDataTransmitter_New(EOMappDataTransmitter_cfg_t *cfg)
{
    EOMappDataTransmitter *retptr = NULL;


    if((NULL == cfg) || (NULL == cfg->appCanSP_ptr) || (NULL == cfg->eth_mod))
    {
        return(retptr);
    }


    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOMappDataTransmitter), 1);

    //save obj's configuration
    memcpy(&retptr->cfg, cfg, sizeof(EOMappDataTransmitter_cfg_t));
    
    retptr->appl_runMode = applrunMode__default;
    
    retptr->mytask = eom_task_New(eom_mtask_EventDriven,
                              TASK_DATATRANSMITTER_PRIO ,
                              2*1024,           //stacksize: da rivedere
                              s_eom_appDataTransmitter_taskStartup, 
                              s_eom_appDataTransmitter_taskRun,  
                              0,                //message queue size. the task is evt based 
                              eok_reltimeINFINITE,
                              retptr, 
                              s_eom_appDataTransmitter_taskInit, 
                              "dataTransmitter");

    retptr->st = eOm_appDataTransmitter_st__idle; 
    
    return(retptr);
}


extern eOresult_t eom_appDataTransmitter_Activate(EOMappDataTransmitter *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    p->st = eOm_appDataTransmitter_st__active;
    ena_tx_onrx = 0;
    return(eores_OK);
}


extern eOresult_t eom_appDataTransmitter_Deactivate(EOMappDataTransmitter *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    p->st = eOm_appDataTransmitter_st__idle;

    return(eores_OK);
}

extern eOresult_t eom_appDataTransmitter_SendData(EOMappDataTransmitter *p)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eOm_appDataTransmitter_st__idle == p->st)
    {
        return(eores_NOK_generic);    
    }

    //in i'm here i'm in activ state
    p->st = eOm_appDataTransmitter_st__transmitting;
    eom_task_SetEvent(p->mytask, EVT_START);
    return(eores_OK);
}


extern eOresult_t eom_appDataTransmitter_SetRunMode(EOMappDataTransmitter *p, eOmn_appl_runMode_t runmode)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    p->appl_runMode = runmode;

    return(eores_OK);
}
    


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
void s_eom_appDataTransmitter_taskInit(void *p)
{
    eom_task_Start(p);
}


static void s_eom_appDataTransmitter_taskStartup(EOMtask *tsk, uint32_t t)
{
    EOMappDataTransmitter *p = (EOMappDataTransmitter*)eom_task_GetExternalData(tsk);

    eo_errman_Assert(eo_errman_GetHandle(), NULL != p, s_eobj_ownname, "extdata() is NULL");

}

static void s_eom_appDataTransmitter_taskRun(EOMtask *tsk, uint32_t evtmsgper)
{
    eOevent_t evt;
    eOresult_t res;
    uint8_t payload[8]; //only 4 test purpose
    
    EOMappDataTransmitter *p = (EOMappDataTransmitter*)eom_task_GetExternalData(tsk);

    evt = (eOevent_t)evtmsgper;   
    
    #ifdef _USE_PROTO_TEST_
    eOmc_setpoint_t     mySetPoint_current = 
    {
        EO_INIT(.type)       eomc_setpoint_current,
        EO_INIT(.to)
        {
            EO_INIT(.current)
            {
                EO_INIT(.value)     0
            }   
        }
    };
    #endif
   
//following activities are independent on runmode
    
    if(eo_common_event_check(evt, EVT_START))
    {
#ifdef _USE_PROTO_TEST_
    mySetPoint_current.to.current.value = pwm_out;
    eo_appCanSP_SendSetPoint(p->cfg.appCanSP_ptr, 3, &mySetPoint_current);    
        
#else        
        res = s_eom_appDataTransmitter_SendRops(p);
        if(eores_OK != res)
        {
            return;
        }        
        
        res = eo_appCanSP_TransmitCanFrames(p->cfg.appCanSP_ptr, eOcanport1);
        if(eores_OK != res)
        {
            return;
        }        

        eo_appCanSP_TransmitCanFrames(p->cfg.appCanSP_ptr, eOcanport2);
        if(eores_OK != res)
        {
            return;
        }        
#endif        
        ((int32_t*)payload)[0]=encoder_can;
        ((int32_t*)payload)[1]=posref_can;

        eo_appCanSP_SendMessage_TEST(p->cfg.appCanSP_ptr, NULL, payload);

        p->st = eOm_appDataTransmitter_st__active;
    }

    
//     
//     
//     
//     switch(p->appl_runMode)
//     {
//         case applrunMode__skinOnly:
//         {
//             s_eom_appDataTransmitter_taskRun_skinOnly_mode(p, evt);
//         }break;
//         
//         case applrunMode__2foc:
//         {
//             s_eom_appDataTransmitter_taskRun_2foc_mode(p, evt);
//         }break;
//         
//         case applrunMode__skinAndMc4:
//         {
//             s_eom_appDataTransmitter_taskRun_skinAndMc4_mode(p, evt);
//         }break;
//         
//         case applrunMode__mc4Only:
//         {
//             ;
//         }break;

//         default:
//         {
//             ;
//         };

//     };


}

static void s_eom_appDataTransmitter_taskRun_skinOnly_mode(EOMappDataTransmitter *p, eOevent_t evt)
{

    eOresult_t res;
    
    if(eo_common_event_check(evt, EVT_START))
    {
        res = s_eom_appDataTransmitter_SendRops(p);
        if(eores_OK != res)
        {
            return;
        }
        p->st = eOm_appDataTransmitter_st__active;
    }

}
static void s_eom_appDataTransmitter_taskRun_2foc_mode(EOMappDataTransmitter *p, eOevent_t evt)
{

    eOresult_t res;
    uint8_t payload[8];  
    
    eOmc_setpoint_t     mySetPoint_current = 
    {
        EO_INIT(.type)       eomc_setpoint_current,
        EO_INIT(.to)
        {
            EO_INIT(.current)
            {
                EO_INIT(.value)     0
            }   
        }
    };

    mySetPoint_current.to.current.value = pwm_out;
    
     if(eo_common_event_check(evt, EVT_START))
    {
    
        res = s_eom_appDataTransmitter_SendRops(p);
        if(eores_OK != res)
        {
            return;
        }        
        
//        eo_appCanSP_SendSetPoint(p->cfg.appCanSP_ptr, 0, &mySetPoint_current); //jid..DA VERIFICARE!!!
        
        ((int32_t*)payload)[0]=encoder_can;
        ((int32_t*)payload)[1]=posref_can;

        eo_appCanSP_SendMessage_TEST(p->cfg.appCanSP_ptr, NULL, payload);

        p->st = eOm_appDataTransmitter_st__active;
    }


}
static void s_eom_appDataTransmitter_taskRun_skinAndMc4_mode(EOMappDataTransmitter *p, eOevent_t evt)
{
    eOresult_t res;
    
    if(eo_common_event_check(evt, EVT_START))
    {
    
        res = s_eom_appDataTransmitter_SendRops(p);
        if(eores_OK != res)
        {
            return;
        }

        eo_appCanSP_TransmitCanFrames(p->cfg.appCanSP_ptr, eOcanport1);
        eo_appCanSP_TransmitCanFrames(p->cfg.appCanSP_ptr, eOcanport2);
//         if( (eomc_motionmonitormode_untilreached == p->monitormode) || (eomc_motionmonitormode_forever == p->monitormode))
//         {
//            #warning VALE--> invia motion done
//             
//         }
        p->st = eOm_appDataTransmitter_st__active;
    }

}

static eOresult_t s_eom_appDataTransmitter_SendRops(EOMappDataTransmitter *p)
{
    EOpacket *pkt_ptr;
    uint16_t numberofrops;
    eOresult_t res;
    

    res = eo_transceiver_Transmit(eo_boardtransceiver_GetHandle(), &pkt_ptr, &numberofrops);
    if(eores_OK != res)
    {
        return(res);
    }
//        if(0 == ena_tx_onrx)
//        {
//            return;
//        }

//        ena_tx_onrx = 0;

    res = eo_ethBaseModule_TransmitPacket(p->cfg.eth_mod, pkt_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    return(eores_OK);

}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



