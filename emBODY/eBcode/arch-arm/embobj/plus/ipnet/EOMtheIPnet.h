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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMTHEIPNET_H_
#define _EOMTHEIPNET_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheIPnet.h
    @brief      This header file implements public interface to the IP net singleton used in MEE.
    @author     marco.accame@iit.it
    @date       12/28/2009
**/

/** @defgroup eom_theipnet Object EOMtheIPnet
    The EOMtheIPnet allows ...... 
    

    @todo acemor-facenda: do documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"
#include "ipal.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
 

/** @typedef    typedef struct EOMtheIPnet_hid EOMtheIPnet
    @brief      EOMtheIPnet is an opaque struct. It is used to implement data abstraction for the multi-task 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOMtheIPnet_hid EOMtheIPnet;

/**	@typedef    typedef enum eOmipnet_taskid_t 
 	@brief      Contains the identifiers of the tasks inside the EOMtheIPnet. 
 **/ 
typedef enum
{
    eomipnet_task_proc      = 0,            /**< it is the task which performs efefctive processing of teh ipal calls. it send and receives packets */ 
    eomipnet_task_tick      = 1             /**< it is teh task which periodically increment timeouts of TCP/IP stack */
} eOmipnet_taskid_t;


/**	@typedef    typedef struct eOmipnet_cfg_t 
 	@brief      Contains the configuration for the EOMtheIPnet. 
 **/ 
typedef struct
{
    uint8_t         procpriority;           /**< The priority of the worker task. The clock task has priority-1 */
    uint16_t        procstacksize;          /**< The stack size of the worker task */
    eOreltime_t     procmaxidletime;        /**< The timeout of the wait for an event in worker task */
    eObool_t        procwakeuponrxframe;    /**< the ISR sends an event to worker task at reception of a frame for faster processing */ 
    uint8_t         tickpriority;           /**< The priority of the tick task. It should have procpriority-1 */
    uint16_t        tickstacksize;          /**< The stack size of the tick task */
} eOmipnet_cfg_t;

/**	@typedef    typedef struct eOmipnet_cfg_dtgskt_t 
 	@brief      Contains the configuration for the EOdatagramSocket managed by EOMtheIPnet. 
 **/ 
typedef struct
{
    uint8_t         numberofsockets;                /**< The number of datagram sockets */
    uint8_t         maxdatagramenqueuedintx;        /**< The number of datagrams than can be enqueued for tx */
} eOmipnet_cfg_dtgskt_t;


/**	@typedef    typedef struct eOmipnet_cfg_addr_t 
 	@brief      Contains the configuration for the addresses managed by EOMtheIPnet. 
 **/ 
typedef struct
{
    eOmacaddr_t     macaddr;
    eOipv4addr_t    ipaddr;
    eOipv4addr_t    ipmask;
} eOmipnet_cfg_addr_t;


typedef struct
{
    eOmipnet_cfg_t              ipnetcfg;
    const ipal_cfg_any_t*       ipalcfg; // it accepts a ipal_cfg_t* or a ipal_cfg2_t* (only if IPAL_use_cfg2 is defined) 
    eOmipnet_cfg_addr_t*        addrcfg;
    eOmipnet_cfg_dtgskt_t       dtgskcfg;
} eOmipnet_whole_cfg_t;


typedef struct
{
    uint32_t    datagrams_failed_to_go_in_rxfifo;
    uint32_t    datagrams_failed_to_go_in_txosalqueue;
    uint32_t    datagrams_failed_to_be_retrieved_from_txfifo;
    uint32_t    datagrams_failed_to_be_sent_by_ipal;    
} eOmipnet_diagnosticsinfo_t;
   
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOmipnet_cfg_t eom_ipnet_DefaultCfg; // = {220, 1024, 20000, eobool_true, 219, 128};

extern const eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_DefaultCfg; // = {2, 8};

extern const eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_NOsocketCfg; // = {0, 0};

extern const eOmipnet_cfg_addr_t eom_ipnet_addr_DefaultCfg; // = {0, 0, 0};




// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheIPnet * eom_ipnet_Initialise(uint8_t prio, uint16_t stacksize, eOreltime_t maxidle, eObool_t taskwakeuponrxframe
                                          ipal_params_cfg_t *ipcfg, 
                                          eOmacaddr_t macaddr, eOipv4addr_t ipaddr, eOipv4addr_t ipmask,
                                          uint8_t maxdgramsocks)
    @brief      Initialises the singleton EOMtheIPnet and puts it in activated state.
                When called the first time, the function creates all data structure required to guarantee the
                correct communication need of upto maxdgramsocks datagram sockets.
    @param      prio            The priority of the worker task
    @param      stacksize       The stack size in bytes of the worker task
    @param      maxidle         The maximum time that the worker task waits for an event before processing the IPAL
    @param      taskwakeuponrxframe     If eobool_true, the worker task is waken up by the Ethernet ISR at teh reception of a frame.
    @param      ipcfg           The configuration of the IPAL. It can be either a ipal_cfg_t* or a ipal_cfg2_t, but
                                this latter only if macro IPAL_use_cfg2 is defined.
    @param      macaddr         If non-zero, it overrides the MAC address contained in ipcfg. Use eo_common_macaddr() to
                                specify a non-zero MAC address.
    @param      ipaddr          If non-zero, it overrides the IP address contained in ipcfg. Use eo_common_ipv4ddr() to
                                specify a non-zero IP address.
    @param      ipmask          If non-zero, it overrides the IP mask contained in ipcfg. Use eo_common_ipv4ddr() to
                                specify a non-zero IP mask.
    @param      maxdgramsocks   The maximum number of datagram sockets that the network can run at the same time.
    @return     The handle to the RTOS net IP.
 **/
