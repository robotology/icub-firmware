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
#ifndef _IPAL_BASE_H_
#define _IPAL_BASE_H_


/** @file       ipal_base.h
	@brief      This header file implements public interface to the ip abstraction layer: base services and types 
	@author     marco.accame@iit.it
	@date       12/12/2011
**/

/* @defgroup ipal_base IP abstraction layer: base services and types
    cercecvecve.
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "stdio.h"



// - public #define  --------------------------------------------------------------------------------------------------


/** @def        IPAL_macaddr(m0, m1, m2, m3, m4, m5)
    @brief      Gives back in IPAL format the MAC-48 address represented by m0-m1-m2-m3-m4-m5 
 **/
#define IPAL_mac48addr(m0, m1, m2, m3, m4, m5)   ((uint64_t)(0|((((uint64_t)(m5))&0xff))<<40)|((((uint64_t)(m4)&0xff))<<32)|((((uint64_t)(m3)&0xff))<<24)|((((uint64_t)(m2)&0xff))<<16)|((((uint64_t)(m1)&0xff))<<8)|((((uint64_t)(m0)&0xff))))
 

/** @def        IPAL_ipv4addr(i0, i1, i2, i3)
    @brief      Gives back in IPAL format the IP address represented by i0.i1.i2.i3 
 **/
#define IPAL_ipv4addr(i0, i1 ,i2, i3)   ((uint32_t)(((((uint32_t)(i3))&0xff))<<24)|((((uint32_t)(i2)&0xff))<<16)|((((uint32_t)(i1)&0xff))<<8)|(((uint32_t)(i0)&0xff)))


/** @def        ipal_ipv4addr_INADDR_ANY
    @brief      The INADDR_ANY of IPAL.
 **/            
#define IPAL_ipv4addr_INADDR_ANY        0


/** @def        ipal_ipv4addr_BROADCAST
    @brief      The main broadcast address.
 **/ 
#define IPAL_ipv4addr_BROADCAST         IPAL_ipv4addr(255, 255, 255, 255)


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum ipal_result_t 
    @brief      ipal_result_t contains all possible results from a function
 **/ 
typedef enum 
{
    ipal_res_OK                     = 0,    /**< correct result */
    ipal_res_NOK_generic            = -1    /**< generic error */
} ipal_result_t;



/** @typedef    typedef enum ipal_ipstack_t 
    @brief      ipal_ipstack_t contains all possible TCP/IP stacks supported by IPAL
 **/ 
typedef enum
{
    ipal_ipstack_iitmod_tcpnetarm   = 0,    /**< the only supported so far is TCPnet by ARM wrapped by IIT */
    ipal_ipstack_iitmod_lwip        = 1     /**< The ligthweigth IP is already API compatible, but not supported yet */
} ipal_ipstack_t;


/** @typedef    typedef enum ipal_memorymodel_t 
    @brief      ipal_memorymodel_t contains all possible memory models supported by IPAL
 **/ 
typedef enum
{
    ipal_memmodel_static            = 0,    /**< uses internally allocated static memory.
                                                 in this mode, the ipal cfg specifies how many objects are required but all the
                                                 memory is already internally allocated by the library. there are 3 static sizes:
                                                 small, medium, and large.  if the ipal_base_initialise() will require more objects
                                                 than what are internally available, then an error is issued. 
                                                 this mode is the only supported so far with the stack ipal_ipstack_iitmod_tcpnetarm
                                                 as source codes are not available to allow the more flexible ipal_memmode_external mode */
    ipal_memmodel_external          = 1,    /**< uses external memory which the user of the library passes with ipal_base_initialise().
                                                 in this mode, the memory required is computed by ipal_base_memory_getsize()
                                                 and tuned on the values inside the ipal cfg. not supported so far  */                                                 
    ipal_memmodel_dynamic           = 2     /**< uses memory allocated at run time on the heap, not supported so far */,
} ipal_memorymodel_t;


/** @typedef    typedef enum ipal_fatalerror_t 
    @brief      ipal_fatalerror_t contains system specific error codes used in user-defined function usr_on_fatal_error()
 **/ 
typedef enum 
{
    ipal_error_generic                  = 0xE0,             
    ipal_error_missingconfiguration     = 0xE1,       
    ipal_error_missingmemory            = 0xE2,       
    ipal_error_incorrectparameter       = 0xE3,       
    ipal_error_unsupportedbehaviour     = 0xE4,
    ipal_error_internal0                = 0xF0,
    ipal_error_internal1                = 0xF1,
    ipal_error_internal2                = 0xF2,
    ipal_error_internal3                = 0xF3,             
    ipal_error_internal4                = 0xF4,
    ipal_error_internal5                = 0xF5
} ipal_fatalerror_t;


