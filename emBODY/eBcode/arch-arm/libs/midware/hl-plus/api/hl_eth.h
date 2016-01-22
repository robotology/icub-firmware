/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HL_ETH_H_
#define _HL_ETH_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_eth.h
    @brief      This header file implements eth utilities for hl library
    @author     marco.accame@iit.it
    @date       10/21/2013
**/

/** @defgroup doxy_group_hl_eth HL ETH utilities

    The HL ETH ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_gpio.h"
#include "hl_sys.h"



// - public #define  --------------------------------------------------------------------------------------------------

#define hl_ETH_macaddress(m1, m2, m3, m4, m5, m6)   ((uint64_t)hl_BASE_8BtoU64(m1, m2, m3, m4, m5, m6, 0, 0))
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum hl_eth_mif_t 
    @brief      contains the possible media interfaces.
 **/
typedef enum
{   
    hl_eth_mif_rmii     = 0,        /**< media interface rmii (Reduced Media Independent Interface) */
    hl_eth_mif_mii      = 1         /**< media interface mii  (Media Independent Interface)         */
} hl_eth_mif_t;


/** @typedef    typedef struct hl_eth_gpio_rmii_t 
    @brief      contains gpio map of the rmii.
 **/
typedef struct
{   // RMII: Reduced Media Independent Interface   
    hl_gpio_map_t       ETH_RMII_REF_CLK;       /**< mapping of gpio used for ETH_RMII_REF_CLK pin  */
    hl_gpio_map_t       ETH_RMII_TX_EN;         /**< mapping of gpio used for ETH_RMII_TX_EN pin    */
    hl_gpio_map_t       ETH_RMII_TXD0;          /**< mapping of gpio used for ETH_RMII_TXD0 pin     */
    hl_gpio_map_t       ETH_RMII_TXD1;          /**< mapping of gpio used for ETH_RMII_TXD1 pin     */
    hl_gpio_map_t       ETH_RMII_CRS_DV;        /**< mapping of gpio used for ETH_RMII_CRS_DV pin   */
    hl_gpio_map_t       ETH_RMII_RXD0;          /**< mapping of gpio used for ETH_RMII_RXD0 pin     */
    hl_gpio_map_t       ETH_RMII_RXD1;          /**< mapping of gpio used for ETH_RMII_RXD1 pin     */
} hl_eth_gpio_rmii_t;


/** @typedef    typedef struct hl_eth_gpio_mii_t 
    @brief      contains gpio map of the mii.
 **/
typedef struct
{   // MII: Media Independent Interface
    uint8_t             tobedone;
} hl_eth_gpio_mii_t;


/** @typedef    typedef union hl_eth_gpio_mif_t 
    @brief      contains gpio map of the media interface.
 **/
typedef union
{   // media interface between the mac and a phy chip
    hl_eth_gpio_rmii_t  rmii;
    hl_eth_gpio_mii_t   mii;
} hl_eth_gpio_mif_t;


/** @typedef    typedef struct hl_eth_gpio_smi_t 
    @brief      contains gpio map of the smi.
 **/
typedef struct
{   // SMI: Serial Management Interface
    hl_gpio_map_t       ETH_MDC;                /**< mapping of gpio used for ETH_MDC pin */
    hl_gpio_map_t       ETH_MDIO;               /**< mapping of gpio used for ETH_MDIO pin */
} hl_eth_gpio_smi_t;


/** @typedef    typedef struct hl_eth_gpiomap_t 
    @brief      hl_eth_gpiomap_t contains the pin mapping for an eth peripherals
 **/
typedef struct
{
    hl_eth_mif_t        mif;            /**< always rmii  */
    hl_eth_gpio_mif_t   gpio_mif;       /**< media interface */    
    hl_eth_gpio_smi_t   gpio_smi;       /**< smi */
} hl_eth_gpiomap_t;


/** @typedef    typedef struct hl_eth_mapping_t 
    @brief      hl_eth_mapping_t contains the pin mapping for an eth peripherals
 **/
typedef struct
{
    hl_bool_t           supported;      /**< hl_true if supported */
    hl_eth_gpiomap_t    gpiomap;        /**< the gpio mapping */
} hl_eth_mapping_t;



/** @typedef    typedef enum hl_eth_behaviour_t 
    @brief      contains the behaviour of the peripheral
 **/
