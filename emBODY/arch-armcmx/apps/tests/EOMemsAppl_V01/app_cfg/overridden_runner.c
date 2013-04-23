
/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdio.h"

//general
#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"
#include "EOarray.h"
#include "EOtheErrorManager.h"

//appl
#include "EOMtheEMSappl.h"
#include "EOtheEMSApplBody.h"

//embobj-cfg-icub
#include "EoMotionControl.h"
#include "EoSensors.h"
#include "EoManagement.h"

#include "EOemsController_hid.h" 




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
//if defined SET_DESIRED_CURR_IN_ONLY_ONE_MSG, the appl sends all desered current to 2fon in only one msg
#define SET_DESIRED_CURR_IN_ONLY_ONE_MSG


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

#ifdef MC_CAN_DEBUG
//extern int32_t encoder_can_pos;
//extern int32_t encoder_can_vel;
extern int16_t torque_debug_can[4];
#endif

//uint16_t hysto_error[4]={0,0,0,0};

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section
uint8_t numoftxfrme_failed[2] = {0, 0}; // indica quante volte durante la fase di rx e do ho provato a mettere in coda un farme
                                        //e ritorna coda piena.
                                        //questo numero e' resettato a fine ciclo
                                
uint8_t numofframe_2send[2] = {0, 0}; // contiene il numero di messaggi da inviare appena entrati nella fase di tx
uint8_t numofframe_2sendRemain[2] = {0, 0};  // contiene il numero di messaggi ancora presenti in coda alla fine della fase di tx.

// nel caso in cui ho ancora dei frame in coda all'uscita dalla fase di tx questi contatori mi dicono cosa succede 
//durante le sucessive fasi rx e DO. 
uint8_t ena_checktx_nexttime = 0; //abilita il controllo
uint8_t countput_netxtime[2] = {0, 0};

uint8_t counput4cycle[2] = {0, 0};


volatile int8_t MY_semaphore_count[2] = {0, 0};

//da usare con funzioni XXX
volatile int8_t RUN_semaphore_count[2] = {0, 0};

//dati di canservicepropvider
extern runnning_data_t run_data;
extern volatile uint8_t numtx[2];


//contatore cicli RX-DO-TX 
uint32_t ciclecount = 0;


#ifdef _BDOOR_DEB_CANMSG_LOG_
extern EOcanFaultLogDEBUG_t EOcanFaultLogDEBUG;
#endif
    
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//RX
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinOnly_mode(EOtheEMSapplBody *p);
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinAndMc4_mode(EOtheEMSapplBody *p);
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_2foc_mode(EOtheEMSapplBody *p);

//DO
static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p);
static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p);



//utils
static void s_eom_emsrunner_hid_readSkin(EOtheEMSapplBody *p);
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
static void s_eom_emsrunner_hid_readMc4andMais(EOtheEMSapplBody *p);
static void s_eom_emsrunner_hid_UpdateJointstatus(EOMtheEMSrunner *p);

#ifdef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_inOneMsgOnly(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
#else
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_with4msg(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);
#endif
EO_static_inline  eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint16_t motionDoneJoin2Use = 0;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
    //char str[100];
    eOmn_appl_runMode_t runmode =  eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
    if(applrunMode__2foc == runmode)
    {
         eo_appEncReader_StartRead(eo_emsapplBody_GetEncoderReaderHandle(eo_emsapplBody_GetHandle()));
    }
    ciclecount++;
    
    // DEBUG
    #ifdef __MC_BACKDOOR__
    eo_emsController_hid_DEBUG_reset();
    #endif
}

extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eOmn_appl_runMode_t runmode  =  eo_emsapplBody_GetAppRunMode(emsappbody_ptr);

    switch(runmode)
    {
        case applrunMode__skinOnly:
        {
            s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinOnly_mode(emsappbody_ptr);
        }break;
        
        case applrunMode__2foc:
        {
            #ifdef __MC_BACKDOOR__
            eo_emsController_hid_DEBUG_evaltransmission();
            #endif
            s_eom_emsrunner_hid_taskRX_act_afterdgramrec_2foc_mode(emsappbody_ptr);
        }break;
        
        case applrunMode__skinAndMc4:
        {
            s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinAndMc4_mode(emsappbody_ptr);
        }break;
        
        case applrunMode__mc4Only:
        {
            ;
        }break;

        default:
        {
            ;
        };
    };
    
}



extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eOmn_appl_runMode_t runmode =  eo_emsapplBody_GetAppRunMode(emsappbody_ptr);

    /* TAG_ALE */
//     if(applrunMode__skinAndMc4 == runmode)
//     {
//         #warning VALE--> remove this code after test on semaphore-can
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//         s_eom_emsrunner_hid_SetCurrentsetpoint(appTheSP, pwm, 0);
//     }
    

    switch(runmode)
    {
        case applrunMode__2foc:
        {
            s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(p);
        }break;
        
        case applrunMode__mc4Only:
        case applrunMode__skinAndMc4:
        {
            s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(p);
        }break;
        
        case applrunMode__skinOnly:
        {
            return; //currently nothing to do 
        }//break;
        
        default:
        {
            return;
        }
    };
  
}


extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{

    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    //eOresult_t res;
    static uint8_t first = 1;
    char str[130];
    
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
    
    #ifdef MC_CAN_DEBUG
    //uint8_t payload[8];
    //((int32_t*)payload)[0]=encoder_can_pos;
    //((int32_t*)payload)[1]=encoder_can_vel;
    //eo_appCanSP_SendMessage_TEST(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), payload);
    static uint16_t cnt = 0;
    
    if (++cnt >= 1000)
    {
        cnt = 0;
        
        eo_appCanSP_SendMessage_TEST(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), (uint8_t*)torque_debug_can);
    }
    #endif
    
    /*
    static uint16_t hysto_error_timer = 0;
    
    if (++hysto_error_timer >= 1000)
    {
        uint8_t payload[8];
        ((uint16_t*)payload)[0]=hysto_error[1];
        ((uint16_t*)payload)[1]=hysto_error[2];
        ((uint16_t*)payload)[2]=hysto_error[3];
        eo_appCanSP_SendMessage_TEST(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), payload);

        hysto_error_timer = 0;
        
        hysto_error[1] = 0;
        hysto_error[2] = 0;
        hysto_error[3] = 0;
    }
    */
    
    
    if((numoftxfrme_failed[0] != 0) || (numoftxfrme_failed[0] != 0))
    {
        snprintf(str, sizeof(str)-1, "num tx FAILED frame: numfailure[0]=%d numfailure[1]=%d", numoftxfrme_failed[0], numoftxfrme_failed[1]);        
        hal_trace_puts(str);
    }
    
    
    if(first)
    {
        snprintf(str, sizeof(str)-1, "FIRST in TX: sem count = %d", MY_semaphore_count[0] );        
        hal_trace_puts(str); 
        first = 0;
       // run_data.isrunning = 1;
    }
    

    eo_appCanSP_GetNumOfTxCanframe(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1, &numofframe_2send[0]);
    eo_appCanSP_GetNumOfTxCanframe(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2, &numofframe_2send[1]);
    
    
    if(ena_checktx_nexttime)
    {
        for(int i=0; i<2; i++)
        {
            snprintf(str, sizeof(str)-1, "NEXT_TIME: cyclecount=%d port=%d numofput=%d, isrcount=%d, semcount=%d", ciclecount, i, countput_netxtime[i], numtx[i], MY_semaphore_count[i]);      
            hal_trace_puts(str); 
        }
        ena_checktx_nexttime = 0;
    }
//     res = eo_appCanSP_StartTransmitCanFrames(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1, eobool_true);
//     if(eores_OK != res)
//     {
//         return;
//     }        

//     res = eo_appCanSP_StartTransmitCanFrames(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2, eobool_true);
//     if(eores_OK != res)
//     {
//         return;
//     }        
    
    eo_appCanSP_starttransmit_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1);
    eo_appCanSP_starttransmit_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2);

}



extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
    uint8_t a =1;
    char str[250];
    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
    eOresult_t res[2];
    
/* METODO 1 */
//     res[0] = eo_appCanSP_WaitTransmitCanFrames(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1);
//     res[1] = eo_appCanSP_WaitTransmitCanFrames(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2);
    a =a;
 /* METODO 2 */   
//     eo_appCanSP_GetNumOfTxCanframe(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1, &numofframe_tx);
//     while(1)
//     {
//        eo_appCanSP_GetNumOfTxCanframe(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1, &numofframe_tx);
//        if(numofframe_tx == 0)
//        {
//            break;
//        }
//     }