/** @typedef    typedef enum ipal_boolval_t 
    @brief      ipal_boolval_t contains values for bool type (ipal_bool_t).
    @warning    C99 contains bool, true, and false. To use C99 include "stdbool.h" and -c99 compiler option.
                At this point please redefine ipal_false and ipal_true to be equal to false and true.
 **/ 
typedef enum
{
    ipal_false = 0,      /**< false */
    ipal_true  = 1       /**< true */
} ipal_boolval_t;


/** @typedef    typedef enum ipal_bool_t 
    @brief      ipal_bool_t is the bool type. If C99 is available, use bool instead of uint8_t.
 **/ 
typedef uint8_t     ipal_bool_t;


/** @typedef    typedef uint32_t ipal_reltime_t 
    @brief      ipal_reltime_t is the relative time used by IPAL. It is expressed in micro-seconds and it max value
                is around 4294 seconds.
 **/  
typedef uint32_t    ipal_reltime_t;


/** @typedef    typedef uint64_t ipal_mac48addr_t
    @brief      The MAC-48 address used in IPAL. Only the least significant 6 bytes are used.
                It must be mapped in memory in network order. For example address aa-bb-cc-00-01-02 
                must be stored with 0xaa as the least significant byte, so that ((uint8_t*)(&macadr))[0] = 0xaa 
                and its memory layout in address order is aa-bb-cc-00-01-02-00-00. In little endian 
                architectures the value of the ipal_mac48addr_t variable is 0x0000020100ccbbaa 
 **/ 
typedef uint64_t    ipal_mac48addr_t;

/** @typedef    typedef uint32_t ipal_ipv4addr_t 
    @brief      The IP address used in IPAL. 
                It must be mapped in memory in network order. For example address 10.3.2.1 must be
                stored with 10 as the least significant byte, so that ((uint8_t*)(&ipadr))[0] = 10 and 
                its memory layout in address order is 0a-03-02-01. In little endian architectures the 
                value of the ipal_ipv4addr_t variable is 0x0102030a
 **/ 
typedef uint32_t    ipal_ipv4addr_t;


/** @typedef    typedef uint16_t ipal_port_t 
    @brief      IP port used in IPAL
 **/
typedef uint16_t    ipal_port_t;


/** @typedef    typedef struct ipal_packet_t 
    @brief      The packet used by a socket.
 **/
typedef struct
{
    uint16_t    size;       /**< size of data       */
    uint8_t*    data;       /**< pointer to data    */
} ipal_packet_t;


/** @typedef    typedef struct ipal_fn_cfg_t
    @brief      ipal_fn_cfg_t keeps pointers to functions of IPAL whcih can be redefined by the user.
 **/
