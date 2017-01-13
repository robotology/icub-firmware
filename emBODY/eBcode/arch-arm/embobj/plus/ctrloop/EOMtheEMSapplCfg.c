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
#include "stdio.h"
#include "EoCommon.h"


#include "EOsm.h"
#include "eOcfg_sm_EMSappl.h"

#include "EOVtheIPnet.h"
#include "EOMmutex.h"

#include "EOMtheSystem.h"
#include "EOVtheSystem.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOMtheIPnet.h"


#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"

#include "EOMtheEMSsocket.h"

#include "EOMtheEMStransceiver.h"

#include "EOMtheEMSconfigurator.h"

#include "EOMtheEMSerror.h"

#include "EOMtheEMSrunner.h"

#include "EOaction.h"


#include "eEcommon.h"
#include "eEmemorymap.h"

#include "hal_cfg.h"
#include "osal_cfg.h"
#include "ipal_cfg.h"

#include "EOMtheSystem.h"

// to see the EOMTHEEMSAPPLCFG_* macros
#include "EOMtheEMSapplCfg_cfg.h"


// -------------------------------------------------------------------------------------------------------------------
// - declaration of extern variables defined elsewhere, for which we dont include the proper .h
// -------------------------------------------------------------------------------------------------------------------




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

//extern EOVtheEMSapplCfgBody theapplbodyconfig;

#if (__ARMCC_VERSION > 6000000)
#error ::: change for compiler arm v6 [placing data at a particular rom address]
// something like: 
// const eEmoduleExtendedInfo_t eom_emsapplcfg_modinfo_extended    __attribute__((section(".ARM.__at_0x08020200"))) = 
#endif


const eEmoduleExtendedInfo_t eom_emsapplcfg_modinfo_extended    __attribute__((at(EENV_MEMMAP_EAPPLICATION_ROMADDR+EENV_MODULEINFO_OFFSET))) = 