typedef enum
{
    hl_eth_beh_dmatxrx  = 0         /**< the tx and rx is done by dma transfer. the rx is managed by isr on dma completed */
} hl_eth_behaviour_t;


/** @typedef    typedef uint64_t hl_eth_macaddress_t 
    @brief      hl_eth_macaddress_t is used to store a mac address in its 6 least significant bytes
 **/ 
typedef uint64_t hl_eth_macaddress_t;


/** @typedef    typedef struct hl_eth_cfg_t 
    @brief      hl_eth_cfg_t contains the runtime configuration for eth.
                there are some hw configuration which are hidden: dutycycle mode is tlow/thigh = 2, the ack is enabled.
                if slave mode (not supported yet) the ack address is 7 bit.
 **/
typedef struct
{
    hl_eth_macaddress_t             macaddress;                 /**< the mac address */
    hl_eth_behaviour_t              behaviour;                  /**< the tx-rx behaviour */
    hl_irqpriority_t                priority;                   /**< keeps priority of the ISR */
    uint8_t                         capacityoftxfifoofframes;   /**< the number of eth frames buffered in dma transmission */
    uint8_t                         capacityofrxfifoofframes;   /**< the number of eth frames buffered in dma reception */
} hl_eth_cfg_t;


//#warning --> TODO: for use with lwip we need to modify it
#ifdef HAL_COMPATIBLE_LWIP
typedef struct
{
    void     *frame;
    uint32_t length;
}hl_eth_genericframe_t;
#else
typedef struct
{
    uint16_t length;                /**< the length of the frame in bytes */
    uint16_t index;                 /**< index to the used byte */
    uint8_t  datafirstbyte[1];    
} hl_eth_frame_t;

typedef void (*hl_eth_fp_onsendframe_t) (hl_eth_frame_t*);

#endif 

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

// it must be externally declared.
extern const hl_eth_mapping_t* hl_eth_map;


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn			extern hl_result_t hl_eth_init(const hl_eth_cfg_t *cfg)
    @brief  	this function initializes an eth port
    @param  	cfg 	        pointer to configuration. If NULL it is used hl_eth_cfg_default.
    @return 	hl_res_OK on success
  */
extern hl_result_t hl_eth_init(const hl_eth_cfg_t *cfg);


/** @fn			extern hl_bool_t hl_eth_initted_is(void)
    @brief  	this function tells if the eth has already been initted
    @return 	hl_true or hl_false
  */
extern hl_bool_t hl_eth_initted_is(void);


/** @fn         extern hl_result_t hl_eth_enable(void)
    @brief      Enable the eth peripheral. 
 **/
extern hl_result_t hl_eth_enable(void);


/** @fn         extern hl_result_t hl_eth_disable(void)
    @brief      Disable the eth peripheral. 
 **/
extern hl_result_t hl_eth_disable(void);


// protection vs multiple calls. it called internally to hl_eth_init()
extern void hl_eth_rmii_refclock_init(void);


// protection vs multiple calls. it is NOT called internally to hl_eth_init()
extern void hl_eth_smi_init(void);

extern hl_bool_t hl_eth_smi_initted(void);

// if hl_eth_smi_init() never called it does nothing and returns 0
extern uint16_t hl_eth_smi_read(uint8_t PHYaddr, uint8_t REGaddr);

// if hl_eth_smi_init() never called it does nothing
extern void hl_eth_smi_write(uint8_t PHYaddr, uint8_t REGaddr, uint16_t value);

#ifdef HAL_COMPATIBLE_LWIP
extern uint32_t hl_eth_moveframe2higherlayer(uint8_t *inputbuffer, uint32_t size);

extern uint32_t hl_eth_moveframe2lowerlayer(hl_eth_genericframe_t *frame, uint8_t *outputbuffer);

extern hl_result_t hl_eth_sendframe(hl_eth_genericframe_t *frame);

#else
extern hl_eth_frame_t* hl_eth_frame_new(uint32_t len);


extern void hl_eth_on_frame_received(hl_eth_frame_t* frame);


extern hl_result_t hl_eth_sendframe(hl_eth_frame_t *frame);

extern hl_result_t hl_eth_set_callback_on_sendframe(hl_eth_fp_onsendframe_t onsendframe);

#endif

extern void hl_eth_alert(void);

/** @}            
    end of group doxy_group_hl_eth  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



