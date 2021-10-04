
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __IPAL_BASE_CFG2_H_
#define __IPAL_BASE_CFG2_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       ipal_base_cfg2.h
    @brief      This header contains data structures and API for a new initialization of the IPAL driver
                which is compatible w/ lwIP (and hopefully future stacks).    
    @author     marco.accame@iit.it
    @date       29 sept 2021
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "ipal_base.h"
#include "ipal_hal_eth.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty section

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{
    ipal_ipstack_t      arch_ipstack;           /**< The underlying TCP/IP stack: use ipal_ipstack_t.  */
    ipal_memorymodel_t  memorymodel;            /**< The used memory model. */
    uint16_t            supportedmodules;       // use flags ...
    uint8_t             filler[4];
} ipal_cfg2_support_t; __ipalVERIFYsizeof(ipal_cfg2_support_t, 8)

typedef struct
{
    ipal_reltime_t      sys_timetick;           /**< The time tick of the stack which is used to count TCP timeout. In usec but multiple of 10 ms. */
    uint16_t            sys_mempoolsize;        /**< The size of memorypool used for IP packets. in ipal_memmodel_static it is used the internal size  */
    uint8_t             filler[2];
} ipal_cfg2_system_t; __ipalVERIFYsizeof(ipal_cfg2_system_t, 8)


typedef struct
{
    ipal_mac48addr_t    eth_mac;                /**< The MAC address of the ethernet interface   */
    ipal_ipv4addr_t     eth_ip;                 /**< The IP address of the ethernet interface   */
    ipal_ipv4addr_t     eth_mask;               /**< The IP mask of the ethernet interface   */
    uint8_t             eth_isrpriority;        /**< the hw priority of the ethernet isr */
    uint8_t             filler[7];
    /** The function is executed to initialise ethernet. its arg is TBD */
    ipal_result_t (*hal_eth_init)(ipal_hal_eth_cfg_t* cfg);    
    /** The function is executed to enable ethernet ISR.  */
    ipal_result_t  (*hal_eth_enable)(void);
    /** The function is executed to disable ethernet ISR. */
    ipal_result_t  (*hal_eth_disable)(void);
    /** The function is executed to send an ethernet frame. Use the HAL version.  */
    ipal_result_t  (*hal_eth_sendframe)(ipal_hal_eth_frame_t* frame);  
	/* Get frame size */
	uint16_t (*get_frame_size)(void);	
	/* Get frame */
	void (*get_frame)(uint16_t length, uint8_t* frame);    
} ipal_cfg2_eth_t; __ipalVERIFYsizeof(ipal_cfg2_eth_t, 48)


typedef struct
{
    ipal_reltime_t      arp_cachetimeout;       /**< The max time in usec (but multiple of sec) a pair mac-ip is stored in table in case of ipal_arp_cache_removeontout   */
    ipal_reltime_t      arp_retrytimeout;       /**< The time between two successive ARP retries (in usec but expressed in sec)    */ 
    uint8_t             arp_cachesize;          /**< The size of the table containing the pairs mac-ip   */
    uint8_t             arp_retrymaxnum;        /**< The number of times a single call to ipal_arp_request() retries to solve the IP address    */
    ipal_bool_t         arp_autonotify;         /**< If 1 then the device sends a gratuitous ARP notification at startup    */
    uint8_t             filler[5];
} ipal_cfg2_arp_t; __ipalVERIFYsizeof(ipal_cfg2_arp_t, 16)


typedef struct
{
    uint8_t             udp_socketnum;          /**< Max number of UDP sockets. TFTP uses one UDP socket. DHCP uses also one. NBNS one */ 
    uint8_t             filler[7];
} ipal_cfg2_udp_t; __ipalVERIFYsizeof(ipal_cfg2_udp_t, 8)

typedef struct
{
    uint8_t             igmp_groupsnum;         /**< Max number of IGMP groups the device can join */
    uint8_t             filler[7];
} ipal_cfg2_igmp_t; __ipalVERIFYsizeof(ipal_cfg2_igmp_t, 8)

typedef struct
{
    uint8_t             notsuppotednow[8];
} ipal_cfg2_dhcp_t; __ipalVERIFYsizeof(ipal_cfg2_dhcp_t, 8)


/** @typedef    typedef struct ipal_fn_cfg2_t
    @brief      ipal_fn_cfg2_t keeps pointers to functions of IPAL which can be redefined by the user.
                it is an improved version of ipal_fn_cfg_t
 **/
typedef struct
{	
    // in here we have system functions defined externally, such as callback, etc.
    // so far we keep out mutex and scheduling funtions
    
    void (*usr_on_fatal_error)(ipal_fatalerror_t errorcode, const char * errormsg);
    
	/* Signal to higher layer - This is the last function called by movetohigherlayer. 
	   For instance, it can be used to send a signal to a waiting thread.
	 */
	void (*signal_rx_frame)(void);
	
	/* Signal to higher layer - This is the last function called by low_level_output 
	   For instance, it can be used to send a signal to a waiting thread.
	 */
	void (*signal_tx_frame)(void);   

    /** The function creates a mutex for internal protection. If NULL then there is no thread-safety */ 
    void* (*osal_mutex_new)(void);
    /** The function takes a mutex with a given timeout in microseconds. If NULL then there is no thread-safety */ 
    ipal_result_t (*osal_mutex_take)(void *mutex, ipal_reltime_t tout);
    /** The function releases a mutex . If NULL then there is no thread-safety */ 
    ipal_result_t (*osal_mutex_release)(void *mutex);
    /** The forever value for a timeout in microseconds. */ 
    ipal_reltime_t osal_param_tout_forever;   
    uint32_t dummy;
    
} ipal_cfg2_extfn_t; __ipalVERIFYsizeof(ipal_cfg2_extfn_t, 32)


typedef enum 
{
    ipal_cfg2_supportedflag_eth     = 0x0001,
    ipal_cfg2_supportedflag_arp     = 0x0002,
    ipal_cfg2_supportedflag_udp     = 0x0004,
    ipal_cfg2_supportedflag_igmp    = 0x0008,
    ipal_cfg2_supportedflag_dhpc    = 0x0010,
    
    ipal_cfg2_supportedflag_extfn2  = 0x8000
} ipal_cfg2_supportedflags_t;


typedef struct
{   // 8+8+4+4+4+4+4+4
    ipal_cfg2_support_t *support;
    ipal_cfg2_system_t  *system;    
    ipal_cfg2_eth_t     *eth;    
    ipal_cfg2_arp_t     *arp;
    ipal_cfg2_udp_t     *udp;
    ipal_cfg2_igmp_t    *igmp;
    ipal_cfg2_dhcp_t    *dhcp;
            
    // external system functions
    ipal_cfg2_extfn_t   *extfn2;  
    
} ipal_cfg2_t; __ipalVERIFYsizeof(ipal_cfg2_t, 32)
 
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const uint16_t ipal_cfg2_supportedflags_basic; // eth, arp, udp, igmp, extfn2


// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint32_t ipal_base_memory_getsize2(const ipal_cfg2_t *cfg, uint32_t *size04aligned); 

extern ipal_result_t ipal_base_initialise2(const ipal_cfg2_t *cfg2, uint32_t *data04aligned);


 
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 


#endif  // include-guard __IPAL_BASE_CFG2_H_


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

