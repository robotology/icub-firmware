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


#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"
#include "EOsm.h"
#include "eOcfg_sm_EMSappl.h"

#include "EOVtheIPnet.h"


#include "EOMtheSystem.h"
#include "EOVtheSystem.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EOMtheIPnet.h"


#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"

#include "EOMtheEMSsocket.h"

#include "EOMtheEMStransceiver.h"

#include "EOMtheEMSconfigurator.h"

#include "EOMtheEMSerror.h"

#include "EOMtheEMSrunner.h"

#include "EOaction_hid.h"

#if !defined(EMSAPPL_USE_CORE)
#include "EOappEncodersReader.h"
#endif



#include "eEcommon.h"
#include "eEmemorymap.h"

#include "hal_cfg.h"
#include "osal_cfg.h"
#include "ipal_cfg.h"


// -------------------------------------------------------------------------------------------------------------------
// - declaration of extern variables defined elsewhere, for which we dont include the proper .h
// -------------------------------------------------------------------------------------------------------------------

extern const hal_cfg_t     hal_cfg;
extern const osal_cfg_t    osal_cfg;
extern const ipal_cfg_t    ipal_cfg;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSapplCfg.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSapplCfg_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


extern const eEmoduleInfo_t eom_emsapplcfg_modinfo __attribute__((at(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET))) = 
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_process,
            .signature  = ee_procApplication,
            .version    = 
            { 
                .major = EOMTHEEMSAPPLCFG_VERSION_MAJOR, 
                .minor = EOMTHEEMSAPPLCFG_VERSION_MINOR
            },  
            .builddate  = 
            {
                .year  = EOMTHEEMSAPPLCFG_BUILDDATE_YEAR,
                .month = EOMTHEEMSAPPLCFG_BUILDDATE_MONTH,
                .day   = EOMTHEEMSAPPLCFG_BUILDDATE_DAY,
                .hour  = EOMTHEEMSAPPLCFG_BUILDDATE_HOUR,
                .min   = EOMTHEEMSAPPLCFG_BUILDDATE_MIN
            }
        },
        .rom        = 
        {   
            .addr   = EENV_MEMMAP_EAPPLICATION_ROMADDR,
            .size   = EENV_MEMMAP_EAPPLICATION_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_MEMMAP_EAPPLICATION_RAMADDR,
            .size   = EENV_MEMMAP_EAPPLICATION_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_none,
            .size   = 0,
            .addr   = 0
        },
        .communication  = ee_commtype_eth,  // later on we may also add can1 and can2
        .name           = EOMTHEEMSAPPLCFG_NAME
    },
    .protocols  =
    {
        .udpprotversion  = { .major = 0, .minor = 1},
        .can1protversion = { .major = 0, .minor = 1},
        .can2protversion = { .major = 0, .minor = 1},
        .gtwprotversion  = { .major = 0, .minor = 0}
    },
    .extra      = {0}
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_emsapplcfg_singleton_verify(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSapplCfg";

static eObool_t s_emsapplcfg_singleton_verified = eobool_false;


static EOMtheEMSapplCfg s_emsapplcfg_singleton = 
{
    .wsyscfg         =
    {
        .msyscfg        =
        {
            .codespaceoffset        = (EENV_MEMMAP_EAPPLICATION_ROMADDR-EENV_ROMSTART),
            .halcfg                 = &hal_cfg,
            .osalcfg                = &osal_cfg,
            .fsalcfg                = NULL    
        },
        .mempoolcfg     =
        {   // so far it is the default one
            .mode                   = eo_mempool_alloc_dynamic,
            .size08                 = 0,
            .data08                 = NULL,
            .size16                 = 0,
            .data16                 = NULL,    
            .size32                 = 0,
            .data32                 = NULL,    
            .size64                 = 0,
            .data64                 = NULL    
        },
        .errmancfg      =
        {
            .extfn.usr_on_error     = eom_emsapplcfg_hid_userdef_OnError
        },
        .tmrmancfg      =
        {
            .priority               = EOMTHEEMSAPPLCFG_TMRMAN_TASK_PRIORITYof,
            .stacksize              = EOMTHEEMSAPPLCFG_TMRMAN_TASK_STACKSIZEof, 
            .messagequeuesize       = EOMTHEEMSAPPLCFG_TMRMAN_TASK_QUEUESIZEof
        },
        .cbkmancfg      =
        {
            .priority               = EOMTHEEMSAPPLCFG_CBKMAN_TASK_PRIORITYof,
            .stacksize              = EOMTHEEMSAPPLCFG_CBKMAN_TASK_STACKSIZEof, 
            .queuesize              = EOMTHEEMSAPPLCFG_CBKMAN_TASK_QUEUESIZEof      
        }
    },
    .applcfg        =
    {
        .emsappinfo             = &eom_emsapplcfg_modinfo, //&eom_emsappl_info_modinfo,
        .hostipv4addr           = EO_COMMON_IPV4ADDR(EOMTHEEMSAPPLCFG_HOSTIPADDR1, EOMTHEEMSAPPLCFG_HOSTIPADDR2, EOMTHEEMSAPPLCFG_HOSTIPADDR3, EOMTHEEMSAPPLCFG_HOSTIPADDR4)
//        .hostipv4port           = EOMTHEEMSAPPLCFG_TRANSCEIVER_HOSTIPPORT        
    },
    .wipnetcfg      =
    {
        .ipnetcfg       =
        {
            .procpriority           = EOMTHEEMSAPPLCFG_IPNETPROC_TASK_PRIORITYof,
            .procstacksize          = EOMTHEEMSAPPLCFG_IPNETPROC_TASK_STACKSIZEof,
            .procmaxidletime        = EOMTHEEMSAPPLCFG_IPNETPROC_TASK_MAXIDLETIMEof,
            .procwakeuponrxframe    = (eObool_t)EOMTHEEMSAPPLCFG_IPNETPROC_WAKEUPONRXFRAME,
            .tickpriority           = EOMTHEEMSAPPLCFG_IPNETTICK_TASK_PRIORITYof,
            .tickstacksize          = EOMTHEEMSAPPLCFG_IPNETTICK_TASK_STACKSIZEof 
        },
        .ipalcfg        = &ipal_cfg,
        .addrcfg        = NULL,
        .dtgskcfg       =
        {
            .numberofsockets        =  EOMTHEEMSAPPLCFG_IPNET_MAXNUMOFSOCKETS,
            .maxdatagramenqueuedintx = EOMTHEEMSAPPLCFG_IPNET_MAXNUMOFDATAGRAMSENQUEUEDINTX
        }   
    },
    .getipaddrFROMenvironment   =   EOMTHEEMSAPPLCFG_IPADDR_FROM_ENVIRONMENT,
    .boardid                    =   (eom_emsapplcfg_boardid_t)EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD,
    .hasdevice                  = 
    {
        EOMTHEEMSAPPLCFG_EBX_hasSKIN, EOMTHEEMSAPPLCFG_EBX_hasMC4, EOMTHEEMSAPPLCFG_EBX_has2FOC
    },
    .eps                        =
    {
        EOMTHEEMSAPPLCFG_EBX_endpoint_mc, EOMTHEEMSAPPLCFG_EBX_endpoint_as, EOMTHEEMSAPPLCFG_EBX_endpoint_sk
    },
    .disclistcfg    =
    {
        .taskpriority           = EOMTHEEMSAPPLCFG_LISTENER_TASK_PRIORITYof,
        .taskstacksize          = EOMTHEEMSAPPLCFG_LISTENER_TASK_STACKSIZEof,
        .inpdatagramnumber      = EOMTHEEMSAPPLCFG_DISCOVERY_INPDGRAMNUMBER,
        .outdatagramnumber      = EOMTHEEMSAPPLCFG_DISCOVERY_OUTDGRAMNUMBER,
        .inpdatagramsizeof      = EOMTHEEMSAPPLCFG_DISCOVERY_INPDGRAMSIZEOF,
        .outdatagramsizeof      = EOMTHEEMSAPPLCFG_DISCOVERY_OUTDGRAMSIZEOF,
        .localport              = EOMTHEEMSAPPLCFG_DISCOVERY_LOCALPORT, 
        .remoteport             = EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEPORT,
        .remoteipaddr           = EO_COMMON_IPV4ADDR(EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR1, EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR2, EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR3, EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR4),
        .usemutex               = eobool_true                   
    },
    .socketcfg      =
    {
        .inpdatagramnumber          = EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMNUMBER, 
        .outdatagramnumber          = EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMNUMBER, 
        .inpdatagramsizeof          = EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMSIZEOF, 
        .outdatagramsizeof          = EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMSIZEOF,
        .localport                  = EOMTHEEMSAPPLCFG_SOCKET_LOCALPORT, 
        .usemutex                   = eobool_true
    },
    .transcfg       =
    {   // the same ipv4 addr and port as in applcfg ...
        .vectorof_endpoint_cfg  = EOMTHEEMSAPPLCFG_vectorof_endpoint_cfg,
        .hashfunction_ep2index  = EOMTHEEMSAPPLCFG_hashfunction_ep2index,
        .hostipv4addr           = EO_COMMON_IPV4ADDR(EOMTHEEMSAPPLCFG_HOSTIPADDR1, EOMTHEEMSAPPLCFG_HOSTIPADDR2, EOMTHEEMSAPPLCFG_HOSTIPADDR3, EOMTHEEMSAPPLCFG_HOSTIPADDR4),
        .hostipv4port           = EOMTHEEMSAPPLCFG_TRANSCEIVER_HOSTIPPORT,
        .sizes                  =
        {
            .capacityofpacket               = EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMECAPACITY, //1024,
            .capacityofrop                  = EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPCAPACITY, //256,
            .capacityofropframeregulars     = EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREGULARSCAPACITY, //768,
            .capacityofropframeoccasionals  = EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEOCCASIONALSCAPACITY, //128,
            .capacityofropframereplies      = EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREPLIESCAPACITY, //128,
            .maxnumberofregularrops         = EOMTHEEMSAPPLCFG_TRANSCEIVER_MAXNUMOFREGULARROPS //32
        }    
    },
    .errobjcfg      =
    {
        .taskpriority               = EOMTHEEMSAPPLCFG_ERROBJ_TASK_PRIORITYof, 
        .taskstacksize              = EOMTHEEMSAPPLCFG_ERROBJ_TASK_STACKSIZEof
    },
    .cfgobjcfg      =
    {
        .taskpriority               = EOMTHEEMSAPPLCFG_CFGOBJ_TASK_PRIORITYof, 
        .taskstacksize              = EOMTHEEMSAPPLCFG_CFGOBJ_TASK_STACKSIZEof        
    },
    .runobjcfg      =
    {
        .taskpriority               = 
        {
            EOMTHEEMSAPPLCFG_RUNOBJ_TASK_RX_PRIORITYof, EOMTHEEMSAPPLCFG_RUNOBJ_TASK_DO_PRIORITYof, EOMTHEEMSAPPLCFG_RUNOBJ_TASK_TX_PRIORITYof
        },
        .taskstacksize              = 
        {
            EOMTHEEMSAPPLCFG_RUNOBJ_TASK_RX_STACKSIZEof, EOMTHEEMSAPPLCFG_RUNOBJ_TASK_DO_STACKSIZEof, EOMTHEEMSAPPLCFG_RUNOBJ_TASK_TX_STACKSIZEof
        },   
        .period                     = EOMTHEEMSAPPLCFG_RUNOBJ_PERIOD,
        .execRXafter                = EOMTHEEMSAPPLCFG_RUNOBJ_RX_AFTER,            
        .safeRXexecutiontime        = EOMTHEEMSAPPLCFG_RUNOBJ_RX_SAFETIME,
        .execDOafter                = EOMTHEEMSAPPLCFG_RUNOBJ_DO_AFTER,
        .safeDOexecutiontime        = EOMTHEEMSAPPLCFG_RUNOBJ_DO_SAFETIME,
        .execTXafter                = EOMTHEEMSAPPLCFG_RUNOBJ_TX_AFTER,
        .safeTXexecutiontime        = EOMTHEEMSAPPLCFG_RUNOBJ_TX_SAFETIME,
        .maxnumofRXpackets          = EOMTHEEMSAPPLCFG_RUNOBJ_RX_MAXPACKETS,                // add a control that is is lower equal to inpdatagramnumber.
        .maxnumofTXpackets          = EOMTHEEMSAPPLCFG_RUNOBJ_TX_MAXPACKETS,                // so far it can be only 0 or 1 
        .modeatstartup              = (eOemsrunner_mode_t) EOMTHEEMSAPPLCFG_RUNOBJ_MODE_AT_STARTUP    
    }
#if !defined(EMSAPPL_USE_CORE)    
     ,
     .applbodycfg     =
     {
         .connectedEncodersMask      = EOMTHEEMSAPPLCFG_EBX_encodersMASK,
         .emsControllerCfg           =
         {
             .emsboard_type          = EOMTHEEMSAPPLCFG_EBX_emscontroller_EMSTYPE
         },
		 .endpoints                  =
         {
             .mc_endpoint            = EOMTHEEMSAPPLCFG_EBX_endpoint_mc,   
             .as_endpoint            = EOMTHEEMSAPPLCFG_EBX_endpoint_as,
             .sk_endpoint            = EOMTHEEMSAPPLCFG_EBX_endpoint_sk,
         },
         .configdataofMC4boards      =
         {
             .shiftvalues            =
            {
             .jointVelocityShift     =  8,
             .jointVelocityEstimationShift = 8,
             .jointAccelerationEstimationShift = 5
            },
             .bcastpolicy            =
             {
                 .val2bcastList      =
                 {
                    /* 0 */ ICUBCANPROTO_PER_MB_CMD_POSITION,
                    /* 1 */ ICUBCANPROTO_PER_MB_CMD_PID_VAL,
                    /* 2 */ ICUBCANPROTO_PER_MB_CMD_PID_ERROR,
                    /* 3 */ ICUBCANPROTO_PER_MB_CMD_VELOCITY
                 }
             }
            
         }
     }
#endif       
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSapplCfg * eom_emsapplcfg_Initialise(void)
{  
    
    if(eobool_true != s_emsapplcfg_singleton_verified)
    {
        s_emsapplcfg_singleton_verify();
        s_emsapplcfg_singleton_verified = eobool_true;        
    }
    
    return(&s_emsapplcfg_singleton);
}


extern EOMtheEMSapplCfg* eom_emsapplcfg_GetHandle(void) 
{
    if(eobool_true != s_emsapplcfg_singleton_verified)
    {
        return(eom_emsapplcfg_Initialise());
    }
    
    return(&s_emsapplcfg_singleton);
}


extern eObool_t eom_emsapplcfg_HasDevice(EOMtheEMSapplCfg *p, eom_emsapplcfg_deviceid_t dev)
{
    return(s_emsapplcfg_singleton.hasdevice[dev]);
}

extern eOnvEP_t eom_emsapplcfg_Get_nvEPfor(EOMtheEMSapplCfg *p, eom_emsapplcfg_eptype_t eptype)
{
    return(s_emsapplcfg_singleton.eps[eptype]);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

__weak extern void eom_emsapplcfg_hid_userdef_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
    const char err[4][16] = {"info", "warning", "weak error", "fatal error"};
    char str[128];

    snprintf(str, sizeof(str)-1, "startup: [eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, err[(uint8_t)errtype], info);
    hal_trace_puts(str);

    if(errtype <= eo_errortype_warning)
    {
        return;
    }
    
    eov_sys_Stop(eov_sys_GetHandle());

    for(;;);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_emsapplcfg_singleton_verify(void)
{

}







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





