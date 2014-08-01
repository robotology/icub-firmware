/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

#ifndef _HAL_ETH_H_
#define _HAL_ETH_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_eth.h
    @brief      This header file implements public interface to the hal eth module.
    @author     marco.accame@iit.it, valentina.gaggero@iit.it
    @date       09/09/2011
**/

/** @defgroup doxy_group_hal_eth HAL ETH

    The HAL ETH ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_common.h"



// - public #define  --------------------------------------------------------------------------------------------------

#define hal_ETH_macaddress(m1, m2, m3, m4, m5, m6)   ((uint64_t)hal_BASE_8BtoU64(m1, m2, m3, m4, m5, m6, 0, 0))
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef uint64_t hal_eth_macaddress_t 
    @brief      hal_eth_macaddress_t is used to store a mac address in its 6 least significant bytes
                in such a way that ...
 **/ 
typedef uint64_t hal_eth_macaddress_t;


#ifdef HAL_COMPATIBLE_LWIP
typedef struct
{
    void     *frame;
    uint32_t length;
}hal_eth_genericframe_t;

#else
/** @typedef    typedef struct hal_eth_frame_t 
    @brief      hal_eth_frame_t is used to exchange ethernet frames to hal. The frame data must begin with
                member datafirstbyte. One method is to use a hal_eth_frame_t pointer and to cast it to
                a portion of memory with size length+4.
 **/ 
typedef struct
{
    uint16_t length;                /**< the length of the frame in bytes */
    uint16_t index;                 /**< index to the used byte */
    uint8_t  datafirstbyte[1];      /**< keeps first byte of the frame. The other length-1 bytes must be consecutive to this */
} hal_eth_frame_t; 
#endif


#ifdef HAL_COMPATIBLE_LWIP
/** @typedef    typedef struct hal_eth_stackIPhookfunctions 
    @brief      hal_eth_stackIPhookfunctions is used to configure the behavior of the hal ethernet depending by stackIP used.
                 - move2higherlayer is used when a frame is received in order to move data from inputbuffer to stack's buffer
                 - move2lowerlayer is used when stackIP calls sendfram function: it is use to copy data from stack's buffer to ha's buffer
                 - alerthigherlevel() lets somone known that one or more frame  frame are available.
 **/
typedef struct
{
    uint32_t (*moveframe2higherlayer)(uint8_t *inputbuffer, uint32_t size);
    uint32_t (*moveframe2lowerlayer)(hal_eth_genericframe_t *genframe, uint8_t *outputbuffer);
    void (*alerthigherlayer)(void);
} hal_eth_stackIPhookfunctions_t;

#else
/** @typedef    typedef struct hal_eth_onframereception_t 
    @brief      hal_eth_onframereception_t is used to configure the behavior of the hal ethernet on reception of
                one frame. The eth ISR will ask memory for copying the new frame using the frame_new() method,
                will give the frame back to application with frame_movetohigherlevel(), and also will call
                a method frame_alerthigherlevel() to let somone known that the frame is available.
 **/
typedef struct
{
    hal_eth_frame_t * (*frame_new)(uint32_t size);
    void (*frame_movetohigherlayer)(hal_eth_frame_t *frame);
    void (*frame_alerthigherlayer)(void);
} hal_eth_onframereception_t;
#endif




/** @typedef    typedef struct hal_eth_cfg_t;
    @brief      contains configuration data of eth peripheral.
 **/
typedef struct
{
    hal_interrupt_priority_t        priority;                   /**< keeps priority of the ISR */
    uint8_t                         capacityoftxfifoofframes;   /**< the number of eth frames buffered by tx dma in transmission */
    uint8_t                         capacityofrxfifoofframes;   /**< the number of eth frames buffered in rx dma before the ISR can move to higher layers */    
    uint8_t                         dummy;                      /**< nothing */
#ifdef HAL_COMPATIBLE_LWIP
    hal_eth_stackIPhookfunctions_t*   stackIpfuncs;
#else
    hal_eth_onframereception_t*     onframerx;                  /**< The user must customise the behavior in reception using these functions. Only the alert function can be NULL. */
#endif
    hal_eth_macaddress_t            macaddress;                 /**< Keeps the mac address */
} hal_eth_cfg_t;


/** @typedef    typedef struct hal_eth_network_functions_t 
    @brief      hal_eth_network_functions_t is used to to group into a single variables all the functions which are
                needed to run a stack based on ethernet, such as a tcp/ip stack.
 **/
typedef struct
{
    hal_result_t (*eth_init)(const hal_eth_cfg_t *cfg);
    hal_result_t (*eth_enable)(void);
    hal_result_t (*eth_disable)(void);
#ifdef HAL_COMPATIBLE_LWIP
    hal_result_t (*eth_sendframe)(hal_eth_genericframe_t *fr);
#else
    hal_result_t (*eth_sendframe)(hal_eth_frame_t *fr);
#endif
} hal_eth_network_functions_t;




/** @typedef    typedef enum hal_eth_t 
    @brief      contains ids of every possible eth peripheral.
 **/ 
typedef enum  
{ 
    hal_eth1        = 0             /**< the only eth */
} hal_eth_t;

enum { hal_eths_number = 1 };

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------





// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t hal_eth_init(const hal_eth_cfg_t *cfg)
    @brief      Initialise an external interface. Memory must have been already allocated using 
                the hal_initialise() function. If @e onrx is NULL or if anything goes wrong, 
                the function calls hal_on_fatalerror() with proper error code.
    @param      cfg         Contains the eth configuration.
    @return     hal_res_OK only on success.
    @warning    the ETH can be configured only once. The second call of hal_eth_init(cfg) will do nothing.
                The return value will be hal_res_OK if the parameter cfg is the same as one used the first time,
                otherwise it will be hal_res_NOK_generic to express the fact that the ETH is not initted as wanted.
 **/