/* METODO 3 */    
//     eo_appCanSP_StartTransmitAndWait(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1);
//     eo_appCanSP_StartTransmitAndWait(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2);
    
 
/* METODO 4*/

    res[0] = eo_appCanSP_wait_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1);
    res[1] = eo_appCanSP_wait_XXX(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2);
    
    eo_appCanSP_GetNumOfTxCanframe(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport1, &numofframe_2sendRemain[0]);
    eo_appCanSP_GetNumOfTxCanframe(eo_emsapplBody_GetCanServiceHandle(emsappbody_ptr), eOcanport2, &numofframe_2sendRemain[1]);
    if((numofframe_2sendRemain[0]!= 0) || (numofframe_2sendRemain[1]!= 0))
    {
        for(int i=0; i<2; i++)
        {
            snprintf(str, sizeof(str)-1, "TX PHASE: cyclenum=%d port=%d before=%d, after=%d isrcount=%d, semcount=%d res=%d",ciclecount, i, numofframe_2send[i], numofframe_2sendRemain[i], numtx[i], MY_semaphore_count[i], res[i]);        
            hal_trace_puts(str);
        }
        //abilito trace per il ciclo sucessivo e resetto il count
        ena_checktx_nexttime = 1;
        countput_netxtime[0] = 0;
        countput_netxtime[1] = 0;
    }

    
    if(counput4cycle[0]>10)
    {
            snprintf(str, sizeof(str)-1, "TX PHASE: cyclenum=%d numof pun in this cylce=%d", ciclecount, counput4cycle[0]);        
            hal_trace_puts(str);
    }
    
    //reset dei contatori
    numoftxfrme_failed[0] = 0;
    numoftxfrme_failed[1] = 0;
    counput4cycle[0] = 0;
    counput4cycle[1] = 0;
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinOnly_mode(EOtheEMSapplBody *p)
{
    s_eom_emsrunner_hid_readSkin(p); 
}

static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_2foc_mode(EOtheEMSapplBody *p)
{
    uint8_t     numofRXcanframe = 0;
    eOresult_t  res;

    //read can msg from port 1
    res = eo_appCanSP_GetNumOfRecCanframe(eo_emsapplBody_GetCanServiceHandle(p), eOcanport1, &numofRXcanframe);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetNumOfRecCanframe");

    eo_appCanSP_read(eo_emsapplBody_GetCanServiceHandle(p), eOcanport1, numofRXcanframe, NULL);

    //read msg from port 2
    res = eo_appCanSP_GetNumOfRecCanframe(eo_emsapplBody_GetCanServiceHandle(p), eOcanport2, &numofRXcanframe);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetNumOfRecCanframe");
    
    eo_appCanSP_read(eo_emsapplBody_GetCanServiceHandle(p), eOcanport2, numofRXcanframe, NULL);
}
static void s_eom_emsrunner_hid_taskRX_act_afterdgramrec_skinAndMc4_mode(EOtheEMSapplBody *p)
{
        
    /* 1) get can frames about skin and process them*/
    s_eom_emsrunner_hid_readSkin(p);
    
    /* 2) get can frame from mais and mc4 board and process them */
    s_eom_emsrunner_hid_readMc4andMais(p);
    
    //something else about mc4 ??????

}

static void s_eom_emsrunner_hid_readSkin(EOtheEMSapplBody *p)
{
    eOsk_skinId_t     sId = 0;//only one skin. (evenif skin is composed by more skin board, here we see the skin like a unique identity)
//    eOskin_status_t     *skstatus_ptr;
    EOarray_of_10canframes *arrayof10canframes_ptr = NULL;
    eOresult_t res;

//uncomment this code when status and config nvvar are used.   
//     res = eo_appTheDB_GetSkinStatusPtr(eo_appTheDB_GetHandle(), sId,  &skstatus_ptr);
//     eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetSkinNVMemoryRef");

//     eo_array_Reset((EOarray*)(&skstatus_ptr->arrayof10canframes));

    
    //1) get pointer to nv array and reset it

    res = eo_appTheDB_GetSkinStArray10CanFramesPtr(eo_appTheDB_GetHandle(), sId,  &arrayof10canframes_ptr);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetSkinStArray10CanFramesPtr");
    
    eo_array_Reset((EOarray*)arrayof10canframes_ptr);
    
    eo_appCanSP_read(eo_emsapplBody_GetCanServiceHandle(p), eOcanport2, 10, NULL); //10 is the max size of sk_array

}