typedef struct
{
    /** The function is executed when a fatal error is detected. If not redefined, it enters in a forever loop. */ 
    void    (*usr_on_fatal_error)(ipal_fatalerror_t errorcode, const char * errormsg);

    /** The function creates a mutex for internal protection. If NULL then there is no thread-safety */ 
    void*           (*osal_mutex_new)(void);
    /** The function takes a mutex with a given timeout in microseconds. If NULL then there is no thread-safety */ 
    ipal_result_t   (*osal_mutex_take)(void *mutex, ipal_reltime_t tout);
    /** The function releases a mutex . If NULL then there is no thread-safety */ 
    ipal_result_t   (*osal_mutex_release)(void *mutex);
    /** The forever value for a timeout in microseconds. */ 
    ipal_reltime_t  osal_param_tout_forever;
    /** The function which is executed to suspend scheduling. Use the function osal_system_scheduling_suspend() of OSAL.
        If not redefined (thus this value is NULL) it does nothing.  It is necessary to redefine only if the IPAL is used 
        in a multitasking execution environment (with an RTOS) */ 
    void            (*osal_system_scheduling_suspend)(void);
    /** The function which is executed to restart scheduling. Use the function osal_system_scheduling_restart() of OSAL.
        If not redefined (thus this value is NULL) it does nothing.  It is necessary to redefine only if the IPAL is used 
        in a multitasking execution environment (with an RTOS) */ 
    void            (*osal_system_scheduling_restart)(void);

    /** The function is executed to initialise ethernet. Use the HAL version. If NULL a fatal error is issued. */
    ipal_result_t  (*hal_eth_init)(void*);     // arg is: hal_eth_cfg_t *cfg
    /** The function is executed to enable ethernet ISR. Use the HAL version. If NULL a fatal error is issued. */
    ipal_result_t  (*hal_eth_enable)(void);
    /** The function is executed to disable ethernet ISR. Use the HAL version. If NULL a fatal error is issued. */
    ipal_result_t  (*hal_eth_disable)(void);
    /** The function is executed to send an ethernet frame. Use the HAL version. If NULL a fatal error is issued. */
    ipal_result_t  (*hal_eth_sendframe)(void*);      // arg is: hal_eth_frame_t*

    /** The function is executed inside the ETH ISR when a new frame is received. Use some send signal function 
        from OSAL to signal to the task periodically running ipal_sys_process_communication() to run it again so that
        it can process the received frame. If NULL nothing is executed. */
    void    (*usr_on_ethframe_received)(void);

    /** The function is executed by TFTP/FTP server. If NULL and TFTP/FTP is enabled, a fatal error is issued. */
    FILE* (*fopen)(const char *filename, const char *mode); 
    /** TThe function is executed by TFTP/FTP server. If NULL and TFTP/FTP is enabled, a fatal error is issued. */
    int (*fclose)(FILE * stream);
    /** The function is executed by TFTP/FTP server. If NULL and TFTP/FTP is enabled, a fatal error is issued. */
    size_t (*fread)(void * ptr, size_t size, size_t count, FILE *stream);
    /** The function is executed by TFTP/FTP server. If NULL and TFTP/FTP is enabled, a fatal error is issued. */
    size_t (*fwrite)(const void *ptr, size_t size, size_t count, FILE *stream);
    /** The function is executed by TFTP server. If NULL and TFTP is enabled, a fatal error is issued. */
    long int (*ftell)(FILE *stream);
    /** The function is executed by TFTP server. If NULL and TFTP is enabled, a fatal error is issued. */
    int (*fseek)(FILE * stream, long int offset, int origin);

    /** The function is executed by FTP server. If NULL and FTP is enabled, a fatal error is issued. */
    int (*fsal_delete)(const char *filename);
    /** The function is executed by FTP server. If NULL and FTP is enabled, a fatal error is issued. */
    int (*fsal_rename)(const char *name, const char *newname);
    /** The function is executed by FTP server. If NULL and FTP is enabled, a fatal error is issued. */
    int (*fsal_find)(const char *pattern, void *infosearch);

    /** The function is executed  when the telnet server receives a string in @e reveivedmsg.
        At this point the function @e usr_tnet_exec() must do its operation and fill a reply messages inside
        @e replymsg, whose length must be returned. If the function wants to terminate teh session, then, it must
        set to 1 the value pointed by @e quitsessionflag.
        If telenet is enables the function must be not NULL or a fatal error is issued. */
    uint16_t (*usr_tnet_exec)(const char *receivedmsg, char *replymsg, uint8_t *quitsessionflag);
    /** The string contains the login message of telnet. If NULL the default is used */
    const char * usr_tnet_login_msg;   
    /** The string contains the welcome message of telnet. If NULL the default is used */           
    const char * usr_tnet_welcome_msg;
    /** The string contains the prompt of telnet. If NULL the default is used */
    const char * usr_tnet_prompt;
    


} ipal_fn_cfg_t;


/** @typedef    typedef struct ipal_cfg_t 
    @brief      ipal_cfg_t is used to pass configuration parameters to the ipal.
 **/