extern hal_result_t hal_eth_init(const hal_eth_cfg_t *cfg);


/** @fn         extern hal_result_t hal_eth_enable(void)
    @brief      Enable the eth peripheral. 
    @warning    It is generally used by a TCP-IP stack
 **/
extern hal_result_t hal_eth_enable(void);


/** @fn         extern hal_result_t hal_eth_disable(void)
    @brief      Disable the eth peripheral. 
    @warning    It is generally used by a TCP-IP stack
 **/
extern hal_result_t hal_eth_disable(void);


/** @fn         extern hal_result_t hal_eth_sendframe(hal_eth_frame_t *frame)
    @brief      Sends a frame over the eth peripheral. 
    @param      frame           The frame to send
    @warning    It is generally used by a TCP-IP stack
 **/
#ifdef HAL_COMPATIBLE_LWIP
extern hal_result_t hal_eth_sendframe(hal_eth_genericframe_t *genframe);
#else
extern hal_result_t hal_eth_sendframe(hal_eth_frame_t *frame);
#endif

/** @fn         extern const hal_eth_network_functions_t * hal_eth_get_network_functions(void)
    @brief      retrieves the network functions used by HAL
    @return     a pointer containing {hal_eth_init, hal_eth_enable, hal_eth_disable, hal_eth_sendframe}
    @warning    It is generally used to initialise a TCP-IP stack
 **/
extern const hal_eth_network_functions_t * hal_eth_get_network_functions(void);


/** @fn         extern void hal_eth_smi_init(void)
    @brief      initialises the smi. this function is called inside hal_eth_init(). however, it is good practice to
                call it again (it does not harm) before using smi.
 **/
extern void hal_eth_smi_init(void);


/** @fn         extern uint16_t hal_eth_smi_read(uint8_t PHYaddr, uint8_t REGaddr)
    @brief      reads the 16 bits of register REGaddr in the physical with address PHYaddr. Both REGaddr and PHYaddr 
                have range is 0-31.
    @param      PHYaddr     The phy address w/ range 0-31.            
    @param      REGaddr     The reg address w/ range 0-31.
    @return     The 16-bit value of the register.    
 **/
extern uint16_t hal_eth_smi_read(uint8_t PHYaddr, uint8_t REGaddr);

/** @fn         extern uint16_t hal_eth_smi_read(uint8_t PHYaddr, uint8_t REGaddr)
    @brief      writes the 16 bits value in register REGaddr in the physical with address PHYaddr. Both REGaddr and PHYaddr 
                have range is 0-31.
    @param      PHYaddr     The phy address w/ range 0-31.            
    @param      REGaddr     The reg address w/ range 0-31.
    @param      value       the 16-bit value to write.    
 **/
extern void hal_eth_smi_write(uint8_t PHYaddr, uint8_t REGaddr, uint16_t value);


// -- eth diagnostics ----
// marco accame on 26 march 2014:
// the following are maintained as hal_eth, but you should use the similar hal_ethtransceiver functions

// added for diagnostics support for ethernet
typedef struct
{
    uint32_t linkisup:1;
    uint32_t autoNeg_done:1;
    uint32_t linkisgood:1;
    uint32_t linkspeed:1; // 1== 100Mb 0==10Mb
    uint32_t linkduplex:1; //1==full 0==half
    uint32_t dummy:27;
} hal_eth_phy_status_t;

typedef enum
{
    rxCrcError  = 0x07,
    rxUnicast   = 0x0D,
    rx64Octets  = 0x0E,
    txUnicast   = 0x1A
} hal_eth_phy_errors_info_type_t;

typedef struct
{
    uint32_t value;
    uint32_t counteroverflow:1;
    uint32_t validvalue:1;
    uint32_t dummy:30;
} hal_eth_phy_errorsinfo_t;


/** @fn         extern hal_boolval_t hal_eth_check_links(void)
    @brief      checks all physical links. In output linkst_mask indicate each phy status: 
                if bit in pos x values 1 then phy num x is up, else is down. phy num starts to count from 0.
                links_num in output contains num of used phy.
    @return     if linkst_mask or links_num is null returns hal_res_NOK_nullpointer else hal_res_OK
    @warning    It is board dependent.
 **/
extern hal_result_t hal_eth_check_links(uint8_t *linkst_mask, uint8_t *links_num);


/** @fn         extern hal_result_t hal_eth_check_links(void)
    @brief      check all physical links.
    @param      links_num           num of links to check. 
                For example, in ems board @links_num can value 1-3.(third links is rmii)
    @param      link_list           pointer to vector of @links_num elements. In output contains links' status.
    @return     error if @links_list is null or links_num is bigger than num of link present on boards.
    @warning    It is board dependent
 **/
extern hal_result_t hal_eth_get_links_status(hal_eth_phy_status_t* link_list, uint8_t links_num);


/** @fn         extern hal_result_t hal_eth_get_errors_info(uint8_t phynum, hal_eth_phy_errors_info_type_t errortype, uint32_t *result)
    @brief      in output @result will contain num of errors of tyepe @errortype on physical link @phynum
    @return     hal_res_OK or hal_res_NOK_...
    @warning    It is board dependent
 **/
extern hal_result_t hal_eth_get_errors_info(uint8_t phynum, hal_eth_phy_errors_info_type_t errortype, hal_eth_phy_errorsinfo_t *result);


/** @}            
    end of group doxy_group_hal_eth  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