{
    EO_INIT(.moduleinfo) 
    {
        EO_INIT(.info)
        {
            EO_INIT(.entity)
            {
                EO_INIT(.type)      ee_entity_process,
                EO_INIT(.signature) ee_procApplication,
                EO_INIT(.version) 
                { 
                    EO_INIT(.major) EOMTHEEMSAPPLCFG_VERSION_MAJOR, 
                    EO_INIT(.minor) EOMTHEEMSAPPLCFG_VERSION_MINOR
                },  
                EO_INIT(.builddate)
                {
                    EO_INIT(.year)  EOMTHEEMSAPPLCFG_BUILDDATE_YEAR,
                    EO_INIT(.month) EOMTHEEMSAPPLCFG_BUILDDATE_MONTH,
                    EO_INIT(.day)   EOMTHEEMSAPPLCFG_BUILDDATE_DAY,
                    EO_INIT(.hour)  EOMTHEEMSAPPLCFG_BUILDDATE_HOUR,
                    EO_INIT(.min)   EOMTHEEMSAPPLCFG_BUILDDATE_MIN
                }
            },
            EO_INIT(.rom)
            {   
                EO_INIT(.addr)  EENV_MEMMAP_EAPPLICATION_ROMADDR,
                EO_INIT(.size)  EENV_MEMMAP_EAPPLICATION_ROMSIZE
            },
            EO_INIT(.ram)
            {   
                EO_INIT(.addr)  EENV_MEMMAP_EAPPLICATION_RAMADDR,
                EO_INIT(.size)  EENV_MEMMAP_EAPPLICATION_RAMSIZE
            },
            EO_INIT(.storage)
            {
                EO_INIT(.type)  ee_strg_none,
                EO_INIT(.size)  0,
                EO_INIT(.addr)  0
            },
            EO_INIT(.communication) (ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2),  
            EO_INIT(.name)          EOMTHEEMSAPPLCFG_NAME
        },
        EO_INIT(.protocols)
        {
            EO_INIT(.udpprotversion)    { EO_INIT(.major) 1, EO_INIT(.minor) 0},
            EO_INIT(.can1protversion)   { EO_INIT(.major) 1, EO_INIT(.minor) 0 },
            EO_INIT(.can2protversion)   { EO_INIT(.major) 1, EO_INIT(.minor) 0 },
            EO_INIT(.gtwprotversion)    { EO_INIT(.major) 0, EO_INIT(.minor) 0 }
        },
        EO_INIT(.extra)     {"EXT"}
    },
    EO_INIT(.compilationdatetime)   __DATE__ " " __TIME__,
    EO_INIT(.userdefined)           {0}
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
    EO_INIT(.wsyscfg)
    {
        EO_INIT(.msyscfg)
        {
            EO_INIT(.codespaceoffset)       (EENV_MEMMAP_EAPPLICATION_ROMADDR-EENV_ROMSTART),
            EO_INIT(.halcfg)                &hal_cfg,
            EO_INIT(.osalcfg)               &osal_cfg,
            EO_INIT(.fsalcfg)               NULL    
        },
        EO_INIT(.mempoolcfg)
        {   
            EO_INIT(.mode)                  eo_mempool_alloc_dynamic,
            EO_INIT(.conf)                  &eom_thesystem_mempool_alloc_config_heaposal
        },
        EO_INIT(.errmancfg)
        {
            EO_INIT(.extfn.usr_on_error)    eom_emsapplcfg_hid_userdef_OnError
        },
        EO_INIT(.tmrmancfg)
        {
            EO_INIT(.priority)              EOMTHEEMSAPPLCFG_TMRMAN_TASK_PRIORITYof,
            EO_INIT(.stacksize)             EOMTHEEMSAPPLCFG_TMRMAN_TASK_STACKSIZEof, 
            EO_INIT(.messagequeuesize)      EOMTHEEMSAPPLCFG_TMRMAN_TASK_QUEUESIZEof
        },
        EO_INIT(.cbkmancfg)
        {
            EO_INIT(.priority)              EOMTHEEMSAPPLCFG_CBKMAN_TASK_PRIORITYof,
            EO_INIT(.stacksize)             EOMTHEEMSAPPLCFG_CBKMAN_TASK_STACKSIZEof, 
            EO_INIT(.queuesize)             EOMTHEEMSAPPLCFG_CBKMAN_TASK_QUEUESIZEof      
        }
    },
    EO_INIT(.applcfg)
    {
        EO_INIT(.emsappinfo)            (const eEmoduleInfo_t*) &eom_emsapplcfg_modinfo_extended, 
        EO_INIT(.hostipv4addr)          EO_COMMON_IPV4ADDR(EOMTHEEMSAPPLCFG_HOSTIPADDR1, EOMTHEEMSAPPLCFG_HOSTIPADDR2, EOMTHEEMSAPPLCFG_HOSTIPADDR3, EOMTHEEMSAPPLCFG_HOSTIPADDR4)    
    },
    EO_INIT(.wipnetcfg)
    {
        EO_INIT(.ipnetcfg)
        {
            EO_INIT(.procpriority)          EOMTHEEMSAPPLCFG_IPNETPROC_TASK_PRIORITYof,
            EO_INIT(.procstacksize)         EOMTHEEMSAPPLCFG_IPNETPROC_TASK_STACKSIZEof,
            EO_INIT(.procmaxidletime)       EOMTHEEMSAPPLCFG_IPNETPROC_TASK_MAXIDLETIMEof,
            EO_INIT(.procwakeuponrxframe)   (eObool_t)EOMTHEEMSAPPLCFG_IPNETPROC_WAKEUPONRXFRAME,
            EO_INIT(.tickpriority)          EOMTHEEMSAPPLCFG_IPNETTICK_TASK_PRIORITYof,
            EO_INIT(.tickstacksize)         EOMTHEEMSAPPLCFG_IPNETTICK_TASK_STACKSIZEof 
        },
        EO_INIT(.ipalcfg)       &ipal_cfg,
        EO_INIT(.addrcfg)       NULL,
        EO_INIT(.dtgskcfg)
        {
            EO_INIT(.numberofsockets)           EOMTHEEMSAPPLCFG_IPNET_MAXNUMOFSOCKETS,
            EO_INIT(.maxdatagramenqueuedintx)   EOMTHEEMSAPPLCFG_IPNET_MAXNUMOFDATAGRAMSENQUEUEDINTX
        }   
    },
    EO_INIT(.getipaddrFROMenvironment)      EOMTHEEMSAPPLCFG_IPADDR_FROM_ENVIRONMENT,
    EO_INIT(.errmng_haltrace_enabled)       EOMTHEEMSAPPLCFG_HALTRACE_ENABLED,
    EO_INIT(.boardid)                       255,
    EO_INIT(.disclistcfg)
    {
        EO_INIT(.taskpriority)          EOMTHEEMSAPPLCFG_LISTENER_TASK_PRIORITYof,
        EO_INIT(.taskstacksize)         EOMTHEEMSAPPLCFG_LISTENER_TASK_STACKSIZEof,
        EO_INIT(.inpdatagramnumber)     EOMTHEEMSAPPLCFG_DISCOVERY_INPDGRAMNUMBER,
        EO_INIT(.outdatagramnumber)     EOMTHEEMSAPPLCFG_DISCOVERY_OUTDGRAMNUMBER,
        EO_INIT(.inpdatagramsizeof)     EOMTHEEMSAPPLCFG_DISCOVERY_INPDGRAMSIZEOF,
        EO_INIT(.outdatagramsizeof)     EOMTHEEMSAPPLCFG_DISCOVERY_OUTDGRAMSIZEOF,
        EO_INIT(.localport)             EOMTHEEMSAPPLCFG_DISCOVERY_LOCALPORT, 
        EO_INIT(.remoteport)            EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEPORT,
        EO_INIT(.remoteipaddr)          EO_COMMON_IPV4ADDR(EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR1, EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR2, EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR3, EOMTHEEMSAPPLCFG_DISCOVERY_REMOTEIPADDR4),
        EO_INIT(.usemutex)              eobool_true,
        EO_INIT(.discoveryprotocol)     eodiscovery_protocol_ethloader_reduced
    },
    EO_INIT(.backdoorcfg)
    {
        EO_INIT(.enabled)               EOMTHEEMSAPPLCFG_BACKDOOR_ENABLED,
        EO_INIT(.taskpriority)          EOMTHEEMSAPPLCFG_BACKDOOR_TASK_PRIORITYof,
        EO_INIT(.taskstacksize)         EOMTHEEMSAPPLCFG_BACKDOOR_TASK_STACKSIZEof,
        EO_INIT(.inpdatagramnumber)     EOMTHEEMSAPPLCFG_BACKDOOR_INPDGRAMNUMBER,
        EO_INIT(.outdatagramnumber)     EOMTHEEMSAPPLCFG_BACKDOOR_OUTDGRAMNUMBER,
        EO_INIT(.inpdatagramsizeof)     EOMTHEEMSAPPLCFG_BACKDOOR_INPDGRAMSIZEOF,
        EO_INIT(.outdatagramsizeof)     EOMTHEEMSAPPLCFG_BACKDOOR_OUTDGRAMSIZEOF,
        EO_INIT(.localport)             EOMTHEEMSAPPLCFG_BACKDOOR_LOCALPORT, 
        EO_INIT(.remoteport)            EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEPORT,
        EO_INIT(.remoteipaddr)          EO_COMMON_IPV4ADDR(EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR1, EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR2, EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR3, EOMTHEEMSAPPLCFG_BACKDOOR_REMOTEIPADDR4),
        EO_INIT(.usemutex)              eobool_true     
    },
    EO_INIT(.socketcfg)
    {
        EO_INIT(.inpdatagramnumber)     EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMNUMBER, 
        EO_INIT(.outdatagramnumber)     EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMNUMBER, 
        EO_INIT(.inpdatagramsizeof)     EOMTHEEMSAPPLCFG_SOCKET_INPDGRAMSIZEOF, 
        EO_INIT(.outdatagramsizeof)     EOMTHEEMSAPPLCFG_SOCKET_OUTDGRAMSIZEOF,
        EO_INIT(.localport)             EOMTHEEMSAPPLCFG_SOCKET_LOCALPORT, 
        EO_INIT(.usemutex)              eobool_true
    },  
    EO_INIT(.transcfg)
    {   // the same ipv4 addr and port as in applcfg ...   
        EO_INIT(.nvsetbrdcfg)           &eonvset_BRDcfgBasic,  
        EO_INIT(.hostipv4addr)          EO_COMMON_IPV4ADDR(EOMTHEEMSAPPLCFG_HOSTIPADDR1, EOMTHEEMSAPPLCFG_HOSTIPADDR2, EOMTHEEMSAPPLCFG_HOSTIPADDR3, EOMTHEEMSAPPLCFG_HOSTIPADDR4),
        EO_INIT(.hostipv4port)          EOMTHEEMSAPPLCFG_TRANSCEIVER_HOSTIPPORT,
        EO_INIT(.sizes)
        {
            EO_INIT(.capacityoftxpacket)            EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMECAPACITY, 
            EO_INIT(.capacityofrop)                 EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPCAPACITY, 
            EO_INIT(.capacityofropframeregulars)    EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREGULARSCAPACITY, 
            EO_INIT(.capacityofropframeoccasionals) EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEOCCASIONALSCAPACITY, 
            EO_INIT(.capacityofropframereplies)     EOMTHEEMSAPPLCFG_TRANSCEIVER_ROPFRAMEREPLIESCAPACITY, 
            EO_INIT(.maxnumberofregularrops)        EOMTHEEMSAPPLCFG_TRANSCEIVER_MAXNUMOFREGULARROPS         
        },
        EO_INIT(.transprotection)       eo_trans_protection_none, //eo_trans_protection_enabled, //eo_trans_protection_none,
        EO_INIT(.nvsetprotection)       eo_nvset_protection_none,//eo_nvset_protection_one_per_endpoint //eo_nvset_protection_none // eo_nvset_protection_one_per_netvar eo_nvset_protection_one_per_endpoint
        EO_INIT(.proxycfg)
        {
            EO_INIT(.mode)                      (eOproxymode_t)EOMTHEEMSAPPLCFG_PROXY_MODE,
            EO_INIT(.capacityoflistofropdes)    EOMTHEEMSAPPLCFG_PROXY_MAXNUMOFREPLYROPS,
            EO_INIT(.replyroptimeout)           EOMTHEEMSAPPLCFG_PROXY_REPLYTIMEOUTMILLISEC*1000,
            EO_INIT(.mutex_fn_new)              NULL,
            EO_INIT(.transceiver)               NULL
        }                      
    },
    EO_INIT(.errobjcfg)
    {
        EO_INIT(.taskpriority)              EOMTHEEMSAPPLCFG_ERROBJ_TASK_PRIORITYof, 
        EO_INIT(.taskstacksize)             EOMTHEEMSAPPLCFG_ERROBJ_TASK_STACKSIZEof
    },
    EO_INIT(.cfgobjcfg)
    {
        EO_INIT(.taskpriority)              EOMTHEEMSAPPLCFG_CFGOBJ_TASK_PRIORITYof, 
        EO_INIT(.taskstacksize)             EOMTHEEMSAPPLCFG_CFGOBJ_TASK_STACKSIZEof        
    },
    EO_INIT(.runobjcfg)
    {
        EO_INIT(.taskpriority) 
        {
            EOMTHEEMSAPPLCFG_RUNOBJ_TASK_RX_PRIORITYof, EOMTHEEMSAPPLCFG_RUNOBJ_TASK_DO_PRIORITYof, EOMTHEEMSAPPLCFG_RUNOBJ_TASK_TX_PRIORITYof
        },
        EO_INIT(.taskstacksize)
        {
            EOMTHEEMSAPPLCFG_RUNOBJ_TASK_RX_STACKSIZEof, EOMTHEEMSAPPLCFG_RUNOBJ_TASK_DO_STACKSIZEof, EOMTHEEMSAPPLCFG_RUNOBJ_TASK_TX_STACKSIZEof
        }, 
        EO_INIT(.haltimerstart)
        {
            EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_STARTRX, EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_STARTDO, EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_STARTTX
        },  
        EO_INIT(.haltimeralert)
        {
            EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_ALERTRX, EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_ALERTDO, EOMTHEEMSAPPLCFG_RUNOBJ_TIMER_ID_ALERTTX
        },        
        EO_INIT(.period)                    EOMTHEEMSAPPLCFG_RUNOBJ_PERIOD,
        EO_INIT(.execRXafter)               EOMTHEEMSAPPLCFG_RUNOBJ_RX_AFTER,            
        EO_INIT(.safeRXexecutiontime)       EOMTHEEMSAPPLCFG_RUNOBJ_RX_SAFETIME,
        EO_INIT(.execDOafter)               EOMTHEEMSAPPLCFG_RUNOBJ_DO_AFTER,
        EO_INIT(.safeDOexecutiontime)       EOMTHEEMSAPPLCFG_RUNOBJ_DO_SAFETIME,
        EO_INIT(.execTXafter)               EOMTHEEMSAPPLCFG_RUNOBJ_TX_AFTER,
        EO_INIT(.safeTXexecutiontime)       EOMTHEEMSAPPLCFG_RUNOBJ_TX_SAFETIME,
        EO_INIT(.maxnumofRXpackets)         EOMTHEEMSAPPLCFG_RUNOBJ_RX_MAXPACKETS,                // add a control that is is lower equal to inpdatagramnumber.
        EO_INIT(.maxnumofTXpackets)         EOMTHEEMSAPPLCFG_RUNOBJ_TX_MAXPACKETS,                // so far it can be only 0 or 1 
        EO_INIT(.modeatstartup)             (eOemsrunner_mode_t) EOMTHEEMSAPPLCFG_RUNOBJ_MODE_AT_STARTUP,
        EO_INIT(.defaultTXdecimationfactor) EOMTHEEMSAPPLCFG_RUNOBJ_TXDECIMATIONFACTOR        
    },
    EO_INIT(.applbodycfg)       &theapplbodyconfig
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




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

EO_weak extern void eom_emsapplcfg_hid_userdef_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    const char empty[] = "EO?";
    const char *err = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
    char str[64];
    EOMtheEMSapplCfg *emsapplcfg = eom_emsapplcfg_GetHandle();
    
    const char *eobjstr = (NULL != caller) ? (caller->eobjstr) : (empty);
    uint32_t taskid = (NULL != caller) ? (caller->taskid) : (0);

    if(emsapplcfg->errmng_haltrace_enabled)
    {
        snprintf(str, sizeof(str), "[%s] %s in tsk %d: %s", err, eobjstr, taskid, info);
        hal_trace_puts(str);
    }
    if(errtype <= eo_errortype_error)
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