typedef struct
{

    // arch
    ipal_ipstack_t      arch_ipstack;           /**< The underlying TCP/IP stack: an enum ipal_ipstack_t. ipal_ipstack_iitmod_tcpnetarm is 0   */
    ipal_memorymodel_t  memorymodel;            /**< The used memory model. */

    // sys
    ipal_reltime_t      sys_timetick;           /**< The time tick of the stack which is used to count TCP timeout. In usec but mutiple of 10 ms. */
    uint16_t            sys_mempoolsize;        /**< The size of memorypool used for IP packets. in ipal_memmodel_static it is used the internal size  */
    
    // eth
    uint8_t             eth_isrpriority;        /**< the hw priority of the ethernet isr */
    ipal_mac48addr_t    eth_mac;                /**< The MAC address of the ethernet interface   */
    ipal_ipv4addr_t     eth_ip;                 /**< The IP address of the ethernet interface   */
    ipal_ipv4addr_t     eth_mask;               /**< The IP mask of the ethernet interface   */
    
    // arp
    uint8_t             arp_cachesize;          /**< The size of the table containing the pairs mac-ip   */
    uint8_t             arp_retrymaxnum;        /**< The number of times a single call to ipal_arp_request() retries to solve the IP address    */
    ipal_bool_t         arp_autonotify;         /**< If 1 then the device sends a gratuitous ARP notification at startup    */
    ipal_reltime_t      arp_cachetimeout;       /**< The max time in usec (but multiple of sec) a pair mac-ip is stored in table in case of ipal_arp_cache_removeontout   */
    ipal_reltime_t      arp_retrytimeout;       /**< The time between two successive ARP retries (in usec but expressed in sec)    */

    
    // igmp
    ipal_bool_t         igmp_enable;            /**< Enables or disables IGMP */
    uint8_t             igmp_groupsnum;         /**< Max number of IGMP groups the device can join */
    

    // dhcp
    ipal_bool_t         dhcp_enable;            /**< Enables or disables DHCP client */
    
    // udp
    ipal_bool_t         udp_enable;             /**< Enables or disables UDP sockets. TFTP uses one UDP socket. DHCP uses also one. NBNS one */
    uint8_t             udp_socketnum;          /**< Max number of UDP sockets */
    
    // tcp sockets
    ipal_bool_t         tcp_enable;             /**< Enables or disables TCP sockets. One (two) TCP sockets are used by each telnet (FTP) session */                     
    uint8_t             tcp_socketnum;          /**< Max number of TCP sockets */        
    uint8_t             tcp_retrymaxnum;        /**< Max number of transmission retries */           
    ipal_reltime_t      tcp_retrytimeout;       /**< Time before each retransmission (in usec, but multiple of seconds) */                     
    ipal_reltime_t      tcp_connectiontimeout;  /**< Time before disconnection (in usec, but multiple of seconds and upto 64 seconds) */

 
    // tftp
    ipal_bool_t         tftp_enable;            /**< Enables or disables TFTP. Uses one UDP socket */
    uint8_t             tftp_retrymaxnum;       /**< Max number of transmission retries */  
    ipal_port_t         tftp_port;              /**< Used port (default is 69) */         
    ipal_reltime_t      tftp_timeout;           /**< Time before each retransmission (in usec, but multiple of seconds and upto 60 seconds) */


    // ftp
    ipal_bool_t         ftp_enable;             /**< Enables or disables FTP. Uses two UDP socket per connection*/        
    ipal_port_t         ftp_port;               /**< Used port (default is 21) */        
    ipal_bool_t         ftp_authenable;         /**< Enables authentication */        
    uint8_t             ftp_user[8];            /**< username */    
    uint8_t             ftp_pass[8];            /**< password */
 
 
    // telnet
    ipal_bool_t         tel_enable;             /**< Enables or disables telnet. Uses one UDP socket per connection*/       
    ipal_port_t         tel_port;               /**< Used port (default is 23) */        
    ipal_bool_t         tel_authenable;         /**< Enables authentication */        
    uint8_t             tel_user[8];            /**< username */    
    uint8_t             tel_pass[8];            /**< password */

    
    // functions
    ipal_fn_cfg_t       extfn;                  /**< configuration functions */   
    
} ipal_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern uint32_t ipal_base_memory_getsize(const ipal_cfg_t *cfg, uint32_t *size04aligned)
    @brief      Function which is returns the size of the required memory for a given configuration.
    @param      cfg             The target configuration.
    @param      size04aligned   The required size (if not NULL).
    @return     The required size.
 **/
extern uint32_t ipal_base_memory_getsize(const ipal_cfg_t *cfg, uint32_t *size04aligned);


/** @fn         extern ipal_result_t ipal_base_initialise(const ipal_cfg_t *cfg, uint32_t *data04aligned)
    @brief      Function which initialises the IPAL.
    @param      cfg             The target configuration.
    @param      data04aligned   The required memory.
    @return     Always ipal_res_OK.  If any error, then it calls usr_on_fatal_error().
 **/
extern ipal_result_t ipal_base_initialise(const ipal_cfg_t *cfg, uint32_t *data04aligned);

  


/* @}            
    end of group ipal_base  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