static void s_eom_emsrunner_hid_readMc4andMais(EOtheEMSapplBody *p)
{
    eOsnsr_maisId_t                 sId = 0; //only one mais per ems
    eOsnsr_mais_status_t            *sstatus_ptr;
    eOresult_t                      res;
    uint8_t                         numofRXcanframe = 10; //default num
    
    //1) reset nv array
    res = eo_appTheDB_GetSnrMaisStatusPtr(eo_appTheDB_GetHandle(), sId,  &sstatus_ptr);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetSnrMaisStatusPtr");

    //reset array
    sstatus_ptr->the15values.head.size = 0;
    sstatus_ptr->the15values.data[0] = 0;

    res = eo_appCanSP_GetNumOfRecCanframe(eo_emsapplBody_GetCanServiceHandle(p), eOcanport1, &numofRXcanframe);
    eo_errman_Assert(eo_errman_GetHandle(), (eores_OK == res), "emsrunner_hid", "err in GetNumOfRecCanframe");

    eo_appCanSP_read(eo_emsapplBody_GetCanServiceHandle(p), eOcanport1, numofRXcanframe, NULL);

}



EO_static_inline  eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
{
    
#ifdef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
    return(s_eom_emsrunner_hid_SetCurrentsetpoint_inOneMsgOnly(p, pwmList, size));
#else	
    return(s_eom_emsrunner_hid_SetCurrentsetpoint_with4msg(p, pwmList,size));
#endif
}

#ifndef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_with4msg(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
{
    eOresult_t 				err;
	eOmeas_current_t            value;
    eOresult_t 				    res = eores_OK;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MB_CMD__SET_DISIRED_CURRENT
    };	

#warning VALE --> solo per test
    pwmList[0] = 0x11AA;
    pwmList[1] = 0x12AA;
    pwmList[2] = 0x13AA;
    pwmList[3] = 0x14AA;
    uint16_t numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());

    for (uint8_t jid = 0; jid <numofjoint; jid++)
    {
        value = pwmList[jid];
    
        /*Since in run mode the frame are sent on demnad...here i can punt in tx queue frame to send.
        they will be sent by transmitter */

        err = eo_appCanSP_SendCmd2Joint(eo_emsapplBody_GetCanServiceHandle(p), jid, msgCmd, (void*)&value);
        
        if (err != eores_OK)
        {
            res = err;
        }
    }
    
    return(res);
   
}
#endif

#ifdef SET_DESIRED_CURR_IN_ONLY_ONE_MSG
static eOresult_t s_eom_emsrunner_hid_SetCurrentsetpoint_inOneMsgOnly(EOtheEMSapplBody *p, int16_t *pwmList, uint8_t size)
{
    eOresult_t 				                res = eores_OK;
    int16_t                                 pwm_aux[4] = {0, 0, 0, 0};
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         dest;
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_periodicMotorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_PER_MB_CMD_EMSTO2FOC_DESIRED_CURRENT
    };
#ifdef _BDOOR_DEB_CANMSG_LOG_
    extern int8_t canLogFaultDeb_count;
#endif

    uint16_t numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    
    for (uint8_t jid = 0; jid <numofjoint; ++jid)
    {
        res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jid,  &canLoc, NULL);
        if(eores_OK != res)
        {
            return(res); //i think i'll be never here
        }
        
        pwm_aux[canLoc.addr-1] = pwmList[jid];
#ifdef _BDOOR_DEB_CANMSG_LOG_
        if(canLogFaultDeb_count == -1)
        {
            EOcanFaultLogDEBUG.currSetPointList[jid] = pwmList[jid];
        }
#endif
    
    }
    //since msg is periodic, all 2foc received it so dest is useless.
    dest.dest = 0;
    eo_appCanSP_SendCmd(eo_emsapplBody_GetCanServiceHandle(p), canLoc.emscanport, dest, msgCmd, (void*)pwm_aux);
    
    return(res);    
}
#endif