extern EOMtheIPnet * eom_ipnet_Initialise(const eOmipnet_cfg_t *ipnetcfg,
                                          const ipal_cfg_any_t *ipcfg, 
                                          const eOmipnet_cfg_addr_t *addrcfg,
                                          const eOmipnet_cfg_dtgskt_t *dtgskcfg
                                          ); 



/** @fn         extern EOMtheIPnet* eom_ipnet_GetHandle(void)
    @brief      Returns an handle to the singleton EOMtheIPnet. The singleton must have been initialised
                with eom_ipnet_Initialise(), otherwise this function call will return NULL.
    @return     The handle to the IP net (or NULL upon in-initialised singleton)
 **/
extern EOMtheIPnet* eom_ipnet_GetHandle(void);
                                          
                                          
extern eOipv4addr_t eom_ipnet_GetIPaddress(EOMtheIPnet *ip);


/** @fn         extern eOresult_t eom_ipnet_Deactivate(EOMtheIPnet *ip)
    @brief      Deactivates the processing of the TCP/IP stack.
    @return     Upon success: eores_OK. Else: eores_NOK_generic if operation fails, eores_NOK_nullpointer if pointer is NULL.
 **/
extern eOresult_t eom_ipnet_Deactivate(EOMtheIPnet *ip);


/** @fn         extern eOresult_t eom_ipnet_Activate(EOMtheIPnet *ip)
    @brief      Activates the processing of the TCP/IP stack.
    @return     Upon success: eores_OK. Else: eores_NOK_generic if operation fails, eores_NOK_nullpointer if pointer is NULL.
 **/
extern eOresult_t eom_ipnet_Activate(EOMtheIPnet *ip);


/** @fn         extern eOresult_t eom_ipnet_ResolveIP(EOMtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout)
    @brief      It asks to IPnet.run to resolve IP (via ARP) in tout usec. The caller task is stopped until
                IPnet.run resolves or exit by timeout. 
    @param      ip          The IP net
    @param      ipaddr      The IP address to be solved.
    @param      tout        The timeout. However, never more than 65000 ARP attempts.
    @return     Upon success: eores_OK, eores_NOK_generic if IP is 0.0.0.0, eores_NOK_timeout upon timeout,
                eores_NOK_nullpointer is @e ip is NULL.
 **/
extern eOresult_t eom_ipnet_ResolveIP(EOMtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout);

 
#if defined(TEST_ARP)
extern eOresult_t eom_ipnet_ResolveIP_TEST(EOMtheIPnet *ip, eOipv4addr_t ipaddr, eOreltime_t tout);
#endif

/** @fn         extern eOresult_t eom_ipnet_IGMPgroupJoin(EOMtheIPnet *ip, eOipv4addr_t igmp)
    @brief      Joins an IGMP multicast group.
    @param      ip          The IP net
    @param      igmp        The group. Must belong to [224-239].x.x.x but not 224.0.0.0 or 224.0.0.1
    @return     Upon success: eores_OK, eores_NOK_generic if group is invalid, eores_NOK_null pointer if @e ip is NULL
 **/
extern eOresult_t eom_ipnet_IGMPgroupJoin(EOMtheIPnet *ip, eOipv4addr_t igmp);


/** @fn         extern eOresult_t eom_ipnet_IGMPgroupLeave(EOMtheIPnet *ip, eOipv4addr_t igmp)
    @brief      Leaves an IGMP multicast group.
    @param      ip          The IP net
    @param      igmp        The group. Must belong to [224-239].x.x.x but not 224.0.0.0 or 224.0.0.1
    @return     Upon success: eores_OK, eores_NOK_generic if group is invalid, eores_NOK_null pointer if @e ip is NULL
 **/
extern eOresult_t eom_ipnet_IGMPgroupLeave(EOMtheIPnet *ip, eOipv4addr_t igmp);



extern EOMtask* eom_ipnet_GetTask(EOMtheIPnet *ip, eOmipnet_taskid_t tskid);

//this funcion copi internal data to memory pointed by dgn_ptr
extern eOresult_t eom_ipnet_GetDiagnosticsInfo(EOMtheIPnet *ip, eOmipnet_diagnosticsinfo_t *dgn_ptr);

//return pointer to diagnostics internal data
extern  eOmipnet_diagnosticsinfo_t * eom_ipnet_GetDiagnosticsInfoHandle(EOMtheIPnet *ip);

/** @}            
    end of group eom_theipnet  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