static void s_eom_emsrunner_hid_userdef_taskDO_activity_2foc(EOMtheEMSrunner *p)
{
    eOresult_t          res;
    EOtheEMSapplBody    *emsappbody_ptr = eo_emsapplBody_GetHandle();
    uint32_t            encvalue[4] = {(uint32_t)ENC_INVALID, (uint32_t)ENC_INVALID, (uint32_t)ENC_INVALID, (uint32_t)ENC_INVALID};
    int16_t             pwm[4];

    uint16_t numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());

    if (eo_appEncReader_isReady(eo_emsapplBody_GetEncoderReaderHandle(emsappbody_ptr)))
    {    
        for (uint8_t enc = 0; enc < numofjoint; ++enc)
        {
            res = eo_appEncReader_GetValue(eo_emsapplBody_GetEncoderReaderHandle(emsappbody_ptr), (eOappEncReader_encoder_t)enc, &(encvalue[enc]));
            
            if (res != eores_OK)
            {
                //if (enc == 3) ++hysto_error[encvalue[3]];
                
                encvalue[enc] = (uint32_t)ENC_INVALID;
            }
        }        
    }

    eo_emsController_ReadEncoders((int32_t*)encvalue);
        
    /* 2) pid calc */
    eo_emsController_PWM(pwm);

    /* 3) prepare and punt in rx queue new setpoint */
    s_eom_emsrunner_hid_SetCurrentsetpoint(emsappbody_ptr, pwm, 4); //4: eo_emsController_PWM fills an arry of 4 item 
                                                                    //because max num of mortor for each ems is 4 
 
    /* 4) update joint status */
    s_eom_emsrunner_hid_UpdateJointstatus(p);
    /*Note: motor status is updated with data sent by 2foc by can */
}


static void s_eom_emsrunner_hid_UpdateJointstatus(EOMtheEMSrunner *p)
{
    eOmc_joint_status_t             *jstatus_ptr;
    eOmc_jointId_t                  jId;
    eOresult_t                      res;
    uint16_t                        numofjoint;
    
    numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    for(jId = 0; jId<numofjoint; jId++)
    {
        res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId, &jstatus_ptr);
        if(eores_OK != res)
        {
            return; //error
        }
        
        eo_emsController_GetJointStatus(jId, &jstatus_ptr->basic);
        
        eo_emsController_GetActivePidStatus(jId, &jstatus_ptr->ofpid);
        
        if(eomc_motionmonitorstatus_setpointnotreachedyet == jstatus_ptr->basic.motionmonitorstatus)
        {
            /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
            - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
            message because the setpoint is alredy reached. this means that:
                - if monitormode is forever, no new set point has been configured 
                - if monitormode is _untilreached, the joint reached the setpoint already.
            - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
            message because pc104 is not interested in getting motion done.
            */
            if(eo_emsController_GetMotionDone(jId))
            {
                jstatus_ptr->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointisreached;
            }
        }
        
        
    }
}

static void s_eom_emsrunner_hid_userdef_taskDO_activity_mc4(EOMtheEMSrunner *p)
{
    eOresult_t                      res;
    uint16_t                        numofjoint;
    eOmc_joint_status_t             *jstatus_ptr;
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MB_CMD__MOTION_DONE
    };
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    numofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());

    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), motionDoneJoin2Use, &jstatus_ptr);
    if(eores_OK != res)
    {
        return; //error
    }
    
    if(jstatus_ptr->basic.motionmonitorstatus == eomc_motionmonitorstatus_setpointnotreachedyet)
    {
        /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
        - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
        message because the setpoint is alredy reached. this means that:
            - if monitormode is forever, no new set point has been configured 
            - if monitormode is _untilreached, the joint reached the setpoint already.
        - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
        message because pc104 is not interested in getting motion done.
        */
        eo_appCanSP_SendCmd2Joint(appCanSP_ptr, motionDoneJoin2Use, msgCmd, NULL);
    }
    
    motionDoneJoin2Use++;
    if(motionDoneJoin2Use == numofjoint)
    {
        motionDoneJoin2Use = 0;
    }

    
//     for(jId = 0; jId<numofjoint; jId++)
//     {
//         res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), jId, &jstatus_ptr);
//         if(eores_OK != res)
//         {
//             return; //error
//         }
//         
//         if(jstatus_ptr->basic.motionmonitorstatus == eomc_motionmonitorstatus_setpointnotreachedyet)
//         {
//             /* if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
//             - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
//             message because the setpoint is alredy reached. this means that:
//                 - if monitormode is forever, no new set point has been configured 
//                 - if monitormode is _untilreached, the joint reached the setpoint already.
//             - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
//             message because pc104 is not interested in getting motion done.
//             */
//             eo_appCanSP_SendCmd2Joint(appCanSP_ptr, jId, msgCmd, NULL);
//         }

//     }   
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




